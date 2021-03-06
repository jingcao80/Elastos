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

#ifndef __ELASTOS_DROID_DIALER_INTERACTIONS_CPHONENUMBERINTERACTION_H__
#define __ELASTOS_DROID_DIALER_INTERACTIONS_CPHONENUMBERINTERACTION_H__

#include "_Elastos_Droid_Dialer_Interactions_CPhoneNumberInteraction.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Net.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Dialer::Activity::ITransactionSafeActivity;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Interactions {

CarClass(CPhoneNumberInteraction)
    , public Singleton
    , public IPhoneNumberInteraction
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    /**
     * Start call action using given contact Uri. If there are multiple candidates for the phone
     * call, dialog is automatically shown and the user is asked to choose one.
     *
     * @param activity that is calling this interaction. This must be of type
     * {@link TransactionSafeActivity} because we need to check on the activity state after the
     * phone numbers have been queried for.
     * @param uri contact Uri (built from {@link Contacts#CONTENT_URI}) or data Uri
     * (built from {@link Data#CONTENT_URI}). Contact Uri may show the disambiguation dialog while
     * data Uri won't.
     */
    CARAPI StartInteractionForPhoneCall(
        /* [in] */ ITransactionSafeActivity* activity,
        /* [in] */ IUri* uri);

    /**
     * Start call action using given contact Uri. If there are multiple candidates for the phone
     * call, dialog is automatically shown and the user is asked to choose one.
     *
     * @param activity that is calling this interaction. This must be of type
     * {@link TransactionSafeActivity} because we need to check on the activity state after the
     * phone numbers have been queried for.
     * @param uri contact Uri (built from {@link Contacts#CONTENT_URI}) or data Uri
     * (built from {@link Data#CONTENT_URI}). Contact Uri may show the disambiguation dialog while
     * data Uri won't.
     * @param useDefault Whether or not to use the primary(default) phone number. If true, the
     * primary phone number will always be used by default if one is available. If false, a
     * disambiguation dialog will be shown regardless of whether or not a primary phone number
     * is available.
     */
    CARAPI StartInteractionForPhoneCall(
        /* [in] */ ITransactionSafeActivity* activity,
        /* [in] */ IUri* uri,
        /* [in] */ Boolean useDefault);

    /**
     * @param activity that is calling this interaction. This must be of type
     * {@link TransactionSafeActivity} because we need to check on the activity state after the
     * phone numbers have been queried for.
     * @param callOrigin If non null, {@link PhoneConstants#EXTRA_CALL_ORIGIN} will be
     * appended to the Intent initiating phone call. See comments in Phone package (PhoneApp)
     * for more detail.
     */
    CARAPI StartInteractionForPhoneCall(
        /* [in] */ ITransactionSafeActivity* activity,
        /* [in] */ IUri* uri,
        /* [in] */ const String& callOrigin);

    /**
     * Start text messaging (a.k.a SMS) action using given contact Uri. If there are multiple
     * candidates for the phone call, dialog is automatically shown and the user is asked to choose
     * one.
     *
     * @param activity that is calling this interaction. This must be of type
     * {@link TransactionSafeActivity} because we need to check on the activity state after the
     * phone numbers have been queried for.
     * @param uri contact Uri (built from {@link Contacts#CONTENT_URI}) or data Uri
     * (built from {@link Data#CONTENT_URI}). Contact Uri may show the disambiguation dialog while
     * data Uri won't.
     */
    CARAPI StartInteractionForTextMessage(
        /* [in] */ ITransactionSafeActivity* activity,
        /* [in] */ IUri* uri);
};

} // Interactions
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_INTERACTIONS_CPHONENUMBERINTERACTION_H__
