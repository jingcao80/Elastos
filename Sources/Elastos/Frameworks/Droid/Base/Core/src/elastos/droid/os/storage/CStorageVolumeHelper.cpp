
#include "elastos/droid/os/storage/CStorageVolumeHelper.h"

namespace Elastos {
namespace Droid {
namespace Os {
namespace Storage {

ECode CStorageVolumeHelper::FromTemplate(
    /* [in] */ IStorageVolume* temp,
    /* [in] */ IFile* path,
    /* [in] */ IUserHandle* owner,
    /* [out] */ IStorageVolume** volume)
{
    VALIDATE_NOT_NULL(volume);

    AutoPtr<IStorageVolume> result;
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

    CStorageVolume::New(path, descriptionId, primary, removable, emulated, mtpReserveSpace,
            allowMassStorage, maxFileSize, owner, (IStorageVolume**)&result);

    *volume = result;
    REFCOUNT_ADD(*volume);
    return NOERROR;
}

} // namespace Storage
} // namespace Os
} // namepsace Droid
} // namespace Elastos
