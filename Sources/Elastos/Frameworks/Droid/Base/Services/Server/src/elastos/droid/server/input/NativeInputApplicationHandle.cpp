
#include "elastos/droid/server/input/NativeInputApplicationHandle.h"
#include "elastos/droid/server/input/InputApplicationHandle.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

NativeInputApplicationHandle::NativeInputApplicationHandle(
    /* [in] */ IWeakReference* obj)
    : mObjWeak(obj)
{}

NativeInputApplicationHandle::~NativeInputApplicationHandle()
{
    mObjWeak = NULL;
}

AutoPtr<Elastos::Droid::Server::Input::InputApplicationHandle> NativeInputApplicationHandle::getInputApplicationHandleObj()
{
    AutoPtr<IInputApplicationHandle> obj;
    mObjWeak->Resolve(EIID_IInputApplicationHandle, (IInterface**)&obj);
    return (Elastos::Droid::Server::Input::InputApplicationHandle*)obj.Get();
}

bool NativeInputApplicationHandle::updateInfo()
{
    AutoPtr<IInputApplicationHandle> obj;
    mObjWeak->Resolve(EIID_IInputApplicationHandle, (IInterface**)&obj);
    if (!obj) {
        releaseInfo();
        return false;
    }

    if (!mInfo) {
        mInfo = new android::InputApplicationInfo();
    }

    Elastos::Droid::Server::Input::InputApplicationHandle* handle =
            (Elastos::Droid::Server::Input::InputApplicationHandle*)obj.Get();
    if (!handle->mName.IsNull()) {
        mInfo->name.setTo(handle->mName.string());
    }
    else {
        mInfo->name.setTo("<null>");
    }

    mInfo->dispatchingTimeout = handle->mDispatchingTimeoutNanos;

    return true;
}

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos
