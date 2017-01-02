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

#ifndef __ELASTOS_IO_BUFFERFACTORY_H__
#define __ELASTOS_IO_BUFFERFACTORY_H__

#include <elastos.h>

namespace Elastos {
namespace IO {

/**
 * Provide factory service of buffer classes.
 * <p>
 * Since all buffer impl classes are package private (except DirectByteBuffer),
 * this factory is the only entrance to access buffer functions from outside of
 * the impl package.
 * </p>
 */
class BufferFactory
{
public:
    /**
     * Returns a new byte buffer based on the specified byte array.
     *
     * @param array
     *            The byte array
     * @return A new byte buffer based on the specified byte array.
     */
    static CARAPI NewByteBuffer(
        /* [in] */ ArrayOf<Byte>* array,
        /* [out] */ IByteBuffer** buf);

    /**
     * Returns a new array based byte buffer with the specified capacity.
     *
     * @param capacity
     *            The capacity of the new buffer
     * @return A new array based byte buffer with the specified capacity.
     */
    static CARAPI NewByteBuffer(
        /* [in] */ Int32 capacity,
        /* [out] */ IByteBuffer** buf);

    /**
     * Returns a new char buffer based on the specified char array.
     *
     * @param array
     *            The char array
     * @return A new char buffer based on the specified char array.
     */
    static CARAPI NewCharBuffer(
        /* [in] */ ArrayOf<Char32>* array,
        /* [out] */ ICharBuffer** buf);

    /**
     * Returns a new readonly char buffer based on the specified char sequence.
     *
     * @param chseq
     *            The char sequence
     * @return A new readonly char buffer based on the specified char sequence.
     */
    static CARAPI NewCharBuffer(
        /* [in] */ ICharSequence* chseq,
        /* [out] */ ICharBuffer** buf);

    /**
     * Returns a new array based char buffer with the specified capacity.
     *
     * @param capacity
     *            The capacity of the new buffer
     * @return A new array based char buffer with the specified capacity.
     */
    static CARAPI NewCharBuffer(
        /* [in] */ Int32 capacity,
        /* [out] */ ICharBuffer** buf);

    /**
     * Returns a new direct byte buffer with the specified capacity.
     *
     * @param capacity
     *            The capacity of the new buffer
     * @return A new direct byte buffer with the specified capacity.
     */
    static CARAPI NewDirectByteBuffer(
        /* [in] */ Int32 capacity,
        /* [out] */ IByteBuffer** buf);

    /**
     * Returns a new double buffer based on the specified double array.
     *
     * @param array
     *            The double array
     * @return A new double buffer based on the specified double array.
     */
    static CARAPI NewDoubleBuffer(
        /* [in] */ ArrayOf<Double>* array,
        /* [out] */ IDoubleBuffer** buf);

    /**
     * Returns a new array based double buffer with the specified capacity.
     *
     * @param capacity
     *            The capacity of the new buffer
     * @return A new array based double buffer with the specified capacity.
     */
    static CARAPI NewDoubleBuffer(
        /* [in] */ Int32 capacity,
        /* [out] */ IDoubleBuffer** buf);

    /**
     * Returns a new float buffer based on the specified float array.
     *
     * @param array
     *            The float array
     * @return A new float buffer based on the specified float array.
     */
    static CARAPI NewFloatBuffer(
        /* [in] */ ArrayOf<Float>* array,
        /* [out] */ IFloatBuffer** buf);

    /**
     * Returns a new array based float buffer with the specified capacity.
     *
     * @param capacity
     *            The capacity of the new buffer
     * @return A new array based float buffer with the specified capacity.
     */
    static CARAPI NewFloatBuffer(
        /* [in] */ Int32 capacity,
        /* [out] */ IFloatBuffer** buf);

    /**
     * Returns a new array based int buffer with the specified capacity.
     *
     * @param capacity
     *            The capacity of the new buffer
     * @return A new array based int buffer with the specified capacity.
     */
    static CARAPI NewInt32Buffer(
        /* [in] */ Int32 capacity,
        /* [out] */ IInt32Buffer** buf);

    /**
     * Returns a new int buffer based on the specified int array.
     *
     * @param array
     *            The int array
     * @return A new int buffer based on the specified int array.
     */
    static CARAPI NewInt32Buffer(
        /* [in] */ ArrayOf<Int32>* array,
        /* [out] */ IInt32Buffer** buf);

    /**
     * Returns a new array based long buffer with the specified capacity.
     *
     * @param capacity
     *            The capacity of the new buffer
     * @return A new array based long buffer with the specified capacity.
     */
    static CARAPI NewInt64Buffer(
        /* [in] */ Int32 capacity,
        /* [out] */ IInt64Buffer** buf);

    /**
     * Returns a new long buffer based on the specified long array.
     *
     * @param array
     *            The long array
     * @return A new long buffer based on the specified long array.
     */
    static CARAPI NewInt64Buffer(
        /* [in] */ ArrayOf<Int64>* array,
        /* [out] */ IInt64Buffer** buf);

    /**
     * Returns a new array based short buffer with the specified capacity.
     *
     * @param capacity
     *            The capacity of the new buffer
     * @return A new array based short buffer with the specified capacity.
     */
    static CARAPI NewInt16Buffer(
        /* [in] */ Int32 capacity,
        /* [out] */ IInt16Buffer** buf);

    /**
     * Returns a new short buffer based on the specified short array.
     *
     * @param array
     *            The short array
     * @return A new short buffer based on the specified short array.
     */
    static CARAPI NewInt16Buffer(
        /* [in] */ ArrayOf<Int16>* array,
        /* [out] */ IInt16Buffer** buf);
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_BUFFERFACTORY_H__
