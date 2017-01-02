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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_COMPREHENSIONTLV_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_COMPREHENSIONTLV_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Class for representing COMPREHENSION-TLV objects.
  *
  * @see "ETSI TS 101 220 subsection 7.1.1"
  *
  * {@hide}
  */
class ComprehensionTlv
    : public Object
{
public:
    virtual CARAPI GetTag(
        /* [out] */ Int32* result);

    virtual CARAPI IsComprehensionRequired(
        /* [out] */ Boolean* result);

    virtual CARAPI GetLength(
        /* [out] */ Int32* result);

    virtual CARAPI GetValueIndex(
        /* [out] */ Int32* result);

    virtual CARAPI GetRawValue(
        /* [out] */ ArrayOf<Byte>** result);

    /**
      * Parses a list of COMPREHENSION-TLV objects from a byte array.
      *
      * @param data A byte array containing data to be parsed
      * @param startIndex Index in data at which to start parsing
      * @return A list of COMPREHENSION-TLV objects parsed
      * @throws ResultException
      */
    static CARAPI_(AutoPtr<IList/*<AutoPtr<ComprehensionTlv>>*/>) DecodeMany(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 startIndex);

    /**
      * Parses an COMPREHENSION-TLV object from a byte array.
      *
      * @param data A byte array containing data to be parsed
      * @param startIndex Index in data at which to start parsing
      * @return A COMPREHENSION-TLV object parsed
      * @throws ResultException
      */
    static CARAPI_(AutoPtr<ComprehensionTlv>) Decode(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 startIndex);

protected:
    /**
      * Constructor. Private on purpose. Use
      * {@link #decodeMany(byte[], int) decodeMany} or
      * {@link #decode(byte[], int) decode} method.
      *
      * @param tag The tag for this object
      * @param cr Comprehension Required flag
      * @param length Length of the value
      * @param data Byte array containing the value
      * @param valueIndex Index in data at which the value starts
      */
    ComprehensionTlv(
        /* [in] */ Int32 tag,
        /* [in] */ Boolean cr,
        /* [in] */ Int32 length,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 valueIndex);

private:
    static const String LOGTAG;
    Int32 mTag;
    Boolean mCr;
    Int32 mLength;
    Int32 mValueIndex;
    AutoPtr<ArrayOf<Byte> > mRawValue;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_COMPREHENSIONTLV_H__

