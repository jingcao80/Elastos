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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_TELEPHONYUTIL_H__
#define __ELASTOS_DROID_SERVER_TELECOM_TELEPHONYUTIL_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Telecom::IPhoneAccountHandle;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Utilities to deal with the system telephony services. The system telephony services are treated
 * differently from 3rd party services in some situations (emergency calls, audio focus, etc...).
 */
class TelephonyUtil
    : public Object
{
public:
    CAR_INTERFACE_DECL()

public:
    /**
     * @return fallback {@link PhoneAccount} to be used by Telecom for emergency calls in the
     * rare case that Telephony has not registered any phone accounts yet. Details about this
     * account are not expected to be displayed in the UI, so the description, etc are not
     * populated.
     */
    static CARAPI GetDefaultEmergencyPhoneAccount(
        /* [out] */ IPhoneAccount** result);

    static CARAPI IsPstnComponentName(
        /* [in] */ IComponentName* componentName,
        /* [out] */ Boolean* result);

    static CARAPI ShouldProcessAsEmergency(
        /* [in] */ IContext* context,
        /* [in] */ IUri* handle,
        /* [out] */ Boolean* result);

private:
    TelephonyUtil();

    static CARAPI_(AutoPtr<IPhoneAccountHandle>) InitDEFAULT_EMERGENCY_PHONE_ACCOUNT_HANDLE();

private:
    static const String TAG;

    static const String TELEPHONY_PACKAGE_NAME;

    static const String PSTN_CALL_SERVICE_CLASS_NAME;

    static AutoPtr<IPhoneAccountHandle> DEFAULT_EMERGENCY_PHONE_ACCOUNT_HANDLE;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_TELEPHONYUTIL_H__
