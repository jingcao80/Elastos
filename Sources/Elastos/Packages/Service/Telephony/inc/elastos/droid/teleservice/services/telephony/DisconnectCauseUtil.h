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

#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_DISCONNECTCAUSEUTIL_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_DISCONNECTCAUSEUTIL_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Telecom.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Telecom::IDisconnectCause;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

class DisconnectCauseUtil
{
public:
    /**
    * Converts from a disconnect code in {@link android.telephony.DisconnectCause} into a more generic
    * {@link android.telecom.DisconnectCause}.object, possibly populated with a localized message
    * and tone.
    *
    * @param context The context.
    * @param telephonyDisconnectCause The code for the reason for the disconnect.
    */
    static CARAPI_(AutoPtr<IDisconnectCause>) ToTelecomDisconnectCause(
        /* [in] */ Int32 telephonyDisconnectCause);

   /**
    * Converts from a disconnect code in {@link android.telephony.DisconnectCause} into a more generic
    * {@link android.telecom.DisconnectCause}.object, possibly populated with a localized message
    * and tone.
    *
    * @param context The context.
    * @param telephonyDisconnectCause The code for the reason for the disconnect.
    * @param reason Description of the reason for the disconnect, not intended for the user to see..
    */
    static CARAPI_(AutoPtr<IDisconnectCause>) ToTelecomDisconnectCause(
        /* [in] */ Int32 telephonyDisconnectCause,
        /* [in] */ const String& reason);

private:
    /**
     * Convert the {@link android.telephony.DisconnectCause} disconnect code into a
     * {@link android.telecom.DisconnectCause} disconnect code.
     * @return The disconnect code as defined in {@link android.telecom.DisconnectCause}.
     */
    static CARAPI_(Int32) ToTelecomDisconnectCauseCode(
        /* [in] */ Int32 telephonyDisconnectCause);

    /**
     * Returns a label for to the disconnect cause to be shown to the user.
     */
    static CARAPI_(AutoPtr<ICharSequence>) ToTelecomDisconnectCauseLabel(
        /* [in] */ IContext* context,
        /* [in] */ Int32 telephonyDisconnectCause);

    /**
     * Returns a description of the disconnect cause to be shown to the user.
     */
    static CARAPI_(AutoPtr<ICharSequence>) ToTelecomDisconnectCauseDescription(
        /* [in] */ IContext* context,
        /* [in] */ Int32 telephonyDisconnectCause);

    static CARAPI_(String) ToTelecomDisconnectReason(
        /* [in] */ Int32 telephonyDisconnectCause,
        /* [in] */ const String& reason);

    /**
     * Returns the tone to play for the disconnect cause, or UNKNOWN if none should be played.
     */
    static CARAPI_(Int32) ToTelecomDisconnectCauseTone(
        /* [in] */ Int32 telephonyDisconnectCause);
};

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_DISCONNECTCAUSEUTIL_H__