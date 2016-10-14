#ifndef __ELASTOS_DROID_SETTINGS_CDEVICEADMINADD_H__
#define __ELASTOS_DROID_SETTINGS_CDEVICEADMINADD_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Droid_Settings_CDeviceAdminAdd.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/os/RemoteCallback.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Admin::IDeviceAdminInfo;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::RemoteCallback;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CDeviceAdminAdd)
    , public Activity
{
private:
    class ViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CDeviceAdminAdd::ViewOnClickListener");

        CAR_INTERFACE_DECL()

        ViewOnClickListener(
            /* [in] */ CDeviceAdminAdd* host,
            /* [in] */ Int32 id);

        ~ViewOnClickListener();

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CDeviceAdminAdd* mHost;
        Int32 mId;
    };

    class MyRemoteCallback
        : public RemoteCallback
    {
    public:
        TO_STRING_IMPL("CDeviceAdminAdd::MyRemoteCallback")

        MyRemoteCallback();

        CARAPI constructor(
            /* [in] */ IHandler* handler,
            /* [in] */ CDeviceAdminAdd* host);

    protected:
        //@Override
        CARAPI_(void) OnResult(
            /* [in] */ IBundle* bundle);

    private:
        CDeviceAdminAdd* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CDeviceAdminAdd::MyRunnable")

        MyRunnable(
            /* [in] */ CDeviceAdminAdd* host);

        //@Override
        CARAPI Run();

    private:
        CDeviceAdminAdd* mHost;
    };

    class DialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CDeviceAdminAdd::DialogInterfaceOnClickListener")

        CAR_INTERFACE_DECL()

        DialogInterfaceOnClickListener(
            /* [in] */ CDeviceAdminAdd* host);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CDeviceAdminAdd* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CDeviceAdminAdd();

    virtual ~CDeviceAdminAdd();

    CARAPI constructor();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI_(AutoPtr<IDialog>) OnCreateDialog(
        /* [in] */ Int32 id,
        /* [in] */ IBundle* args);

    virtual CARAPI_(void) AddAndFinish();

    virtual CARAPI_(void) ContinueRemoveAction(
        /* [in] */ ICharSequence* msg);

    static CARAPI_(void) SetViewVisibility(
        /* [in] */ IArrayList* views, //ArrayList<View>
        /* [in] */ Int32 visibility);

    virtual CARAPI_(void) UpdateInterface();

    virtual CARAPI_(void) ToggleMessageEllipsis(
        /* [in] */ IView* v);

    virtual CARAPI_(Int32) GetEllipsizedLines();

protected:
    static const String TAG;// = "DeviceAdminAdd";

    static const Int32 DIALOG_WARNING;// = 1;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IDevicePolicyManager> mDPM;
    AutoPtr<IAppOpsManager> mAppOps;
    AutoPtr<IDeviceAdminInfo> mDeviceAdmin;
    AutoPtr<ICharSequence> mAddMsgText;
    String mProfileOwnerName;

    AutoPtr<IImageView> mAdminIcon;
    AutoPtr<ITextView> mAdminName;
    AutoPtr<ITextView> mAdminDescription;
    AutoPtr<ITextView> mAddMsg;
    AutoPtr<ITextView> mProfileOwnerWarning;
    AutoPtr<IImageView> mAddMsgExpander;
    Boolean mAddMsgEllipsized;// = TRUE;
    AutoPtr<ITextView> mAdminWarning;
    AutoPtr<IViewGroup> mAdminPolicies;
    AutoPtr<IButton> mActionButton;
    AutoPtr<IButton> mCancelButton;

    // final ArrayList<View> mAddingPolicies = new ArrayList<View>();
    AutoPtr<IArrayList> mAddingPolicies;
    // final ArrayList<View> mActivePolicies = new ArrayList<View>();
    AutoPtr<IArrayList> mActivePolicies;

    Boolean mAdding;
    Boolean mRefreshing;
    Boolean mWaitingForRemoveMsg;
    Boolean mAddingProfileOwner;
    Int32 mCurSysAppOpMode;
    Int32 mCurToastAppOpMode;

private:
    static const Int32 MAX_ADD_MSG_LINES_PORTRAIT;// = 5;
    static const Int32 MAX_ADD_MSG_LINES_LANDSCAPE;// = 2;
    static const Int32 MAX_ADD_MSG_LINES;// = 15;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CDEVICEADMINADD_H__