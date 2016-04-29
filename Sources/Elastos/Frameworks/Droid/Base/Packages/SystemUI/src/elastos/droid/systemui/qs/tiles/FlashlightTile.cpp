
#include "elastos/droid/systemui/qs/tiles/FlashlightTile.h"
#include "../../R.h"
#include "Elastos.Droid.App.h"
#include <elastos/droid/os/SystemClock.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IFlashlightListener;
using Elastos::Core::CBoolean;
using Elastos::Core::IBoolean;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

FlashlightTile::RecentlyOnTimeout::RecentlyOnTimeout(
    /* [in] */ FlashlightTile* host)
    : mHost(host)
{}

ECode FlashlightTile::RecentlyOnTimeout::Run()
{
    mHost->RefreshState();
    return NOERROR;
}

const Int64 FlashlightTile::RECENTLY_ON_DURATION_MILLIS = 500;
CAR_INTERFACE_IMPL(FlashlightTile, QSTile, IFlashlightListener);
FlashlightTile::FlashlightTile(
    /* [in] */ IQSTileHost* host)
{
    QSTile::constructor(host);
    mRecentlyOnTimeout = new RecentlyOnTimeout(this);
    host->GetFlashlightController((IFlashlightController**)&mFlashlightController);
    mFlashlightController->AddListener(this);
}

void FlashlightTile::HandleDestroy()
{
    QSTile::HandleDestroy();
    mFlashlightController->RemoveListener(this);
}

AutoPtr<QSTile::State> FlashlightTile::NewTileState()
{
    return new BooleanState();
}

ECode FlashlightTile::SetListening(
    /* [in] */ Boolean listening)
{
    return NOERROR;
}

void FlashlightTile::HandleUserSwitch(
    /* [in] */ Int32 newUserId)
{
}

void FlashlightTile::HandleClick()
{
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Boolean tmp = FALSE;
    if (helper->IsUserAMonkey(&tmp), tmp) {
        return;
    }
    Boolean newState = !((BooleanState*)mState.Get())->mValue;
    mFlashlightController->SetFlashlight(newState);
    AutoPtr<IBoolean> o;
    CBoolean::New(newState, (IBoolean**)&o);
    RefreshState(o);
}

void FlashlightTile::HandleUpdateState(
    /* [in] */ State* state,
    /* [in] */ IInterface* arg)
{
    if (((BooleanState*)state)->mValue) {
        mWasLastOn = SystemClock::GetUptimeMillis();
    }

    if (IBoolean::Probe(arg)) {
        Boolean v;
        IBoolean::Probe(arg)->GetValue(&v);
        ((BooleanState*)state)->mValue = v;
    }

    Boolean tmp = FALSE;
    if (!((BooleanState*)state)->mValue && mWasLastOn != 0) {
        if (SystemClock::GetUptimeMillis() > mWasLastOn + RECENTLY_ON_DURATION_MILLIS) {
            mWasLastOn = 0;
        }
        else {
            mHandler->RemoveCallbacks(mRecentlyOnTimeout);
            mHandler->PostAtTime(mRecentlyOnTimeout, mWasLastOn + RECENTLY_ON_DURATION_MILLIS, &tmp);
        }
    }

    // Always show the tile when the flashlight is or was recently on. This is needed because
    // the camera is not available while it is being used for the flashlight.
    state->mVisible = mWasLastOn != 0 || (mFlashlightController->IsAvailable(&tmp), tmp);

    AutoPtr<IContext> ctx;
    mHost->GetContext((IContext**)&ctx);
    ctx->GetString(R::string::quick_settings_flashlight_label, &state->mLabel);
    state->mIconId = ((BooleanState*)state)->mValue
            ? R::drawable::ic_qs_flashlight_on : R::drawable::ic_qs_flashlight_off;
    Int32 onOrOffId = ((BooleanState*)state)->mValue
            ? R::string::accessibility_quick_settings_flashlight_on
            : R::string::accessibility_quick_settings_flashlight_off;
    mContext->GetString(onOrOffId, &state->mContentDescription);
}

String FlashlightTile::ComposeChangeAnnouncement()
{
    String v;
    if (((BooleanState*)mState.Get())->mValue) {
        mContext->GetString(R::string::accessibility_quick_settings_flashlight_changed_on, &v);
    }
    else {
        mContext->GetString(R::string::accessibility_quick_settings_flashlight_changed_off, &v);
    }
    return v;
}

ECode FlashlightTile::OnFlashlightOff()
{
    AutoPtr<IBoolean> o;
    CBoolean::New(FALSE, (IBoolean**)&o);
    RefreshState(o);
    return NOERROR;
}

ECode FlashlightTile::OnFlashlightError()
{
    AutoPtr<IBoolean> o;
    CBoolean::New(FALSE, (IBoolean**)&o);
    RefreshState(o);
    return NOERROR;
}

ECode FlashlightTile::OnFlashlightAvailabilityChanged(
    /* [in] */ Boolean available)
{
    RefreshState();
    return NOERROR;
}

String FlashlightTile::GetSimpleName()
{
    return String("FlashlightTile");
}

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
