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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_VOICEMAILCONSTANTS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_VOICEMAILCONSTANTS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * {@hide}
  */
class VoiceMailConstants
    : public Object
{
public:
    VoiceMailConstants();

    virtual CARAPI ContainsCarrier(
        /* [in] */ const String& carrier,
        /* [out] */ Boolean* result);

    virtual CARAPI GetCarrierName(
        /* [in] */ const String& carrier,
        /* [out] */ String* result);

    virtual CARAPI GetVoiceMailNumber(
        /* [in] */ const String& carrier,
        /* [out] */ String* result);

    virtual CARAPI GetVoiceMailTag(
        /* [in] */ const String& carrier,
        /* [out] */ String* result);

private:
    CARAPI_(void) LoadVoiceMail();

public:
    static const String LOGTAG;
    static const String PARTNER_VOICEMAIL_PATH;
    static const Int32 NAME = 0;
    static const Int32 NUMBER = 1;
    static const Int32 TAG = 2;
    static const Int32 SIZE = 3;

private:
     AutoPtr<IHashMap> mCarrierVmMap;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_VOICEMAILCONSTANTS_H__

