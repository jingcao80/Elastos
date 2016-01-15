
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_APP_CONTENTAPPLICATION_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_APP_CONTENTAPPLICATION_H__

#include "Elastos.Droid.Os.h"
#include "elastos/droid/webkit/native/base/BaseChromiumApplication.h"

using Elastos::Droid::Os::IIdleHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::Webkit::Base::BaseChromiumApplication;

// import org.chromium.base.BaseChromiumApplication;
// import org.chromium.base.library_loader.LibraryLoader;
// import org.chromium.content.browser.TracingControllerAndroid;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

class TracingControllerElastos;

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

using Elastos::Droid::Webkit::Content::Browser::TracingControllerElastos;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace App {

/**
 * Basic application functionality that should be shared among all browser applications
 * based on the content layer.
 */
class ContentApplication : public BaseChromiumApplication
{
private:
    class InnerIdleHandler
        : public Object
        , public IIdleHandler
    {
    public:
        CAR_INTERFACE_DECL();

        InnerIdleHandler(
            /* [in] */ ContentApplication* owner);

        CARAPI QueueIdle(
            /* [out] */ Boolean* result);

    private:
        ContentApplication* mOwner;
    };

public:
    CARAPI_(AutoPtr<TracingControllerElastos>) GetTracingController();

    //@Override
    CARAPI OnCreate();

    /**
     * For emulated process environment only. On a production device, the application process is
     * simply killed without calling this method. We don't need to unregister the broadcast
     * receiver in the latter case.
     */
    //@Override
    CARAPI OnTerminate();

private:
    AutoPtr<TracingControllerElastos> mTracingController;
};

} // namespace App
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_APP_CONTENTAPPLICATION_H__
