
#include "elastos/droid/hardware/display/IDisplayManager.h"
#include "elastos/droid/os/AndroidParcelUtils.h"
#include <binder/Parcel.h>

using Elastos::Droid::Os::AndroidParcelUtils;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

const String IDisplayManagerProxy::DESCRIPTOR("android.hardware.display.IDisplayManager");
const Int32 IDisplayManagerProxy::TRANSACTION_getDisplayInfo = android::IBinder::FIRST_CALL_TRANSACTION + 0;
const Int32 IDisplayManagerProxy::TRANSACTION_registerCallback = android::IBinder::FIRST_CALL_TRANSACTION + 2;

CAR_INTERFACE_IMPL(IDisplayManagerProxy, Object, IIDisplayManager);

IDisplayManagerProxy::IDisplayManagerProxy(
    /* [in] */ android::sp<android::IBinder>& dm)
    : mDm(dm)
{}

ECode IDisplayManagerProxy::GetDisplayInfo(
    /* [in] */ Int32 displayId,
    /* [out] */ IDisplayInfo** displayInfo)
{
    VALIDATE_NOT_NULL(displayInfo);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteInt32(data, displayId);
    mDm->transact(TRANSACTION_getDisplayInfo, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IDisplayInfo> info = AndroidParcelUtils::ReadDisplayInfo(reply);
        *displayInfo = info;
        REFCOUNT_ADD(*displayInfo);
    }
    else {
        *displayInfo = NULL;
    }
    return ec;
}

ECode IDisplayManagerProxy::GetDisplayIds(
    /* [out, callee] */ ArrayOf<Int32>** displayIds)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IDisplayManagerProxy::RegisterCallback(
    /* [in] */ IIDisplayManagerCallback* cb)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteIIDisplayManagerCallback(data, cb);
    mDm->transact(TRANSACTION_registerCallback, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

ECode IDisplayManagerProxy::StartWifiDisplayScan()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IDisplayManagerProxy::StopWifiDisplayScan()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IDisplayManagerProxy::ConnectWifiDisplay(
    /* [in] */ const String& address)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IDisplayManagerProxy::DisconnectWifiDisplay()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IDisplayManagerProxy::RenameWifiDisplay(
    /* [in] */ const String& address,
    /* [in] */ const String& alias)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IDisplayManagerProxy::ForgetWifiDisplay(
    /* [in] */ const String& address)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IDisplayManagerProxy::PauseWifiDisplay()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IDisplayManagerProxy::ResumeWifiDisplay()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IDisplayManagerProxy::CreateVirtualDisplay(
    /* [in] */ IIVirtualDisplayCallback* clbk,
    /* [in] */ IIMediaProjection* projectionToken,
    /* [in] */ const String& packageName,
    /* [in] */ const String& name,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 densityDpi,
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 flags,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IDisplayManagerProxy::ResizeVirtualDisplay(
    /* [in] */ IIVirtualDisplayCallback* token,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 densityDpi)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IDisplayManagerProxy::SetVirtualDisplaySurface(
    /* [in] */ IIVirtualDisplayCallback* token,
    /* [in] */ ISurface* surface)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IDisplayManagerProxy::ReleaseVirtualDisplay(
    /* [in] */ IIVirtualDisplayCallback* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IDisplayManagerProxy::GetWifiDisplayStatus(
    /* [out] */ IWifiDisplayStatus** wds)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace Display
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
