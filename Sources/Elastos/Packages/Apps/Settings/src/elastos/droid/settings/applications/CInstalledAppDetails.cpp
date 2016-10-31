
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Text.h"
#include "elastos/droid/settings/applications/CInstalledAppDetails.h"
#include "elastos/droid/settings/applications/CInstalledAppDetailsAlertDialogFragment.h"
#include "elastos/droid/settings/applications/CInstalledAppDetailsClearCacheObserver.h"
#include "elastos/droid/settings/applications/CInstalledAppDetailsClearUserDataObserver.h"
#include "elastos/droid/settings/applications/CInstalledAppDetailsPackageMoveObserver.h"
#include "elastos/droid/settings/applications/ManageApplications.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/format/Formatter.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/widget/ArrayAdapter.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IApplication;
using Elastos::Droid::AppWidget::IAppWidgetManagerHelper;
using Elastos::Droid::AppWidget::CAppWidgetManagerHelper;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::EIID_IIPackageDataObserver;
using Elastos::Droid::Content::Pm::EIID_IIPackageMoveObserver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::CSpannableString;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Format::Formatter;
using Elastos::Droid::Text::Style::CBulletSpan;
using Elastos::Droid::Text::Style::IBulletSpan;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::Widget::CAppSecurityPermissions;
using Elastos::Droid::Widget::IAppSecurityPermissions;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

const String CInstalledAppDetails::ARG_PACKAGE_NAME("package");

const Int32 CInstalledAppDetails::UNINSTALL_ALL_USERS_MENU = 1;

const Int32 CInstalledAppDetails::REQUEST_UNINSTALL = 1;
const Int32 CInstalledAppDetails::REQUEST_MANAGE_SPACE = 2;

const String CInstalledAppDetails::TAG("CInstalledAppDetails");
const Boolean CInstalledAppDetails::localLOGV = FALSE;

const Int32 CInstalledAppDetails::OP_SUCCESSFUL = 1;
const Int32 CInstalledAppDetails::OP_FAILED = 2;
const Int32 CInstalledAppDetails::CLEAR_USER_DATA = 1;
const Int32 CInstalledAppDetails::CLEAR_CACHE = 3;
const Int32 CInstalledAppDetails::PACKAGE_MOVE = 4;

const Int32 CInstalledAppDetails::SIZE_INVALID = -1;

const Int32 CInstalledAppDetails::DLG_BASE = 0;
const Int32 CInstalledAppDetails::DLG_CLEAR_DATA = DLG_BASE + 1;
const Int32 CInstalledAppDetails::DLG_FACTORY_RESET = DLG_BASE + 2;
const Int32 CInstalledAppDetails::DLG_APP_NOT_FOUND = DLG_BASE + 3;
const Int32 CInstalledAppDetails::DLG_CANNOT_CLEAR_DATA = DLG_BASE + 4;
const Int32 CInstalledAppDetails::DLG_FORCE_STOP = DLG_BASE + 5;
const Int32 CInstalledAppDetails::DLG_MOVE_FAILED = DLG_BASE + 6;
const Int32 CInstalledAppDetails::DLG_DISABLE = DLG_BASE + 7;
const Int32 CInstalledAppDetails::DLG_DISABLE_NOTIFICATIONS = DLG_BASE + 8;
const Int32 CInstalledAppDetails::DLG_SPECIAL_DISABLE = DLG_BASE + 9;

//===============================================================================
//                  CInstalledAppDetails::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL_3(CInstalledAppDetails::InnerListener, Object, IViewOnClickListener, ICompoundButtonOnCheckedChangeListener, IApplicationsStateCallbacks)

CInstalledAppDetails::InnerListener::InnerListener(
    /* [in] */ CInstalledAppDetails* host)
    : mHost(host)
{}

ECode CInstalledAppDetails::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

ECode CInstalledAppDetails::InnerListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    return mHost->OnCheckedChanged(buttonView, isChecked);
}

ECode CInstalledAppDetails::InnerListener::OnRunningStateChanged(
    /* [in] */ Boolean running)
{
    return mHost->OnRunningStateChanged(running);
}

ECode CInstalledAppDetails::InnerListener::OnPackageListChanged()
{
    return mHost->OnPackageListChanged();
}

ECode CInstalledAppDetails::InnerListener::OnRebuildComplete(
    /* [in] */ IArrayList* apps)
{
    return mHost->OnRebuildComplete(apps);
}

ECode CInstalledAppDetails::InnerListener::OnPackageIconChanged()
{
    return mHost->OnPackageIconChanged();
}

ECode CInstalledAppDetails::InnerListener::OnPackageSizeChanged(
    /* [in] */ const String& packageName)
{
    return mHost->OnPackageSizeChanged(packageName);
}

ECode CInstalledAppDetails::InnerListener::OnAllSizesComputed()
{
    return mHost->OnAllSizesComputed();
}

//===============================================================================
//                  CInstalledAppDetails::MyAlertDialogFragment
//===============================================================================

CInstalledAppDetails::MyAlertDialogFragment::MyAlertDialogFragment()
{}

CInstalledAppDetails::MyAlertDialogFragment::~MyAlertDialogFragment()
{}

ECode CInstalledAppDetails::MyAlertDialogFragment::constructor()
{
    return DialogFragment::constructor();
}

AutoPtr<IDialogFragment> CInstalledAppDetails::MyAlertDialogFragment::NewInstance(
    /* [in] */ Int32 id,
    /* [in] */ Int32 moveErrorCode)
{
    AutoPtr<IDialogFragment> frag;
    CInstalledAppDetailsAlertDialogFragment::New((IDialogFragment**)&frag);
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    args->PutInt32(String("id"), id);
    args->PutInt32(String("moveError"), moveErrorCode);
    IFragment::Probe(frag)->SetArguments(args);
    return frag;
}

AutoPtr<CInstalledAppDetails> CInstalledAppDetails::MyAlertDialogFragment::GetOwner()
{
    AutoPtr<IFragment> frag;
    GetTargetFragment((IFragment**)&frag);
    AutoPtr<CInstalledAppDetails> result = (CInstalledAppDetails*)frag.Get();
    return result;
}

ECode CInstalledAppDetails::MyAlertDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IBundle> bundles;
    GetArguments((IBundle**)&bundles);
    Int32 id;
    bundles->GetInt32(String("id"), &id);
    Int32 moveErrorCode;
    bundles->GetInt32(String("moveError"), &moveErrorCode);

    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(activity, (IAlertDialogBuilder**)&builder);
    AutoPtr<ICharSequence> text;
    switch (id) {
        case DLG_CLEAR_DATA: {
            activity->GetText(R::string::clear_data_dlg_title, (ICharSequence**)&text);
            builder->SetTitle(text);
            text = NULL;
            activity->GetText(R::string::clear_data_dlg_text, (ICharSequence**)&text);
            builder->SetMessage(text);

            AutoPtr<MyOnClickListener> listener = new MyOnClickListener(this, 0);
            builder->SetPositiveButton(R::string::dlg_ok, listener);
            builder->SetNegativeButton(R::string::dlg_cancel, NULL);
            AutoPtr<IAlertDialog> dialog;
            builder->Create((IAlertDialog**)&dialog);
            *result = IDialog::Probe(dialog);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }

        case DLG_FACTORY_RESET: {
            activity->GetText(R::string::app_factory_reset_dlg_title, (ICharSequence**)&text);
            builder->SetTitle(text);
            text = NULL;
            activity->GetText(R::string::app_factory_reset_dlg_text, (ICharSequence**)&text);
            builder->SetMessage(text);

            AutoPtr<MyOnClickListener> listener = new MyOnClickListener(this, 1);
            builder->SetPositiveButton(R::string::dlg_ok, listener);
            builder->SetNegativeButton(R::string::dlg_cancel, NULL);
            AutoPtr<IAlertDialog> dialog;
            builder->Create((IAlertDialog**)&dialog);
            *result = IDialog::Probe(dialog);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }

        case DLG_APP_NOT_FOUND: {
            activity->GetText(R::string::app_not_found_dlg_title, (ICharSequence**)&text);
            builder->SetTitle(text);
            text = NULL;
            activity->GetText(R::string::app_not_found_dlg_text, (ICharSequence**)&text);
            builder->SetMessage(text);

            text = NULL;
            activity->GetText(R::string::dlg_ok, (ICharSequence**)&text);
            AutoPtr<MyOnClickListener> listener = new MyOnClickListener(this, 2);
            builder->SetNeutralButton(text, listener);
            AutoPtr<IAlertDialog> dialog;
            builder->Create((IAlertDialog**)&dialog);
            *result = IDialog::Probe(dialog);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }

        case DLG_CANNOT_CLEAR_DATA: {
            activity->GetText(R::string::clear_failed_dlg_title, (ICharSequence**)&text);
            builder->SetTitle(text);
            text = NULL;
            activity->GetText(R::string::clear_failed_dlg_text, (ICharSequence**)&text);
            builder->SetMessage(text);

            AutoPtr<MyOnClickListener> listener = new MyOnClickListener(this, 3);
            builder->SetNeutralButton(R::string::dlg_ok, listener);
            AutoPtr<IAlertDialog> dialog;
            builder->Create((IAlertDialog**)&dialog);
            *result = IDialog::Probe(dialog);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }

        case DLG_FORCE_STOP: {
            activity->GetText(R::string::force_stop_dlg_title, (ICharSequence**)&text);
            builder->SetTitle(text);
            text = NULL;
            activity->GetText(R::string::force_stop_dlg_text, (ICharSequence**)&text);
            builder->SetMessage(text);

            AutoPtr<MyOnClickListener> listener = new MyOnClickListener(this, 4);
            builder->SetPositiveButton(R::string::dlg_ok, listener);
            builder->SetNegativeButton(R::string::dlg_cancel, NULL);
            AutoPtr<IAlertDialog> dialog;
            builder->Create((IAlertDialog**)&dialog);
            *result = IDialog::Probe(dialog);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }

        case DLG_MOVE_FAILED: {
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, GetOwner()->GetMoveErrMsg(moveErrorCode));
            String str;
            activity->GetString(R::string::move_app_failed_dlg_text, args, &str);
            AutoPtr<ICharSequence> msg = CoreUtils::Convert(str);
            activity->GetText(R::string::move_app_failed_dlg_title, (ICharSequence**)&text);
            builder->SetTitle(text);
            builder->SetMessage(msg);
            builder->SetNeutralButton(R::string::dlg_ok, NULL);
            AutoPtr<IAlertDialog> dialog;
            builder->Create((IAlertDialog**)&dialog);
            *result = IDialog::Probe(dialog);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        case DLG_DISABLE: {
            activity->GetText(R::string::app_disable_dlg_title, (ICharSequence**)&text);
            builder->SetTitle(text);
            text = NULL;
            activity->GetText(R::string::app_disable_dlg_text, (ICharSequence**)&text);
            builder->SetMessage(text);

            AutoPtr<MyOnClickListener> listener = new MyOnClickListener(this, 5);
            builder->SetPositiveButton(R::string::dlg_ok, listener);
            builder->SetNegativeButton(R::string::dlg_cancel, NULL);
            AutoPtr<IAlertDialog> dialog;
            builder->Create((IAlertDialog**)&dialog);
            *result = IDialog::Probe(dialog);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }

        case DLG_DISABLE_NOTIFICATIONS: {
            activity->GetText(R::string::app_disable_notifications_dlg_title, (ICharSequence**)&text);
            builder->SetTitle(text);
            text = NULL;
            activity->GetText(R::string::app_disable_notifications_dlg_text, (ICharSequence**)&text);
            builder->SetMessage(text);

            AutoPtr<MyOnClickListener> listener1 = new MyOnClickListener(this, 6);
            builder->SetPositiveButton(R::string::dlg_ok, listener1);
            AutoPtr<MyOnClickListener> listener2 = new MyOnClickListener(this, 7);
            builder->SetNegativeButton(R::string::dlg_cancel, listener2);
            AutoPtr<IAlertDialog> dialog;
            builder->Create((IAlertDialog**)&dialog);
            *result = IDialog::Probe(dialog);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }

        case DLG_SPECIAL_DISABLE: {
            activity->GetText(R::string::app_special_disable_dlg_title, (ICharSequence**)&text);
            builder->SetTitle(text);
            text = NULL;
            activity->GetText(R::string::app_special_disable_dlg_text, (ICharSequence**)&text);
            builder->SetMessage(text);

            AutoPtr<MyOnClickListener> listener = new MyOnClickListener(this, 8);
            builder->SetPositiveButton(R::string::dlg_ok, listener);
            builder->SetNegativeButton(R::string::dlg_cancel, NULL);
            AutoPtr<IAlertDialog> dialog;
            builder->Create((IAlertDialog**)&dialog);
            *result = IDialog::Probe(dialog);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }

    Logger::E(TAG, "unknown id %d", id);
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

//===============================================================================
//                  CInstalledAppDetails::ClearUserDataObserver
//===============================================================================

CAR_INTERFACE_IMPL_2(CInstalledAppDetails::ClearUserDataObserver, Object, IIPackageDataObserver, IBinder)

CInstalledAppDetails::ClearUserDataObserver::ClearUserDataObserver()
{}

ECode CInstalledAppDetails::ClearUserDataObserver::constructor(
    /* [in] */ IFragment* host)
{
    mHost = (CInstalledAppDetails*)host;
    return NOERROR;
}

CInstalledAppDetails::ClearUserDataObserver::~ClearUserDataObserver()
{}

ECode CInstalledAppDetails::ClearUserDataObserver::OnRemoveCompleted(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean succeeded)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(CLEAR_USER_DATA, (IMessage**)&msg);
    msg->SetArg1(succeeded ? OP_SUCCESSFUL : OP_FAILED);
    Boolean res;
    mHost->mHandler->SendMessage(msg, &res);
    return NOERROR;
}

//===============================================================================
//                  CInstalledAppDetails::ClearCacheObserver
//===============================================================================

CAR_INTERFACE_IMPL_2(CInstalledAppDetails::ClearCacheObserver, Object, IIPackageDataObserver, IBinder)

CInstalledAppDetails::ClearCacheObserver::ClearCacheObserver()
{}

ECode CInstalledAppDetails::ClearCacheObserver::constructor(
    /* [in] */ IFragment* host)
{
    mHost = (CInstalledAppDetails*)host;
    return NOERROR;
}

CInstalledAppDetails::ClearCacheObserver::~ClearCacheObserver()
{}

ECode CInstalledAppDetails::ClearCacheObserver::OnRemoveCompleted(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean succeeded)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(CLEAR_CACHE, (IMessage**)&msg);
    msg->SetArg1(succeeded ? OP_SUCCESSFUL : OP_FAILED);
    Boolean res;
    mHost->mHandler->SendMessage(msg, &res);
    return NOERROR;
}

//===============================================================================
//                  CInstalledAppDetails::PackageMoveObserver
//===============================================================================

CAR_INTERFACE_IMPL_2(CInstalledAppDetails::PackageMoveObserver, Object, IIPackageMoveObserver, IBinder)

CInstalledAppDetails::PackageMoveObserver::PackageMoveObserver()
{}

ECode CInstalledAppDetails::PackageMoveObserver::constructor(
    /* [in] */ IFragment* host)
{
    mHost = (CInstalledAppDetails*)host;
    return NOERROR;
}

CInstalledAppDetails::PackageMoveObserver::~PackageMoveObserver()
{}

ECode CInstalledAppDetails::PackageMoveObserver::PackageMoved(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 returnCode)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(PACKAGE_MOVE, (IMessage**)&msg);
    msg->SetArg1(returnCode);
    Boolean res;
    mHost->mHandler->SendMessage(msg, &res);
    return NOERROR;
}

//===============================================================================
//                  CInstalledAppDetails::DisableChanger
//===============================================================================

CInstalledAppDetails::DisableChanger::DisableChanger(
    /* [in] */ CInstalledAppDetails* activity,
    /* [in] */ IApplicationInfo* info,
    /* [in] */ Int32 state)
{
    mPm = activity->mPm;
    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe((IFragment*)activity);
    assert(wrs != NULL);
    mActivity = NULL;
    wrs->GetWeakReference((IWeakReference**)&mActivity);
    mInfo = info;
    mState = state;
}

CInstalledAppDetails::DisableChanger::~DisableChanger()
{}

ECode CInstalledAppDetails::DisableChanger::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    String packageName;
    IPackageItemInfo::Probe(mInfo)->GetPackageName(&packageName);
    mPm->SetApplicationEnabledSetting(packageName, mState, 0);
    return NOERROR;
}

//===============================================================================
//                  CInstalledAppDetails::InitHandler
//===============================================================================

CInstalledAppDetails::InitHandler::InitHandler(
    /* [in] */ CInstalledAppDetails* host)
    : mHost(host)
{}

CInstalledAppDetails::InitHandler::~InitHandler()
{}

ECode CInstalledAppDetails::InitHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // If the fragment is gone, don't process any more messages.
    AutoPtr<IView> view;
    mHost->GetView((IView**)&view);
    if (view == NULL) {
        return NOERROR;
    }
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CLEAR_USER_DATA:
            mHost->ProcessClearMsg(msg);
            break;
        case CLEAR_CACHE: {
            String packageName;
            IPackageItemInfo::Probe(mHost->mAppEntry->mInfo)->GetPackageName(&packageName);
            // Refresh size info
            mHost->mState->RequestSize(packageName);
            break;
        }
        case PACKAGE_MOVE:
            mHost->ProcessMoveMsg(msg);
            break;
        default:
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  CInstalledAppDetails::PremiumSmsSelectionListener
//===============================================================================

CAR_INTERFACE_IMPL(CInstalledAppDetails::PremiumSmsSelectionListener, Object, IAdapterViewOnItemSelectedListener)

CInstalledAppDetails::PremiumSmsSelectionListener::PremiumSmsSelectionListener(
    /* [in] */ const String& packageName,
    /* [in] */ IISms* smsManager)
{
    mPackageName = packageName;
    mSmsManager = smsManager;
}

ECode CInstalledAppDetails::PremiumSmsSelectionListener::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    if (position >= 0 && position < 3) {
        Logger::D(TAG, "Selected premium SMS policy %d", position);
        SetPremiumSmsPermission(mPackageName, (position + 1));
    }
    else {
        Logger::E(TAG, "Error: unknown premium SMS policy %d", position);
    }
    return NOERROR;
}

ECode CInstalledAppDetails::PremiumSmsSelectionListener::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    // Ignored
    return NOERROR;
}

void CInstalledAppDetails::PremiumSmsSelectionListener::SetPremiumSmsPermission(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 permission)
{
    // try {
        if (mSmsManager != NULL) {
            mSmsManager->SetPremiumSmsPermission(packageName, permission);
        }
    // } catch (RemoteException ex) {
    //     // ignored
    // }
}

//===============================================================================
//                  CInstalledAppDetails::InitBroadcastReceiver
//===============================================================================

CInstalledAppDetails::InitBroadcastReceiver::InitBroadcastReceiver(
    /* [in] */ CInstalledAppDetails* host)
    : mHost(host)
{}

CInstalledAppDetails::InitBroadcastReceiver::~InitBroadcastReceiver()
{}

ECode CInstalledAppDetails::InitBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 code;
    GetResultCode(&code);
    mHost->UpdateForceStopButton(code != IActivity::RESULT_CANCELED);
    return NOERROR;
}

//===============================================================================
//                  CInstalledAppDetails::MyOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CInstalledAppDetails::MyOnClickListener, Object, IDialogInterfaceOnClickListener)

CInstalledAppDetails::MyOnClickListener::MyOnClickListener(
    /* [in] */ MyAlertDialogFragment* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

CInstalledAppDetails::MyOnClickListener::~MyOnClickListener()
{}

ECode CInstalledAppDetails::MyOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    switch (mId) {
        case 0:
            // Clear user data here
            mHost->GetOwner()->InitiateClearUserData();
            return NOERROR;
        case 1: {
            // Clear user data here
            String packageName;
            IPackageItemInfo::Probe(mHost->GetOwner()->mAppEntry->mInfo)->GetPackageName(&packageName);
            mHost->GetOwner()->UninstallPkg(packageName, FALSE, FALSE);
            return NOERROR;
        }
        case 2:
            //force to recompute changed value
            mHost->GetOwner()->SetIntentAndFinish(TRUE, TRUE);
            return NOERROR;
        case 3:
            IView::Probe(mHost->GetOwner()->mClearDataButton)->SetEnabled(FALSE);
            //force to recompute changed value
            mHost->GetOwner()->SetIntentAndFinish(FALSE, FALSE);
            return NOERROR;
        case 4: {
            // Force stop
            String packageName;
            IPackageItemInfo::Probe(mHost->GetOwner()->mAppEntry->mInfo)->GetPackageName(&packageName);
            mHost->GetOwner()->ForceStopPackage(packageName);
            return NOERROR;
        }
        case 5: {
            // Disable the app
            AutoPtr<DisableChanger> obj = new DisableChanger(mHost->GetOwner(),
                    mHost->GetOwner()->mAppEntry->mInfo,
                    IPackageManager::COMPONENT_ENABLED_STATE_DISABLED_USER);
            AutoPtr< ArrayOf<IInterface*> > args;
            obj->Execute(args);
            return NOERROR;
        }
        case 6:
            // Disable the package's notifications
            mHost->GetOwner()->SetNotificationsEnabled(FALSE);
            return NOERROR;
        case 7:
            // Re-enable the checkbox
            ICheckable::Probe(mHost->GetOwner()->mNotificationSwitch)->SetChecked(TRUE);
            return NOERROR;
        case 8: {
            // Clear user data here
            String packageName;
            IPackageItemInfo::Probe(mHost->GetOwner()->mAppEntry->mInfo)->GetPackageName(&packageName);
            mHost->GetOwner()->UninstallPkg(packageName, FALSE, TRUE);
            return NOERROR;
        }

    }
    return NOERROR;
}

//===============================================================================
//                  CInstalledAppDetails
//===============================================================================

CAR_OBJECT_IMPL(CInstalledAppDetails)

CInstalledAppDetails::CInstalledAppDetails()
    : mInitialized(FALSE)
    , mShowUninstalled(FALSE)
    , mMoveInProgress(FALSE)
    , mUpdatedSysApp(FALSE)
    , mCanClearData(TRUE)
    , mAppControlRestricted(FALSE)
    , mDisableAfterUninstall(FALSE)
    , mHaveSizes(FALSE)
    , mLastCodeSize(-1)
    , mLastDataSize(-1)
    , mLastExternalCodeSize(-1)
    , mLastExternalDataSize(-1)
    , mLastCacheSize(-1)
    , mLastTotalSize(-1)
{
    CHashSet::New((IHashSet**)&mHomePackages);
    mHandler = new InitHandler(this);
    mHandler->constructor();
    mCheckKillProcessesReceiver = new InitBroadcastReceiver(this);
    mListener = new InnerListener(this);
}

CInstalledAppDetails::~CInstalledAppDetails()
{}

ECode CInstalledAppDetails::constructor()
{
    return Fragment::constructor();
}

String CInstalledAppDetails::GetSizeStr(
    /* [in] */ Int64 size)
{
    if (size == SIZE_INVALID) {
        String str;
        mInvalidSizeStr->ToString(&str);
        return str;
    }
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    return Formatter::FormatFileSize(IContext::Probe(activity), size);
}

void CInstalledAppDetails::InitDataButtons()
{
    // If the app doesn't have its own space management UI
    // And it's a system app that doesn't allow clearing user data or is an active admin
    // Then disable the Clear Data button.
    String manageSpaceActivityName;
    mAppEntry->mInfo->GetManageSpaceActivityName(&manageSpaceActivityName);
    Int32 flags;
    String packageName;
    Boolean res;
    ITextView* clearDataButton1 = ITextView::Probe(mClearDataButton);
    IView* clearDataButton2 = IView::Probe(mClearDataButton);
    if (manageSpaceActivityName.IsNull()
            && (((mAppEntry->mInfo->GetFlags(&flags), flags) & (IApplicationInfo::FLAG_SYSTEM
                    | IApplicationInfo::FLAG_ALLOW_CLEAR_USER_DATA))
                    == IApplicationInfo::FLAG_SYSTEM
                    || (mDpm->PackageHasActiveAdmins((mPackageInfo->GetPackageName(&packageName), packageName), &res), res))) {
        clearDataButton1->SetText(R::string::clear_user_data_text);
        clearDataButton2->SetEnabled(FALSE);
        mCanClearData = FALSE;
    }
    else {
        if (!manageSpaceActivityName.IsNull()) {
            clearDataButton1->SetText(R::string::manage_space_text);
        }
        else {
            clearDataButton1->SetText(R::string::clear_user_data_text);
        }
        clearDataButton2->SetOnClickListener(mListener);
    }

    if (mAppControlRestricted) {
        clearDataButton2->SetEnabled(FALSE);
    }
}

AutoPtr<ICharSequence> CInstalledAppDetails::GetMoveErrMsg(
    /* [in] */ Int32 errCode)
{
    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    String str;
    switch (errCode) {
        case IPackageManager::MOVE_FAILED_INSUFFICIENT_STORAGE:
            activity->GetString(R::string::insufficient_storage, &str);
            return CoreUtils::Convert(str);
        case IPackageManager::MOVE_FAILED_DOESNT_EXIST:
            activity->GetString(R::string::does_not_exist, &str);
            return CoreUtils::Convert(str);
        case IPackageManager::MOVE_FAILED_FORWARD_LOCKED:
            activity->GetString(R::string::app_forward_locked, &str);
            return CoreUtils::Convert(str);
        case IPackageManager::MOVE_FAILED_INVALID_LOCATION:
            activity->GetString(R::string::invalid_location, &str);
            return CoreUtils::Convert(str);
        case IPackageManager::MOVE_FAILED_SYSTEM_PACKAGE:
            activity->GetString(R::string::system_package, &str);
            return CoreUtils::Convert(str);
        case IPackageManager::MOVE_FAILED_INTERNAL_ERROR:
            return CoreUtils::Convert("");
    }
    return CoreUtils::Convert("");
}

void CInstalledAppDetails::InitMoveButton()
{
    AutoPtr<IEnvironment> envir;
    CEnvironment::AcquireSingleton((IEnvironment**)&envir);
    Boolean res;
    IView* moveAppButton1 = IView::Probe(mMoveAppButton);
    ITextView* moveAppButton2 = ITextView::Probe(mMoveAppButton);
    if (envir->IsExternalStorageEmulated(&res), res) {
        moveAppButton1->SetVisibility(IView::INVISIBLE);
        return;
    }
    Boolean dataOnly = FALSE;
    dataOnly = (mPackageInfo == NULL) && (mAppEntry != NULL);
    Boolean moveDisable = TRUE;
    Int32 flags;
    if (dataOnly) {
        moveAppButton2->SetText(R::string::move_app);
    }
    else if (((mAppEntry->mInfo->GetFlags(&flags), flags) & IApplicationInfo::FLAG_EXTERNAL_STORAGE) != 0) {
        moveAppButton2->SetText(R::string::move_app_to_internal);
        // Always let apps move to internal storage from sdcard.
        moveDisable = FALSE;
    }
    else {
        moveAppButton2->SetText(R::string::move_app_to_sdcard);
        mCanBeOnSdCardChecker->Init();
        moveDisable = !mCanBeOnSdCardChecker->Check(mAppEntry->mInfo);
    }
    if (moveDisable || mAppControlRestricted) {
        moveAppButton1->SetEnabled(FALSE);
    }
    else {
        moveAppButton1->SetOnClickListener(mListener);
        moveAppButton1->SetEnabled(TRUE);
    }
}

Boolean CInstalledAppDetails::HandleDisableable(
    /* [in] */ IButton* button)
{
    Boolean disableable = FALSE;
    // Try to prevent the user from bricking their phone
    // by not allowing disabling of apps signed with the
    // system cert and any launcher app in the system.
    String packageName;
    IPackageItemInfo::Probe(mAppEntry->mInfo)->GetPackageName(&packageName);
    Boolean res = FALSE;
    Boolean enabled = FALSE;
    ITextView* _button = ITextView::Probe(button);
    if ((mHomePackages->Contains(CoreUtils::Convert(packageName), &res), res)
            || Utils::IsSystemPackage(mPm, mPackageInfo)) {
        // Disable button for core system applications.
        _button->SetText(R::string::disable_text);
    }
    else if ((mAppEntry->mInfo->GetEnabled(&enabled), enabled)) {
        _button->SetText(R::string::disable_text);
        disableable = TRUE;
    }
    else {
        _button->SetText(R::string::enable_text);
        disableable = TRUE;
    }

    return disableable;
}

void CInstalledAppDetails::InitUninstallButtons()
{
    Int32 aeFlags;
    mAppEntry->mInfo->GetFlags(&aeFlags);
    mUpdatedSysApp = (aeFlags & IApplicationInfo::FLAG_UPDATED_SYSTEM_APP) != 0;
    const Boolean isBundled = (aeFlags & IApplicationInfo::FLAG_SYSTEM) != 0;
    Boolean enabled = TRUE;
    ITextView* uninstallButton = ITextView::Probe(mUninstallButton);
    if (mUpdatedSysApp) {
        uninstallButton->SetText(R::string::app_factory_reset);
        Boolean showSpecialDisable = FALSE;
        if (isBundled) {
            showSpecialDisable = HandleDisableable(mSpecialDisableButton);
            IView::Probe(mSpecialDisableButton)->SetOnClickListener(mListener);
        }
        if (mAppControlRestricted) {
            showSpecialDisable = FALSE;
        }
        mMoreControlButtons->SetVisibility(showSpecialDisable ? IView::VISIBLE : IView::GONE);
    }
    else {
        mMoreControlButtons->SetVisibility(IView::GONE);
        AutoPtr<IApplicationInfo> applicationInfo;
        mPackageInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        Int32 flags;
        applicationInfo->GetFlags(&flags);
        AutoPtr<IList> list;
        Int32 size;
        if (isBundled) {
            enabled = HandleDisableable(mUninstallButton);
        }
        else if ((flags & IApplicationInfo::FLAG_INSTALLED) == 0
                && ((mUserManager->GetUsers((IList**)&list), list)->GetSize(&size), size) >= 2) {
            // When we have multiple users, there is a separate menu
            // to uninstall for all users.
            uninstallButton->SetText(R::string::uninstall_text);
            enabled = FALSE;
        }
        else {
            uninstallButton->SetText(R::string::uninstall_text);
        }
    }
    // If this is a device admin, it can't be uninstalled or disabled.
    // We do this here so the text of the button is still set correctly.
    String packageName;
    mPackageInfo->GetPackageName(&packageName);
    Boolean res;
    if (mDpm->PackageHasActiveAdmins(packageName, &res), res) {
        enabled = FALSE;
    }

    // Home apps need special handling.  Bundled ones we don't risk downgrading
    // because that can interfere with home-key resolution.  Furthermore, we
    // can't allow uninstallation of the only home app, and we don't want to
    // allow uninstallation of an explicitly preferred one -- the user can go
    // to Home settings and pick a different one, after which we'll permit
    // uninstallation of the now-not-default one.
    if (enabled && (mHomePackages->Contains(CoreUtils::Convert(packageName), &res), res)) {
        if (isBundled) {
            enabled = FALSE;
        }
        else {
            AutoPtr<IArrayList> homeActivities;//ArrayList<ResolveInfo>
            CArrayList::New((IArrayList**)&homeActivities);
            AutoPtr<IComponentName> currentDefaultHome;
            mPm->GetHomeActivities(IList::Probe(homeActivities), (IComponentName**)&currentDefaultHome);
            if (currentDefaultHome == NULL) {
                // No preferred default, so permit uninstall only when
                // there is more than one candidate
                Int32 size;
                enabled = ((mHomePackages->GetSize(&size), size) > 1);
            }
            else {
                // There is an explicit default home app -- forbid uninstall of
                // that one, but permit it for installed-but-inactive ones.
                String pkg;
                currentDefaultHome->GetPackageName(&pkg);
                enabled = !packageName.Equals(pkg);
            }
        }
    }

    if (mAppControlRestricted) {
        enabled = FALSE;
    }

    IView* _uninstallButton = IView::Probe(mUninstallButton);
    _uninstallButton->SetEnabled(enabled);
    if (enabled) {
        // Register listener
        _uninstallButton->SetOnClickListener(mListener);
    }
}

void CInstalledAppDetails::InitNotificationButton()
{
    IINotificationManager* nm = IINotificationManager::Probe(
            ServiceManager::GetService(IContext::NOTIFICATION_SERVICE));
    Boolean enabled = TRUE; // default on
    // try {
        String packageName;
        IPackageItemInfo::Probe(mAppEntry->mInfo)->GetPackageName(&packageName);
        Int32 uid;
        mAppEntry->mInfo->GetUid(&uid);
        ECode ec = nm->AreNotificationsEnabledForPackage(packageName, uid, &enabled);
        if (FAILED(ec)) {
        // } catch (android.os.RemoteException ex) {
        //     // this does not bode well
        // }
        }
    ICheckable::Probe(mNotificationSwitch)->SetChecked(enabled);
    IView* notificationSwitch = IView::Probe(mNotificationSwitch);
    if (Utils::IsSystemPackage(mPm, mPackageInfo)) {
        notificationSwitch->SetEnabled(FALSE);
    }
    else {
        notificationSwitch->SetEnabled(TRUE);
        mNotificationSwitch->SetOnCheckedChangeListener(mListener);
    }
}

ECode CInstalledAppDetails::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Fragment::OnCreate(icicle);

    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    AutoPtr<IApplication> application;
    _activity->GetApplication((IApplication**)&application);
    mState = ApplicationsState::GetInstance(application);
    mSession = mState->NewSession(mListener);
    IContext* activity = IContext::Probe(_activity);
    activity->GetPackageManager((IPackageManager**)&mPm);
    AutoPtr<IInterface> obj;
    activity->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUserManager = IUserManager::Probe(obj);
    Logger::I(TAG, "OnCreate, IIUsbManager is TODO");
    // AutoPtr<IInterface> b = ServiceManager::GetService(IContext::USB_SERVICE);
    // mUsbManager = IIUsbManager::Probe(b);
    AutoPtr<IAppWidgetManagerHelper> helper;
    CAppWidgetManagerHelper::AcquireSingleton((IAppWidgetManagerHelper**)&helper);
    helper->GetInstance(activity, (IAppWidgetManager**)&mAppWidgetManager);
    obj = NULL;
    activity->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&obj);
    mDpm = IDevicePolicyManager::Probe(obj);
    mSmsManager = IISms::Probe(ServiceManager::GetService(String("isms")));

    mCanBeOnSdCardChecker = new CanBeOnSdCardChecker();

    // Need to make sure we have loaded applications at this point.
    mSession->Resume();

    RetrieveAppEntry();

    SetHasOptionsMenu(TRUE);
    return NOERROR;
}

ECode CInstalledAppDetails::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IView> view;
    inflater->Inflate(R::layout::installed_app_details, container, FALSE, (IView**)&view);

    AutoPtr<IView> allDetailsTmp;
    view->FindViewById(R::id::all_details, (IView**)&allDetailsTmp);
    //IViewGroup* allDetails = IViewGroup::Probe(allDetailsTmp);
    Utils::ForceCustomPadding(allDetailsTmp, TRUE /* additive padding */);

    mRootView = view;
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mComputingStr = NULL;
    IContext::Probe(activity)->GetText(R::string::computing_size, (ICharSequence**)&mComputingStr);

    // Set default values on sizes
    AutoPtr<IView> tmp;
    view->FindViewById(R::id::total_size_text, (IView**)&tmp);
    mTotalSize = ITextView::Probe(tmp);
    tmp = NULL;
    view->FindViewById(R::id::application_size_text, (IView**)&tmp);
    mAppSize = ITextView::Probe(tmp);
    tmp = NULL;
    view->FindViewById(R::id::data_size_text, (IView**)&tmp);
    mDataSize = ITextView::Probe(tmp);
    AutoPtr<IView> externalCodeSizeTmp;
    view->FindViewById(R::id::external_code_size_text, (IView**)&externalCodeSizeTmp);
    mExternalCodeSize = ITextView::Probe(externalCodeSizeTmp);
    AutoPtr<IView> externalDataSizeTmp;
    view->FindViewById(R::id::external_data_size_text, (IView**)&externalDataSizeTmp);
    mExternalDataSize = ITextView::Probe(externalDataSizeTmp);

    AutoPtr<IEnvironment> envir;
    CEnvironment::AcquireSingleton((IEnvironment**)&envir);
    Boolean res;
    if (envir->IsExternalStorageEmulated(&res), res) {
        AutoPtr<IViewParent> parent;
        externalCodeSizeTmp->GetParent((IViewParent**)&parent);
        IView::Probe(parent)->SetVisibility(IView::GONE);
        parent = NULL;
        externalDataSizeTmp->GetParent((IViewParent**)&parent);
        IView::Probe(parent)->SetVisibility(IView::GONE);
    }

    // Get Control button panel
    AutoPtr<IView> btnPanel;
    view->FindViewById(R::id::control_buttons_panel, (IView**)&btnPanel);
    AutoPtr<IView> forceStopButtonTmp;
    btnPanel->FindViewById(R::id::left_button, (IView**)&forceStopButtonTmp);
    mForceStopButton = IButton::Probe(forceStopButtonTmp);
    ITextView::Probe(mForceStopButton)->SetText(R::string::force_stop);
    tmp = NULL;
    btnPanel->FindViewById(R::id::right_button, (IView**)&tmp);
    mUninstallButton = IButton::Probe(tmp);
    forceStopButtonTmp->SetEnabled(FALSE);

    // Get More Control button panel
    mMoreControlButtons = NULL;
    view->FindViewById(R::id::more_control_buttons_panel, (IView**)&mMoreControlButtons);
    tmp = NULL;
    mMoreControlButtons->FindViewById(R::id::left_button, (IView**)&tmp);
    tmp->SetVisibility(IView::INVISIBLE);
    tmp = NULL;
    mMoreControlButtons->FindViewById(R::id::right_button, (IView**)&tmp);
    mSpecialDisableButton = IButton::Probe(tmp);
    mMoreControlButtons->SetVisibility(IView::GONE);

    // Initialize clear data and move install location buttons
    AutoPtr<IView> data_buttons_panel;
    view->FindViewById(R::id::data_buttons_panel, (IView**)&data_buttons_panel);
    tmp = NULL;
    data_buttons_panel->FindViewById(R::id::right_button, (IView**)&tmp);
    mClearDataButton = IButton::Probe(tmp);
    tmp = NULL;
    data_buttons_panel->FindViewById(R::id::left_button, (IView**)&tmp);
    mMoveAppButton = IButton::Probe(tmp);

    // Cache section
    tmp = NULL;
    view->FindViewById(R::id::cache_size_text, (IView**)&tmp);
    mCacheSize = ITextView::Probe(tmp);
    tmp = NULL;
    view->FindViewById(R::id::clear_cache_button, (IView**)&tmp);
    mClearCacheButton = IButton::Probe(tmp);

    tmp = NULL;
    view->FindViewById(R::id::clear_activities_button, (IView**)&tmp);
    mActivitiesButton = IButton::Probe(tmp);

    // Screen compatibility control
    mScreenCompatSection = NULL;
    view->FindViewById(R::id::screen_compatibility_section, (IView**)&mScreenCompatSection);
    tmp = NULL;
    view->FindViewById(R::id::ask_compatibility_cb, (IView**)&tmp);
    mAskCompatibilityCB = ICheckBox::Probe(tmp);
    view->FindViewById(R::id::enable_compatibility_cb, (IView**)&tmp);
    mEnableCompatibilityCB = ICheckBox::Probe(tmp);

    view->FindViewById(R::id::notification_switch, (IView**)&tmp);
    mNotificationSwitch = ICompoundButton::Probe(tmp);

    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CInstalledAppDetails::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuInflater* inflater)
{
    AutoPtr<IMenuItem> item;
    menu->Add(0, UNINSTALL_ALL_USERS_MENU, 1, R::string::uninstall_all_users_text, (IMenuItem**)&item);
    item->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_NEVER);
    return NOERROR;
}

ECode CInstalledAppDetails::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu)
{
    Boolean showIt = TRUE;
    Int32 flags;
    String packageName;
    AutoPtr<IList> list;
    Int32 size;
    Boolean res;
    if (mUpdatedSysApp) {
        showIt = FALSE;
    }
    else if (mAppEntry == NULL) {
        showIt = FALSE;
    }
    else if (((mAppEntry->mInfo->GetFlags(&flags), flags) & IApplicationInfo::FLAG_SYSTEM) != 0) {
        showIt = FALSE;
    }
    else if (mPackageInfo == NULL || (mDpm->PackageHasActiveAdmins((mPackageInfo->GetPackageName(&packageName), packageName), &res), res)) {
        showIt = FALSE;
    }
    else if (UserHandle::GetMyUserId() != 0) {
        showIt = FALSE;
    }
    else if (((mUserManager->GetUsers((IList**)&list), list)->GetSize(&size), size) < 2 ) {
        showIt = FALSE;
    }

    AutoPtr<IMenuItem> item;
    menu->FindItem(UNINSTALL_ALL_USERS_MENU, (IMenuItem**)&item);
    item->SetVisible(showIt);
    return NOERROR;
}

ECode CInstalledAppDetails::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 menuId;
    item->GetItemId(&menuId);
    if (menuId == UNINSTALL_ALL_USERS_MENU) {
        String packageName;
        IPackageItemInfo::Probe(mAppEntry->mInfo)->GetPackageName(&packageName);
        UninstallPkg(packageName, TRUE, FALSE);
        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

ECode CInstalledAppDetails::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    Fragment::OnActivityResult(requestCode, resultCode, data);
    if (requestCode == REQUEST_UNINSTALL) {
        if (mDisableAfterUninstall) {
            mDisableAfterUninstall = FALSE;
            // try {
                AutoPtr<IActivity> activity;
                GetActivity((IActivity**)&activity);
                AutoPtr<IPackageManager> pm;
                IContext::Probe(activity)->GetPackageManager((IPackageManager**)&pm);
                String packageName;
                IPackageItemInfo::Probe(mAppEntry->mInfo)->GetPackageName(&packageName);
                AutoPtr<IApplicationInfo> ainfo;
                ECode ec = pm->GetApplicationInfo(packageName,
                        IPackageManager::GET_UNINSTALLED_PACKAGES | IPackageManager::GET_DISABLED_COMPONENTS,
                        (IApplicationInfo**)&ainfo);
                if (SUCCEEDED(ec)) {
                    Int32 flags;
                    ainfo->GetFlags(&flags);
                    if ((flags & IApplicationInfo::FLAG_UPDATED_SYSTEM_APP) == 0) {
                        AutoPtr<DisableChanger> dc = new DisableChanger(this, mAppEntry->mInfo,
                                IPackageManager::COMPONENT_ENABLED_STATE_DISABLED_USER);
                        AutoPtr< ArrayOf<IInterface*> > args;
                        dc->Execute(args);
                    }
                }
            // } catch (NameNotFoundException e) {
            // }
        }
        if (!RefreshUi()) {
            SetIntentAndFinish(TRUE, TRUE);
        }
    }
    return NOERROR;
}

void CInstalledAppDetails::SetAppLabelAndIcon(
    /* [in] */ IPackageInfo* pkgInfo)
{
    AutoPtr<IView> appSnippet;
    mRootView->FindViewById(R::id::app_snippet, (IView**)&appSnippet);
    Int32 top, bottom;
    appSnippet->GetPaddingTop(&top);
    appSnippet->GetPaddingBottom(&bottom);
    appSnippet->SetPaddingRelative(0, top, 0, bottom);

    AutoPtr<IView> iconTmp;
    appSnippet->FindViewById(R::id::app_icon, (IView**)&iconTmp);
    IImageView* icon = IImageView::Probe(iconTmp);
    mState->EnsureIcon(mAppEntry);
    icon->SetImageDrawable(mAppEntry->mIcon);
    // Set application name.
    AutoPtr<IView> labelTmp;
    appSnippet->FindViewById(R::id::app_name, (IView**)&labelTmp);
    ITextView* label = ITextView::Probe(labelTmp);
    label->SetText(CoreUtils::Convert(mAppEntry->mLabel));
    // Version number of application
    AutoPtr<IView> appVersionTmp;
    appSnippet->FindViewById(R::id::app_size, (IView**)&appVersionTmp);
    mAppVersion = ITextView::Probe(appVersionTmp);

    String versionName;
    if (pkgInfo != NULL && !(pkgInfo->GetVersionName(&versionName), versionName).IsNull()) {
        appVersionTmp->SetVisibility(IView::VISIBLE);
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(versionName));
        String str;
        IContext::Probe(activity)->GetString(R::string::version_text, args, &str);
        mAppVersion->SetText(CoreUtils::Convert(str));
    }
    else {
        appVersionTmp->SetVisibility(IView::INVISIBLE);
    }
}

ECode CInstalledAppDetails::OnResume()
{
    Fragment::OnResume();

    mUserManager->HasUserRestriction(IUserManager::DISALLOW_APPS_CONTROL, &mAppControlRestricted);
    mSession->Resume();
    if (!RefreshUi()) {
        SetIntentAndFinish(TRUE, TRUE);
    }
    return NOERROR;
}

ECode CInstalledAppDetails::OnPause()
{
    Fragment::OnPause();
    mSession->Pause();
    return NOERROR;
}

ECode CInstalledAppDetails::OnDestroyView()
{
    Fragment::OnDestroyView();
    mSession->ReleaseItem();
    return NOERROR;
}

ECode CInstalledAppDetails::OnAllSizesComputed()
{
    return NOERROR;
}

ECode CInstalledAppDetails::OnPackageIconChanged()
{
    return NOERROR;
}

ECode CInstalledAppDetails::OnPackageListChanged()
{
    RefreshUi();
    return NOERROR;
}

ECode CInstalledAppDetails::OnRebuildComplete(
    /* [in] */ IArrayList* apps) /*ArrayList<AppEntry>*/
{
    return NOERROR;
}

ECode CInstalledAppDetails::OnPackageSizeChanged(
    /* [in] */ const String& packageName)
{
    String pkg;
    IPackageItemInfo::Probe(mAppEntry->mInfo)->GetPackageName(&pkg);
    if (packageName.Equals(pkg)) {
        RefreshSizeInfo();
    }
    return NOERROR;
}

ECode CInstalledAppDetails::OnRunningStateChanged(
    /* [in] */ Boolean running)
{
    return NOERROR;
}

String CInstalledAppDetails::RetrieveAppEntry()
{
    AutoPtr<IBundle> args;
    GetArguments((IBundle**)&args);
    String packageName;
    if (args != NULL) {
        args->GetString(ARG_PACKAGE_NAME, &packageName);
    }
    if (packageName.IsNull()) {
        AutoPtr<IIntent> intent;
        if (args == NULL) {
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            activity->GetIntent((IIntent**)&intent);
        }
        else {
            AutoPtr<IParcelable> parcel;
            args->GetParcelable(String("intent"), (IParcelable**)&parcel);
            intent = IIntent::Probe(parcel);
        }
        if (intent != NULL) {
            AutoPtr<IUri> data;
            intent->GetData((IUri**)&data);
            data->GetSchemeSpecificPart(&packageName);
        }
    }
    mAppEntry = mState->GetEntry(packageName);
    if (mAppEntry != NULL) {
        // Get application info again to refresh changed properties of application
        // try {
            String packageName;
            IPackageItemInfo::Probe(mAppEntry->mInfo)->GetPackageName(&packageName);
            ECode ec = mPm->GetPackageInfo(packageName,
                    IPackageManager::GET_DISABLED_COMPONENTS |
                    IPackageManager::GET_UNINSTALLED_PACKAGES |
                    IPackageManager::GET_SIGNATURES, (IPackageInfo**)&mPackageInfo);
            if (FAILED(ec)) {
            // } catch (NameNotFoundException e) {
                Logger::E(TAG, "Exception when retrieving package:%s, 08%08x", packageName.string(), ec);
            // }
            }
    }
    else {
        Logger::W(TAG, "Missing AppEntry; maybe reinstalling?");
        mPackageInfo = NULL;
    }

    return packageName;
}

Boolean CInstalledAppDetails::SignaturesMatch(
    /* [in] */ const String& pkg1,
    /* [in] */ const String& pkg2)
{
    if (!pkg1.IsNull() && !pkg2.IsNull()) {
        // try {
            Int32 match;
            ECode ec = mPm->CheckSignatures(pkg1, pkg2, &match);
            if (SUCCEEDED(ec)) {
                if (match >= IPackageManager::SIGNATURE_MATCH) {
                    return TRUE;
                }
            }
            else {
            // } catch (Exception e) {
            //     // e.g. named alternate package not found during lookup;
            //     // this is an expected case sometimes
            // }
            }
    }
    return FALSE;
}

Boolean CInstalledAppDetails::RefreshUi()
{
    if (mMoveInProgress) {
        return TRUE;
    }
    const String packageName = RetrieveAppEntry();

    if (mAppEntry == NULL) {
        return FALSE; // onCreate must have failed, make sure to exit
    }

    if (mPackageInfo == NULL) {
        return FALSE; // onCreate must have failed, make sure to exit
    }

    // Get list of "home" apps and trace through any meta-data references
    AutoPtr<IList> homeActivities; // List<ResolveInfo>
    CArrayList::New((IList**)&homeActivities);
    AutoPtr<IComponentName> comp;
    mPm->GetHomeActivities(homeActivities, (IComponentName**)&comp);
    mHomePackages->Clear();
    Int32 size;
    homeActivities->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        homeActivities->Get(i, (IInterface**)&obj);
        IResolveInfo* ri = IResolveInfo::Probe(obj);
        AutoPtr<IActivityInfo> activityInfo;
        ri->GetActivityInfo((IActivityInfo**)&activityInfo);
        String activityPkg;
        IPackageItemInfo::Probe(activityInfo)->GetPackageName(&activityPkg);
        mHomePackages->Add(CoreUtils::Convert(activityPkg));

        // Also make sure to include anything proxying for the home app
        AutoPtr<IBundle> metadata;
        IPackageItemInfo::Probe(activityInfo)->GetMetaData((IBundle**)&metadata);
        if (metadata != NULL) {
            String metaPkg;
            metadata->GetString(IActivityManager::META_HOME_ALTERNATE, &metaPkg);
            if (SignaturesMatch(metaPkg, activityPkg)) {
                mHomePackages->Add(CoreUtils::Convert(metaPkg));
            }
        }
    }

    // Get list of preferred activities
    AutoPtr<IList> prefActList;//List<ComponentName>
    CArrayList::New((IList**)&prefActList);

    // Intent list cannot be NULL. so pass empty list
    AutoPtr<IList> intentList;//List<IntentFilter>
    CArrayList::New((IList**)&intentList);
    Int32 data;
    mPm->GetPreferredActivities(intentList, prefActList, packageName, &data);
    prefActList->GetSize(&size);
    if (localLOGV)
        Logger::I(TAG, "Have %d number of activities in preferred list", size);
    Boolean hasUsbDefaults = FALSE;
    ECode ec = NOERROR;
    // try {
        Logger::I(TAG, "RefreshUi, IIUsbManager is TODO");
        // ec = mUsbManager->HasDefaults(packageName, UserHandle::GetMyUserId(), &hasUsbDefaults);
        // if (FAILED(ec)) {
        //     Logger::E(TAG, "mUsbManager.hasDefaults 08%08x", ec);
        // }
    // } catch (RemoteException e) {
            // Logger::E(TAG, "mUsbManager.hasDefaults 08%08x", ec);
    // }

    String aePkgName;
    IPackageItemInfo::Probe(mAppEntry->mInfo)->GetPackageName(&aePkgName);
    Boolean hasBindAppWidgetPermission;
    mAppWidgetManager->HasBindAppWidgetPermission(aePkgName, &hasBindAppWidgetPermission);

    AutoPtr<IView> tmp;
    mRootView->FindViewById(R::id::auto_launch_title, (IView**)&tmp);
    AutoPtr<ITextView> autoLaunchTitleView = ITextView::Probe(tmp);
    tmp = NULL;
    mRootView->FindViewById(R::id::auto_launch, (IView**)&tmp);
    AutoPtr<ITextView> autoLaunchView = ITextView::Probe(tmp);
    Boolean autoLaunchEnabled = (prefActList->GetSize(&size), size) > 0 || hasUsbDefaults;
    if (!autoLaunchEnabled && !hasBindAppWidgetPermission) {
        ResetLaunchDefaultsUi(autoLaunchTitleView, autoLaunchView);
    }
    else {
        Boolean useBullets = hasBindAppWidgetPermission && autoLaunchEnabled;

        if (hasBindAppWidgetPermission) {
            autoLaunchTitleView->SetText(R::string::auto_launch_label_generic);
        }
        else {
            autoLaunchTitleView->SetText(R::string::auto_launch_label);
        }

        AutoPtr<ICharSequence> text;
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        Int32 bulletIndent;
        resources->GetDimensionPixelSize(R::dimen::installed_app_details_bullet_offset, &bulletIndent);
        if (autoLaunchEnabled) {
            AutoPtr<ICharSequence> autoLaunchEnableText;
            GetText(R::string::auto_launch_enable_text, (ICharSequence**)&autoLaunchEnableText);
            AutoPtr<ISpannableString> s;
            CSpannableString::New(autoLaunchEnableText, (ISpannableString**)&s);
            if (useBullets) {
                Int32 length;
                autoLaunchEnableText->GetLength(&length);
                AutoPtr<IBulletSpan> bs;
                CBulletSpan::New(bulletIndent, (IBulletSpan**)&bs);
                ISpannable::Probe(s)->SetSpan(bs, 0, length, 0);
            }
            if (text == NULL) {
                AutoPtr<ArrayOf<ICharSequence*> > args = ArrayOf<ICharSequence*>::Alloc(2);
                args->Set(0, ICharSequence::Probe(s));
                args->Set(1, CoreUtils::Convert("\n"));

                text = TextUtils::Concat(args);
            }
            else {
                AutoPtr<ArrayOf<ICharSequence*> > args = ArrayOf<ICharSequence*>::Alloc(4);
                args->Set(0, text);
                args->Set(1, CoreUtils::Convert("\n"));
                args->Set(2, ICharSequence::Probe(s));
                args->Set(3, CoreUtils::Convert("\n"));

                text = TextUtils::Concat(args);
            }
        }
        if (hasBindAppWidgetPermission) {
            AutoPtr<ICharSequence> alwaysAllowBindAppWidgetsText;
            GetText(R::string::always_allow_bind_appwidgets_text, (ICharSequence**)&alwaysAllowBindAppWidgetsText);
            AutoPtr<ISpannableString> s;
            CSpannableString::New(alwaysAllowBindAppWidgetsText, (ISpannableString**)&s);
            if (useBullets) {
                Int32 length;
                alwaysAllowBindAppWidgetsText->GetLength(&length);
                AutoPtr<IBulletSpan> bs;
                CBulletSpan::New(bulletIndent, (IBulletSpan**)&bs);
                ISpannable::Probe(s)->SetSpan(bs, 0, length, 0);
            }
            if (text == NULL) {
                AutoPtr<ArrayOf<ICharSequence*> > args = ArrayOf<ICharSequence*>::Alloc(2);
                args->Set(0, ICharSequence::Probe(s));
                args->Set(1, CoreUtils::Convert("\n"));

                text = TextUtils::Concat(args);
            }
            else {
                AutoPtr<ArrayOf<ICharSequence*> > args = ArrayOf<ICharSequence*>::Alloc(4);
                args->Set(0, text);
                args->Set(1, CoreUtils::Convert("\n"));
                args->Set(2, ICharSequence::Probe(s));
                args->Set(3, CoreUtils::Convert("\n"));

                text = TextUtils::Concat(args);
            }
        }
        autoLaunchView->SetText(text);
        IView* activitiesButton = IView::Probe(mActivitiesButton);
        activitiesButton->SetEnabled(TRUE);
        activitiesButton->SetOnClickListener(mListener);
    }

    // Screen compatibility section.
    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    AutoPtr<IInterface> obj;
    activity->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&obj);
    IActivityManager* am = IActivityManager::Probe(obj);
    Int32 compatMode;
    am->GetPackageScreenCompatMode(packageName, &compatMode);
    // For now these are always off; this is the old UI model which we
    // are no longer using.
    if (FALSE && (compatMode == IActivityManager::COMPAT_MODE_DISABLED
            || compatMode == IActivityManager::COMPAT_MODE_ENABLED)) {
        mScreenCompatSection->SetVisibility(IView::VISIBLE);
        Boolean res;
        am->GetPackageAskScreenCompat(packageName, &res);
        ICheckable::Probe(mAskCompatibilityCB)->SetChecked(res);
        ICompoundButton::Probe(mAskCompatibilityCB)->SetOnCheckedChangeListener(mListener);
        ICheckable::Probe(mEnableCompatibilityCB)->SetChecked(compatMode == IActivityManager::COMPAT_MODE_ENABLED);
        ICompoundButton::Probe(mEnableCompatibilityCB)->SetOnCheckedChangeListener(mListener);
    }
    else {
        mScreenCompatSection->SetVisibility(IView::GONE);
    }

    // Security permissions section
    AutoPtr<IView> permsViewTmp;
    mRootView->FindViewById(R::id::permissions_section, (IView**)&permsViewTmp);
    // ILinearLayout* permsView = ILinearLayout::Probe(permsViewTmp);
    AutoPtr<IAppSecurityPermissions> asp;
    CAppSecurityPermissions::New(activity, packageName, (IAppSecurityPermissions**)&asp);
    Int32 premiumSmsPermission = GetPremiumSmsPermission(packageName);
    // Premium SMS permission implies the app also has SEND_SMS permission, so the original
    // application permissions list doesn't have to be shown/hidden separately. The premium
    // SMS subsection should only be visible if the app has tried to send to a premium SMS.
    Int32 count;
    if ((asp->GetPermissionCount(&count), count) > 0
            || premiumSmsPermission != ISmsUsageMonitor::PREMIUM_SMS_PERMISSION_UNKNOWN) {
        permsViewTmp->SetVisibility(IView::VISIBLE);
    }
    else {
        permsViewTmp->SetVisibility(IView::GONE);
    }
    // Premium SMS permission subsection
    AutoPtr<IView> securityBillingDescTmp;
    permsViewTmp->FindViewById(
            R::id::security_settings_billing_desc, (IView**)&securityBillingDescTmp);
    // ITextView* securityBillingDesc = ITextView::Probe(securityBillingDescTmp);
    AutoPtr<IView> securityBillingListTmp;
    permsViewTmp->FindViewById(
            R::id::security_settings_billing_list, (IView**)&securityBillingListTmp);
    // ILinearLayout* securityBillingList = ILinearLayout::Probe(securityBillingListTmp);
    if (premiumSmsPermission != ISmsUsageMonitor::PREMIUM_SMS_PERMISSION_UNKNOWN) {
        // Show the premium SMS permission selector
        securityBillingDescTmp->SetVisibility(IView::VISIBLE);
        securityBillingListTmp->SetVisibility(IView::VISIBLE);
        AutoPtr<IView> spinnerTmp;
        permsViewTmp->FindViewById(
                R::id::security_settings_premium_sms_list, (IView**)&spinnerTmp);
        ISpinner* spinner = ISpinner::Probe(spinnerTmp);
        AutoPtr<IArrayAdapter> adapter = ArrayAdapter::CreateFromResource(activity,
                R::array::security_settings_premium_sms_values,
                Elastos::Droid::R::layout::simple_spinner_item);
        adapter->SetDropDownViewResource(Elastos::Droid::R::layout::simple_spinner_dropdown_item);
        IAdapterView* _spinner = IAdapterView::Probe(spinner);
        _spinner->SetAdapter(IAdapter::Probe(adapter));
        // List items are in the same order as SmsUsageMonitor constants, offset by 1.
        _spinner->SetSelection(premiumSmsPermission - 1);
        _spinner->SetOnItemSelectedListener(new PremiumSmsSelectionListener(
                packageName, mSmsManager));
    }
    else {
        // Hide the premium SMS permission selector
        securityBillingDescTmp->SetVisibility(IView::GONE);
        securityBillingListTmp->SetVisibility(IView::GONE);
    }
    // App permissions subsection
    if ((asp->GetPermissionCount(&count), count) > 0) {
        // Make the security sections header visible
        AutoPtr<IView> securityListTmp;
        permsViewTmp->FindViewById(
                R::id::security_settings_list, (IView**)&securityListTmp);
        /*ILinearLayout*/ IViewGroup* securityList = IViewGroup::Probe(securityListTmp);
        securityList->RemoveAllViews();
        AutoPtr<IView> tmp;
        asp->GetPermissionsViewWithRevokeButtons((IView**)&tmp);
        securityList->AddView(tmp);
        // If this app is running under a shared user ID with other apps,
        // update the description to explain this.
        AutoPtr<IApplicationInfo> applicationInfo;
        mPackageInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        Int32 uid;
        applicationInfo->GetUid(&uid);
        AutoPtr< ArrayOf<String> > packages;
        mPm->GetPackagesForUid(uid, (ArrayOf<String>**)&packages);
        if (packages != NULL && packages->GetLength() > 1) {
            AutoPtr<IArrayList> pnames;//ArrayList<CharSequence>
            CArrayList::New((IArrayList**)&pnames);
            for (Int32 i = 0; i < packages->GetLength(); i++) {
                String pkg = (*packages)[i];
                String packageName;
                mPackageInfo->GetPackageName(&packageName);
                if (packageName.Equals(pkg)) {
                    continue;
                }
                // try {
                    AutoPtr<IApplicationInfo> ainfo;
                    ec = mPm->GetApplicationInfo(pkg, 0, (IApplicationInfo**)&ainfo);
                    if (SUCCEEDED(ec)) {
                        AutoPtr<ICharSequence> cs;
                        IPackageItemInfo::Probe(ainfo)->LoadLabel(mPm, (ICharSequence**)&cs);
                        pnames->Add(cs);
                    }
                // } catch (IPackageManager::NameNotFoundException e) {
                // }
            }
            Int32 N;
            pnames->GetSize(&N);
            if (N > 0) {
                AutoPtr<IResources> res;
                activity->GetResources((IResources**)&res);
                String appListStr;
                if (N == 1) {
                    AutoPtr<IInterface> obj;
                    pnames->Get(0, (IInterface**)&obj);
                    ICharSequence::Probe(obj)->ToString(&appListStr);
                }
                else if (N == 2) {
                    AutoPtr<IInterface> obj;
                    pnames->Get(0, (IInterface**)&obj);
                    AutoPtr<IInterface> obj1;
                    pnames->Get(1, (IInterface**)&obj1);
                    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
                    args->Set(0, obj);
                    args->Set(1, obj1);
                    res->GetString(R::string::join_two_items, args, &appListStr);
                }
                else {
                    AutoPtr<IInterface> obj;
                    pnames->Get(N - 2, (IInterface**)&obj);
                    ICharSequence::Probe(obj)->ToString(&appListStr);
                    for (Int32 i = N-3; i >= 0; i--) {
                        AutoPtr<IInterface> pnamesObj;
                        pnames->Get(i, (IInterface**)&pnamesObj);
                        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
                        args->Set(0, pnamesObj);
                        args->Set(1, CoreUtils::Convert(appListStr));

                        res->GetString(i == 0 ? R::string::join_many_items_first
                                : R::string::join_many_items_middle, args, &appListStr);
                    }
                    obj = NULL;
                    pnames->Get(N-1, (IInterface**)&obj);
                    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
                    args->Set(0, CoreUtils::Convert(appListStr));
                    args->Set(1, obj);

                    res->GetString(R::string::join_many_items_last, args, &appListStr);
                }
                AutoPtr<IView> descrTmp;
                mRootView->FindViewById(R::id::security_settings_desc, (IView**)&descrTmp);
                ITextView* descr = ITextView::Probe(descrTmp);

                AutoPtr<ICharSequence> seq;
                IPackageItemInfo::Probe(applicationInfo)->LoadLabel(mPm, (ICharSequence**)&seq);
                AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
                args->Set(0, seq);
                args->Set(1, CoreUtils::Convert(appListStr));
                String str;
                res->GetString(R::string::security_settings_desc_multi, args, &str);
                descr->SetText(CoreUtils::Convert(str));
            }
        }
    }

    CheckForceStop();
    SetAppLabelAndIcon(mPackageInfo);
    RefreshButtons();
    RefreshSizeInfo();

    Int32 flags;
    if (!mInitialized) {
        // First time init: are we displaying an uninstalled app?
        mInitialized = TRUE;
        mShowUninstalled = ((mAppEntry->mInfo->GetFlags(&flags), flags) & IApplicationInfo::FLAG_INSTALLED) == 0;
    }
    else {
        // All other times: if the app no longer exists then we want
        // to go away.
        // try {
            AutoPtr<IPackageManager> pm;
            activity->GetPackageManager((IPackageManager**)&pm);
            AutoPtr<IApplicationInfo> ainfo;
            ec = pm->GetApplicationInfo(aePkgName, IPackageManager::GET_UNINSTALLED_PACKAGES
                    | IPackageManager::GET_DISABLED_COMPONENTS, (IApplicationInfo**)&ainfo);
            if (SUCCEEDED(ec)) {
                if (!mShowUninstalled) {
                    // If we did not start out with the app uninstalled, then
                    // it transitioning to the uninstalled state for the current
                    // user means we should go away as well.
                    return ((ainfo->GetFlags(&flags), flags) & IApplicationInfo::FLAG_INSTALLED) != 0;
                }
            }
            else {
        // } catch (NameNotFoundException e) {
                return FALSE;
        // }
            }
    }

    return TRUE;
}

void CInstalledAppDetails::ResetLaunchDefaultsUi(
    /* [in] */ ITextView* title,
    /* [in] */ ITextView* autoLaunchView)
{
    title->SetText(R::string::auto_launch_label);
    autoLaunchView->SetText(R::string::auto_launch_disable_text);
    // Disable clear activities button
    IView::Probe(mActivitiesButton)->SetEnabled(FALSE);
}

void CInstalledAppDetails::SetIntentAndFinish(
    /* [in] */ Boolean finish,
    /* [in] */ Boolean appChanged)
{
    if (localLOGV) Logger::I(TAG, "appChanged= %d", appChanged);
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->PutBooleanExtra(ManageApplications::APP_CHG, appChanged);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    CSettingsActivity* sa = (CSettingsActivity*)ISettingsActivity::Probe(activity);
    sa->FinishPreferencePanel(this, IActivity::RESULT_OK, intent);
}

void CInstalledAppDetails::RefreshSizeInfo()
{
    IView* clearDataButton = IView::Probe(mClearDataButton);
    IView* clearCacheButton = IView::Probe(mClearCacheButton);
    if (mAppEntry->mSize == ApplicationsState::SIZE_INVALID
            || mAppEntry->mSize == ApplicationsState::SIZE_UNKNOWN) {
        mLastCodeSize = mLastDataSize = mLastCacheSize = mLastTotalSize = -1;
        if (!mHaveSizes) {
            mAppSize->SetText(mComputingStr);
            mDataSize->SetText(mComputingStr);
            mCacheSize->SetText(mComputingStr);
            mTotalSize->SetText(mComputingStr);
        }
        clearDataButton->SetEnabled(FALSE);
        clearCacheButton->SetEnabled(FALSE);

    }
    else {
        mHaveSizes = TRUE;
        Int64 codeSize = mAppEntry->mCodeSize;
        Int64 dataSize = mAppEntry->mDataSize;
        AutoPtr<IEnvironment> envir;
        CEnvironment::AcquireSingleton((IEnvironment**)&envir);
        Boolean res;
        if (envir->IsExternalStorageEmulated(&res), res) {
            codeSize += mAppEntry->mExternalCodeSize;
            dataSize +=  mAppEntry->mExternalDataSize;
        }
        else {
            if (mLastExternalCodeSize != mAppEntry->mExternalCodeSize) {
                mLastExternalCodeSize = mAppEntry->mExternalCodeSize;
                mExternalCodeSize->SetText(CoreUtils::Convert(GetSizeStr(mAppEntry->mExternalCodeSize)));
            }
            if (mLastExternalDataSize !=  mAppEntry->mExternalDataSize) {
                mLastExternalDataSize =  mAppEntry->mExternalDataSize;
                mExternalDataSize->SetText(CoreUtils::Convert(GetSizeStr( mAppEntry->mExternalDataSize)));
            }
        }
        if (mLastCodeSize != codeSize) {
            mLastCodeSize = codeSize;
            mAppSize->SetText(CoreUtils::Convert(GetSizeStr(codeSize)));
        }
        if (mLastDataSize != dataSize) {
            mLastDataSize = dataSize;
            mDataSize->SetText(CoreUtils::Convert(GetSizeStr(dataSize)));
        }
        Int64 cacheSize = mAppEntry->mCacheSize + mAppEntry->mExternalCacheSize;
        if (mLastCacheSize != cacheSize) {
            mLastCacheSize = cacheSize;
            mCacheSize->SetText(CoreUtils::Convert(GetSizeStr(cacheSize)));
        }
        if (mLastTotalSize != mAppEntry->mSize) {
            mLastTotalSize = mAppEntry->mSize;
            mTotalSize->SetText(CoreUtils::Convert(GetSizeStr(mAppEntry->mSize)));
        }

        if ((mAppEntry->mDataSize + mAppEntry->mExternalDataSize) <= 0 || !mCanClearData) {
            clearDataButton->SetEnabled(FALSE);
        }
        else {
            clearDataButton->SetEnabled(TRUE);
            clearDataButton->SetOnClickListener(mListener);
        }
        if (cacheSize <= 0) {
            clearCacheButton->SetEnabled(FALSE);
        }
        else {
            clearCacheButton->SetEnabled(TRUE);
            clearCacheButton->SetOnClickListener(mListener);
        }
    }
    if (mAppControlRestricted) {
        clearCacheButton->SetEnabled(FALSE);
        clearDataButton->SetEnabled(FALSE);
    }
}

void CInstalledAppDetails::ProcessClearMsg(
    /* [in] */ IMessage* msg)
{
    Int32 result;
    msg->GetArg1(&result);
    String packageName;
    IPackageItemInfo::Probe(mAppEntry->mInfo)->GetPackageName(&packageName);
    ITextView::Probe(mClearDataButton)->SetText(R::string::clear_user_data_text);
    if (result == OP_SUCCESSFUL) {
        Logger::I(TAG, "Cleared user data for package : %s", packageName.string());
        mState->RequestSize(packageName);
    }
    else {
        IView::Probe(mClearDataButton)->SetEnabled(TRUE);
    }
    CheckForceStop();
}

void CInstalledAppDetails::RefreshButtons()
{
    if (!mMoveInProgress) {
        InitUninstallButtons();
        InitDataButtons();
        InitMoveButton();
        InitNotificationButton();
    }
    else {
        ITextView::Probe(mMoveAppButton)->SetText(R::string::moving);
        IView::Probe(mMoveAppButton)->SetEnabled(FALSE);
        IView::Probe(mUninstallButton)->SetEnabled(FALSE);
        IView::Probe(mSpecialDisableButton)->SetEnabled(FALSE);
    }
}

void CInstalledAppDetails::ProcessMoveMsg(
    /* [in] */ IMessage* msg)
{
    Int32 result;
    msg->GetArg1(&result);
    String packageName;
    IPackageItemInfo::Probe(mAppEntry->mInfo)->GetPackageName(&packageName);
    // Refresh the button attributes.
    mMoveInProgress = FALSE;
    if (result == IPackageManager::MOVE_SUCCEEDED) {
        Logger::I(TAG, "Moved resources for %s", packageName.string());
        // Refresh size information again.
        mState->RequestSize(packageName);
    }
    else {
        ShowDialogInner(DLG_MOVE_FAILED, result);
    }
    RefreshUi();
}

void CInstalledAppDetails::InitiateClearUserData()
{
    IView::Probe(mClearDataButton)->SetEnabled(FALSE);
    // Invoke uninstall or clear user data based on sysPackage
    String packageName;
    IPackageItemInfo::Probe(mAppEntry->mInfo)->GetPackageName(&packageName);
    Logger::I(TAG, "Clearing user data for package : %s", packageName.string());
    if (mClearDataObserver == NULL) {
        CInstalledAppDetailsClearUserDataObserver::New((IFragment*)this, (IIPackageDataObserver**)&mClearDataObserver);
    }
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&obj);
    IActivityManager* am = IActivityManager::Probe(obj);

    Boolean res;
    am->ClearApplicationUserData(packageName, mClearDataObserver, &res);
    if (!res) {
        // Clearing data failed for some obscure reason. Just log error for now
        Logger::I(TAG, "Couldnt clear application user data for package:%s", packageName.string());
        ShowDialogInner(DLG_CANNOT_CLEAR_DATA, 0);
    }
    else {
        ITextView::Probe(mClearDataButton)->SetText(R::string::recompute_size);
    }
}

void CInstalledAppDetails::ShowDialogInner(
    /* [in] */ Int32 id,
    /* [in] */ Int32 moveErrorCode)
{
    AutoPtr<IDialogFragment> newFragment = MyAlertDialogFragment::NewInstance(id, moveErrorCode);
    IFragment::Probe(newFragment)->SetTargetFragment(this, 0);
    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    newFragment->Show(manager, String("dialog ") + StringUtils::ToString(id));
}

void CInstalledAppDetails::UninstallPkg(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean allUsers,
    /* [in] */ Boolean andDisable)
{
    // Create new intent to launch Uninstaller activity
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> packageURI;
    helper->Parse(String("package:") + packageName, (IUri**)&packageURI);
    AutoPtr<IIntent> uninstallIntent;
    CIntent::New(IIntent::ACTION_UNINSTALL_PACKAGE, packageURI, (IIntent**)&uninstallIntent);
    uninstallIntent->PutBooleanExtra(IIntent::EXTRA_UNINSTALL_ALL_USERS, allUsers);
    StartActivityForResult(uninstallIntent, REQUEST_UNINSTALL);
    mDisableAfterUninstall = andDisable;
}

void CInstalledAppDetails::ForceStopPackage(
    /* [in] */ const String& pkgName)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(
            IContext::ACTIVITY_SERVICE, (IInterface**)&obj);
    IActivityManager* am = IActivityManager::Probe(obj);
    am->ForceStopPackage(pkgName);
    mState->InvalidatePackage(pkgName);
    AutoPtr<ApplicationsState::AppEntry> newEnt = mState->GetEntry(pkgName);
    if (newEnt != NULL) {
        mAppEntry = newEnt;
    }
    CheckForceStop();
}

void CInstalledAppDetails::UpdateForceStopButton(
    /* [in] */ Boolean enabled)
{
    IView* forceStopButton = IView::Probe(mForceStopButton);
    if (mAppControlRestricted) {
        forceStopButton->SetEnabled(FALSE);
    }
    else {
        forceStopButton->SetEnabled(enabled);
        forceStopButton->SetOnClickListener(mListener);
    }
}

void CInstalledAppDetails::CheckForceStop()
{
    String packageName;
    mPackageInfo->GetPackageName(&packageName);
    Int32 flags;
    Boolean res;
    if (mDpm->PackageHasActiveAdmins(packageName, &res), res) {
        // User can't force stop device admin.
        UpdateForceStopButton(FALSE);
    }
    else if (((mAppEntry->mInfo->GetFlags(&flags), flags) & IApplicationInfo::FLAG_STOPPED) == 0) {
        // If the app isn't explicitly stopped, then always show the
        // force stop button.
        UpdateForceStopButton(TRUE);
    }
    else {
        String packageName;
        IPackageItemInfo::Probe(mAppEntry->mInfo)->GetPackageName(&packageName);
        Int32 uid;
        mAppEntry->mInfo->GetUid(&uid);
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->FromParts(String("package"), packageName, String(NULL), (IUri**)&uri);
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_QUERY_PACKAGE_RESTART, uri, (IIntent**)&intent);

        AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
        (*args)[0] = packageName;
        intent->PutExtra(IIntent::EXTRA_PACKAGES, args);
        intent->PutExtra(IIntent::EXTRA_UID, uid);
        intent->PutExtra(IIntent::EXTRA_USER_HANDLE, UserHandle::GetUserId(uid));
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        IContext::Probe(activity)->SendOrderedBroadcastAsUser(intent, UserHandle::CURRENT, String(NULL),
                mCheckKillProcessesReceiver, NULL, IActivity::RESULT_CANCELED, String(NULL), NULL);
    }
}

void CInstalledAppDetails::SetNotificationsEnabled(
    /* [in] */ Boolean enabled)
{
    String packageName;
    IPackageItemInfo::Probe(mAppEntry->mInfo)->GetPackageName(&packageName);
    IINotificationManager* nm = IINotificationManager::Probe(
            ServiceManager::GetService(IContext::NOTIFICATION_SERVICE));
    ICheckable* notificationSwitch = ICheckable::Probe(mNotificationSwitch);
    // try {
        Boolean enable;
        notificationSwitch->IsChecked(&enable);
        Int32 uid;
        mAppEntry->mInfo->GetUid(&uid);
        ECode ec = nm->SetNotificationsEnabledForPackage(packageName, uid, enabled);
    // } catch (android.os.RemoteException ex) {
        if (FAILED(ec)) {
            notificationSwitch->SetChecked(!enabled); // revert
        }
    // }
}

Int32 CInstalledAppDetails::GetPremiumSmsPermission(
    /* [in] */ const String& packageName)
{
    // try {
        if (mSmsManager != NULL) {
            Int32 result;
            ECode ec = mSmsManager->GetPremiumSmsPermission(packageName, &result);
            if (SUCCEEDED(ec)) {
                return result;
            }
        }
    // } catch (RemoteException ex) {
    //     // ignored
    // }
    return ISmsUsageMonitor::PREMIUM_SMS_PERMISSION_UNKNOWN;
}

ECode CInstalledAppDetails::OnClick(
    /* [in] */ IView* v)
{
    String packageName;
    IPackageItemInfo::Probe(mAppEntry->mInfo)->GetPackageName(&packageName);
    Int32 flags;
    if (v == IView::Probe(mUninstallButton)) {
        if (mUpdatedSysApp) {
            ShowDialogInner(DLG_FACTORY_RESET, 0);
        }
        else {
            mAppEntry->mInfo->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
                Boolean enabled;
                mAppEntry->mInfo->GetEnabled(&enabled);
                if (enabled) {
                    ShowDialogInner(DLG_DISABLE, 0);
                }
                else {
                    AutoPtr<DisableChanger> dc = new DisableChanger(this, mAppEntry->mInfo,
                            IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT);
                    AutoPtr< ArrayOf<IInterface*> > args;
                    dc->Execute(args);
                }
            }
            else if ((flags & IApplicationInfo::FLAG_INSTALLED) == 0) {
                UninstallPkg(packageName, TRUE, FALSE);
            }
            else {
                UninstallPkg(packageName, FALSE, FALSE);
            }
        }
    }
    else if (v == IView::Probe(mSpecialDisableButton)) {
        ShowDialogInner(DLG_SPECIAL_DISABLE, 0);
    }
    else if (v == IView::Probe(mActivitiesButton)) {
        mPm->ClearPackagePreferredActivities(packageName);
        // try {
            Logger::I(TAG, "OnClick, IIUsbManager is TODO");
            // ECode ec = mUsbManager->ClearDefaults(packageName, UserHandle::GetMyUserId());
            // if (FAILED(ec)) {
            //     Logger::E(TAG, "mUsbManager.clearDefaults 08%08x", ec);
            // }
        // } catch (RemoteException e) {
            // Logger::E(TAG, "mUsbManager.clearDefaults", e);
        // }
        mAppWidgetManager->SetBindAppWidgetPermission(packageName, FALSE);
        AutoPtr<IView> tmp;
        mRootView->FindViewById(R::id::auto_launch_title, (IView**)&tmp);
        AutoPtr<ITextView> autoLaunchTitleView = ITextView::Probe(tmp);
        tmp = NULL;
        mRootView->FindViewById(R::id::auto_launch, (IView**)&tmp);
        AutoPtr<ITextView> autoLaunchView = ITextView::Probe(tmp);
        ResetLaunchDefaultsUi(autoLaunchTitleView, autoLaunchView);
    }
    else if (v == IView::Probe(mClearDataButton)) {
        String manageSpaceActivityName;
        mAppEntry->mInfo->GetManageSpaceActivityName(&manageSpaceActivityName);
        if (!manageSpaceActivityName.IsNull()) {
            if (!Utils::IsMonkeyRunning()) {
                AutoPtr<IIntent> intent;
                CIntent::New(IIntent::ACTION_DEFAULT, (IIntent**)&intent);
                intent->SetClassName(packageName, manageSpaceActivityName);
                StartActivityForResult(intent, REQUEST_MANAGE_SPACE);
            }
        }

        else {
            ShowDialogInner(DLG_CLEAR_DATA, 0);
        }
    }
    else if (v == IView::Probe(mClearCacheButton)) {
        // Lazy initialization of observer
        if (mClearCacheObserver == NULL) {
            CInstalledAppDetailsClearCacheObserver::New((IFragment*)this, (IIPackageDataObserver**)&mClearCacheObserver);
        }
        mPm->DeleteApplicationCacheFiles(packageName, mClearCacheObserver);
    }
    else if (v == IView::Probe(mForceStopButton)) {
        ShowDialogInner(DLG_FORCE_STOP, 0);
        //ForceStopPackage(mAppInfo.packageName);
    }
    else if (v == IView::Probe(mMoveAppButton)) {
        if (mPackageMoveObserver == NULL) {
            CInstalledAppDetailsPackageMoveObserver::New((IFragment*)this, (IIPackageMoveObserver**)&mPackageMoveObserver);
        }
        mAppEntry->mInfo->GetFlags(&flags);
        Int32 moveFlags = (flags & IApplicationInfo::FLAG_EXTERNAL_STORAGE) != 0 ?
                IPackageManager::MOVE_INTERNAL : IPackageManager::MOVE_EXTERNAL_MEDIA;
        mMoveInProgress = TRUE;
        RefreshButtons();
        mPm->MovePackage(packageName, mPackageMoveObserver, moveFlags);
    }
    return NOERROR;
}

ECode CInstalledAppDetails::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    String packageName;
    IPackageItemInfo::Probe(mAppEntry->mInfo)->GetPackageName(&packageName);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&obj);
    IActivityManager* am = IActivityManager::Probe(obj);
    if (buttonView == ICompoundButton::Probe(mAskCompatibilityCB)) {
        am->SetPackageAskScreenCompat(packageName, isChecked);
    }
    else if (buttonView == ICompoundButton::Probe(mEnableCompatibilityCB)) {
        am->SetPackageScreenCompatMode(packageName, isChecked ?
                IActivityManager::COMPAT_MODE_ENABLED : IActivityManager::COMPAT_MODE_DISABLED);
    }
    else if (buttonView == mNotificationSwitch.Get()) {
        if (!isChecked) {
            ShowDialogInner(DLG_DISABLE_NOTIFICATIONS, 0);
        }
        else {
            SetNotificationsEnabled(TRUE);
        }
    }
    return NOERROR;
}

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos