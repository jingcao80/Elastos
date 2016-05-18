#ifndef __ELASTOS_DROID_APP_CKEYGUARDMANAGER_H__
#define __ELASTOS_DROID_APP_CKEYGUARDMANAGER_H__

#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_App_CKeyguardManager.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Class that can be used to lock and unlock the keyboard. Get an instance of this
 * class by calling {@link android.content.Context#getSystemService(java.lang.String)}
 * with argument {@link android.content.Context#KEYGUARD_SERVICE}. The
 * actual class to control the keyboard locking is
 * {@link android.app.KeyguardManager.KeyguardLock}.
 */
CarClass(CKeyguardManager)
    , public Object
    , public IKeyguardManager
{
public:
    class KeyguardLock
        : public Object
        , public IKeyguardManagerKeyguardLock
    {
    public:
        CAR_INTERFACE_DECL()

        KeyguardLock();

        virtual ~KeyguardLock();

        CARAPI constructor(
            /* [in] */ const String& tag,
            /* [in] */ IKeyguardManager* owner);

        /**
         * Disable the keyguard from showing.  If the keyguard is currently
         * showing, hide it.  The keyguard will be prevented from showing again
         * until {@link #reenableKeyguard()} is called.
         *
         * A good place to call this is from {@link android.app.Activity#onResume()}
         *
         * Note: This call has no effect while any {@link android.app.admin.DevicePolicyManager}
         * is enabled that requires a password.
         *
         * <p>This method requires the caller to hold the permission
         * {@link android.Manife
         st.permission#DISABLE_KEYGUARD}.
         *
         * @see #reenableKeyguard()
         */
        CARAPI DisableKeyguard();

        /**
         * Reenable the keyguard.  The keyguard will reappear if the previous
         * call to {@link #disableKeyguard()} caused it to be hidden.
         *
         * A good place to call this is from {@link android.app.Activity#onPause()}
         *
         * Note: This call has no effect while any {@link android.app.admin.DevicePolicyManager}
         * is enabled that requires a password.
         *
         * <p>This method requires the caller to hold the permission
         * {@link android.Manifest.permission#DISABLE_KEYGUARD}.
         *
         * @see #disableKeyguard()
         */
        CARAPI ReenableKeyguard();

    private:
        AutoPtr<IBinder> mToken;
        String mTag;
        AutoPtr<CKeyguardManager> mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Get an intent to prompt the user to confirm credentials (pin, pattern or password)
     * for the current user of the device. The caller is expected to launch this activity using
     * {@link android.app.Activity#startActivityForResult(Intent, int)} and check for
     * {@link android.app.Activity#RESULT_OK} if the user successfully completes the challenge.
     *
     * @return the intent for launching the activity or null if no password is required.
     **/
    CARAPI CreateConfirmDeviceCredentialIntent(
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* description,
        /* [out] */ IIntent** intent);

    /**
     * @deprecated Use {@link android.view.WindowManager.LayoutParams#FLAG_DISMISS_KEYGUARD}
     * and/or {@link android.view.WindowManager.LayoutParams#FLAG_SHOW_WHEN_LOCKED}
     * instead; this allows you to seamlessly hide the keyguard as your application
     * moves in and out of the foreground and does not require that any special
     * permissions be requested.
     *
     * Enables you to lock or unlock the keyboard. Get an instance of this class by
     * calling {@link android.content.Context#getSystemService(java.lang.String) Context.getSystemService()}.
     * This class is wrapped by {@link android.app.KeyguardManager KeyguardManager}.
     * @param tag A tag that informally identifies who you are (for debugging who
     *   is disabling he keyguard).
     *
     * @return A {@link KeyguardLock} handle to use to disable and reenable the
     *   keyguard.
     */
    CARAPI NewKeyguardLock(
        /* [in] */ const String& tag,
        /* [out] */ IKeyguardManagerKeyguardLock** keyguardLock);

    /**
     * Return whether the keyguard is currently locked.
     *
     * @return true if keyguard is locked.
     */
    CARAPI IsKeyguardLocked(
        /* [out] */ Boolean* isLocked);

    /**
     * Return whether the keyguard requires a password to unlock.
     *
     * @return true if keyguard is secure.
     */
    CARAPI IsKeyguardSecure(
        /* [out] */ Boolean* isSecure);

    /**
     * If keyguard screen is showing or in restricted key input mode (i.e. in
     * keyguard password emergency screen). When in such mode, certain keys,
     * such as the Home key and the right soft keys, don't work.
     *
     * @return true if in keyguard restricted input mode.
     *
     * @see android.view.WindowManagerPolicy#inKeyguardRestrictedKeyInputMode
     */
    CARAPI InKeyguardRestrictedInputMode(
        /* [out] */ Boolean* isInMode);

    /**
     * @deprecated Use {@link android.view.WindowManager.LayoutParams#FLAG_DISMISS_KEYGUARD}
     * and/or {@link android.view.WindowManager.LayoutParams#FLAG_SHOW_WHEN_LOCKED}
     * instead; this allows you to seamlessly hide the keyguard as your application
     * moves in and out of the foreground and does not require that any special
     * permissions be requested.
     *
     * Exit the keyguard securely.  The use case for this api is that, after
     * disabling the keyguard, your app, which was granted permission to
     * disable the keyguard and show a limited amount of information deemed
     * safe without the user getting past the keyguard, needs to navigate to
     * something that is not safe to view without getting past the keyguard.
     *
     * This will, if the keyguard is secure, bring up the unlock screen of
     * the keyguard.
     *
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#DISABLE_KEYGUARD}.
     *
     * @param callback Let's you know whether the operation was succesful and
     *   it is safe to launch anything that would normally be considered safe
     *   once the user has gotten past the keyguard.
     */
    CARAPI ExitKeyguardSecurely(
        /* [in] */ IKeyguardManagerOnKeyguardExitResult* cb);

public:
    AutoPtr<IIWindowManager> mWM;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CKEYGUARDMANAGER_H__
