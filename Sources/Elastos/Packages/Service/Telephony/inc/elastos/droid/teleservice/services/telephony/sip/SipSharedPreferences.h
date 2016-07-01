#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_SIPSHAREDPREFERENCE_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_SIPSHAREDPREFERENCE_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Telecomm.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringBuffer.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;
using Elastos::Core::Object;
using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

class SipSharedPreferences
    : public Object
{
public:
    TO_STRING_IMPL("SipSharedPreferences")

    SipSharedPreferences(
        /* [in] */ IContext* context);

    /**
     * Returns the primary account URI or null if it does not exist.
     * @deprecated The primary account setting is no longer used.
     */
    //@Deprecated
    CARAPI GetPrimaryAccount(
        /* [out] */ String* account);

    CARAPI SetProfilesCount(
        /* [in] */ Int32 number);

    CARAPI GetProfilesCount(
        /* [out] */ Int32* count);

    CARAPI SetSipCallOption(
        /* [in] */ const String& option);

    CARAPI GetSipCallOption(
        /* [out] */ String* str);

    CARAPI SetReceivingCallsEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsReceivingCallsEnabled(
        /* [out] */ Boolean* result);

    /**
     * Performs cleanup of the shared preferences, removing the deprecated primary account key if
     * it exists.
     */
    CARAPI CleanupPrimaryAccountSetting();

private:
    // TODO: back up to Android Backup 144.55

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String PREFIX;
    static const Boolean VERBOSE; /* STOP SHIP if true */

    static const String SIP_SHARED_PREFERENCES;

    /**
     * @deprecated Primary account selection for SIP accounts is no longer relevant.
     */
    //@Deprecated
    static const String KEY_PRIMARY_ACCOUNT;

    static const String KEY_NUMBER_OF_PROFILES;

    AutoPtr<ISharedPreferences> mPreferences;
    AutoPtr<IContext> mContext;
};

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_SIPSHAREDPREFERENCE_H__