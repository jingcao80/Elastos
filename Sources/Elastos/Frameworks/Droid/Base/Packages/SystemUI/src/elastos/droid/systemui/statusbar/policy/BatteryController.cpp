
#include "elastos/droid/systemui/statusbar/policy/BatteryController.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String BatteryController::TAG("BatteryController");
const Boolean BatteryController::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);
CAR_INTERFACE_IMPL(BatteryController, BroadcastReceiver, IBatteryController);
BatteryController::BatteryController(
    /* [in] */ IContext* context)
{
    CArrayList::New((IArrayList**)&mChangeCallbacks);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    mPowerManager = IPowerManager::Probe(obj);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_BATTERY_CHANGED);
    filter->AddAction(IPowerManager::ACTION_POWER_SAVE_MODE_CHANGED);
    filter->AddAction(IPowerManager::ACTION_POWER_SAVE_MODE_CHANGING);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(this, filter, (IIntent**)&intent);

    UpdatePowerSave();
}

ECode BatteryController::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("BatteryController state:"));
    pw->Print(String("  mLevel="));
    pw->Println(mLevel);
    pw->Print(String("  mPluggedIn="));
    pw->Println(mPluggedIn);
    pw->Print(String("  mCharging="));
    pw->Println(mCharging);
    pw->Print(String("  mCharged="));
    pw->Println(mCharged);
    pw->Print(String("  mPowerSave="));
    pw->Println(mPowerSave);
    return NOERROR;
}

ECode BatteryController::AddStateChangedCallback(
    /* [in] */ IBatteryStateChangeCallback* cb)
{
    mChangeCallbacks->Add(cb);
    cb->OnBatteryLevelChanged(mLevel, mPluggedIn, mCharging);
    return NOERROR;
}

ECode BatteryController::RemoveStateChangedCallback(
    /* [in] */ IBatteryStateChangeCallback* cb)
{
    mChangeCallbacks->Remove(cb);
    return NOERROR;
}

ECode BatteryController::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_BATTERY_CHANGED)) {
        Int32 v1 = 0, v2 = 0;
        mLevel = (Int32)(100.f
                * (intent->GetInt32Extra(IBatteryManager::EXTRA_LEVEL, 0, &v1), v1)
                / (intent->GetInt32Extra(IBatteryManager::EXTRA_SCALE, 100, &v2), v2));
        mPluggedIn = (intent->GetInt32Extra(IBatteryManager::EXTRA_PLUGGED, 0, &v1), v1) != 0;

        Int32 status = 0;
        intent->GetInt32Extra(IBatteryManager::EXTRA_STATUS,
                IBatteryManager::BATTERY_STATUS_UNKNOWN, &status);
        mCharged = status == IBatteryManager::BATTERY_STATUS_FULL;
        mCharging = mCharged || status == IBatteryManager::BATTERY_STATUS_CHARGING;

        FireBatteryLevelChanged();
    }
    else if (action.Equals(IPowerManager::ACTION_POWER_SAVE_MODE_CHANGED)) {
        UpdatePowerSave();
    }
    else if (action.Equals(IPowerManager::ACTION_POWER_SAVE_MODE_CHANGING)) {
        Boolean tmp = FALSE;
        intent->GetBooleanExtra(IPowerManager::EXTRA_POWER_SAVE_MODE, FALSE, &tmp);
        SetPowerSave(tmp);
    }
    return NOERROR;
}

ECode BatteryController::IsPowerSave(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPowerSave;
    return NOERROR;
}

void BatteryController::UpdatePowerSave()
{
    Boolean tmp = FALSE;
    SetPowerSave((mPowerManager->IsPowerSaveMode(&tmp), tmp));
}

void BatteryController::SetPowerSave(
    /* [in] */ Boolean powerSave)
{
    if (powerSave == mPowerSave) return;
    mPowerSave = powerSave;
    if (DEBUG) Logger::D(TAG, "Power save is %s", (mPowerSave ? "on" : "off"));
    FirePowerSaveChanged();
}

void BatteryController::FireBatteryLevelChanged()
{
    Int32 N = 0;
    mChangeCallbacks->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mChangeCallbacks->Get(i, (IInterface**)&obj);
        IBatteryStateChangeCallback::Probe(obj)->OnBatteryLevelChanged(mLevel, mPluggedIn, mCharging);
    }
}

void BatteryController::FirePowerSaveChanged()
{
    Int32 N = 0;
    mChangeCallbacks->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mChangeCallbacks->Get(i, (IInterface**)&obj);
        IBatteryStateChangeCallback::Probe(obj)->OnPowerSaveChanged();
    }
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
