#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWBROWSERPROCESS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWBROWSERPROCESS_H__

#include <Elastos.CoreLibrary.Core.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::IRunnable;

// import org.chromium.base.library_loader.LibraryLoader;
// import org.chromium.base.library_loader.ProcessInitException;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * Wrapper for the steps needed to initialize the java and native sides of webview chromium.
 */
class AwBrowserProcess
{
public:
    class InnerRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL();
        InnerRunnable(
            /* [in] */ IContext* context);

        CARAPI Run();

    private:
        AwBrowserProcess* mOwner;
        AutoPtr<IContext> mContext;
    };

public:
    /**
     * Loads the native library, and performs basic static construction of objects needed
     * to run webview in this process. Does not create threads; safe to call from zygote.
     * Note: it is up to the caller to ensure this is only called once.
     */
    static CARAPI_(void) LoadLibrary();

    /**
     * Starts the chromium browser process running within this process. Creates threads
     * and performs other per-app resource allocations; must not be called from zygote.
     * Note: it is up to the caller to ensure this is only called once.
     * @param context The Android application context
     */
    static CARAPI_(void) Start(
        /* [in] */ IContext* context);

private:
    static const String PRIVATE_DATA_DIRECTORY_SUFFIX;
    static const String TAG;

    static CARAPI_(void) InitializePlatformKeySystem();
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWBROWSERPROCESS_H__
