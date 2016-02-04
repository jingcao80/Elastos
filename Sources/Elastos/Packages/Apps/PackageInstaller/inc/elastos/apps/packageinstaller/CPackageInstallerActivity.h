
#ifndef __ELASTOS_APPS_PACKAGEINSTALLER_CPACKAGEINSTALLERACTIVITY_H__
#define __ELASTOS_APPS_PACKAGEINSTALLER_CPACKAGEINSTALLERACTIVITY_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
//#include "elsatos/droid/app/Activity.h"
#include "_Elastos_Apps_PackageInstaller_CPackageInstallerActivity.h"
#include "elastos/apps/packageinstaller/InstallFlowAnalytics.h"
#include "elastos/core/Object.h"

//using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IManifestDigest;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageInstaller;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITabHostOnTabChangeListener;
using Elastos::Core::IRunnable;
using Elastos::Core::Object;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

/*
  * This activity is launched when a new application is installed via side loading
  * The package is first parsed and the user is notified of parse errors via a dialog.
  * If the package is successfully parsed, the user is notified to turn on the install unknown
  * applications setting. A memory check is made at this point and the user is notified of out
  * of memory conditions if any. If the package is already existing on the device,
  * a confirmation dialog (to replace the existing package) is presented to the user.
  * Based on the user response the package is then installed by launching InstallAppConfirm
  * sub activity. All state transitions are handled in this activity
  */
CarClass(CPackageInstallerActivity)
    , public Object // Activity
    , public IDialogInterfaceOnCancelListener
    , public IViewOnClickListener
    , public IPackageInstallerActivity
{
private:
    class InnerOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnClickListener(
            /* [in] */ CPackageInstallerActivity* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CPackageInstallerActivity* mOwner;
    };

    class InnerOnClickListener1
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnClickListener1(
            /* [in] */ CPackageInstallerActivity* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CPackageInstallerActivity* mOwner;
    };

    class InnerOnClickListener2
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnClickListener2(
            /* [in] */ CPackageInstallerActivity* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CPackageInstallerActivity* mOwner;
    };

    class InnerOnClickListener3
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnClickListener3(
            /* [in] */ CPackageInstallerActivity* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CPackageInstallerActivity* mOwner;
    };

    class InnerOnClickListener4
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnClickListener4(
            /* [in] */ CPackageInstallerActivity* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CPackageInstallerActivity* mOwner;
    };

    class InnerOnClickListener5
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnClickListener5(
            /* [in] */ CPackageInstallerActivity* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CPackageInstallerActivity* mOwner;
    };

    class InnerOnClickListener6
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnClickListener6(
            /* [in] */ CPackageInstallerActivity* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CPackageInstallerActivity* mOwner;
    };

    class InnerOnClickListener7
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerOnClickListener7(
            /* [in] */ CPackageInstallerActivity* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CPackageInstallerActivity* mOwner;
    };

    class InnerTabHostOnTabChangeListener
        : public Object
        , public ITabHostOnTabChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerTabHostOnTabChangeListener(
            /* [in] */ CPackageInstallerActivity* owner);

        // @Override
        CARAPI OnTabChanged(
            /* [in] */ const String& tabId);

    private:
        CPackageInstallerActivity* mOwner;
    };

    class InnerRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerRunnable(
            /* [in] */ CPackageInstallerActivity* owner);

        // @Override
        CARAPI Run();

    private:
        CPackageInstallerActivity* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
        /* [in] */ Int32 id,
        /* [in] */ IBundle* bundle);

    virtual CARAPI SetPmResult(
        /* [in] */ Int32 pmResult);

    // @Override
    CARAPI OnBackPressed();

    // Generic handling when pressing back key
    virtual CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

    virtual CARAPI OnClick(
        /* [in] */ IView* v);

protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

private:
    CARAPI_(void) StartInstallConfirm();

    CARAPI_(void) ShowDialogInner(
        /* [in] */ Int32 id);

    CARAPI_(void) LaunchSettingsAppAndFinish();

    CARAPI_(Boolean) IsInstallingUnknownAppsAllowed();

    CARAPI_(Boolean) IsInstallRequestFromUnknownSource(
        /* [in] */ IIntent* intent);

    CARAPI_(Boolean) IsVerifyAppsEnabled();

    CARAPI_(Boolean) IsAppVerifierInstalled();

    CARAPI_(void) InitiateInstall();

    /** Get the ApplicationInfo for the calling package, if available */
    CARAPI_(AutoPtr<IApplicationInfo>) GetSourceInfo();

    /** Get the originating uid if possible, or VerificationParams.NO_UID if not available */
    CARAPI_(Int32) GetOriginatingUid(
        /* [in] */ IIntent* intent);

public:
    AutoPtr<IPackageManager> mPm;
    AutoPtr<IPackageInstaller> mInstaller;
    AutoPtr<IPackageInfo> mPkgInfo;
    AutoPtr<IApplicationInfo> mSourceInfo;
    // View for install progress
    AutoPtr<IView> mInstallConfirm;
    //AutoPtr<CaffeinatedScrollView> mScrollView;
    static const String PREFS_ALLOWED_SOURCES;

private:
    static const String TAG;
    Int32 mSessionId;
    AutoPtr<IUri> mPackageURI;
    AutoPtr<IUri> mOriginatingURI;
    AutoPtr<IUri> mReferrerURI;
    Int32 mOriginatingUid;
    AutoPtr<IManifestDigest> mPkgDigest;
    Boolean localLOGV;
    // ApplicationInfo object primarily used for already existing applications
    AutoPtr<IApplicationInfo> mAppInfo;
    AutoPtr<InstallFlowAnalytics> mInstallFlowAnalytics;
    // Buttons to indicate user acceptance
    AutoPtr<IButton> mOk;
    AutoPtr<IButton> mCancel;
    Boolean mOkCanInstall;
    static const String PACKAGE_MIME_TYPE;
    static const String TAB_ID_ALL;
    static const String TAB_ID_NEW;
    // Dialog identifiers used in showDialog
    static const Int32 DLG_BASE = 0;
    static const Int32 DLG_UNKNOWN_APPS = DLG_BASE + 1;
    static const Int32 DLG_PACKAGE_ERROR = DLG_BASE + 2;
    static const Int32 DLG_OUT_OF_SPACE = DLG_BASE + 3;
    static const Int32 DLG_INSTALL_ERROR = DLG_BASE + 4;
    static const Int32 DLG_ALLOW_SOURCE = DLG_BASE + 5;
};

} // namespace PackageInstaller
} // namespace Apps
} // namespace Elastos

#endif // __ELASTOS_APPS_PACKAGEINSTALLER_CPACKAGEINSTALLERACTIVITY_H__

