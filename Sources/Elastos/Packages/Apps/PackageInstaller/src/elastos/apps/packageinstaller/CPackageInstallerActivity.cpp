
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/SystemClock.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Provider.h>
#include "elastos/apps/packageinstaller/CPackageInstallerActivity.h"
#include "elastos/apps/packageinstaller/PackageUtil.h"
#include "elastos/apps/packageinstaller/TabsAdapter.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>

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
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Droid::Widget::ITabSpec;
using Elastos::Droid::Widget::ITextView;
using Elastos::Apps::PackageInstaller::EIID_IPackageInstallerActivity;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

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
    //--: mOwner->Finish();
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
    //--: Finish();
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
    //StartActivity(intent);
    //--: Finish();
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
    //--: Finish();
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
    //--: Finish();
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
    //--: SetResult(mOwner->RESULT_CANCELED);
    //--: Finish();
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
    //--: GetSharedPreferences(PREFS_ALLOWED_SOURCES, IContext::MODE_PRIVATE, (ISharedPreferences**)&prefs);
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
    // ==================before translated======================
    // mOwner = owner;

    assert(NULL != mOwner);
}

ECode CPackageInstallerActivity::InnerTabHostOnTabChangeListener::OnTabChanged(
    /* [in] */ const String& tabId)
{
    // ==================before translated======================
    // if (TAB_ID_ALL.equals(tabId)) {
    //     mInstallFlowAnalytics.setAllPermissionsDisplayed(true);
    // } else if (TAB_ID_NEW.equals(tabId)) {
    //     mInstallFlowAnalytics.setNewPermissionsDisplayed(true);
    // }

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
    // ==================before translated======================
    // mOwner = owner;

    assert(NULL != mOwner);
}

ECode CPackageInstallerActivity::InnerRunnable::Run()
{
    // ==================before translated======================
    // mOk.setText(R.string.install);
    // mOkCanInstall = true;

    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(String("")/*R::string::install*/, (ICharSequence**)&charSequenceTmp);
    ITextView::Probe(mOwner->mOk)->SetText(charSequenceTmp);
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

CAR_INTERFACE_IMPL_3(CPackageInstallerActivity, Object/*Activity*/, IDialogInterfaceOnCancelListener, IViewOnClickListener, IPackageInstallerActivity)

AutoPtr<IDialog> CPackageInstallerActivity::OnCreateDialog(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* bundle)
{
    // ==================before translated======================
    //  switch (id) {
    //  case DLG_UNKNOWN_APPS:
    //      return new AlertDialog.Builder(this)
    //              .setTitle(R.string.unknown_apps_dlg_title)
    //              .setMessage(R.string.unknown_apps_dlg_text)
    //              .setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
    //                  public void onClick(DialogInterface dialog, int which) {
    //                      Log.i(TAG, "Finishing off activity so that user can navigate to settings manually");
    //                      finish();
    //                  }})
    //              .setPositiveButton(R.string.settings, new DialogInterface.OnClickListener() {
    //                  public void onClick(DialogInterface dialog, int which) {
    //                      Log.i(TAG, "Launching settings");
    //                      launchSettingsAppAndFinish();
    //                  }
    //              })
    //              .setOnCancelListener(this)
    //              .create();
    //  case DLG_PACKAGE_ERROR :
    //      return new AlertDialog.Builder(this)
    //              .setTitle(R.string.Parse_error_dlg_title)
    //              .setMessage(R.string.Parse_error_dlg_text)
    //              .setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
    //                  public void onClick(DialogInterface dialog, int which) {
    //                      finish();
    //                  }
    //              })
    //              .setOnCancelListener(this)
    //              .create();
    //  case DLG_OUT_OF_SPACE:
    //      // Guaranteed not to be null. will default to package name if not set by app
    //      CharSequence appTitle = mPm.getApplicationLabel(mPkgInfo.applicationInfo);
    //      String dlgText = getString(R.string.out_of_space_dlg_text,
    //              appTitle.toString());
    //      return new AlertDialog.Builder(this)
    //              .setTitle(R.string.out_of_space_dlg_title)
    //              .setMessage(dlgText)
    //              .setPositiveButton(R.string.manage_applications, new DialogInterface.OnClickListener() {
    //                  public void onClick(DialogInterface dialog, int which) {
    //                      //launch manage applications
    //                      Intent intent = new Intent("android.intent.action.MANAGE_PACKAGE_STORAGE");
    //                      intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
    //                      startActivity(intent);
    //                      finish();
    //                  }
    //              })
    //              .setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
    //                  public void onClick(DialogInterface dialog, int which) {
    //                      Log.i(TAG, "Canceling installation");
    //                      finish();
    //                  }
    //            })
    //            .setOnCancelListener(this)
    //            .create();
    //  case DLG_INSTALL_ERROR :
    //      // Guaranteed not to be null. will default to package name if not set by app
    //      CharSequence appTitle1 = mPm.getApplicationLabel(mPkgInfo.applicationInfo);
    //      String dlgText1 = getString(R.string.install_failed_msg,
    //              appTitle1.toString());
    //      return new AlertDialog.Builder(this)
    //              .setTitle(R.string.install_failed)
    //              .setNeutralButton(R.string.ok, new DialogInterface.OnClickListener() {
    //                  public void onClick(DialogInterface dialog, int which) {
    //                      finish();
    //                  }
    //              })
    //              .setMessage(dlgText1)
    //              .setOnCancelListener(this)
    //              .create();
    //  case DLG_ALLOW_SOURCE:
    //      CharSequence appTitle2 = mPm.getApplicationLabel(mSourceInfo);
    //      String dlgText2 = getString(R.string.allow_source_dlg_text,
    //              appTitle2.toString());
    //      return new AlertDialog.Builder(this)
    //              .setTitle(R.string.allow_source_dlg_title)
    //              .setMessage(dlgText2)
    //              .setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
    //                  public void onClick(DialogInterface dialog, int which) {
    //                      setResult(RESULT_CANCELED);
    //                      finish();
    //                  }})
    //              .setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
    //                  public void onClick(DialogInterface dialog, int which) {
    //                      SharedPreferences prefs = getSharedPreferences(PREFS_ALLOWED_SOURCES,
    //                              Context.MODE_PRIVATE);
    //                      prefs.edit().putBoolean(mSourceInfo.packageName, true).apply();
    //                      startInstallConfirm();
    //                  }
    //              })
    //              .setOnCancelListener(this)
    //              .create();
    // }
    // return null;

    switch (id) {
    case DLG_UNKNOWN_APPS:
        {
            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(IContext::Probe(this), (IAlertDialogBuilder**)&builder);
            AutoPtr<ICharSequence> charSequenceTmp;
            CString::New(String("")/*R::string::unknown_apps_dlg_title*/, (ICharSequence**)&charSequenceTmp);
            builder->SetTitle(charSequenceTmp);

            charSequenceTmp = NULL;
            CString::New(String("")/*R.string.unknown_apps_dlg_text*/, (ICharSequence**)&charSequenceTmp);
            builder->SetMessage(charSequenceTmp);

            charSequenceTmp = NULL;
            CString::New(String("")/*R.string.cancel*/, (ICharSequence**)&charSequenceTmp);
            AutoPtr<IDialogInterfaceOnClickListener> listener = new InnerOnClickListener(this);
            builder->SetNegativeButton(charSequenceTmp, listener);

            charSequenceTmp = NULL;
            CString::New(String("")/*R.string.settings*/, (ICharSequence**)&charSequenceTmp);
            AutoPtr<IDialogInterfaceOnClickListener> listener1 = new InnerOnClickListener1(this);
            builder->SetPositiveButton(charSequenceTmp, listener1);

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

            AutoPtr<ICharSequence> charSequenceTmp;
            CString::New(String("")/*R::string::Parse_error_dlg_title*/, (ICharSequence**)&charSequenceTmp);
            builder->SetTitle(charSequenceTmp);

            charSequenceTmp = NULL;
            CString::New(String("")/*R.string.Parse_error_dlg_text*/, (ICharSequence**)&charSequenceTmp);
            builder->SetMessage(charSequenceTmp);

            charSequenceTmp = NULL;
            CString::New(String("")/*R.string.ok*/, (ICharSequence**)&charSequenceTmp);
            AutoPtr<IDialogInterfaceOnClickListener> listener = new InnerOnClickListener2(this);
            builder->SetPositiveButton(charSequenceTmp, listener);
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

            String sAppTitle;
            appTitle->ToString(&sAppTitle);
            String dlgText;// = GetString(String("")/*R.string.out_of_space_dlg_text*/, sAppTitle);

            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(IContext::Probe(this), (IAlertDialogBuilder**)&builder);

            AutoPtr<ICharSequence> charSequenceTmp;
            CString::New(String("")/*R.string.out_of_space_dlg_title*/, (ICharSequence**)&charSequenceTmp);
            builder->SetTitle(charSequenceTmp);

            charSequenceTmp = NULL;
            CString::New(dlgText, (ICharSequence**)&charSequenceTmp);
            builder->SetMessage(charSequenceTmp);

            charSequenceTmp = NULL;
            CString::New(String("")/*R.string.manage_applications*/, (ICharSequence**)&charSequenceTmp);
            AutoPtr<IDialogInterfaceOnClickListener> listener = new InnerOnClickListener3(this);
            builder->SetPositiveButton(charSequenceTmp, listener);

            charSequenceTmp = NULL;
            CString::New(String("")/*R.string.cancel*/, (ICharSequence**)&charSequenceTmp);
            AutoPtr<IDialogInterfaceOnClickListener> listener1 = new InnerOnClickListener4(this);
            builder->SetNegativeButton(charSequenceTmp, listener1);
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

            String sAppTitle;
            appTitle1->ToString(&sAppTitle);
            String dlgText1;// = GetString(R.string.install_failed_msg, sAppTitle);

            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(IContext::Probe(this), (IAlertDialogBuilder**)&builder);

            AutoPtr<ICharSequence> charSequenceTmp;
            CString::New(String("")/*R.string.install_failed*/, (ICharSequence**)&charSequenceTmp);
            builder->SetTitle(charSequenceTmp);

            charSequenceTmp = NULL;
            CString::New(String("")/*R.string.ok*/, (ICharSequence**)&charSequenceTmp);
            AutoPtr<IDialogInterfaceOnClickListener> listener = new InnerOnClickListener5(this);
            builder->SetNeutralButton(charSequenceTmp, listener);

            charSequenceTmp = NULL;
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

            String sAppTitle2;
            appTitle2->ToString(&sAppTitle2);
            String dlgText2;// = GetString(R.string.allow_source_dlg_text, sAppTitle2);

            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(IContext::Probe(this), (IAlertDialogBuilder**)&builder);

            AutoPtr<ICharSequence> charSequenceTmp;
            CString::New(String("")/*R.string.allow_source_dlg_title*/, (ICharSequence**)&charSequenceTmp);
            builder->SetTitle(charSequenceTmp);

            charSequenceTmp = NULL;
            CString::New(dlgText2, (ICharSequence**)&charSequenceTmp);
            builder->SetMessage(charSequenceTmp);

            charSequenceTmp = NULL;
            CString::New(String("")/*R.string.cancel*/, (ICharSequence**)&charSequenceTmp);
            AutoPtr<IDialogInterfaceOnClickListener> listener = new InnerOnClickListener5(this);
            builder->SetNegativeButton(charSequenceTmp, listener);

            charSequenceTmp = NULL;
            CString::New(String("")/*R.string.ok*/, (ICharSequence**)&charSequenceTmp);
            AutoPtr<IDialogInterfaceOnClickListener> listener1 = new InnerOnClickListener6(this);
            builder->SetNeutralButton(charSequenceTmp, listener1);
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
    // ==================before translated======================
    // Intent result = new Intent();
    // result.putExtra(Intent.EXTRA_INSTALL_RESULT, pmResult);
    // setResult(pmResult == PackageManager.INSTALL_SUCCEEDED
    //         ? RESULT_OK : RESULT_FIRST_USER, result);

    AutoPtr<IIntent> result;
    CIntent::New((IIntent**)&result);
    result->PutExtra(IIntent::EXTRA_INSTALL_RESULT, pmResult);
    //SetResult(pmResult == IPackageManager::INSTALL_SUCCEEDED ? RESULT_OK : RESULT_FIRST_USER, result);
    return NOERROR;
}

ECode CPackageInstallerActivity::OnBackPressed()
{
    // ==================before translated======================
    // if (mSessionId != -1) {
    //     mInstaller.setPermissionsResult(mSessionId, false);
    // }
    // mInstallFlowAnalytics.setFlowFinished(
    //         InstallFlowAnalytics.RESULT_CANCELLED_BY_USER);
    // super.onBackPressed();

    if (mSessionId != -1) {
        mInstaller->SetPermissionsResult(mSessionId, FALSE);
    }
    mInstallFlowAnalytics->SetFlowFinished(InstallFlowAnalytics::RESULT_CANCELLED_BY_USER);
    //Activity::OnBackPressed();
    return NOERROR;
}

ECode CPackageInstallerActivity::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    VALIDATE_NOT_NULL(dialog);
    // ==================before translated======================
    // finish();

    //--:Finish();
    return NOERROR;
}

ECode CPackageInstallerActivity::OnClick(
    /* [in] */ IView* v)
{
    VALIDATE_NOT_NULL(v);
    // ==================before translated======================
    // if(v == mOk) {
    //     if (mOkCanInstall || mScrollView == null) {
    //         mInstallFlowAnalytics.setInstallButtonClicked();
    //         if (mSessionId != -1) {
    //             mInstaller.setPermissionsResult(mSessionId, true);
    //
    //             // We're only confirming permissions, so we don't really know how the
    //             // story ends; assume success.
    //             mInstallFlowAnalytics.setFlowFinishedWithPackageManagerResult(
    //                     PackageManager.INSTALL_SUCCEEDED);
    //         } else {
    //             // Start subactivity to actually install the application
    //             Intent newIntent = new Intent();
    //             newIntent.putExtra(PackageUtil.INTENT_ATTR_APPLICATION_INFO,
    //                     mPkgInfo.applicationInfo);
    //             newIntent.setData(mPackageURI);
    //             newIntent.setClass(this, InstallAppProgress.class);
    //             newIntent.putExtra(InstallAppProgress.EXTRA_MANIFEST_DIGEST, mPkgDigest);
    //             newIntent.putExtra(
    //                     InstallAppProgress.EXTRA_INSTALL_FLOW_ANALYTICS, mInstallFlowAnalytics);
    //             String installerPackageName = getIntent().getStringExtra(
    //                     Intent.EXTRA_INSTALLER_PACKAGE_NAME);
    //             if (mOriginatingURI != null) {
    //                 newIntent.putExtra(Intent.EXTRA_ORIGINATING_URI, mOriginatingURI);
    //             }
    //             if (mReferrerURI != null) {
    //                 newIntent.putExtra(Intent.EXTRA_REFERRER, mReferrerURI);
    //             }
    //             if (mOriginatingUid != VerificationParams.NO_UID) {
    //                 newIntent.putExtra(Intent.EXTRA_ORIGINATING_UID, mOriginatingUid);
    //             }
    //             if (installerPackageName != null) {
    //                 newIntent.putExtra(Intent.EXTRA_INSTALLER_PACKAGE_NAME,
    //                         installerPackageName);
    //             }
    //             if (getIntent().getBooleanExtra(Intent.EXTRA_RETURN_RESULT, false)) {
    //                 newIntent.putExtra(Intent.EXTRA_RETURN_RESULT, true);
    //                 newIntent.addFlags(Intent.FLAG_ACTIVITY_FORWARD_RESULT);
    //             }
    //             if(localLOGV) Log.i(TAG, "downloaded app uri="+mPackageURI);
    //             startActivity(newIntent);
    //         }
    //         finish();
    //     } else {
    //         mScrollView.pageScroll(View.FOCUS_DOWN);
    //     }
    // } else if(v == mCancel) {
    //     // Cancel and finish
    //     setResult(RESULT_CANCELED);
    //     if (mSessionId != -1) {
    //         mInstaller.setPermissionsResult(mSessionId, false);
    //     }
    //     mInstallFlowAnalytics.setFlowFinished(
    //             InstallFlowAnalytics.RESULT_CANCELLED_BY_USER);
    //     finish();
    // }

    if(TO_IINTERFACE(v) == TO_IINTERFACE(mOk)) {
        if (mOkCanInstall /*|| mScrollView == NULL*/) {
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
                assert(0);
                //--: need ClassIDs
                //newIntent->SetClass(this, InstallAppProgress.class);
                newIntent->PutExtra(IInstallAppProgress::EXTRA_MANIFEST_DIGEST, IParcelable::Probe(mPkgDigest));
                newIntent->PutExtra(IInstallAppProgress::EXTRA_INSTALL_FLOW_ANALYTICS, mInstallFlowAnalytics);

                AutoPtr<IIntent> intent;
                //--: GetIntent((IIntent**)&intent);
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
                    newIntent->PutExtra(IIntent::EXTRA_RETURN_RESULT, TRUE);
                    newIntent->AddFlags(IIntent::FLAG_ACTIVITY_FORWARD_RESULT);
                }
                if(localLOGV) {
                    String sUri;
                    mPackageURI->ToSafeString(&sUri);
                    Logger::I(TAG, String("downloaded app uri=") + sUri);
                }
                //--: StartActivity(newIntent);
            }
            //--: Finish();
        }
        else {
            //mScrollView->PageScroll(IView::FOCUS_DOWN);
        }
    }
    else if(TO_IINTERFACE(v) == TO_IINTERFACE(mCancel)) {
        // Cancel and finish
        //--: SetResult(RESULT_CANCELED);
        if (mSessionId != -1) {
            mInstaller->SetPermissionsResult(mSessionId, FALSE);
        }
        mInstallFlowAnalytics->SetFlowFinished(InstallFlowAnalytics::RESULT_CANCELLED_BY_USER);
        //--: Finish();
    }
    return NOERROR;
}

ECode CPackageInstallerActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    // ==================before translated======================
    // super.onCreate(icicle);
    //
    // mPm = getPackageManager();
    // mInstaller = mPm.getPackageInstaller();
    //
    // final Intent intent = getIntent();
    // if (PackageInstaller.ACTION_CONFIRM_PERMISSIONS.equals(intent.getAction())) {
    //     final int sessionId = intent.getIntExtra(PackageInstaller.EXTRA_SESSION_ID, -1);
    //     final PackageInstaller.SessionInfo info = mInstaller.getSessionInfo(sessionId);
    //     if (info == null || !info.sealed || info.resolvedBaseCodePath == null) {
    //         Log.w(TAG, "Session " + mSessionId + " in funky state; ignoring");
    //         finish();
    //         return;
    //     }
    //
    //     mSessionId = sessionId;
    //     mPackageURI = Uri.fromFile(new File(info.resolvedBaseCodePath));
    //     mOriginatingURI = null;
    //     mReferrerURI = null;
    // } else {
    //     mSessionId = -1;
    //     mPackageURI = intent.getData();
    //     mOriginatingURI = intent.getParcelableExtra(Intent.EXTRA_ORIGINATING_URI);
    //     mReferrerURI = intent.getParcelableExtra(Intent.EXTRA_REFERRER);
    // }
    //
    // boolean requestFromUnknownSource = isInstallRequestFromUnknownSource(intent);
    // mInstallFlowAnalytics = new InstallFlowAnalytics();
    // mInstallFlowAnalytics.setContext(this);
    // mInstallFlowAnalytics.setStartTimestampMillis(SystemClock.elapsedRealtime());
    // mInstallFlowAnalytics.setInstallsFromUnknownSourcesPermitted(
    //         isInstallingUnknownAppsAllowed());
    // mInstallFlowAnalytics.setInstallRequestFromUnknownSource(requestFromUnknownSource);
    // mInstallFlowAnalytics.setVerifyAppsEnabled(isVerifyAppsEnabled());
    // mInstallFlowAnalytics.setAppVerifierInstalled(isAppVerifierInstalled());
    // mInstallFlowAnalytics.setPackageUri(mPackageURI.toString());
    //
    // final String scheme = mPackageURI.getScheme();
    // if (scheme != null && !"file".equals(scheme) && !"package".equals(scheme)) {
    //     Log.w(TAG, "Unsupported scheme " + scheme);
    //     setPmResult(PackageManager.INSTALL_FAILED_INVALID_URI);
    //     mInstallFlowAnalytics.setFlowFinished(
    //             InstallFlowAnalytics.RESULT_FAILED_UNSUPPORTED_SCHEME);
    //     finish();
    //     return;
    // }
    //
    // final PackageUtil.AppSnippet as;
    // if ("package".equals(mPackageURI.getScheme())) {
    //     mInstallFlowAnalytics.setFileUri(false);
    //     try {
    //         mPkgInfo = mPm.getPackageInfo(mPackageURI.getSchemeSpecificPart(),
    //                 PackageManager.GET_PERMISSIONS | PackageManager.GET_UNINSTALLED_PACKAGES);
    //     } catch (NameNotFoundException e) {
    //     }
    //     if (mPkgInfo == null) {
    //         Log.w(TAG, "Requested package " + mPackageURI.getScheme()
    //                 + " not available. Discontinuing installation");
    //         showDialogInner(DLG_PACKAGE_ERROR);
    //         setPmResult(PackageManager.INSTALL_FAILED_INVALID_APK);
    //         mInstallFlowAnalytics.setPackageInfoObtained();
    //         mInstallFlowAnalytics.setFlowFinished(
    //                 InstallFlowAnalytics.RESULT_FAILED_PACKAGE_MISSING);
    //         return;
    //     }
    //     as = new PackageUtil.AppSnippet(mPm.getApplicationLabel(mPkgInfo.applicationInfo),
    //             mPm.getApplicationIcon(mPkgInfo.applicationInfo));
    // } else {
    //     mInstallFlowAnalytics.setFileUri(true);
    //     final File sourceFile = new File(mPackageURI.getPath());
    //     PackageParser.Package parsed = PackageUtil.getPackageInfo(sourceFile);
    //
    //     // Check for parse errors
    //     if (parsed == null) {
    //         Log.w(TAG, "Parse error when parsing manifest. Discontinuing installation");
    //         showDialogInner(DLG_PACKAGE_ERROR);
    //         setPmResult(PackageManager.INSTALL_FAILED_INVALID_APK);
    //         mInstallFlowAnalytics.setPackageInfoObtained();
    //         mInstallFlowAnalytics.setFlowFinished(
    //                 InstallFlowAnalytics.RESULT_FAILED_TO_GET_PACKAGE_INFO);
    //         return;
    //     }
    //     mPkgInfo = PackageParser.generatePackageInfo(parsed, null,
    //             PackageManager.GET_PERMISSIONS, 0, 0, null,
    //             new PackageUserState());
    //     mPkgDigest = parsed.manifestDigest;
    //     as = PackageUtil.getAppSnippet(this, mPkgInfo.applicationInfo, sourceFile);
    // }
    // mInstallFlowAnalytics.setPackageInfoObtained();
    //
    // //set view
    // setContentView(R.layout.install_start);
    // mInstallConfirm = findViewById(R.id.install_confirm_panel);
    // mInstallConfirm.setVisibility(View.INVISIBLE);
    // PackageUtil.initSnippetForNewApp(this, as, R.id.app_snippet);
    //
    // mOriginatingUid = getOriginatingUid(intent);
    //
    // // Block the install attempt on the Unknown Sources setting if necessary.
    // if ((requestFromUnknownSource) && (!isInstallingUnknownAppsAllowed())) {
    //     //ask user to enable setting first
    //     showDialogInner(DLG_UNKNOWN_APPS);
    //     mInstallFlowAnalytics.setFlowFinished(
    //             InstallFlowAnalytics.RESULT_BLOCKED_BY_UNKNOWN_SOURCES_SETTING);
    //     return;
    // }
    // initiateInstall();

    //--: Activity::OnCreate(icicle);

    //--: GetPackageManager((IPackageManager**)&mPm);
    mPm->GetPackageInstaller((IPackageInstaller**)&mInstaller);

    AutoPtr<IIntent> intent;
    //--: GetIntent((IIntent**)&intent);
    String action;
    intent->GetAction(&action);
    if (IPackageInstaller::ACTION_CONFIRM_PERMISSIONS.Equals(action)) {
        Int32 sessionId = 0;
        intent->GetInt32Extra(IPackageInstaller::EXTRA_SESSION_ID, -1, &sessionId);
        AutoPtr<IPackageInstallerSessionInfo> info;
        mInstaller->GetSessionInfo(sessionId, (IPackageInstallerSessionInfo**)&info);

        assert(0);
        //--: member is private but it is public in java.
        if (info == NULL || FALSE/*!info.sealed*/ || FALSE/*info->resolvedBaseCodePath == NULL*/) {
            Logger::W(TAG, "Session %d in funky state; ignoring", mSessionId);
            //Finish();
            return NOERROR;
        }

        mSessionId = sessionId;
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);

        assert(0);
        //--: member is private but it is public in java.
        AutoPtr<IFile> file;
        CFile::New(String("")/*info->resolvedBaseCodePath*/, (IFile**)&file);
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
    mInstallFlowAnalytics = new InstallFlowAnalytics();
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
    if (scheme != String("") && !String("file").Equals(scheme) && !String("package").Equals(scheme)) {
        Logger::W(TAG, String("Unsupported scheme ") + scheme);
        SetPmResult(IPackageManager::INSTALL_FAILED_INVALID_URI);
        mInstallFlowAnalytics->SetFlowFinished(InstallFlowAnalytics::RESULT_FAILED_UNSUPPORTED_SCHEME);
        //Finish();
        return NOERROR;
    }

    AutoPtr<PackageUtil::AppSnippet> as;
    mPackageURI->GetScheme(&scheme);
    if (String("package").Equals(scheme)) {
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
            Logger::W(TAG, String("Requested package ") + scheme + String(" not available. Discontinuing installation"));
            ShowDialogInner(DLG_PACKAGE_ERROR);
            SetPmResult(IPackageManager::INSTALL_FAILED_INVALID_APK);
            mInstallFlowAnalytics->SetPackageInfoObtained();
            mInstallFlowAnalytics->SetFlowFinished(InstallFlowAnalytics::RESULT_FAILED_PACKAGE_MISSING);
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
        //AutoPtr<PackageParser::Package> parsed;// = PackageUtil::GetPackageInfo(sourceFile);

        // Check for parse errors
        if (/*parsed == */NULL) {
            Logger::W(TAG, "Parse error when parsing manifest. Discontinuing installation");
            ShowDialogInner(DLG_PACKAGE_ERROR);
            SetPmResult(IPackageManager::INSTALL_FAILED_INVALID_APK);
            mInstallFlowAnalytics->SetPackageInfoObtained();
            mInstallFlowAnalytics->SetFlowFinished(InstallFlowAnalytics::RESULT_FAILED_TO_GET_PACKAGE_INFO);
            return NOERROR;
        }

        //AutoPtr<PackageUserState> userState = new PackageUserState();
        //mPkgInfo = PackageParser::GeneratePackageInfo(parsed, NULL, IPackageManager::GET_PERMISSIONS, 0, 0, NULL, userState);
        //mPkgDigest = parsed->manifestDigest;
        AutoPtr<IApplicationInfo> applicationInfo;
        mPkgInfo->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        as = PackageUtil::GetAppSnippet(IActivity::Probe(this), applicationInfo, sourceFile);
    }
    mInstallFlowAnalytics->SetPackageInfoObtained();

    //set view
    //SetContentView(-1/*R::layout::install_start*/);
    //--: FindViewById(R::id::install_confirm_panel, (IView**)&mInstallConfirm);
    mInstallConfirm->SetVisibility(IView::INVISIBLE);
    PackageUtil::InitSnippetForNewApp(IActivity::Probe(this), as, -1/*R::id::app_snippet*/);

    mOriginatingUid = GetOriginatingUid(intent);

    // Block the install attempt on the Unknown Sources setting if necessary.
    if ((requestFromUnknownSource) && (!IsInstallingUnknownAppsAllowed())) {
        //ask user to enable setting first
        ShowDialogInner(DLG_UNKNOWN_APPS);
        mInstallFlowAnalytics->SetFlowFinished(InstallFlowAnalytics::RESULT_BLOCKED_BY_UNKNOWN_SOURCES_SETTING);
        return NOERROR;
    }
    InitiateInstall();
    return NOERROR;
}

void CPackageInstallerActivity::StartInstallConfirm()
{
    // ==================before translated======================
    // TabHost tabHost = (TabHost)findViewById(android.R.id.tabhost);
    // tabHost.setup();
    // ViewPager viewPager = (ViewPager)findViewById(R.id.pager);
    // TabsAdapter adapter = new TabsAdapter(this, tabHost, viewPager);
    // adapter.setOnTabChangedListener(new TabHost.OnTabChangeListener() {
    //     @Override
    //     public void onTabChanged(String tabId) {
    //         if (TAB_ID_ALL.equals(tabId)) {
    //             mInstallFlowAnalytics.setAllPermissionsDisplayed(true);
    //         } else if (TAB_ID_NEW.equals(tabId)) {
    //             mInstallFlowAnalytics.setNewPermissionsDisplayed(true);
    //         }
    //     }
    // });
    //
    // boolean permVisible = false;
    // mScrollView = null;
    // mOkCanInstall = false;
    // int msg = 0;
    // if (mPkgInfo != null) {
    //     AppSecurityPermissions perms = new AppSecurityPermissions(this, mPkgInfo);
    //     final int NP = perms.getPermissionCount(AppSecurityPermissions.WHICH_PERSONAL);
    //     final int ND = perms.getPermissionCount(AppSecurityPermissions.WHICH_DEVICE);
    //     if (mAppInfo != null) {
    //         msg = (mAppInfo.flags & ApplicationInfo.FLAG_SYSTEM) != 0
    //                 ? R.string.install_confirm_question_update_system
    //                 : R.string.install_confirm_question_update;
    //         mScrollView = new CaffeinatedScrollView(this);
    //         mScrollView.setFillViewport(true);
    //         boolean newPermissionsFound =
    //                 (perms.getPermissionCount(AppSecurityPermissions.WHICH_NEW) > 0);
    //         mInstallFlowAnalytics.setNewPermissionsFound(newPermissionsFound);
    //         if (newPermissionsFound) {
    //             permVisible = true;
    //             mScrollView.addView(perms.getPermissionsView(
    //                     AppSecurityPermissions.WHICH_NEW));
    //         } else {
    //             LayoutInflater inflater = (LayoutInflater)getSystemService(
    //                     Context.LAYOUT_INFLATER_SERVICE);
    //             TextView label = (TextView)inflater.inflate(R.layout.label, null);
    //             label.setText(R.string.no_new_perms);
    //             mScrollView.addView(label);
    //         }
    //         adapter.addTab(tabHost.newTabSpec(TAB_ID_NEW).setIndicator(
    //                 getText(R.string.newPerms)), mScrollView);
    //     } else  {
    //         findViewById(R.id.tabscontainer).setVisibility(View.GONE);
    //         findViewById(R.id.divider).setVisibility(View.VISIBLE);
    //     }
    //     if (NP > 0 || ND > 0) {
    //         permVisible = true;
    //         LayoutInflater inflater = (LayoutInflater)getSystemService(
    //                 Context.LAYOUT_INFLATER_SERVICE);
    //         View root = inflater.inflate(R.layout.permissions_list, null);
    //         if (mScrollView == null) {
    //             mScrollView = (CaffeinatedScrollView)root.findViewById(R.id.scrollview);
    //         }
    //         if (NP > 0) {
    //             ((ViewGroup)root.findViewById(R.id.privacylist)).addView(
    //                     perms.getPermissionsView(AppSecurityPermissions.WHICH_PERSONAL));
    //         } else {
    //             root.findViewById(R.id.privacylist).setVisibility(View.GONE);
    //         }
    //         if (ND > 0) {
    //             ((ViewGroup)root.findViewById(R.id.devicelist)).addView(
    //                     perms.getPermissionsView(AppSecurityPermissions.WHICH_DEVICE));
    //         } else {
    //             root.findViewById(R.id.devicelist).setVisibility(View.GONE);
    //         }
    //         adapter.addTab(tabHost.newTabSpec(TAB_ID_ALL).setIndicator(
    //                 getText(R.string.allPerms)), root);
    //     }
    // }
    // mInstallFlowAnalytics.setPermissionsDisplayed(permVisible);
    // if (!permVisible) {
    //     if (mAppInfo != null) {
    //         // This is an update to an application, but there are no
    //         // permissions at all.
    //         msg = (mAppInfo.flags & ApplicationInfo.FLAG_SYSTEM) != 0
    //                 ? R.string.install_confirm_question_update_system_no_perms
    //                 : R.string.install_confirm_question_update_no_perms;
    //     } else {
    //         // This is a new application with no permissions.
    //         msg = R.string.install_confirm_question_no_perms;
    //     }
    //     tabHost.setVisibility(View.GONE);
    //     mInstallFlowAnalytics.setAllPermissionsDisplayed(false);
    //     mInstallFlowAnalytics.setNewPermissionsDisplayed(false);
    //     findViewById(R.id.filler).setVisibility(View.VISIBLE);
    //     findViewById(R.id.divider).setVisibility(View.GONE);
    //     mScrollView = null;
    // }
    // if (msg != 0) {
    //     ((TextView)findViewById(R.id.install_confirm_question)).setText(msg);
    // }
    // mInstallConfirm.setVisibility(View.VISIBLE);
    // mOk = (Button)findViewById(R.id.ok_button);
    // mCancel = (Button)findViewById(R.id.cancel_button);
    // mOk.setOnClickListener(this);
    // mCancel.setOnClickListener(this);
    // if (mScrollView == null) {
    //     // There is nothing to scroll view, so the ok button is immediately
    //     // set to install.
    //     mOk.setText(R.string.install);
    //     mOkCanInstall = true;
    // } else {
    //     mScrollView.setFullScrollAction(new Runnable() {
    //         @Override
    //         public void run() {
    //             mOk.setText(R.string.install);
    //             mOkCanInstall = true;
    //         }
    //     });
    // }

    AutoPtr<IView> viewTmp;
    //--: FindViewById(R::id::tabhost, (IView**)&viewTmp);
    ITabHost* tabHost = ITabHost::Probe(viewTmp);
    tabHost->Setup();

    viewTmp = NULL;
    //--: FindViewById(R::id::pager, (IView**)&viewTmp);
    assert(0);
    //--: android.support.v4.view.ViewPager
    //IViewPager* viewPager = IViewPager::Probe(viewTmp);

    AutoPtr<TabsAdapter> adapter = new TabsAdapter(IActivity::Probe(this), tabHost, NULL/*viewPager*/);
    AutoPtr<ITabHostOnTabChangeListener> listener = new InnerTabHostOnTabChangeListener(this);
    adapter->SetOnTabChangedListener(listener);

    Boolean permVisible = FALSE;
    //mScrollView = NULL;
    mOkCanInstall = FALSE;
    Int32 msg = 0;
    if (mPkgInfo != NULL) {
        //--: AutoPtr<AppSecurityPermissions> perms = new AppSecurityPermissions(this, mPkgInfo);
        Int32 NP = 0;//perms->GetPermissionCount(AppSecurityPermissions::WHICH_PERSONAL);
        Int32 ND = 0;//perms->GetPermissionCount(AppSecurityPermissions::WHICH_DEVICE);
        if (mAppInfo != NULL) {
            Int32 flags = 0;
            mAppInfo->GetFlags(&flags);
            msg = (flags & IApplicationInfo::FLAG_SYSTEM) != 0 ? -1/*R::string::install_confirm_question_update_system*/
                    : -1/*R::string::install_confirm_question_update*/;

            //mScrollView = new CaffeinatedScrollView(this);
            //IView::Probe(mScrollView)->SetFillViewport(TRUE);
            Boolean newPermissionsFound = FALSE;//(perms->GetPermissionCount(AppSecurityPermissions::WHICH_NEW) > 0);
            mInstallFlowAnalytics->SetNewPermissionsFound(newPermissionsFound);
            if (newPermissionsFound) {
                permVisible = TRUE;
                //IViewGroup::Probe(mScrollView)->AddView(perms->GetPermissionsView(
                //        AppSecurityPermissions::WHICH_NEW));
            }
            else {
                AutoPtr<IInterface> interfaceTmp;
                //--: GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&interfaceTmp);
                ILayoutInflater* inflater = ILayoutInflater::Probe(interfaceTmp);

                viewTmp = NULL;
                inflater->Inflate(-1/*R::layout::label*/, NULL, (IView**)&viewTmp);
                ITextView* label = ITextView::Probe(viewTmp);

                AutoPtr<ICharSequence> charSequenceTmp;
                CString::New(String("")/*R::string::no_new_perms*/, (ICharSequence**)&charSequenceTmp);
                label->SetText(charSequenceTmp);
                //IViewGroup::Probe(mScrollView)->AddView(IView::Probe(label));
            }

            AutoPtr<ITabSpec> tabSpec;
            tabHost->NewTabSpec(TAB_ID_NEW, (ITabSpec**)&tabSpec);

            AutoPtr<ICharSequence> charSequenceTmp;
            //--: GetText(R::string::newPerms, (ICharSequence**)&charSequenceTmp);
            tabSpec->SetIndicator(charSequenceTmp);
            //adapter->AddTab(tabSpec, mScrollView);
        }
        else  {
            viewTmp = NULL;
            //--: FindViewById(R::id::tabscontainer, (IView**)&viewTmp);
            viewTmp->SetVisibility(IView::GONE);

            viewTmp = NULL;
            //--: FindViewById(R::id::divider, (IView**)&viewTmp);
            viewTmp->SetVisibility(IView::VISIBLE);
        }
        if (NP > 0 || ND > 0) {
            permVisible = TRUE;
            AutoPtr<IInterface> interfaceTmp;
            //--: GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&interfaceTmp);
            ILayoutInflater* inflater = ILayoutInflater::Probe(interfaceTmp);

            AutoPtr<IView> root;
            inflater->Inflate(-1/*R::layout::permissions_list*/, NULL, (IView**)&root);
            if (/*mScrollView ==*/ NULL) {
                viewTmp = NULL;
                //--: root->FindViewById(R::id::scrollview, (IView**)&viewTmp);
                //mScrollView = (CaffeinatedScrollView*)viewTmp.Get();
            }
            if (NP > 0) {
                viewTmp = NULL;
                //--: root->FindViewById(R::id::privacylist, (IView**)&viewTmp);
                //IViewGroup::Probe(viewTmp)->AddView(perms->GetPermissionsView(AppSecurityPermissions::WHICH_PERSONAL));
            }
            else {
                viewTmp = NULL;
                //--: root->FindViewById(R::id::privacylist, (IView**)&viewTmp);
                viewTmp->SetVisibility(IView::GONE);
            }
            if (ND > 0) {
                viewTmp = NULL;
                //--: root->FindViewById(R::id::devicelist, (IView**)&viewTmp);
                //IViewGroup::Probe(viewTmp)->AddView(perms->GetPermissionsView(AppSecurityPermissions::WHICH_DEVICE));
            }
            else {
                viewTmp = NULL;
                //--: root->FindViewById(R::id::devicelist, (IView**)&viewTmp);
                viewTmp->SetVisibility(IView::GONE);
            }

            AutoPtr<ITabSpec> tabSpec;
            tabHost->NewTabSpec(TAB_ID_ALL, (ITabSpec**)&tabSpec);

            AutoPtr<ICharSequence> charSequenceTmp;
            //--: GetText(R::string::allPerms, (ICharSequence**)&charSequenceTmp);
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
            msg = (flags & IApplicationInfo::FLAG_SYSTEM) != 0 ? -1/*R::string::install_confirm_question_update_system_no_perms*/
                    : -1/*R::string::install_confirm_question_update_no_perms*/;
        }
        else {
            // This is a new application with no permissions.
            //msg = R::string::install_confirm_question_no_perms;
        }
        IView::Probe(tabHost)->SetVisibility(IView::GONE);
        mInstallFlowAnalytics->SetAllPermissionsDisplayed(FALSE);
        mInstallFlowAnalytics->SetNewPermissionsDisplayed(FALSE);

        viewTmp = NULL;
        //--: FindViewById(R::id::filler, (IView**)&viewTmp);
        viewTmp->SetVisibility(IView::VISIBLE);

        viewTmp = NULL;
        //--: FindViewById(R::id::divider, (IView**)&viewTmp);
        viewTmp->SetVisibility(IView::GONE);

        //mScrollView = NULL;
    }
    if (msg != 0) {
        viewTmp = NULL;
        //--: FindViewById(R::id::install_confirm_question, (IView**)&viewTmp);

        AutoPtr<ICharSequence> charSequenceTmp;
        CString::New(StringUtils::ToString(msg), (ICharSequence**)&charSequenceTmp);
        ITextView::Probe(viewTmp)->SetText(charSequenceTmp);
    }
    mInstallConfirm->SetVisibility(IView::VISIBLE);

    viewTmp = NULL;
    //--: FindViewById(R::id::ok_button, (IView**)&viewTmp);
    mOk = IButton::Probe(viewTmp);

    viewTmp = NULL;
    //--: FindViewById(R::id::cancel_button, (IView**)&viewTmp);
    mCancel = IButton::Probe(viewTmp);

    IView::Probe(mOk)->SetOnClickListener(IViewOnClickListener::Probe(this));
    IView::Probe(mCancel)->SetOnClickListener(IViewOnClickListener::Probe(this));
    if (/*mScrollView ==*/ NULL) {
        // There is nothing to scroll view, so the ok button is immediately
        // set to install.
        AutoPtr<ICharSequence> charSequenceTmp;
        CString::New(String("")/*R::string::install*/, (ICharSequence**)&charSequenceTmp);
        ITextView::Probe(mOk)->SetText(charSequenceTmp);
        mOkCanInstall = TRUE;
    }
    else {
        AutoPtr<IRunnable> runnable = new InnerRunnable(this);
        //mScrollView->SetFullScrollAction(runnable);
    }
}

void CPackageInstallerActivity::ShowDialogInner(
    /* [in] */ Int32 id)
{
    // ==================before translated======================
    // // TODO better fix for this? Remove dialog so that it gets created again
    // removeDialog(id);
    // showDialog(id);

    //--: RemoveDialog(id);
    //--: ShowDialog(id);
}

void CPackageInstallerActivity::LaunchSettingsAppAndFinish()
{
    // ==================before translated======================
    // // Create an intent to launch SettingsTwo activity
    // Intent launchSettingsIntent = new Intent(Settings.ACTION_SECURITY_SETTINGS);
    // launchSettingsIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
    // startActivity(launchSettingsIntent);
    // finish();

    AutoPtr<IIntent> launchSettingsIntent;
    CIntent::New(ISettings::ACTION_SECURITY_SETTINGS, (IIntent**)&launchSettingsIntent);
    launchSettingsIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    //--: StartActivity(launchSettingsIntent);
    //--: Finish();
}

Boolean CPackageInstallerActivity::IsInstallingUnknownAppsAllowed()
{
    // ==================before translated======================
    // UserManager um = (UserManager) getSystemService(USER_SERVICE);
    //
    // boolean disallowedByUserManager = um.getUserRestrictions()
    //         .getBoolean(UserManager.DISALLOW_INSTALL_UNKNOWN_SOURCES, false);
    // boolean allowedBySecureSettings = Settings.Secure.getInt(getContentResolver(),
    //     Settings.Secure.INSTALL_NON_MARKET_APPS, 0) > 0;
    //
    // return (allowedBySecureSettings && (!disallowedByUserManager));

    AutoPtr<IInterface> interfaceTmp;
    //--: GetSystemService(USER_SERVICE, (IInterface**)&interfaceTmp);
    IUserManager* um = IUserManager::Probe(interfaceTmp);

    AutoPtr<IBundle> bundle;
    um->GetUserRestrictions((IBundle**)&bundle);
    Boolean disallowedByUserManager;
    bundle->GetBoolean(IUserManager::DISALLOW_INSTALL_UNKNOWN_SOURCES, FALSE, &disallowedByUserManager);

    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);

    AutoPtr<IContentResolver> contentResolver;
    //--: GetContentResolver((IContentResolver**)&contentResolver);
    Int32 val = 0;
    settingsSecure->GetInt32(contentResolver, ISettingsSecure::INSTALL_NON_MARKET_APPS, 0, &val);
    Boolean allowedBySecureSettings = val > 0;
    return (allowedBySecureSettings && (!disallowedByUserManager));
}

Boolean CPackageInstallerActivity::IsInstallRequestFromUnknownSource(
    /* [in] */ IIntent* intent)
{
    // ==================before translated======================
    // String callerPackage = getCallingPackage();
    // if (callerPackage != null && intent.getBooleanExtra(
    //         Intent.EXTRA_NOT_UNKNOWN_SOURCE, false)) {
    //     try {
    //         mSourceInfo = mPm.getApplicationInfo(callerPackage, 0);
    //         if (mSourceInfo != null) {
    //             if ((mSourceInfo.flags & ApplicationInfo.FLAG_PRIVILEGED) != 0) {
    //                 // Privileged apps are not considered an unknown source.
    //                 return false;
    //             }
    //         }
    //     } catch (NameNotFoundException e) {
    //     }
    // }
    //
    // return true;

    String callerPackage;
    //--: GetCallingPackage(&callerPackage);
    Boolean boolExtra = FALSE;
    intent->GetBooleanExtra(IIntent::EXTRA_NOT_UNKNOWN_SOURCE, FALSE, &boolExtra);
    if (callerPackage != "" && boolExtra) {
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
    // ==================before translated======================
    // UserManager um = (UserManager) getSystemService(USER_SERVICE);
    // if (um.hasUserRestriction(UserManager.ENSURE_VERIFY_APPS)) {
    //     return true;
    // }
    // return Settings.Global.getInt(getContentResolver(),
    //         Settings.Global.PACKAGE_VERIFIER_ENABLE, 1) > 0;

    AutoPtr<IInterface> interfaceTmp;
    //--: GetSystemService(USER_SERVICE, (IInterface**)&interfaceTmp);
    IUserManager* um = IUserManager::Probe(interfaceTmp);
    Boolean has = FALSE;
    um->HasUserRestriction(IUserManager::ENSURE_VERIFY_APPS, &has);
    if (has) {
        return TRUE;
    }

    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    AutoPtr<IContentResolver> contentResolver;
    //--: GetContentResolver((IContentResolver**)&contentResolver);
    Int32 val = 0;
    settingsGlobal->GetInt32(contentResolver, ISettingsGlobal::PACKAGE_VERIFIER_ENABLE, 1, &val);
    return val > 0;
}

Boolean CPackageInstallerActivity::IsAppVerifierInstalled()
{
    // ==================before translated======================
    // final PackageManager pm = getPackageManager();
    // final Intent verification = new Intent(Intent.ACTION_PACKAGE_NEEDS_VERIFICATION);
    // verification.setType(PACKAGE_MIME_TYPE);
    // verification.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
    // final List<ResolveInfo> receivers = pm.queryBroadcastReceivers(verification, 0);
    // return (receivers.size() > 0) ? true : false;

    AutoPtr<IPackageManager> pm;
    //--: GetPackageManager((IPackageManager**)&pm);
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
    // ==================before translated======================
    // String pkgName = mPkgInfo.packageName;
    // // Check if there is already a package on the device with this name
    // // but it has been renamed to something else.
    // String[] oldName = mPm.canonicalToCurrentPackageNames(new String[] { pkgName });
    // if (oldName != null && oldName.length > 0 && oldName[0] != null) {
    //     pkgName = oldName[0];
    //     mPkgInfo.packageName = pkgName;
    //     mPkgInfo.applicationInfo.packageName = pkgName;
    // }
    // // Check if package is already installed. display confirmation dialog if replacing pkg
    // try {
    //     // This is a little convoluted because we want to get all uninstalled
    //     // apps, but this may include apps with just data, and if it is just
    //     // data we still want to count it as "installed".
    //     mAppInfo = mPm.getApplicationInfo(pkgName,
    //             PackageManager.GET_UNINSTALLED_PACKAGES);
    //     if ((mAppInfo.flags&ApplicationInfo.FLAG_INSTALLED) == 0) {
    //         mAppInfo = null;
    //     }
    // } catch (NameNotFoundException e) {
    //     mAppInfo = null;
    // }
    //
    // mInstallFlowAnalytics.setReplace(mAppInfo != null);
    // mInstallFlowAnalytics.setSystemApp(
    //         (mAppInfo != null) && ((mAppInfo.flags & ApplicationInfo.FLAG_SYSTEM) != 0));
    //
    // startInstallConfirm();

    String pkgName;
    mPkgInfo->GetPackageName(&pkgName);
    // Check if there is already a package on the device with this name
    // but it has been renamed to something else.
    AutoPtr< ArrayOf<String> > packageNames = ArrayOf<String>::Alloc(1);
    packageNames->Set(0, pkgName);

    AutoPtr< ArrayOf<String> > oldName;
    mPm->CanonicalToCurrentPackageNames(*packageNames, (ArrayOf<String>**)&oldName);
    if (oldName != NULL && oldName->GetLength() > 0 && (*oldName)[0] != "") {
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
        mPm->GetApplicationInfo(pkgName, IPackageManager::GET_UNINSTALLED_PACKAGES, (IApplicationInfo**)&mAppInfo);
        Int32 flags = 0;
        mAppInfo->GetFlags(&flags);
        if ((flags & IApplicationInfo::FLAG_INSTALLED) == 0) {
            mAppInfo = NULL;
        }
    //} catch (NameNotFoundException e) {
        //mAppInfo = null;
    //}

    mInstallFlowAnalytics->SetReplace(mAppInfo != NULL);
    flags = 0;
    mAppInfo->GetFlags(&flags);
    mInstallFlowAnalytics->SetSystemApp((mAppInfo != NULL) && ((flags & IApplicationInfo::FLAG_SYSTEM) != 0));
    StartInstallConfirm();
}

AutoPtr<IApplicationInfo> CPackageInstallerActivity::GetSourceInfo()
{
    // ==================before translated======================
    // String callingPackage = getCallingPackage();
    // if (callingPackage != null) {
    //     try {
    //         return mPm.getApplicationInfo(callingPackage, 0);
    //     } catch (NameNotFoundException ex) {
    //         // ignore
    //     }
    // }
    // return null;

    String callingPackage;
    //--: GetCallingPackage(&callingPackage);
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
    // ==================before translated======================
    // // The originating uid from the intent. We only trust/use this if it comes from a
    // // system application
    // int uidFromIntent = intent.getIntExtra(Intent.EXTRA_ORIGINATING_UID,
    //         VerificationParams.NO_UID);
    //
    // // Get the source info from the calling package, if available. This will be the
    // // definitive calling package, but it only works if the intent was started using
    // // startActivityForResult,
    // ApplicationInfo sourceInfo = getSourceInfo();
    // if (sourceInfo != null) {
    //     if (uidFromIntent != VerificationParams.NO_UID &&
    //             (mSourceInfo.flags & ApplicationInfo.FLAG_PRIVILEGED) != 0) {
    //         return uidFromIntent;
    //
    //     }
    //     // We either didn't get a uid in the intent, or we don't trust it. Use the
    //     // uid of the calling package instead.
    //     return sourceInfo.uid;
    // }
    //
    // // We couldn't get the specific calling package. Let's get the uid instead
    // int callingUid;
    // try {
    //     callingUid = ActivityManagerNative.getDefault()
    //             .getLaunchedFromUid(getActivityToken());
    // } catch (android.os.RemoteException ex) {
    //     Log.w(TAG, "Could not determine the launching uid.");
    //     // nothing else we can do
    //     return VerificationParams.NO_UID;
    // }
    //
    // // If we got a uid from the intent, we need to verify that the caller is a
    // // privileged system package before we use it
    // if (uidFromIntent != VerificationParams.NO_UID) {
    //     String[] callingPackages = mPm.getPackagesForUid(callingUid);
    //     if (callingPackages != null) {
    //         for (String packageName: callingPackages) {
    //             try {
    //                 ApplicationInfo applicationInfo =
    //                         mPm.getApplicationInfo(packageName, 0);
    //
    //                 if ((applicationInfo.flags & ApplicationInfo.FLAG_PRIVILEGED) != 0) {
    //                     return uidFromIntent;
    //                 }
    //             } catch (NameNotFoundException ex) {
    //                 // ignore it, and try the next package
    //             }
    //         }
    //     }
    // }
    // // We either didn't get a uid from the intent, or we don't trust it. Use the
    // // calling uid instead.
    // return callingUid;

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
        //--: GetActivityToken((IBinder**)&binder);
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
} // namespace Apps
} // namespace Elastos


