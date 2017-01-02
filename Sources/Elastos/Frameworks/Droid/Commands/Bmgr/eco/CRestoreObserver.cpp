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

#include "CRestoreObserver.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Bmgr {

ECode CRestoreObserver::RestoreSetsAvailable(
    /* [in] */ ArrayOf<Elastos::Droid::App::Backup::IRestoreSet *> * pResult)
{
    // TODO: Add your code here
    AutoLock lock(mRestoreObserverlock);
    mSet = pResult;
    mDone = TRUE;
    mRestoreObserverlock.NotifyAll();
    return NOERROR;
}

ECode CRestoreObserver::RestoreStarting(
    /* [in] */ Int32 numPackages)
{
    // TODO: Add your code here
    printf("Backup RestoreObserver::RestoreStarting[%d]\n", numPackages);
    return NOERROR;
}

ECode CRestoreObserver::OnUpdate(
    /* [in] */ Int32 nowBeingRestored,
    /* [in] */ const String& curentPackage)
{
    // TODO: Add your code here
    printf("Backup RestoreObserver::OnUpdate[%d-- %s]\n", nowBeingRestored, curentPackage.string());
    return NOERROR;
}

ECode CRestoreObserver::RestoreFinished(
    /* [in] */ Int32 error)
{
    // TODO: Add your code here
    printf("Backup RestoreObserver::RestoreFinished[%d]\n", error);
    {
        AutoLock lock(mRestoreObserverlock);
        mDone = TRUE;
        mRestoreObserverlock.NotifyAll();
    }
    return NOERROR;
}

ECode CRestoreObserver::ToString(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CRestoreObserver::WaitForCompletion()
{
    AutoLock lock(mRestoreObserverlock);
    while(!mDone) {
        mRestoreObserverlock.Wait();
    }

    printf("Backup RestoreObserver::completed\n");
    return NOERROR;
}

}
}
}
}
