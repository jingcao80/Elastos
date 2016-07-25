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