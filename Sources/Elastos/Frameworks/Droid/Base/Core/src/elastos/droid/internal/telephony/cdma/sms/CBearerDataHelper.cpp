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

#include "elastos/droid/internal/telephony/cdma/sms/CBearerDataHelper.h"
#include "elastos/droid/internal/telephony/cdma/sms/BearerData.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

CAR_INTERFACE_IMPL(CBearerDataHelper, Singleton, IBearerDataHelper)
CAR_SINGLETON_IMPL(CBearerDataHelper)

ECode CBearerDataHelper::CalcTextEncodingDetails(
    /* [in] */ ICharSequence* msg,
    /* [in] */ Boolean force7BitEncoding,
    /* [out] */ IGsmAlphabetTextEncodingDetails** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IGsmAlphabetTextEncodingDetails> gsted = BearerData::CalcTextEncodingDetails(msg, force7BitEncoding);
    *result = gsted;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBearerDataHelper::Encode(
    /* [in] */ IBearerData* bData,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > array = BearerData::Encode(bData);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBearerDataHelper::Decode(
    /* [in] */ ArrayOf<Byte>* smsData,
    /* [out] */ IBearerData** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IBearerData> bd = BearerData::Decode(smsData);
    *result = bd;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBearerDataHelper::Decode(
    /* [in] */ ArrayOf<Byte>* smsData,
    /* [in] */ Int32 serviceCategory,
    /* [out] */ IBearerData** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IBearerData> bd = BearerData::Decode(smsData, serviceCategory);
    *result = bd;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
