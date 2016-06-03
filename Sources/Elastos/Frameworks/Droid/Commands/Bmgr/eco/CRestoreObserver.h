
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
