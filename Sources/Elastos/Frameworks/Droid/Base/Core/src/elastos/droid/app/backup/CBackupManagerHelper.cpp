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

#include "elastos/droid/app/backup/CBackupManagerHelper.h"
#include "elastos/droid/app/backup/CBackupManager.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CAR_INTERFACE_IMPL(CBackupManagerHelper, Singleton, IBackupManagerHelper)

CAR_SINGLETON_IMPL(CBackupManagerHelper)

ECode CBackupManagerHelper::DataChanged(
    /* [in] */ const String& packageName)
{
    return CBackupManager::DataChanged(packageName);
}

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
