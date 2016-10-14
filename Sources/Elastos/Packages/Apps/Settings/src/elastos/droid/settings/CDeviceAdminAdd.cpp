
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/settings/CDeviceAdminAdd.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
// using Elastos::Droid::App::Admin::CDeviceAdminInfo;
using Elastos::Droid::App::Admin::IDeviceAdminReceiver;
using Elastos::Droid::App::Admin::IDeviceAdminInfoPolicyInfo;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::CResolveInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Text::TextUtilsTruncateAt_END;
using Elastos::Droid::Text::TextUtilsTruncateAt_NONE;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IAppSecurityPermissionsHelper;
using Elastos::Droid::Widget::CAppSecurityPermissionsHelper;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CDeviceAdminAdd::ViewOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CDeviceAdminAdd::ViewOnClickListener, Object, IViewOnClickListener)

CDeviceAdminAdd::ViewOnClickListener::ViewOnClickListener(
    /* [in] */ CDeviceAdminAdd* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

CDeviceAdminAdd::ViewOnClickListener::~ViewOnClickListener()
{}

ECode CDeviceAdminAdd::ViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    switch (mId) {
        case 0:
            mHost->ToggleMessageEllipsis(v);
            break;
        case 1:
            Logger::D("CDeviceAdminAdd::ViewOnClickListener", "EventLog is TODO");
            // EventLog->WriteEvent(EventLogTags.EXP_DET_DEVICE_ADMIN_DECLINED_BY_USER,
            //     mHost->mDeviceAdmin->GetActivityInfo().applicationInfo.uid);
            mHost->Finish();
            break;
        case 2: {
            if (mHost->mAdding) {
                mHost->AddAndFinish();
            }
            else if (!mHost->mWaitingForRemoveMsg) {
                // try {
                // Don't allow the admin to put a dialog up in front
                // of us while we interact with the user.
                ActivityManagerNative::GetDefault()->StopAppSwitches();
                // } catch (RemoteException e) {
                // }
                mHost->mWaitingForRemoveMsg = TRUE;

                AutoPtr<IComponentName> comp;
                mHost->mDeviceAdmin->GetComponent((IComponentName**)&comp);
                AutoPtr<MyRemoteCallback> callback = new MyRemoteCallback();
                callback->constructor(mHost->mHandler, mHost);
                mHost->mDPM->GetRemoveWarning(comp, callback);
                // Don't want to wait too Int64.
                AutoPtr<IWindow> window;
                mHost->GetWindow((IWindow**)&window);
                AutoPtr<IView> view;
                window->GetDecorView((IView**)&view);
                AutoPtr<IHandler> handler;
                view->GetHandler((IHandler**)&handler);
                AutoPtr<MyRunnable> runnable = new MyRunnable(mHost);
                Boolean res;
                handler->PostDelayed(runnable, 2*1000, &res);
            }
            break;
        }
    }
    return NOERROR;
}

//===============================================================================
//                  CDeviceAdminAdd::MyRemoteCallback
//===============================================================================

CDeviceAdminAdd::MyRemoteCallback::MyRemoteCallback()
{}

ECode CDeviceAdminAdd::MyRemoteCallback::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ CDeviceAdminAdd* host)
{
    mHost = host;
    return RemoteCallback::constructor(handler);
}

void CDeviceAdminAdd::MyRemoteCallback::OnResult(
    /* [in] */ IBundle* bundle)
{
    AutoPtr<ICharSequence> msg;
    if (bundle != NULL) {
        bundle->GetCharSequence(
                IDeviceAdminReceiver::EXTRA_DISABLE_WARNING, (ICharSequence**)&msg);

    }
    mHost->ContinueRemoveAction(msg);
}

//===============================================================================
//                  CDeviceAdminAdd::MyRunnable
//===============================================================================

CDeviceAdminAdd::MyRunnable::MyRunnable(
    /* [in] */ CDeviceAdminAdd* host)
    : mHost(host)
{}

ECode CDeviceAdminAdd::MyRunnable::Run()
{
    mHost->ContinueRemoveAction(NULL);
    return NOERROR;
}

//===============================================================================
//                  CDeviceAdminAdd::DialogInterfaceOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CDeviceAdminAdd::DialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener)

CDeviceAdminAdd::DialogInterfaceOnClickListener::DialogInterfaceOnClickListener(
    /* [in] */ CDeviceAdminAdd* host)
    : mHost(host)
{}

ECode CDeviceAdminAdd::DialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    // try {
    ActivityManagerNative::GetDefault()->ResumeAppSwitches();
    // } catch (RemoteException e) {
    // }
    AutoPtr<IComponentName> comp;
    mHost->mDeviceAdmin->GetComponent((IComponentName**)&comp);
    mHost->mDPM->RemoveActiveAdmin(comp);
    mHost->Finish();
    return NOERROR;
}

//===============================================================================
//                  CDeviceAdminAdd::
//===============================================================================

const String CDeviceAdminAdd::TAG("CDeviceAdminAdd");

const Int32 CDeviceAdminAdd::DIALOG_WARNING = 1;

const Int32 CDeviceAdminAdd::MAX_ADD_MSG_LINES_PORTRAIT = 5;
const Int32 CDeviceAdminAdd::MAX_ADD_MSG_LINES_LANDSCAPE = 2;
const Int32 CDeviceAdminAdd::MAX_ADD_MSG_LINES = 15;

CAR_OBJECT_IMPL(CDeviceAdminAdd)

CDeviceAdminAdd::CDeviceAdminAdd()
    : mAddMsgEllipsized(TRUE)
    , mAdding(FALSE)
    , mRefreshing(FALSE)
    , mWaitingForRemoveMsg(FALSE)
    , mAddingProfileOwner(FALSE)
    , mCurSysAppOpMode(0)
    , mCurToastAppOpMode(0)
{}

CDeviceAdminAdd::~CDeviceAdminAdd()
{}

ECode CDeviceAdminAdd::constructor()
{
    CArrayList::New((IArrayList**)&mAddingPolicies);
    CArrayList::New((IArrayList**)&mActivePolicies);
    return Activity::constructor();
}

ECode CDeviceAdminAdd::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    AutoPtr<ILooper> looper;
    GetMainLooper((ILooper**)&looper);
    CHandler::New(looper, (IHandler**)&mHandler);

    AutoPtr<IInterface> obj;
    GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&obj);
    mDPM = IDevicePolicyManager::Probe(obj);
    obj = NULL;
    GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
    mAppOps = IAppOpsManager::Probe(obj);
    AutoPtr<IPackageManager> packageManager;
    GetPackageManager((IPackageManager**)&packageManager);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    Int32 flags;
    if ((intent->GetFlags(&flags), (flags & IIntent::FLAG_ACTIVITY_NEW_TASK)) != 0) {
        Logger::W(TAG, "Cannot start ADD_DEVICE_ADMIN as a new task");
        Finish();
        return NOERROR;
    }

    String action;
    intent->GetAction(&action);
    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(
            IDevicePolicyManager::EXTRA_DEVICE_ADMIN, (IParcelable**)&parcelable);
    IComponentName* who = IComponentName::Probe(parcelable);
    if (who == NULL) {
        Logger::W(TAG, "No component specified in %s", action.string());
        Finish();
        return NOERROR;
    }

    ECode ec;
    if (!action.IsNull() && action.Equals(IDevicePolicyManager::ACTION_SET_PROFILE_OWNER)) {
        SetResult(RESULT_CANCELED);
        SetFinishOnTouchOutside(TRUE);
        mAddingProfileOwner = TRUE;
        intent->GetStringExtra(IDevicePolicyManager::EXTRA_PROFILE_OWNER_NAME, &mProfileOwnerName);
        String callingPackage;
        GetCallingPackage(&callingPackage);
        String name;
        who->GetPackageName(&name);
        if (callingPackage.IsNull() || !callingPackage.Equals(name)) {
            Logger::E(TAG, "Unknown or incorrect caller");
            Finish();
            return NOERROR;
        }
        // try {
        AutoPtr<IPackageInfo> packageInfo;
        ec = packageManager->GetPackageInfo(callingPackage, 0, (IPackageInfo**)&packageInfo);
        if (ec == (ECode) E_NAME_NOT_FOUND_EXCEPTION) {
            Logger::E(TAG, "Cannot find the package %s", callingPackage.string());
            Finish();
            return NOERROR;
        }
        AutoPtr<IApplicationInfo> applicationInfo;
        packageInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        if ((applicationInfo->GetFlags(&flags), (flags & IApplicationInfo::FLAG_SYSTEM)) == 0) {
            Logger::E(TAG, "Cannot set a non-system app as a profile owner");
            Finish();
            return NOERROR;
        }
    }

    AutoPtr<IActivityInfo> ai;
    ec = packageManager->GetReceiverInfo(who, IPackageManager::GET_META_DATA, (IActivityInfo**)&ai);
    if (ec == (ECode) E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION) {
        Logger::W(TAG, "Unable to retrieve device policy %s, 08%08x", TO_CSTR(who), ec);
        Finish();
        return NOERROR;
    }

    // When activating, make sure the given component name is actually a valid device admin.
    // No need to check this when deactivating, because it is safe to deactivate an active
    // invalid device admin.
    Boolean res;
    if (mDPM->IsAdminActive(who, &res), !res) {
        AutoPtr<IIntent> otherIntent;
        CIntent::New(IDeviceAdminReceiver::ACTION_DEVICE_ADMIN_ENABLED, (IIntent**)&otherIntent);
        AutoPtr<IList> avail; // List<ResolveInfo>
        packageManager->QueryBroadcastReceivers(
                otherIntent, IPackageManager::GET_DISABLED_UNTIL_USED_COMPONENTS, (IList**)&avail);
        Int32 count = 0;
        if (avail != NULL) {
            avail->GetSize(&count);
        }
        Boolean found = FALSE;
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            avail->Get(i, (IInterface**)&obj);
            AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(obj);
            String aiPackageName, aiName, riPackageName, riName;
            IPackageItemInfo* aiPii = IPackageItemInfo::Probe(ai);
            aiPii->GetPackageName(&aiPackageName);
            aiPii->GetName(&aiName);
            AutoPtr<IActivityInfo> activityInfo;
            ri->GetActivityInfo((IActivityInfo**)&activityInfo);
            IPackageItemInfo* activityInfoPii = IPackageItemInfo::Probe(activityInfo);
            activityInfoPii->GetPackageName(&riPackageName);
            activityInfoPii->GetName(&riName);
            if (aiPackageName.Equals(riPackageName) && aiName.Equals(riName)) {
                // We didn't retrieve the meta data for all possible matches, so
                // need to use the activity info of this specific one that was retrieved.
                ri->SetActivityInfo(ai);
                AutoPtr<IDeviceAdminInfo> dpi;
                assert(0 && "TODO");
                // ec = CDeviceAdminInfo::New(this, ri, (IDeviceAdminInfo**)&dpi);
                // if (SUCCEEDED(ec)) {
                //     found = TRUE;
                // }
                // else if (ec == (ECode) E_XML_PULL_PARSER_EXCEPTION) {
                //     Logger::W(TAG, "Bad %s, 08%08x", TO_CSTR(activityInfo), ec);
                // }
                // else if (ec == (ECode) E_IO_EXCEPTION) {
                //     Logger::W(TAG, "Bad %s, 08%08x", TO_CSTR(activityInfo), ec);
                // }
                break;
            }
        }
        if (!found) {
            Logger::W(TAG, "Request to add invalid device admin: %s", TO_CSTR(who));
            Finish();
            return NOERROR;
        }
    }

    AutoPtr<IResolveInfo> ri;
    CResolveInfo::New((IResolveInfo**)&ri);
    ri->SetActivityInfo(ai);
    mDeviceAdmin = NULL;
    assert(0 && "TODO");
    // ec = CDeviceAdminInfo::New(this, ri, (IDeviceAdminInfo**)&mDeviceAdmin);
    // if (ec == (ECode) E_XML_PULL_PARSER_EXCEPTION) {
    //     Logger::W(TAG, "Unable to retrieve device policy %s, 08%08x", TO_CSTR(who), ec);
    //     Finish();
    //     return NOERROR;
    // }
    // else if (ec == (ECode) E_IO_EXCEPTION) {
    //     Logger::W(TAG, "Unable to retrieve device policy %s, 08%08x", TO_CSTR(who), ec);
    //     Finish();
    //     return NOERROR;
    // }

    // This admin already exists, an we have two options at this point.  If new policy
    // bits are set, show the user the new list.  If nothing has changed, simply return
    // "OK" immediately.
    intent = NULL;
    GetIntent((IIntent**)&intent);
    intent->GetAction(&action);
    if (IDevicePolicyManager::ACTION_ADD_DEVICE_ADMIN.Equals(action)) {
        mRefreshing = FALSE;
        if (mDPM->IsAdminActive(who, &res), res) {
            AutoPtr<IArrayList> newPolicies;//ArrayList<IDeviceAdminInfoPolicyInfo>
            mDeviceAdmin->GetUsedPolicies((IArrayList**)&newPolicies);
            Int32 size;
            newPolicies->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                newPolicies->Get(i, (IInterface**)&obj);
                IDeviceAdminInfoPolicyInfo* pi = IDeviceAdminInfoPolicyInfo::Probe(obj);
                Int32 ident;
                pi->GetIdent(&ident);
                if (mDPM->HasGrantedPolicy(who, ident, &res), !res) {
                    mRefreshing = TRUE;
                    break;
                }
            }
            if (!mRefreshing) {
                // Nothing changed (or policies were removed) - return immediately
                SetResult(IActivity::RESULT_OK);
                Finish();
                return NOERROR;
            }
        }
    }

    // If we're trying to add a profile owner and user setup hasn't completed yet, no
    // need to prompt for permission. Just add and finish.
    if (mAddingProfileOwner && (mDPM->HasUserSetupCompleted(&res), !res)) {
        AddAndFinish();
        return NOERROR;
    }

    intent->GetCharSequenceExtra(IDevicePolicyManager::EXTRA_ADD_EXPLANATION, (ICharSequence**)&mAddMsgText);

    SetContentView(R::layout::device_admin_add);

    AutoPtr<IView> tmp;
    FindViewById(R::id::admin_icon, (IView**)&tmp);
    mAdminIcon = IImageView::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::admin_name, (IView**)&tmp);
    mAdminName = ITextView::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::admin_description, (IView**)&tmp);
    mAdminDescription = ITextView::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::profile_owner_warning, (IView**)&tmp);
    mProfileOwnerWarning = ITextView::Probe(tmp);

    AutoPtr<IView> addMsgTmp;
    FindViewById(R::id::add_msg, (IView**)&addMsgTmp);
    mAddMsg = ITextView::Probe(addMsgTmp);
    tmp = NULL;
    FindViewById(R::id::add_msg_expander, (IView**)&tmp);
    mAddMsgExpander = IImageView::Probe(tmp);
    AutoPtr<ViewOnClickListener> listener = new ViewOnClickListener(this, 0);
    addMsgTmp->SetOnClickListener(listener);

    // toggleMessageEllipsis also handles initial layout:
    ToggleMessageEllipsis(addMsgTmp);

    tmp = NULL;
    FindViewById(R::id::admin_warning, (IView**)&tmp);
    mAdminWarning = ITextView::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::admin_policies, (IView**)&tmp);
    mAdminPolicies = IViewGroup::Probe(tmp);
    AutoPtr<IView> cancelButtonTmp;
    FindViewById(R::id::cancel_button, (IView**)&cancelButtonTmp);
    mCancelButton = IButton::Probe(cancelButtonTmp);
    AutoPtr<ViewOnClickListener> listener1 = new ViewOnClickListener(this, 1);
    cancelButtonTmp->SetOnClickListener(listener1);
    AutoPtr<IView> actionButtonTmp;
    FindViewById(R::id::action_button, (IView**)&actionButtonTmp);
    mActionButton = IButton::Probe(actionButtonTmp);
    AutoPtr<ViewOnClickListener> listener2 = new ViewOnClickListener(this, 2);
    actionButtonTmp->SetOnClickListener(listener2);
    return NOERROR;
}

void CDeviceAdminAdd::AddAndFinish()
{
    AutoPtr<IComponentName> comp;
    mDeviceAdmin->GetComponent((IComponentName**)&comp);
    Boolean res;
    ECode ec = mDPM->SetActiveAdmin(comp, mRefreshing);
    if (SUCCEEDED(ec)){
        Logger::D("CDeviceAdminAdd::AddAndFinish", "EventLog is TODO");
        // EventLog->WriteEvent(EventLogTags.EXP_DET_DEVICE_ADMIN_ACTIVATED_BY_USER,
        //     mDeviceAdmin->GetActivityInfo().applicationInfo.uid);
        SetResult(IActivity::RESULT_OK);
    }
    else if (ec == (ECode) E_RUNTIME_EXCEPTION) {
        // Something bad happened...  could be that it was
        // already set, though.
        Logger::W(TAG, "Exception trying to activate admin %s", TO_CSTR(comp), ec);
        if (mDPM->IsAdminActive(comp, &res), res) {
            SetResult(IActivity::RESULT_OK);
        }
    }

    if (mAddingProfileOwner) {
        ec = mDPM->SetProfileOwner(comp, mProfileOwnerName, UserHandle::GetMyUserId(), &res);
        if (ec == (ECode) E_RUNTIME_EXCEPTION) {
            SetResult(IActivity::RESULT_CANCELED);
        }
    }
    Finish();
}

void CDeviceAdminAdd::ContinueRemoveAction(
    /* [in] */ ICharSequence* msg)
{
    if (!mWaitingForRemoveMsg) {
        return;
    }
    mWaitingForRemoveMsg = FALSE;
    ECode ec;
    if (msg == NULL) {
        // try {
        ActivityManagerNative::GetDefault()->ResumeAppSwitches();
        // } catch (RemoteException e) {
        // }
        AutoPtr<IComponentName> comp;
        mDeviceAdmin->GetComponent((IComponentName**)&comp);
        mDPM->RemoveActiveAdmin(comp);
        Finish();
    }
    else {
        // try {
            // Continue preventing anything from coming in front.
        ActivityManagerNative::GetDefault()->StopAppSwitches();
        // } catch (RemoteException e) {
        // }
        AutoPtr<IBundle> args;
        CBundle::New((IBundle**)&args);
        args->PutCharSequence(
                IDeviceAdminReceiver::EXTRA_DISABLE_WARNING, msg);
        Boolean res;
        ShowDialog(DIALOG_WARNING, args, &res);
    }
}

ECode CDeviceAdminAdd::OnResume()
{
    Activity::OnResume();
    UpdateInterface();
    // As Int64 as we are running, don't let this admin overlay stuff on top of the screen.
    AutoPtr<IActivityInfo> activityInfo;
    mDeviceAdmin->GetActivityInfo((IActivityInfo**)&activityInfo);
    AutoPtr<IApplicationInfo> applicationInfo;
    IComponentInfo::Probe(activityInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    Int32 uid;
    applicationInfo->GetUid(&uid);
    String pkg;
    IPackageItemInfo::Probe(applicationInfo)->GetPackageName(&pkg);

    mAppOps->CheckOp(IAppOpsManager::OP_SYSTEM_ALERT_WINDOW, uid, pkg, &mCurSysAppOpMode);
    mAppOps->CheckOp(IAppOpsManager::OP_TOAST_WINDOW, uid, pkg, &mCurToastAppOpMode);
    mAppOps->SetMode(IAppOpsManager::OP_SYSTEM_ALERT_WINDOW, uid, pkg, IAppOpsManager::MODE_IGNORED);
    mAppOps->SetMode(IAppOpsManager::OP_TOAST_WINDOW, uid, pkg, IAppOpsManager::MODE_IGNORED);
    return NOERROR;
}

ECode CDeviceAdminAdd::OnPause()
{
    Activity::OnPause();
    // As Int64 as we are running, don't let this admin overlay stuff on top of the screen.
    AutoPtr<IActivityInfo> activityInfo;
    mDeviceAdmin->GetActivityInfo((IActivityInfo**)&activityInfo);
    AutoPtr<IApplicationInfo> applicationInfo;
    IComponentInfo::Probe(activityInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    Int32 uid;
    applicationInfo->GetUid(&uid);
    String pkg;
    IPackageItemInfo::Probe(applicationInfo)->GetPackageName(&pkg);

    mAppOps->SetMode(IAppOpsManager::OP_SYSTEM_ALERT_WINDOW, uid, pkg, mCurSysAppOpMode);
    mAppOps->SetMode(IAppOpsManager::OP_TOAST_WINDOW, uid, pkg, mCurToastAppOpMode);
    // try {
    ActivityManagerNative::GetDefault()->ResumeAppSwitches();
    // } catch (RemoteException e) {
    // }
    return NOERROR;
}

AutoPtr<IDialog> CDeviceAdminAdd::OnCreateDialog(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* args)
{
    switch (id) {
        case DIALOG_WARNING: {
            AutoPtr<ICharSequence> msg;
            args->GetCharSequence(IDeviceAdminReceiver::EXTRA_DISABLE_WARNING, (ICharSequence**)&msg);
            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
            builder->SetMessage(msg);
            AutoPtr<DialogInterfaceOnClickListener> listener = new DialogInterfaceOnClickListener(this);
            builder->SetPositiveButton(R::string::dlg_ok, listener);
            builder->SetNegativeButton(R::string::dlg_cancel, NULL);
            AutoPtr<IAlertDialog> dialog;
            builder->Create((IAlertDialog**)&dialog);
            return IDialog::Probe(dialog);
        }
        default:
            return Activity::OnCreateDialog(id, args);

    }
}

void CDeviceAdminAdd::SetViewVisibility(
    /* [in] */ IArrayList* views, //ArrayList<View>
    /* [in] */ Int32 visibility)
{
    Int32 N;
    views->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        views->Get(i, (IInterface**)&obj);
        IView::Probe(obj)->SetVisibility(visibility);
    }
}

void CDeviceAdminAdd::UpdateInterface()
{
    AutoPtr<IPackageManager> pm;
    GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IDrawable> drawable;
    mDeviceAdmin->LoadIcon(pm, (IDrawable**)&drawable);
    mAdminIcon->SetImageDrawable(drawable);
    AutoPtr<ICharSequence> cs;
    mDeviceAdmin->LoadLabel(pm, (ICharSequence**)&cs);
    mAdminName->SetText(cs);
    // try {
    cs = NULL;
    ECode ec = mDeviceAdmin->LoadDescription(pm, (ICharSequence**)&cs);
    if (SUCCEEDED(ec)) {
        mAdminDescription->SetText(cs);
        IView::Probe(mAdminDescription)->SetVisibility(IView::VISIBLE);
    }
    else { //Resources.NotFoundException
        IView::Probe(mAdminDescription)->SetVisibility(IView::GONE);
    }
    if (mAddingProfileOwner) {
        IView::Probe(mProfileOwnerWarning)->SetVisibility(IView::VISIBLE);
    }
    if (mAddMsgText != NULL) {
        mAddMsg->SetText(mAddMsgText);
        IView::Probe(mAddMsg)->SetVisibility(IView::VISIBLE);
    }
    else {
        IView::Probe(mAddMsg)->SetVisibility(IView::GONE);
        IView::Probe(mAddMsgExpander)->SetVisibility(IView::GONE);
    }

    AutoPtr<IComponentName> comp;
    AutoPtr<ICharSequence> text, text1;
    Boolean res;
    Int32 size;
    String str;
    AutoPtr<IView> view;

    AutoPtr<IAppSecurityPermissionsHelper> helper;
    CAppSecurityPermissionsHelper::AcquireSingleton((IAppSecurityPermissionsHelper**)&helper);

    AutoPtr<IActivityInfo> activityInfo;
    mDeviceAdmin->GetActivityInfo((IActivityInfo**)&activityInfo);
    AutoPtr<IApplicationInfo> applicationInfo;
    IComponentInfo::Probe(activityInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    AutoPtr<ICharSequence> seq;
    IPackageItemInfo::Probe(applicationInfo)->LoadLabel(pm, (ICharSequence**)&seq);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, seq);

    if (!mRefreshing && !mAddingProfileOwner
            && (mDPM->IsAdminActive((mDeviceAdmin->GetComponent((IComponentName**)&comp), comp), &res), res) ) {
        if (mActivePolicies->GetSize(&size), size == 0) {
            AutoPtr<IArrayList> policies;//ArrayList<IDeviceAdminInfoPolicyInfo>
            mDeviceAdmin->GetUsedPolicies((IArrayList**)&policies);
            for (Int32 i = 0; i < (policies->GetSize(&size), size); i++) {
                AutoPtr<IInterface> obj;
                policies->Get(i, (IInterface**)&obj);
                IDeviceAdminInfoPolicyInfo* pi = IDeviceAdminInfoPolicyInfo::Probe(obj);
                Int32 label;
                pi->GetLabel(&label);
                text = NULL;
                GetText(label, (ICharSequence**)&text);
                view = NULL;
                helper->GetPermissionItemView(
                        this, text, CoreUtils::Convert(""), TRUE, (IView**)&view);
                mActivePolicies->Add(view);
                mAdminPolicies->AddView(view);
            }
        }
        SetViewVisibility(mActivePolicies, IView::VISIBLE);
        SetViewVisibility(mAddingPolicies, IView::GONE);
        GetString(R::string::device_admin_status, args, &str);
        mAdminWarning->SetText(CoreUtils::Convert(str));
        text = NULL;
        GetText(R::string::active_device_admin_msg, (ICharSequence**)&text);
        SetTitle(text);
        text = NULL;
        GetText(R::string::remove_device_admin, (ICharSequence**)&text);
        ITextView::Probe(mActionButton)->SetText(text);
        mAdding = FALSE;
    }
    else {
        if (mAddingPolicies->GetSize(&size), size == 0) {
            AutoPtr<IArrayList> policies; //ArrayList<IDeviceAdminInfoPolicyInfo>
            mDeviceAdmin->GetUsedPolicies((IArrayList**)&policies);
            for (Int32 i = 0; i < (policies->GetSize(&size), size); i++) {
                AutoPtr<IInterface> obj;
                policies->Get(i, (IInterface**)&obj);
                IDeviceAdminInfoPolicyInfo* pi = IDeviceAdminInfoPolicyInfo::Probe(obj);
                Int32 label, description;
                pi->GetLabel(&label);
                pi->GetDescription(&description);
                text = NULL;
                GetText(label, (ICharSequence**)&text);
                text1 = NULL;
                GetText(description, (ICharSequence**)&text1);
                view = NULL;
                helper->GetPermissionItemView(this, text, text1, TRUE, (IView**)&view);
                mAddingPolicies->Add(view);
                mAdminPolicies->AddView(view);
            }
        }
        SetViewVisibility(mAddingPolicies, IView::VISIBLE);
        SetViewVisibility(mActivePolicies, IView::GONE);
        GetString(R::string::device_admin_warning, args, &str);
        mAdminWarning->SetText(CoreUtils::Convert(str));
        if (mAddingProfileOwner) {
            text = NULL;
            GetText(R::string::profile_owner_add_title, (ICharSequence**)&text);
            SetTitle(text);
        }
        else {
            text = NULL;
            GetText(R::string::add_device_admin_msg, (ICharSequence**)&text);
            SetTitle(text);
        }
        text = NULL;
        GetText(R::string::add_device_admin, (ICharSequence**)&text);
        ITextView::Probe(mActionButton)->SetText(text);
        mAdding = TRUE;
    }
}

void CDeviceAdminAdd::ToggleMessageEllipsis(
    /* [in] */ IView* v)
{
    AutoPtr<ITextView> tv = ITextView::Probe(v);

    mAddMsgEllipsized = ! mAddMsgEllipsized;
    tv->SetEllipsize(mAddMsgEllipsized ? TextUtilsTruncateAt_END : TextUtilsTruncateAt_NONE);
    tv->SetMaxLines(mAddMsgEllipsized ? GetEllipsizedLines() : MAX_ADD_MSG_LINES);

    mAddMsgExpander->SetImageResource(mAddMsgEllipsized ?
        Elastos::Droid::R::drawable::expander_ic_minimized :
        Elastos::Droid::R::drawable::expander_ic_maximized);
}

Int32 CDeviceAdminAdd::GetEllipsizedLines()
{
    AutoPtr<IInterface> obj;
    GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    AutoPtr<IDisplay> d;
    IWindowManager::Probe(obj)->GetDefaultDisplay((IDisplay**)&d);

    Int32 height, width;
    return (d->GetHeight(&height), height) > (d->GetWidth(&width), width) ?
        MAX_ADD_MSG_LINES_PORTRAIT : MAX_ADD_MSG_LINES_LANDSCAPE;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos