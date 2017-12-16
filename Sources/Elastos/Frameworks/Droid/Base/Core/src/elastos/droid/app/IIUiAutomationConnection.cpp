
#include "elastos/droid/app/IIUiAutomationConnection.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(IIUiAutomationConnectionProxy, Object, IIUiAutomationConnection);

IIUiAutomationConnectionProxy::IIUiAutomationConnectionProxy(
    /* [in] */ const android::sp<android::IBinder>& bpBinder)
    : mBpBinder(bpBinder)
{}

ECode IIUiAutomationConnectionProxy::Connect(
    /* [in] */ IIAccessibilityServiceClient* client)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IIUiAutomationConnectionProxy::Disconnect()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IIUiAutomationConnectionProxy::InjectInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Boolean sync,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IIUiAutomationConnectionProxy::SetRotation(
    /* [in] */ Int32 rotation,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IIUiAutomationConnectionProxy::TakeScreenshot(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IBitmap** bitmap)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IIUiAutomationConnectionProxy::Shutdown()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IIUiAutomationConnectionProxy::ClearWindowContentFrameStats(
    /* [in] */ Int32 windowId,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IIUiAutomationConnectionProxy::GetWindowContentFrameStats(
    /* [in] */ Int32 windowId,
    /* [out] */ IWindowContentFrameStats** stats)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IIUiAutomationConnectionProxy::ClearWindowAnimationFrameStats()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IIUiAutomationConnectionProxy::GetWindowAnimationFrameStats(
    /* [out] */ IWindowAnimationFrameStats** stats)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IIUiAutomationConnectionProxy::ExecuteShellCommand(
    /* [in] */ const String& command,
    /* [in] */ IParcelFileDescriptor* fd)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
