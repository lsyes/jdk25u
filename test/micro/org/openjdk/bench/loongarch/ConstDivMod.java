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

package org.openjdk.bench.loongarch;

import java.util.Random;

import org.openjdk.jmh.annotations.Benchmark;
import org.openjdk.jmh.annotations.BenchmarkMode;
import org.openjdk.jmh.annotations.Fork;
import org.openjdk.jmh.annotations.Measurement;
import org.openjdk.jmh.annotations.Mode;
import org.openjdk.jmh.annotations.OutputTimeUnit;
import org.openjdk.jmh.annotations.Warmup;

import java.util.concurrent.TimeUnit;

/**
 * Benchmarks for the code generation of constant integer division/remainder
 * and constant multiplication on LoongArch.
 *
 * <p>These exercise the paths that are strength reduced to one or more ALU
 * instructions instead of a hardware div/mod (12 cycles for 32 bit, 25 cycles
 * for 64 bit on LA464/LA664):
 * <ul>
 *   <li>signed division/remainder by an invariant divisor (magic number
 *       multiply high, {@code mulh.w});</li>
 *   <li>unsigned division/remainder by an invariant divisor (planned by the
 *       Rust instruction scheduler, {@code mulh.wu}/{@code mulh.du});</li>
 *   <li>{@code x*3}, {@code x*5}, {@code x*9} which a single {@code alsl}
 *       implements.</li>
 * </ul>
 */
@BenchmarkMode(Mode.AverageTime)
@OutputTimeUnit(TimeUnit.MICROSECONDS)
@Warmup(iterations = 3, time = 1)
@Measurement(iterations = 5, time = 1)
@Fork(1)
public class ConstDivMod {

    static final int SIZE = 4096;
    static final int[] INTS = new int[SIZE];
    static final long[] LONGS = new long[SIZE];

    static {
        Random r = new Random(4711);
        for (int i = 0; i < SIZE; i++) {
            INTS[i] = r.nextInt();
            LONGS[i] = r.nextLong();
        }
    }

    @Benchmark
    public int signedDiv() {
        int s = 0;
        for (int v : INTS) {
            s += v / 3 + v / 10 + v / 100;
        }
        return s;
    }

    @Benchmark
    public int signedRem() {
        int s = 0;
        for (int v : INTS) {
            s += v % 3 + v % 7 + v % 1000;
        }
        return s;
    }

    @Benchmark
    public int unsignedDiv() {
        int s = 0;
        for (int v : INTS) {
            s += Integer.divideUnsigned(v, 3) + Integer.divideUnsigned(v, 1000);
        }
        return s;
    }

    @Benchmark
    public int unsignedRem() {
        int s = 0;
        for (int v : INTS) {
            s += Integer.remainderUnsigned(v, 7) + Integer.remainderUnsigned(v, 1000);
        }
        return s;
    }

    @Benchmark
    public long unsignedDivLong() {
        long s = 0;
        for (long v : LONGS) {
            s += Long.divideUnsigned(v, 3) + Long.divideUnsigned(v, 1000);
        }
        return s;
    }

    @Benchmark
    public long unsignedRemLong() {
        long s = 0;
        for (long v : LONGS) {
            s += Long.remainderUnsigned(v, 7) + Long.remainderUnsigned(v, 1000);
        }
        return s;
    }

    @Benchmark
    public int mulConst() {
        int s = 0;
        for (int v : INTS) {
            s += v * 3 + v * 5 + v * 9;
        }
        return s;
    }

    @Benchmark
    public long mulConstLong() {
        long s = 0;
        for (long v : LONGS) {
            s += v * 3 + v * 5 + v * 9;
        }
        return s;
    }
}
