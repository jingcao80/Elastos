#ifndef __ELASTOS_APPS_CONTACTS_COMMON_CALLUTIL_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_CALLUTIL_H__

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

/**
 * Utilities related to calls.
 */
class CallUtil
    : Object
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
        /* [in] */ IPhoneAccountHandle accountHandle*,
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
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_CALLUTIL_H__
