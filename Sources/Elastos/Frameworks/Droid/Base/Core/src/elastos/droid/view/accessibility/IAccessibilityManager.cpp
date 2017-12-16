
#include "elastos/droid/view/accessibility/IAccessibilityManager.h"
#include "elastos/droid/os/AndroidParcelUtils.h"
#include <binder/Parcel.h>

using Elastos::Droid::Os::AndroidParcelUtils;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

const String IAccessibilityManagerProxy::DESCRIPTOR("android.view.accessibility.IAccessibilityManager");
const Int32 IAccessibilityManagerProxy::TRANSACTION_sendAccessibilityEvent = android::IBinder::FIRST_CALL_TRANSACTION + 1;
const Int32 IAccessibilityManagerProxy::TRANSACTION_addClient = android::IBinder::FIRST_CALL_TRANSACTION + 2;
const Int32 IAccessibilityManagerProxy::TRANSACTION_addAccessibilityInteractionConnection = android::IBinder::FIRST_CALL_TRANSACTION + 5;
const Int32 IAccessibilityManagerProxy::TRANSACTION_removeAccessibilityInteractionConnection = android::IBinder::FIRST_CALL_TRANSACTION + 6;

CAR_INTERFACE_IMPL(IAccessibilityManagerProxy, Object, IIAccessibilityManager);

IAccessibilityManagerProxy::IAccessibilityManagerProxy(
    /* [in] */ android::sp<android::IBinder>& am)
    : mAm(am)
{}

ECode IAccessibilityManagerProxy::AddClient(
    /* [in] */ IIAccessibilityManagerClient* client,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteIIAccessibilityManagerClient(data, client);
    AndroidParcelUtils::WriteInt32(data, userId);
    mAm->transact(TRANSACTION_addClient, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    *result = (Int32)AndroidParcelUtils::ReadInt64(reply);
    return ec;
}

ECode IAccessibilityManagerProxy::SendAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* uiEvent,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    android::Parcel data;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    if (uiEvent != NULL) {
        AndroidParcelUtils::WriteInt32(data, 1);
        AndroidParcelUtils::WriteAccessibilityEvent(data, uiEvent);
    }
    else {
        AndroidParcelUtils::WriteInt32(data, 0);
    }
    AndroidParcelUtils::WriteInt32(data, userId);
    mAm->transact(TRANSACTION_sendAccessibilityEvent, data, NULL, android::IBinder::FLAG_ONEWAY);
    *result = TRUE;
    return NOERROR;
}

ECode IAccessibilityManagerProxy::GetInstalledAccessibilityServiceList(
    /* [in] */ Int32 userId,
    /* [out] */ IList** services)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAccessibilityManagerProxy::GetEnabledAccessibilityServiceList(
    /* [in] */ Int32 feedbackType,
    /* [in] */ Int32 userId,
    /* [out] */ IList** services)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAccessibilityManagerProxy::Interrupt(
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAccessibilityManagerProxy::AddAccessibilityInteractionConnection(
    /* [in] */ IIWindow* windowToken,
    /* [in] */ IIAccessibilityInteractionConnection* connection,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteIIWindow(data, windowToken);
    AndroidParcelUtils::WriteIIAccessibilityInteractionConnection(data, connection);
    AndroidParcelUtils::WriteInt32(data, userId);
    mAm->transact(TRANSACTION_addAccessibilityInteractionConnection, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    *result = AndroidParcelUtils::ReadInt32(reply);
    return ec;
}

ECode IAccessibilityManagerProxy::RemoveAccessibilityInteractionConnection(
    /* [in] */ IIWindow* windowToken)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteIIWindow(data, windowToken);
    mAm->transact(TRANSACTION_removeAccessibilityInteractionConnection, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

ECode IAccessibilityManagerProxy::RegisterUiTestAutomationService(
    /* [in] */ IBinder* owner,
    /* [in] */ IIAccessibilityServiceClient* client,
    /* [in] */ IAccessibilityServiceInfo* info)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAccessibilityManagerProxy::UnregisterUiTestAutomationService(
    /* [in] */ IIAccessibilityServiceClient* client)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAccessibilityManagerProxy::TemporaryEnableAccessibilityStateUntilKeyguardRemoved(
    /* [in] */ IComponentName* service,
    /* [in] */ Boolean touchExplorationEnabled)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IAccessibilityManagerProxy::GetWindowToken(
    /* [in] */ Int32 windowId,
    /* [out] */ IBinder** binder)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace Accessibility
} // namespace View
} // namespace Droid
} // namespace Accessibility
