
#include "elastos/droid/systemui/statusbar/policy/CastDevice.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_INTERFACE_IMPL(CastDevice, Object, ICastControllerCastDevice)
CastDevice::CastDevice()
    : mState(STATE_DISCONNECTED)
{}

ECode CastDevice::SetId(
    /* [in] */ const String& value)
{
    mId = value;
    return NOERROR;
}

ECode CastDevice::GetId(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mId;
    return NOERROR;
}

ECode CastDevice::SetName(
    /* [in] */ const String& value)
{
    mName = value;
    return NOERROR;
}

ECode CastDevice::GetName(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mName;
    return NOERROR;
}

ECode CastDevice::SetDescription(
    /* [in] */ const String& value)
{
    mDescription = value;
    return NOERROR;
}

ECode CastDevice::GetDescription(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mDescription;
    return NOERROR;
}

ECode CastDevice::SetState(
    /* [in] */ Int32 state)
{
    mState = state;
    return NOERROR;
}

ECode CastDevice::GetState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mState;
    return NOERROR;
}

ECode CastDevice::SetTag(
    /* [in] */ IInterface* tag)
{
    mTag = tag;
    return NOERROR;
}

ECode CastDevice::GetTag(
    /* [out] */ IInterface** tag)
{
    VALIDATE_NOT_NULL(tag);
    *tag = mTag;
    REFCOUNT_ADD(*tag);
    return NOERROR;
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
