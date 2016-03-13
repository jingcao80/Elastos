
#include "elastos/droid/webkit/webview/chromium/native/base/PathService.h"
#include "elastos/droid/webkit/webview/chromium/native/base/api/PathService_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

const Int32 PathService::DIR_MODULE;

// Prevent instantiation.
PathService::PathService()
{
}

void PathService::Override(
    /* [in] */ Int32 what,
    /* [in] */ const String& path)
{
    NativeOverride(what, path);
}

void PathService::NativeOverride(
    /* [in] */ Int32 what,
    /* [in] */ const String& path)
{
    Elastos_PathService_nativeOverride(what, path);
}

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
