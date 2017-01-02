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

#ifndef __CRESTOREOBSERVER_H__
#define __CRESTOREOBSERVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_CRestoreObserver.h"
#include <elastos.h>
#include <elastos/core/Object.h>

using namespace Elastos;
using Elastos::Droid::App::Backup::IRestoreSet;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Bmgr {

CarClass(CRestoreObserver)
{
public:
    CAR_OBJECT_DECL()

    CARAPI RestoreSetsAvailable(
        /* [in] */ ArrayOf<Elastos::Droid::App::Backup::IRestoreSet *> * pResult);

    CARAPI RestoreStarting(
        /* [in] */ Int32 numPackages);

    CARAPI OnUpdate(
        /* [in] */ Int32 nowBeingRestored,
        /* [in] */ const String& curentPackage);

    CARAPI RestoreFinished(
        /* [in] */ Int32 error);

    CARAPI ToString(
        /* [out] */ String * pStr);

    CARAPI WaitForCompletion();
private:
    Boolean mDone;
    AutoPtr<ArrayOf<IRestoreSet*> > mSet;
    Object mRestoreObserverlock;
};

}
}
}
}

#endif // __CRESTOREOBSERVER_H__
