
#include "elastos/droid/app/backup/CBackupTransport.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CAR_INTERFACE_IMPL(CBackupTransport, Object, IBackupTransport)

CAR_OBJECT_IMPL(CBackupTransport)

ECode CBackupTransport::constructor()
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::GetBinder(
    /* [out] */ IBinder** binder)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::GetName(
    /* [out] */ String* name)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::GetConfigurationIntent(
    /* [out] */ IIntent** intent)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::GetCurrentDestinationString(
    /* [out] */ String* str)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::GetDataManagementIntent(
    /* [out] */ IIntent** intent)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::GetDataManagementLabel(
    /* [out] */ String* label)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::GetTransportDirName(
    /* [out] */ String* name)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::InitializeDevice(
    /* [out] */ Int32* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::ClearBackupData(
    /* [in] */ IPackageInfo* packageInfo,
    /* [out] */ Int32* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::FinishBackup(
    /* [out] */ Int32* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::RequestBackupTime(
    /* [out] */ Int64* time)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::PerformBackup(
    /* [in] */ IPackageInfo* packageInfo,
    /* [in] */ IParcelFileDescriptor* inFd,
    /* [out] */ Int32* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::GetAvailableRestoreSets(
    /* [out, callee] */ ArrayOf<IRestoreSet*>** sets)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::GetCurrentRestoreSet(
    /* [out] */ Int64* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::StartRestore(
    /* [in] */ Int64 token,
    /* [in] */ ArrayOf<IPackageInfo*>* packages,
    /* [out] */ Int32* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::GetNextRestorePackage(
    /* [out] */ IRestoreDescription** rd)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::GetRestoreData(
    /* [in] */ IParcelFileDescriptor* outFd,
    /* [out] */ Int32* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::FinishRestore()
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::RequestFullBackupTime(
    /* [out] */ Int64* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::PerformFullBackup(
    /* [in] */ IPackageInfo* targetPackage,
    /* [in] */ IParcelFileDescriptor* socket,
    /* [out] */ Int32* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::SendBackupData(
    /* [in] */ Int32 numBytes,
    /* [out] */ Int32* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::CancelFullBackup()
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::GetNextFullRestoreDataChunk(
    /* [in] */ IParcelFileDescriptor* socket,
    /* [out] */ Int32* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CBackupTransport::AbortFullRestore(
    /* [out] */ Int32* result)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
