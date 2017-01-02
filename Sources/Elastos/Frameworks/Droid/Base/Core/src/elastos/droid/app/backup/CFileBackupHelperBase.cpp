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

#include "elastos/droid/app/backup/CFileBackupHelperBase.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CAR_OBJECT_IMPL(CFileBackupHelperBase)

CFileBackupHelperBase::~CFileBackupHelperBase()
{
}

ECode CFileBackupHelperBase::constructor(
    /* [in] */ IContext* constext)
{
    return FileBackupHelperBase::Init(constext);
}

ECode CFileBackupHelperBase::WriteNewStateDescription(
    /* [in] */ IParcelFileDescriptor* fd)
{

    return FileBackupHelperBase::WriteNewStateDescription(fd);
}

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
