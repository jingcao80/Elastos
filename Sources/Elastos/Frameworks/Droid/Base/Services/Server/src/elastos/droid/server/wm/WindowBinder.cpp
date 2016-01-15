
#include "wm/WindowBinder.h"
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
    android::BBinder* b = new android::BBinder();
    b->incStrong(NULL);
    mNativeBinder = reinterpret_cast<Int32>(b);
}

WindowBinder::~WindowBinder()
{
    android::BBinder* b = reinterpret_cast<android::BBinder*>(mNativeBinder);
    assert(b != NULL);
    b->decStrong(NULL);
}

ECode WindowBinder::Register()
{
    android::BBinder* b = reinterpret_cast<android::BBinder*>(mNativeBinder);
    assert(b != NULL);
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    int res = sm->addService(android::String16(IContext::WINDOW_SERVICE.string()), b);
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