
#ifndef __ELASTOS_APPS_PACKAGEINSTALLER_CUNINSTALLERACTIVITY_H__
#define __ELASTOS_APPS_PACKAGEINSTALLER_CUNINSTALLERACTIVITY_H__

#include "elastos/droid/app/Activity.h"
#include "elastos/droid/app/DialogFragment.h"
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include "_Elastos_Apps_PackageInstaller_CUninstallerActivity.h"
#include "elastos/core/Object.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Core::Object;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

/*
  * This activity presents UI to uninstall an application. Usually launched with intent
  * Intent.ACTION_UNINSTALL_PKG_COMMAND and attribute
  * com.android.packageinstaller.PackageName set to the application package name
  */
CarClass(CUninstallerActivity)
    , public Activity
    , public IUninstallerActivity
{
public:
    class UninstallAlertDialogFragment
        : public DialogFragment
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnCreateDialog(
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IDialog** result);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

        // @Override
        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);
    };

    class AppNotFoundDialogFragment
        : public Object // DialogFragment
    {
    public:
        // @Override
        CARAPI OnCreateDialog(
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IDialog** result);

        // @Override
        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);
    };

    class DialogInfo
        : public Object
    {
    public:
        AutoPtr<IApplicationInfo> appInfo;
        AutoPtr<IActivityInfo> activityInfo;
        Boolean allUsers;
        AutoPtr<IUserHandle> user;
        AutoPtr<IBinder> callback;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CUninstallerActivity();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    virtual CARAPI StartUninstallProgress();

    virtual CARAPI DispatchAborted();

private:
    CARAPI_(void) ShowConfirmationDialog();

    CARAPI_(void) ShowAppNotFound();

    CARAPI_(void) ShowDialogFragment(
        /* [in] */ IDialogFragment* fragment);

private:
    static const String TAG;
    AutoPtr<DialogInfo> mDialogInfo;
};

} // namespace PackageInstaller
} // namespace Apps
} // namespace Elastos

#endif // __ELASTOS_APPS_PACKAGEINSTALLER_CUNINSTALLERACTIVITY_H__

