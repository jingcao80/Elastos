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

#include "elastos/droid/app/backup/RestoreObserver.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

RestoreObserver::RestoreObserver()
{

}

RestoreObserver::~RestoreObserver()
{

}

void RestoreObserver::RestoreSetsAvailable(
    /* [in] */ ArrayOf<IRestoreSet*>* result)
{
}

void RestoreObserver::RestoreStarting(
    /* [in] */ Int32 numPackages)
{
}

void RestoreObserver::OnUpdate(
    /* [in] */ Int32 nowBeingRestored,
    /* [in] */ const String& currentPackage)
{
}

void RestoreObserver::RestoreFinished(
    /* [in] */ Int32 error)
{
}

}
}
}
}