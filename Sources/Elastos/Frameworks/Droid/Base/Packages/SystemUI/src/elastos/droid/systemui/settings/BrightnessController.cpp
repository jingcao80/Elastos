#include "elastos/droid/systemui/settings/BrightnessController.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/R.h"
#include "../R.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::SystemUI::Settings::IBrightnessStateChangeCallback;
using Elastos::Droid::R;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

//==================================
// BrightnessController::BrightnessObserver
//==================================

BrightnessController::BrightnessObserver::BrightnessObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ BrightnessController* host)
    : mHost(host)
{
    ContentObserver::constructor(handler);
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    ss->GetUriFor(ISettingsSystem::SCREEN_BRIGHTNESS_MODE, (IUri**)&BRIGHTNESS_MODE_URI);
    ss->GetUriFor(ISettingsSystem::SCREEN_BRIGHTNESS, (IUri**)&BRIGHTNESS_URI);
    ss->GetUriFor(ISettingsSystem::SCREEN_AUTO_BRIGHTNESS_ADJ, (IUri**)&BRIGHTNESS_ADJ_URI);
}

ECode BrightnessController::BrightnessObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    return OnChange(selfChange, NULL);
}

ECode BrightnessController::BrightnessObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    if (selfChange) return E_NULL_POINTER_EXCEPTION;

    mHost->mExternalChange = TRUE;
    Boolean isEqual1, isEqual2, isEqual3;
    IObject::Probe(BRIGHTNESS_MODE_URI)->Equals(uri, &isEqual1);
    IObject::Probe(BRIGHTNESS_URI)->Equals(uri, &isEqual2);
    IObject::Probe(BRIGHTNESS_ADJ_URI)->Equals(uri, &isEqual3);
    if (isEqual1) {
        mHost->UpdateMode();
        mHost->UpdateSlider();
    }
    else if (isEqual2 && !(mHost->mAutomatic)) {
        mHost->UpdateSlider();
    }
    else if (isEqual3 && mHost->mAutomatic) {
        mHost->UpdateSlider();
    }
    else {
        mHost->UpdateMode();
        mHost->UpdateSlider();
    }
    AutoPtr<IIterator> it;
    IIterable::Probe(mHost->mChangeCallbacks)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IBrightnessStateChangeCallback> cb = IBrightnessStateChangeCallback::Probe(obj);
        cb->OnBrightnessLevelChanged();
    }
    mHost->mExternalChange = FALSE;

    return NOERROR;
}

ECode BrightnessController::BrightnessObserver::StartObserving()
{
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    cr->UnregisterContentObserver(this);
    cr->RegisterContentObserver(
        BRIGHTNESS_MODE_URI, FALSE, this, IUserHandle::USER_ALL);
    cr->RegisterContentObserver(
        BRIGHTNESS_URI, FALSE, this, IUserHandle::USER_ALL);
    cr->RegisterContentObserver(
        BRIGHTNESS_ADJ_URI, FALSE, this, IUserHandle::USER_ALL);
    return NOERROR;
}

ECode BrightnessController::BrightnessObserver::StopObserving()
{
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    cr->UnregisterContentObserver(this);
    return NOERROR;
}

//==================================
// BrightnessController::MyCurrentUserTracker
//==================================

BrightnessController::MyCurrentUserTracker::MyCurrentUserTracker(
    /* [in] */ IContext* context,
    /* [in] */ BrightnessController* host)
    : CurrentUserTracker(context)
    , mHost(host)
{}

void BrightnessController::MyCurrentUserTracker::OnUserSwitched(
    /* [in] */ Int32 newUserId)
{
    mHost->UpdateMode();
    mHost->UpdateSlider();
}

//==================================
// BrightnessController::Runnable1
//==================================

BrightnessController::Runnable1::Runnable1(
    /* [in] */ Int32 val,
    /* [in] */ BrightnessController* host)
    : mVal(val)
    , mHost(host)
{}

ECode BrightnessController::Runnable1::Run()
{
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    Boolean result;
    return ss->PutInt32ForUser(cr,
        ISettingsSystem::SCREEN_BRIGHTNESS, mVal,
        IUserHandle::USER_CURRENT, &result);
}

//==================================
// BrightnessController::Runnable2
//==================================

BrightnessController::Runnable2::Runnable2(
    /* [in] */ Float adj,
    /* [in] */ BrightnessController* host)
    : mAdj(adj)
    , mHost(host)
{}

ECode BrightnessController::Runnable2::Run()
{
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    Boolean result;
    return ss->PutFloatForUser(cr,
        ISettingsSystem::SCREEN_AUTO_BRIGHTNESS_ADJ, mAdj,
        IUserHandle::USER_CURRENT, &result);
}

//==================================
// BrightnessController
//==================================
const String BrightnessController::TAG("StatusBar.BrightnessController");
const Boolean BrightnessController::SHOW_AUTOMATIC_ICON = FALSE;
const Float BrightnessController::BRIGHTNESS_ADJ_RESOLUTION = 100.0f;

CAR_INTERFACE_IMPL(BrightnessController, Object, IToggleSliderListener)

BrightnessController::BrightnessController(
    /* [in] */ IContext* context,
    /* [in] */ IImageView* icon,
    /* [in] */ CToggleSlider* control)
    : mContext(context)
    , mIcon(icon)
    , mControl(control)
    , mAutomaticAvailable(FALSE)
    , mAutomatic(FALSE)
    , mListening(FALSE)
    , mExternalChange(FALSE)
{
    CHandler::New((IHandler**)&mHandler);
    mUserTracker = new MyCurrentUserTracker(mContext, this);
    mBrightnessObserver = new BrightnessObserver(mHandler, this);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
    pm->GetMinimumScreenBrightnessSetting(&mMinimumBacklight);
    pm->GetMaximumScreenBrightnessSetting(&mMaximumBacklight);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetBoolean(Elastos::Droid::R::bool_::config_automatic_brightness_available, &mAutomaticAvailable);

    AutoPtr<IInterface> smObj = ServiceManager::GetService(String("power"));
    mPower = IIPowerManager::Probe(smObj);
}

void BrightnessController::AddStateChangedCallback(
    /* [in] */ IBrightnessStateChangeCallback* cb)
{
    mChangeCallbacks->Add(cb);
}

Boolean BrightnessController::RemoveStateChangedCallback(
    /* [in] */ IBrightnessStateChangeCallback* cb)
{
    Boolean isRemoved;
    mChangeCallbacks->Remove(cb, &isRemoved);
    return isRemoved;
}

ECode BrightnessController::OnInit(
    /* [in] */ IToggleSlider* control)
{
    // Do nothing
    return NOERROR;
}

void BrightnessController::RegisterCallbacks()
{
    if (mListening) {
        return;
    }

    mBrightnessObserver->StartObserving();
    mUserTracker->StartTracking();

    // Update the slider and mode before attaching the listener so we don't
    // receive the onChanged notifications for the initial values.
    UpdateMode();
    UpdateSlider();

    mControl->SetOnChangedListener(this);
    mListening = TRUE;
}

void BrightnessController::UnregisterCallbacks()
{
    if (!mListening) {
        return;
    }

    mBrightnessObserver->StopObserving();
    mUserTracker->StopTracking();
    mControl->SetOnChangedListener(NULL);
    mListening = FALSE;
}

ECode BrightnessController::OnChanged(
    /* [in] */ IToggleSlider* view,
    /* [in] */ Boolean tracking,
    /* [in] */ Boolean automatic,
    /* [in] */ Int32 value)
{
    UpdateIcon(mAutomatic);
    if (mExternalChange) return E_NULL_POINTER_EXCEPTION;

    if (!mAutomatic) {
        const Int32 val = value + mMinimumBacklight;
        SetBrightness(val);
        if (!tracking) {
            AutoPtr<Runnable1> runnable = new Runnable1(val, this);
            AsyncTask::Execute(runnable);
        }
    }
    else {
        const Float adj = value / (BRIGHTNESS_ADJ_RESOLUTION / 2.0f) - 1;
        SetBrightnessAdj(adj);
        if (!tracking) {
            AutoPtr<Runnable2> runnable = new Runnable2(adj, this);
            AsyncTask::Execute(runnable);
        }
    }

    AutoPtr<IIterator> it;
    IIterable::Probe(mChangeCallbacks)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IBrightnessStateChangeCallback> cb = IBrightnessStateChangeCallback::Probe(obj);
        cb->OnBrightnessLevelChanged();
    }
    return NOERROR;
}

void BrightnessController::SetMode(
    /* [in] */ Int32 mode)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    Boolean result;
    ss->PutInt32ForUser(cr,
        ISettingsSystem::SCREEN_BRIGHTNESS_MODE, mode,
        mUserTracker->GetCurrentUserId(), &result);
}

ECode BrightnessController::SetBrightness(
    /* [in] */ Int32 brightness)
{
    ECode ec = mPower->SetTemporaryScreenBrightnessSettingOverride(brightness);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode BrightnessController::SetBrightnessAdj(
    /* [in] */ Float brightness)
{
    ECode ec = mPower->SetTemporaryScreenAutoBrightnessAdjustmentSettingOverride(brightness);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

void BrightnessController::UpdateIcon(
    /* [in] */ Boolean automatic)
{
    if (mIcon != NULL) {
        mIcon->SetImageResource(automatic && SHOW_AUTOMATIC_ICON ?
            R::drawable::ic_qs_brightness_auto_on : R::drawable::ic_qs_brightness_auto_off);
    }
}

void BrightnessController::UpdateMode()
{
    if (mAutomaticAvailable) {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsSystem> ss;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
        Int32 automatic;
        ss->GetInt32ForUser(cr,
            ISettingsSystem::SCREEN_BRIGHTNESS_MODE,
            ISettingsSystem::SCREEN_BRIGHTNESS_MODE_MANUAL,
            IUserHandle::USER_CURRENT, &automatic);
        mAutomatic = automatic != ISettingsSystem::SCREEN_BRIGHTNESS_MODE_MANUAL;
        UpdateIcon(mAutomatic);
    }
    else {
        mControl->SetChecked(FALSE);
        UpdateIcon(FALSE /*automatic*/);
    }
}

void BrightnessController::UpdateSlider()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    if (mAutomatic) {
        Float value;
        ss->GetFloatForUser(cr,
            ISettingsSystem::SCREEN_AUTO_BRIGHTNESS_ADJ, 0,
            IUserHandle::USER_CURRENT, &value);
        mControl->SetMax((Int32) BRIGHTNESS_ADJ_RESOLUTION);
        mControl->SetValue((Int32) ((value + 1) * BRIGHTNESS_ADJ_RESOLUTION / 2.0f));
    }
    else {
        Int32 value;
        ss->GetInt32ForUser(cr, ISettingsSystem::SCREEN_BRIGHTNESS,
            mMaximumBacklight, IUserHandle::USER_CURRENT, &value);
        mControl->SetMax(mMaximumBacklight - mMinimumBacklight);
        mControl->SetValue(value - mMinimumBacklight);
    }
}

} // namespace Settings
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos