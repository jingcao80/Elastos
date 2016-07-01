#ifndef __ELASTOS_APPS_CONTACTS_COMMON_CCALLUTIL_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_CCALLUTIL_H__

#include "_Elastos_Apps_Contacts_Common_CCallUtil.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

/**
 * Utilities related to calls.
 */
CarClass(CCallUtil)
    , public singleton
    , public ICallUtil
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Return an Intent for making a phone call. Scheme (e.g. tel, sip) will be determined
     * automatically.
     */
    CARAPI GetCallIntent(
        /* [in] */ const String& number,
        /* [out] */ IIntent** intent);

    /**
     * Return an Intent for making a phone call. A given Uri will be used as is (without any
     * sanity check).
     */
    CARAPI GetCallIntent(
        /* [in] */ IUri* uri,
        /* [out] */ IIntent** intent);

    /**
     * A variant of {@link #getCallIntent(String)} but also accept a call origin.
     * For more information about call origin, see comments in Phone package (PhoneApp).
     */
    CARAPI GetCallIntent(
        /* [in] */ const String& number,
        /* [in] */ const String& callOrigin,
        /* [out] */ IIntent** intent);

    /**
     * A variant of {@link #getCallIntent(String)} but also include {@code Account}.
     */
    CARAPI GetCallIntent(
        /* [in] */ const String& number,
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [out] */ IIntent** intent);

    /**
     * A variant of {@link #getCallIntent(android.net.Uri)} but also include {@code Account}.
     */
    CARAPI GetCallIntent(
        /* [in] */ IUri* uri,
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [out] */ IIntent** intent);

    /**
     * A variant of {@link #getCallIntent(String, String)} but also include {@code Account}.
     */
    CARAPI GetCallIntent(
        /* [in] */ const String& number,
        /* [in] */ const String& callOrigin,
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [out] */ IIntent** intent);

    /**
     * A variant of {@link #getCallIntent(android.net.Uri)} but also accept a call
     * origin and {@code Account}.
     * For more information about call origin, see comments in Phone package (PhoneApp).
     */
    CARAPI GetCallIntent(
        /* [in] */ IUri* uri,
        /* [in] */ const String& callOrigin,
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [out] */ IIntent** intent);

    /**
     * A variant of {@link #getCallIntent(String, String)} for starting a video call.
     */
    CARAPI GetVideoCallIntent(
        /* [in] */ const String& number,
        /* [in] */ const String& callOrigin,
        /* [out] */ IIntent** intent);

    /**
     * A variant of {@link #getCallIntent(String, String, android.telecom.PhoneAccountHandle)} for
     * starting a video call.
     */
    CARAPI GetVideoCallIntent(
        /* [in] */ const String& number,
        /* [in] */ const String& callOrigin,
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [out] */ IIntent** intent);

    /**
     * A variant of {@link #getCallIntent(String, String, android.telecom.PhoneAccountHandle)} for
     * starting a video call.
     */
    CARAPI GetVideoCallIntent(
        /* [in] */ const String& number,
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [out] */ IIntent** intent);

    /**
     * A variant of {@link #getCallIntent(android.net.Uri)} but also accept a call
     * origin and {@code Account} and {@code VideoCallProfile} state.
     * For more information about call origin, see comments in Phone package (PhoneApp).
     */
    CARAPI GetCallIntent(
        /* [in] */ IUri* uri,
        /* [in] */ const String& callOrigin,
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [in] */ Int32 videoState,
        /* [out] */ IIntent** intent);

    /**
     * Return Uri with an appropriate scheme, accepting both SIP and usual phone call
     * numbers.
     */
    CARAPI GetCallUri(
        /* [in] */ const String& number,
        /* [out] */ IUri* uri);

    CARAPI IsVideoEnabled(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);
};

} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_CCALLUTIL_H__
