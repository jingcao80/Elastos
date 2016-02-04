
#include "elastos/droid/R.h"
#include <Elastos.Droid.Graphics.h>
#include "elastos/apps/packageinstaller/CInstallAppProgress.h"
#include "elastos/apps/packageinstaller/PackageUtil.h"
#include <elastos/utility/logging/Logger.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>

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
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Apps::PackageInstaller::EIID_IInstallAppProgress;
using Elastos::Core::CString;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

//=====================================================================
//              CInstallAppProgress::PackageInstallObserver
//=====================================================================
CAR_INTERFACE_IMPL(CInstallAppProgress::PackageInstallObserver, Object, IIPackageInstallObserver)

CInstallAppProgress::PackageInstallObserver::PackageInstallObserver(
    /* [in] */ CInstallAppProgress* owner)
    : mOwner(owner)
{
    assert(NULL != mOwner);
}

ECode CInstallAppProgress::PackageInstallObserver::PackageInstalled(
    /* [in] */ String packageName,
    /* [in] */ Int32 returnCode)
{
    // ==================before translated======================
    // Message msg = mHandler.obtainMessage(INSTALL_COMPLETE);
    // msg.arg1 = returnCode;
    // mHandler.sendMessage(msg);

    AutoPtr<IMessage> msg;
    mOwner->mHandler->ObtainMessage(mOwner->INSTALL_COMPLETE, (IMessage**)&msg);
    msg->SetArg1(returnCode);
    Boolean resTmp;
    mOwner->mHandler->SendMessage(msg, &resTmp);
    return NOERROR;
}

//=====================================================================
//                   CInstallAppProgress::InnerHandler
//=====================================================================
CAR_INTERFACE_IMPL(CInstallAppProgress::InnerHandler, Object, IHandler)

CInstallAppProgress::InnerHandler::InnerHandler(
    /* [in] */ CInstallAppProgress* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(NULL != mOwner);
}

ECode CInstallAppProgress::InnerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    // switch (msg.what) {
    //     case INSTALL_COMPLETE:
    //         mInstallFlowAnalytics.setFlowFinishedWithPackageManagerResult(msg.arg1);
    //         if (getIntent().getBooleanExtra(Intent.EXTRA_RETURN_RESULT, false)) {
    //             Intent result = new Intent();
    //             result.putExtra(Intent.EXTRA_INSTALL_RESULT, msg.arg1);
    //             setResult(msg.arg1 == PackageManager.INSTALL_SUCCEEDED
    //                     ? Activity.RESULT_OK : Activity.RESULT_FIRST_USER,
    //                             result);
    //             finish();
    //             return;
    //         }
    //         // Update the status text
    //         mProgressBar.setVisibility(View.INVISIBLE);
    //         // Show the ok button
    //         int centerTextLabel;
    //         int centerExplanationLabel = -1;
    //         LevelListDrawable centerTextDrawable = (LevelListDrawable) getResources()
    //                 .getDrawable(R.drawable.ic_result_status);
    //         if (msg.arg1 == PackageManager.INSTALL_SUCCEEDED) {
    //             mLaunchButton.setVisibility(View.VISIBLE);
    //             centerTextDrawable.setLevel(0);
    //             centerTextLabel = R.string.install_done;
    //             // Enable or disable launch button
    //             mLaunchIntent = getPackageManager().getLaunchIntentForPackage(
    //                     mAppInfo.packageName);
    //             boolean enabled = false;
    //             if(mLaunchIntent != null) {
    //                 List<ResolveInfo> list = getPackageManager().
    //                         queryIntentActivities(mLaunchIntent, 0);
    //                 if (list != null && list.size() > 0) {
    //                     enabled = true;
    //                 }
    //             }
    //             if (enabled) {
    //                 mLaunchButton.setOnClickListener(CInstallAppProgress.this);
    //             } else {
    //                 mLaunchButton.setEnabled(false);
    //             }
    //         } else if (msg.arg1 == PackageManager.INSTALL_FAILED_INSUFFICIENT_STORAGE){
    //             showDialogInner(DLG_OUT_OF_SPACE);
    //             return;
    //         } else {
    //             // Generic error handling for all other error codes.
    //             centerTextDrawable.setLevel(1);
    //             centerExplanationLabel = getExplanationFromErrorCode(msg.arg1);
    //             centerTextLabel = R.string.install_failed;
    //             mLaunchButton.setVisibility(View.INVISIBLE);
    //         }
    //         if (centerTextDrawable != null) {
    //         centerTextDrawable.setBounds(0, 0,
    //                 centerTextDrawable.getIntrinsicWidth(),
    //                 centerTextDrawable.getIntrinsicHeight());
    //             mStatusTextView.setCompoundDrawablesRelative(centerTextDrawable, null,
    //                     null, null);
    //         }
    //         mStatusTextView.setText(centerTextLabel);
    //         if (centerExplanationLabel != -1) {
    //             mExplanationTextView.setText(centerExplanationLabel);
    //             mExplanationTextView.setVisibility(View.VISIBLE);
    //         } else {
    //             mExplanationTextView.setVisibility(View.GONE);
    //         }
    //         mDoneButton.setOnClickListener(CInstallAppProgress.this);
    //         mOkPanel.setVisibility(View.VISIBLE);
    //         break;
    //     default:
    //         break;
    // }

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case CInstallAppProgress::INSTALL_COMPLETE:
            {
                Int32 arg1;
                msg->GetArg1(&arg1);
                mOwner->mInstallFlowAnalytics->SetFlowFinishedWithPackageManagerResult(arg1);

                AutoPtr<IIntent> intent;
                //--: GetIntent((IIntent**)&intent);
                Boolean resTmp;
                intent->GetBooleanExtra(IIntent::EXTRA_RETURN_RESULT, FALSE, &resTmp);
                if (resTmp) {
                    AutoPtr<IIntent> result;
                    CIntent::New((IIntent**)&result);
                    result->PutExtra(IIntent::EXTRA_INSTALL_RESULT, arg1);
                    //--: SetResult(arg1 == IPackageManager::INSTALL_SUCCEEDED
                    //        ? IActivity::RESULT_OK : IActivity::RESULT_FIRST_USER, result);
                    //--: Finish();
                    return NOERROR;
                }

                // Update the status text
                IView::Probe(mOwner->mProgressBar)->SetVisibility(IView::INVISIBLE);
                // Show the ok button
                Int32 centerTextLabel = 0;
                Int32 centerExplanationLabel = -1;

                AutoPtr<IResources> resources;
                //--: GetResources((IResources**)&resources);
                AutoPtr<IDrawable> drawable;
                resources->GetDrawable(-1/*R::drawable::ic_result_status*/, (IDrawable**)&drawable);
                ILevelListDrawable* centerTextDrawable = ILevelListDrawable::Probe(drawable);

                if (arg1 == IPackageManager::INSTALL_SUCCEEDED) {
                    IView::Probe(mOwner->mLaunchButton)->SetVisibility(IView::VISIBLE);
                    Boolean resTmp;
                    IDrawable::Probe(centerTextDrawable)->SetLevel(0, &resTmp);
                    //--: centerTextLabel = R::string::install_done;
                    // Enable or disable launch button
                    AutoPtr<IPackageManager> packageManager;
                    //--: GetPackageManager((IPackageManager**)&packageManager);
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
                        IView::Probe(mOwner->mLaunchButton)->SetOnClickListener(mOwner);
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
                    //--: centerTextLabel = R::string::install_failed;
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
                IView::Probe(mOwner->mDoneButton)->SetOnClickListener(mOwner);
                mOwner->mOkPanel->SetVisibility(IView::VISIBLE);
            }
            break;
        default:
            break;
    }
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
    //--: StartActivity(intent);
    //--: Finish();
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
    //--: Finish();
    return NOERROR;
}

//=====================================================================
//                          CInstallAppProgress
//=====================================================================
const Int32 CInstallAppProgress::DLG_OUT_OF_SPACE;
const Int32 CInstallAppProgress::INSTALL_COMPLETE;

CAR_INTERFACE_IMPL_3(CInstallAppProgress, Object/*Activity*/, IViewOnClickListener, IDialogInterfaceOnCancelListener, IInstallAppProgress)

CInstallAppProgress::CInstallAppProgress()
    : TAG("CInstallAppProgress")
{
}

ECode CInstallAppProgress::OnCreate(
    /* [in] */ IBundle* icicle)
{
    VALIDATE_NOT_NULL(icicle);
    // ==================before translated======================
    // super.onCreate(icicle);
    // Intent intent = getIntent();
    // mAppInfo = intent.getParcelableExtra(PackageUtil.INTENT_ATTR_APPLICATION_INFO);
    // mInstallFlowAnalytics = intent.getParcelableExtra(EXTRA_INSTALL_FLOW_ANALYTICS);
    // mInstallFlowAnalytics.setContext(this);
    // mPackageURI = intent.getData();
    //
    // final String scheme = mPackageURI.getScheme();
    // if (scheme != null && !"file".equals(scheme) && !"package".equals(scheme)) {
    //     mInstallFlowAnalytics.setFlowFinished(
    //             InstallFlowAnalytics.RESULT_FAILED_UNSUPPORTED_SCHEME);
    //     throw new IllegalArgumentException("unexpected scheme " + scheme);
    // }
    //
    // initView();

    assert(0);
    //--: Activity::OnCreate(icicle);
    AutoPtr<IIntent> intent;
    //--: GetIntent((IIntent**)&intent);

    AutoPtr<IParcelable> parcelableTmp;
    intent->GetParcelableExtra(PackageUtil::INTENT_ATTR_APPLICATION_INFO, (IParcelable**)&parcelableTmp);
    mAppInfo = IApplicationInfo::Probe(parcelableTmp);

    AutoPtr<IParcelable> parcelableTmp1;
    intent->GetParcelableExtra(IInstallAppProgress::EXTRA_INSTALL_FLOW_ANALYTICS, (IParcelable**)&parcelableTmp1);
    IObject* objTmp = IObject::Probe(parcelableTmp1);
    mInstallFlowAnalytics = (InstallFlowAnalytics*)objTmp;

    mInstallFlowAnalytics->SetContext(IContext::Probe(this));
    intent->GetData((IUri**)&mPackageURI);

    String scheme;
    mPackageURI->GetScheme(&scheme);
    if (scheme != NULL && !String("file").Equals(scheme) && !String("package").Equals(scheme)) {
        mInstallFlowAnalytics->SetFlowFinished(InstallFlowAnalytics::RESULT_FAILED_UNSUPPORTED_SCHEME);
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
    // ==================before translated======================
    //  switch (id) {
    //  case DLG_OUT_OF_SPACE:
    //      String dlgText = getString(R.string.out_of_space_dlg_text, mLabel);
    //      return new AlertDialog.Builder(this)
    //              .setTitle(R.string.out_of_space_dlg_title)
    //              .setMessage(dlgText)
    //              .setPositiveButton(R.string.manage_applications, new DialogInterface.OnClickListener() {
    //                  public void onClick(DialogInterface dialog, int which) {
    //                      //launch manage applications
    //                      Intent intent = new Intent("android.intent.action.MANAGE_PACKAGE_STORAGE");
    //                      startActivity(intent);
    //                      finish();
    //                  }
    //              })
    //              .setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
    //                  public void onClick(DialogInterface dialog, int which) {
    //                      Log.i(TAG, "Canceling installation");
    //                      finish();
    //                  }
    //              })
    //              .setOnCancelListener(this)
    //              .create();
    //  }
    // return null;

    assert(0);
    switch (id) {
    case DLG_OUT_OF_SPACE:
        {
            String dlgText;
            //--: GetString(R::string::out_of_space_dlg_text, mLabel, &dlgText);

            AutoPtr<IAlertDialogBuilder> resTmp;
            CAlertDialogBuilder::New(IContext::Probe(this), (IAlertDialogBuilder**)&resTmp);

            AutoPtr<ICharSequence> charSequenceTmp;
            CString::New(String("")/*R::string::out_of_space_dlg_title*/, (ICharSequence**)&charSequenceTmp);
            resTmp->SetTitle(charSequenceTmp);

            charSequenceTmp = NULL;
            CString::New(dlgText, (ICharSequence**)&charSequenceTmp);
            resTmp->SetMessage(charSequenceTmp);

            charSequenceTmp = NULL;
            CString::New(String("")/*R::string::manage_applications*/, (ICharSequence**)&charSequenceTmp);
            AutoPtr<IDialogInterfaceOnClickListener> startActivityListener = new InnerStartActivityOnClickListener(this);
            resTmp->SetPositiveButton(charSequenceTmp, startActivityListener);

            charSequenceTmp = NULL;
            CString::New(String("")/*R::string::cancel*/, (ICharSequence**)&charSequenceTmp);
            AutoPtr<IDialogInterfaceOnClickListener> logListener = new InnerLogOnClickListener(this);
            resTmp->SetNegativeButton(charSequenceTmp, logListener);
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
    // ==================before translated======================
    // setContentView(R.layout.op_progress);
    // int installFlags = 0;
    // PackageManager pm = getPackageManager();
    // try {
    //     PackageInfo pi = pm.getPackageInfo(mAppInfo.packageName,
    //             PackageManager.GET_UNINSTALLED_PACKAGES);
    //     if(pi != null) {
    //         installFlags |= PackageManager.INSTALL_REPLACE_EXISTING;
    //     }
    // } catch (NameNotFoundException e) {
    // }
    // if((installFlags & PackageManager.INSTALL_REPLACE_EXISTING )!= 0) {
    //     Log.w(TAG, "Replacing package:" + mAppInfo.packageName);
    // }
    //
    // final PackageUtil.AppSnippet as;
    // if ("package".equals(mPackageURI.getScheme())) {
    //     as = new PackageUtil.AppSnippet(pm.getApplicationLabel(mAppInfo),
    //             pm.getApplicationIcon(mAppInfo));
    // } else {
    //     final File sourceFile = new File(mPackageURI.getPath());
    //     as = PackageUtil.getAppSnippet(this, mAppInfo, sourceFile);
    // }
    // mLabel = as.label;
    // PackageUtil.initSnippetForNewApp(this, as, R.id.app_snippet);
    // mStatusTextView = (TextView)findViewById(R.id.center_text);
    // mStatusTextView.setText(R.string.installing);
    // mExplanationTextView = (TextView) findViewById(R.id.center_explanation);
    // mProgressBar = (ProgressBar) findViewById(R.id.progress_bar);
    // mProgressBar.setIndeterminate(true);
    // // Hide button till progress is being displayed
    // mOkPanel = (View)findViewById(R.id.buttons_panel);
    // mDoneButton = (Button)findViewById(R.id.done_button);
    // mLaunchButton = (Button)findViewById(R.id.launch_button);
    // mOkPanel.setVisibility(View.INVISIBLE);
    //
    // String installerPackageName = getIntent().getStringExtra(
    //         Intent.EXTRA_INSTALLER_PACKAGE_NAME);
    // Uri originatingURI = getIntent().getParcelableExtra(Intent.EXTRA_ORIGINATING_URI);
    // Uri referrer = getIntent().getParcelableExtra(Intent.EXTRA_REFERRER);
    // int originatingUid = getIntent().getIntExtra(Intent.EXTRA_ORIGINATING_UID,
    //         VerificationParams.NO_UID);
    // ManifestDigest manifestDigest = getIntent().getParcelableExtra(EXTRA_MANIFEST_DIGEST);
    // VerificationParams verificationParams = new VerificationParams(null, originatingURI,
    //         referrer, originatingUid, manifestDigest);
    // PackageInstallObserver observer = new PackageInstallObserver();
    //
    // if ("package".equals(mPackageURI.getScheme())) {
    //     try {
    //         pm.installExistingPackage(mAppInfo.packageName);
    //         observer.packageInstalled(mAppInfo.packageName,
    //                 PackageManager.INSTALL_SUCCEEDED);
    //     } catch (PackageManager.NameNotFoundException e) {
    //         observer.packageInstalled(mAppInfo.packageName,
    //                 PackageManager.INSTALL_FAILED_INVALID_APK);
    //     }
    // } else {
    //     pm.installPackageWithVerificationAndEncryption(mPackageURI, observer, installFlags,
    //             installerPackageName, verificationParams, null);
    // }

    assert(0);
    //--: SetContentView(R::layout::op_progress);
    Int32 installFlags = 0;
    AutoPtr<IPackageManager> pm;
    //--: GetPackageManager((IPackageManager**)&pm);

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
    if (String("package").Equals(scheme)) {

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
    PackageUtil::InitSnippetForNewApp(IActivity::Probe(this), as, -1/*R::id::app_snippet*/);

    AutoPtr<IView> viewTmp;
    //--: FindViewById(R::id::center_text, (IView**)&viewTmp);
    mStatusTextView = ITextView::Probe(viewTmp);

    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(String("")/*R::string::installing*/, (ICharSequence**)&charSequenceTmp);
    mStatusTextView->SetText(charSequenceTmp);

    viewTmp = NULL;
    //--: FindViewById(R.id.center_explanation, (IView**)&viewTmp);
    mExplanationTextView = ITextView::Probe(viewTmp);

    viewTmp = NULL;
    //--: FindViewById(R::id::progress_bar, (IView**)&viewTmp);
    mProgressBar = IProgressBar::Probe(viewTmp);
    mProgressBar->SetIndeterminate(TRUE);
    // Hide button till progress is being displayed

    //--: FindViewById(R::id::buttons_panel, (IView**)&mOkPanel);

    viewTmp = NULL;
    //--: FindViewById(R::id::done_button, (IView**)&viewTmp);
    mDoneButton = IButton::Probe(viewTmp);

    viewTmp = NULL;
    //--: FindViewById(R::id::launch_button, (IView**)&viewTmp);
    mLaunchButton = IButton::Probe(viewTmp);
    mOkPanel->SetVisibility(IView::INVISIBLE);

    AutoPtr<IIntent> intentTmp;
    //--: GetIntent((IIntent**)&intentTmp);

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
    intentTmp->GetParcelableExtra(IInstallAppProgress::EXTRA_MANIFEST_DIGEST, (IParcelable**)&parcelableTmp);
    IManifestDigest* manifestDigest = IManifestDigest::Probe(parcelableTmp);

    AutoPtr<IVerificationParams> verificationParams;
    CVerificationParams::New(NULL, originatingURI,
            referrer, originatingUid, manifestDigest, (IVerificationParams**)&verificationParams);

    //--: PackageInstallObserver observer = new PackageInstallObserver();
    //--: mPackageURI->GetScheme(&scheme);
    if (String("package").Equals(scheme)) {
        //try {
            String packageName;
            IPackageItemInfo::Probe(mAppInfo)->GetPackageName(&packageName);
            Int32 resTmp;
            pm->InstallExistingPackage(packageName, &resTmp);
            //--: observer.packageInstalled(mAppInfo.packageName, IPackageManager::INSTALL_SUCCEEDED);
        //} catch (PackageManager.NameNotFoundException e) {
            //observer.packageInstalled(mAppInfo.packageName,
            //        PackageManager.INSTALL_FAILED_INVALID_APK);
        //}
    }
    else {
        assert(0);
        //IIPackageInstallObserver* installObserver = NULL;
        //IContainerEncryptionParams* encryptionParams = NULL;
        //--: pm->InstallPackageWithVerificationAndEncryption(mPackageURI, NULL/*observer*/, installFlags, installerPackageName, verificationParams, NULL);
    }
    return NOERROR;
}

ECode CInstallAppProgress::OnClick(
    /* [in] */ IView* v)
{
    VALIDATE_NOT_NULL(v);
    // ==================before translated======================
    // if(v == mDoneButton) {
    //     if (mAppInfo.packageName != null) {
    //         Log.i(TAG, "Finished installing "+mAppInfo.packageName);
    //     }
    //     finish();
    // } else if(v == mLaunchButton) {
    //     startActivity(mLaunchIntent);
    //     finish();
    // }

    assert(0);
    if(TO_IINTERFACE(v) == TO_IINTERFACE(mDoneButton)) {
        String packageName;
        IPackageItemInfo::Probe(mAppInfo)->GetPackageName(&packageName);
        if (packageName != "") {
            Logger::I(TAG, String("Finished installing ") + packageName);
        }
        //--: Finish();
    }
    else if(TO_IINTERFACE(v) == TO_IINTERFACE(mLaunchButton)) {
        //--: StartActivity(mLaunchIntent);
        //--: Finish();
    }
    return NOERROR;
}

ECode CInstallAppProgress::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    VALIDATE_NOT_NULL(dialog);
    // ==================before translated======================
    // finish();

    assert(0);
    //--: Finish();
    return NOERROR;
}

ECode CInstallAppProgress::OnDestroy()
{
    // ==================before translated======================
    // super.onDestroy();

    assert(0);
    //--: Activity::OnDestroy();
    return NOERROR;
}

Int32 CInstallAppProgress::GetExplanationFromErrorCode(
    /* [in] */ Int32 errCode)
{
    // ==================before translated======================
    // Log.d(TAG, "Installation error code: " + errCode);
    // switch (errCode) {
    //     case PackageManager.INSTALL_FAILED_INVALID_APK:
    //         return R.string.install_failed_invalid_apk;
    //     case PackageManager.INSTALL_PARSE_FAILED_INCONSISTENT_CERTIFICATES:
    //         return R.string.install_failed_inconsistent_certificates;
    //     case PackageManager.INSTALL_FAILED_OLDER_SDK:
    //         return R.string.install_failed_older_sdk;
    //     case PackageManager.INSTALL_FAILED_CPU_ABI_INCOMPATIBLE:
    //         return R.string.install_failed_cpu_abi_incompatible;
    //     default:
    //         return -1;
    // }

    assert(0);
    Logger::D(TAG, "Installation error code: %d", errCode);
    switch (errCode) {
        case IPackageManager::INSTALL_FAILED_INVALID_APK:
            return -1;//R::string::install_failed_invalid_apk;
        case IPackageManager::INSTALL_PARSE_FAILED_INCONSISTENT_CERTIFICATES:
            return -1;//R::string::install_failed_inconsistent_certificates;
        case IPackageManager::INSTALL_FAILED_OLDER_SDK:
            return -1;//R::string::install_failed_older_sdk;
        case IPackageManager::INSTALL_FAILED_CPU_ABI_INCOMPATIBLE:
            return -1;//R::string::install_failed_cpu_abi_incompatible;
        default:
            return -1;
    }
}

void CInstallAppProgress::ShowDialogInner(
    /* [in] */ Int32 id)
{
    // ==================before translated======================
    // removeDialog(id);
    // //ActionsCode(liuxinxu, BUGFIX: BUG00253949,APP crash when show dialog.)
    // if (!isFinishing()) {
    //     showDialog(id);
    // } else {
    //     Log.e(TAG, "APP already finished.i can't show any dialog at all.");
    // }

    assert(0);
    //--: RemoveDialog(id);
    //ActionsCode(liuxinxu, BUGFIX: BUG00253949,APP crash when show dialog.)
    Boolean isFinishing = FALSE;
    //--: IsFinishing(&isFinishing);
    if (!isFinishing) {
        //--: ShowDialog(id);
    }
    else {
        Logger::E(TAG, "APP already finished.i can't show any dialog at all.");
    }
}

} // namespace PackageInstaller
} // namespace Apps
} // namespace Elastos


