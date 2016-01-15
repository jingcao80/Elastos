
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
