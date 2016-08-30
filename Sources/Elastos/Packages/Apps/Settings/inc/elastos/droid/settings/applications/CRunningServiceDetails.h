#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_CRUNNINGSERVICEDETAILS_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_CRUNNINGSERVICEDETAILS_H__

#include "_Elastos_Droid_Settings_Applications_CRunningServiceDetails.h"
#include "elastos/droid/settings/applications/CRunningProcessesView.h"
#include "elastos/droid/app/DialogFragment.h"
#include "elastos/droid/app/Fragment.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::DialogFragment;
using Elastos::Droid::App::Fragment;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::IArrayList;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

CarClass(CRunningServiceDetails)
    , public Fragment
    , public IRunningStateOnRefreshUiListener
{
public:
    class MyAlertDialogFragment
        : public DialogFragment
    {
    public:
        TO_STRING_IMPL("CRunningServiceDetails::MyAlertDialogFragment")

        MyAlertDialogFragment();

        virtual ~MyAlertDialogFragment();

        CARAPI constructor();

        static CARAPI_(AutoPtr<IDialogFragment>) /*MyAlertDialogFragment*/ NewConfirmStop(
            /* [in] */ Int32 id,
            /* [in] */ IComponentName* comp);

        CARAPI_(AutoPtr<CRunningServiceDetails>) GetOwner();

        //@Override
        CARAPI OnCreateDialog(
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IDialog** result);
    };

protected:
    class ActiveDetail
        : public Object
        , public IViewOnClickListener
    {
        friend class CRunningServiceDetails;
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CRunningServiceDetails::ActiveDetail")

        ActiveDetail(
            /* [in] */ CRunningServiceDetails* host);

        ~ActiveDetail();

        CARAPI_(void) StopActiveService(
            /* [in] */ Boolean confirmed);

        CARAPI OnClick(
            /* [in] */ IView* v);

    protected:
        AutoPtr<IView> mRootView;
        AutoPtr<IButton> mStopButton;
        AutoPtr<IButton> mReportButton;
        AutoPtr<RunningState::ServiceItem> mServiceItem;
        AutoPtr<CRunningProcessesView::ActiveItem> mActiveItem;
        AutoPtr<CRunningProcessesView::ViewHolder> mViewHolder;
        AutoPtr<IPendingIntent> mManageIntent;
        AutoPtr<IComponentName> mInstaller;

    private:
        CRunningServiceDetails* mHost;
    };

private:
    class MyOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CRunningServiceDetails::MyOnClickListener")

        MyOnClickListener(
            /* [in] */ MyAlertDialogFragment* host,
            /* [in] */ IComponentName* comp);

        ~MyOnClickListener();

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        MyAlertDialogFragment* mHost;
        AutoPtr<IComponentName> mComp;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CRunningServiceDetails::MyRunnable");

        MyRunnable(
            /* [in] */ CRunningServiceDetails* host);

        ~MyRunnable();

        //@Override
        CARAPI Run();

    private:
        CRunningServiceDetails* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    TO_STRING_IMPL("CRunningServiceDetails")

    CRunningServiceDetails();

    ~CRunningServiceDetails();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnRefreshUi(
        /* [in] */ Int32 what);

protected:
    virtual CARAPI_(Boolean) FindMergedItem();

    virtual CARAPI_(void) AddServicesHeader();

    virtual CARAPI_(void) AddProcessesHeader();

    virtual CARAPI_(void) AddServiceDetailsView(
        /* [in] */ RunningState::ServiceItem* si,
        /* [in] */ RunningState::MergedItem* mi,
        /* [in] */ Boolean isService,
        /* [in] */ Boolean inclDetails);

    virtual CARAPI_(void) AddProcessDetailsView(
        /* [in] */ RunningState::ProcessItem* pi,
        /* [in] */ Boolean isMain);

    virtual CARAPI_(void) AddDetailsViews(
        /* [in] */ RunningState::MergedItem* item,
        /* [in] */ Boolean inclServices,
        /* [in] */ Boolean inclProcesses);

    virtual CARAPI_(void) AddDetailViews();

    virtual CARAPI_(void) RefreshUi(
        /* [in] */ Boolean dataChanged);

    virtual CARAPI_(AutoPtr<ActiveDetail>) ActiveDetailForService(
        /* [in] */ IComponentName* comp);

    virtual CARAPI_(void) EnsureData();

    virtual CARAPI_(void) UpdateTimes();

private:
    CARAPI_(void) Finish();

    CARAPI_(void) ShowConfirmStopDialog(
        /* [in] */ IComponentName* comp);

protected:
    static const String TAG;

    static const String KEY_UID;
    static const String KEY_USER_ID;
    static const String KEY_PROCESS;
    static const String KEY_BACKGROUND;

    static const Int32 DIALOG_CONFIRM_STOP;

    AutoPtr<IActivityManager> mAm;
    AutoPtr<ILayoutInflater> mInflater;

    AutoPtr<RunningState> mState;
    Boolean mHaveData;

    Int32 mUid;
    Int32 mUserId;
    String mProcessName;
    Boolean mShowBackground;

    AutoPtr<RunningState::MergedItem> mMergedItem;

    AutoPtr<IView> mRootView;
    AutoPtr<IViewGroup> mAllDetails;
    AutoPtr<IViewGroup> mSnippet;
    AutoPtr<CRunningProcessesView::ActiveItem> mSnippetActiveItem;
    AutoPtr<CRunningProcessesView::ViewHolder> mSnippetViewHolder;

    Int32 mNumServices, mNumProcesses;

    AutoPtr<ITextView> mServicesHeader;
    AutoPtr<ITextView> mProcessesHeader;
    // final ArrayList<ActiveDetail> mActiveDetails = new ArrayList<ActiveDetail>();
    AutoPtr<IArrayList> mActiveDetails;

    AutoPtr<StringBuilder> mBuilder;// = new StringBuilder(128);
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_CRUNNINGSERVICEDETAILS_H__