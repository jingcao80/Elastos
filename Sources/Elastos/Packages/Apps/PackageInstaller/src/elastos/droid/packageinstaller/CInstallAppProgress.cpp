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

#include "elastos/droid/packageinstaller/CInstallAppProgress.h"
#include "elastos/droid/packageinstaller/CPackageInstallObserver.h"
#include "elastos/droid/packageinstaller/PackageUtil.h"
#include <elastos/utility/logging/Logger.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::CVerificationParams;
using Elastos::Droid::Content::Pm::EIID_IIPackageInstallObserver;
using Elastos::Droid::Content::Pm::IManifestDigest;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IVerificationParams;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::ILevelListDrawable;
using Elastos::Droid::Graphics::Drawable::ILevelListDrawable;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Core::CString;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

//=====================================================================
//              CInstallAppProgress::PackageInstallObserver
//=====================================================================
CAR_INTERFACE_IMPL_2(CInstallAppProgress::PackageInstallObserver, Object, IIPackageInstallObserver, IBinder)

CAR_OBJECT_IMPL(CPackageInstallObserver)

ECode CInstallAppProgress::PackageInstallObserver::constructor(
    /* [in] */ IInstallAppProgress* owner)
{
    mOwner = (CInstallAppProgress*)owner;
    return NOERROR;
}

ECode CInstallAppProgress::PackageInstallObserver::PackageInstalled(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 returnCode)
{
    AutoPtr<IMessage> msg;
    mOwner->mHandler->ObtainMessage(mOwner->INSTALL_COMPLETE, (IMessage**)&msg);
    msg->SetArg1(returnCode);
    Boolean resTmp;
    mOwner->mHandler->SendMessage(msg, &resTmp);
    return NOERROR;
}

ECode CInstallAppProgress::PackageInstallObserver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//=====================================================================
//                   CInstallAppProgress::ViewOnClickListener
//=====================================================================
CAR_INTERFACE_IMPL(CInstallAppProgress::ViewOnClickListener, Object, IViewOnClickListener)

ECode CInstallAppProgress::ViewOnClickListener::OnClick(
  /* [in] */ IView* v)
{
    return mOwner->OnClick(v);
}

//=====================================================================
//                   CInstallAppProgress::InnerHandler
//=====================================================================

CInstallAppProgress::InnerHandler::InnerHandler(
    /* [in] */ CInstallAppProgress* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
    Handler::constructor();
}

ECode CInstallAppProgress::InnerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CInstallAppProgress::INSTALL_COMPLETE:
            {
                Int32 arg1;
                msg->GetArg1(&arg1);
                mOwner->mInstallFlowAnalytics->SetFlowFinishedWithPackageManagerResult(arg1);

                AutoPtr<IIntent> intent;
                mOwner->GetIntent((IIntent**)&intent);
                Boolean resTmp;
                intent->GetBooleanExtra(IIntent::EXTRA_RETURN_RESULT, FALSE, &resTmp);
                if (resTmp) {
                    AutoPtr<IIntent> result;
                    CIntent::New((IIntent**)&result);
                    result->PutExtra(IIntent::EXTRA_INSTALL_RESULT, arg1);
                    mOwner->SetResult(arg1 == IPackageManager::INSTALL_SUCCEEDED
                            ? IActivity::RESULT_OK : IActivity::RESULT_FIRST_USER, result);
                    mOwner->Finish();
                    return NOERROR;
                }

                // Update the status text
                IView::Probe(mOwner->mProgressBar)->SetVisibility(IView::INVISIBLE);
                // Show the ok button
                Int32 centerTextLabel = 0;
                Int32 centerExplanationLabel = -1;

                AutoPtr<IResources> resources;
                mOwner->GetResources((IResources**)&resources);
                AutoPtr<IDrawable> drawable;
                resources->GetDrawable(R::drawable::ic_result_status, (IDrawable**)&drawable);
                ILevelListDrawable* centerTextDrawable = ILevelListDrawable::Probe(drawable);

                AutoPtr<IViewOnClickListener> listener = new ViewOnClickListener(mOwner);
                if (arg1 == IPackageManager::INSTALL_SUCCEEDED) {
                    IView::Probe(mOwner->mLaunchButton)->SetVisibility(IView::VISIBLE);
                    Boolean resTmp;
                    IDrawable::Probe(centerTextDrawable)->SetLevel(0, &resTmp);
                    centerTextLabel = R::string::install_done;
                    // Enable or disable launch button
                    AutoPtr<IPackageManager> packageManager;
                    mOwner->GetPackageManager((IPackageManager**)&packageManager);
                    String packageName;
                    IPackageItemInfo::Probe(mOwner->mAppInfo)->GetPackageName(&packageName);
                    packageManager->GetLaunchIntentForPackage(packageName, (IIntent**)&(mOwner->mLaunchIntent));
                    Boolean enabled = FALSE;
                    if(mOwner->mLaunchIntent != NULL) {
                        AutoPtr<IList> list;
                        packageManager->QueryIntentActivities(mOwner->mLaunchIntent, 0, (IList**)&list);
                        Int32 size;
                        list->GetSize(&size);
                        if (list != NULL && size > 0) {
                            enabled = TRUE;
                        }
                    }
                    if (enabled) {
                        IView::Probe(mOwner->mLaunchButton)->SetOnClickListener(listener);
                    }
                    else {
                        IView::Probe(mOwner->mLaunchButton)->SetEnabled(FALSE);
                    }
                }
                else if (arg1 == IPackageManager::INSTALL_FAILED_INSUFFICIENT_STORAGE){
                    mOwner->ShowDialogInner(DLG_OUT_OF_SPACE);
                    return NOERROR;
                }
                else {
                    // Generic error handling for all other error codes.
                    Boolean resTmp;
                    IDrawable::Probe(centerTextDrawable)->SetLevel(1, &resTmp);
                    centerExplanationLabel = mOwner->GetExplanationFromErrorCode(arg1);
                    centerTextLabel = R::string::install_failed;
                    IView::Probe(mOwner->mLaunchButton)->SetVisibility(IView::INVISIBLE);
                }
                if (centerTextDrawable != NULL) {
                    Int32 width;
                    IDrawable::Probe(centerTextDrawable)->GetIntrinsicWidth(&width);
                    Int32 height;
                    IDrawable::Probe(centerTextDrawable)->GetIntrinsicHeight(&height);
                    IDrawable::Probe(centerTextDrawable)->SetBounds(0, 0, width, height);
                    mOwner->mStatusTextView->SetCompoundDrawablesRelative(IDrawable::Probe(centerTextDrawable), NULL,
                            NULL, NULL);
                }
                mOwner->mStatusTextView->SetText(centerTextLabel);
                if (centerExplanationLabel != -1) {
                    mOwner->mExplanationTextView->SetText(centerExplanationLabel);
                    IView::Probe(mOwner->mExplanationTextView)->SetVisibility(IView::VISIBLE);
                }
                else {
                    IView::Probe(mOwner->mExplanationTextView)->SetVisibility(IView::GONE);
                }
                IView::Probe(mOwner->mDoneButton)->SetOnClickListener(listener);
                mOwner->mOkPanel->SetVisibility(IView::VISIBLE);
            }
            break;
        default:
            break;
    }
    return NOERROR;
}

ECode CInstallAppProgress::InnerHandler::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "CInstallAppProgress::InnerHandler";
    return NOERROR;
}


//=====================================================================
//        CInstallAppProgress::InnerStartActivityOnClickListener
//=====================================================================
CAR_INTERFACE_IMPL(CInstallAppProgress::InnerStartActivityOnClickListener, Object, IDialogInterfaceOnClickListener)

CInstallAppProgress::InnerStartActivityOnClickListener::InnerStartActivityOnClickListener(
    /* [in] */ CInstallAppProgress* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

ECode CInstallAppProgress::InnerStartActivityOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    //launch manage applications
    AutoPtr<IIntent> intent;
    CIntent::New(String("android.intent.action.MANAGE_PACKAGE_STORAGE"), (IIntent**)&intent);
    mOwner->StartActivity(intent);
    mOwner->Finish();
    return NOERROR;
}

//=====================================================================
//            CInstallAppProgress::InnerLogOnClickListener
//=====================================================================
CAR_INTERFACE_IMPL(CInstallAppProgress::InnerLogOnClickListener, Object, IDialogInterfaceOnClickListener)

CInstallAppProgress::InnerLogOnClickListener::InnerLogOnClickListener(
    /* [in] */ CInstallAppProgress* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

ECode CInstallAppProgress::InnerLogOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    Logger::I(mOwner->TAG, "Canceling installation");
    mOwner->Finish();
    return NOERROR;
}

//=====================================================================
//                          CInstallAppProgress
//=====================================================================
const Int32 CInstallAppProgress::DLG_OUT_OF_SPACE;
const Int32 CInstallAppProgress::INSTALL_COMPLETE;

CAR_INTERFACE_IMPL_3(CInstallAppProgress, Activity, IViewOnClickListener, IDialogInterfaceOnCancelListener, IInstallAppProgress)

CAR_OBJECT_IMPL(CInstallAppProgress)

CInstallAppProgress::CInstallAppProgress()
    : TAG("CInstallAppProgress")
{
    mHandler = new InnerHandler(this);
}

ECode CInstallAppProgress::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);

    AutoPtr<IParcelable> parcelableTmp;
    intent->GetParcelableExtra(PackageUtil::INTENT_ATTR_APPLICATION_INFO, (IParcelable**)&parcelableTmp);
    mAppInfo = IApplicationInfo::Probe(parcelableTmp);

    AutoPtr<IParcelable> parcelableTmp1;
    intent->GetParcelableExtra(EXTRA_INSTALL_FLOW_ANALYTICS, (IParcelable**)&parcelableTmp1);
    mInstallFlowAnalytics = (CInstallFlowAnalytics*)parcelableTmp1.Get();

    mInstallFlowAnalytics->SetContext(IContext::Probe(this));
    intent->GetData((IUri**)&mPackageURI);

    String scheme;
    mPackageURI->GetScheme(&scheme);
    if (scheme != NULL && !scheme.Equals("file") && !scheme.Equals("package")) {
        mInstallFlowAnalytics->SetFlowFinished(CInstallFlowAnalytics::RESULT_FAILED_UNSUPPORTED_SCHEME);
        //throw new IllegalArgumentException("unexpected scheme " + scheme);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    InitView();
    return NOERROR;
}

AutoPtr<IDialog> CInstallAppProgress::OnCreateDialog(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* bundle)
{
    switch (id) {
    case DLG_OUT_OF_SPACE:
        {
            AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, mLabel);
            String dlgText;
            GetString(R::string::out_of_space_dlg_text, args, &dlgText);

            AutoPtr<IAlertDialogBuilder> resTmp;
            CAlertDialogBuilder::New(IContext::Probe(this), (IAlertDialogBuilder**)&resTmp);

            resTmp->SetTitle(R::string::out_of_space_dlg_title);

            AutoPtr<ICharSequence> charSequenceTmp;
            CString::New(dlgText, (ICharSequence**)&charSequenceTmp);
            resTmp->SetMessage(charSequenceTmp);

            AutoPtr<IDialogInterfaceOnClickListener> startActivityListener = new InnerStartActivityOnClickListener(this);
            resTmp->SetPositiveButton(R::string::manage_applications, startActivityListener);

            AutoPtr<IDialogInterfaceOnClickListener> logListener = new InnerLogOnClickListener(this);
            resTmp->SetNegativeButton(R::string::cancel, logListener);
            resTmp->SetOnCancelListener(this);

            AutoPtr<IAlertDialog> dialogTmp;
            resTmp->Create((IAlertDialog**)&dialogTmp);
            AutoPtr<IDialog> result = IDialog::Probe(dialogTmp.Get());
            return result;
        }
    }
    return NULL;
}

ECode CInstallAppProgress::InitView()
{
    SetContentView(R::layout::op_progress);
    Int32 installFlags = 0;
    AutoPtr<IPackageManager> pm;
    GetPackageManager((IPackageManager**)&pm);

    String packageName;
    IPackageItemInfo::Probe(mAppInfo)->GetPackageName(&packageName);
    //try {
        AutoPtr<IPackageInfo> pi;
        pm->GetPackageInfo(packageName,
                IPackageManager::GET_UNINSTALLED_PACKAGES, (IPackageInfo**)&pi);
        if(pi != NULL) {
            installFlags |= IPackageManager::INSTALL_REPLACE_EXISTING;
        }
    //} catch (NameNotFoundException e) {
    //}
    if((installFlags & IPackageManager::INSTALL_REPLACE_EXISTING )!= 0) {
        Logger::W(TAG, String("Replacing package:") + packageName);
    }

    AutoPtr<PackageUtil::AppSnippet> as;
    String scheme;
    mPackageURI->GetScheme(&scheme);
    if (scheme.Equals("package")) {
        AutoPtr<ICharSequence> charSequenceTmp;
        pm->GetApplicationLabel(mAppInfo, (ICharSequence**)&charSequenceTmp);
        AutoPtr<IDrawable> icon;
        pm->GetApplicationIcon(mAppInfo, (IDrawable**)&icon);
        as = new PackageUtil::AppSnippet(charSequenceTmp, icon);
    }
    else {
        String path;
        mPackageURI->GetPath(&path);
        AutoPtr<IFile> sourceFile;
        CFile::New(path, (IFile**)&sourceFile);
        as = PackageUtil::GetAppSnippet(IActivity::Probe(this), mAppInfo, sourceFile);
    }

    mLabel = as->mLabel;
    PackageUtil::InitSnippetForNewApp(IActivity::Probe(this), as, R::id::app_snippet);

    AutoPtr<IView> viewTmp;
    FindViewById(R::id::center_text, (IView**)&viewTmp);
    mStatusTextView = ITextView::Probe(viewTmp);
    mStatusTextView->SetText(R::string::installing);

    viewTmp = NULL;
    FindViewById(R::id::center_explanation, (IView**)&viewTmp);
    mExplanationTextView = ITextView::Probe(viewTmp);

    viewTmp = NULL;
    FindViewById(R::id::progress_bar, (IView**)&viewTmp);
    mProgressBar = IProgressBar::Probe(viewTmp);
    mProgressBar->SetIndeterminate(TRUE);
    // Hide button till progress is being displayed

    FindViewById(R::id::buttons_panel, (IView**)&mOkPanel);

    viewTmp = NULL;
    FindViewById(R::id::done_button, (IView**)&viewTmp);
    mDoneButton = IButton::Probe(viewTmp);

    viewTmp = NULL;
    FindViewById(R::id::launch_button, (IView**)&viewTmp);
    mLaunchButton = IButton::Probe(viewTmp);
    mOkPanel->SetVisibility(IView::INVISIBLE);

    AutoPtr<IIntent> intentTmp;
    GetIntent((IIntent**)&intentTmp);

    String installerPackageName;
    intentTmp->GetStringExtra(IIntent::EXTRA_INSTALLER_PACKAGE_NAME, &installerPackageName);

    AutoPtr<IParcelable> parcelableTmp;
    intentTmp->GetParcelableExtra(IIntent::EXTRA_ORIGINATING_URI, (IParcelable**)&parcelableTmp);
    IUri* originatingURI = IUri::Probe(parcelableTmp);

    parcelableTmp = NULL;
    intentTmp->GetParcelableExtra(IIntent::EXTRA_REFERRER, (IParcelable**)&parcelableTmp);
    IUri* referrer = IUri::Probe(parcelableTmp);

    Int32 originatingUid = 0;
    intentTmp->GetInt32Extra(IIntent::EXTRA_ORIGINATING_UID, IVerificationParams::NO_UID, &originatingUid);

    parcelableTmp = NULL;
    intentTmp->GetParcelableExtra(EXTRA_MANIFEST_DIGEST, (IParcelable**)&parcelableTmp);
    IManifestDigest* manifestDigest = IManifestDigest::Probe(parcelableTmp);

    AutoPtr<IVerificationParams> verificationParams;
    CVerificationParams::New(NULL, originatingURI,
            referrer, originatingUid, manifestDigest, (IVerificationParams**)&verificationParams);

    AutoPtr<IIPackageInstallObserver> observer;
    CPackageInstallObserver::New(this, (IIPackageInstallObserver**)&observer);
    mPackageURI->GetScheme(&scheme);
    if (scheme.Equals("package")) {
        String packageName;
        IPackageItemInfo::Probe(mAppInfo)->GetPackageName(&packageName);
        Int32 resTmp;
        if (FAILED(pm->InstallExistingPackage(packageName, &resTmp))) {
            observer->PackageInstalled(packageName, IPackageManager::INSTALL_FAILED_INVALID_APK);
        }
        else {
            observer->PackageInstalled(packageName, IPackageManager::INSTALL_SUCCEEDED);
        }
    }
    else {
        pm->InstallPackageWithVerificationAndEncryption(mPackageURI, observer, installFlags,
            installerPackageName, verificationParams, NULL);
    }
    return NOERROR;
}

ECode CInstallAppProgress::OnClick(
    /* [in] */ IView* v)
{
    VALIDATE_NOT_NULL(v);

    if(v == IView::Probe(mDoneButton)) {
        String packageName;
        IPackageItemInfo::Probe(mAppInfo)->GetPackageName(&packageName);
        if (packageName != NULL) {
            Logger::I(TAG, "Finished installing %s", packageName.string());
        }
        Finish();
    }
    else if(v == IView::Probe(mLaunchButton)) {
        StartActivity(mLaunchIntent);
        Finish();
    }
    return NOERROR;
}

ECode CInstallAppProgress::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    return Finish();
}

ECode CInstallAppProgress::OnDestroy()
{
    return Activity::OnDestroy();
}

Int32 CInstallAppProgress::GetExplanationFromErrorCode(
    /* [in] */ Int32 errCode)
{
    Logger::D(TAG, "Installation error code: %d", errCode);
    switch (errCode) {
        case IPackageManager::INSTALL_FAILED_INVALID_APK:
            return R::string::install_failed_invalid_apk;
        case IPackageManager::INSTALL_PARSE_FAILED_INCONSISTENT_CERTIFICATES:
            return R::string::install_failed_inconsistent_certificates;
        case IPackageManager::INSTALL_FAILED_OLDER_SDK:
            return R::string::install_failed_older_sdk;
        case IPackageManager::INSTALL_FAILED_CPU_ABI_INCOMPATIBLE:
            return R::string::install_failed_cpu_abi_incompatible;
        default:
            return -1;
    }
}

void CInstallAppProgress::ShowDialogInner(
    /* [in] */ Int32 id)
{
    RemoveDialog(id);
    //ActionsCode(liuxinxu, BUGFIX: BUG00253949,APP crash when show dialog.)
    Boolean isFinishing = FALSE;
    IsFinishing(&isFinishing);
    if (!isFinishing) {
        ShowDialog(id);
    }
    else {
        Logger::E(TAG, "APP already finished.i can't show any dialog at all.");
    }
}

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos
