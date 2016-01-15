
#include "elastos/droid/webkit/native/content/app/ContentApplication.h"
#include "elastos/droid/webkit/native/content/browser/TracingControllerElastos.h"

using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Os::EIID_IIdleHandler;
// TODO using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::ILooperHelper;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace App {

//=====================================================================
//            ContentApplication::InnerIdleHandler
//=====================================================================

CAR_INTERFACE_IMPL(ContentApplication::InnerIdleHandler, Object, IIdleHandler);

ContentApplication::InnerIdleHandler::InnerIdleHandler(
    /* [in] */ ContentApplication* owner)
    : mOwner(owner)
{
}

ECode ContentApplication::InnerIdleHandler::QueueIdle(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // Will retry if the native library has not been initialized.
    assert(0);
    // TODO
    // if (!LibraryLoader::IsInitialized()) {
    //     *result = TRUE;
    //     return result;
    // }

    // try {
        AutoPtr<IContext> context = IContext::Probe(mOwner);
        mOwner->GetTracingController()->RegisterReceiver(context);
    // } catch (SecurityException e) {
    //     // Happens if the process is isolated. Ignore.
    // }

    // Remove the idle handler.
    *result = FALSE;

    return NOERROR;
}

//=====================================================================
//                     ContentApplication
//=====================================================================

AutoPtr<TracingControllerElastos> ContentApplication::GetTracingController()
{
    if (mTracingController == NULL) {
        AutoPtr<IContext> context = THIS_PROBE(IContext);
        mTracingController = new TracingControllerElastos(context);
    }
    return mTracingController;
}

//@Override
ECode ContentApplication::OnCreate()
{
    BaseChromiumApplication::OnCreate();

    // Delay TracingControllerAndroid.registerReceiver() until the main loop is idle.
    AutoPtr<ILooperHelper> helper;
    // TODO CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
    AutoPtr<IMessageQueue> queue;
    helper->GetMyQueue((IMessageQueue**)&queue);
    AutoPtr<IIdleHandler> handler = new InnerIdleHandler(this);
    queue->AddIdleHandler(handler);

    return NOERROR;
}

/**
 * For emulated process environment only. On a production device, the application process is
 * simply killed without calling this method. We don't need to unregister the broadcast
 * receiver in the latter case.
 */
//@Override
ECode ContentApplication::OnTerminate()
{
    // try {
        AutoPtr<IContext> context = THIS_PROBE(IContext);
        GetTracingController()->UnregisterReceiver(context);
    // } catch (SecurityException e) {
    //     // Happens if the process is isolated. Ignore.
    // }

    BaseChromiumApplication::OnTerminate();

    return NOERROR;
}

} // namespace App
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
