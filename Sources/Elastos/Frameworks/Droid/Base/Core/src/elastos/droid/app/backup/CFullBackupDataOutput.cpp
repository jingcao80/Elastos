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

#include "elastos/droid/app/backup/CFullBackupDataOutput.h"
#include "elastos/droid/app/backup/CBackupDataOutput.h"

using Elastos::Droid::App::Backup::CBackupDataOutput;
using Elastos::Droid::App::Backup::IBackupDataOutput;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CAR_INTERFACE_IMPL(CFullBackupDataOutput, Object, IFullBackupDataOutput)

CAR_OBJECT_IMPL(CFullBackupDataOutput)

ECode CFullBackupDataOutput::constructor(
    /* [in] */ IParcelFileDescriptor* fdes)
{
    assert(fdes != NULL);
    AutoPtr<IFileDescriptor>  fd;
    fdes->GetFileDescriptor((IFileDescriptor**)&fd);
    return CBackupDataOutput::New(fd, (IBackupDataOutput**)&mData);
}

ECode CFullBackupDataOutput::GetData(
    /* [out] */ IBackupDataOutput** data)
{
    *data = mData;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
