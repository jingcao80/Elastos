
#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/CCryptKeeperConfirm.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::Storage::IIMountService;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CCryptKeeperConfirm::Blank::MyRunnable
//===============================================================================

CCryptKeeperConfirm::Blank::MyRunnable::MyRunnable(
    /* [in] */ Blank* host)
    : mHost(host)
{}

ECode CCryptKeeperConfirm::Blank::MyRunnable::Run()
{
    AutoPtr<IInterface> service = ServiceManager::GetService(String("mount"));
    if (service == NULL) {
        Logger::E("CCryptKeeperConfirm", "Failed to find the mount service");
        mHost->Finish();
        return NOERROR;
    }

    IIMountService* mountService = IIMountService::Probe(service);
    // try {
    AutoPtr<IIntent> intent;
    mHost->GetIntent((IIntent**)&intent);
    AutoPtr<IBundle> args;
    intent->GetExtras((IBundle**)&args);
    Int32 data;
    args->GetInt32(String("type"), -1, &data);
    String str;
    args->GetString(String("password"), &str);
    Int32 result;
    mountService->EncryptStorage(data, str, &result);
    // } catch (Exception e) {
    //     Logger::E("CCryptKeeperConfirm", "Error while encrypting...", e);
    // }
    return NOERROR;
}

//===============================================================================
//                  CCryptKeeperConfirm::Blank
//===============================================================================

CCryptKeeperConfirm::Blank::Blank()
{}

CCryptKeeperConfirm::Blank::~Blank()
{}

ECode CCryptKeeperConfirm::Blank::constructor()
{
    CHandler::New((IHandler**)&mHandler);
    return Activity::constructor();
}

ECode CCryptKeeperConfirm::Blank::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);

    SetContentView(R::layout::crypt_keeper_blank);

    if (Utils::IsMonkeyRunning()) {
        Finish();
    }

    AutoPtr<IInterface> obj;
    GetSystemService(IContext::STATUS_BAR_SERVICE, (IInterface**)&obj);
    IStatusBarManager* sbm = IStatusBarManager::Probe(obj);
    sbm->Disable(IStatusBarManager::DISABLE_EXPAND
            | IStatusBarManager::DISABLE_NOTIFICATION_ICONS
            | IStatusBarManager::DISABLE_NOTIFICATION_ALERTS
            | IStatusBarManager::DISABLE_SYSTEM_INFO
            | IStatusBarManager::DISABLE_HOME
            | IStatusBarManager::DISABLE_SEARCH
            | IStatusBarManager::DISABLE_RECENT
            | IStatusBarManager::DISABLE_BACK);

    // Post a delayed message in 700 milliseconds to enable encryption.
    // NOTE: The animation on this activity is set for 500 milliseconds
    // I am giving it a little extra time to complete.
    AutoPtr<MyRunnable> runnable = new MyRunnable(this);
    Boolean res;
    mHandler->PostDelayed(runnable, 700, &res);
    return NOERROR;
}

//===============================================================================
//                  CCryptKeeperConfirm::MyOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CCryptKeeperConfirm::MyOnClickListener, Object, IViewOnClickListener)

CCryptKeeperConfirm::MyOnClickListener::MyOnClickListener(
    /* [in] */ CCryptKeeperConfirm* host)
    : mHost(host)
{}

ECode CCryptKeeperConfirm::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    if (Utils::IsMonkeyRunning()) {
        return NOERROR;
    }

    /* WARNING - nasty hack!
       Settings for the lock screen are not available to the crypto
       screen (CryptKeeper) at boot. We duplicate the ones that
       CryptKeeper needs to the crypto key/value store when they are
       modified (see LockPatternUtils).
       However, prior to encryption, the crypto key/value store is not
       persisted across reboots, thus modified settings are lost to
       CryptKeeper.
       In order to make sure CryptKeeper had the correct settings after
       first encrypting, we thus need to rewrite them, which ensures the
       crypto key/value store is up to date. On encryption, this store
       is then persisted, and the settings will be there on future
       reboots.
     */

    // 1. The owner info.
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    AutoPtr<ILockPatternUtils> utils;
    CLockPatternUtils::New(context, (ILockPatternUtils**)&utils);
    Boolean res;
    utils->IsVisiblePatternEnabled(&res);
    utils->SetVisiblePatternEnabled(res);
    if (utils->IsOwnerInfoEnabled(&res), res) {
        String str;
        utils->GetOwnerInfo(IUserHandle::USER_OWNER, &str);
        utils->SetOwnerInfo(str, IUserHandle::USER_OWNER);
    }
    AutoPtr<IIntent> intent;
    CIntent::New(context, ECLSID_CCryptKeeperConfirmBlank, (IIntent**)&intent);
    AutoPtr<IBundle> arguments;
    mHost->GetArguments((IBundle**)&arguments);
    intent->PutExtras(arguments);
    mHost->StartActivity(intent);

    // 2. The system locale.
    // try {
    AutoPtr<IInterface> service = ServiceManager::GetService(String("mount"));
    IIMountService* mountService = IIMountService::Probe(service);
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    String lt;
    locale->ToLanguageTag(&lt);
    mountService->SetField(String("SystemLocale"), lt);
    // } catch (Exception e) {
    //     Logger::E(TAG, "Error storing locale for decryption UI", e);
    // }
    return NOERROR;
}

//===============================================================================
//                  CCryptKeeperConfirm
//===============================================================================

const String CCryptKeeperConfirm::TAG("CCryptKeeperConfirm");

CAR_OBJECT_IMPL(CCryptKeeperConfirm)

CCryptKeeperConfirm::CCryptKeeperConfirm()
{}

CCryptKeeperConfirm::~CCryptKeeperConfirm()
{}

ECode CCryptKeeperConfirm::constructor()
{
    mFinalClickListener = new MyOnClickListener(this);
    return Fragment::constructor();
}

void CCryptKeeperConfirm::EstablishFinalConfirmationState()
{
    AutoPtr<IView> tmp;
    mContentView->FindViewById(R::id::execute_encrypt, (IView**)&tmp);
    mFinalButton = IButton::Probe(tmp);
    tmp->SetOnClickListener(mFinalClickListener);
}

ECode CCryptKeeperConfirm::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    inflater->Inflate(R::layout::crypt_keeper_confirm, NULL, (IView**)&mContentView);
    EstablishFinalConfirmationState();
    *result = mContentView;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos