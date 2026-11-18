# LoongArch Rust scheduler

`la_sched.rs` is a small `#![no_std]` Rust library that is compiled into a
`staticlib` by `make/hotspot/lib/CompileJvm.gmk` and linked into `libjvm`:

```
rustc --edition 2021 --crate-type staticlib \
      -C opt-level=3 -C panic=abort -C codegen-units=1 \
      -C relocation-model=pic -C debuginfo=0 \
      -o libla_sched.a la_sched.rs
```

The build integration is conditional on `rustc` being on `PATH`; without it the
VM builds unchanged and the C++ call sites keep their generic fallbacks.  The C
ABI shared with HotSpot is declared in `../la_sched_ffi.hpp`; both sides agree
on `LA_SCHED_ABI_VERSION` (`la_sched_version()` returns it at runtime and the
C++ side refuses to call into a library with a different version).

Properties of the library:

* no Rust runtime, no allocator: every entry point works on caller provided
  buffers, so the library never calls `malloc`, never unwinds and never crosses
  the FFI boundary with a Rust object;
* all entry points are pure functions: they can be called concurrently by
  several CompilerThreads (the JVM calls them without holding a lock);
* when the planner cannot handle the input it returns `0` and the caller
  emits the generic sequence (for example a hardware `div`/`mod`).

## Entry points

### `la_order_traces()`

Orders C2's `PhaseBlockLayout` traces so that the hottest edges between
traces become fall-through edges (the trace walk of GCC's `bb-reorder` pass,
`gcc/bb-reorder.cc`).  C2 orders traces by the frequency of their first block
only, which leaves some hot inter-trace edges as taken branches.  The entry
trace always stays first and the connector-block trace always stays last.

Controlled by `-XX:+UseLATraceScheduler`, **default off**: A/B runs on LA664
(JMH, 11 benchmarks, 15 iterations each) showed the global greedy variant
costing up to 13% on `Integer.toString`/`HashMap` and the conservative
frequency-preserving variant still being within noise but a little behind
(geometric mean 0.986x for on vs off).  The frequency based ordering produced
by `PhaseBlockLayout::merge_traces()` is already good on this core, so the
scheduler is kept as an opt-in experiment and as the integration point for
further work rather than being enabled by default.

### `la_plan_udivmod()`

Instruction selection for unsigned division and remainder by a constant
divisor.  C2 only strength-reduces power-of-two divisors
(`share/opto/divnode.cpp:unsigned_div_ideal`); all other constant divisors used
to be emitted as a hardware `div.wu`/`div.du`, which costs 12 cycles (32 bit)
or 25 cycles (64 bit) on LA464/LA664.

The planner ports `choose_multiplier()` from `gcc/expmed.cc` together with the
sequence selection of `expand_divmod()` (multiply-high + shift, with the
"add" correction when the multiplier needs an extra bit, the pre-shift for even
divisors and the `x >= d` special case for divisors with the top bit set).
It returns the complete, register-allocated instruction sequence; the C++ side
only translates the returned opcodes into `Assembler` calls
(`C2_MacroAssembler::rust_udivmod`).  The `UDivI`/`UDivL`/`UModI`/`UModL` AD
rules with a constant operand call it.

## Validation

The multiplier algorithm and the emitted sequence were validated with a
standalone simulator (all opcodes emulated, 1M + 720k random and edge cases,
zero mismatches) and with a generated self checking Java test that compares
the JIT result against `long`/`BigInteger` reference arithmetic, including
`MIN_VALUE`, `-1`, divisors >= 2^31 / 2^63 and division by zero.
