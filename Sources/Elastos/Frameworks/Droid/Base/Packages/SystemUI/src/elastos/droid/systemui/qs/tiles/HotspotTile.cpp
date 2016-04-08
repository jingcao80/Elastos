
#include "elastos/droid/systemui/qs/tiles/HotspotTile.h"
#include "../../R.h"

using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IHotspotControllerCallback;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

HotspotTile::APChangedReceiver::APChangedReceiver(
    /* [in] */ HotspotTile* host)
    : mHost(host)
{}

ECode HotspotTile::APChangedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (mUsageTracker == NULL) {
        mUsageTracker = new UsageTracker(context, mHost/*HotspotTile.class*/);
    }
    mUsageTracker->TrackUsage();
    return NOERROR;
}

CAR_INTERFACE_IMPL(HotspotTile::Callback, Object, IHotspotControllerCallback);
HotspotTile::Callback::Callback(
    /* [in] */ HotspotTile* host)
    : mHost(host)
{}

ECode HotspotTile::Callback::OnHotspotChanged(
    /* [in] */ Boolean enabled)
{
    mHost->RefreshState();
    return NOERROR;
}

HotspotTile::HotspotTile(
    /* [in] */ IQSTileHost* host)
    : QSTile(host)
{
    mCallback = new Callback(this);
    host->GetHotspotController((IHotspotController**)&mController);
    AutoPtr<IContext> ctx;
    host->GetContext((IContext**)&ctx);
    mUsageTracker = new UsageTracker(ctx, this/*HotspotTile.class*/);
    mUsageTracker->SetListening(TRUE);
}

void HotspotTile::HandleDestroy()
{
    QSTile::HandleDestroy();
    mUsageTracker->SetListening(FALSE);
}

AutoPtr<QSTile::State> HotspotTile::NewTileState()
{
    return new BooleanState();
}

ECode HotspotTile::SetListening(
    /* [in] */ Boolean listening)
{
    if (listening) {
        mController->AddCallback(mCallback);
    }
    else {
        mController->RemoveCallback(mCallback);
    }
    return NOERROR;
}

void HotspotTile::HandleClick()
{
    const Boolean isEnabled = ((BooleanState*)mState.Get())->mValue;
    mController->SetHotspotEnabled(!isEnabled);
}

void HotspotTile::HandleUpdateState(
    /* [in] */ State* state,
    /* [in] */ IInterface* arg)
{
    Boolean tmp = FALSE;
    state->mVisible = (mController->IsHotspotSupported(&tmp), tmp) && mUsageTracker->IsRecentlyUsed()
            && (mController->IsProvisioningNeeded(&tmp), !tmp);
    mContext->GetString(R::string::quick_settings_hotspot_label, &state->mLabel);

    mController->IsHotspotEnabled(&((BooleanState*)state)->mValue);
    state->mIconId = state->mVisible && ((BooleanState*)state)->mValue ? R::drawable::ic_qs_hotspot_on
            : R::drawable::ic_qs_hotspot_off;
}

String HotspotTile::ComposeChangeAnnouncement()
{
    String v;
    if (((BooleanState*)mState.Get())->mValue) {
        mContext->GetString(R::string::accessibility_quick_settings_hotspot_changed_on, &v);
    }
    else {
        mContext->GetString(R::string::accessibility_quick_settings_hotspot_changed_off, &v);
    }
    return v;
}

String HotspotTile::GetSimpleName()
{
    return String("HotspotTile");
}

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
