//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_CINSTALLEDAPPDETAILS_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_CINSTALLEDAPPDETAILS_H__

#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos_Droid_Settings_Applications_CInstalledAppDetails.h"
#include "elastos/droid/settings/applications/ApplicationsState.h"
#include "elastos/droid/app/DialogFragment.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::DialogFragment;
using Elastos::Droid::App::Fragment;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageDataObserver;
using Elastos::Droid::Content::Pm::IIPackageMoveObserver;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Hardware::Usb::IIUsbManager;
using Elastos::Droid::Internal::Telephony::IISms;
using Elastos::Droid::Internal::Telephony::ISmsUsageMonitor;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

/**
 * Activity to display application information from Settings. This activity presents
 * extended information associated with a package like code, data, total size, permissions
 * used by the application and also the set of default launchable activities.
 * For system applications, an option to clear user data is displayed only if data size is > 0.
 * System applications that do not want clear user data do not have this option.
 * For non-system applications, there is no option to clear data. Instead there is an option to
 * uninstall the application.
 */
CarClass(CInstalledAppDetails)
    , public Fragment
{
public:
    class InnerListener
        : public Object
        , public IViewOnClickListener
        , public ICompoundButtonOnCheckedChangeListener
        , public IApplicationsStateCallbacks
    {
    public:
        CAR_INTERFACE_DECL();

        TO_STRING_IMPL("CInstalledAppDetails::InnerListener")

        InnerListener(
            /* [in] */ CInstalledAppDetails* host);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

        //@Override
        CARAPI OnCheckedChanged(
            /* [in] */ ICompoundButton* buttonView,
            /* [in] */ Boolean isChecked);

        //@Override
        CARAPI OnRunningStateChanged(
            /* [in] */ Boolean running);

        //@Override
        CARAPI OnPackageListChanged();

        //@Override
        CARAPI OnRebuildComplete(
            /* [in] */ IArrayList* apps);

        //@Override
        CARAPI OnPackageIconChanged();

        //@Override
        CARAPI OnPackageSizeChanged(
            /* [in] */ const String& packageName);

        //@Override
        CARAPI OnAllSizesComputed();

    private:
        CInstalledAppDetails* mHost;
    };

    class MyAlertDialogFragment
        : public DialogFragment
    {
    public:
        TO_STRING_IMPL("CInstalledAppDetails::MyAlertDialogFragment")

        MyAlertDialogFragment();

        virtual ~MyAlertDialogFragment();

        CARAPI constructor();

        static CARAPI_(AutoPtr<IDialogFragment>) /*MyAlertDialogFragment*/ NewInstance(
            /* [in] */ Int32 id,
            /* [in] */ Int32 moveErrorCode);

        CARAPI_(AutoPtr<CInstalledAppDetails>) GetOwner();

        //@Override
        CARAPI OnCreateDialog(
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IDialog** result);
    };

    class ClearUserDataObserver
        : public Object
        , public IIPackageDataObserver
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CInstalledAppDetails::ClearUserDataObserver")

        ClearUserDataObserver();

        CARAPI constructor(
            /* [in] */ IFragment* host);

        ~ClearUserDataObserver();

        CARAPI OnRemoveCompleted(
            /* [in] */ const String& packageName,
            /* [in] */ Boolean succeeded);

    private:
        CInstalledAppDetails* mHost;
    };

    class ClearCacheObserver
        : public Object
        , public IIPackageDataObserver
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CInstalledAppDetails::ClearCacheObserver")

        ClearCacheObserver();

        CARAPI constructor(
            /* [in] */ IFragment* host);

        ~ClearCacheObserver();

        CARAPI OnRemoveCompleted(
            /* [in] */ const String& packageName,
            /* [in] */ Boolean succeeded);

    private:
        CInstalledAppDetails* mHost;
    };

    class PackageMoveObserver
        : public Object
        , public IIPackageMoveObserver
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CInstalledAppDetails::PackageMoveObserver")

        PackageMoveObserver();

        CARAPI constructor(
            /* [in] */ IFragment* host);

        ~PackageMoveObserver();

        CARAPI PackageMoved(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 returnCode);

    private:
        CInstalledAppDetails* mHost;
    };

protected:
    class DisableChanger
        //<Object, Object, Object>
        : public AsyncTask
    {
    public:
        TO_STRING_IMPL("CInstalledAppDetails::DisableChanger")

        DisableChanger(
            /* [in] */ CInstalledAppDetails* activity,
            /* [in] */ IApplicationInfo* info,
            /* [in] */ Int32 state);

        ~DisableChanger();

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

    protected:
        AutoPtr<IPackageManager> mPm;
        // final WeakReference<InstalledAppDetails> mActivity;
        AutoPtr<IWeakReference> mActivity;
        AutoPtr<IApplicationInfo> mInfo;
        Int32 mState;
    };

private:
    class InitHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CInstalledAppDetails::InitHandler")

        InitHandler(
            /* [in] */ CInstalledAppDetails* host);

        ~InitHandler();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CInstalledAppDetails* mHost;
    };


    class PremiumSmsSelectionListener
        : public Object
        , public IAdapterViewOnItemSelectedListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CInstalledAppDetails::PremiumSmsSelectionListener")

        PremiumSmsSelectionListener(
            /* [in] */ const String& packageName,
            /* [in] */ IISms* smsManager);

        //@Override
        CARAPI OnItemSelected(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        //@Override
        CARAPI OnNothingSelected(
            /* [in] */ IAdapterView* parent);

    private:
        CARAPI_(void) SetPremiumSmsPermission(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 permission);

    private:
        String mPackageName;
        AutoPtr<IISms> mSmsManager;
    };

    class InitBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CInstalledAppDetails::InitBroadcastReceiver")

        InitBroadcastReceiver(
            /* [in] */ CInstalledAppDetails* host);

        ~InitBroadcastReceiver();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CInstalledAppDetails* mHost;
    };

    class MyOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CInstalledAppDetails::MyOnClickListener")

        MyOnClickListener(
            /* [in] */ MyAlertDialogFragment* host,
            /* [in] */ Int32 id);

        ~MyOnClickListener();

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        MyAlertDialogFragment* mHost;
        Int32 mId;
    };

public:
    CAR_OBJECT_DECL()

    TO_STRING_IMPL("CInstalledAppDetails")

    CInstalledAppDetails();

    ~CInstalledAppDetails();

    CARAPI constructor();

    /** Called when the activity is first created. */
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuInflater* inflater);

    //@Override
    CARAPI OnPrepareOptionsMenu(
        /* [in] */ IMenu* menu);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnDestroyView();

    //@Override
    CARAPI OnAllSizesComputed();

    //@Override
    CARAPI OnPackageIconChanged();

    //@Override
    CARAPI OnPackageListChanged();

    //@Override
    CARAPI OnRebuildComplete(
        /* [in] */ IArrayList* apps); /*ArrayList<AppEntry>*/

    //@Override
    CARAPI OnPackageSizeChanged(
        /* [in] */ const String& packageName);

    //@Override
    CARAPI OnRunningStateChanged(
        /* [in] */ Boolean running);

    /*
     * Method implementing functionality of buttons clicked
     * @see android.view.View.OnClickListener#OnClick(android.view.View)
     */
    CARAPI OnClick(
        /* [in] */ IView* v);

    //@Override
    CARAPI OnCheckedChanged(
        /* [in] */ ICompoundButton* buttonView,
        /* [in] */ Boolean isChecked);

private:
    CARAPI_(String) GetSizeStr(
        /* [in] */ Int64 size);

    CARAPI_(void) InitDataButtons();

    CARAPI_(AutoPtr<ICharSequence>) GetMoveErrMsg(
        /* [in] */ Int32 errCode);

    CARAPI_(void) InitMoveButton();

    CARAPI_(Boolean) HandleDisableable(
        /* [in] */ IButton* button);

    CARAPI_(void) InitUninstallButtons();

    CARAPI_(void) InitNotificationButton();

    // Utility method to set application label and icon.
    CARAPI_(void) SetAppLabelAndIcon(
        /* [in] */ IPackageInfo* pkgInfo);

    CARAPI_(String) RetrieveAppEntry();

    CARAPI_(Boolean) SignaturesMatch(
        /* [in] */ const String& pkg1,
        /* [in] */ const String& pkg2);

    CARAPI_(Boolean) RefreshUi();

    CARAPI_(void) ResetLaunchDefaultsUi(
        /* [in] */ ITextView* title,
        /* [in] */ ITextView* autoLaunchView);

    CARAPI_(void) SetIntentAndFinish(
        /* [in] */ Boolean finish,
        /* [in] */ Boolean appChanged);

    CARAPI_(void) RefreshSizeInfo();

    /*
     * Private method to handle clear message notification from observer when
     * the async operation from PackageManager is complete
     */
    CARAPI_(void) ProcessClearMsg(
        /* [in] */ IMessage* msg);

    CARAPI_(void) RefreshButtons();

    CARAPI_(void) ProcessMoveMsg(
        /* [in] */ IMessage* msg);

    /*
     * Private method to initiate clearing user data when the user clicks the clear data
     * button for a system package
     */
    CARAPI_(void) InitiateClearUserData();

    CARAPI_(void) ShowDialogInner(
        /* [in] */ Int32 id,
        /* [in] */ Int32 moveErrorCode);

    CARAPI_(void) UninstallPkg(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean allUsers,
        /* [in] */ Boolean andDisable);

    CARAPI_(void) ForceStopPackage(
        /* [in] */ const String& pkgName);

    CARAPI_(void) UpdateForceStopButton(
        /* [in] */ Boolean enabled);

    CARAPI_(void) CheckForceStop();

    CARAPI_(void) SetNotificationsEnabled(
        /* [in] */ Boolean enabled);

    CARAPI_(Int32) GetPremiumSmsPermission(
        /* [in] */ const String& packageName);

public:
    static const String ARG_PACKAGE_NAME;

    // Menu identifiers
    static const Int32 UNINSTALL_ALL_USERS_MENU;

    // Result code identifiers
    static const Int32 REQUEST_UNINSTALL;
    static const Int32 REQUEST_MANAGE_SPACE;

private:
    static const String TAG;
    static const Boolean localLOGV;

    //internal constants used in Handler
    static const Int32 OP_SUCCESSFUL;
    static const Int32 OP_FAILED;
    static const Int32 CLEAR_USER_DATA;
    static const Int32 CLEAR_CACHE;
    static const Int32 PACKAGE_MOVE;

    // invalid size value used initially and also when size retrieval through PackageManager
    // fails for whatever reason
    static const Int32 SIZE_INVALID;

    // Dialog identifiers used in showDialog
    static const Int32 DLG_BASE;
    static const Int32 DLG_CLEAR_DATA;
    static const Int32 DLG_FACTORY_RESET;
    static const Int32 DLG_APP_NOT_FOUND;
    static const Int32 DLG_CANNOT_CLEAR_DATA;
    static const Int32 DLG_FORCE_STOP;
    static const Int32 DLG_MOVE_FAILED;
    static const Int32 DLG_DISABLE;
    static const Int32 DLG_DISABLE_NOTIFICATIONS;
    static const Int32 DLG_SPECIAL_DISABLE;

    AutoPtr<IPackageManager> mPm;
    AutoPtr<IUserManager> mUserManager;
    // TODO
    // AutoPtr<IIUsbManager> mUsbManager;
    AutoPtr<IAppWidgetManager> mAppWidgetManager;
    AutoPtr<IDevicePolicyManager> mDpm;
    AutoPtr<IISms> mSmsManager;
    AutoPtr<ApplicationsState> mState;
    AutoPtr<ApplicationsState::Session> mSession;
    AutoPtr<ApplicationsState::AppEntry> mAppEntry;
    Boolean mInitialized;
    Boolean mShowUninstalled;
    AutoPtr<IPackageInfo> mPackageInfo;
    AutoPtr<CanBeOnSdCardChecker> mCanBeOnSdCardChecker;
    AutoPtr<IView> mRootView;
    AutoPtr<IButton> mUninstallButton;
    AutoPtr<IView> mMoreControlButtons;
    AutoPtr<IButton> mSpecialDisableButton;
    Boolean mMoveInProgress;
    Boolean mUpdatedSysApp;
    AutoPtr<IButton> mActivitiesButton;
    AutoPtr<IView> mScreenCompatSection;
    AutoPtr<ICheckBox> mAskCompatibilityCB;
    AutoPtr<ICheckBox> mEnableCompatibilityCB;
    Boolean mCanClearData;
    Boolean mAppControlRestricted;
    AutoPtr<ITextView> mAppVersion;
    AutoPtr<ITextView> mTotalSize;
    AutoPtr<ITextView> mAppSize;
    AutoPtr<ITextView> mDataSize;
    AutoPtr<ITextView> mExternalCodeSize;
    AutoPtr<ITextView> mExternalDataSize;
    AutoPtr<IIPackageDataObserver> /*ClearUserDataObserver*/ mClearDataObserver;
    // Views related to cache info
    AutoPtr<ITextView> mCacheSize;
    AutoPtr<IButton> mClearCacheButton;
    AutoPtr<IIPackageDataObserver> /*ClearCacheObserver*/ mClearCacheObserver;
    AutoPtr<IButton> mForceStopButton;
    AutoPtr<IButton> mClearDataButton;
    AutoPtr<IButton> mMoveAppButton;
    AutoPtr<ICompoundButton> mNotificationSwitch;

    AutoPtr<IIPackageMoveObserver>/*PackageMoveObserver*/ mPackageMoveObserver;

    // final HashSet<String> mHomePackages = new HashSet<String>();
    AutoPtr<IHashSet> mHomePackages;

    Boolean mDisableAfterUninstall;

    Boolean mHaveSizes;
    Int64 mLastCodeSize;
    Int64 mLastDataSize;
    Int64 mLastExternalCodeSize;
    Int64 mLastExternalDataSize;
    Int64 mLastCacheSize;
    Int64 mLastTotalSize;

    // Resource strings
    AutoPtr<ICharSequence> mInvalidSizeStr;
    AutoPtr<ICharSequence> mComputingStr;

    AutoPtr<Handler> mHandler;

    AutoPtr<BroadcastReceiver> mCheckKillProcessesReceiver;

    AutoPtr<InnerListener> mListener;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_CINSTALLEDAPPDETAILS_H__