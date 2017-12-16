
#include "elastos/droid/view/IWindowSession.h"
#include "elastos/droid/os/AndroidParcelUtils.h"

using Elastos::Droid::Os::AndroidParcelUtils;

namespace Elastos {
namespace Droid {
namespace View {

const String IWindowSessionProxy::DESCRIPTOR("android.view.IWindowSession");
const Int32 IWindowSessionProxy::TRANSACTION_addToDisplay = android::IBinder::FIRST_CALL_TRANSACTION + 1;
const Int32 IWindowSessionProxy::TRANSACTION_remove = android::IBinder::FIRST_CALL_TRANSACTION + 4;
const Int32 IWindowSessionProxy::TRANSACTION_relayout = android::IBinder::FIRST_CALL_TRANSACTION + 5;
const Int32 IWindowSessionProxy::TRANSACTION_finishDrawing = android::IBinder::FIRST_CALL_TRANSACTION + 11;
const Int32 IWindowSessionProxy::TRANSACTION_getInTouchMode = android::IBinder::FIRST_CALL_TRANSACTION + 13;
const Int32 IWindowSessionProxy::TRANSACTION_onRectangleOnScreenRequested = android::IBinder::FIRST_CALL_TRANSACTION + 26;

CAR_INTERFACE_IMPL(IWindowSessionProxy, Object, IWindowSession);

IWindowSessionProxy::IWindowSessionProxy(
    /* [in] */ const android::sp<android::IBinder>& remote)
    : mRemote(remote)
{}

ECode IWindowSessionProxy::Add(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 seq,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ Int32 viewVisibility,
    /* [in] */ IRect* inContentInsets,
    /* [in] */ IInputChannel* inInputChannel,
    /* [out] */ IRect** outContentInsets,
    /* [out] */ IInputChannel** outInputChannel,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::AddToDisplay(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 seq,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ Int32 viewVisibility,
    /* [in] */ Int32 layerStackId,
    /* [in] */ IRect* inContentInsets,
    /* [in] */ IInputChannel* inInputChannel,
    /* [out] */ IRect** outContentInsets,
    /* [out] */ IInputChannel** outInputChannel,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(outContentInsets);
    VALIDATE_NOT_NULL(outInputChannel);
    VALIDATE_NOT_NULL(result);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR));
    AndroidParcelUtils::WriteIIWindow(data, window);
    AndroidParcelUtils::WriteInt32(data, seq);
    if (attrs != NULL) {
        AndroidParcelUtils::WriteInt32(data, 1);
        AndroidParcelUtils::WriteWindowManagerLayoutParams(data, attrs);
    }
    else {
        AndroidParcelUtils::WriteInt32(data, 0);
    }
    AndroidParcelUtils::WriteInt32(data, viewVisibility);
    AndroidParcelUtils::WriteInt32(data, layerStackId);
    mRemote->transact(TRANSACTION_addToDisplay, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    *result = AndroidParcelUtils::ReadInt32(reply);
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IRect> rect = AndroidParcelUtils::ReadRect(reply);
        *outContentInsets = rect;
        REFCOUNT_ADD(*outContentInsets);
    }
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IRect> rect = AndroidParcelUtils::ReadRect(reply);
    }
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IRect> rect = AndroidParcelUtils::ReadRect(reply);
    }
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IInputChannel> ic = AndroidParcelUtils::ReadInputChannel(reply);
        *outInputChannel = ic;
        REFCOUNT_ADD(*outInputChannel);
    }

    return ec;
}

ECode IWindowSessionProxy::AddWithoutInputChannel(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 seq,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ Int32 viewVisibility,
    /* [in] */ IRect* inContentInsets,
    /* [out] */ IRect** outContentInsets,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::AddToDisplayWithoutInputChannel(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 seq,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ Int32 viewVisibility,
    /* [in] */ Int32 layerStackId,
    /* [in] */ IRect* inContentInsets,
    /* [out] */ IRect** outContentInsets,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::Remove(
    /* [in] */ IIWindow* window)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR));
    AndroidParcelUtils::WriteIIWindow(data, window);
    mRemote->transact(TRANSACTION_remove, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

ECode IWindowSessionProxy::Relayout(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 seq,
    /* [in] */ IWindowManagerLayoutParams* attrs,
    /* [in] */ Int32 requestedWidth,
    /* [in] */ Int32 requestedHeight,
    /* [in] */ Int32 viewVisibility,
    /* [in] */ Int32 flags,
    /* [in] */ IRect* inFrame,
    /* [in] */ IRect* inOverscanInsets,
    /* [in] */ IRect* inContentInsets,
    /* [in] */ IRect* inVisibleInsets,
    /* [in] */ IRect* inStableInsets,
    /* [in] */ IConfiguration* inConfig,
    /* [in] */ ISurface* inSurface,
    /* [out] */ IRect** outFrame,
    /* [out] */ IRect** outOverscanInsets,
    /* [out] */ IRect** outContentInsets,
    /* [out] */ IRect** outVisibleInsets,
    /* [out] */ IRect** outStableInsets,
    /* [out] */ IConfiguration** outConfig,
    /* [out] */ Int32* result,
    /* [out] */ ISurface** outSurface)
{
    VALIDATE_NOT_NULL(outFrame);
    VALIDATE_NOT_NULL(outOverscanInsets);
    VALIDATE_NOT_NULL(outContentInsets);
    VALIDATE_NOT_NULL(outVisibleInsets);
    VALIDATE_NOT_NULL(outStableInsets);
    VALIDATE_NOT_NULL(outConfig);
    VALIDATE_NOT_NULL(result);
    VALIDATE_NOT_NULL(outSurface);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteIIWindow(data, window);
    AndroidParcelUtils::WriteInt32(data, seq);
    if (attrs != NULL) {
        AndroidParcelUtils::WriteInt32(data, 1);
        AndroidParcelUtils::WriteWindowManagerLayoutParams(data, attrs);
    }
    else {
        AndroidParcelUtils::WriteInt32(data, 0);
    }
    AndroidParcelUtils::WriteInt32(data, requestedWidth);
    AndroidParcelUtils::WriteInt32(data, requestedHeight);
    AndroidParcelUtils::WriteInt32(data, flags);
    mRemote->transact(TRANSACTION_relayout, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    *result = AndroidParcelUtils::ReadInt32(reply);
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IRect> rect = AndroidParcelUtils::ReadRect(reply);
        *outFrame = rect;
        REFCOUNT_ADD(*outFrame);
    }
    else {
        *outFrame = NULL;
    }
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IRect> rect = AndroidParcelUtils::ReadRect(reply);
        *outOverscanInsets = rect;
        REFCOUNT_ADD(*outOverscanInsets);
    }
    else {
        *outOverscanInsets = NULL;
    }
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IRect> rect = AndroidParcelUtils::ReadRect(reply);
        *outContentInsets = rect;
        REFCOUNT_ADD(*outContentInsets);
    }
    else {
        *outContentInsets = NULL;
    }
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IRect> rect = AndroidParcelUtils::ReadRect(reply);
        *outVisibleInsets = rect;
        REFCOUNT_ADD(*outVisibleInsets);
    }
    else {
        *outVisibleInsets = NULL;
    }
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IRect> rect = AndroidParcelUtils::ReadRect(reply);
        *outStableInsets = rect;
        REFCOUNT_ADD(*outStableInsets);
    }
    else {
        *outStableInsets = NULL;
    }
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IRect> outOutsets = AndroidParcelUtils::ReadRect(reply);
    }
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IRect> outBackdropFrame = AndroidParcelUtils::ReadRect(reply);
    }
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        String klass = AndroidParcelUtils::ReadString(reply);
        AutoPtr<IConfiguration> globalConfig = AndroidParcelUtils::ReadConfiguration(reply);
        klass = AndroidParcelUtils::ReadString(reply);
        AutoPtr<IConfiguration> overrideConfig = AndroidParcelUtils::ReadConfiguration(reply);
        klass = AndroidParcelUtils::ReadString(reply);
        AutoPtr<IConfiguration> mergedConfig = AndroidParcelUtils::ReadConfiguration(reply);
        *outConfig = globalConfig;
        REFCOUNT_ADD(*outConfig);
    }
    else {
        *outConfig = NULL;
    }
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<ISurface> surface = AndroidParcelUtils::ReadSurface(reply);
        *outSurface = surface;
        REFCOUNT_ADD(*outSurface);
    }
    else {
        *outSurface = NULL;
    }
    return ec;
}

ECode IWindowSessionProxy::PerformDeferredDestroy(
    /* [in] */ IIWindow* window)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::OutOfMemory(
    /* [in] */ IIWindow* window,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::SetTransparentRegion(
    /* [in] */ IIWindow* window,
    /* [in] */ IRegion* region)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::SetInsets(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 touchableInsets,
    /* [in] */ IRect* contentInsets,
    /* [in] */ IRect* visibleInsets,
    /* [in] */ IRegion* touchableRegion)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::GetDisplayFrame(
    /* [in] */ IIWindow* window,
    /* [out] */ IRect** outDisplayFrame)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::FinishDrawing(
    /* [in] */ IIWindow* window)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteIIWindow(data, window);
    mRemote->transact(TRANSACTION_finishDrawing, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

ECode IWindowSessionProxy::SetInTouchMode(
    /* [in] */ Boolean showFocus)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::GetInTouchMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    mRemote->transact(TRANSACTION_getInTouchMode, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    *result = AndroidParcelUtils::ReadInt32(reply) != 0;
    return ec;
}

ECode IWindowSessionProxy::PerformHapticFeedback(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 effectId,
    /* [in] */ Boolean always,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::PrepareDrag(
    /* [in] */ IIWindow* window,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 thumbnailWidth,
    /* [in] */ Int32 thumbnailHeight,
    /* [out] */ ISurface** outSurface,
    /* [out] */ IBinder** binder)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::PerformDrag(
    /* [in] */ IIWindow* window,
    /* [in] */ IBinder* dragToken,
    /* [in] */ Float touchX,
    /* [in] */ Float touchY,
    /* [in] */ Float thumbCenterX,
    /* [in] */ Float thumbCenterY,
    /* [in] */ IClipData* data,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::ReportDropResult(
    /* [in] */ IIWindow* window,
    /* [in] */ Boolean consumed)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::DragRecipientEntered(
    /* [in] */ IIWindow* window)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::DragRecipientExited(
    /* [in] */ IIWindow* window)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::SetWallpaperPosition(
    /* [in] */ IBinder* windowToken,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float xstep,
    /* [in] */ Float ystep)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::WallpaperOffsetsComplete(
    /* [in] */ IBinder* window)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::SetWallpaperDisplayOffset(
    /* [in] */ IBinder* windowToken,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::GetLastWallpaperX(
    /* [out] */ Int32* x)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::GetLastWallpaperY(
    /* [out] */ Int32* y)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::SendWallpaperCommand(
    /* [in] */ IBinder* window,
    /* [in] */ const String& action,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean sync,
    /* [out] */ IBundle** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::WallpaperCommandComplete(
    /* [in] */ IBinder* window,
    /* [in] */ IBundle* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::SetUniverseTransform(
    /* [in] */ IBinder* window,
    /* [in] */ Float alpha,
    /* [in] */ Float offx,
    /* [in] */ Float offy,
    /* [in] */ Float dsdx,
    /* [in] */ Float dtdx,
    /* [in] */ Float dsdy,
    /* [in] */ Float dtdy)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowSessionProxy::OnRectangleOnScreenRequested(
    /* [in] */ IBinder* token,
    /* [in] */ IRect* rectangle)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteIIWindow(data, IIWindow::Probe(token));
    if (rectangle != NULL) {
        AndroidParcelUtils::WriteInt32(data, 1);
        AndroidParcelUtils::WriteRect(data, rectangle);
    }
    else {
        AndroidParcelUtils::WriteInt32(data, 0);
    }
    mRemote->transact(TRANSACTION_onRectangleOnScreenRequested, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

ECode IWindowSessionProxy::GetWindowId(
    /* [in] */ IBinder* window,
    /* [out] */ IIWindowId** windowId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
