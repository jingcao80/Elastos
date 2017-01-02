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

#ifndef __ELASTOS_DROID_TELEPHONY_CICCOPENLOGICALCHANNELRESPONSE_H__
#define __ELASTOS_DROID_TELEPHONY_CICCOPENLOGICALCHANNELRESPONSE_H__

#include "_Elastos_Droid_Telephony_CIccOpenLogicalChannelResponse.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CIccOpenLogicalChannelResponse)
    , public Object
    , public IIccOpenLogicalChannelResponse
    , public IParcelable
{
public:
    CIccOpenLogicalChannelResponse();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 channel,
        /* [in] */ Int32 status,
        /* [in] */ ArrayOf<Byte>* selectResponse);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetChannel(
        /* [out] */ Int32* result);

    CARAPI GetStatus(
        /* [out] */ Int32* result);

    CARAPI GetSelectResponse(
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    Int32 mChannel;
    Int32 mStatus;
    AutoPtr<ArrayOf<Byte> > mSelectResponse;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CICCOPENLOGICALCHANNELRESPONSE_H__
