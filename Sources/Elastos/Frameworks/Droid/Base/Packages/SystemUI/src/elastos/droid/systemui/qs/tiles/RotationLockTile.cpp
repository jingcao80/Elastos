
#include "elastos/droid/systemui/qs/tiles/RotationLockTile.h"
#include "../R.h"

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IRotationLockControllerCallback;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

CAR_INTERFACE_IMPL(RotationLockTile::Callback, Object, IRotationLockControllerCallback)
RotationLockTile::Callback::Callback(
    /* [in] */ RotationLockTile* host)
    : mHost(host)
{}

ECode RotationLockTile::Callback::OnRotationLockStateChanged(
    /* [in] */ Boolean rotationLocked,
    /* [in] */ Boolean affordanceVisible)
{
    mHost->RefreshState();
    return NOERROR;
}

RotationLockTile::constructor(
    /* [in] */ IQSTileHost* host)
{
    QSTile::constructor(host);
    mCallback = new Callback(this);
    host->GetRotationLockController((IRotationLockController**)&mController);
    return NOERROR;
}

AutoPtr<QSTile::State> RotationLockTile::NewTileState()
{
    return new BooleanState();
}

ECode RotationLockTile::SetListening(
    /* [in] */ Boolean listening)
{
    if (mController == NULL) return NOERROR;
    if (listening) {
        mController->AddRotationLockControllerCallback(mCallback);
    }
    else {
        mController->RemoveRotationLockControllerCallback(mCallback);
    }
    return NOERROR;
}

void RotationLockTile::HandleClick()
{
    if (mController == NULL) return;
    mController->SetRotationLocked(!((BooleanState*)mState.Get())->mValue);
}

void RotationLockTile::HandleUpdateState(
    /* [in] */ State* state,
    /* [in] */ IInterface* arg)
{
    if (mController == NULL) return;
    Boolean rotationLocked = FALSE;
    mController->IsRotationLocked(&rotationLocked);
    mController->IsRotationLockAffordanceVisible(&state->mVisible);
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    ((BooleanState*)state)->mValue = rotationLocked;
    if (rotationLocked) {
        AutoPtr<IConfiguration> config;
        res->GetConfiguration((IConfiguration**)&config);
        Int32 orientation = 0;
        config->GetOrientation(&orientation);
        const Boolean portrait = orientation != IConfiguration::ORIENTATION_LANDSCAPE;
        const Int32 label = portrait ? R::string::quick_settings_rotation_locked_portrait_label
                : R::string::quick_settings_rotation_locked_landscape_label;
        const Int32 icon = portrait ? R::drawable::ic_qs_rotation_portrait
                : R::drawable::ic_qs_rotation_landscape;
        mContext->GetString(label, &state->mLabel);
        mContext->GetDrawable(icon, (IDrawable**)&state->mIcon);
    }
    else {
        mContext->GetString(R::string::quick_settings_rotation_unlocked_label, &state->mLabel);
        res->GetDrawable(R::drawable::ic_qs_rotation_unlocked, (IDrawable**)&state->mIcon);
    }
    state->mContentDescription = GetAccessibilityString(
            R::string::accessibility_rotation_lock_on_portrait,
            R::string::accessibility_rotation_lock_on_landscape,
            R::string::accessibility_rotation_lock_off);
}

/**
 * Get the correct accessibility string based on the state
 *
 * @param idWhenPortrait The id which should be used when locked in portrait.
 * @param idWhenLandscape The id which should be used when locked in landscape.
 * @param idWhenOff The id which should be used when the rotation lock is off.
 * @return
 */
String RotationLockTile::GetAccessibilityString(
    /* [in] */ Int32 idWhenPortrait,
    /* [in] */ Int32 idWhenLandscape,
    /* [in] */ Int32 idWhenOff)
{
    Int32 stringID;
    if (((BooleanState*)mState.Get())->mValue) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        AutoPtr<IConfiguration> config;
        res->GetConfiguration((IConfiguration**)&config);
        Int32 orientation = 0;
        config->GetOrientation(&orientation);
        const Boolean portrait = orientation != IConfiguration::ORIENTATION_LANDSCAPE;
        stringID = portrait ? idWhenPortrait: idWhenLandscape;
    }
    else {
        stringID = idWhenOff;
    }
    String v;
    mContext->GetString(stringID, &v);
    return v;
}

String RotationLockTile::ComposeChangeAnnouncement()
{
    return GetAccessibilityString(
            R::string::accessibility_rotation_lock_on_portrait_changed,
            R::string::accessibility_rotation_lock_on_landscape_changed,
            R::string::accessibility_rotation_lock_off_changed);
}

String RotationLockTile::GetSimpleName()
{
    return String("RotationLockTile");
}

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
