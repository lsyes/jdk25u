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
 */

package compiler.c2.loongarch;

import java.math.BigInteger;
import java.util.Random;
import java.util.function.IntUnaryOperator;
import java.util.function.LongUnaryOperator;

/**
 * @test
 * @summary Test C2 strength reduction of unsigned division and remainder by
 *          constant divisors on LoongArch
 * @requires os.arch == "loongarch64"
 * @run main/othervm -Xcomp -XX:-TieredCompilation compiler.c2.loongarch.TestUnsignedConstantDivMod
 */
public class TestUnsignedConstantDivMod {
    private static final int[] INT_VALUES = {
        0, 1, 2, 3, 6, 7, 13, 14, 15, 1_000,
        Integer.MAX_VALUE - 1, Integer.MAX_VALUE,
        Integer.MIN_VALUE, Integer.MIN_VALUE + 1,
        -2, -1
    };

    private static final long[] LONG_VALUES = {
        0L, 1L, 2L, 3L, 6L, 7L, 13L, 14L, 15L, 1_000L,
        Long.MAX_VALUE - 1, Long.MAX_VALUE,
        Long.MIN_VALUE, Long.MIN_VALUE + 1,
        -2L, -1L
    };

    private static final BigInteger TWO_64 = BigInteger.ONE.shiftLeft(64);

    public static void main(String[] args) {
        checkInt("udiv3", TestUnsignedConstantDivMod::udiv3, 3);
        checkInt("urem3", TestUnsignedConstantDivMod::urem3, 3);
        checkInt("udiv7", TestUnsignedConstantDivMod::udiv7, 7);
        checkInt("urem7", TestUnsignedConstantDivMod::urem7, 7);
        checkInt("udiv14", TestUnsignedConstantDivMod::udiv14, 14);
        checkInt("urem14", TestUnsignedConstantDivMod::urem14, 14);
        checkInt("udivTopBit", TestUnsignedConstantDivMod::udivTopBit, 0x8000_0001);
        checkInt("uremTopBit", TestUnsignedConstantDivMod::uremTopBit, 0x8000_0001);

        checkLong("uldiv3", TestUnsignedConstantDivMod::uldiv3, 3L);
        checkLong("ulrem3", TestUnsignedConstantDivMod::ulrem3, 3L);
        checkLong("uldiv7", TestUnsignedConstantDivMod::uldiv7, 7L);
        checkLong("ulrem7", TestUnsignedConstantDivMod::ulrem7, 7L);
        checkLong("uldiv14", TestUnsignedConstantDivMod::uldiv14, 14L);
        checkLong("ulrem14", TestUnsignedConstantDivMod::ulrem14, 14L);
        checkLong("uldivTopBit", TestUnsignedConstantDivMod::uldivTopBit, 0x8000_0000_0000_0001L);
        checkLong("ulremTopBit", TestUnsignedConstantDivMod::ulremTopBit, 0x8000_0000_0000_0001L);
    }

    private static void checkInt(String name, IntUnaryOperator op, int divisor) {
        long unsignedDivisor = Integer.toUnsignedLong(divisor);
        for (int value : INT_VALUES) {
            checkInt(name, op, divisor, value, unsignedDivisor);
        }

        Random random = new Random(0x4c6f_6f6e_6741_7263L);
        for (int i = 0; i < 20_000; i++) {
            checkInt(name, op, divisor, random.nextInt(), unsignedDivisor);
        }
    }

    private static void checkInt(String name, IntUnaryOperator op, int divisor,
                                 int value, long unsignedDivisor) {
        long unsignedValue = Integer.toUnsignedLong(value);
        long expected = name.startsWith("udiv")
                ? unsignedValue / unsignedDivisor
                : unsignedValue % unsignedDivisor;
        int actual = op.applyAsInt(value);
        if (actual != (int) expected) {
            throw new RuntimeException(name + " failed: value=" + unsignedValue
                    + ", divisor=" + unsignedDivisor + ", expected=" + expected
                    + ", actual=" + Integer.toUnsignedLong(actual));
        }
    }

    private static void checkLong(String name, LongUnaryOperator op, long divisor) {
        BigInteger unsignedDivisor = toUnsigned(divisor);
        for (long value : LONG_VALUES) {
            checkLong(name, op, divisor, value, unsignedDivisor);
        }

        Random random = new Random(0x4c6f_6f6e_6741_7263L);
        for (int i = 0; i < 20_000; i++) {
            checkLong(name, op, divisor, random.nextLong(), unsignedDivisor);
        }
    }

    private static void checkLong(String name, LongUnaryOperator op, long divisor,
                                  long value, BigInteger unsignedDivisor) {
        BigInteger unsignedValue = toUnsigned(value);
        BigInteger expected = name.startsWith("uldiv")
                ? unsignedValue.divide(unsignedDivisor)
                : unsignedValue.remainder(unsignedDivisor);
        long actual = op.applyAsLong(value);
        if (actual != expected.longValue()) {
            throw new RuntimeException(name + " failed: value=" + unsignedValue
                    + ", divisor=" + unsignedDivisor + ", expected=" + expected
                    + ", actual=" + toUnsigned(actual));
        }
    }

    private static BigInteger toUnsigned(long value) {
        BigInteger result = BigInteger.valueOf(value);
        return value < 0 ? result.add(TWO_64) : result;
    }

    private static int udiv3(int value) {
        return Integer.divideUnsigned(value, 3);
    }

    private static int urem3(int value) {
        return Integer.remainderUnsigned(value, 3);
    }

    private static int udiv7(int value) {
        return Integer.divideUnsigned(value, 7);
    }

    private static int urem7(int value) {
        return Integer.remainderUnsigned(value, 7);
    }

    private static int udiv14(int value) {
        return Integer.divideUnsigned(value, 14);
    }

    private static int urem14(int value) {
        return Integer.remainderUnsigned(value, 14);
    }

    private static int udivTopBit(int value) {
        return Integer.divideUnsigned(value, 0x8000_0001);
    }

    private static int uremTopBit(int value) {
        return Integer.remainderUnsigned(value, 0x8000_0001);
    }

    private static long uldiv3(long value) {
        return Long.divideUnsigned(value, 3L);
    }

    private static long ulrem3(long value) {
        return Long.remainderUnsigned(value, 3L);
    }

    private static long uldiv7(long value) {
        return Long.divideUnsigned(value, 7L);
    }

    private static long ulrem7(long value) {
        return Long.remainderUnsigned(value, 7L);
    }

    private static long uldiv14(long value) {
        return Long.divideUnsigned(value, 14L);
    }

    private static long ulrem14(long value) {
        return Long.remainderUnsigned(value, 14L);
    }

    private static long uldivTopBit(long value) {
        return Long.divideUnsigned(value, 0x8000_0000_0000_0001L);
    }

    private static long ulremTopBit(long value) {
        return Long.remainderUnsigned(value, 0x8000_0000_0000_0001L);
    }
}
