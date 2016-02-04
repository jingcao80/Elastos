
#ifndef __ELASTOS_APPS_PACKAGEINSTALLER_CUNINSTALLAPPPROGRESS_H__
#define __ELASTOS_APPS_PACKAGEINSTALLER_CUNINSTALLAPPPROGRESS_H__

//#include "elastos/droid/app/Activity.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include "_Elastos_Apps_PackageInstaller_CUninstallAppProgress.h"
#include "elastos/core/Object.h"

//using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IPackageDeleteObserver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageDeleteObserver;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::Object;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

/**
  * This activity corresponds to a download progress screen that is displayed
  * when an application is uninstalled. The result of the application uninstall
  * is indicated in the result code that gets set to 0 or 1. The application gets launched
  * by an intent with the intent's class name explicitly set to CUninstallAppProgress and expects
  * the application object of the application to uninstall.
  */
CarClass(CUninstallAppProgress)
    , public Object // Activity
    , public IViewOnClickListener
    , public IUninstallAppProgress
{
public:
    class PackageDeleteObserver
        : public Object
        , public IIPackageDeleteObserver
    {
    public:
        CAR_INTERFACE_DECL()

        PackageDeleteObserver(
            /* [in] */ CUninstallAppProgress* owner);

        virtual CARAPI PackageDeleted(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 returnCode);

    private:
        CUninstallAppProgress* mOwner;
    };

private:
    class InnerHandler
        : public Handler
    {
    public:
        InnerHandler(
            /* [in] */ CUninstallAppProgress* owner);

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CUninstallAppProgress* mOwner;
    };

    class InnerOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnClickListener(
            /* [in] */ CUninstallAppProgress* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CUninstallAppProgress* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    CUninstallAppProgress();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    virtual CARAPI SetResultAndFinish(
        /* [in] */ Int32 retCode);

    virtual CARAPI InitView();

    virtual CARAPI OnClick(
        /* [in] */ IView* v);

    // @Override
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* ev,
        /* [out] */ Boolean* result);

private:
    /*const*/ String TAG;
    Boolean localLOGV;
    AutoPtr<IApplicationInfo> mAppInfo;
    Boolean mAllUsers;
    AutoPtr<IUserHandle> mUser;
    //AutoPtr<IBinder> mCallback;
    AutoPtr<ITextView> mStatusTextView;
    AutoPtr<IButton> mOkButton;
    AutoPtr<IButton> mDeviceManagerButton;
    AutoPtr<IProgressBar> mProgressBar;
    AutoPtr<IView> mOkPanel;
    volatile Int32 mResultCode;
    static const Int32 UNINSTALL_COMPLETE = 1;
    AutoPtr<IHandler> mHandler;
};

} // namespace PackageInstaller
} // namespace Apps
} // namespace Elastos

#endif // __ELASTOS_APPS_PACKAGEINSTALLER_CUNINSTALLAPPPROGRESS_H__

