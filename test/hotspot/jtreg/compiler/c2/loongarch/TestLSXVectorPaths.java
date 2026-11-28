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

import java.util.Arrays;
import java.util.Random;

import jdk.incubator.vector.IntVector;
import jdk.incubator.vector.VectorSpecies;

/*
 * @test
 * @enablePreview
 * @modules jdk.incubator.vector
 * @summary Exercise GCC-aligned LSX integer abs and load+broadcast paths.
 * @requires vm.cpu.features ~= ".*lsx.*"
 * @run main/othervm -Xbatch -XX:-TieredCompilation -XX:-UseLASX -XX:+UseLSX
 *                   compiler.c2.loongarch.TestLSXVectorPaths
 */
public class TestLSXVectorPaths {
    private static final VectorSpecies<Integer> SPECIES = IntVector.SPECIES_128;

    private static void run(int[] input, int[] output) {
        IntVector vector = IntVector.fromArray(SPECIES, input, 0);
        vector.abs().intoArray(output, 0);

        int broadcast = input[0];
        IntVector broadcastVector = IntVector.broadcast(SPECIES, broadcast);
        broadcastVector.intoArray(output, SPECIES.length());
        broadcastVector.add(vector).intoArray(output, SPECIES.length() * 2);
    }

    public static void main(String[] args) {
        Random random = new Random(1);
        int length = SPECIES.length() * 3;
        int[] input = new int[SPECIES.length()];
        int[] output = new int[length];
        int[] expected = new int[length];

        for (int iteration = 0; iteration < 20_000; iteration++) {
            for (int i = 0; i < input.length; i++) {
                input[i] = random.nextInt();
            }
            run(input, output);

            for (int i = 0; i < input.length; i++) {
                expected[i] = Math.abs(input[i]);
                expected[input.length + i] = input[0];
                expected[2 * input.length + i] = input[0] + input[i];
            }
            if (!Arrays.equals(expected, output)) {
                throw new AssertionError("iteration " + iteration +
                    ", input=" + Arrays.toString(input) +
                    ": expected=" + Arrays.toString(expected) +
                    ", actual=" + Arrays.toString(output));
            }
        }
    }
}
