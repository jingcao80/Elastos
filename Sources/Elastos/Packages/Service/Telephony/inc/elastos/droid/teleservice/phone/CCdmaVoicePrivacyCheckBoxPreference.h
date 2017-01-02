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

#ifndef  __ELASTOS_DROID_PHONE_CCDMAVOICEPRIVACYCHECKBOXPREFERENCE_H__
#define  __ELASTOS_DROID_PHONE_CCDMAVOICEPRIVACYCHECKBOXPREFERENCE_H__

#include "_Elastos_Droid_TeleService_Phone_CCdmaVoicePrivacyCheckBoxPreference.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/preference/CheckBoxPreference.h"

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Preference::CheckBoxPreference;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CarClass(CCdmaVoicePrivacyCheckBoxPreference)
    , public CheckBoxPreference
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CCdmaVoicePrivacyCheckBoxPreference::MyHandler")

        MyHandler(
            /* [in] */ CCdmaVoicePrivacyCheckBoxPreference* mHost);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(void) HandleGetVPResponse(
            /* [in] */ IMessage* msg);

        CARAPI_(void) HandleSetVPResponse(
            /* [in] */ IMessage* msg);

    public:
        static const Int32 MESSAGE_GET_VP;
        static const Int32 MESSAGE_SET_VP;

        CCdmaVoicePrivacyCheckBoxPreference* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CCdmaVoicePrivacyCheckBoxPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

protected:
    //@Override
    CARAPI OnClick();

private:
    static const String TAG;
    const Boolean DBG;

    AutoPtr<IPhone> mPhone;
    AutoPtr<IHandler> mHandler;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CCDMAVOICEPRIVACYCHECKBOXPREFERENCE_H__