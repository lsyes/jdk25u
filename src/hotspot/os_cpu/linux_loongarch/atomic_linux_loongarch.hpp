/*
 * Copyright (c) 1999, 2013, Oracle and/or its affiliates. All rights reserved.
 * Copyright (c) 2015, 2025, Loongson Technology. All rights reserved.
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

#ifndef OS_CPU_LINUX_LOONGARCH_ATOMIC_LINUX_LOONGARCH_HPP
#define OS_CPU_LINUX_LOONGARCH_ATOMIC_LINUX_LOONGARCH_HPP

#include "runtime/vm_version.hpp"

// Implementation of class atomic

#define AMCAS_MACRO asm volatile (                                          \
      ".ifndef _ASM_ASMMACRO_                                         \n\t" \
      ".set _ASM_ASMMACRO_, 1                                         \n\t" \
      ".macro   parse_r var r                                         \n\t" \
      "\\var    = -1                                                  \n\t" \
      ".ifc     \\r, $r0                                              \n\t" \
      "\\var    = 0                                                   \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r1                                              \n\t" \
      "\\var    = 1                                                   \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r2                                              \n\t" \
      "\\var    = 2                                                   \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r3                                              \n\t" \
      "\\var    = 3                                                   \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r4                                              \n\t" \
      "\\var    = 4                                                   \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r5                                              \n\t" \
      "\\var    = 5                                                   \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r6                                              \n\t" \
      "\\var    = 6                                                   \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r7                                              \n\t" \
      "\\var    = 7                                                   \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r8                                              \n\t" \
      "\\var    = 8                                                   \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r9                                              \n\t" \
      "\\var    = 9                                                   \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r10                                             \n\t" \
      "\\var    = 10                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r11                                             \n\t" \
      "\\var    = 11                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r12                                             \n\t" \
      "\\var    = 12                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r13                                             \n\t" \
      "\\var    = 13                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r14                                             \n\t" \
      "\\var    = 14                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r15                                             \n\t" \
      "\\var    = 15                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r16                                             \n\t" \
      "\\var    = 16                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r17                                             \n\t" \
      "\\var    = 17                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r18                                             \n\t" \
      "\\var    = 18                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r19                                             \n\t" \
      "\\var    = 19                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r20                                             \n\t" \
      "\\var    = 20                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r21                                             \n\t" \
      "\\var    = 21                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r22                                             \n\t" \
      "\\var    = 22                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r23                                             \n\t" \
      "\\var    = 23                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r24                                             \n\t" \
      "\\var    = 24                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r25                                             \n\t" \
      "\\var    = 25                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r26                                             \n\t" \
      "\\var    = 26                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r27                                             \n\t" \
      "\\var    = 27                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r28                                             \n\t" \
      "\\var    = 28                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r29                                             \n\t" \
      "\\var    = 29                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r30                                             \n\t" \
      "\\var    = 30                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".ifc     \\r, $r31                                             \n\t" \
      "\\var    = 31                                                  \n\t" \
      ".endif                                                         \n\t" \
      ".iflt    \\var                                                 \n\t" \
      ".endif                                                         \n\t" \
      ".endm                                                          \n\t" \
      ".macro amcas_b rd, rk, rj                                      \n\t" \
      "parse_r d, \\rd                                                \n\t" \
      "parse_r j, \\rj                                                \n\t" \
      "parse_r k, \\rk                                                \n\t" \
      ".word ((0b00111000010110000 << 15) | (k << 10) | (j << 5) | d) \n\t" \
      ".endm                                                          \n\t" \
      ".macro amcas_h rd, rk, rj                                      \n\t" \
      "parse_r d, \\rd                                                \n\t" \
      "parse_r j, \\rj                                                \n\t" \
      "parse_r k, \\rk                                                \n\t" \
      ".word ((0b00111000010110001 << 15) | (k << 10) | (j << 5) | d) \n\t" \
      ".endm                                                          \n\t" \
      ".macro amcas_w rd, rk, rj                                      \n\t" \
      "parse_r d, \\rd                                                \n\t" \
      "parse_r j, \\rj                                                \n\t" \
      "parse_r k, \\rk                                                \n\t" \
      ".word ((0b00111000010110010 << 15) | (k << 10) | (j << 5) | d) \n\t" \
      ".endm                                                          \n\t" \
      ".macro amcas_d rd, rk, rj                                      \n\t" \
      "parse_r d, \\rd                                                \n\t" \
      "parse_r j, \\rj                                                \n\t" \
      "parse_r k, \\rk                                                \n\t" \
      ".word ((0b00111000010110011 << 15) | (k << 10) | (j << 5) | d) \n\t" \
      ".endm                                                          \n\t" \
      ".macro amcas_db_b rd, rk, rj                                   \n\t" \
      "parse_r d, \\rd                                                \n\t" \
      "parse_r j, \\rj                                                \n\t" \
      "parse_r k, \\rk                                                \n\t" \
      ".word ((0b00111000010110100 << 15) | (k << 10) | (j << 5) | d) \n\t" \
      ".endm                                                          \n\t" \
      ".macro amcas_db_h rd, rk, rj                                   \n\t" \
      "parse_r d, \\rd                                                \n\t" \
      "parse_r j, \\rj                                                \n\t" \
      "parse_r k, \\rk                                                \n\t" \
      ".word ((0b00111000010110101 << 15) | (k << 10) | (j << 5) | d) \n\t" \
      ".endm                                                          \n\t" \
      ".macro amcas_db_w rd, rk, rj                                   \n\t" \
      "parse_r d, \\rd                                                \n\t" \
      "parse_r j, \\rj                                                \n\t" \
      "parse_r k, \\rk                                                \n\t" \
      ".word ((0b00111000010110110 << 15) | (k << 10) | (j << 5) | d) \n\t" \
      ".endm                                                          \n\t" \
      ".macro amcas_db_d rd, rk, rj                                   \n\t" \
      "parse_r d, \\rd                                                \n\t" \
      "parse_r j, \\rj                                                \n\t" \
      "parse_r k, \\rk                                                \n\t" \
      ".word ((0b00111000010110111 << 15) | (k << 10) | (j << 5) | d) \n\t" \
      ".endm                                                          \n\t" \
      ".endif                                                         \n\t" \
     );

template<size_t byte_size>
struct Atomic::PlatformAdd {
  template<typename D, typename I>
  D fetch_then_add(D volatile* dest, I add_value, atomic_memory_order order) const;

  template<typename D, typename I>
  D add_then_fetch(D volatile* dest, I add_value, atomic_memory_order order) const {
    return fetch_then_add(dest, add_value, order) + add_value;
  }
};

template<>
template<typename D, typename I>
inline D Atomic::PlatformAdd<4>::fetch_then_add(D volatile* dest, I add_value,
                                               atomic_memory_order order) const {
  STATIC_ASSERT(4 == sizeof(I));
  STATIC_ASSERT(4 == sizeof(D));
  D old_value;

  switch (order) {
  case memory_order_relaxed:
    asm volatile (
      "amadd.w %[old], %[add], %[dest] \n\t"
      : [old] "=&r" (old_value)
      : [add] "r" (add_value), [dest] "r" (dest)
      : "memory");
    break;
  default:
    asm volatile (
      "amadd_db.w %[old], %[add], %[dest] \n\t"
      : [old] "=&r" (old_value)
      : [add] "r" (add_value), [dest] "r" (dest)
      : "memory");
    break;
  }

  return old_value;
}

template<>
template<typename D, typename I>
inline D Atomic::PlatformAdd<8>::fetch_then_add(D volatile* dest, I add_value,
                                               atomic_memory_order order) const {
  STATIC_ASSERT(8 == sizeof(I));
  STATIC_ASSERT(8 == sizeof(D));
  D old_value;

  switch (order) {
  case memory_order_relaxed:
    asm volatile (
      "amadd.d %[old], %[add], %[dest] \n\t"
      : [old] "=&r" (old_value)
      : [add] "r" (add_value), [dest] "r" (dest)
      : "memory");
    break;
  default:
    asm volatile (
      "amadd_db.d %[old], %[add], %[dest] \n\t"
      : [old] "=&r" (old_value)
      : [add] "r" (add_value), [dest] "r" (dest)
      : "memory");
    break;
  }

  return old_value;
}

template<>
template<typename T>
inline T Atomic::PlatformXchg<4>::operator()(T volatile* dest,
                                             T exchange_value,
                                             atomic_memory_order order) const {
  STATIC_ASSERT(4 == sizeof(T));
  T old_value;

  switch (order) {
  case memory_order_relaxed:
    asm volatile (
      "amswap.w %[_old], %[_new], %[dest] \n\t"
      : [_old] "=&r" (old_value)
      : [_new] "r" (exchange_value), [dest] "r" (dest)
      : "memory");
    break;
  default:
    asm volatile (
      "amswap_db.w %[_old], %[_new], %[dest] \n\t"
      : [_old] "=&r" (old_value)
      : [_new] "r" (exchange_value), [dest] "r" (dest)
      : "memory");
    break;
  }

  return old_value;
}

template<>
template<typename T>
inline T Atomic::PlatformXchg<8>::operator()(T volatile* dest,
                                             T exchange_value,
                                             atomic_memory_order order) const {
  STATIC_ASSERT(8 == sizeof(T));
  T old_value;

  switch (order) {
  case memory_order_relaxed:
    asm volatile (
      "amswap.d %[_old], %[_new], %[dest] \n\t"
      : [_old] "=&r" (old_value)
      : [_new] "r" (exchange_value), [dest] "r" (dest)
      : "memory");
    break;
  default:
    asm volatile (
      "amswap_db.d %[_old], %[_new], %[dest] \n\t"
      : [_old] "=&r" (old_value)
      : [_new] "r" (exchange_value), [dest] "r" (dest)
      : "memory");
    break;
  }

  return old_value;
}

// Native 1-byte compare-and-swap.
//
// Uses the AMCAS{,_DB}.B instruction when available.  Two details matter for
// correctness and are easy to get wrong:
//
//  * The comparison only involves the low 8 bits of the operands, and only
//    1 byte is stored.  Adjacent bytes of the containing word are never
//    touched, so this cannot cause spurious failures or lost updates on
//    neighbours (which is what the CmpxchgByteUsingInt fallback risks).
//  * The old value written to rd is *sign extended* from bit 7 (this is
//    documented behaviour of the AMCAS.B instruction, and it is why the
//    macroAssembler path uses bstrpick.w after amcas.b).  The result is
//    therefore masked down to 8 bits before being returned.
//
// When UseAMCAS is off we keep the previous ll/sc based byte-in-word
// implementation, which is correct (just wider than necessary).
template<>
template<typename T>
inline T Atomic::PlatformCmpxchg<1>::operator()(T volatile* dest,
                                                T compare_value,
                                                T exchange_value,
                                                atomic_memory_order order) const {
  STATIC_ASSERT(1 == sizeof(T));
  if (!UseAMCAS) {
    return Atomic::CmpxchgByteUsingInt()(dest, compare_value, exchange_value, order);
  }

  AMCAS_MACRO
  uint64_t prev;
  uint64_t old_value = (uint64_t)(uint8_t)compare_value;
  uint64_t new_value = (uint64_t)(uint8_t)exchange_value;

  switch (order) {
  case memory_order_relaxed:
    asm volatile (
      " move %[prev], %[_old] \n\t"
      " amcas_b %[prev], %[_new], %[dest] \n\t"
      : [prev] "+&r" (prev)
      : [_old] "r" (old_value), [_new] "r" (new_value), [dest] "r" (dest)
      : "memory");
    break;
  case memory_order_acquire:
    asm volatile (
      " move %[prev], %[_old] \n\t"
      " amcas_b %[prev], %[_new], %[dest] \n\t"
      " dbar 0x14 \n\t"
      : [prev] "+&r" (prev)
      : [_old] "r" (old_value), [_new] "r" (new_value), [dest] "r" (dest)
      : "memory");
    break;
  case memory_order_release:
    asm volatile (
      " move %[prev], %[_old] \n\t"
      " dbar 0x12 \n\t"
      " amcas_b %[prev], %[_new], %[dest] \n\t"
      : [prev] "+&r" (prev)
      : [_old] "r" (old_value), [_new] "r" (new_value), [dest] "r" (dest)
      : "memory");
    break;
  default:
    asm volatile (
      " move %[prev], %[_old] \n\t"
      " amcas_db_b %[prev], %[_new], %[dest] \n\t"
      : [prev] "+&r" (prev)
      : [_old] "r" (old_value), [_new] "r" (new_value), [dest] "r" (dest)
      : "memory");
    break;
  }

  return (T)(uint8_t)prev;  // amcas.b returns a sign-extended old value
}

// Native 2-byte compare-and-swap, analogous to the byte case above.
template<>
template<typename T>
inline T Atomic::PlatformCmpxchg<2>::operator()(T volatile* dest,
                                                T compare_value,
                                                T exchange_value,
                                                atomic_memory_order order) const {
  STATIC_ASSERT(2 == sizeof(T));
  if (!UseAMCAS) {
    // No AMCAS.H available.  LoongArch only offers ll.w/sc.w and ll.d/sc.d
    // (there is no ll.h/sc.h), so emulate the halfword CAS the same way
    // CmpxchgByteUsingInt does for bytes: align down to the containing word
    // and CAS that, splicing in only the halfword we want to update.
    //
    // The comparison still only involves the intended halfword, so a
    // differing neighbour yields a correct "no match" result rather than a
    // spurious failure.
    uint32_t* aligned_dest = (uint32_t*)align_down((void*)dest, sizeof(uint32_t));
    uint32_t offset = (uint32_t)pointer_delta(dest, aligned_dest, 1);
    uint32_t shift = BitsPerByte * offset;  // little endian
    uint32_t old_h = (uint32_t)(uint16_t)compare_value;
    uint32_t new_h = (uint32_t)(uint16_t)exchange_value;

    uint32_t cur = (Atomic::load(aligned_dest) & ~(0xffffU << shift)) | (old_h << shift);
    uint32_t res, temp_w;
    do {
      uint32_t new_w = (cur & ~(0xffffU << shift)) | (new_h << shift);
      switch (order) {
      case memory_order_relaxed:
      case memory_order_release:
        if (UseSameCachelineLoadLoadReorder) {
          asm volatile (
            "1: ll.w %[prev], %[dest]     \n\t"
            "   bne  %[prev], %[_old], 2f \n\t"
            "   move %[temp], %[_new]     \n\t"
            "   sc.w %[temp], %[dest]     \n\t"
            "   beqz %[temp], 1b          \n\t"
            "   b    3f                   \n\t"
            "2: dbar 0x700                \n\t"
            "3:                           \n\t"
            : [prev] "=&r" (res), [temp] "=&r" (temp_w)
            : [_old] "r" (cur), [_new] "r" (new_w), [dest] "ZC" (*aligned_dest)
            : "memory");
        } else {
          asm volatile (
            "1: ll.w %[prev], %[dest]     \n\t"
            "   bne  %[prev], %[_old], 2f \n\t"
            "   move %[temp], %[_new]     \n\t"
            "   sc.w %[temp], %[dest]     \n\t"
            "   beqz %[temp], 1b          \n\t"
            "2:                           \n\t"
            : [prev] "=&r" (res), [temp] "=&r" (temp_w)
            : [_old] "r" (cur), [_new] "r" (new_w), [dest] "ZC" (*aligned_dest)
            : "memory");
        }
        break;
      default:
        asm volatile (
          "1: ll.w %[prev], %[dest]     \n\t"
          "   bne  %[prev], %[_old], 2f \n\t"
          "   move %[temp], %[_new]     \n\t"
          "   sc.w %[temp], %[dest]     \n\t"
          "   beqz %[temp], 1b          \n\t"
          "   b    3f                   \n\t"
          "2: dbar 0x14                 \n\t"
          "3:                           \n\t"
          : [prev] "=&r" (res), [temp] "=&r" (temp_w)
          : [_old] "r" (cur), [_new] "r" (new_w), [dest] "ZC" (*aligned_dest)
          : "memory");
        break;
      }
      if (res == cur) break;  // success
      cur = res;
    } while (true);
    return (T)(uint16_t)(cur >> shift);
  }

  AMCAS_MACRO
  uint64_t prev;
  uint64_t old_value = (uint64_t)(uint16_t)compare_value;
  uint64_t new_value = (uint64_t)(uint16_t)exchange_value;

  switch (order) {
  case memory_order_relaxed:
    asm volatile (
      " move %[prev], %[_old] \n\t"
      " amcas_h %[prev], %[_new], %[dest] \n\t"
      : [prev] "+&r" (prev)
      : [_old] "r" (old_value), [_new] "r" (new_value), [dest] "r" (dest)
      : "memory");
    break;
  case memory_order_acquire:
    asm volatile (
      " move %[prev], %[_old] \n\t"
      " amcas_h %[prev], %[_new], %[dest] \n\t"
      " dbar 0x14 \n\t"
      : [prev] "+&r" (prev)
      : [_old] "r" (old_value), [_new] "r" (new_value), [dest] "r" (dest)
      : "memory");
    break;
  case memory_order_release:
    asm volatile (
      " move %[prev], %[_old] \n\t"
      " dbar 0x12 \n\t"
      " amcas_h %[prev], %[_new], %[dest] \n\t"
      : [prev] "+&r" (prev)
      : [_old] "r" (old_value), [_new] "r" (new_value), [dest] "r" (dest)
      : "memory");
    break;
  default:
    asm volatile (
      " move %[prev], %[_old] \n\t"
      " amcas_db_h %[prev], %[_new], %[dest] \n\t"
      : [prev] "+&r" (prev)
      : [_old] "r" (old_value), [_new] "r" (new_value), [dest] "r" (dest)
      : "memory");
    break;
  }

  return (T)(uint16_t)prev;  // amcas.h returns a sign-extended old value
}

template<>
template<typename T>
inline T Atomic::PlatformCmpxchg<4>::operator()(T volatile* dest,
                                                T compare_value,
                                                T exchange_value,
                                                atomic_memory_order order) const {
  STATIC_ASSERT(4 == sizeof(T));
  T prev, temp;

  if (UseAMCAS) {
    AMCAS_MACRO
    switch (order) {
    case memory_order_relaxed:
      asm volatile (
        " move %[prev], %[_old] \n\t"
        " amcas_w %[prev], %[_new], %[dest] \n\t"
        : [prev] "+&r" (prev)
        : [_old] "r" (compare_value), [_new] "r" (exchange_value), [dest] "r" (dest)
        : "memory");
      break;
    case memory_order_acquire:
      asm volatile (
        " move %[prev], %[_old] \n\t"
        " amcas_w %[prev], %[_new], %[dest] \n\t"
        " dbar 0x14 \n\t"
        : [prev] "+&r" (prev)
        : [_old] "r" (compare_value), [_new] "r" (exchange_value), [dest] "r" (dest)
        : "memory");
      break;
    case memory_order_release:
      asm volatile (
        " move %[prev], %[_old] \n\t"
        " dbar 0x12 \n\t"
        " amcas_w %[prev], %[_new], %[dest] \n\t"
        : [prev] "+&r" (prev)
        : [_old] "r" (compare_value), [_new] "r" (exchange_value), [dest] "r" (dest)
        : "memory");
      break;
    default:
      asm volatile (
        " move %[prev], %[_old] \n\t"
        " amcas_db_w %[prev], %[_new], %[dest] \n\t"
        : [prev] "+&r" (prev)
        : [_old] "r" (compare_value), [_new] "r" (exchange_value), [dest] "r" (dest)
        : "memory");
      break;
    }
  } else {
    switch (order) {
    case memory_order_relaxed:
    case memory_order_release:
      if (UseSameCachelineLoadLoadReorder) {
        asm volatile (
          "1: ll.w %[prev], %[dest]     \n\t"
          "   bne  %[prev], %[_old], 2f \n\t"
          "   move %[temp], %[_new]     \n\t"
          "   sc.w %[temp], %[dest]     \n\t"
          "   beqz %[temp], 1b          \n\t"
          "   b    3f                   \n\t"
          "2: dbar 0x700                \n\t"
          "3:                           \n\t"
          : [prev] "=&r" (prev), [temp] "=&r" (temp)
          : [_old] "r" (compare_value), [_new] "r" (exchange_value), [dest] "ZC" (*dest)
          : "memory");
      } else {
        asm volatile (
          "1: ll.w %[prev], %[dest]     \n\t"
          "   bne  %[prev], %[_old], 2f \n\t"
          "   move %[temp], %[_new]     \n\t"
          "   sc.w %[temp], %[dest]     \n\t"
          "   beqz %[temp], 1b          \n\t"
          "2:                           \n\t"
          : [prev] "=&r" (prev), [temp] "=&r" (temp)
          : [_old] "r" (compare_value), [_new] "r" (exchange_value), [dest] "ZC" (*dest)
          : "memory");
      }
      break;
    default:
      asm volatile (
        "1: ll.w %[prev], %[dest]     \n\t"
        "   bne  %[prev], %[_old], 2f \n\t"
        "   move %[temp], %[_new]     \n\t"
        "   sc.w %[temp], %[dest]     \n\t"
        "   beqz %[temp], 1b          \n\t"
        "   b    3f                   \n\t"
        "2: dbar 0x14                 \n\t"
        "3:                           \n\t"
        : [prev] "=&r" (prev), [temp] "=&r" (temp)
        : [_old] "r" (compare_value), [_new] "r" (exchange_value), [dest] "ZC" (*dest)
        : "memory");
      break;
    }
  }

  return prev;
}

template<>
template<typename T>
inline T Atomic::PlatformCmpxchg<8>::operator()(T volatile* dest,
                                                T compare_value,
                                                T exchange_value,
                                                atomic_memory_order order) const {
  STATIC_ASSERT(8 == sizeof(T));
  T prev, temp;

  if (UseAMCAS) {
    AMCAS_MACRO
    switch (order) {
    case memory_order_relaxed:
      asm volatile (
        " move %[prev], %[_old] \n\t"
        " amcas_d %[prev], %[_new], %[dest] \n\t"
        : [prev] "+&r" (prev)
        : [_old] "r" (compare_value), [_new] "r" (exchange_value), [dest] "r" (dest)
        : "memory");
      break;
    case memory_order_acquire:
      asm volatile (
        " move %[prev], %[_old] \n\t"
        " amcas_d %[prev], %[_new], %[dest] \n\t"
        " dbar 0x14 \n\t"
        : [prev] "+&r" (prev)
        : [_old] "r" (compare_value), [_new] "r" (exchange_value), [dest] "r" (dest)
        : "memory");
      break;
    case memory_order_release:
      asm volatile (
        " move %[prev], %[_old] \n\t"
        " dbar 0x12 \n\t"
        " amcas_d %[prev], %[_new], %[dest] \n\t"
        : [prev] "+&r" (prev)
        : [_old] "r" (compare_value), [_new] "r" (exchange_value), [dest] "r" (dest)
        : "memory");
      break;
    default:
      asm volatile (
        " move %[prev], %[_old] \n\t"
        " amcas_db_d %[prev], %[_new], %[dest] \n\t"
        : [prev] "+&r" (prev)
        : [_old] "r" (compare_value), [_new] "r" (exchange_value), [dest] "r" (dest)
        : "memory");
      break;
    }
  } else {
    switch (order) {
    case memory_order_relaxed:
    case memory_order_release:
      if (UseSameCachelineLoadLoadReorder) {
        asm volatile (
          "1: ll.d %[prev], %[dest]     \n\t"
          "   bne  %[prev], %[_old], 2f \n\t"
          "   move %[temp], %[_new]     \n\t"
          "   sc.d %[temp], %[dest]     \n\t"
          "   beqz %[temp], 1b          \n\t"
          "   b    3f                   \n\t"
          "2: dbar 0x700                \n\t"
          "3:                           \n\t"
          : [prev] "=&r" (prev), [temp] "=&r" (temp)
          : [_old] "r" (compare_value), [_new] "r" (exchange_value), [dest] "ZC" (*dest)
          : "memory");
      } else {
        asm volatile (
          "1: ll.d %[prev], %[dest]     \n\t"
          "   bne  %[prev], %[_old], 2f \n\t"
          "   move %[temp], %[_new]     \n\t"
          "   sc.d %[temp], %[dest]     \n\t"
          "   beqz %[temp], 1b          \n\t"
          "2:                           \n\t"
          : [prev] "=&r" (prev), [temp] "=&r" (temp)
          : [_old] "r" (compare_value), [_new] "r" (exchange_value), [dest] "ZC" (*dest)
          : "memory");
      }
      break;
    default:
      asm volatile (
        "1: ll.d %[prev], %[dest]     \n\t"
        "   bne  %[prev], %[_old], 2f \n\t"
        "   move %[temp], %[_new]     \n\t"
        "   sc.d %[temp], %[dest]     \n\t"
        "   beqz %[temp], 1b          \n\t"
        "   b    3f                   \n\t"
        "2: dbar 0x14                 \n\t"
        "3:                           \n\t"
        : [prev] "=&r" (prev), [temp] "=&r" (temp)
        : [_old] "r" (compare_value), [_new] "r" (exchange_value), [dest] "ZC" (*dest)
        : "memory");
      break;
    }
  }

  return prev;
}

template<size_t byte_size>
struct Atomic::PlatformOrderedLoad<byte_size, X_ACQUIRE>
{
  template <typename T>
  T operator()(const volatile T* p) const { T data; __atomic_load(const_cast<T*>(p), &data, __ATOMIC_ACQUIRE); return data; }
};

template<>
struct Atomic::PlatformOrderedStore<4, RELEASE_X>
{
  template <typename T>
  void operator()(volatile T* p, T v) const { xchg(p, v, memory_order_release); }
};

template<>
struct Atomic::PlatformOrderedStore<8, RELEASE_X>
{
  template <typename T>
  void operator()(volatile T* p, T v) const { xchg(p, v, memory_order_release); }
};

template<>
struct Atomic::PlatformOrderedStore<4, RELEASE_X_FENCE>
{
  template <typename T>
  void operator()(volatile T* p, T v) const { xchg(p, v, memory_order_conservative); }
};

template<>
struct Atomic::PlatformOrderedStore<8, RELEASE_X_FENCE>
{
  template <typename T>
  void operator()(volatile T* p, T v) const { xchg(p, v, memory_order_conservative); }
};

#endif // OS_CPU_LINUX_LOONGARCH_ATOMIC_LINUX_LOONGARCH_HPP
