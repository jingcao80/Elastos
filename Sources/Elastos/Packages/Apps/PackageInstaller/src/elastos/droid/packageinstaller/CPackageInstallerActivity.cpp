
#include "elastos/droid/packageinstaller/CPackageInstallerActivity.h"
#include "elastos/droid/packageinstaller/PackageUtil.h"
#include "elastos/droid/packageinstaller/TabsAdapter.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/content/pm/PackageUserState.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/utility/logging/Logger.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::Pm::IPackageInstallerSessionInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IVerificationParams;
using Elastos::Droid::Content::Pm::PackageUserState;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::EIID_ITabHostOnTabChangeListener;
using Elastos::Droid::Widget::CAppSecurityPermissions;
using Elastos::Droid::Widget::IAppSecurityPermissions;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Droid::Widget::ITabSpec;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

CAR_INTERFACE_IMPL(CPackageInstallerActivity::ViewOnClickListener, Object, IViewOnClickListener)

ECode CPackageInstallerActivity::ViewOnClickListener::OnClick(
  /* [in] */ IView* v)
{
    return mOwner->OnClick(v);
}

//=====================================================================
//           CPackageInstallerActivity::InnerOnClickListener
//=====================================================================
CAR_INTERFACE_IMPL(CPackageInstallerActivity::InnerOnClickListener, Object, IDialogInterfaceOnClickListener)

CPackageInstallerActivity::InnerOnClickListener::InnerOnClickListener(
    /* [in] */ CPackageInstallerActivity* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

// @Override
ECode CPackageInstallerActivity::InnerOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    Logger::I(TAG, "Finishing off activity so that user can navigate to settings manually");
    mOwner->Finish();
    return NOERROR;
}

//=====================================================================
//          CPackageInstallerActivity::InnerOnClickListener1
//=====================================================================
CAR_INTERFACE_IMPL(CPackageInstallerActivity::InnerOnClickListener1, Object, IDialogInterfaceOnClickListener)

CPackageInstallerActivity::InnerOnClickListener1::InnerOnClickListener1(
    /* [in] */ CPackageInstallerActivity* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

// @Override
ECode CPackageInstallerActivity::InnerOnClickListener1::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    Logger::I(TAG, "Launching settings");
    mOwner->LaunchSettingsAppAndFinish();
    return NOERROR;
}

//=====================================================================
//          CPackageInstallerActivity::InnerOnClickListener2
//=====================================================================
CAR_INTERFACE_IMPL(CPackageInstallerActivity::InnerOnClickListener2, Object, IDialogInterfaceOnClickListener)

CPackageInstallerActivity::InnerOnClickListener2::InnerOnClickListener2(
    /* [in] */ CPackageInstallerActivity* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

// @Override
ECode CPackageInstallerActivity::InnerOnClickListener2::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mOwner->Finish();
    return NOERROR;
}

//=====================================================================
//          CPackageInstallerActivity::InnerOnClickListener3
//=====================================================================
CAR_INTERFACE_IMPL(CPackageInstallerActivity::InnerOnClickListener3, Object, IDialogInterfaceOnClickListener)

CPackageInstallerActivity::InnerOnClickListener3::InnerOnClickListener3(
    /* [in] */ CPackageInstallerActivity* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

// @Override
ECode CPackageInstallerActivity::InnerOnClickListener3::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<IIntent> intent;
    CIntent::New(String("android.intent.action.MANAGE_PACKAGE_STORAGE"), (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    mOwner->StartActivity(intent);
    mOwner->Finish();
    return NOERROR;
}

//=====================================================================
//          CPackageInstallerActivity::InnerOnClickListener4
//=====================================================================
CAR_INTERFACE_IMPL(CPackageInstallerActivity::InnerOnClickListener4, Object, IDialogInterfaceOnClickListener)

CPackageInstallerActivity::InnerOnClickListener4::InnerOnClickListener4(
    /* [in] */ CPackageInstallerActivity* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

// @Override
ECode CPackageInstallerActivity::InnerOnClickListener4::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    Logger::I(TAG, "Canceling installation");
    mOwner->Finish();
    return NOERROR;
}

//=====================================================================
//          CPackageInstallerActivity::InnerOnClickListener5
//=====================================================================
CAR_INTERFACE_IMPL(CPackageInstallerActivity::InnerOnClickListener5, Object, IDialogInterfaceOnClickListener)

CPackageInstallerActivity::InnerOnClickListener5::InnerOnClickListener5(
    /* [in] */ CPackageInstallerActivity* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

// @Override
ECode CPackageInstallerActivity::InnerOnClickListener5::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mOwner->Finish();
    return NOERROR;
}

//=====================================================================
//          CPackageInstallerActivity::InnerOnClickListener6
//=====================================================================
CAR_INTERFACE_IMPL(CPackageInstallerActivity::InnerOnClickListener6, Object, IDialogInterfaceOnClickListener)

CPackageInstallerActivity::InnerOnClickListener6::InnerOnClickListener6(
    /* [in] */ CPackageInstallerActivity* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

// @Override
ECode CPackageInstallerActivity::InnerOnClickListener6::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mOwner->SetResult(mOwner->RESULT_CANCELED);
    mOwner->Finish();
    return NOERROR;
}

//=====================================================================
//          CPackageInstallerActivity::InnerOnClickListener7
//=====================================================================
CAR_INTERFACE_IMPL(CPackageInstallerActivity::InnerOnClickListener7, Object, IDialogInterfaceOnClickListener)

CPackageInstallerActivity::InnerOnClickListener7::InnerOnClickListener7(
    /* [in] */ CPackageInstallerActivity* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

// @Override
ECode CPackageInstallerActivity::InnerOnClickListener7::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<ISharedPreferences> prefs;
    mOwner->GetSharedPreferences(PREFS_ALLOWED_SOURCES, IContext::MODE_PRIVATE, (ISharedPreferences**)&prefs);
    AutoPtr<ISharedPreferencesEditor> editor;
    prefs->Edit((ISharedPreferencesEditor**)&editor);
    String packageName;
    IPackageItemInfo::Probe(mOwner->mSourceInfo)->GetPackageName(&packageName);
    editor->PutBoolean(packageName, TRUE);
    editor->Apply();
    mOwner->StartInstallConfirm();
    return NOERROR;
}

//=====================================================================
//      CPackageInstallerActivity::InnerTabHostOnTabChangeListener
//=====================================================================
CAR_INTERFACE_IMPL(CPackageInstallerActivity::InnerTabHostOnTabChangeListener, Object, ITabHostOnTabChangeListener)

CPackageInstallerActivity::InnerTabHostOnTabChangeListener::InnerTabHostOnTabChangeListener(
    /* [in] */ CPackageInstallerActivity* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

ECode CPackageInstallerActivity::InnerTabHostOnTabChangeListener::OnTabChanged(
    /* [in] */ const String& tabId)
{
    if (TAB_ID_ALL.Equals(tabId)) {
        mOwner->mInstallFlowAnalytics->SetAllPermissionsDisplayed(TRUE);
    }
    else if (TAB_ID_NEW.Equals(tabId)) {
        mOwner->mInstallFlowAnalytics->SetNewPermissionsDisplayed(TRUE);
    }
    return NOERROR;
}

//=====================================================================
//               CPackageInstallerActivity::InnerRunnable
//=====================================================================
CAR_INTERFACE_IMPL(CPackageInstallerActivity::InnerRunnable, Object, IRunnable)

CPackageInstallerActivity::InnerRunnable::InnerRunnable(
    /* [in] */ CPackageInstallerActivity* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

ECode CPackageInstallerActivity::InnerRunnable::Run()
{
    ITextView::Probe(mOwner->mOk)->SetText(R::string::install);
    mOwner->mOkCanInstall = TRUE;
    return NOERROR;
}

//=====================================================================
//                       CPackageInstallerActivity
//=====================================================================
const String CPackageInstallerActivity::PREFS_ALLOWED_SOURCES("allowed_sources");
const String CPackageInstallerActivity::TAG("PackageInstaller");
const String CPackageInstallerActivity::PACKAGE_MIME_TYPE("application/vnd.android.package-archive");
const String CPackageInstallerActivity::TAB_ID_ALL("all");
const String CPackageInstallerActivity::TAB_ID_NEW("new");
const Int32 CPackageInstallerActivity::DLG_BASE;
const Int32 CPackageInstallerActivity::DLG_UNKNOWN_APPS;
const Int32 CPackageInstallerActivity::DLG_PACKAGE_ERROR;
const Int32 CPackageInstallerActivity::DLG_OUT_OF_SPACE;
const Int32 CPackageInstallerActivity::DLG_INSTALL_ERROR;
const Int32 CPackageInstallerActivity::DLG_ALLOW_SOURCE;

CAR_INTERFACE_IMPL_3(CPackageInstallerActivity, Activity, IDialogInterfaceOnCancelListener, IViewOnClickListener, IPackageInstallerActivity)

CAR_OBJECT_IMPL(CPackageInstallerActivity)

AutoPtr<IDialog> CPackageInstallerActivity::OnCreateDialog(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* bundle)
{
    switch (id) {
    case DLG_UNKNOWN_APPS:
        {
            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(IContext::Probe(this), (IAlertDialogBuilder**)&builder);
            builder->SetTitle(R::string::unknown_apps_dlg_title);
            builder->SetMessage(R::string::unknown_apps_dlg_text);

            AutoPtr<IDialogInterfaceOnClickListener> listener = new InnerOnClickListener(this);
            builder->SetNegativeButton(R::string::cancel, listener);

            AutoPtr<IDialogInterfaceOnClickListener> listener1 = new InnerOnClickListener1(this);
            builder->SetPositiveButton(R::string::settings, listener1);

            builder->SetOnCancelListener(this);
            AutoPtr<IAlertDialog> resTmp;
            builder->Create((IAlertDialog**)&resTmp);
            AutoPtr<IDialog> result = IDialog::Probe(resTmp.Get());
            return result;
        }
    case DLG_PACKAGE_ERROR :
        {
            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(IContext::Probe(this), (IAlertDialogBuilder**)&builder);

            builder->SetTitle(R::string::Parse_error_dlg_title);
            builder->SetMessage(R::string::Parse_error_dlg_text);

            AutoPtr<IDialogInterfaceOnClickListener> listener = new InnerOnClickListener2(this);
            builder->SetPositiveButton(R::string::ok, listener);
            builder->SetOnCancelListener(this);

            AutoPtr<IAlertDialog> resTmp;
            builder->Create((IAlertDialog**)&resTmp);
            AutoPtr<IDialog> result = IDialog::Probe(resTmp.Get());
            return result;
        }
    case DLG_OUT_OF_SPACE:
        {
            // Guaranteed not to be null. will default to package name if not set by app
            AutoPtr<IApplicationInfo> applicationInfo;
            mPkgInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
            AutoPtr<ICharSequence> appTitle;
            mPm->GetApplicationLabel(applicationInfo, (ICharSequence**)&appTitle);

            AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, appTitle);
            String dlgText;
            GetString(R::string::out_of_space_dlg_text, args, &dlgText);

            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(IContext::Probe(this), (IAlertDialogBuilder**)&builder);

            builder->SetTitle(R::string::out_of_space_dlg_title);

            AutoPtr<ICharSequence> charSequenceTmp;
            CString::New(dlgText, (ICharSequence**)&charSequenceTmp);
            builder->SetMessage(charSequenceTmp);

            AutoPtr<IDialogInterfaceOnClickListener> listener = new InnerOnClickListener3(this);
            builder->SetPositiveButton(R::string::manage_applications, listener);

            AutoPtr<IDialogInterfaceOnClickListener> listener1 = new InnerOnClickListener4(this);
            builder->SetNegativeButton(R::string::cancel, listener1);
            builder->SetOnCancelListener(this);

            AutoPtr<IAlertDialog> resTmp;
            builder->Create((IAlertDialog**)&resTmp);
            AutoPtr<IDialog> result = IDialog::Probe(resTmp.Get());
            return result;
        }
    case DLG_INSTALL_ERROR :
        {
            // Guaranteed not to be null. will default to package name if not set by app
            AutoPtr<IApplicationInfo> applicationInfo;
            mPkgInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
            AutoPtr<ICharSequence> appTitle1;
            mPm->GetApplicationLabel(applicationInfo, (ICharSequence**)&appTitle1);

            AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, appTitle1);
            String dlgText1;
            GetString(R::string::install_failed_msg, args, &dlgText1);

            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(IContext::Probe(this), (IAlertDialogBuilder**)&builder);

            builder->SetTitle(R::string::install_failed);

            AutoPtr<IDialogInterfaceOnClickListener> listener = new InnerOnClickListener5(this);
            builder->SetNeutralButton(R::string::ok, listener);

            AutoPtr<ICharSequence> charSequenceTmp;
            CString::New(dlgText1, (ICharSequence**)&charSequenceTmp);
            builder->SetMessage(charSequenceTmp);
            builder->SetOnCancelListener(this);

            AutoPtr<IAlertDialog> resTmp;
            builder->Create((IAlertDialog**)&resTmp);
            AutoPtr<IDialog> result = IDialog::Probe(resTmp.Get());
            return result;
        }
    case DLG_ALLOW_SOURCE:
        {
            AutoPtr<ICharSequence> appTitle2;
            mPm->GetApplicationLabel(mSourceInfo, (ICharSequence**)&appTitle2);

            AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, appTitle2);
            String dlgText2;
            GetString(R::string::allow_source_dlg_text, args, &dlgText2);

            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(IContext::Probe(this), (IAlertDialogBuilder**)&builder);

            builder->SetTitle(R::string::allow_source_dlg_title);

            AutoPtr<ICharSequence> charSequenceTmp;
            CString::New(dlgText2, (ICharSequence**)&charSequenceTmp);
            builder->SetMessage(charSequenceTmp);

            AutoPtr<IDialogInterfaceOnClickListener> listener = new InnerOnClickListener5(this);
            builder->SetNegativeButton(R::string::cancel, listener);

            AutoPtr<IDialogInterfaceOnClickListener> listener1 = new InnerOnClickListener6(this);
            builder->SetNeutralButton(R::string::ok, listener1);
            builder->SetOnCancelListener(this);

            AutoPtr<IAlertDialog> resTmp;
            builder->Create((IAlertDialog**)&resTmp);
            AutoPtr<IDialog> result = IDialog::Probe(resTmp.Get());
            return result;
        }
   }
   return NULL;
}

ECode CPackageInstallerActivity::SetPmResult(
    /* [in] */ Int32 pmResult)
{
    AutoPtr<IIntent> result;
    CIntent::New((IIntent**)&result);
    result->PutExtra(IIntent::EXTRA_INSTALL_RESULT, pmResult);
    SetResult(pmResult == IPackageManager::INSTALL_SUCCEEDED ? RESULT_OK : RESULT_FIRST_USER, result);
    return NOERROR;
}

ECode CPackageInstallerActivity::OnBackPressed()
{
    if (mSessionId != -1) {
        mInstaller->SetPermissionsResult(mSessionId, FALSE);
    }
    mInstallFlowAnalytics->SetFlowFinished(CInstallFlowAnalytics::RESULT_CANCELLED_BY_USER);
    return Activity::OnBackPressed();
}

ECode CPackageInstallerActivity::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    return Finish();
}

ECode CPackageInstallerActivity::OnClick(
    /* [in] */ IView* v)
{
    if(v == IView::Probe(mOk)) {
        if (mOkCanInstall || mScrollView == NULL) {
            mInstallFlowAnalytics->SetInstallButtonClicked();
            if (mSessionId != -1) {
                mInstaller->SetPermissionsResult(mSessionId, TRUE);

                // We're only confirming permissions, so we don't really know how the
                // story ends; assume success.
                mInstallFlowAnalytics->SetFlowFinishedWithPackageManagerResult(
                        IPackageManager::INSTALL_SUCCEEDED);
            }
            else {
                // Start subactivity to actually install the application
                AutoPtr<IIntent> newIntent;
                CIntent::New((IIntent**)&newIntent);

                AutoPtr<IApplicationInfo> applicationInfo;
                mPkgInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
                newIntent->PutExtra(PackageUtil::INTENT_ATTR_APPLICATION_INFO, IParcelable::Probe(applicationInfo));
                newIntent->SetData(mPackageURI);
                newIntent->SetClass(this, ECLSID_CInstallAppProgress);
                newIntent->PutExtra(IInstallAppProgress::EXTRA_MANIFEST_DIGEST, IParcelable::Probe(mPkgDigest));
                newIntent->PutExtra(IInstallAppProgress::EXTRA_INSTALL_FLOW_ANALYTICS, mInstallFlowAnalytics);

                AutoPtr<IIntent> intent;
                GetIntent((IIntent**)&intent);
                String installerPackageName;
                intent->GetStringExtra(IIntent::EXTRA_INSTALLER_PACKAGE_NAME, &installerPackageName);
                if (mOriginatingURI != NULL) {
                    newIntent->PutExtra(IIntent::EXTRA_ORIGINATING_URI, IParcelable::Probe(mOriginatingURI));
                }
                if (mReferrerURI != NULL) {
                    newIntent->PutExtra(IIntent::EXTRA_REFERRER, IParcelable::Probe(mReferrerURI));
                }
                if (mOriginatingUid != IVerificationParams::NO_UID) {
                    newIntent->PutExtra(IIntent::EXTRA_ORIGINATING_UID, mOriginatingUid);
                }
                if (installerPackageName != NULL) {
                    newIntent->PutExtra(IIntent::EXTRA_INSTALLER_PACKAGE_NAME, installerPackageName);
                }

                Boolean extraVal = FALSE;
                intent->GetBooleanExtra(IIntent::EXTRA_RETURN_RESULT, FALSE, &extraVal);
                if (extraVal) {
                    newIntent->PutBooleanExtra(IIntent::EXTRA_RETURN_RESULT, TRUE);
                    newIntent->AddFlags(IIntent::FLAG_ACTIVITY_FORWARD_RESULT);
                }
                if(localLOGV) {
                    String sUri;
                    mPackageURI->ToSafeString(&sUri);
                    Logger::I(TAG, "downloaded app uri=%s", sUri.string());
                }
                StartActivity(newIntent);
            }
            Finish();
        }
        else {
            Boolean res;
            mScrollView->PageScroll(IView::FOCUS_DOWN, &res);
        }
    }
    else if(v == IView::Probe(mCancel)) {
        // Cancel and finish
        SetResult(RESULT_CANCELED);
        if (mSessionId != -1) {
            mInstaller->SetPermissionsResult(mSessionId, FALSE);
        }
        mInstallFlowAnalytics->SetFlowFinished(CInstallFlowAnalytics::RESULT_CANCELLED_BY_USER);
        Finish();
    }
    return NOERROR;
}

ECode CPackageInstallerActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    GetPackageManager((IPackageManager**)&mPm);
    mPm->GetPackageInstaller((IPackageInstaller**)&mInstaller);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    String action;
    intent->GetAction(&action);
    if (IPackageInstaller::ACTION_CONFIRM_PERMISSIONS.Equals(action)) {
        Int32 sessionId = 0;
        intent->GetInt32Extra(IPackageInstaller::EXTRA_SESSION_ID, -1, &sessionId);
        AutoPtr<IPackageInstallerSessionInfo> info;
        mInstaller->GetSessionInfo(sessionId, (IPackageInstallerSessionInfo**)&info);

        Boolean sealed;
        String path;
        if (info == NULL || !(info->IsSealed(&sealed), sealed) ||
            (info->GetResolvedBaseCodePath(&path), path == NULL)) {
            Logger::W(TAG, "Session %d in funky state; ignoring", mSessionId);
            //Finish();
            return NOERROR;
        }

        mSessionId = sessionId;
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);

        AutoPtr<IFile> file;
        CFile::New(path, (IFile**)&file);
        helper->FromFile(file, (IUri**)&mPackageURI);
        mOriginatingURI = NULL;
        mReferrerURI = NULL;
    }
    else {
        mSessionId = -1;
        intent->GetData((IUri**)&mPackageURI);

        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(IIntent::EXTRA_ORIGINATING_URI, (IParcelable**)&parcelable);
        mOriginatingURI = IUri::Probe(parcelable);

        parcelable = NULL;
        intent->GetParcelableExtra(IIntent::EXTRA_REFERRER, (IParcelable**)&parcelable);
        mReferrerURI = IUri::Probe(parcelable);
    }

    Boolean requestFromUnknownSource = IsInstallRequestFromUnknownSource(intent);
    CInstallFlowAnalytics::NewByFriend((CInstallFlowAnalytics**)&mInstallFlowAnalytics);
    mInstallFlowAnalytics->SetContext(IContext::Probe(this));
    mInstallFlowAnalytics->SetStartTimestampMillis(SystemClock::GetElapsedRealtime());
    mInstallFlowAnalytics->SetInstallsFromUnknownSourcesPermitted(IsInstallingUnknownAppsAllowed());
    mInstallFlowAnalytics->SetInstallRequestFromUnknownSource(requestFromUnknownSource);
    mInstallFlowAnalytics->SetVerifyAppsEnabled(IsVerifyAppsEnabled());
    mInstallFlowAnalytics->SetAppVerifierInstalled(IsAppVerifierInstalled());
    String str;
    mPackageURI->ToSafeString(&str);
    mInstallFlowAnalytics->SetPackageUri(str);

    String scheme;
    mPackageURI->GetScheme(&scheme);
    if (scheme != NULL && !scheme.Equals("file") && !scheme.Equals("package")) {
        Logger::W(TAG, String("Unsupported scheme ") + scheme);
        SetPmResult(IPackageManager::INSTALL_FAILED_INVALID_URI);
        mInstallFlowAnalytics->SetFlowFinished(CInstallFlowAnalytics::RESULT_FAILED_UNSUPPORTED_SCHEME);
        Finish();
        return NOERROR;
    }

    AutoPtr<PackageUtil::AppSnippet> as;
    mPackageURI->GetScheme(&scheme);
    if (scheme.Equals("package")) {
        mInstallFlowAnalytics->SetFileUri(FALSE);
        //try {
            String schemeSpecificPart;
            mPackageURI->GetSchemeSpecificPart(&schemeSpecificPart);
            mPm->GetPackageInfo(schemeSpecificPart,
                IPackageManager::GET_PERMISSIONS | IPackageManager::GET_UNINSTALLED_PACKAGES,
                (IPackageInfo**)&mPkgInfo);
        //} catch (NameNotFoundException e) {
        //}
        if (mPkgInfo == NULL) {
            mPackageURI->GetScheme(&scheme);
            Logger::W(TAG, "Requested package %s not available. Discontinuing installation", scheme.string());
            ShowDialogInner(DLG_PACKAGE_ERROR);
            SetPmResult(IPackageManager::INSTALL_FAILED_INVALID_APK);
            mInstallFlowAnalytics->SetPackageInfoObtained();
            mInstallFlowAnalytics->SetFlowFinished(CInstallFlowAnalytics::RESULT_FAILED_PACKAGE_MISSING);
            return NOERROR;
        }

        AutoPtr<IApplicationInfo> applicationInfo;
        mPkgInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        AutoPtr<ICharSequence> charSequenceTmp;
        mPm->GetApplicationLabel(applicationInfo, (ICharSequence**)&charSequenceTmp);
        AutoPtr<IDrawable> drawable;
        mPm->GetApplicationIcon(applicationInfo, (IDrawable**)&drawable);
        as = new PackageUtil::AppSnippet(charSequenceTmp, drawable);
    }
    else {
        mInstallFlowAnalytics->SetFileUri(TRUE);
        String path;
        mPackageURI->GetPath(&path);
        AutoPtr<IFile> sourceFile;
        CFile::New(path, (IFile**)&sourceFile);
        AutoPtr<PackageParser::Package> parsed = PackageUtil::GetPackageInfo(sourceFile);

        // Check for parse errors
        if (parsed == NULL) {
            Logger::W(TAG, "Parse error when parsing manifest. Discontinuing installation");
            ShowDialogInner(DLG_PACKAGE_ERROR);
            SetPmResult(IPackageManager::INSTALL_FAILED_INVALID_APK);
            mInstallFlowAnalytics->SetPackageInfoObtained();
            mInstallFlowAnalytics->SetFlowFinished(CInstallFlowAnalytics::RESULT_FAILED_TO_GET_PACKAGE_INFO);
            return NOERROR;
        }

        AutoPtr<PackageUserState> userState = new PackageUserState();
        mPkgInfo = PackageParser::GeneratePackageInfo(parsed, NULL, IPackageManager::GET_PERMISSIONS, 0, 0, NULL, userState);
        mPkgDigest = parsed->mManifestDigest;
        AutoPtr<IApplicationInfo> applicationInfo;
        mPkgInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        as = PackageUtil::GetAppSnippet(this, applicationInfo, sourceFile);
    }
    mInstallFlowAnalytics->SetPackageInfoObtained();

    //set view
    SetContentView(R::layout::install_start);
    FindViewById(R::id::install_confirm_panel, (IView**)&mInstallConfirm);
    mInstallConfirm->SetVisibility(IView::INVISIBLE);
    PackageUtil::InitSnippetForNewApp(this, as, R::id::app_snippet);

    mOriginatingUid = GetOriginatingUid(intent);

    // Block the install attempt on the Unknown Sources setting if necessary.
    if ((requestFromUnknownSource) && (!IsInstallingUnknownAppsAllowed())) {
        //ask user to enable setting first
        ShowDialogInner(DLG_UNKNOWN_APPS);
        mInstallFlowAnalytics->SetFlowFinished(CInstallFlowAnalytics::RESULT_BLOCKED_BY_UNKNOWN_SOURCES_SETTING);
        return NOERROR;
    }
    InitiateInstall();
    return NOERROR;
}

void CPackageInstallerActivity::StartInstallConfirm()
{
    AutoPtr<IView> viewTmp;
    FindViewById(Elastos::Droid::R::id::tabhost, (IView**)&viewTmp);
    ITabHost* tabHost = ITabHost::Probe(viewTmp);
    tabHost->Setup();

    viewTmp = NULL;
    FindViewById(R::id::pager, (IView**)&viewTmp);
    AutoPtr<IViewPager> viewPager = IViewPager::Probe(viewTmp);

    AutoPtr<TabsAdapter> adapter = new TabsAdapter(this, tabHost, viewPager);
    AutoPtr<ITabHostOnTabChangeListener> listener = new InnerTabHostOnTabChangeListener(this);
    adapter->SetOnTabChangedListener(listener);

    Boolean permVisible = FALSE;
    mScrollView = NULL;
    mOkCanInstall = FALSE;
    Int32 msg = 0;
    if (mPkgInfo != NULL) {
        AutoPtr<IAppSecurityPermissions> perms;
        CAppSecurityPermissions::New(this, mPkgInfo, (IAppSecurityPermissions**)&perms);
        Int32 NP = 0;
        perms->GetPermissionCount(IAppSecurityPermissions::WHICH_PERSONAL, &NP);
        Int32 ND = 0;
        perms->GetPermissionCount(IAppSecurityPermissions::WHICH_DEVICE, &ND);
        if (mAppInfo != NULL) {
            Int32 flags = 0;
            mAppInfo->GetFlags(&flags);
            msg = (flags & IApplicationInfo::FLAG_SYSTEM) != 0 ? R::string::install_confirm_question_update_system
                    : R::string::install_confirm_question_update;

            CCaffeinatedScrollView::NewByFriend(this, (CCaffeinatedScrollView**)&mScrollView);
            mScrollView->SetFillViewport(TRUE);
            Int32 count = 0;
            //perms->GetPermissionCount(IAppSecurityPermissions::WHICH_NEW, &count);
            Boolean newPermissionsFound = count > 0;
            mInstallFlowAnalytics->SetNewPermissionsFound(newPermissionsFound);
            if (newPermissionsFound) {
                permVisible = TRUE;
                AutoPtr<IView> view;
                perms->GetPermissionsView(IAppSecurityPermissions::WHICH_NEW, (IView**)&view);
                mScrollView->AddView(view);
            }
            else {
                AutoPtr<IInterface> interfaceTmp;
                GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&interfaceTmp);
                ILayoutInflater* inflater = ILayoutInflater::Probe(interfaceTmp);

                viewTmp = NULL;
                inflater->Inflate(R::layout::label, NULL, (IView**)&viewTmp);
                ITextView* label = ITextView::Probe(viewTmp);

                label->SetText(R::string::no_new_perms);
                mScrollView->AddView(IView::Probe(label));
            }

            AutoPtr<ITabSpec> tabSpec;
            tabHost->NewTabSpec(TAB_ID_NEW, (ITabSpec**)&tabSpec);

            AutoPtr<ICharSequence> charSequenceTmp;
            GetText(R::string::newPerms, (ICharSequence**)&charSequenceTmp);
            tabSpec->SetIndicator(charSequenceTmp);
            adapter->AddTab(tabSpec, mScrollView);
        }
        else  {
            viewTmp = NULL;
            FindViewById(R::id::tabscontainer, (IView**)&viewTmp);
            viewTmp->SetVisibility(IView::GONE);

            viewTmp = NULL;
            FindViewById(R::id::divider, (IView**)&viewTmp);
            viewTmp->SetVisibility(IView::VISIBLE);
        }
        if (NP > 0 || ND > 0) {
            permVisible = TRUE;
            AutoPtr<IInterface> interfaceTmp;
            GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&interfaceTmp);
            ILayoutInflater* inflater = ILayoutInflater::Probe(interfaceTmp);

            AutoPtr<IView> root;
            inflater->Inflate(R::layout::permissions_list, NULL, (IView**)&root);
            if (mScrollView == NULL) {
                viewTmp = NULL;
                root->FindViewById(R::id::scrollview, (IView**)&viewTmp);
                mScrollView = (CCaffeinatedScrollView*)viewTmp.Get();
            }
            if (NP > 0) {
                viewTmp = NULL;
                root->FindViewById(R::id::privacylist, (IView**)&viewTmp);
                AutoPtr<IView> view;
                perms->GetPermissionsView(IAppSecurityPermissions::WHICH_PERSONAL, (IView**)&view);
                IViewGroup::Probe(viewTmp)->AddView(view);
            }
            else {
                viewTmp = NULL;
                root->FindViewById(R::id::privacylist, (IView**)&viewTmp);
                viewTmp->SetVisibility(IView::GONE);
            }
            if (ND > 0) {
                viewTmp = NULL;
                root->FindViewById(R::id::devicelist, (IView**)&viewTmp);
                AutoPtr<IView> view;
                perms->GetPermissionsView(IAppSecurityPermissions::WHICH_DEVICE, (IView**)&view);
                IViewGroup::Probe(viewTmp)->AddView(view);
            }
            else {
                viewTmp = NULL;
                root->FindViewById(R::id::devicelist, (IView**)&viewTmp);
                viewTmp->SetVisibility(IView::GONE);
            }

            AutoPtr<ITabSpec> tabSpec;
            tabHost->NewTabSpec(TAB_ID_ALL, (ITabSpec**)&tabSpec);

            AutoPtr<ICharSequence> charSequenceTmp;
            GetText(R::string::allPerms, (ICharSequence**)&charSequenceTmp);
            tabSpec->SetIndicator(charSequenceTmp);
            adapter->AddTab(tabSpec, root);
        }
    }
    mInstallFlowAnalytics->SetPermissionsDisplayed(permVisible);
    if (!permVisible) {
        if (mAppInfo != NULL) {
            // This is an update to an application, but there are no
            // permissions at all.
            Int32 flags = 0;
            mAppInfo->GetFlags(&flags);
            msg = (flags & IApplicationInfo::FLAG_SYSTEM) != 0 ? R::string::install_confirm_question_update_system_no_perms
                    : R::string::install_confirm_question_update_no_perms;
        }
        else {
            // This is a new application with no permissions.
            msg = R::string::install_confirm_question_no_perms;
        }
        IView::Probe(tabHost)->SetVisibility(IView::GONE);
        mInstallFlowAnalytics->SetAllPermissionsDisplayed(FALSE);
        mInstallFlowAnalytics->SetNewPermissionsDisplayed(FALSE);

        viewTmp = NULL;
        FindViewById(R::id::filler, (IView**)&viewTmp);
        viewTmp->SetVisibility(IView::VISIBLE);

        viewTmp = NULL;
        FindViewById(R::id::divider, (IView**)&viewTmp);
        viewTmp->SetVisibility(IView::GONE);

        mScrollView = NULL;
    }
    if (msg != 0) {
        viewTmp = NULL;
        FindViewById(R::id::install_confirm_question, (IView**)&viewTmp);
        ITextView::Probe(viewTmp)->SetText(msg);
    }
    mInstallConfirm->SetVisibility(IView::VISIBLE);

    viewTmp = NULL;
    FindViewById(R::id::ok_button, (IView**)&viewTmp);
    mOk = IButton::Probe(viewTmp);

    viewTmp = NULL;
    FindViewById(R::id::cancel_button, (IView**)&viewTmp);
    mCancel = IButton::Probe(viewTmp);

    AutoPtr<IViewOnClickListener> listener2 = new ViewOnClickListener(this);
    IView::Probe(mOk)->SetOnClickListener(listener2);
    IView::Probe(mCancel)->SetOnClickListener(listener2);
    if (mScrollView == NULL) {
        // There is nothing to scroll view, so the ok button is immediately
        // set to install.
        ITextView::Probe(mOk)->SetText(R::string::install);
        mOkCanInstall = TRUE;
    }
    else {
        AutoPtr<IRunnable> runnable = new InnerRunnable(this);
        mScrollView->SetFullScrollAction(runnable);
    }
}

void CPackageInstallerActivity::ShowDialogInner(
    /* [in] */ Int32 id)
{
    RemoveDialog(id);
    ShowDialog(id);
}

void CPackageInstallerActivity::LaunchSettingsAppAndFinish()
{
    AutoPtr<IIntent> launchSettingsIntent;
    CIntent::New(ISettings::ACTION_SECURITY_SETTINGS, (IIntent**)&launchSettingsIntent);
    launchSettingsIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    StartActivity(launchSettingsIntent);
    Finish();
}

Boolean CPackageInstallerActivity::IsInstallingUnknownAppsAllowed()
{
    Logger::E(TAG, "TODO: need Elasto's Settings can set AllowInstallUnknownSources apk!");
    // AutoPtr<IInterface> interfaceTmp;
    // GetSystemService(USER_SERVICE, (IInterface**)&interfaceTmp);
    // IUserManager* um = IUserManager::Probe(interfaceTmp);

    // AutoPtr<IBundle> bundle;
    // um->GetUserRestrictions((IBundle**)&bundle);
    // Boolean disallowedByUserManager;
    // bundle->GetBoolean(IUserManager::DISALLOW_INSTALL_UNKNOWN_SOURCES, FALSE, &disallowedByUserManager);

    // AutoPtr<ISettingsSecure> settingsSecure;
    // CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);

    // AutoPtr<IContentResolver> contentResolver;
    // GetContentResolver((IContentResolver**)&contentResolver);
    // Int32 val = 0;
    // settingsSecure->GetInt32(contentResolver, ISettingsSecure::INSTALL_NON_MARKET_APPS, 0, &val);
    // Boolean allowedBySecureSettings = val > 0;
    // return (allowedBySecureSettings && (!disallowedByUserManager));
    return TRUE;
}

Boolean CPackageInstallerActivity::IsInstallRequestFromUnknownSource(
    /* [in] */ IIntent* intent)
{
    String callerPackage;
    GetCallingPackage(&callerPackage);
    Boolean boolExtra = FALSE;
    intent->GetBooleanExtra(IIntent::EXTRA_NOT_UNKNOWN_SOURCE, FALSE, &boolExtra);
    if (callerPackage != NULL && boolExtra) {
        //try {
            mPm->GetApplicationInfo(callerPackage, 0, (IApplicationInfo**)&mSourceInfo);
            if (mSourceInfo != NULL) {
                Int32 flags = 0;
                mSourceInfo->GetFlags(&flags);
                if ((flags & IApplicationInfo::FLAG_PRIVILEGED) != 0) {
                    // Privileged apps are not considered an unknown source.
                    return FALSE;
                }
            }
        //} catch (NameNotFoundException e) {
        //}
    }
    return TRUE;
}

Boolean CPackageInstallerActivity::IsVerifyAppsEnabled()
{
    AutoPtr<IInterface> interfaceTmp;
    GetSystemService(USER_SERVICE, (IInterface**)&interfaceTmp);
    IUserManager* um = IUserManager::Probe(interfaceTmp);
    Boolean has = FALSE;
    um->HasUserRestriction(IUserManager::ENSURE_VERIFY_APPS, &has);
    if (has) {
        return TRUE;
    }

    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    Int32 val = 0;
    settingsGlobal->GetInt32(contentResolver, ISettingsGlobal::PACKAGE_VERIFIER_ENABLE, 1, &val);
    return val > 0;
}

Boolean CPackageInstallerActivity::IsAppVerifierInstalled()
{
    AutoPtr<IPackageManager> pm;
    GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IIntent> verification;
    CIntent::New(IIntent::ACTION_PACKAGE_NEEDS_VERIFICATION, (IIntent**)&verification);
    verification->SetType(PACKAGE_MIME_TYPE);
    verification->AddFlags(IIntent::FLAG_GRANT_READ_URI_PERMISSION);
    AutoPtr<IList> receivers;
    pm->QueryBroadcastReceivers(verification, 0, (IList**)&receivers);
    Int32 size = 0;
    receivers->GetSize(&size);
    return (size > 0) ? TRUE : FALSE;
}

void CPackageInstallerActivity::InitiateInstall()
{
    String pkgName;
    mPkgInfo->GetPackageName(&pkgName);
    // Check if there is already a package on the device with this name
    // but it has been renamed to something else.
    AutoPtr< ArrayOf<String> > packageNames = ArrayOf<String>::Alloc(1);
    packageNames->Set(0, pkgName);

    AutoPtr< ArrayOf<String> > oldName;
    mPm->CanonicalToCurrentPackageNames(*packageNames, (ArrayOf<String>**)&oldName);
    if (oldName != NULL && oldName->GetLength() > 0 && (*oldName)[0] != NULL) {
        pkgName = (*oldName)[0];
        mPkgInfo->SetPackageName(pkgName);

        AutoPtr<IApplicationInfo> applicationInfo;
        mPkgInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        IPackageItemInfo::Probe(applicationInfo)->SetPackageName(pkgName);
    }
    // Check if package is already installed. display confirmation dialog if replacing pkg
    //try {
        // This is a little convoluted because we want to get all uninstalled
        // apps, but this may include apps with just data, and if it is just
        // data we still want to count it as "installed".

        if (FAILED(mPm->GetApplicationInfo(pkgName, IPackageManager::GET_UNINSTALLED_PACKAGES, (IApplicationInfo**)&mAppInfo))) {
            mAppInfo = NULL;
        }
        else {
            Int32 flags = 0;
            mAppInfo->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_INSTALLED) == 0) {
                mAppInfo = NULL;
            }
        }
    //} catch (NameNotFoundException e) {
        //mAppInfo = null;
    //}

    mInstallFlowAnalytics->SetReplace(mAppInfo != NULL);
    Int32 flags = 0;
    mInstallFlowAnalytics->SetSystemApp((mAppInfo != NULL) &&
        (mAppInfo->GetFlags(&flags), (flags & IApplicationInfo::FLAG_SYSTEM) != 0));
    StartInstallConfirm();
}

AutoPtr<IApplicationInfo> CPackageInstallerActivity::GetSourceInfo()
{
    String callingPackage;
    GetCallingPackage(&callingPackage);
    if (callingPackage != NULL) {
        //try {
            AutoPtr<IApplicationInfo> applicationInfo;
            mPm->GetApplicationInfo(callingPackage, 0, (IApplicationInfo**)&applicationInfo);
            return applicationInfo;
        //} catch (NameNotFoundException ex) {
            // ignore
        //}
    }
    return NULL;
}

Int32 CPackageInstallerActivity::GetOriginatingUid(
    /* [in] */ IIntent* intent)
{
    // The originating uid from the intent. We only trust/use this if it comes from a
    // system application
    Int32 uidFromIntent = 0;
    intent->GetInt32Extra(IIntent::EXTRA_ORIGINATING_UID, IVerificationParams::NO_UID, &uidFromIntent);

    // Get the source info from the calling package, if available. This will be the
    // definitive calling package, but it only works if the intent was started using
    // startActivityForResult,
    AutoPtr<IApplicationInfo> sourceInfo = GetSourceInfo();
    if (sourceInfo != NULL) {
        Int32 flags = 0;
        mSourceInfo->GetFlags(&flags);
        if (uidFromIntent != IVerificationParams::NO_UID && (flags & IApplicationInfo::FLAG_PRIVILEGED) != 0) {
            return uidFromIntent;
        }
        // We either didn't get a uid in the intent, or we don't trust it. Use the
        // uid of the calling package instead.
        Int32 uid = 0;
        sourceInfo->GetUid(&uid);
        return uid;
    }

    // We couldn't get the specific calling package. Let's get the uid instead
    Int32 callingUid;
    //try {
        AutoPtr<IIActivityManager> activityManager = ActivityManagerNative::GetDefault();
        AutoPtr<IBinder> binder;
        GetActivityToken((IBinder**)&binder);
        activityManager->GetLaunchedFromUid(binder, &callingUid);
    //} catch (android.os.RemoteException ex) {
        //Log.w(TAG, "Could not determine the launching uid.");
        // nothing else we can do
        //return VerificationParams.NO_UID;
    //}

    // If we got a uid from the intent, we need to verify that the caller is a
    // privileged system package before we use it
    if (uidFromIntent != IVerificationParams::NO_UID) {
        AutoPtr< ArrayOf<String> > callingPackages;
        mPm->GetPackagesForUid(callingUid, (ArrayOf<String>**)&callingPackages);
        if (callingPackages != NULL) {
            String packageName;
            for (Int32 idx=0; idx<callingPackages->GetLength(); ++idx) {
                packageName = (*callingPackages)[idx];
                //try {
                    AutoPtr<IApplicationInfo> applicationInfo;
                    mPm->GetApplicationInfo(packageName, 0, (IApplicationInfo**)&applicationInfo);

                    Int32 flags;
                    applicationInfo->GetFlags(&flags);
                    if ((flags & IApplicationInfo::FLAG_PRIVILEGED) != 0) {
                        return uidFromIntent;
                    }
                //} catch (NameNotFoundException ex) {
                    // ignore it, and try the next package
                //}
            }
        }
    }
    // We either didn't get a uid from the intent, or we don't trust it. Use the
    // calling uid instead.
    return callingUid;
}

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos


