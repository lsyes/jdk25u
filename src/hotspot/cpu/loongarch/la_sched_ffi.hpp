/*
 * Copyright (c) 2026, Loongson Technology. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#ifndef CPU_LOONGARCH_LA_SCHED_FFI_HPP
#define CPU_LOONGARCH_LA_SCHED_FFI_HPP

// C ABI of the Rust machine-level scheduler (src/hotspot/cpu/loongarch/rust/
// la_sched.rs, built into libjvm by make/hotspot/lib/CompileJvm.gmk).
// Both sides must agree on LA_SCHED_ABI_VERSION; la_sched_version() returns the
// version implemented by the linked library.

#define LA_SCHED_ABI_VERSION 1

// One C2 trace (a straight-line chain of basic blocks).
struct LaTraceInfo {
  int   first;         // pre_order() of the first block of the trace
  int   last;          // pre_order() of the last block of the trace
  double freq;         // estimated execution frequency
  int   is_connector;  // non-zero for the connector-block trace
};

// A candidate fall-through edge between two traces: the source block is the
// last block of trace "from" and the destination is the first block of trace
// "to".  Placing "to" right after "from" turns the edge into a fall-through.
struct LaTraceEdge {
  int    from;
  int    to;
  double weight;
};

// One machine instruction of a planned sequence.  Registers are passed as
// hardware encodings so the plan is independent of the C++ register objects.
struct LaInsn {
  int op;    // one of the LA_OP_* opcodes below
  int rd;
  int rj;
  int rk;
  long imm;
};

// Opcodes understood by C2_MacroAssembler::rust_udivmod.
enum LaSchedOp {
  LA_OP_MOVE_W = 0,
  LA_OP_MOVE_D = 1,
  LA_OP_LI = 2,
  LA_OP_MULH_WU = 3,
  LA_OP_MULH_DU = 4,
  LA_OP_SRLI_W = 5,
  LA_OP_SRLI_D = 6,
  LA_OP_SUB_W = 7,
  LA_OP_SUB_D = 8,
  LA_OP_ADD_W = 9,
  LA_OP_ADD_D = 10,
  LA_OP_MUL_W = 11,
  LA_OP_MUL_D = 12,
  LA_OP_AND = 13,
  LA_OP_ANDI = 14,
  LA_OP_SLTU = 15,
  LA_OP_XORI = 16,
  LA_OP_DIV_WU = 17,
  LA_OP_DIV_DU = 18,
  LA_OP_MOD_WU = 19,
  LA_OP_MOD_DU = 20,
  LA_OP_ZEXT_W = 21
};

extern "C" {
  int la_sched_version();
  int la_order_traces(int n, const LaTraceInfo* info,
                      int m, const LaTraceEdge* edges,
                      int* order);
  // Plan an unsigned division or remainder by a constant divisor.
  // Returns the number of entries written to `out`, or 0 if unsupported.
  int la_plan_udivmod(int is_64, int is_mod, unsigned long divisor,
                      int dst, int src, int t1, int t2,
                      LaInsn* out, int cap);
}

#endif // CPU_LOONGARCH_LA_SCHED_FFI_HPP
