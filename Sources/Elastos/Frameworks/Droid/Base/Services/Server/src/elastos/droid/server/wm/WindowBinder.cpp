
#include <Elastos.Droid.Content.h>
#include "elastos/droid/server/wm/WindowBinder.h"
#include <elastos/utility/logging/Slogger.h>
#include <binder/Binder.h>
#include <binder/IServiceManager.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

WindowBinder::WindowBinder()
{
    mNativeBinder = new android::BBinder();
    mNativeBinder->incStrong(NULL);
}

WindowBinder::~WindowBinder()
{
    assert(mNativeBinder != NULL);
    mNativeBinder->decStrong(NULL);
}

ECode WindowBinder::Register()
{
    assert(mNativeBinder != NULL);
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    int res = sm->addService(android::String16(IContext::WINDOW_SERVICE.string()), mNativeBinder);
    if (res != 0) {
        Slogger::E("WindowBinder", "add service window failed");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Wm
} // namespace Server
} // namespace Droid
} // namespace Elastos