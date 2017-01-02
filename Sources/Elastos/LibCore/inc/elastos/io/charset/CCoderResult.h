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

#ifndef __ELASTOS_IO_CHARSET_CCODERRESULT_H__
#define __ELASTOS_IO_CHARSET_CCODERRESULT_H__

#include "_Elastos_IO_Charset_CCoderResult.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::Object;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace IO {
namespace Charset {

CarClass(CCoderResult)
    , public Object
    , public ICoderResult
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CCoderResult();

    CARAPI constructor();

    /**
     * Result object indicating that there is insufficient data in the
     * encoding/decoding buffer or that additional data is required.
     */
    static CARAPI GetUNDERFLOW(
        /* [out] */ ICoderResult** result);

    /**
     * Result object used to indicate that the output buffer does not have
     * enough space available to store the result of the encoding/decoding.
     */
    static CARAPI GetOVERFLOW(
        /* [out] */ ICoderResult** result);

    /**
     * Gets a <code>CoderResult</code> object indicating a malformed-input
     * error.
     *
     * @param length
     *            the length of the malformed-input.
     * @return a <code>CoderResult</code> object indicating a malformed-input
     *         error.
     * @throws IllegalArgumentException
     *             if <code>length</code> is non-positive.
     */
    static CARAPI MalformedForLength(
        /* [in] */ Int32 length,
        /* [out] */ ICoderResult** result);

    /**
     * Gets a <code>CoderResult</code> object indicating an unmappable
     * character error.
     *
     * @param length
     *            the length of the input unit sequence denoting the unmappable
     *            character.
     * @return a <code>CoderResult</code> object indicating an unmappable
     *         character error.
     * @throws IllegalArgumentException
     *             if <code>length</code> is non-positive.
     */
    static CARAPI UnmappableForLength(
        /* [in] */ Int32 length,
        /* [out] */ ICoderResult** result);

    /**
     * Returns true if this result is an underflow condition.
     *
     * @return true if an underflow, otherwise false.
     */
    CARAPI IsUnderflow(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this result represents a malformed-input error or an
     * unmappable-character error.
     *
     * @return true if this is a malformed-input error or an
     *         unmappable-character error, otherwise false.
     */
    CARAPI IsError(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this result represents a malformed-input error.
     *
     * @return true if this is a malformed-input error, otherwise false.
     */
    CARAPI IsMalformed(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this result is an overflow condition.
     *
     * @return true if this is an overflow, otherwise false.
     */
    CARAPI IsOverflow(
        /* [out] */ Boolean* result);

    /**
     * Returns true if this result represents an unmappable-character error.
     *
     * @return true if this is an unmappable-character error, otherwise false.
     */
    CARAPI IsUnmappable(
        /* [out] */ Boolean* result);

    /**
     * Gets the length of the erroneous input. The length is only meaningful to
     * a malformed-input error or an unmappable character error.
     *
     * @return the length, as an integer, of this object's erroneous input.
     * @throws UnsupportedOperationException
     *             if this result is an overflow or underflow.
     */
    CARAPI GetLength(
        /* [out] */ Int32* length);

    /**
     * Throws an exception corresponding to this coder result.
     *
     * @throws BufferUnderflowException
     *             in case this is an underflow.
     * @throws BufferOverflowException
     *             in case this is an overflow.
     * @throws UnmappableCharacterException
     *             in case this is an unmappable-character error.
     * @throws MalformedInputException
     *             in case this is a malformed-input error.
     * @throws CharacterCodingException
     *             the default exception.
     */
    CARAPI ThrowException();

    /**
     * Returns a text description of this result.
     *
     * @return a text description of this result.
     */
    CARAPI ToString(
        /* [out] */ String* toString);

private:
    static AutoPtr<ICoderResult> CreateCoderResult(
        /* [in] */ Int32 type,
        /* [in] */ Int32 length);

public:
    static const AutoPtr<ICoderResult> UNDERFLOW;

    static const AutoPtr<ICoderResult> OVERFLOW;

private:
    // indicating underflow error type
    static const Int32 TYPE_UNDERFLOW = 1;

    // indicating overflow error type
    static const Int32 TYPE_OVERFLOW = 2;

    // indicating malformed-input error type
    static const Int32 TYPE_MALFORMED_INPUT = 3;

    // indicating unmappable character error type
    static const Int32 TYPE_UNMAPPABLE_CHAR = 4;

    static Object sMalformedMutex;

    static Object sUnmappableMutex;

    // TODO:
    /*
     * Stores unique result objects for each malformed-input error of a certain
     * length
     */
    // static WeakHashMap<Integer, CoderResult> _malformedErrors = new WeakHashMap<Integer, CoderResult>();
    static AutoPtr< HashMap<Int32, AutoPtr<ICoderResult> > > sMalformedErrors; // Temporary achieve

    /*
     * Stores unique result objects for each unmappable-character error of a
     * certain length
     */
    // static WeakHashMap<Integer, CoderResult> _unmappableErrors = new WeakHashMap<Integer, CoderResult>();
    static AutoPtr< HashMap<Int32, AutoPtr<ICoderResult> > > sUnmappableErrors; // Temporary achieve

    // the type of this result
    Int32 mType;

    // the length of the erroneous input
    Int32 mLength;
};

} // namespace Charset
} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHARSET_CCODERRESULT_H__
