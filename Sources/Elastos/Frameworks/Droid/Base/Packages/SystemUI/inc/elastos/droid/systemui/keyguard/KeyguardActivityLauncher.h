
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDACTIVITYLAUNCHER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDACTIVITYLAUNCHER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>
#include <elastos/droid/os/Runnable.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class KeyguardActivityLauncher
    : public Object
    , public IKeyguardActivityLauncher
{
public:
    class CameraWidgetInfo
        : public Object
        , public IKeyguardActivityLauncherCameraWidgetInfo
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("KeyguardActivityLauncher::CameraWidgetInfo")

        CameraWidgetInfo();

        CARAPI SetContextPackage(
            /* [in] */ const String& str);

        CARAPI GetContextPackage(
            /* [out] */ String* str);

        CARAPI SetLayoutId(
            /* [in] */ Int32 id);

        CARAPI GetLayoutId(
            /* [out] */ Int32* id);

    public:
        String mContextPackage;
        Int32 mLayoutId;
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
            /* [in] */ KeyguardActivityLauncher* host,
            /* [in] */ IIntent* intent,
            /* [in] */ IBundle* animation,
            /* [in] */ IHandler* worker,
            /* [in] */ IRunnable* onStarted);

        //@Override
        CARAPI OnDismiss(
            /* [out] */ Boolean* result);

    private:
        KeyguardActivityLauncher* mHost;
        AutoPtr<IIntent> mIntent;
        AutoPtr<IBundle> mAnimation;
        AutoPtr<IHandler> mWorker;
        AutoPtr<IRunnable> mOnStarted;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("KeyguardActivityLauncher::MyRunnable")

        MyRunnable(
            /* [in] */ KeyguardActivityLauncher* host,
            /* [in] */ IIntent* intent,
            /* [in] */ IBundle* options,
            /* [in] */ IRunnable* onStarted,
            /* [in] */ IUserHandle* user);

        //@Override
        CARAPI Run();

    private:
        KeyguardActivityLauncher* mHost;
        AutoPtr<IIntent> mIntent;
        AutoPtr<IBundle> mOptions;
        AutoPtr<IRunnable> mOnStarted;
        AutoPtr<IUserHandle> mUser;
    };

public:
    CAR_INTERFACE_DECL()

    virtual CARAPI GetContext(
        /* [out] */ IContext** context) = 0;

    virtual CARAPI GetLockPatternUtils(
        /* [out] */ ILockPatternUtils** utils) = 0;

    virtual CARAPI SetOnDismissAction(
        /* [in] */ IKeyguardHostViewOnDismissAction* action) = 0;

    virtual CARAPI RequestDismissKeyguard() = 0;

    CARAPI GetCameraWidgetInfo(
        /* [out] */ IKeyguardActivityLauncherCameraWidgetInfo** info);

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