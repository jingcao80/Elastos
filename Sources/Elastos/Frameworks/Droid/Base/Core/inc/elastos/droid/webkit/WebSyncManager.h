
#ifndef __ELASTOS_DROID_WEBKIT_WEBSYNCMANAGER_H__
#define __ELASTOS_DROID_WEBKIT_WEBSYNCMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Core::Runnable;
using Elastos::Core::IRunnable;
using Elastos::Core::IThread;

namespace Elastos {
namespace Droid {
namespace Webkit {

class WebViewDatabase;

class WebSyncManager : public Runnable
{
public:
    CARAPI Run();

    /**
     * sync() forces sync manager to sync now
     */
    virtual CARAPI Sync();

    /**
     * resetSync() resets sync manager's timer
     */
    virtual CARAPI ResetSync();

    /**
     * startSync() requests sync manager to start sync
     */
    virtual CARAPI StartSync();

    /**
     * stopSync() requests sync manager to stop sync. remove any SYNC_MESSAGE in
     * the queue to break the sync loop
     */
    virtual CARAPI StopSync();

    virtual CARAPI_(void) SyncFromRamToFlash() = 0;

    CARAPI ToString(
        /* [out] */ String* info);

protected:
    WebSyncManager();

    WebSyncManager(
        /* [in] */ IContext* context,
        /* [in] */ const String& name);

    virtual CARAPI_(AutoPtr<IInterface>) Clone();

    virtual CARAPI_(void) OnSyncInit();

protected:
    static const String LOGTAG;
    AutoPtr<IWebViewDatabase> mDataBase;
    AutoPtr<IHandler> mHandler;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_WEBSYNCMANAGER_H__
