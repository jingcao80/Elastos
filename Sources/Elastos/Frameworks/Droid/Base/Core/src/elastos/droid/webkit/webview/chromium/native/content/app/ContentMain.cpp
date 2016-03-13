
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/webview/chromium/native/content/app/ContentMain.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/ContentMain_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace App {

/**
 * Initialize application context in native side.
 **/
void ContentMain::InitApplicationContext(
    /* [in] */ IContext* context)
{
    NativeInitApplicationContext(context);
}

/**
 * Start the ContentMainRunner in native side.
 **/
Int32 ContentMain::Start()
{
    return NativeStart();
}

void ContentMain::NativeInitApplicationContext(
    /* [in] */ IContext* context)
{
    AutoPtr<IContext> c = IContext::Probe(context);
    Elastos_ContentMain_nativeInitApplicationContext(c);
}

Int32 ContentMain::NativeStart()
{
    return Elastos_ContentMain_nativeStart();
}

} // namespace App
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
