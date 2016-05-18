
#include "elastos/droid/systemui/statusbar/policy/PairedDevice.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_INTERFACE_IMPL(PairedDevice, Object, IBluetoothControllerPairedDevice)
PairedDevice::PairedDevice()
    : mState(STATE_DISCONNECTED)
{}

ECode PairedDevice::SetId(
    /* [in] */ const String& id)
{
    mId = id;
    return NOERROR;
}

ECode PairedDevice::GetId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode PairedDevice::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode PairedDevice::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode PairedDevice::SetState(
    /* [in] */ Int32 state)
{
    mState = state;
    return NOERROR;
}

ECode PairedDevice::GetState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mState;
    return NOERROR;
}

ECode PairedDevice::SetTag(
    /* [in] */ IInterface* tag)
{
    mTag = tag;
    return NOERROR;
}

ECode PairedDevice::GetTag(
    /* [out] */ IInterface** tag)
{
    VALIDATE_NOT_NULL(tag);
    *tag = mTag;
    REFCOUNT_ADD(*tag);
    return NOERROR;
}

String PairedDevice::StateToString(
    /* [in] */ Int32 state)
{
    if (state == STATE_DISCONNECTED) return String("STATE_DISCONNECTED");
    if (state == STATE_CONNECTING) return String("STATE_CONNECTING");
    if (state == STATE_CONNECTED) return String("STATE_CONNECTED");
    if (state == STATE_DISCONNECTING) return String("STATE_DISCONNECTING");
    return String("UNKNOWN");
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
