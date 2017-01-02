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

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/ComprehensionTlv.h"
#include "elastos/droid/internal/telephony/cat/CatLog.h"

#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                           ComprehensionTlv
//=====================================================================
const String ComprehensionTlv::LOGTAG("ComprehensionTlv");

ComprehensionTlv::ComprehensionTlv(
    /* [in] */ Int32 tag,
    /* [in] */ Boolean cr,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 valueIndex)
{
    mTag = tag;
    mCr = cr;
    mLength = length;
    mValueIndex = valueIndex;
    mRawValue = data;
}

ECode ComprehensionTlv::GetTag(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTag;
    return NOERROR;
}

ECode ComprehensionTlv::IsComprehensionRequired(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCr;
    return NOERROR;
}

ECode ComprehensionTlv::GetLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLength;
    return NOERROR;
}

ECode ComprehensionTlv::GetValueIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValueIndex;
    return NOERROR;
}

ECode ComprehensionTlv::GetRawValue(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRawValue;
    return NOERROR;
}

AutoPtr<IList/*< AutoPtr<ComprehensionTlv> >*/> ComprehensionTlv::DecodeMany(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 startIndex)
{
    AutoPtr<IArrayList> items;
    CArrayList::New((IArrayList**)&items);
    Int32 endIndex = data->GetLength();
    while (startIndex < endIndex) {
        AutoPtr<ComprehensionTlv> ctlv = ComprehensionTlv::Decode(data, startIndex);
        if (ctlv != NULL) {
            items->Add((IObject*)ctlv.Get());
            startIndex = ctlv->mValueIndex + ctlv->mLength;
        }
        else {
            CatLog::D(LOGTAG, String("decodeMany: ctlv is null, stop decoding"));
            break;
        }
    }
    return IList::Probe(items);
}

AutoPtr<ComprehensionTlv> ComprehensionTlv::Decode(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 startIndex)
{
    Int32 curIndex = startIndex;
    Int32 endIndex = data->GetLength();

    // try {
        /* tag */
        Int32 tag = 0;
        Boolean cr; // Comprehension required flag
        Int32 temp = (*data)[curIndex++] & 0xff;
        switch (temp) {
        case 0:
        case 0xff:
        case 0x80:
            CatLog::D(String("CAT     "), String("decode: unexpected first tag byte=")
                    + StringUtils::ToString(temp)
                    + String(", startIndex=") + StringUtils::ToString(startIndex)
                    + String(" curIndex=") + StringUtils::ToString(curIndex)
                    + String(" endIndex=") + StringUtils::ToString(endIndex));
            // Return null which will stop decoding, this has occurred
            // with Ghana MTN simcard and JDI simcard.
            return NULL;

        case 0x7f: // tag is in three-byte format
            tag = (((*data)[curIndex] & 0xff) << 8)
                    | ((*data)[curIndex + 1] & 0xff);
            cr = (tag & 0x8000) != 0;
            tag &= ~0x8000;
            curIndex += 2;
            break;

        default: // tag is in single-byte format
            tag = temp;
            cr = (tag & 0x80) != 0;
            tag &= ~0x80;
            break;
        }

        /* length */
        Int32 length = 0;
        temp = (*data)[curIndex++] & 0xff;
        if (temp < 0x80) {
            length = temp;
        }
        else if (temp == 0x81) {
            length = (*data)[curIndex++] & 0xff;
            if (length < 0x80) {
                // throw new ResultException(
                //         ResultCode.CMD_DATA_NOT_UNDERSTOOD,
                //         "length < 0x80 length=" + Integer.toHexString(length) +
                //         " startIndex=" + startIndex + " curIndex=" + curIndex +
                //         " endIndex=" + endIndex);
                return NULL;
            }
        }
        else if (temp == 0x82) {
            length = (((*data)[curIndex] & 0xff) << 8)
                    | ((*data)[curIndex + 1] & 0xff);
            curIndex += 2;
            if (length < 0x100) {
                // throw new ResultException(
                //         ResultCode.CMD_DATA_NOT_UNDERSTOOD,
                //         "two byte length < 0x100 length=" + Integer.toHexString(length) +
                //         " startIndex=" + startIndex + " curIndex=" + curIndex +
                //         " endIndex=" + endIndex);
                return NULL;
            }
        }
        else if (temp == 0x83) {
            length = (((*data)[curIndex] & 0xff) << 16)
                    | (((*data)[curIndex + 1] & 0xff) << 8)
                    | ((*data)[curIndex + 2] & 0xff);
            curIndex += 3;
            if (length < 0x10000) {
                // throw new ResultException(
                //         ResultCode.CMD_DATA_NOT_UNDERSTOOD,
                //         "three byte length < 0x10000 length=0x" + Integer.toHexString(length) +
                //         " startIndex=" + startIndex + " curIndex=" + curIndex +
                //         " endIndex=" + endIndex);
                return NULL;
            }
        }
        else {
            // throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD,
            //         "Bad length modifer=" + temp +
            //         " startIndex=" + startIndex + " curIndex=" + curIndex +
            //         " endIndex=" + endIndex);
            return NULL;
        }

        AutoPtr<ComprehensionTlv> p = new ComprehensionTlv(tag, cr, length, data, curIndex);
        return p;

    // } catch (IndexOutOfBoundsException e) {
    //     throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD,
    //             "IndexOutOfBoundsException" + " startIndex=" + startIndex +
    //             " curIndex=" + curIndex + " endIndex=" + endIndex);
    // }
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
