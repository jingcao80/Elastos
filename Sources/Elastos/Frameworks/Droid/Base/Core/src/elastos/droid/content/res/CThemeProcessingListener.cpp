
#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/res/CThemeProcessingListener.h"
#include "elastos/droid/content/res/CThemeManager.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/etl/List.h>
#include <elastos/core/AutoLock.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

//==============================================================================
//                  CThemeProcessingListener::OnFinishedProcessingRunnable
//==============================================================================

ECode CThemeProcessingListener::OnFinishedProcessingRunnable::Run()
{
    synchronized (mHost->mProcessingListenersLock) {
        List<AutoPtr<IThemeProcessingListener> > listenersToRemove;
        HashSet<AutoPtr<IThemeProcessingListener> >::Iterator it = mHost->mProcessingListeners.Begin();
        for (; it != mHost->mProcessingListeners.End(); ++it) {
            // try {
            ECode ec = (*it)->OnFinishedProcessing(mPkgName);
            if (FAILED(ec)) {
                Logger::W(CThemeManager::TAG, "Unable to update theme change progress 0x%08x", ec);
                listenersToRemove.PushBack(*it);
            }
            // } catch (Throwable e) {
            //     Log.w(CThemeManager::TAG, "Unable to update theme change progress", e);
            //     listenersToRemove.add(listener);
            // }
        }
        if (listenersToRemove.Begin() != listenersToRemove.End()) {
            List<AutoPtr<IThemeProcessingListener> >::Iterator removeIt = listenersToRemove.Begin();
            for (; removeIt != listenersToRemove.End(); ++removeIt) {
                mHost->mProcessingListeners.Erase(*removeIt);
            }
        }
    }
    return NOERROR;
}


//==============================================================================
//                  CThemeProcessingListener
//==============================================================================

CAR_INTERFACE_IMPL(CThemeProcessingListener, Object, IIThemeProcessingListener)

CAR_OBJECT_IMPL(CThemeProcessingListener)

ECode CThemeProcessingListener::constructor(
    /* [in] */ Handle64 host)
{
    mHost = (CThemeManager*)host;
    return NOERROR;
}

ECode CThemeProcessingListener::OnFinishedProcessing(
    /* [in] */ const String& pkgName)
{
    AutoPtr<IRunnable> runnable = (IRunnable*)new OnFinishedProcessingRunnable(pkgName, mHost);
    Boolean result;
    return mHost->mHandler->Post(runnable, &result);
}

}
}
}
}
