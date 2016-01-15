
#ifndef __ELASTOS_DROID_WEBKIT_COOKIESYNCMANAGER_H__
#define __ELASTOS_DROID_WEBKIT_COOKIESYNCMANAGER_H__

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/WebSyncManager.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * The CookieSyncManager is used to synchronize the browser cookie store
 * between RAM and permanent storage. To get the best performance, browser cookies are
 * saved in RAM. A separate thread saves the cookies between, driven by a timer.
 * <p>
 *
 * To use the CookieSyncManager, the host application has to call the following
 * when the application starts:
 * <p>
 *
 * <pre class="prettyprint">CookieSyncManager.createInstance(context)</pre><p>
 *
 * To set up for sync, the host application has to call<p>
 * <pre class="prettyprint">CookieSyncManager.getInstance().startSync()</pre><p>
 *
 * in Activity.onResume(), and call
 * <p>
 *
 * <pre class="prettyprint">
 * CookieSyncManager.getInstance().stopSync()
 * </pre><p>
 *
 * in Activity.onPause().<p>
 *
 * To get instant sync instead of waiting for the timer to trigger, the host can
 * call
 * <p>
 * <pre class="prettyprint">CookieSyncManager.getInstance().sync()</pre><p>
 *
 * The sync interval is 5 minutes, so you will want to force syncs
 * manually anyway, for instance in {@link
 * WebViewClient#onPageFinished}. Note that even sync() happens
 * asynchronously, so don't do it just as your activity is shutting
 * down.
 */
class CookieSyncManager
    : public ICookieSyncManager
    , public WebSyncManager
{
    friend class CCookieSyncManagerHelper;
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor();

    /**
     * sync() forces sync manager to sync now
     * @deprecated Use {@link CookieManager#flush} instead.
     */
    //@Deprecated
    CARAPI Sync();

    /**
     * resetSync() resets sync manager's timer.
     * @deprecated Calling resetSync is no longer necessary as the WebView automatically
     *             syncs cookies.
     */
    //@Deprecated
    CARAPI ResetSync();

    /**
     * startSync() requests sync manager to start sync.
     * @deprecated Calling startSync is no longer necessary as the WebView automatically
     *             syncs cookies.
     */
    //@Deprecated
    CARAPI StartSync();

    /**
     * stopSync() requests sync manager to stop sync. remove any SYNC_MESSAGE in
     * the queue to break the sync loop
     * @deprecated Calling stopSync is no longer useful as the WebView
     *             automatically syncs cookies.
     */
    //@Deprecated
    CARAPI StopSync();

    CARAPI ToString(
        /* [out] */ String* info);

    static CARAPI SetGetInstanceIsAllowed();

protected:
    CookieSyncManager();

    CARAPI_(void) SyncFromRamToFlash();

private:
    static CARAPI CheckInstanceIsAllowed();

private:
    static AutoPtr<ICookieSyncManager> sRef;
    static Boolean sGetInstanceAllowed;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_COOKIESYNCMANAGER_H__
