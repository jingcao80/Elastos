
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDACTIVITYLAUNCHER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDACTIVITYLAUNCHER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/Service.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class KeyguardActivityLauncher
    : public Object
{
public:
    class CameraWidgetInfo
        : public Object
    {
    public:
        TO_STRING_IMPL("KeyguardActivityLauncher::CameraWidgetInfo")

    public:
        String contextPackage;
        Int32 layoutId;
    };

private:
    class MyOnDismissAction
        : public Object
        , public IKeyguardHostViewOnDismissAction
    {
    public:
        TO_STRING_IMPL("KeyguardActivityLauncher::MyOnDismissAction")

        CAR_INTERFACE_DECL()

        MyOnDismissAction(
            /* [in] */ KeyguardActivityLauncher* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnDismiss(
            /* [out] */ Boolean* result);

    private:
        KeyguardActivityLauncher* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("KeyguardActivityLauncher::MyRunnable")

        MyRunnable(
            /* [in] */ KeyguardActivityLauncher* host,
            /* [in] */ IUserHandle* user)
            : mHost(host)
            , mUser(user)
        {}

        //@Override
        CARAPI Run();

    private:
        KeyguardActivityLauncher* mHost;
        AutoPtr<IUserHandle> mUser;
    };

public:
    CAR_INTERFACE_DECL()

    virtual GetContext(
        /* [out] */ IContext** context) = 0;

    virtual GetLockPatternUtils(
        /* [out] */ ILockPatternUtils** utils) = 0;

    virtual CARAPI SetOnDismissAction(
        /* [in] */ IKeyguardHostViewOnDismissAction action) = 0;

    virtual CARAPI RequestDismissKeyguard() = 0;

    CARAPI_(AutoPtr<CameraWidgetInfo>) GetCameraWidgetInfo();

    CARAPI LaunchCamera(
        /* [in] */ IHandler* worker,
        /* [in] */ IRunnable* onSecureCameraStarted);

    CARAPI LaunchWidgetPicker(
        /* [in] */ Int32 appWidgetId);

    /**
     * Launches the said intent for the current foreground user.
     *
     * @param intent
     * @param showsWhileLocked true if the activity can be run on top of keyguard.
     *   See {@link WindowManager#FLAG_SHOW_WHEN_LOCKED}
     * @param useDefaultAnimations true if default transitions should be used, else suppressed.
     * @param worker if supplied along with onStarted, used to launch the blocking activity call.
     * @param onStarted if supplied along with worker, called after activity is started.
     */
    CARAPI LaunchActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean showsWhileLocked,
        /* [in] */ Boolean useDefaultAnimations,
        /* [in] */ IHandler* worker,
        /* [in] */ IRunnable* onStarted);

    CARAPI LaunchActivityWithAnimation(
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean showsWhileLocked,
        /* [in] */ IBundle* animation,
        /* [in] */ IHandler* worker,
        /* [in] */ IRunnable* onStarted);

private:
    CARAPI_(Boolean) MustLaunchSecurely();

    CARAPI_(void) DismissKeyguardOnNextActivity();

    CARAPI_(void) StartActivityForCurrentUser(
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* options,
        /* [in] */ IHandler* worker,
        /* [in] */ IRunnable* onStarted);

    CARAPI_(AutoPtr<IIntent>) GetCameraIntent();

    CARAPI_(Boolean) WouldLaunchResolverActivity(
        /* [in] */ IIntent* intent);

    CARAPI_(Boolean) WouldLaunchResolverActivity(
        /* [in] */ IResolveInfo* resolved,
        /* [in] */ IList* appList);

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const String META_DATA_KEYGUARD_LAYOUT;
    static AutoPtr<IIntent> SECURE_CAMERA_INTENT;
    static AutoPtr<IIntent> INSECURE_CAMERA_INTENT;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDACTIVITYLAUNCHER_H__