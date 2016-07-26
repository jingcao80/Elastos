
#ifndef __ELASTOS_APPS_PACKAGEINSTALLER_CINSTALLAPPPROGRESS_H__
#define __ELASTOS_APPS_PACKAGEINSTALLER_CINSTALLAPPPROGRESS_H__

#include "_Elastos_Droid_PackageInstaller_CInstallAppProgress.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/packageinstaller/CInstallFlowAnalytics.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageInstallObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

/**
  * This activity corresponds to a download progress screen that is displayed
  * when the user tries
  * to install an application bundled as an apk file. The result of the application install
  * is indicated in the result code that gets set to the corresponding installation status
  * codes defined in PackageManager. If the package being installed already exists,
  * the existing package is replaced with the new one.
  */
CarClass(CInstallAppProgress)
    , public Activity
    , public IViewOnClickListener
    , public IDialogInterfaceOnCancelListener
    , public IInstallAppProgress
{
public:
    class PackageInstallObserver
        : public Object
        , public IIPackageInstallObserver
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IInstallAppProgress* owner);

        CARAPI PackageInstalled(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 returnCode);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        CInstallAppProgress* mOwner;
    };

private:
    class ViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        ViewOnClickListener(
            /* [in] */ CInstallAppProgress* owner)
            : mOwner(owner)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
          /* [in] */ IView* v);

    private:
        CInstallAppProgress* mOwner;
    };

    class InnerHandler : public Handler
    {
    public:
        InnerHandler(
            /* [in] */ CInstallAppProgress* owner);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        CInstallAppProgress* mOwner;
    };

    class InnerStartActivityOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerStartActivityOnClickListener(
            /* [in] */ CInstallAppProgress* owner);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CInstallAppProgress* mOwner;
    };

    class InnerLogOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerLogOnClickListener(
            /* [in] */ CInstallAppProgress* owner);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CInstallAppProgress* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CInstallAppProgress();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    // @Override
    CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
        /* [in] */ Int32 id,
        /* [in] */ IBundle* bundle);

    virtual CARAPI InitView();

    virtual CARAPI OnClick(
        /* [in] */ IView* v);

    virtual CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

protected:
    // @Override
    CARAPI OnDestroy();

private:
    CARAPI_(Int32) GetExplanationFromErrorCode(
        /* [in] */ Int32 errCode);

    /**
     *
     *xxxxx
     *xxxxx
     *
     ************************************
     *
     *ActionsCode(author:liuxinxu, change_code)
     */
    CARAPI_(void) ShowDialogInner(
        /* [in] */ Int32 id);

private:
    /*const*/ String TAG;
    Boolean localLOGV;
    AutoPtr<IApplicationInfo> mAppInfo;
    AutoPtr<IUri> mPackageURI;
    AutoPtr<CInstallFlowAnalytics> mInstallFlowAnalytics;
    AutoPtr<IProgressBar> mProgressBar;
    AutoPtr<IView> mOkPanel;
    AutoPtr<ITextView> mStatusTextView;
    AutoPtr<ITextView> mExplanationTextView;
    AutoPtr<IButton> mDoneButton;
    AutoPtr<IButton> mLaunchButton;
    static const Int32 INSTALL_COMPLETE = 1;
    AutoPtr<IIntent> mLaunchIntent;
    static const Int32 DLG_OUT_OF_SPACE = 1;
    AutoPtr<ICharSequence> mLabel;
    AutoPtr<IHandler> mHandler;
};

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_APPS_PACKAGEINSTALLER_CINSTALLAPPPROGRESS_H__

