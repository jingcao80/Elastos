
#include "elastos/droid/os/storage/CStorageVolumeHelper.h"

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

CAR_INTERFACE_IMPL(CStorageVolumeHelper, Singleton, IStorageVolumeHelper)
CAR_SINGLETON_IMPL(CStorageVolumeHelper)

ECode CStorageVolumeHelper::FromTemplate(
    /* [in] */ IStorageVolume* temp,
    /* [in] */ IFile* path,
    /* [in] */ IUserHandle* owner,
    /* [out] */ IStorageVolume** volume)
{
    VALIDATE_NOT_NULL(volume);

    Int32 descriptionId, mtpReserveSpace;
    Int64 maxFileSize;
    Boolean primary, removable, emulated, allowMassStorage;

    temp->GetDescriptionId(&descriptionId);
    temp->IsPrimary(&primary);
    temp->IsRemovable(&removable);
    temp->IsEmulated(&emulated);
    temp->GetMtpReserveSpace(&mtpReserveSpace);
    temp->AllowMassStorage(&allowMassStorage);
    temp->GetMaxFileSize(&maxFileSize);

    return CStorageVolume::New(path, descriptionId, primary, removable, emulated, mtpReserveSpace,
            allowMassStorage, maxFileSize, owner, volume);
}

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos
