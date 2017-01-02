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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_BERTLV_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_BERTLV_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Class for representing BER-TLV objects.
  *
  * @see "ETSI TS 102 223 Annex C" for more information.
  *
  * {@hide}
  */
class BerTlv
    : public Object
{
public:
    /**
      * Gets a list of ComprehensionTlv objects contained in this BER-TLV object.
      *
      * @return A list of COMPREHENSION-TLV object
      */
    virtual CARAPI GetComprehensionTlvs(
        /* [out] */ IList/*<ComprehensionTlv>*/** result);

    /**
      * Gets a tag id of the BER-TLV object.
      *
      * @return A tag integer.
      */
    virtual CARAPI GetTag(
        /* [out] */ Int32* result);

    /**
      * Gets if the length of the BER-TLV object is valid
      *
      * @return if length valid
      */
    virtual CARAPI IsLengthValid(
        /* [out] */ Boolean* result);

    /**
      * Decodes a BER-TLV object from a byte array.
      *
      * @param data A byte array to decode from
      * @return A BER-TLV object decoded
      * @throws ResultException
      */
    static CARAPI_(AutoPtr<BerTlv>) Decode(
        /* [in] */ ArrayOf<Byte>* data);

private:
    BerTlv(
        /* [in] */ Int32 tag,
        /* [in] */ IList/*<ComprehensionTlv*>*/* ctlvs,
        /* [in] */ Boolean lengthValid);

public:
    static const Int32 BER_UNKNOWN_TAG = 0x00;
    static const Int32 BER_PROACTIVE_COMMAND_TAG = 0xd0;
    static const Int32 BER_MENU_SELECTION_TAG = 0xd3;
    static const Int32 BER_EVENT_DOWNLOAD_TAG = 0xd6;

private:
    Int32 mTag;
    AutoPtr<IList/*<ComprehensionTlv>*/> mCompTlvs;
    Boolean mLengthValid;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_BERTLV_H__

