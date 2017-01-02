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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_CALLUTIL_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_CALLUTIL_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telecom.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Telecom::IPhoneAccountHandle;

namespace Elastos{
namespace Droid {
namespace Contacts {
namespace Common {

/**
 * Utilities related to calls.
 */
class CallUtil
{
public:
    /**
     * Return an Intent for making a phone call. Scheme (e.g. tel, sip) will be determined
     * automatically.
     */
    static CARAPI_(AutoPtr<IIntent>) GetCallIntent(
        /* [in] */ const String& number);

    /**
     * Return an Intent for making a phone call. A given Uri will be used as is (without any
     * sanity check).
     */
    static CARAPI_(AutoPtr<IIntent>) GetCallIntent(
        /* [in] */ IUri* uri);

    /**
     * A variant of {@link #getCallIntent(String)} but also accept a call origin.
     * For more information about call origin, see comments in Phone package (PhoneApp).
     */
    static CARAPI_(AutoPtr<IIntent>) GetCallIntent(
        /* [in] */ const String& number,
        /* [in] */ const String& callOrigin);

    /**
     * A variant of {@link #getCallIntent(String)} but also include {@code Account}.
     */
    static CARAPI_(AutoPtr<IIntent>) GetCallIntent(
        /* [in] */ const String& number,
        /* [in] */ IPhoneAccountHandle* accountHandle);

    /**
     * A variant of {@link #getCallIntent(android.net.Uri)} but also include {@code Account}.
     */
    static CARAPI_(AutoPtr<IIntent>) GetCallIntent(
        /* [in] */ IUri* uri,
        /* [in] */ IPhoneAccountHandle* accountHandle);

    /**
     * A variant of {@link #getCallIntent(String, String)} but also include {@code Account}.
     */
    static CARAPI_(AutoPtr<IIntent>) GetCallIntent(
            /* [in] */ const String& number,
            /* [in] */ const String& callOrigin,
            /* [in] */ IPhoneAccountHandle* accountHandle);

    /**
     * A variant of {@link #getCallIntent(android.net.Uri)} but also accept a call
     * origin and {@code Account}.
     * For more information about call origin, see comments in Phone package (PhoneApp).
     */
    static CARAPI_(AutoPtr<IIntent>) GetCallIntent(
        /* [in] */ IUri* uri,
        /* [in] */ const String& callOrigin,
        /* [in] */ IPhoneAccountHandle* accountHandle);

    /**
     * A variant of {@link #getCallIntent(String, String)} for starting a video call.
     */
    static CARAPI_(AutoPtr<IIntent>) GetVideoCallIntent(
        /* [in] */ const String& number,
        /* [in] */ const String& callOrigin);

    /**
     * A variant of {@link #getCallIntent(String, String, android.telecom.PhoneAccountHandle)} for
     * starting a video call.
     */
    static CARAPI_(AutoPtr<IIntent>) GetVideoCallIntent(
        /* [in] */ const String& number,
        /* [in] */ const String& callOrigin,
        /* [in] */ IPhoneAccountHandle* accountHandle);

    /**
     * A variant of {@link #getCallIntent(String, String, android.telecom.PhoneAccountHandle)} for
     * starting a video call.
     */
    static CARAPI_(AutoPtr<IIntent>) GetVideoCallIntent(
        /* [in] */ const String& number,
        /* [in] */ IPhoneAccountHandle* accountHandle);

    /**
     * A variant of {@link #getCallIntent(android.net.Uri)} but also accept a call
     * origin and {@code Account} and {@code VideoCallProfile} state.
     * For more information about call origin, see comments in Phone package (PhoneApp).
     */
    static CARAPI_(AutoPtr<IIntent>) GetCallIntent(
        /* [in] */ IUri* uri,
        /* [in] */ const String& callOrigin,
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [in] */ Int32 videoState);


    /**
     * Return Uri with an appropriate scheme, accepting both SIP and usual phone call
     * numbers.
     */
    static CARAPI_(AutoPtr<IUri>) GetCallUri(
        /* [in] */ const String& number);

    static CARAPI_(Boolean) IsVideoEnabled(
        /* [in] */ IContext* context);
};

} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_CALLUTIL_H__
