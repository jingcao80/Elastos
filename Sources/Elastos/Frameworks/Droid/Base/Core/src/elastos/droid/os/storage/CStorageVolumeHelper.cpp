//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
