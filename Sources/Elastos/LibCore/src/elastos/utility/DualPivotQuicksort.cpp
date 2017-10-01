//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "DualPivotQuicksort.h"
#include "Arrays.h"
#include "Math.h"

namespace Elastos {
namespace Utility {

const Int32 DualPivotQuicksort::INSERTION_SORT_THRESHOLD = 32;

const Int32 DualPivotQuicksort::COUNTING_SORT_THRESHOLD_FOR_BYTE = 128;

const Int32 DualPivotQuicksort::COUNTING_SORT_THRESHOLD_FOR_SHORT_OR_CHAR = 32768;

const Int32 DualPivotQuicksort::NUM_SHORT_VALUES = 1 << 16;

const Int32 DualPivotQuicksort::NUM_CHAR_VALUES = 1 << 16;

const Int32 DualPivotQuicksort::NUM_BYTE_VALUES = 1 << 8;

ECode DualPivotQuicksort::Sort(
    /* [in] */ ArrayOf<Int32>* a)
{
    return DoSort(a, 0, a->GetLength() - 1);
}

ECode DualPivotQuicksort::Sort(
    /* [in] */ ArrayOf<Int32>* a,
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int32 toIndex)
{
    FAIL_RETURN(Arrays::CheckStartAndEnd(a->GetLength(), fromIndex, toIndex));
    return DoSort(a, fromIndex, toIndex - 1);
}


ECode DualPivotQuicksort::DoSort(
    /* [in] */ ArrayOf<Int32>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    // Use insertion sort on tiny arrays
    if (right - left + 1 < INSERTION_SORT_THRESHOLD) {
        for (Int32 i = left + 1; i <= right; i++) {
            Int32 ai = (*a)[i];
            Int32 j;
            for (j = i - 1; j >= left && ai < (*a)[j]; j--) {
                (*a)[j + 1] = (*a)[j];
            }
            (*a)[j + 1] = ai;
        }
    }
    else { // Use Dual-Pivot Quicksort on large arrays
        return _DualPivotQuicksort(a, left, right);
    }
    return NOERROR;
}

ECode DualPivotQuicksort::_DualPivotQuicksort(
    /* [in] */ ArrayOf<Int32>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    // Compute indices of five evenly spaced elements
    Int32 sixth = (right - left + 1) / 6;
    Int32 e1 = left  + sixth;
    Int32 e5 = right - sixth;
    Int32 e3 = (UInt32)(left + right) >> 1; // The midpoint
    Int32 e4 = e3 + sixth;
    Int32 e2 = e3 - sixth;

    // Sort these elements using a 5-element sorting network
    Int32 ae1 = (*a)[e1], ae2 = (*a)[e2], ae3 = (*a)[e3], ae4 = (*a)[e4], ae5 = (*a)[e5];

    if (ae1 > ae2) { Int32 t = ae1; ae1 = ae2; ae2 = t; }
    if (ae4 > ae5) { Int32 t = ae4; ae4 = ae5; ae5 = t; }
    if (ae1 > ae3) { Int32 t = ae1; ae1 = ae3; ae3 = t; }
    if (ae2 > ae3) { Int32 t = ae2; ae2 = ae3; ae3 = t; }
    if (ae1 > ae4) { Int32 t = ae1; ae1 = ae4; ae4 = t; }
    if (ae3 > ae4) { Int32 t = ae3; ae3 = ae4; ae4 = t; }
    if (ae2 > ae5) { Int32 t = ae2; ae2 = ae5; ae5 = t; }
    if (ae2 > ae3) { Int32 t = ae2; ae2 = ae3; ae3 = t; }
    if (ae4 > ae5) { Int32 t = ae4; ae4 = ae5; ae5 = t; }

    (*a)[e1] = ae1; (*a)[e3] = ae3; (*a)[e5] = ae5;

    /*
     * Use the second and fourth of the five sorted elements as pivots.
     * These values are inexpensive approximations of the first and
     * second terciles of the array. Note that pivot1 <= pivot2.
     *
     * The pivots are stored in local variables, and the first and
     * the last of the elements to be sorted are moved to the locations
     * formerly occupied by the pivots. When partitioning is complete,
     * the pivots are swapped back into their final positions, and
     * excluded from subsequent sorting.
     */
    Int32 pivot1 = ae2; (*a)[e2] = (*a)[left];
    Int32 pivot2 = ae4; (*a)[e4] = (*a)[right];

    // Pointers
    Int32 less  = left  + 1; // The index of first element of center part
    Int32 great = right - 1; // The index before first element of right part

    Boolean pivotsDiffer = (pivot1 != pivot2);

    if (pivotsDiffer) {
        /*
         * Partitioning:
         *
         *   left part         center part                    right part
         * +------------------------------------------------------------+
         * | < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2 |
         * +------------------------------------------------------------+
         *              ^                          ^       ^
         *              |                          |       |
         *             less                        k     great
         *
         * Invariants:
         *
         *              all in (left, less)   < pivot1
         *    pivot1 <= all in [less, k)     <= pivot2
         *              all in (great, right) > pivot2
         *
         * Pointer k is the first index of ?-part
         */
        for (Int32 k = less; k <= great; k++) {
            Int32 ak = (*a)[k];
            if (ak < pivot1) { // Move (*a)[k] to left part
                if (k != less) {
                    (*a)[k] = (*a)[less];
                    (*a)[less] = ak;
                }
                less++;
            }
            else if (ak > pivot2) { // Move (*a)[k] to right part
                while ((*a)[great] > pivot2) {
                    if (great-- == k) {
                        goto outer;
                    }
                }
                if ((*a)[great] < pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = (*a)[great];
                    (*a)[great--] = ak;
                }
                else { // pivot1 <= (*a)[great] <= pivot2
                    (*a)[k] = (*a)[great];
                    (*a)[great--] = ak;
                }
            }
        }

    outer: { /* nothing */ } { /* nothing */ }
    }
    else { // Pivots are equal
        /*
         * Partition degenerates to the traditional 3-way,
         * or "Dutch National Flag", partition:
         *
         *   left part   center part            right part
         * +----------------------------------------------+
         * |  < pivot  |  == pivot  |    ?    |  > pivot  |
         * +----------------------------------------------+
         *              ^            ^       ^
         *              |            |       |
         *             less          k     great
         *
         * Invariants:
         *
         *   all in (left, less)   < pivot
         *   all in [less, k)     == pivot
         *   all in (great, right) > pivot
         *
         * Pointer k is the first index of ?-part
        */
        for (Int32 k = less; k <= great; k++) {
            Int32 ak = (*a)[k];
            if (ak == pivot1) {
                continue;
            }
            if (ak < pivot1) { // Move (*a)[k] to left part
                if (k != less) {
                    (*a)[k] = (*a)[less];
                    (*a)[less] = ak;
                }
                less++;
            }
            else { // ((*a)[k] > pivot1) -  Move (*a)[k] to right part
                /*
                 * We know that pivot1 == (*a)[e3] == pivot2. Thus, we know
                 * that great will still be >= k when the following loop
                 * terminates, even though we don't test for it explicitly.
                 * In other words, (*a)[e3] acts as a sentinel for great.
                 */
                while ((*a)[great] > pivot1) {
                    great--;
                }
                if ((*a)[great] < pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = (*a)[great];
                    (*a)[great--] = ak;
                }
                else { // (*a)[great] == pivot1
                    (*a)[k] = pivot1;
                    (*a)[great--] = ak;
                }
            }
        }
    }

    // Swap pivots into their final positions
    (*a)[left]  = (*a)[less  - 1]; (*a)[less  - 1] = pivot1;
    (*a)[right] = (*a)[great + 1]; (*a)[great + 1] = pivot2;

    // Sort left and right parts recursively, excluding known pivot values
    DoSort(a, left,   less - 2);
    DoSort(a, great + 2, right);

    /*
     * If pivot1 == pivot2, all elements from center
     * part are equal and, therefore, already sorted
     */
    if (!pivotsDiffer) {
        return NOERROR;
    }

    /*
     * If center part is too large (comprises > 2/3 of the array),
     * swap internal pivot values to ends
     */
    if (less < e1 && great > e5) {
        while ((*a)[less] == pivot1) {
            less++;
        }
        while ((*a)[great] == pivot2) {
            great--;
        }

        /*
         * Partitioning:
         *
         *   left part       center part                   right part
         * +----------------------------------------------------------+
         * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
         * +----------------------------------------------------------+
         *              ^                        ^       ^
         *              |                        |       |
         *             less                      k     great
         *
         * Invariants:
         *
         *              all in (*, less)  == pivot1
         *     pivot1 < all in [less, k)   < pivot2
         *              all in (great, *) == pivot2
         *
         * Pointer k is the first index of ?-part
         */

        for (Int32 k = less; k <= great; k++) {
            Int32 ak = (*a)[k];
            if (ak == pivot2) { // Move (*a)[k] to right part
                while ((*a)[great] == pivot2) {
                    if (great-- == k) {
                        goto outer2;
                    }
                }
                if ((*a)[great] == pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = pivot1;
                }
                else { // pivot1 < (*a)[great] < pivot2
                    (*a)[k] = (*a)[great];
                }
                (*a)[great--] = pivot2;
            }
            else if (ak == pivot1) { // Move (*a)[k] to left part
                (*a)[k] = (*a)[less];
                (*a)[less++] = pivot1;
            }
        }

    outer2: { /* nothing */ }
    }

    // Sort center part recursively, excluding known pivot values
    DoSort(a, less, great);
    return NOERROR;
}

ECode DualPivotQuicksort::Sort(
    /* [in] */ ArrayOf<Int64>* a)
{
    return DoSort(a, 0, a->GetLength() - 1);
}

ECode DualPivotQuicksort::Sort(
    /* [in] */ ArrayOf<Int64>* a,
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int32 toIndex)
{
    FAIL_RETURN(Arrays::CheckStartAndEnd(a->GetLength(), fromIndex, toIndex));
    return DoSort(a, fromIndex, toIndex - 1);
}

ECode DualPivotQuicksort::DoSort(
    /* [in] */ ArrayOf<Int64>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    // Use insertion sort on tiny arrays
    if (right - left + 1 < INSERTION_SORT_THRESHOLD) {
        for (Int32 i = left + 1; i <= right; i++) {
            Int64 ai = (*a)[i];
            Int32 j;
            for (j = i - 1; j >= left && ai < (*a)[j]; j--) {
                (*a)[j + 1] = (*a)[j];
            }
            (*a)[j + 1] = ai;
        }
    }
    else { // Use Dual-Pivot Quicksort on large arrays
        return _DualPivotQuicksort(a, left, right);
    }
    return NOERROR;
}

ECode DualPivotQuicksort::_DualPivotQuicksort(
    /* [in] */ ArrayOf<Int64>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    // Compute indices of five evenly spaced elements
    Int32 sixth = (right - left + 1) / 6;
    Int32 e1 = left  + sixth;
    Int32 e5 = right - sixth;
    Int32 e3 = (UInt32)(left + right) >> 1; // The midpoint
    Int32 e4 = e3 + sixth;
    Int32 e2 = e3 - sixth;

    // Sort these elements using a 5-element sorting network
    Int64 ae1 = (*a)[e1], ae2 = (*a)[e2], ae3 = (*a)[e3], ae4 = (*a)[e4], ae5 = (*a)[e5];

    if (ae1 > ae2) { Int64 t = ae1; ae1 = ae2; ae2 = t; }
    if (ae4 > ae5) { Int64 t = ae4; ae4 = ae5; ae5 = t; }
    if (ae1 > ae3) { Int64 t = ae1; ae1 = ae3; ae3 = t; }
    if (ae2 > ae3) { Int64 t = ae2; ae2 = ae3; ae3 = t; }
    if (ae1 > ae4) { Int64 t = ae1; ae1 = ae4; ae4 = t; }
    if (ae3 > ae4) { Int64 t = ae3; ae3 = ae4; ae4 = t; }
    if (ae2 > ae5) { Int64 t = ae2; ae2 = ae5; ae5 = t; }
    if (ae2 > ae3) { Int64 t = ae2; ae2 = ae3; ae3 = t; }
    if (ae4 > ae5) { Int64 t = ae4; ae4 = ae5; ae5 = t; }

    (*a)[e1] = ae1; (*a)[e3] = ae3; (*a)[e5] = ae5;

    /*
     * Use the second and fourth of the five sorted elements as pivots.
     * These values are inexpensive approximations of the first and
     * second terciles of the array. Note that pivot1 <= pivot2.
     *
     * The pivots are stored in local variables, and the first and
     * the last of the elements to be sorted are moved to the locations
     * formerly occupied by the pivots. When partitioning is complete,
     * the pivots are swapped back into their final positions, and
     * excluded from subsequent sorting.
     */
    Int64 pivot1 = ae2; (*a)[e2] = (*a)[left];
    Int64 pivot2 = ae4; (*a)[e4] = (*a)[right];

    // Pointers
    Int32 less  = left  + 1; // The index of first element of center part
    Int32 great = right - 1; // The index before first element of right part

    Boolean pivotsDiffer = (pivot1 != pivot2);

    if (pivotsDiffer) {
        /*
         * Partitioning:
         *
         *   left part         center part                    right part
         * +------------------------------------------------------------+
         * | < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2 |
         * +------------------------------------------------------------+
         *              ^                          ^       ^
         *              |                          |       |
         *             less                        k     great
         *
         * Invariants:
         *
         *              all in (left, less)   < pivot1
         *    pivot1 <= all in [less, k)     <= pivot2
         *              all in (great, right) > pivot2
         *
         * Pointer k is the first index of ?-part
         */
        for (Int32 k = less; k <= great; k++) {
            Int64 ak = (*a)[k];
            if (ak < pivot1) { // Move (*a)[k] to left part
                if (k != less) {
                    (*a)[k] = (*a)[less];
                    (*a)[less] = ak;
                }
                less++;
            }
            else if (ak > pivot2) { // Move (*a)[k] to right part
                while ((*a)[great] > pivot2) {
                    if (great-- == k) {
                        goto outer;
                    }
                }
                if ((*a)[great] < pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = (*a)[great];
                    (*a)[great--] = ak;
                }
                else { // pivot1 <= (*a)[great] <= pivot2
                    (*a)[k] = (*a)[great];
                    (*a)[great--] = ak;
                }
            }
        }
    outer: { /* nothing */ }
    }
    else { // Pivots are equal
        /*
         * Partition degenerates to the traditional 3-way,
         * or "Dutch National Flag", partition:
         *
         *   left part   center part            right part
         * +----------------------------------------------+
         * |  < pivot  |  == pivot  |    ?    |  > pivot  |
         * +----------------------------------------------+
         *              ^            ^       ^
         *              |            |       |
         *             less          k     great
         *
         * Invariants:
         *
         *   all in (left, less)   < pivot
         *   all in [less, k)     == pivot
         *   all in (great, right) > pivot
         *
         * Pointer k is the first index of ?-part
        */
        for (Int32 k = less; k <= great; k++) {
            Int64 ak = (*a)[k];
            if (ak == pivot1) {
                continue;
            }
            if (ak < pivot1) { // Move (*a)[k] to left part
                if (k != less) {
                    (*a)[k] = (*a)[less];
                    (*a)[less] = ak;
                }
                less++;
            }
            else { // ((*a)[k] > pivot1) -  Move (*a)[k] to right part
                /*
                 * We know that pivot1 == (*a)[e3] == pivot2. Thus, we know
                 * that great will still be >= k when the following loop
                 * terminates, even though we don't test for it explicitly.
                 * In other words, (*a)[e3] acts as a sentinel for great.
                 */
                while ((*a)[great] > pivot1) {
                    great--;
                }
                if ((*a)[great] < pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = (*a)[great];
                    (*a)[great--] = ak;
                }
                else { // (*a)[great] == pivot1
                    (*a)[k] = pivot1;
                    (*a)[great--] = ak;
                }
            }
        }
    }

    // Swap pivots into their final positions
    (*a)[left]  = (*a)[less  - 1]; (*a)[less  - 1] = pivot1;
    (*a)[right] = (*a)[great + 1]; (*a)[great + 1] = pivot2;

    // Sort left and right parts recursively, excluding known pivot values
    DoSort(a, left,   less - 2);
    DoSort(a, great + 2, right);

    /*
     * If pivot1 == pivot2, all elements from center
     * part are equal and, therefore, already sorted
     */
    if (!pivotsDiffer) {
        return NOERROR;
    }

    /*
     * If center part is too large (comprises > 2/3 of the array),
     * swap internal pivot values to ends
     */
    if (less < e1 && great > e5) {
        while ((*a)[less] == pivot1) {
            less++;
        }
        while ((*a)[great] == pivot2) {
            great--;
        }

        /*
         * Partitioning:
         *
         *   left part       center part                   right part
         * +----------------------------------------------------------+
         * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
         * +----------------------------------------------------------+
         *              ^                        ^       ^
         *              |                        |       |
         *             less                      k     great
         *
         * Invariants:
         *
         *              all in (*, less)  == pivot1
         *     pivot1 < all in [less, k)   < pivot2
         *              all in (great, *) == pivot2
         *
         * Pointer k is the first index of ?-part
         */
        for (Int32 k = less; k <= great; k++) {
            Int64 ak = (*a)[k];
            if (ak == pivot2) { // Move (*a)[k] to right part
                while ((*a)[great] == pivot2) {
                    if (great-- == k) {
                        goto outer2;
                    }
                }
                if ((*a)[great] == pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = pivot1;
                }
                else { // pivot1 < (*a)[great] < pivot2
                    (*a)[k] = (*a)[great];
                }
                (*a)[great--] = pivot2;
            }
            else if (ak == pivot1) { // Move (*a)[k] to left part
                (*a)[k] = (*a)[less];
                (*a)[less++] = pivot1;
            }
        }
    outer2: { /* nothing */ }
    }

    // Sort center part recursively, excluding known pivot values
    DoSort(a, less, great);
    return NOERROR;
}

ECode DualPivotQuicksort::Sort(
    /* [in] */ ArrayOf<Int16>* a)
{
    return DoSort(a, 0, a->GetLength() - 1);
}

ECode DualPivotQuicksort::Sort(
    /* [in] */ ArrayOf<Int16>* a,
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int32 toIndex)
{
    FAIL_RETURN(Arrays::CheckStartAndEnd(a->GetLength(), fromIndex, toIndex));
    return DoSort(a, fromIndex, toIndex - 1);
}

ECode DualPivotQuicksort::DoSort(
    /* [in] */ ArrayOf<Int16>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    // Use insertion sort on tiny arrays
    if (right - left + 1 < INSERTION_SORT_THRESHOLD) {
        for (Int32 i = left + 1; i <= right; i++) {
            Int16 ai = (*a)[i];
            Int32 j;
            for (j = i - 1; j >= left && ai < (*a)[j]; j--) {
                (*a)[j + 1] = (*a)[j];
            }
            (*a)[j + 1] = ai;
        }
    }
    else if (right-left+1 > COUNTING_SORT_THRESHOLD_FOR_SHORT_OR_CHAR) {
        // Use counting sort on huge arrays
        Int32 count[NUM_SHORT_VALUES] = {0};
        for (Int32 i = left; i <= right; i++) {
            count[(*a)[i] - Elastos::Core::Math::INT16_MIN_VALUE]++;
        }
        for (Int32 i = 0, k = left; i < NUM_SHORT_VALUES && k <= right; i++) {
            Int16 value = (Int16) (i + Elastos::Core::Math::INT16_MIN_VALUE);

            for (Int32 s = count[i]; s > 0; s--) {
                (*a)[k++] = value;
           }
        }
    }
    else { // Use Dual-Pivot Quicksort on large arrays
        return _DualPivotQuicksort(a, left, right);
    }
    return NOERROR;
}

ECode DualPivotQuicksort::_DualPivotQuicksort(
    /* [in] */ ArrayOf<Int16>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    // Compute indices of five evenly spaced elements
    Int32 sixth = (right - left + 1) / 6;
    Int32 e1 = left  + sixth;
    Int32 e5 = right - sixth;
    Int32 e3 = (UInt32)(left + right) >> 1; // The midpoint
    Int32 e4 = e3 + sixth;
    Int32 e2 = e3 - sixth;

    // Sort these elements using a 5-element sorting network
    Int16 ae1 = (*a)[e1], ae2 = (*a)[e2], ae3 = (*a)[e3], ae4 = (*a)[e4], ae5 = (*a)[e5];

    if (ae1 > ae2) { Int16 t = ae1; ae1 = ae2; ae2 = t; }
    if (ae4 > ae5) { Int16 t = ae4; ae4 = ae5; ae5 = t; }
    if (ae1 > ae3) { Int16 t = ae1; ae1 = ae3; ae3 = t; }
    if (ae2 > ae3) { Int16 t = ae2; ae2 = ae3; ae3 = t; }
    if (ae1 > ae4) { Int16 t = ae1; ae1 = ae4; ae4 = t; }
    if (ae3 > ae4) { Int16 t = ae3; ae3 = ae4; ae4 = t; }
    if (ae2 > ae5) { Int16 t = ae2; ae2 = ae5; ae5 = t; }
    if (ae2 > ae3) { Int16 t = ae2; ae2 = ae3; ae3 = t; }
    if (ae4 > ae5) { Int16 t = ae4; ae4 = ae5; ae5 = t; }

    (*a)[e1] = ae1; (*a)[e3] = ae3; (*a)[e5] = ae5;

    /*
     * Use the second and fourth of the five sorted elements as pivots.
     * These values are inexpensive approximations of the first and
     * second terciles of the array. Note that pivot1 <= pivot2.
     *
     * The pivots are stored in local variables, and the first and
     * the last of the elements to be sorted are moved to the locations
     * formerly occupied by the pivots. When partitioning is complete,
     * the pivots are swapped back into their final positions, and
     * excluded from subsequent sorting.
     */
    Int16 pivot1 = ae2; (*a)[e2] = (*a)[left];
    Int16 pivot2 = ae4; (*a)[e4] = (*a)[right];

    // Pointers
    Int32 less  = left  + 1; // The index of first element of center part
    Int32 great = right - 1; // The index before first element of right part

    Boolean pivotsDiffer = (pivot1 != pivot2);

    if (pivotsDiffer) {
        /*
         * Partitioning:
         *
         *   left part         center part                    right part
         * +------------------------------------------------------------+
         * | < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2 |
         * +------------------------------------------------------------+
         *              ^                          ^       ^
         *              |                          |       |
         *             less                        k     great
         *
         * Invariants:
         *
         *              all in (left, less)   < pivot1
         *    pivot1 <= all in [less, k)     <= pivot2
         *              all in (great, right) > pivot2
         *
         * Pointer k is the first index of ?-part
         */
        for (Int32 k = less; k <= great; k++) {
            Int16 ak = (*a)[k];
            if (ak < pivot1) { // Move (*a)[k] to left part
                if (k != less) {
                    (*a)[k] = (*a)[less];
                    (*a)[less] = ak;
                }
                less++;
            }
            else if (ak > pivot2) { // Move (*a)[k] to right part
                while ((*a)[great] > pivot2) {
                    if (great-- == k) {
                        goto outer;
                    }
                }
                if ((*a)[great] < pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = (*a)[great];
                    (*a)[great--] = ak;
                }
                else { // pivot1 <= (*a)[great] <= pivot2
                    (*a)[k] = (*a)[great];
                    (*a)[great--] = ak;
                }
            }
        }
    outer: { /* nothing */ }
    }
    else { // Pivots are equal
        /*
         * Partition degenerates to the traditional 3-way,
         * or "Dutch National Flag", partition:
         *
         *   left part   center part            right part
         * +----------------------------------------------+
         * |  < pivot  |  == pivot  |    ?    |  > pivot  |
         * +----------------------------------------------+
         *              ^            ^       ^
         *              |            |       |
         *             less          k     great
         *
         * Invariants:
         *
         *   all in (left, less)   < pivot
         *   all in [less, k)     == pivot
         *   all in (great, right) > pivot
         *
         * Pointer k is the first index of ?-part
        */
        for (Int32 k = less; k <= great; k++) {
            Int16 ak = (*a)[k];
            if (ak == pivot1) {
                continue;
            }
            if (ak < pivot1) { // Move (*a)[k] to left part
                if (k != less) {
                    (*a)[k] = (*a)[less];
                    (*a)[less] = ak;
                }
                less++;
            }
            else { // ((*a)[k] > pivot1) -  Move (*a)[k] to right part
                /*
                 * We know that pivot1 == (*a)[e3] == pivot2. Thus, we know
                 * that great will still be >= k when the following loop
                 * terminates, even though we don't test for it explicitly.
                 * In other words, (*a)[e3] acts as a sentinel for great.
                 */
                while ((*a)[great] > pivot1) {
                    great--;
                }
                if ((*a)[great] < pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = (*a)[great];
                    (*a)[great--] = ak;
                }
                else { // (*a)[great] == pivot1
                    (*a)[k] = pivot1;
                    (*a)[great--] = ak;
                }
            }
        }
    }

    // Swap pivots into their final positions
    (*a)[left]  = (*a)[less  - 1]; (*a)[less  - 1] = pivot1;
    (*a)[right] = (*a)[great + 1]; (*a)[great + 1] = pivot2;

    // Sort left and right parts recursively, excluding known pivot values
    DoSort(a, left,   less - 2);
    DoSort(a, great + 2, right);

    /*
     * If pivot1 == pivot2, all elements from center
     * part are equal and, therefore, already sorted
     */
    if (!pivotsDiffer) {
        return NOERROR;
    }

    /*
     * If center part is too large (comprises > 2/3 of the array),
     * swap internal pivot values to ends
     */
    if (less < e1 && great > e5) {
        while ((*a)[less] == pivot1) {
            less++;
        }
        while ((*a)[great] == pivot2) {
            great--;
        }

        /*
         * Partitioning:
         *
         *   left part       center part                   right part
         * +----------------------------------------------------------+
         * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
         * +----------------------------------------------------------+
         *              ^                        ^       ^
         *              |                        |       |
         *             less                      k     great
         *
         * Invariants:
         *
         *              all in (*, less)  == pivot1
         *     pivot1 < all in [less, k)   < pivot2
         *              all in (great, *) == pivot2
         *
         * Pointer k is the first index of ?-part
         */
        for (Int32 k = less; k <= great; k++) {
            Int16 ak = (*a)[k];
            if (ak == pivot2) { // Move (*a)[k] to right part
                while ((*a)[great] == pivot2) {
                    if (great-- == k) {
                        goto outer2;
                    }
                }
                if ((*a)[great] == pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = pivot1;
                }
                else { // pivot1 < (*a)[great] < pivot2
                    (*a)[k] = (*a)[great];
                }
                (*a)[great--] = pivot2;
            }
            else if (ak == pivot1) { // Move (*a)[k] to left part
                (*a)[k] = (*a)[less];
                (*a)[less++] = pivot1;
            }
        }
    outer2: { /* nothing */ }
    }

    // Sort center part recursively, excluding known pivot values
    DoSort(a, less, great);
    return NOERROR;
}

ECode DualPivotQuicksort::Sort(
    /* [in] */ ArrayOf<Char32>* a)
{
    return DoSort(a, 0, a->GetLength() - 1);
}

ECode DualPivotQuicksort::Sort(
    /* [in] */ ArrayOf<Char32>* a,
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int32 toIndex)
{
    FAIL_RETURN(Arrays::CheckStartAndEnd(a->GetLength(), fromIndex, toIndex));
    return DoSort(a, fromIndex, toIndex - 1);
}

ECode DualPivotQuicksort::DoSort(
    /* [in] */ ArrayOf<Char32>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    // Use insertion sort on tiny arrays
    if (right - left + 1 < INSERTION_SORT_THRESHOLD) {
        for (Int32 i = left + 1; i <= right; i++) {
            Char32 ai = (*a)[i];
            Int32 j;
            for (j = i - 1; j >= left && ai < (*a)[j]; j--) {
                (*a)[j + 1] = (*a)[j];
            }
            (*a)[j + 1] = ai;
        }
    }
    else if (right - left + 1 > COUNTING_SORT_THRESHOLD_FOR_SHORT_OR_CHAR) {
        // Use counting sort on huge arrays
        Int32 count[NUM_CHAR_VALUES] = {0};

        for (Int32 i = left; i <= right; i++) {
            count[(*a)[i]]++;
        }
        for (Int32 i = 0, k = left; i < NUM_CHAR_VALUES && k <= right; i++) {
            for (Int32 s = count[i]; s > 0; s--) {
                (*a)[k++] = (Char32) i;
           }
        }
    }
    else { // Use Dual-Pivot Quicksort on large arrays
        return _DualPivotQuicksort(a, left, right);
    }
    return NOERROR;
}

ECode DualPivotQuicksort::_DualPivotQuicksort(
    /* [in] */ ArrayOf<Char32>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    // Compute indices of five evenly spaced elements
    Int32 sixth = (right - left + 1) / 6;
    Int32 e1 = left  + sixth;
    Int32 e5 = right - sixth;
    Int32 e3 = (UInt32)(left + right) >> 1; // The midpoint
    Int32 e4 = e3 + sixth;
    Int32 e2 = e3 - sixth;

    // Sort these elements using a 5-element sorting network
    Char32 ae1 = (*a)[e1], ae2 = (*a)[e2], ae3 = (*a)[e3], ae4 = (*a)[e4], ae5 = (*a)[e5];

    if (ae1 > ae2) { Char32 t = ae1; ae1 = ae2; ae2 = t; }
    if (ae4 > ae5) { Char32 t = ae4; ae4 = ae5; ae5 = t; }
    if (ae1 > ae3) { Char32 t = ae1; ae1 = ae3; ae3 = t; }
    if (ae2 > ae3) { Char32 t = ae2; ae2 = ae3; ae3 = t; }
    if (ae1 > ae4) { Char32 t = ae1; ae1 = ae4; ae4 = t; }
    if (ae3 > ae4) { Char32 t = ae3; ae3 = ae4; ae4 = t; }
    if (ae2 > ae5) { Char32 t = ae2; ae2 = ae5; ae5 = t; }
    if (ae2 > ae3) { Char32 t = ae2; ae2 = ae3; ae3 = t; }
    if (ae4 > ae5) { Char32 t = ae4; ae4 = ae5; ae5 = t; }

    (*a)[e1] = ae1; (*a)[e3] = ae3; (*a)[e5] = ae5;

    /*
     * Use the second and fourth of the five sorted elements as pivots.
     * These values are inexpensive approximations of the first and
     * second terciles of the array. Note that pivot1 <= pivot2.
     *
     * The pivots are stored in local variables, and the first and
     * the last of the elements to be sorted are moved to the locations
     * formerly occupied by the pivots. When partitioning is complete,
     * the pivots are swapped back into their final positions, and
     * excluded from subsequent sorting.
     */
    Char32 pivot1 = ae2; (*a)[e2] = (*a)[left];
    Char32 pivot2 = ae4; (*a)[e4] = (*a)[right];

    // Pointers
    Int32 less  = left  + 1; // The index of first element of center part
    Int32 great = right - 1; // The index before first element of right part

    Boolean pivotsDiffer = (pivot1 != pivot2);

    if (pivotsDiffer) {
        /*
         * Partitioning:
         *
         *   left part         center part                    right part
         * +------------------------------------------------------------+
         * | < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2 |
         * +------------------------------------------------------------+
         *              ^                          ^       ^
         *              |                          |       |
         *             less                        k     great
         *
         * Invariants:
         *
         *              all in (left, less)   < pivot1
         *    pivot1 <= all in [less, k)     <= pivot2
         *              all in (great, right) > pivot2
         *
         * Pointer k is the first index of ?-part
         */
        for (Int32 k = less; k <= great; k++) {
            Char32 ak = (*a)[k];
            if (ak < pivot1) { // Move (*a)[k] to left part
                if (k != less) {
                    (*a)[k] = (*a)[less];
                    (*a)[less] = ak;
                }
                less++;
            }
            else if (ak > pivot2) { // Move (*a)[k] to right part
                while ((*a)[great] > pivot2) {
                    if (great-- == k) {
                        goto outer;
                    }
                }
                if ((*a)[great] < pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = (*a)[great];
                    (*a)[great--] = ak;
                }
                else { // pivot1 <= (*a)[great] <= pivot2
                    (*a)[k] = (*a)[great];
                    (*a)[great--] = ak;
                }
            }
        }
    outer: { /* nothing */ }
    }
    else { // Pivots are equal
        /*
         * Partition degenerates to the traditional 3-way,
         * or "Dutch National Flag", partition:
         *
         *   left part   center part            right part
         * +----------------------------------------------+
         * |  < pivot  |  == pivot  |    ?    |  > pivot  |
         * +----------------------------------------------+
         *              ^            ^       ^
         *              |            |       |
         *             less          k     great
         *
         * Invariants:
         *
         *   all in (left, less)   < pivot
         *   all in [less, k)     == pivot
         *   all in (great, right) > pivot
         *
         * Pointer k is the first index of ?-part
        */
        for (Int32 k = less; k <= great; k++) {
            Char32 ak = (*a)[k];
            if (ak == pivot1) {
                continue;
            }
            if (ak < pivot1) { // Move (*a)[k] to left part
                if (k != less) {
                    (*a)[k] = (*a)[less];
                    (*a)[less] = ak;
                }
                less++;
            }
            else { // ((*a)[k] > pivot1) -  Move (*a)[k] to right part
                /*
                 * We know that pivot1 == (*a)[e3] == pivot2. Thus, we know
                 * that great will still be >= k when the following loop
                 * terminates, even though we don't test for it explicitly.
                 * In other words, (*a)[e3] acts as a sentinel for great.
                 */
                while ((*a)[great] > pivot1) {
                    great--;
                }
                if ((*a)[great] < pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = (*a)[great];
                    (*a)[great--] = ak;
                }
                else { // (*a)[great] == pivot1
                    (*a)[k] = pivot1;
                    (*a)[great--] = ak;
                }
            }
        }
    }

    // Swap pivots into their final positions
    (*a)[left]  = (*a)[less  - 1]; (*a)[less  - 1] = pivot1;
    (*a)[right] = (*a)[great + 1]; (*a)[great + 1] = pivot2;

    // Sort left and right parts recursively, excluding known pivot values
    DoSort(a, left,   less - 2);
    DoSort(a, great + 2, right);

    /*
     * If pivot1 == pivot2, all elements from center
     * part are equal and, therefore, already sorted
     */
    if (!pivotsDiffer) {
        return NOERROR;
    }

    /*
     * If center part is too large (comprises > 2/3 of the array),
     * swap internal pivot values to ends
     */
    if (less < e1 && great > e5) {
        while ((*a)[less] == pivot1) {
            less++;
        }
        while ((*a)[great] == pivot2) {
            great--;
        }

        /*
         * Partitioning:
         *
         *   left part       center part                   right part
         * +----------------------------------------------------------+
         * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
         * +----------------------------------------------------------+
         *              ^                        ^       ^
         *              |                        |       |
         *             less                      k     great
         *
         * Invariants:
         *
         *              all in (*, less)  == pivot1
         *     pivot1 < all in [less, k)   < pivot2
         *              all in (great, *) == pivot2
         *
         * Pointer k is the first index of ?-part
         */
        for (Int32 k = less; k <= great; k++) {
            Char32 ak = (*a)[k];
            if (ak == pivot2) { // Move (*a)[k] to right part
                while ((*a)[great] == pivot2) {
                    if (great-- == k) {
                        goto outer2;
                    }
                }
                if ((*a)[great] == pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = pivot1;
                }
                else { // pivot1 < (*a)[great] < pivot2
                    (*a)[k] = (*a)[great];
                }
                (*a)[great--] = pivot2;
            }
            else if (ak == pivot1) { // Move (*a)[k] to left part
                (*a)[k] = (*a)[less];
                (*a)[less++] = pivot1;
            }
        }
    outer2: { /* nothing */ }
    }

    // Sort center part recursively, excluding known pivot values
    DoSort(a, less, great);
    return NOERROR;
}

ECode DualPivotQuicksort::Sort(
    /* [in] */ ArrayOf<Byte>* a)
{
    return DoSort(a, 0, a->GetLength() - 1);
}

ECode DualPivotQuicksort::Sort(
    /* [in] */ ArrayOf<Byte>* a,
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int32 toIndex)
{
    FAIL_RETURN(Arrays::CheckStartAndEnd(a->GetLength(), fromIndex, toIndex));
    return DoSort(a, fromIndex, toIndex - 1);
}

ECode DualPivotQuicksort::DoSort(
    /* [in] */ ArrayOf<Byte>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    // Use insertion sort on tiny arrays
    if (right - left + 1 < INSERTION_SORT_THRESHOLD) {
        for (Int32 i = left + 1; i <= right; i++) {
            Byte ai = (*a)[i];
            Int32 j;
            for (j = i - 1; j >= left && ai < (*a)[j]; j--) {
                (*a)[j + 1] = (*a)[j];
            }
            (*a)[j + 1] = ai;
        }
    }
    else if (right - left + 1 > COUNTING_SORT_THRESHOLD_FOR_BYTE) {
        // Use counting sort on huge arrays
        Int32 count[NUM_BYTE_VALUES] = {0};
        for (Int32 i = left; i <= right; i++) {
            count[(*a)[i] - Elastos::Core::Math::BYTE_MIN_VALUE]++;
        }
        for (Int32 i = 0, k = left; i < NUM_BYTE_VALUES && k <= right; i++) {
            Byte value = (Byte) (i + Elastos::Core::Math::BYTE_MIN_VALUE);

            for (Int32 s = count[i]; s > 0; s--) {
                (*a)[k++] = value;
           }
        }
    } else { // Use Dual-Pivot Quicksort on large arrays
        return _DualPivotQuicksort(a, left, right);
    }
    return NOERROR;
}

ECode DualPivotQuicksort::_DualPivotQuicksort(
    /* [in] */ ArrayOf<Byte>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    // Compute indices of five evenly spaced elements
    Int32 sixth = (right - left + 1) / 6;
    Int32 e1 = left  + sixth;
    Int32 e5 = right - sixth;
    Int32 e3 = (UInt32)(left + right) >> 1; // The midpoint
    Int32 e4 = e3 + sixth;
    Int32 e2 = e3 - sixth;

    // Sort these elements using a 5-element sorting network
    Byte ae1 = (*a)[e1], ae2 = (*a)[e2], ae3 = (*a)[e3], ae4 = (*a)[e4], ae5 = (*a)[e5];

    if (ae1 > ae2) { Byte t = ae1; ae1 = ae2; ae2 = t; }
    if (ae4 > ae5) { Byte t = ae4; ae4 = ae5; ae5 = t; }
    if (ae1 > ae3) { Byte t = ae1; ae1 = ae3; ae3 = t; }
    if (ae2 > ae3) { Byte t = ae2; ae2 = ae3; ae3 = t; }
    if (ae1 > ae4) { Byte t = ae1; ae1 = ae4; ae4 = t; }
    if (ae3 > ae4) { Byte t = ae3; ae3 = ae4; ae4 = t; }
    if (ae2 > ae5) { Byte t = ae2; ae2 = ae5; ae5 = t; }
    if (ae2 > ae3) { Byte t = ae2; ae2 = ae3; ae3 = t; }
    if (ae4 > ae5) { Byte t = ae4; ae4 = ae5; ae5 = t; }

    (*a)[e1] = ae1; (*a)[e3] = ae3; (*a)[e5] = ae5;

    /*
     * Use the second and fourth of the five sorted elements as pivots.
     * These values are inexpensive approximations of the first and
     * second terciles of the array. Note that pivot1 <= pivot2.
     *
     * The pivots are stored in local variables, and the first and
     * the last of the elements to be sorted are moved to the locations
     * formerly occupied by the pivots. When partitioning is complete,
     * the pivots are swapped back into their final positions, and
     * excluded from subsequent sorting.
     */
    Byte pivot1 = ae2; (*a)[e2] = (*a)[left];
    Byte pivot2 = ae4; (*a)[e4] = (*a)[right];

    // Pointers
    Int32 less  = left  + 1; // The index of first element of center part
    Int32 great = right - 1; // The index before first element of right part

    Boolean pivotsDiffer = (pivot1 != pivot2);

    if (pivotsDiffer) {
        /*
         * Partitioning:
         *
         *   left part         center part                    right part
         * +------------------------------------------------------------+
         * | < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2 |
         * +------------------------------------------------------------+
         *              ^                          ^       ^
         *              |                          |       |
         *             less                        k     great
         *
         * Invariants:
         *
         *              all in (left, less)   < pivot1
         *    pivot1 <= all in [less, k)     <= pivot2
         *              all in (great, right) > pivot2
         *
         * Pointer k is the first index of ?-part
         */
        for (Int32 k = less; k <= great; k++) {
            Byte ak = (*a)[k];
            if (ak < pivot1) { // Move (*a)[k] to left part
                if (k != less) {
                    (*a)[k] = (*a)[less];
                    (*a)[less] = ak;
                }
                less++;
            }
            else if (ak > pivot2) { // Move (*a)[k] to right part
                while ((*a)[great] > pivot2) {
                    if (great-- == k) {
                        goto outer;
                    }
                }
                if ((*a)[great] < pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = (*a)[great];
                    (*a)[great--] = ak;
                }
                else { // pivot1 <= (*a)[great] <= pivot2
                    (*a)[k] = (*a)[great];
                    (*a)[great--] = ak;
                }
            }
        }
    outer: { /* nothing */ }
    }
    else { // Pivots are equal
        /*
         * Partition degenerates to the traditional 3-way,
         * or "Dutch National Flag", partition:
         *
         *   left part   center part            right part
         * +----------------------------------------------+
         * |  < pivot  |  == pivot  |    ?    |  > pivot  |
         * +----------------------------------------------+
         *              ^            ^       ^
         *              |            |       |
         *             less          k     great
         *
         * Invariants:
         *
         *   all in (left, less)   < pivot
         *   all in [less, k)     == pivot
         *   all in (great, right) > pivot
         *
         * Pointer k is the first index of ?-part
        */
        for (Int32 k = less; k <= great; k++) {
            Byte ak = (*a)[k];
            if (ak == pivot1) {
                continue;
            }
            if (ak < pivot1) { // Move (*a)[k] to left part
                if (k != less) {
                    (*a)[k] = (*a)[less];
                    (*a)[less] = ak;
                }
                less++;
            }
            else { // ((*a)[k] > pivot1) -  Move (*a)[k] to right part
                /*
                 * We know that pivot1 == (*a)[e3] == pivot2. Thus, we know
                 * that great will still be >= k when the following loop
                 * terminates, even though we don't test for it explicitly.
                 * In other words, (*a)[e3] acts as a sentinel for great.
                 */
                while ((*a)[great] > pivot1) {
                    great--;
                }
                if ((*a)[great] < pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = (*a)[great];
                    (*a)[great--] = ak;
                }
                else { // (*a)[great] == pivot1
                    (*a)[k] = pivot1;
                    (*a)[great--] = ak;
                }
            }
        }
    }

    // Swap pivots into their final positions
    (*a)[left]  = (*a)[less  - 1]; (*a)[less  - 1] = pivot1;
    (*a)[right] = (*a)[great + 1]; (*a)[great + 1] = pivot2;

    // Sort left and right parts recursively, excluding known pivot values
    DoSort(a, left,   less - 2);
    DoSort(a, great + 2, right);

    /*
     * If pivot1 == pivot2, all elements from center
     * part are equal and, therefore, already sorted
     */
    if (!pivotsDiffer) {
        return NOERROR;
    }

    /*
     * If center part is too large (comprises > 2/3 of the array),
     * swap internal pivot values to ends
     */
    if (less < e1 && great > e5) {
        while ((*a)[less] == pivot1) {
            less++;
        }
        while ((*a)[great] == pivot2) {
            great--;
        }

        /*
         * Partitioning:
         *
         *   left part       center part                   right part
         * +----------------------------------------------------------+
         * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
         * +----------------------------------------------------------+
         *              ^                        ^       ^
         *              |                        |       |
         *             less                      k     great
         *
         * Invariants:
         *
         *              all in (*, less)  == pivot1
         *     pivot1 < all in [less, k)   < pivot2
         *              all in (great, *) == pivot2
         *
         * Pointer k is the first index of ?-part
         */
        for (Int32 k = less; k <= great; k++) {
            Byte ak = (*a)[k];
            if (ak == pivot2) { // Move (*a)[k] to right part
                while ((*a)[great] == pivot2) {
                    if (great-- == k) {
                        goto outer2;
                    }
                }
                if ((*a)[great] == pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = pivot1;
                }
                else { // pivot1 < (*a)[great] < pivot2
                    (*a)[k] = (*a)[great];
                }
                (*a)[great--] = pivot2;
            }
            else if (ak == pivot1) { // Move (*a)[k] to left part
                (*a)[k] = (*a)[less];
                (*a)[less++] = pivot1;
            }
        }
    outer2: { /* nothing */ }
    }

    // Sort center part recursively, excluding known pivot values
    DoSort(a, less, great);
    return NOERROR;
}

ECode DualPivotQuicksort::Sort(
    /* [in] */ ArrayOf<Float>* a)
{
    return SortNegZeroAndNaN(a, 0, a->GetLength() - 1);
}

ECode DualPivotQuicksort::Sort(
    /* [in] */ ArrayOf<Float>* a,
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int32 toIndex)
{
    FAIL_RETURN(Arrays::CheckStartAndEnd(a->GetLength(), fromIndex, toIndex));
    return SortNegZeroAndNaN(a, fromIndex, toIndex - 1);
}

ECode DualPivotQuicksort::SortNegZeroAndNaN(
    /* [in] */ ArrayOf<Float>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    /*
     * Phase 1: Count negative zeros and move NaNs to end of array
     */
    const Int32 NEGATIVE_ZERO = Elastos::Core::Math::FloatToInt32Bits(-0.0f);
    Int32 numNegativeZeros = 0;
    Int32 n = right;

    for (Int32 k = left; k <= n; k++) {
        Float ak = (*a)[k];
        if (ak == 0.0f && NEGATIVE_ZERO == Elastos::Core::Math::FloatToRawInt32Bits(ak)) {
            (*a)[k] = 0.0f;
            numNegativeZeros++;
        }
        else if (ak != ak) { // i.e., ak is NaN
            (*a)[k--] = (*a)[n];
            (*a)[n--] = Elastos::Core::Math::FLOAT_NAN;
        }
    }

    /*
     * Phase 2: Sort everything except NaNs (which are already in place)
    */
    DoSort(a, left, n);

    /*
     * Phase 3: Turn positive zeros back into negative zeros as appropriate
     */
    if (numNegativeZeros == 0) {
        return NOERROR;
    }

    // Find first zero element
    Int32 zeroIndex = FindAnyZero(a, left, n);

    for (Int32 i = zeroIndex - 1; i >= left && (*a)[i] == 0.0f; i--) {
        zeroIndex = i;
    }

    // Turn the right number of positive zeros back into negative zeros
    for (Int32 i = zeroIndex, m = zeroIndex + numNegativeZeros; i < m; i++) {
        (*a)[i] = -0.0f;
    }
    return NOERROR;
}

Int32 DualPivotQuicksort::FindAnyZero(
    /* [in] */ ArrayOf<Float>* a,
    /* [in] */ Int32 low,
    /* [in] */ Int32 high)
{
    while (TRUE) {
        Int32 middle = (UInt32)(low + high) >> 1;
        Float middleValue = (*a)[middle];

        if (middleValue < 0.0f) {
            low = middle + 1;
        }
        else if (middleValue > 0.0f) {
            high = middle - 1;
        }
        else { // middleValue == 0.0f
            return middle;
        }
    }
}

ECode DualPivotQuicksort::DoSort(
    /* [in] */ ArrayOf<Float>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    // Use insertion sort on tiny arrays
    if (right - left + 1 < INSERTION_SORT_THRESHOLD) {
        for (Int32 i = left + 1; i <= right; i++) {
            Float ai = (*a)[i];
            Int32 j;
            for (j = i - 1; j >= left && ai < (*a)[j]; j--) {
                (*a)[j + 1] = (*a)[j];
            }
            (*a)[j + 1] = ai;
        }
    }
    else { // Use Dual-Pivot Quicksort on large arrays
        return _DualPivotQuicksort(a, left, right);
    }
    return NOERROR;
}

ECode DualPivotQuicksort::_DualPivotQuicksort(
    /* [in] */ ArrayOf<Float>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    // Compute indices of five evenly spaced elements
    Int32 sixth = (right - left + 1) / 6;
    Int32 e1 = left  + sixth;
    Int32 e5 = right - sixth;
    Int32 e3 = (UInt32)(left + right) >> 1; // The midpoint
    Int32 e4 = e3 + sixth;
    Int32 e2 = e3 - sixth;

    // Sort these elements using a 5-element sorting network
    Float ae1 = (*a)[e1], ae2 = (*a)[e2], ae3 = (*a)[e3], ae4 = (*a)[e4], ae5 = (*a)[e5];

    if (ae1 > ae2) { Float t = ae1; ae1 = ae2; ae2 = t; }
    if (ae4 > ae5) { Float t = ae4; ae4 = ae5; ae5 = t; }
    if (ae1 > ae3) { Float t = ae1; ae1 = ae3; ae3 = t; }
    if (ae2 > ae3) { Float t = ae2; ae2 = ae3; ae3 = t; }
    if (ae1 > ae4) { Float t = ae1; ae1 = ae4; ae4 = t; }
    if (ae3 > ae4) { Float t = ae3; ae3 = ae4; ae4 = t; }
    if (ae2 > ae5) { Float t = ae2; ae2 = ae5; ae5 = t; }
    if (ae2 > ae3) { Float t = ae2; ae2 = ae3; ae3 = t; }
    if (ae4 > ae5) { Float t = ae4; ae4 = ae5; ae5 = t; }

    (*a)[e1] = ae1; (*a)[e3] = ae3; (*a)[e5] = ae5;

    /*
     * Use the second and fourth of the five sorted elements as pivots.
     * These values are inexpensive approximations of the first and
     * second terciles of the array. Note that pivot1 <= pivot2.
     *
     * The pivots are stored in local variables, and the first and
     * the last of the elements to be sorted are moved to the locations
     * formerly occupied by the pivots. When partitioning is complete,
     * the pivots are swapped back into their final positions, and
     * excluded from subsequent sorting.
     */
    Float pivot1 = ae2; (*a)[e2] = (*a)[left];
    Float pivot2 = ae4; (*a)[e4] = (*a)[right];

    // Pointers
    Int32 less  = left  + 1; // The index of first element of center part
    Int32 great = right - 1; // The index before first element of right part

    Boolean pivotsDiffer = (pivot1 != pivot2);

    if (pivotsDiffer) {
        /*
         * Partitioning:
         *
         *   left part         center part                    right part
         * +------------------------------------------------------------+
         * | < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2 |
         * +------------------------------------------------------------+
         *              ^                          ^       ^
         *              |                          |       |
         *             less                        k     great
         *
         * Invariants:
         *
         *              all in (left, less)   < pivot1
         *    pivot1 <= all in [less, k)     <= pivot2
         *              all in (great, right) > pivot2
         *
         * Pointer k is the first index of ?-part
         */
        for (Int32 k = less; k <= great; k++) {
            Float ak = (*a)[k];
            if (ak < pivot1) { // Move (*a)[k] to left part
                if (k != less) {
                    (*a)[k] = (*a)[less];
                    (*a)[less] = ak;
                }
                less++;
            }
            else if (ak > pivot2) { // Move (*a)[k] to right part
                while ((*a)[great] > pivot2) {
                    if (great-- == k) {
                        goto outer2;
                    }
                }
                if ((*a)[great] < pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = (*a)[great];
                    (*a)[great--] = ak;
                }
                else { // pivot1 <= (*a)[great] <= pivot2
                    (*a)[k] = (*a)[great];
                    (*a)[great--] = ak;
                }
            }
        }
    outer2: { /* nothing */ }
    }
    else { // Pivots are equal
        /*
         * Partition degenerates to the traditional 3-way,
         * or "Dutch National Flag", partition:
         *
         *   left part   center part            right part
         * +----------------------------------------------+
         * |  < pivot  |  == pivot  |    ?    |  > pivot  |
         * +----------------------------------------------+
         *              ^            ^       ^
         *              |            |       |
         *             less          k     great
         *
         * Invariants:
         *
         *   all in (left, less)   < pivot
         *   all in [less, k)     == pivot
         *   all in (great, right) > pivot
         *
         * Pointer k is the first index of ?-part
         */
        for (Int32 k = less; k <= great; k++) {
            Float ak = (*a)[k];
            if (ak == pivot1) {
                continue;
            }
            if (ak < pivot1) { // Move (*a)[k] to left part
                if (k != less) {
                    (*a)[k] = (*a)[less];
                    (*a)[less] = ak;
                }
                less++;
            }
            else { // ((*a)[k] > pivot1) -  Move (*a)[k] to right part
                /*
                 * We know that pivot1 == (*a)[e3] == pivot2. Thus, we know
                 * that great will still be >= k when the following loop
                 * terminates, even though we don't test for it explicitly.
                 * In other words, (*a)[e3] acts as a sentinel for great.
                 */
                while ((*a)[great] > pivot1) {
                    great--;
                }
                if ((*a)[great] < pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = (*a)[great];
                    (*a)[great--] = ak;
                } else { // (*a)[great] == pivot1
                    (*a)[k] = pivot1;
                    (*a)[great--] = ak;
                }
            }
        }
    }

    // Swap pivots into their final positions
    (*a)[left]  = (*a)[less  - 1]; (*a)[less  - 1] = pivot1;
    (*a)[right] = (*a)[great + 1]; (*a)[great + 1] = pivot2;

    // Sort left and right parts recursively, excluding known pivot values
    DoSort(a, left,   less - 2);
    DoSort(a, great + 2, right);

    /*
     * If pivot1 == pivot2, all elements from center
     * part are equal and, therefore, already sorted
     */
    if (!pivotsDiffer) {
        return NOERROR;
    }

    /*
     * If center part is too large (comprises > 2/3 of the array),
     * swap internal pivot values to ends
     */
    if (less < e1 && great > e5) {
        while ((*a)[less] == pivot1) {
            less++;
        }
        while ((*a)[great] == pivot2) {
            great--;
        }

        /*
         * Partitioning:
         *
         *   left part       center part                   right part
         * +----------------------------------------------------------+
         * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
         * +----------------------------------------------------------+
         *              ^                        ^       ^
         *              |                        |       |
         *             less                      k     great
         *
         * Invariants:
         *
         *              all in (*, less)  == pivot1
         *     pivot1 < all in [less, k)   < pivot2
         *              all in (great, *) == pivot2
         *
         * Pointer k is the first index of ?-part
         */
        for (Int32 k = less; k <= great; k++) {
            Float ak = (*a)[k];
            if (ak == pivot2) { // Move (*a)[k] to right part
                while ((*a)[great] == pivot2) {
                    if (great-- == k) {
                        goto outer;
                    }
                }
                if ((*a)[great] == pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = pivot1;
                }
                else { // pivot1 < (*a)[great] < pivot2
                    (*a)[k] = (*a)[great];
                }
                (*a)[great--] = pivot2;
            }
            else if (ak == pivot1) { // Move (*a)[k] to left part
                (*a)[k] = (*a)[less];
                (*a)[less++] = pivot1;
            }
        }
    outer: { /* nothing */ }
    }

    // Sort center part recursively, excluding known pivot values
    DoSort(a, less, great);
    return NOERROR;
}

ECode DualPivotQuicksort::Sort(
    /* [in] */ ArrayOf<Double>* a)
{
    return SortNegZeroAndNaN(a, 0, a->GetLength() - 1);;
}

ECode DualPivotQuicksort::Sort(
    /* [in] */ ArrayOf<Double>* a,
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int32 toIndex)
{
    FAIL_RETURN(Arrays::CheckStartAndEnd(a->GetLength(), fromIndex, toIndex));
    return SortNegZeroAndNaN(a, fromIndex, toIndex - 1);
}

ECode DualPivotQuicksort::SortNegZeroAndNaN(
    /* [in] */ ArrayOf<Double>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    /*
     * Phase 1: Count negative zeros and move NaNs to end of array
     */
    const Int64 NEGATIVE_ZERO = Elastos::Core::Math::DoubleToInt64Bits(-0.0l);
    Int32 numNegativeZeros = 0;
    Int32 n = right;

    for (Int32 k = left; k <= n; k++) {
        Double ak = (*a)[k];
        if (ak == 0.0l && NEGATIVE_ZERO == Elastos::Core::Math::DoubleToRawInt64Bits(ak)) {
            (*a)[k] = 0.0l;
            numNegativeZeros++;
        }
        else if (ak != ak) { // i.e., ak is NaN
            (*a)[k--] = (*a)[n];
            (*a)[n--] = Elastos::Core::Math::DOUBLE_NAN;// Double.NaN;
        }
    }

    /*
     * Phase 2: Sort everything except NaNs (which are already in place)
    */
    DoSort(a, left, n);

    /*
     * Phase 3: Turn positive zeros back into negative zeros as appropriate
     */
    if (numNegativeZeros == 0) {
        return NOERROR;
    }

    // Find first zero element
    Int32 zeroIndex = FindAnyZero(a, left, n);

    for (Int32 i = zeroIndex - 1; i >= left && (*a)[i] == 0.0l; i--) {
        zeroIndex = i;
    }

    // Turn the right number of positive zeros back into negative zeros
    for (Int32 i = zeroIndex, m = zeroIndex + numNegativeZeros; i < m; i++) {
        (*a)[i] = -0.0l;
    }
    return NOERROR;
}

Int32 DualPivotQuicksort::FindAnyZero(
    /* [in] */ ArrayOf<Double>* a,
    /* [in] */ Int32 low,
    /* [in] */ Int32 high)
{
    while (TRUE) {
        Int32 middle = (UInt32)(low + high) >> 1;
        Double middleValue = (*a)[middle];

        if (middleValue < 0.0l) {
            low = middle + 1;
        }
        else if (middleValue > 0.0l) {
            high = middle - 1;
        }
        else { // middleValue == 0.0l
            return middle;
        }
    }
}

ECode DualPivotQuicksort::DoSort(
    /* [in] */ ArrayOf<Double>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    // Use insertion sort on tiny arrays
    if (right - left + 1 < INSERTION_SORT_THRESHOLD) {
        for (Int32 i = left + 1; i <= right; i++) {
            Double ai = (*a)[i];
            Int32 j;
            for (j = i - 1; j >= left && ai < (*a)[j]; j--) {
                (*a)[j + 1] = (*a)[j];
            }
            (*a)[j + 1] = ai;
        }
    }
    else { // Use Dual-Pivot Quicksort on large arrays
        return _DualPivotQuicksort(a, left, right);
    }
    return NOERROR;
}

ECode DualPivotQuicksort::_DualPivotQuicksort(
    /* [in] */ ArrayOf<Double>* a,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    // Compute indices of five evenly spaced elements
    Int32 sixth = (right - left + 1) / 6;
    Int32 e1 = left  + sixth;
    Int32 e5 = right - sixth;
    Int32 e3 = (UInt32)(left + right) >> 1; // The midpoint
    Int32 e4 = e3 + sixth;
    Int32 e2 = e3 - sixth;

    // Sort these elements using a 5-element sorting network
    Double ae1 = (*a)[e1], ae2 = (*a)[e2], ae3 = (*a)[e3], ae4 = (*a)[e4], ae5 = (*a)[e5];

    if (ae1 > ae2) { Double t = ae1; ae1 = ae2; ae2 = t; }
    if (ae4 > ae5) { Double t = ae4; ae4 = ae5; ae5 = t; }
    if (ae1 > ae3) { Double t = ae1; ae1 = ae3; ae3 = t; }
    if (ae2 > ae3) { Double t = ae2; ae2 = ae3; ae3 = t; }
    if (ae1 > ae4) { Double t = ae1; ae1 = ae4; ae4 = t; }
    if (ae3 > ae4) { Double t = ae3; ae3 = ae4; ae4 = t; }
    if (ae2 > ae5) { Double t = ae2; ae2 = ae5; ae5 = t; }
    if (ae2 > ae3) { Double t = ae2; ae2 = ae3; ae3 = t; }
    if (ae4 > ae5) { Double t = ae4; ae4 = ae5; ae5 = t; }

    (*a)[e1] = ae1; (*a)[e3] = ae3; (*a)[e5] = ae5;

    /*
     * Use the second and fourth of the five sorted elements as pivots.
     * These values are inexpensive approximations of the first and
     * second terciles of the array. Note that pivot1 <= pivot2.
     *
     * The pivots are stored in local variables, and the first and
     * the last of the elements to be sorted are moved to the locations
     * formerly occupied by the pivots. When partitioning is complete,
     * the pivots are swapped back into their final positions, and
     * excluded from subsequent sorting.
     */
    Double pivot1 = ae2; (*a)[e2] = (*a)[left];
    Double pivot2 = ae4; (*a)[e4] = (*a)[right];

    // Pointers
    Int32 less  = left  + 1; // The index of first element of center part
    Int32 great = right - 1; // The index before first element of right part

    Boolean pivotsDiffer = (pivot1 != pivot2);

    if (pivotsDiffer) {
        /*
         * Partitioning:
         *
         *   left part         center part                    right part
         * +------------------------------------------------------------+
         * | < pivot1  |  pivot1 <= && <= pivot2  |    ?    |  > pivot2 |
         * +------------------------------------------------------------+
         *              ^                          ^       ^
         *              |                          |       |
         *             less                        k     great
         *
         * Invariants:
         *
         *              all in (left, less)   < pivot1
         *    pivot1 <= all in [less, k)     <= pivot2
         *              all in (great, right) > pivot2
         *
         * Pointer k is the first index of ?-part
         */
        for (Int32 k = less; k <= great; k++) {
            Double ak = (*a)[k];
            if (ak < pivot1) { // Move (*a)[k] to left part
                if (k != less) {
                    (*a)[k] = (*a)[less];
                    (*a)[less] = ak;
                }
                less++;
            }
            else if (ak > pivot2) { // Move (*a)[k] to right part
                while ((*a)[great] > pivot2) {
                    if (great-- == k) {
                        goto outer;
                    }
                }
                if ((*a)[great] < pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = (*a)[great];
                    (*a)[great--] = ak;
                }
                else { // pivot1 <= (*a)[great] <= pivot2
                    (*a)[k] = (*a)[great];
                    (*a)[great--] = ak;
                }
            }
        }
    outer: { /* nothing */ }
    }
    else { // Pivots are equal
        /*
         * Partition degenerates to the traditional 3-way,
         * or "Dutch National Flag", partition:
         *
         *   left part   center part            right part
         * +----------------------------------------------+
         * |  < pivot  |  == pivot  |    ?    |  > pivot  |
         * +----------------------------------------------+
         *              ^            ^       ^
         *              |            |       |
         *             less          k     great
         *
         * Invariants:
         *
         *   all in (left, less)   < pivot
         *   all in [less, k)     == pivot
         *   all in (great, right) > pivot
         *
         * Pointer k is the first index of ?-part
         */
        for (Int32 k = less; k <= great; k++) {
            Double ak = (*a)[k];
            if (ak == pivot1) {
                continue;
            }
            if (ak < pivot1) { // Move (*a)[k] to left part
                if (k != less) {
                    (*a)[k] = (*a)[less];
                    (*a)[less] = ak;
                }
                less++;
            }
            else { // ((*a)[k] > pivot1) -  Move (*a)[k] to right part
                /*
                 * We know that pivot1 == (*a)[e3] == pivot2. Thus, we know
                 * that great will still be >= k when the following loop
                 * terminates, even though we don't test for it explicitly.
                 * In other words, (*a)[e3] acts as a sentinel for great.
                 */
                while ((*a)[great] > pivot1) {
                    great--;
                }
                if ((*a)[great] < pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = (*a)[great];
                    (*a)[great--] = ak;
                }
                else { // (*a)[great] == pivot1
                    (*a)[k] = pivot1;
                    (*a)[great--] = ak;
                }
            }
        }
    }

    // Swap pivots into their final positions
    (*a)[left]  = (*a)[less  - 1]; (*a)[less  - 1] = pivot1;
    (*a)[right] = (*a)[great + 1]; (*a)[great + 1] = pivot2;

    // Sort left and right parts recursively, excluding known pivot values
    DoSort(a, left,   less - 2);
    DoSort(a, great + 2, right);

    /*
     * If pivot1 == pivot2, all elements from center
     * part are equal and, therefore, already sorted
     */
    if (!pivotsDiffer) {
        return NOERROR;
    }

    /*
     * If center part is too large (comprises > 2/3 of the array),
     * swap internal pivot values to ends
     */
    if (less < e1 && great > e5) {
        while ((*a)[less] == pivot1) {
            less++;
        }
        while ((*a)[great] == pivot2) {
            great--;
        }

        /*
         * Partitioning:
         *
         *   left part       center part                   right part
         * +----------------------------------------------------------+
         * | == pivot1 |  pivot1 < && < pivot2  |    ?    | == pivot2 |
         * +----------------------------------------------------------+
         *              ^                        ^       ^
         *              |                        |       |
         *             less                      k     great
         *
         * Invariants:
         *
         *              all in (*, less)  == pivot1
         *     pivot1 < all in [less, k)   < pivot2
         *              all in (great, *) == pivot2
         *
         * Pointer k is the first index of ?-part
         */
        for (Int32 k = less; k <= great; k++) {
            Double ak = (*a)[k];
            if (ak == pivot2) { // Move (*a)[k] to right part
                while ((*a)[great] == pivot2) {
                    if (great-- == k) {
                        goto outer2;
                    }
                }
                if ((*a)[great] == pivot1) {
                    (*a)[k] = (*a)[less];
                    (*a)[less++] = pivot1;
                }
                else { // pivot1 < (*a)[great] < pivot2
                    (*a)[k] = (*a)[great];
                }
                (*a)[great--] = pivot2;
            }
            else if (ak == pivot1) { // Move (*a)[k] to left part
                (*a)[k] = (*a)[less];
                (*a)[less++] = pivot1;
            }
        }
    outer2: { /* nothing */ }
    }

    // Sort center part recursively, excluding known pivot values
    DoSort(a, less, great);
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
