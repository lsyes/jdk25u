// LoongArch machine-level trace scheduler for C2.
//
// This crate is compiled to a `staticlib` and linked into libjvm; it is called
// through the plain C ABI declared in la_sched_ffi.hpp.  It is `no_std` (there
// is no allocator and no Rust runtime in the JVM) and every entry point uses
// caller provided buffers, so no Rust object ever crosses the FFI boundary.
//
// What it does
// ------------
// C2 lays out basic blocks with PhaseBlockLayout (a Pettis-Hansen trace
// builder, see share/opto/block.cpp).  After the traces have been grown, the
// traces themselves are currently ordered by the frequency of their first
// block only.  That ignores the edges *between* traces: an edge from the last
// block of trace A to the first block of trace B is executed as a taken branch
// unless B immediately follows A.
//
// The scheduler below reproduces the block-ordering idea of GCC's bb-reorder
// pass (gcc/bb-reorder.cc): walk the trace graph and prefer the hottest
// successor, starting a new chain only when no fall-through continuation with
// a worthwhile frequency exists.  Maximizing fall-through edges removes taken
// branches and improves instruction cache locality.
//
// The ABI is versioned (LA_SCHED_ABI_VERSION, mirrored in la_sched_ffi.hpp).

#![no_std]

use core::panic::PanicInfo;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    // Unreachable: this crate contains no panicking operations.  Keeping a
    // handler is required to link a no_std staticlib.
    loop {}
}

/// Incremented whenever LaTraceInfo/LaTraceEdge or the entry points change.
pub const LA_SCHED_ABI_VERSION: i32 = 1;

/// Upper bound on the number of traces handled by one compilation.  Methods
/// with more traces keep the generic frequency ordering (return 0).
const MAX_TRACES: i32 = 512;

#[repr(C)]
pub struct LaTraceInfo {
    /// pre_order() of the first block of the trace.
    pub first: i32,
    /// pre_order() of the last block of the trace.
    pub last: i32,
    /// Estimated execution frequency of the trace.
    pub freq: f64,
    /// Non-zero for the (single) trace of connector blocks, which must stay at
    /// the very end of the layout.
    pub is_connector: i32,
}

#[repr(C)]
pub struct LaTraceEdge {
    /// Index of the trace that contains the source block.
    pub from: i32,
    /// Index of the trace that contains the destination block.
    pub to: i32,
    /// Estimated execution frequency of the edge.
    pub weight: f64,
}

// Even with panic=abort some objects in the bundled core library reference the
// personality routine, so provide a dummy definition; it is never called.
#[no_mangle]
pub extern "C" fn rust_eh_personality() {}

#[no_mangle]
pub extern "C" fn la_sched_version() -> i32 {
    LA_SCHED_ABI_VERSION
}

/// Compute an order for `n` traces that maximizes fall-through edges.
///
/// `order` must have room for `n` entries; on success the number of written
/// entries is returned (always `n`), and 0 is returned when the input is
/// unsupported and the caller should keep the generic ordering.
#[no_mangle]
pub extern "C" fn la_order_traces(
    n: i32,
    info: *const LaTraceInfo,
    m: i32,
    edges: *const LaTraceEdge,
    order: *mut i32,
) -> i32 {
    if n <= 0 || n > MAX_TRACES || info.is_null() || order.is_null() || m < 0 {
        return 0;
    }
    let info = unsafe { core::slice::from_raw_parts(info, n as usize) };
    let edges: &[LaTraceEdge] = if m > 0 && !edges.is_null() {
        unsafe { core::slice::from_raw_parts(edges, m as usize) }
    } else {
        &[]
    };

    let mut out = [0i32; MAX_TRACES as usize];
    let mut len = 0usize;

    // 1) Frequency order, exactly like the generic C++ ordering, but keep the
    //    entry trace (index 0) first and the connector trace last.
    let mut placed = [0u8; MAX_TRACES as usize];
    out[len] = 0;
    placed[0] = 1;
    len += 1;
    while len < n as usize {
        let mut pick: i32 = -1;
        let mut pick_freq = -1.0f64;
        for i in 0..n {
            if placed[i as usize] == 0 && info[i as usize].is_connector == 0
                && info[i as usize].freq > pick_freq
            {
                pick_freq = info[i as usize].freq;
                pick = i;
            }
        }
        if pick < 0 {
            break;
        }
        out[len] = pick;
        placed[pick as usize] = 1;
        len += 1;
    }
    loop {
        let mut pick: i32 = -1;
        let mut pick_freq = -1.0f64;
        for i in 0..n {
            if placed[i as usize] == 0 && info[i as usize].freq > pick_freq {
                pick_freq = info[i as usize].freq;
                pick = i;
            }
        }
        if pick < 0 {
            break;
        }
        if len >= n as usize {
            return 0;
        }
        out[len] = pick;
        placed[pick as usize] = 1;
        len += 1;
    }
    if len != n as usize {
        return 0;
    }

    // 2) Conservative local improvement: swap adjacent traces only when it
    //    turns more (estimated) execution into fall-through.  This keeps the
    //    hot-first locality of the frequency order intact; a globally greedy
    //    trace walk was measured to hurt code locality on LA664.
    //    succ[i] is the hottest trace that trace i can fall through into.
    //    Bound the work for very large methods.
    if (n as i64) * (m as i64) > 200_000 {
        unsafe { core::ptr::copy_nonoverlapping(out.as_ptr(), order, len); }
        return n;
    }
    let mut succ = [0i32; MAX_TRACES as usize];
    let mut succ_w = [0.0f64; MAX_TRACES as usize];
    for i in 0..n as usize {
        succ[i] = -1;
        succ_w[i] = 0.0;
    }
    for e in edges {
        if e.from >= 0 && e.to >= 0 && (e.from as usize) < n as usize
            && (e.to as usize) < n as usize && e.weight > succ_w[e.from as usize]
        {
            succ_w[e.from as usize] = e.weight;
            succ[e.from as usize] = e.to;
        }
    }
    // Two bubble passes over the regular traces (index 1 .. len-1, connectors
    // are at the end and must stay there).
    let last_regular = {
        let mut l = len;
        while l > 1 && info[out[l - 1] as usize].is_connector != 0 {
            l -= 1;
        }
        l
    };
    let w = |a: i32, b: i32, succ: &[i32; MAX_TRACES as usize],
             succ_w: &[f64; MAX_TRACES as usize]| -> f64 {
        if a >= 0 && b >= 0 && succ[a as usize] == b {
            succ_w[a as usize]
        } else {
            0.0
        }
    };
    for _pass in 0..2 {
        let mut i = 2usize;
        while i < last_regular {
            let a = out[i - 1];
            let b = out[i];
            let before = w(a, b, &succ, &succ_w) + w(b, out[i + 1], &succ, &succ_w);
            let after = w(a, out[i + 1], &succ, &succ_w) + w(b, a, &succ, &succ_w);
            if after > before {
                out[i - 1] = b;
                out[i] = a;
            }
            i += 1;
        }
    }

    unsafe {
        core::ptr::copy_nonoverlapping(out.as_ptr(), order, len);
    }
    n
}

// ---------------------------------------------------------------------------
// Instruction selection for unsigned division / remainder by a constant.
//
// C2 only strength-reduces unsigned division when the divisor is a power of
// two (share/opto/divnode.cpp:unsigned_div_ideal); every other constant
// divisor ends up as a hardware div/mod instruction, which costs 12 (32 bit)
// or 25 (64 bit) cycles on LA464/LA664.  GCC instead expands the division into
// a multiply-high/shift sequence using the multiplier chosen by
// choose_multiplier() (gcc/expmed.cc, the Granlund-Montgomery algorithm).
//
// The planner below is a direct port of that algorithm.  It also decides the
// instruction sequence and the registers to use, and returns it as a list of
// machine instructions that the C++ emitter (C2_MacroAssembler::rust_udivmod)
// writes out verbatim, so the JIT backend only needs a tiny switch statement.
// ---------------------------------------------------------------------------

/// Opcodes of the planned instruction list; mirrored by enum LaSchedOp in
/// la_sched_ffi.hpp and the switch in c2_MacroAssembler_loongarch.cpp.
pub const OP_MOVE_W: i32 = 0;
pub const OP_MOVE_D: i32 = 1;
pub const OP_LI: i32 = 2;
pub const OP_MULH_WU: i32 = 3;
pub const OP_MULH_DU: i32 = 4;
pub const OP_SRLI_W: i32 = 5;
pub const OP_SRLI_D: i32 = 6;
pub const OP_SUB_W: i32 = 7;
pub const OP_SUB_D: i32 = 8;
pub const OP_ADD_W: i32 = 9;
pub const OP_ADD_D: i32 = 10;
pub const OP_MUL_W: i32 = 11;
pub const OP_MUL_D: i32 = 12;
pub const OP_AND: i32 = 13;
pub const OP_ANDI: i32 = 14;
pub const OP_SLTU: i32 = 15;
pub const OP_XORI: i32 = 16;
pub const OP_DIV_WU: i32 = 17;
pub const OP_DIV_DU: i32 = 18;
pub const OP_MOD_WU: i32 = 19;
pub const OP_MOD_DU: i32 = 20;
pub const OP_ZEXT_W: i32 = 21; // rd = zero extension of the low 32 bits of rj

#[repr(C)]
pub struct LaInsn {
    pub op: i32,
    pub rd: i32,
    pub rj: i32,
    pub rk: i32,
    pub imm: i64,
}

struct Plan<'a> {
    buf: &'a mut [LaInsn],
    len: usize,
    ok: bool,
}

impl<'a> Plan<'a> {
    fn new(buf: &'a mut [LaInsn]) -> Plan<'a> {
        Plan { buf, len: 0, ok: true }
    }
    fn emit(&mut self, op: i32, rd: i32, rj: i32, rk: i32, imm: i64) {
        if self.len >= self.buf.len() {
            self.ok = false;
            return;
        }
        self.buf[self.len] = LaInsn { op, rd, rj, rk, imm };
        self.len += 1;
    }
}

/// Smallest k with 2^k >= d, for d > 1.
fn ceil_log2(d: u128) -> u32 {
    let mut k = 0u32;
    let mut v = 1u128;
    while v < d {
        v <<= 1;
        k += 1;
    }
    k
}

/// Port of gcc/expmed.cc:choose_multiplier.  Returns (multiplier, post_shift,
/// needs_add).  n is the number of significant bits of the dividend,
/// precision the number of bits of the operation.
fn choose_multiplier(d: u128, n: u32, precision: u32) -> (u64, u32, bool) {
    let lgup = ceil_log2(d);
    let pow1 = n + lgup;
    let pow2 = n + lgup - precision;
    let mut mlow = (1u128 << pow1) / d;
    let mut mhigh = ((1u128 << pow1) + (1u128 << pow2)) / d;
    let mut post_shift = lgup;
    while post_shift > 0 {
        let ml_lo = (mlow >> 1) & (u64::MAX as u128);
        let mh_lo = (mhigh >> 1) & (u64::MAX as u128);
        if ml_lo >= mh_lo {
            break;
        }
        mlow = ml_lo;
        mhigh = mh_lo;
        post_shift -= 1;
    }
    let mask: u128 = (1u128 << n) - 1;
    ((mhigh & mask) as u64, post_shift, mhigh > mask)
}

fn plan_udivmod32(p: &mut Plan, d: u32, is_mod: bool, dst: i32, src: i32, t1: i32, t2: i32) {
    let q = if is_mod { t2 } else { dst };

    if d == 0 {
        p.emit(OP_LI, t1, 0, 0, 0);
        if is_mod {
            p.emit(OP_MOD_WU, dst, src, t1, 0);
        } else {
            p.emit(OP_DIV_WU, dst, src, t1, 0);
        }
        return;
    }
    if d == 1 {
        if is_mod {
            p.emit(OP_LI, dst, 0, 0, 0);
        } else {
            p.emit(OP_MOVE_W, dst, src, 0, 0);
        }
        return;
    }
    if d.is_power_of_two() {
        if is_mod {
            let mask = (d - 1) as i64;
            if mask <= 0xfff {
                p.emit(OP_ANDI, dst, src, 0, mask);
            } else {
                p.emit(OP_LI, t1, 0, 0, mask);
                p.emit(OP_AND, dst, src, t1, 0);
            }
        } else {
            p.emit(OP_SRLI_W, dst, src, 0, d.trailing_zeros() as i64);
        }
        return;
    }

    if d >= 0x8000_0000 {
        // Most significant bit set: the quotient is 0 or 1.  sltu is a 64 bit
        // unsigned compare, so zero extend the int operand first.
        p.emit(OP_LI, t1, 0, 0, d as i64);
        p.emit(OP_ZEXT_W, q, src, 0, 0);
        p.emit(OP_SLTU, q, q, t1, 0);
        p.emit(OP_XORI, q, q, 0, 1);
    } else {
        let (mut ml, mut post, mut mh) = choose_multiplier(d as u128, 32, 32);
        let mut pre_shift = 0u32;
        if mh && (d & 1) == 0 {
            pre_shift = d.trailing_zeros();
            let r = choose_multiplier((d >> pre_shift) as u128, 32, 32 - pre_shift);
            ml = r.0;
            post = r.1;
            mh = r.2;
        }
        let magic = ml as u32 as i64;
        if pre_shift > 0 {
            p.emit(OP_SRLI_W, t1, src, 0, pre_shift as i64);
            p.emit(OP_LI, t2, 0, 0, magic);
            p.emit(OP_MULH_WU, q, t1, t2, 0);
            if post > 0 {
                p.emit(OP_SRLI_W, q, q, 0, post as i64);
            }
        } else if mh {
            p.emit(OP_LI, t1, 0, 0, magic);
            p.emit(OP_MULH_WU, t2, src, t1, 0);
            p.emit(OP_SUB_W, t1, src, t2, 0);
            p.emit(OP_SRLI_W, t1, t1, 0, 1);
            p.emit(OP_ADD_W, t2, t2, t1, 0);
            p.emit(OP_SRLI_W, q, t2, 0, (post - 1) as i64);
        } else {
            p.emit(OP_LI, t1, 0, 0, magic);
            p.emit(OP_MULH_WU, q, src, t1, 0);
            if post > 0 {
                p.emit(OP_SRLI_W, q, q, 0, post as i64);
            }
        }
    }
    if is_mod {
        // remainder = x - q * d
        p.emit(OP_LI, t1, 0, 0, d as i64);
        p.emit(OP_MUL_W, t1, t2, t1, 0);
        p.emit(OP_SUB_W, dst, src, t1, 0);
    }
}

fn plan_udivmod64(p: &mut Plan, d: u64, is_mod: bool, dst: i32, src: i32, t1: i32, t2: i32) {
    let q = if is_mod { t2 } else { dst };

    if d == 0 {
        p.emit(OP_LI, t1, 0, 0, 0);
        if is_mod {
            p.emit(OP_MOD_DU, dst, src, t1, 0);
        } else {
            p.emit(OP_DIV_DU, dst, src, t1, 0);
        }
        return;
    }
    if d == 1 {
        if is_mod {
            p.emit(OP_LI, dst, 0, 0, 0);
        } else {
            p.emit(OP_MOVE_D, dst, src, 0, 0);
        }
        return;
    }
    if d.is_power_of_two() {
        if is_mod {
            p.emit(OP_LI, t1, 0, 0, (d - 1) as i64);
            p.emit(OP_AND, dst, src, t1, 0);
        } else {
            p.emit(OP_SRLI_D, dst, src, 0, d.trailing_zeros() as i64);
        }
        return;
    }

    if d >= 0x8000_0000_0000_0000 {
        p.emit(OP_LI, t1, 0, 0, d as i64);
        p.emit(OP_SLTU, q, src, t1, 0);
        p.emit(OP_XORI, q, q, 0, 1);
    } else {
        let (mut ml, mut post, mut mh) = choose_multiplier(d as u128, 64, 64);
        let mut pre_shift = 0u32;
        if mh && (d & 1) == 0 {
            pre_shift = d.trailing_zeros();
            let r = choose_multiplier((d >> pre_shift) as u128, 64, 64 - pre_shift);
            ml = r.0;
            post = r.1;
            mh = r.2;
        }
        let magic = ml as i64;
        if pre_shift > 0 {
            p.emit(OP_SRLI_D, t1, src, 0, pre_shift as i64);
            p.emit(OP_LI, t2, 0, 0, magic);
            p.emit(OP_MULH_DU, q, t1, t2, 0);
            if post > 0 {
                p.emit(OP_SRLI_D, q, q, 0, post as i64);
            }
        } else if mh {
            p.emit(OP_LI, t1, 0, 0, magic);
            p.emit(OP_MULH_DU, t2, src, t1, 0);
            p.emit(OP_SUB_D, t1, src, t2, 0);
            p.emit(OP_SRLI_D, t1, t1, 0, 1);
            p.emit(OP_ADD_D, t2, t2, t1, 0);
            p.emit(OP_SRLI_D, q, t2, 0, (post - 1) as i64);
        } else {
            p.emit(OP_LI, t1, 0, 0, magic);
            p.emit(OP_MULH_DU, q, src, t1, 0);
            if post > 0 {
                p.emit(OP_SRLI_D, q, q, 0, post as i64);
            }
        }
    }

    if is_mod {
        p.emit(OP_LI, t1, 0, 0, d as i64);
        p.emit(OP_MUL_D, t1, t2, t1, 0);
        p.emit(OP_SUB_D, dst, src, t1, 0);
    }
}

/// Plan an unsigned division or remainder by a constant divisor.
/// Returns the number of instructions written to `out`, or 0 when the input is
/// not supported (the caller then emits a hardware div/mod).
#[no_mangle]
pub extern "C" fn la_plan_udivmod(
    is_64: i32,
    is_mod: i32,
    divisor: u64,
    dst: i32,
    src: i32,
    t1: i32,
    t2: i32,
    out: *mut LaInsn,
    cap: i32,
) -> i32 {
    if out.is_null() || cap <= 0 || dst == t1 || dst == t2 || t1 == t2 || src == t1 || src == t2 {
        return 0;
    }
    let buf = unsafe { core::slice::from_raw_parts_mut(out, cap as usize) };
    let mut p = Plan::new(buf);
    if is_64 != 0 {
        plan_udivmod64(&mut p, divisor, is_mod != 0, dst, src, t1, t2);
    } else {
        plan_udivmod32(&mut p, divisor as u32, is_mod != 0, dst, src, t1, t2);
    }
    if p.ok {
        p.len as i32
    } else {
        0
    }
}
