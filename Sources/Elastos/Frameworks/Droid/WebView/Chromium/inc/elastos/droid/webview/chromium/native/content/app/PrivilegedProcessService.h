
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_APP_PRIVILEGEDPROCESSSERVICE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_APP_PRIVILEGEDPROCESSSERVICE_H__

#include "elastos/droid/webkit/webview/chromium/native/content/app/ChildProcessService.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace App {

/**
 * Privileged (unsandboxed) Services inherit from this class. We enforce the
 * privileged/sandboxed distinction by type-checking objects against this parent class.
 */
class PrivilegedProcessService : public ChildProcessService
{
};

} // namespace App
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_APP_PRIVILEGEDPROCESSSERVICE_H__
