
#include "input/NativeInputApplicationHandle.h"
#include "input/InputApplicationHandle.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

NativeInputApplicationHandle::NativeInputApplicationHandle(
    /* [in] */ Input::InputApplicationHandle* obj)
    : mObject(obj)
{}

NativeInputApplicationHandle::~NativeInputApplicationHandle()
{
    mObject = NULL;
}

Input::InputApplicationHandle* NativeInputApplicationHandle::getInputApplicationHandleObj()
{
    return mObject;
}

bool NativeInputApplicationHandle::updateInfo()
{
    if (mObject == NULL) {
        releaseInfo();
        return false;
    }

    if (!mInfo) {
        mInfo = new android::InputApplicationInfo();
    }

    if (!mObject->mName.IsNull()) {
        mInfo->name.setTo((const char*)mObject->mName);
    }
    else {
        mInfo->name.setTo("<null>");
    }

    mInfo->dispatchingTimeout = mObject->mDispatchingTimeoutNanos;

    return true;
}

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos
