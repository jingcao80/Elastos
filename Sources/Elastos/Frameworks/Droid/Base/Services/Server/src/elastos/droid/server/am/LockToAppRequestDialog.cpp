#include "elastos/droid/server/am/LockToAppRequestDialog.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/provider/Settings.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/R.h"

#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Internal::Policy::CPolicyManager;
using Elastos::Droid::Internal::Policy::IPolicyManager;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::CLockPatternUtilsCacheHelper;
using Elastos::Droid::Internal::Widget::ILockPatternUtilsCache;
using Elastos::Droid::Internal::Widget::ILockPatternUtilsCacheHelper;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::R;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::ITextView;

using Elastos::Core::CoreUtils;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

//=====================================================================
//                        LockToAppRequestDialog
//=====================================================================
CAR_INTERFACE_IMPL(LockToAppRequestDialog, Object, IDialogInterfaceOnClickListener);

const String LockToAppRequestDialog::TAG("ActivityManager");

LockToAppRequestDialog::LockToAppRequestDialog(
    /* [in] */ IContext* context,
    /* [in] */ CActivityManagerService* activityManagerService)
{
    mContext = context;
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
    mAccessibilityService = IAccessibilityManager::Probe(obj);
    mService = activityManagerService;
    AutoPtr<IPolicyManager> pm;
    CPolicyManager::AcquireSingleton((IPolicyManager**)&pm);
    pm->MakeNewWindowManager((IWindowManagerPolicy**)&mPolicy);
}

ECode LockToAppRequestDialog::ClearPrompt()
{
    if (mDialog != NULL) {
        IDialogInterface::Probe(mDialog)->Dismiss();
        mDialog = NULL;
    }
    return NOERROR;
}

ECode LockToAppRequestDialog::ShowLockTaskPrompt(
    /* [in] */ TaskRecord* task)
{
    ClearPrompt();
    mRequestedTask = task;
    Int32 unlockStringId = GetLockString(task->mUserId);

    AutoPtr<IResourcesHelper> resHelper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&resHelper);
    AutoPtr<IResources> r;
    resHelper->GetSystem((IResources**)&r);
    String description;
    Boolean hasNavigationBar;
    mPolicy->HasNavigationBar(&hasNavigationBar);
    if (!hasNavigationBar) {
        Boolean isEnabled;
        r->GetString((mAccessibilityService->IsEnabled(&isEnabled), isEnabled)
                ? R::string::lock_to_app_description_accessible
                : R::string::lock_to_app_description, &description);
    }
    else {
        r->GetString(R::string::lock_to_app_description_no_navbar, &description);
    }

    String title, positiveButton, negativeButtion;
    r->GetString(R::string::lock_to_app_title, &title);
    r->GetString(R::string::lock_to_app_positive, &positiveButton);
    r->GetString(R::string::lock_to_app_negative, &negativeButtion);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mContext, (IAlertDialogBuilder**)&builder);
    AutoPtr<ICharSequence> titleCS = CoreUtils::Convert(title);
    AutoPtr<ICharSequence> descriptionCS = CoreUtils::Convert(description);
    AutoPtr<ICharSequence> positiveButtonCS = CoreUtils::Convert(positiveButton);
    AutoPtr<ICharSequence> negativeButtionCS = CoreUtils::Convert(negativeButtion);
    builder->SetTitle(titleCS);
    builder->SetMessage(descriptionCS);
    builder->SetPositiveButton(positiveButtonCS, this);
    builder->SetNegativeButton(negativeButtionCS, this);

    if (unlockStringId != 0) {
        assert(0 && "TODO");
        // builder->SetView(R::layout::lock_to_app_checkbox);
    }
    builder->Create((IAlertDialog**)&mDialog);

    AutoPtr<IWindow> window;
    IDialog::Probe(mDialog)->GetWindow((IWindow**)&window);
    window->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    AutoPtr<IWindowManagerLayoutParams> attributes;
    window->GetAttributes((IWindowManagerLayoutParams**)&attributes);
    Int32 privateFlags;
    attributes->GetPrivateFlags(&privateFlags);
    privateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_SHOW_FOR_ALL_USERS;
    attributes->SetPrivateFlags(privateFlags);
    IDialog::Probe(mDialog)->Show();

    if (unlockStringId != 0) {
        String unlockString;
        mContext->GetString(unlockStringId, &unlockString);
        AutoPtr<IView> checkbox;
        IView::Probe(mDialog)->FindViewById(R::id::lock_to_app_checkbox, (IView**)&checkbox);
        mCheckbox = ICheckBox::Probe(checkbox);
        AutoPtr<ICharSequence> unlockStringCS;
        CString::New(unlockString, (ICharSequence**)&unlockStringCS);
        ITextView::Probe(mCheckbox)->SetText(unlockStringCS);

        // Remember state.
        //try {
            AutoPtr<IContentResolver> contentResolver;
            mContext->GetContentResolver((IContentResolver**)&contentResolver);
            Int32 iTemp;
            Settings::System::GetInt32(contentResolver,
                    ISettingsSystem::LOCK_TO_APP_EXIT_LOCKED, &iTemp);
            Boolean useLock = iTemp != 0;
            ICheckable::Probe(mCheckbox)->SetChecked(useLock);
        //} catch (SettingNotFoundException e) {
        //}
    } else {
        mCheckbox = NULL;
    }
    return NOERROR;
}

ECode LockToAppRequestDialog::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (IDialogInterface::BUTTON_POSITIVE == which) {
        Slogger::D(TAG, "accept lock-to-app request");
        // Set whether to use the lock screen when exiting.
        AutoPtr<IContentResolver> contentResolver;
        mContext->GetContentResolver((IContentResolver**)&contentResolver);
        Boolean isChecked;
        Boolean bTemp;
        Settings::System::PutInt32(contentResolver,
                ISettingsSystem::LOCK_TO_APP_EXIT_LOCKED,
                mCheckbox != NULL && (ICheckable::Probe(mCheckbox)->IsChecked(&isChecked), isChecked) ? 1 : 0, &bTemp);

        // Start lock-to-app.
        mService->StartLockTaskMode(mRequestedTask);
    } else {
        Slogger::D(TAG, "ignore lock-to-app request");
    }
    return NOERROR;
}

AutoPtr<IILockSettings> LockToAppRequestDialog::GetLockSettings()
{
    if (mLockSettingsService == NULL) {
        AutoPtr<IInterface> service;
        AutoPtr<IServiceManager> serviceManager;
        CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
        serviceManager->GetService(String("lock_settings"), (IInterface**)&service);

        AutoPtr<ILockPatternUtilsCacheHelper> lpucHelper;
        CLockPatternUtilsCacheHelper::AcquireSingleton((ILockPatternUtilsCacheHelper**)&lpucHelper);
        AutoPtr<ILockPatternUtilsCache> lpuc;
        lpucHelper->GetInstance(IILockSettings::Probe(service), (ILockPatternUtilsCache**)&lpuc);
        mLockSettingsService = IILockSettings::Probe(lpuc);
    }
    return mLockSettingsService;
}

Int32 LockToAppRequestDialog::GetLockString(
    /* [in] */ Int32 userId)
{
    //try {
        AutoPtr<IILockSettings> ls = GetLockSettings();
        Int64 res;
        ls->GetInt64(ILockPatternUtils::PASSWORD_TYPE_KEY,
                        IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED, userId, &res);
        Int32 quality = (Int32)res;
        switch (quality) {
            case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC:
            case IDevicePolicyManager::PASSWORD_QUALITY_NUMERIC_COMPLEX:
                return R::string::lock_to_app_unlock_pin;
            case IDevicePolicyManager::PASSWORD_QUALITY_ALPHABETIC:
            case IDevicePolicyManager::PASSWORD_QUALITY_ALPHANUMERIC:
            case IDevicePolicyManager::PASSWORD_QUALITY_COMPLEX:
                return R::string::lock_to_app_unlock_password;
            case IDevicePolicyManager::PASSWORD_QUALITY_SOMETHING:
                Boolean bTemp;
                if (ls->GetBoolean(ISettingsSecure::LOCK_PATTERN_ENABLED, FALSE,
                        userId, &bTemp), bTemp) {
                    return R::string::lock_to_app_unlock_pattern;
                }
        }
    //} catch (RemoteException e) {
    //}
    return 0;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
