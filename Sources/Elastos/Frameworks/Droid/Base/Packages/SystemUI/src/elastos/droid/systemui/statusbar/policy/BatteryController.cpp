
#include "elastos/droid/systemui/statusbar/policy/BatteryController.h"
#include "elastos/droid/systemui/statusbar/policy/CBatteryControllerBroadcastReceiver.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
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
CAR_INTERFACE_IMPL(BatteryController, Object, IBatteryController);
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

    AutoPtr<IBroadcastReceiver> b;
    CBatteryControllerBroadcastReceiver::New(this, (IBroadcastReceiver**)&b);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(b, filter, (IIntent**)&intent);

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
