
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/settings/applications/CRunningServiceDetails.h"
#include "elastos/droid/settings/applications/CRunningServiceDetailsAlertDialogFragment.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::CApplicationErrorReportHelper;
using Elastos::Droid::App::CApplicationErrorReport;
using Elastos::Droid::App::IApplicationErrorReportHelper;
using Elastos::Droid::App::IApplicationErrorReport;
using Elastos::Droid::App::IApplicationErrorReportRunningServiceInfo;
using Elastos::Droid::App::CApplicationErrorReportRunningServiceInfo;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IProviderInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Res::IResources;
// using Elastos::Droid::Os::IDebug;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

//===============================================================================
//                  CRunningServiceDetails::ActiveDetail
//===============================================================================

CAR_INTERFACE_IMPL(CRunningServiceDetails::ActiveDetail, Object, IViewOnClickListener)

CRunningServiceDetails::ActiveDetail::ActiveDetail(
    /* [in] */ CRunningServiceDetails* host)
    : mHost(host)
{}

void CRunningServiceDetails::ActiveDetail::StopActiveService(
    /* [in] */ Boolean confirmed)
{
    AutoPtr<RunningState::ServiceItem> si = mServiceItem;
    if (!confirmed) {
        AutoPtr<IApplicationInfo> info;
        IComponentInfo::Probe(si->mServiceInfo)->GetApplicationInfo((IApplicationInfo**)&info);
        Int32 flags;
        info->GetFlags(&flags);
        if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
            AutoPtr<IComponentName> service;
            si->mRunningService->GetService((IComponentName**)&service);
            mHost->ShowConfirmStopDialog(service);
            return;
        }
    }
    AutoPtr<IComponentName> service;
    si->mRunningService->GetService((IComponentName**)&service);
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetComponent(service);
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    Boolean res;
    IContext::Probe(activity)->StopService(intent, &res);
    Int32 size;
    if (mHost->mMergedItem == NULL) {
        // If this is gone, we are gone.
        mHost->mState->UpdateNow();
        mHost->Finish();
    }
    else if (!mHost->mShowBackground && (mHost->mMergedItem->mServices->GetSize(&size), size) <= 1) {
        // If there was only one service, we are finishing it,
        // so no reason for the UI to stick around.
        mHost->mState->UpdateNow();
        mHost->Finish();
    }
    else {
        mHost->mState->UpdateNow();
    }
}

ECode CRunningServiceDetails::ActiveDetail::OnClick(
    /* [in] */ IView* v)
{
    ECode ec = NOERROR;
    if (v == IView::Probe(mReportButton)) {
        AutoPtr<IApplicationErrorReport> report;
        CApplicationErrorReport::New((IApplicationErrorReport**)&report);
        report->SetType(IApplicationErrorReport::TYPE_RUNNING_SERVICE);
        String packageName;
        IPackageItemInfo::Probe(mServiceItem->mServiceInfo)->GetPackageName(&packageName);
        report->SetPackageName(packageName);
        String installerPackageName;
        mInstaller->GetPackageName(&installerPackageName);
        report->SetInstallerPackageName(installerPackageName);
        String process;
        mServiceItem->mRunningService->GetProcess(&process);
        report->SetProcessName(process);
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        Int64 currentTime;
        sys->GetCurrentTimeMillis(&currentTime);
        report->SetTime(currentTime);

        AutoPtr<IApplicationInfo> applicationInfo;
        IComponentInfo::Probe(mServiceItem->mServiceInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        Int32 flags;
        applicationInfo->GetFlags(&flags);
        report->SetSystemApp((flags & IApplicationInfo::FLAG_SYSTEM) != 0);
        AutoPtr<IApplicationErrorReportRunningServiceInfo> info;
        CApplicationErrorReportRunningServiceInfo::New((IApplicationErrorReportRunningServiceInfo**)&info);
        if (mActiveItem->mFirstRunTime >= 0) {
            info->SetDurationMillis(SystemClock::GetElapsedRealtime() - mActiveItem->mFirstRunTime);
        }
        else {
            info->SetDurationMillis(-1);
        }
        String name;
        IPackageItemInfo::Probe(mServiceItem->mServiceInfo)->GetName(&name);
        AutoPtr<IComponentName> comp;
        CComponentName::New(packageName, name, (IComponentName**)&comp);

        AutoPtr<IActivity> activity;
        mHost->GetActivity((IActivity**)&activity);
        AutoPtr<IFile> filename;
        IContext::Probe(activity)->GetFileStreamPath(String("service_dump.txt"), (IFile**)&filename);
        AutoPtr<IFileOutputStream> output;
        // try {
            ec = CFileOutputStream::New(filename, (IFileOutputStream**)&output);
            if (SUCCEEDED(ec)) {
                Logger::W("CRunningServiceDetails::ActiveDetail::OnClick()", "TODO, CDebug::DumpService is not implemented");
            //     Debug->DumpService("activity", output->GetFD(),
            //             new String[] { "-a", "service", comp->FlattenToString() });
            }
            else {
                Logger::W(TAG, "Can't dump service: %s", TO_CSTR(comp));
            }
        // } catch (IOException e) {
        //     Logger::W(TAG, "Can't dump service: " + comp, e);
        // } finally {
            if (output != NULL) {
                //try {
                ICloseable::Probe(output)->Close();
                //} catch (IOException e) {}
            }
        // }

        AutoPtr<IInputStream> input;
        // try {
            ec = CFileInputStream::New(filename, (IInputStream**)&input);
            if (SUCCEEDED(ec)) {
                Int64 len;
                filename->GetLength(&len);
                AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc((Int32) len);
                Int32 number;
                input->Read(buffer, &number);
                String serviceDetails(*buffer);
                info->SetServiceDetails(serviceDetails);
            }
            else {
                Logger::W(TAG, "Can't read service dump: %s", TO_CSTR(comp));
            }
        // } catch (IOException e) {
        //     Logger::W(TAG, "Can't read service dump: " + comp, e);
        // } finally {
            if (input != NULL) {
                // try {
                ICloseable::Probe(input)->Close();
                // } catch (IOException e) {}
            }
        // }

        filename->Delete();
        String serviceDetails;
        info->GetServiceDetails(&serviceDetails);
        Logger::I(TAG, "Details: %s", serviceDetails.string());
        report->SetRunningServiceInfo(info);
        AutoPtr<IIntent> result;
        CIntent::New(IIntent::ACTION_APP_ERROR, (IIntent**)&result);
        result->SetComponent(mInstaller);
        result->PutExtra(IIntent::EXTRA_BUG_REPORT, IParcelable::Probe(report));
        result->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        mHost->StartActivity(result);
        return NOERROR;
    }

    if (mManageIntent != NULL) {
        // try {
            AutoPtr<IIntentSender> intentSender;
            mManageIntent->GetIntentSender((IIntentSender**)&intentSender);
            AutoPtr<IActivity> activity;
            mHost->GetActivity((IActivity**)&activity);
            ec = IContext::Probe(activity)->StartIntentSender(intentSender, NULL,
                    IIntent::FLAG_ACTIVITY_NEW_TASK
                            | IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET,
                    IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET, 0);
            if (FAILED(ec)) {
                Logger::W(TAG, "08%08x", ec);
            }
        // } catch (IntentSender.SendIntentException e) {
        //     Logger::W(TAG, e);
        // } catch (IllegalArgumentException e) {
        //     Logger::W(TAG, e);
        // } catch (ActivityNotFoundException e) {
        //     Logger::W(TAG, e);
        // }
    }
    else if (mServiceItem != NULL) {
        StopActiveService(FALSE);
    }
    else if (mActiveItem->mItem->mBackground) {
        // Background process.  Just kill it.
        String packageName;
        mActiveItem->mItem->mPackageInfo->GetPackageName(&packageName);
        mHost->mAm->KillBackgroundProcesses(packageName);
        mHost->Finish();
    }
    else {
        String packageName;
        mActiveItem->mItem->mPackageInfo->GetPackageName(&packageName);
        // Heavy-weight process.  We'll do a force-stop on it.
        mHost->mAm->ForceStopPackage(packageName);
        mHost->Finish();
    }
    return NOERROR;
}


//===============================================================================
//                  CRunningServiceDetails::MyAlertDialogFragment
//===============================================================================

ECode CRunningServiceDetails::MyAlertDialogFragment::constructor()
{
    return DialogFragment::constructor();
}

AutoPtr<IDialogFragment> CRunningServiceDetails::MyAlertDialogFragment::NewConfirmStop(
    /* [in] */ Int32 id,
    /* [in] */ IComponentName* comp)
{
    AutoPtr<IDialogFragment> frag;
    CRunningServiceDetailsAlertDialogFragment::New((IDialogFragment**)&frag);
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    args->PutInt32(String("id"), id);
    args->PutParcelable(String("comp"), IParcelable::Probe(comp));
    IFragment::Probe(frag)->SetArguments(args);
    return frag;
}

AutoPtr<CRunningServiceDetails> CRunningServiceDetails::MyAlertDialogFragment::GetOwner()
{
    AutoPtr<IFragment> frag;
    GetTargetFragment((IFragment**)&frag);
    AutoPtr<CRunningServiceDetails> result = (CRunningServiceDetails*)frag.Get();
    return result;
}

ECode CRunningServiceDetails::MyAlertDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IBundle> bundles;
    GetArguments((IBundle**)&bundles);
    Int32 id;
    bundles->GetInt32(String("id"), &id);
    switch (id) {
        case DIALOG_CONFIRM_STOP: {
            AutoPtr<IParcelable> parcelable;
            bundles->GetParcelable(String("comp"), (IParcelable**)&parcelable);
            AutoPtr<IComponentName> comp = IComponentName::Probe(parcelable);
            if (GetOwner()->ActiveDetailForService(comp) == NULL) {
                return NOERROR;
            }

            AutoPtr<IActivity> _activity;
            GetActivity((IActivity**)&_activity);
            IContext* activity = IContext::Probe(_activity);
            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(activity, (IAlertDialogBuilder**)&builder);
            String str;
            activity->GetString(R::string::runningservicedetails_stop_dlg_title, &str);
            builder->SetTitle(CoreUtils::Convert(str));
            activity->GetString(R::string::runningservicedetails_stop_dlg_text, &str);
            builder->SetMessage(CoreUtils::Convert(str));

            AutoPtr<MyOnClickListener> listener = new MyOnClickListener(this, comp);
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
//                  CRunningServiceDetails::MyOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CRunningServiceDetails::MyOnClickListener, Object, IDialogInterfaceOnClickListener)

CRunningServiceDetails::MyOnClickListener::MyOnClickListener(
    /* [in] */ MyAlertDialogFragment* host,
    /* [in] */ IComponentName* comp)
    : mHost(host)
    , mComp(comp)
{}

ECode CRunningServiceDetails::MyOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<ActiveDetail> ad = mHost->GetOwner()->ActiveDetailForService(mComp);
    if (ad != NULL) {
        ad->StopActiveService(TRUE);
    }
    return NOERROR;
}


//===============================================================================
//                  CRunningServiceDetails::MyRunnable
//===============================================================================

CRunningServiceDetails::MyRunnable::MyRunnable(
    /* [in] */ CRunningServiceDetails* host)
    : mHost(host)
{}

ECode CRunningServiceDetails::MyRunnable::Run()
{
    AutoPtr<IActivity> a;
    mHost->GetActivity((IActivity**)&a);
    if (a != NULL) {
        a->OnBackPressed();
    }
    return NOERROR;
}


//===============================================================================
//                  CRunningServiceDetails
//===============================================================================
const String CRunningServiceDetails::TAG("CRunningServiceDetails");

const String CRunningServiceDetails::KEY_UID("uid");
const String CRunningServiceDetails::KEY_USER_ID("user_id");
const String CRunningServiceDetails::KEY_PROCESS("process");
const String CRunningServiceDetails::KEY_BACKGROUND("background");

const Int32 CRunningServiceDetails::DIALOG_CONFIRM_STOP;

CAR_INTERFACE_IMPL(CRunningServiceDetails, Fragment, IRunningStateOnRefreshUiListener)

CAR_OBJECT_IMPL(CRunningServiceDetails)

CRunningServiceDetails::CRunningServiceDetails()
    : mHaveData(FALSE)
    , mUid(0)
    , mUserId(0)
    , mShowBackground(FALSE)
    , mNumServices(0)
    , mNumProcesses(0)
{
    CArrayList::New((IArrayList**)&mActiveDetails);
    mBuilder = new StringBuilder(128);
}

ECode CRunningServiceDetails::constructor()
{
    return Fragment::constructor();
}

Boolean CRunningServiceDetails::FindMergedItem()
{
    AutoPtr<RunningState::MergedItem> item;
    AutoPtr<IArrayList> newItems;// ArrayList<RunningState.MergedItem>
    if (mShowBackground) {
        newItems = mState->GetCurrentBackgroundItems();
    }
    else {
        newItems = mState->GetCurrentMergedItems();
    }
    if (newItems != NULL) {
        Int32 size;
        newItems->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            newItems->Get(i, (IInterface**)&obj);
            AutoPtr<RunningState::MergedItem> mi = (RunningState::MergedItem*)IRunningStateMergedItem::Probe(obj);
            if (mi->mUserId != mUserId) {
                continue;
            }
            if (mUid >= 0 && mi->mProcess != NULL && mi->mProcess->mUid != mUid) {
                continue;
            }
            if (mProcessName.IsNull() || (mi->mProcess != NULL
                    && mProcessName.Equals(mi->mProcess->mProcessName))) {
                item = mi;
                break;
            }
        }
    }

    if (mMergedItem != item) {
        mMergedItem = item;
        return TRUE;
    }
    return FALSE;
}

void CRunningServiceDetails::AddServicesHeader()
{
    if (mNumServices == 0) {
        AutoPtr<IView> tmp;
        mInflater->Inflate(R::layout::separator_label, mAllDetails, FALSE, (IView**)&tmp);
        mServicesHeader = ITextView::Probe(tmp);
        mServicesHeader->SetText(R::string::runningservicedetails_services_title);
        mAllDetails->AddView(tmp);
    }
    mNumServices++;
}

void CRunningServiceDetails::AddProcessesHeader()
{
    if (mNumProcesses == 0) {
        AutoPtr<IView> tmp;
        mInflater->Inflate(R::layout::separator_label, mAllDetails, FALSE, (IView**)&tmp);
        mProcessesHeader = ITextView::Probe(tmp);
        mProcessesHeader->SetText(R::string::runningservicedetails_processes_title);
        mAllDetails->AddView(tmp);
    }
    mNumProcesses++;
}

void CRunningServiceDetails::AddServiceDetailsView(
    /* [in] */ RunningState::ServiceItem* si,
    /* [in] */ RunningState::MergedItem* mi,
    /* [in] */ Boolean isService,
    /* [in] */ Boolean inclDetails)
{
    if (isService) {
        AddServicesHeader();
    }
    else if (mi->mUserId != UserHandle::GetMyUserId()) {
        // This is being called for another user, and is not a service...
        // That is, it is a background processes, being added for the
        // details of a user.  In this case we want a header for processes,
        // since the top subject line is for the user.
        AddProcessesHeader();
    }

    AutoPtr<RunningState::BaseItem> bi = si != NULL ? (RunningState::BaseItem*)si : (RunningState::BaseItem*)mi;

    AutoPtr<ActiveDetail> detail = new ActiveDetail(this);
    AutoPtr<IView> root;
    mInflater->Inflate(R::layout::running_service_details_service,
            mAllDetails, FALSE, (IView**)&root);
    mAllDetails->AddView(root);
    detail->mRootView = root;
    detail->mServiceItem = si;
    detail->mViewHolder = new CRunningProcessesView::ViewHolder(root);
    // TODO:
    detail->mActiveItem = detail->mViewHolder->Bind(mState, mMergedItem/*bi*/, mBuilder->ToString());

    if (!inclDetails) {
        AutoPtr<IView> tmp;
        root->FindViewById(R::id::service, (IView**)&tmp);
        tmp->SetVisibility(IView::GONE);
    }

    Int32 clientLabel;
    if (si != NULL && (si->mRunningService->GetClientLabel(&clientLabel), clientLabel) != 0) {
        AutoPtr<IComponentName> service;
        si->mRunningService->GetService((IComponentName**)&service);
        mAm->GetRunningServiceControlPanel(service, (IPendingIntent**)&(detail->mManageIntent));
    }

    AutoPtr<IView> tmp;
    root->FindViewById(R::id::comp_description, (IView**)&tmp);
    AutoPtr<ITextView> description = ITextView::Probe(tmp);
    tmp = NULL;
    root->FindViewById(R::id::left_button, (IView**)&tmp);
    detail->mStopButton = IButton::Probe(tmp);
    tmp = NULL;
    root->FindViewById(R::id::right_button, (IView**)&tmp);
    detail->mReportButton = IButton::Probe(tmp);

    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    if (isService && mi->mUserId != UserHandle::GetMyUserId()) {
        // For services from other users, we don't show any description or
        // controls, because the current user can not perform
        // actions on them.
        IView::Probe(description)->SetVisibility(IView::GONE);
        tmp = NULL;
        root->FindViewById(R::id::control_buttons_panel, (IView**)&tmp);
        tmp->SetVisibility(IView::GONE);
    }
    else {
        Int32 descriptionRes;
        if (si != NULL &&
                (IComponentInfo::Probe(si->mServiceInfo)->GetDescriptionRes(&descriptionRes), descriptionRes) != 0) {
            String packageName;
            IPackageItemInfo::Probe(si->mServiceInfo)->GetPackageName(&packageName);
            AutoPtr<IApplicationInfo> applicationInfo;
            IComponentInfo::Probe(si->mServiceInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);

            AutoPtr<IPackageManager> packageManager;
            activity->GetPackageManager((IPackageManager**)&packageManager);

            AutoPtr<ICharSequence> cs;
            packageManager->GetText(
                    packageName, descriptionRes, applicationInfo, (ICharSequence**)&cs);
            description->SetText(cs);
        }
        else {
            if (mi->mBackground) {
                description->SetText(R::string::background_process_stop_description);
            }
            else if (detail->mManageIntent != NULL) {
                // try {
                    AutoPtr<IPackageManager> packageManager;
                    activity->GetPackageManager((IPackageManager**)&packageManager);

                    String clientPackage;
                    si->mRunningService->GetClientPackage(&clientPackage);
                    AutoPtr<IResources> clientr;
                    ECode ec = packageManager->GetResourcesForApplication(
                            clientPackage, (IResources**)&clientr);
                    if (SUCCEEDED(ec)) {
                        Int32 clientLabel;
                        si->mRunningService->GetClientLabel(&clientLabel);
                        String label;
                        clientr->GetString(clientLabel, &label);
                        AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
                        args->Set(0, CoreUtils::Convert(label));
                        String str;
                        activity->GetString(R::string::service_manage_description, args, &str);
                        description->SetText(CoreUtils::Convert(str));
                    }
                // } catch (PackageManager.NameNotFoundException e) {
                // }
            }
            else {
                AutoPtr<ICharSequence> text;
                activity->GetText(si != NULL
                        ? R::string::service_stop_description
                        : R::string::heavy_weight_stop_description, (ICharSequence**)&text);
                description->SetText(text);
            }
        }

        IView::Probe(detail->mStopButton)->SetOnClickListener(detail);
        AutoPtr<ICharSequence> text;
        activity->GetText(detail->mManageIntent != NULL
                ? R::string::service_manage : R::string::service_stop, (ICharSequence**)&text);
        ITextView::Probe(detail->mStopButton)->SetText(text);
        IView::Probe(detail->mReportButton)->SetOnClickListener(detail);
        ITextView::Probe(detail->mReportButton)->SetText(Elastos::Droid::R::string::report);
        // check if error reporting is enabled in secure settings
        AutoPtr<IContentResolver> resolver;
        activity->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsGlobal> global;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
        Int32 enabled;
        global->GetInt32(resolver, ISettingsGlobal::SEND_ACTION_APP_ERROR, 0, &enabled);
        if (enabled != 0 && si != NULL) {
            String packageName;
            IPackageItemInfo::Probe(si->mServiceInfo)->GetPackageName(&packageName);
            AutoPtr<IApplicationInfo> applicationInfo;
            IComponentInfo::Probe(si->mServiceInfo)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
            Int32 flags;
            applicationInfo->GetFlags(&flags);
            AutoPtr<IApplicationErrorReportHelper> helper;
            CApplicationErrorReportHelper::AcquireSingleton((IApplicationErrorReportHelper**)&helper);
            helper->GetErrorReportReceiver(
                    activity, packageName, flags, (IComponentName**)&(detail->mInstaller));
            IView::Probe(detail->mReportButton)->SetEnabled(detail->mInstaller != NULL);
        }
        else {
            IView::Probe(detail->mReportButton)->SetEnabled(FALSE);
        }
    }

    mActiveDetails->Add((IObject*)detail);
}

void CRunningServiceDetails::AddProcessDetailsView(
    /* [in] */ RunningState::ProcessItem* pi,
    /* [in] */ Boolean isMain)
{
    AddProcessesHeader();

    AutoPtr<ActiveDetail> detail = new ActiveDetail(this);
    AutoPtr<IView> root;
    mInflater->Inflate(R::layout::running_service_details_process,
            mAllDetails, FALSE, (IView**)&root);
    mAllDetails->AddView(root);
    detail->mRootView = root;
    detail->mViewHolder = new CRunningProcessesView::ViewHolder(root);
    detail->mActiveItem = detail->mViewHolder->Bind(mState, pi, mBuilder->ToString());

    AutoPtr<IView> tmp;
    root->FindViewById(R::id::comp_description, (IView**)&tmp);
    AutoPtr<ITextView> description = ITextView::Probe(tmp);
    if (pi->mUserId != UserHandle::GetMyUserId()) {
        // Processes for another user are all shown batched together; there is
        // no reason to have a description.
        tmp->SetVisibility(IView::GONE);
    }
    else if (isMain) {
        description->SetText(R::string::main_running_process_description);
    }
    else {
        Int32 textid = 0;
        AutoPtr<ICharSequence> label;
        AutoPtr<IActivityManagerRunningAppProcessInfo> rpi = pi->mRunningProcessInfo;
        AutoPtr<IComponentName> comp;
        rpi->GetImportanceReasonComponent((IComponentName**)&comp);
        //Logger::I(TAG, "Secondary proc: code=" + rpi.importanceReasonCode
        //        + " pid=" + rpi.importanceReasonPid + " comp=" + comp);
        Int32 importanceReasonCode;
        rpi->GetImportanceReasonCode(&importanceReasonCode);
        switch (importanceReasonCode) {
            case IActivityManagerRunningAppProcessInfo::REASON_PROVIDER_IN_USE:
                textid = R::string::process_provider_in_use_description;
                if (comp != NULL) {
                    // try {
                        AutoPtr<IActivity> activity;
                        GetActivity((IActivity**)&activity);
                        AutoPtr<IPackageManager> packageManager;
                        IContext::Probe(activity)->GetPackageManager((IPackageManager**)&packageManager);
                        AutoPtr<IProviderInfo> prov;
                        ECode ec = packageManager->GetProviderInfo(comp, 0, (IProviderInfo**)&prov);
                        if (SUCCEEDED(ec)) {
                            IPackageItemInfo* _prov = IPackageItemInfo::Probe(prov);
                            String name;
                            _prov->GetName(&name);
                            label = RunningState::MakeLabel(packageManager, name, _prov);
                        }
                    // } catch (NameNotFoundException e) {
                    // }
                }
                break;
            case IActivityManagerRunningAppProcessInfo::REASON_SERVICE_IN_USE:
                textid = R::string::process_service_in_use_description;
                if (comp != NULL) {
                    // try {
                        AutoPtr<IActivity> activity;
                        GetActivity((IActivity**)&activity);
                        AutoPtr<IPackageManager> packageManager;
                        IContext::Probe(activity)->GetPackageManager((IPackageManager**)&packageManager);
                        AutoPtr<IServiceInfo> serv;
                        ECode ec = packageManager->GetServiceInfo(comp, 0, (IServiceInfo**)&serv);
                        if (SUCCEEDED(ec)) {
                            IPackageItemInfo* _serv = IPackageItemInfo::Probe(serv);
                            String name;
                            _serv->GetName(&name);
                            label = RunningState::MakeLabel(packageManager, name, _serv);
                        }
                    // } catch (NameNotFoundException e) {
                    // }
                }
                break;
        }
        if (textid != 0 && label != NULL) {
            AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, label);
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            String str;
            IContext::Probe(activity)->GetString(textid, args, &str);
            description->SetText(CoreUtils::Convert(str));
        }
    }

    mActiveDetails->Add((IObject*)detail);
}

void CRunningServiceDetails::AddDetailsViews(
    /* [in] */ RunningState::MergedItem* item,
    /* [in] */ Boolean inclServices,
    /* [in] */ Boolean inclProcesses)
{
    if (item != NULL) {
        Int32 size;
        if (inclServices) {
            item->mServices->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                item->mServices->Get(i, (IInterface**)&obj);
                AddServiceDetailsView((RunningState::ServiceItem*)IRunningStateServiceItem::Probe(obj), item, TRUE, TRUE);
            }
        }

        if (inclProcesses) {
            item->mServices->GetSize(&size);
            if (size <= 0) {
                // This item does not have any services, so it must be
                // another interesting process...  we will put a fake service
                // entry for it, to allow the user to "stop" it.
                AddServiceDetailsView(NULL, item, FALSE, item->mUserId != UserHandle::GetMyUserId());
            }
            else {
                // This screen is actually showing services, so also show
                // the process details.
                item->mOtherProcesses->GetSize(&size);
                for (Int32 i = -1; i < size; i++) {
                    AutoPtr<RunningState::ProcessItem> pi;
                    if (i < 0) {
                        pi = item->mProcess;
                    }
                    else {
                        AutoPtr<IInterface> obj;
                        item->mOtherProcesses->Get(i, (IInterface**)&obj);
                        pi = (RunningState::ProcessItem*)IRunningStateProcessItem::Probe(obj);
                    }
                    if (pi != NULL && pi->mPid <= 0) {
                        continue;
                    }

                    AddProcessDetailsView(pi, i < 0);
                }
            }
        }
    }
}

void CRunningServiceDetails::AddDetailViews()
{
    Int32 size;
    mActiveDetails->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mActiveDetails->Get(i, (IInterface**)&obj);
        mAllDetails->RemoveView(((ActiveDetail*)IObject::Probe(obj))->mRootView);
    }
    mActiveDetails->Clear();

    if (mServicesHeader != NULL) {
        mAllDetails->RemoveView(IView::Probe(mServicesHeader));
        mServicesHeader = NULL;
    }

    if (mProcessesHeader != NULL) {
        mAllDetails->RemoveView(IView::Probe(mProcessesHeader));
        mProcessesHeader = NULL;
    }

    mNumServices = mNumProcesses = 0;

    if (mMergedItem != NULL) {
        if (mMergedItem->mUser != NULL) {
            AutoPtr<IArrayList> items;
            // ArrayList<RunningState.MergedItem> items;
            if (mShowBackground) {
                CArrayList::New(ICollection::Probe(mMergedItem->mChildren), (IArrayList**)&items);
                AutoPtr<ICollections> coll;
                CCollections::AcquireSingleton((ICollections**)&coll);
                coll->Sort(IList::Probe(items), mState->mBackgroundComparator);
            }
            else {
                items = mMergedItem->mChildren;
            }

            items->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                items->Get(i, (IInterface**)&obj);
                AddDetailsViews((RunningState::MergedItem*)IRunningStateMergedItem::Probe(obj), TRUE, FALSE);
            }
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                items->Get(i, (IInterface**)&obj);
                AddDetailsViews((RunningState::MergedItem*)IRunningStateMergedItem::Probe(obj), FALSE, TRUE);
            }
        }
        else {
            AddDetailsViews(mMergedItem, TRUE, TRUE);
        }
    }
}

void CRunningServiceDetails::RefreshUi(
    /* [in] */ Boolean dataChanged)
{
    if (FindMergedItem()) {
        dataChanged = TRUE;
    }
    if (dataChanged) {
        if (mMergedItem != NULL) {
            mSnippetActiveItem = mSnippetViewHolder->Bind(mState,
                    mMergedItem, mBuilder->ToString());
        }
        else if (mSnippetActiveItem != NULL) {
            // Clear whatever is currently being shown.
            mSnippetActiveItem->mHolder->mSize->SetText(CoreUtils::Convert(""));
            mSnippetActiveItem->mHolder->mUptime->SetText(CoreUtils::Convert(""));
            mSnippetActiveItem->mHolder->mDescription->SetText(R::string::no_services);
        }
        else {
            // No merged item, never had one.  Nothing to do.
            Finish();
            return;
        }
        AddDetailViews();
    }
}

void CRunningServiceDetails::Finish()
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    AutoPtr<MyRunnable> runnable = new MyRunnable(this);
    Boolean res;
    handler->Post(runnable, &res);
}

ECode CRunningServiceDetails::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Fragment::OnCreate(savedInstanceState);

    AutoPtr<IBundle> arguments;
    GetArguments((IBundle**)&arguments);
    arguments->GetInt32(KEY_UID, -1, &mUid);
    arguments->GetInt32(KEY_USER_ID, 0, &mUserId);
    arguments->GetString(KEY_PROCESS, String(NULL), &mProcessName);
    arguments->GetBoolean(KEY_BACKGROUND, FALSE, &mShowBackground);

    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    AutoPtr<IInterface> obj;
    activity->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&obj);
    mAm = IActivityManager::Probe(obj);
    obj = NULL;
    activity->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    mInflater = ILayoutInflater::Probe(obj);

    mState = RunningState::GetInstance(activity);
    return NOERROR;
}

ECode CRunningServiceDetails::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IView> view;
    inflater->Inflate(R::layout::running_service_details, container, FALSE, (IView**)&view);
    Utils::PrepareCustomPreferencesList(container, view, view, FALSE);

    mRootView = view;
    AutoPtr<IView> tmpAllDetails;
    view->FindViewById(R::id::all_details, (IView**)&tmpAllDetails);
    mAllDetails = IViewGroup::Probe(tmpAllDetails);
    AutoPtr<IView> tmpSnippet;
    view->FindViewById(R::id::snippet, (IView**)&tmpSnippet);
    mSnippet = IViewGroup::Probe(tmpSnippet);
    mSnippetViewHolder = new CRunningProcessesView::ViewHolder(tmpSnippet);

    // We want to retrieve the data right now, so any active managed
    // dialog that gets created can find it.
    EnsureData();

    *result = view;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRunningServiceDetails::OnPause()
{
    Fragment::OnPause();
    mHaveData = FALSE;
    mState->Pause();
    return NOERROR;
}

ECode CRunningServiceDetails::OnResume()
{
    Fragment::OnResume();
    EnsureData();
    return NOERROR;
}

AutoPtr<CRunningServiceDetails::ActiveDetail> CRunningServiceDetails::ActiveDetailForService(
    /* [in] */ IComponentName* comp)
{
    Int32 size;
    mActiveDetails->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mActiveDetails->Get(i, (IInterface**)&obj);
        ActiveDetail* ad = (ActiveDetail*)IObject::Probe(obj);
        if (ad->mServiceItem != NULL && ad->mServiceItem->mRunningService != NULL) {
            AutoPtr<IComponentName> service;
            ad->mServiceItem->mRunningService->GetService((IComponentName**)&service);
            Boolean res;
            IObject::Probe(comp)->Equals(service, &res);
            if (res) {
                return ad;
            }
        }
    }
    return NULL;
}

void CRunningServiceDetails::ShowConfirmStopDialog(
    /* [in] */ IComponentName* comp)
{
    AutoPtr<IDialogFragment> newFragment = MyAlertDialogFragment::NewConfirmStop(
            DIALOG_CONFIRM_STOP, comp);
    IFragment::Probe(newFragment)->SetTargetFragment(this, 0);
    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    newFragment->Show(manager, String("confirmstop"));
}

void CRunningServiceDetails::EnsureData()
{
    if (!mHaveData) {
        mHaveData = TRUE;
        mState->Resume(this);

        // We want to go away if the service being shown no longer exists,
        // so we need to ensure we have done the initial data retrieval before
        // showing our ui.
        mState->WaitForData();

        // And since we know we have the data, let's show the UI right away
        // to avoid flicker.
        RefreshUi(TRUE);
    }
}

void CRunningServiceDetails::UpdateTimes()
{
    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    if (mSnippetActiveItem != NULL) {
        mSnippetActiveItem->UpdateTime(activity, mBuilder->ToString());
    }
    Int32 size;
    mActiveDetails->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mActiveDetails->Get(i, (IInterface**)&obj);
        ((ActiveDetail*)IObject::Probe(obj))->mActiveItem->UpdateTime(activity, mBuilder->ToString());
    }
}

ECode CRunningServiceDetails::OnRefreshUi(
    /* [in] */ Int32 what)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    if (activity == NULL) return NOERROR;
    switch (what) {
        case REFRESH_TIME:
            UpdateTimes();
            break;
        case REFRESH_DATA:
            RefreshUi(FALSE);
            UpdateTimes();
            break;
        case REFRESH_STRUCTURE:
            RefreshUi(TRUE);
            UpdateTimes();
            break;
    }
    return NOERROR;
}

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos