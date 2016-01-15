
#ifndef __ELASTOS_DROID_APP_BACKUP_CRESTOREOBSERVERWRAPPER_H__
#define  __ELASTOS_DROID_APP_BACKUP_CRESTOREOBSERVERWRAPPER_H__

#include "_Elastos_Droid_App_Backup_CRestoreObserverWrapper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/HandlerBase.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::App::Backup::IRestoreObserver;
using Elastos::Droid::App::Backup::IRestoreSet;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::HandlerBase;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {
/*
 * We wrap incoming binder calls with a private class implementation that
 * redirects them into main-thread actions.  This serializes the restore
 * progress callbacks nicely within the usual main-thread lifecycle pattern.
 */
CarClass(CRestoreObserverWrapper)
{

private:
    class WrapperHandler: public HandlerBase
    {
    public:
        WrapperHandler(
            /* [in] */ IRestoreObserver* appObserver);

        HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        AutoPtr<IRestoreObserver> mObserver;
    };

public:
    CRestoreObserverWrapper();

    ~CRestoreObserverWrapper();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IRestoreObserver* appObserver);

    // Binder calls into this object just enqueue on the main-thread handler
    CARAPI RestoreSetsAvailable(
        /* [in] */ ArrayOf<IRestoreSet*>* result);

    CARAPI RestoreStarting(
        /* [in] */ Int32 numPackages);

    CARAPI OnUpdate(
        /* [in] */ Int32 nowBeingRestored,
        /* [in] */ const String& currentPackage);

    CARAPI RestoreFinished(
        /* [in] */ Int32 error);

private:
    AutoPtr<IHandler> mHandler;
    AutoPtr<IRestoreObserver> mAppObserver;

    static const Int32 MSG_RESTORE_STARTING;
    static const Int32 MSG_UPDATE;
    static const Int32 MSG_RESTORE_FINISHED;
    static const Int32 MSG_RESTORE_SETS_AVAILABLE;
};

}
}
}
}
#endif  //__ELASTOS_DROID_APP_BACKUP_CRESTOREOBSERVERWRAPPER_H__

