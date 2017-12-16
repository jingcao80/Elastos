
#include "elastos/droid/content/IContentService.h"
#include "elastos/droid/os/AndroidParcelUtils.h"

using Elastos::Droid::Os::AndroidParcelUtils;

namespace Elastos {
namespace Droid {
namespace Content {

const String IContentServiceProxy::DESCRIPTOR("android.content.IContentService");
const Int32 IContentServiceProxy::TRANSACTION_unregisterContentObserver = android::IBinder::FIRST_CALL_TRANSACTION + 0;
const Int32 IContentServiceProxy::TRANSACTION_registerContentObserver = android::IBinder::FIRST_CALL_TRANSACTION + 1;

CAR_INTERFACE_IMPL(IContentServiceProxy, Object, IIContentService);

IContentServiceProxy::IContentServiceProxy(
    /* [in] */ android::sp<android::IBinder> remote)
    : mRemote(remote)
{}

ECode IContentServiceProxy::UnregisterContentObserver(
    /* [in] */ IIContentObserver* observer)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteIIContentObserver(data, observer);
    mRemote->transact(TRANSACTION_unregisterContentObserver, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

ECode IContentServiceProxy::RegisterContentObserver(
    /* [in] */ IUri* uri,
    /* [in] */ Boolean notifyForDescendants,
    /* [in] */ IIContentObserver* observer,
    /* [in] */ Int32 userHandle)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    if (uri != NULL) {
        AndroidParcelUtils::WriteInt32(data, 1);
        AndroidParcelUtils::WriteUri(data, uri);
    }
    else {
        AndroidParcelUtils::WriteInt32(data, 0);
    }
    AndroidParcelUtils::WriteInt32(data, notifyForDescendants? 1 : 0);
    AndroidParcelUtils::WriteIIContentObserver(data, observer);
    AndroidParcelUtils::WriteInt32(data, userHandle);
    Int32 targetSdkVersion = 0;
    AndroidParcelUtils::WriteInt32(data, targetSdkVersion);
    mRemote->transact(TRANSACTION_registerContentObserver, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

ECode IContentServiceProxy::NotifyChange(
    /* [in] */ IUri* uri,
    /* [in] */ IIContentObserver* observer,
    /* [in] */ Boolean observerWantsSelfNotifications,
    /* [in] */ Boolean syncToNetwork,
    /* [in] */ Int32 userHandle)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::RequestSync(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IBundle* extras)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::Sync(
    /* [in] */ ISyncRequest* request)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::SyncAsUser(
    /* [in] */ ISyncRequest* request,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::CancelSync(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IComponentName* cname)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::CancelSyncAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IComponentName* cname,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::CancelRequest(
    /* [in] */ ISyncRequest* request)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::GetSyncAutomatically(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::GetSyncAutomaticallyAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::SetSyncAutomatically(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [in] */ Boolean sync)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::SetSyncAutomaticallyAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [in] */ Boolean sync,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::GetPeriodicSyncs(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [out] */ IList** periodicSyncList)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::AddPeriodicSync(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [in] */ IBundle* extras,
    /* [in] */ Int64 pollFrequency)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::RemovePeriodicSync(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [in] */ IBundle* extras)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::GetIsSyncable(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [out] */ Int32* value)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::GetIsSyncableAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* value)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::SetIsSyncable(
    /* [in] */ IAccount* account,
    /* [in] */ const String& providerName,
    /* [in] */ Int32 syncable)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::SetMasterSyncAutomatically(
    /* [in] */ Boolean flag)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::SetMasterSyncAutomaticallyAsUser(
    /* [in] */ Boolean flag,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::GetMasterSyncAutomatically(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::GetMasterSyncAutomaticallyAsUser(
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::GetCurrentSyncs(
    /* [out] */ IList** syncInfoList)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::GetCurrentSyncsAsUser(
    /* [in] */ Int32 userId,
    /* [out] */ IList** syncInfoList)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::GetSyncAdapterTypes(
    /* [out, callee] */ ArrayOf<ISyncAdapterType*>** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::GetSyncAdapterTypesAsUser(
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<ISyncAdapterType*>** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::IsSyncActive(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IComponentName* name,
    /* [out] */ Boolean* isActive)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::GetSyncStatus(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IComponentName* name,
    /* [out] */ ISyncStatusInfo** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::GetSyncStatusAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 userId,
    /* [out] */ ISyncStatusInfo** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::IsSyncPending(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IComponentName* name,
    /* [out] */ Boolean* isPending)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::IsSyncPendingAsUser(
    /* [in] */ IAccount* account,
    /* [in] */ const String& authority,
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* isPending)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::AddStatusChangeListener(
    /* [in] */ Int32 mask,
    /* [in] */ IISyncStatusObserver* observer)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IContentServiceProxy::RemoveStatusChangeListener(
    /* [in] */ IISyncStatusObserver* observer)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace Content
} // namespace Droid
} // namespace Elastos
