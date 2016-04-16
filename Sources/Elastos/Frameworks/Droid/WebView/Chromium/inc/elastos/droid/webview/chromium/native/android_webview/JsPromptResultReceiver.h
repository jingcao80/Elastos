#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_JSPROMPTRESULTRECEIVER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_JSPROMPTRESULTRECEIVER_H__
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * This interface is used when the AwContentsClient offers a JavaScript
 * modal prompt dialog  to enable the client to handle the dialog in their own way.
 * AwContentsClient will offer an object that implements this interface to the
 * client and when the client has handled the dialog, it must either callback with
 * confirm() or cancel() to allow processing to continue.
 */
class JsPromptResultReceiver
    : public IInterface
{
public:
    virtual CARAPI Confirm(
        /* [in] */ const String& result) = 0;

    virtual CARAPI Cancel() = 0;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_JSPROMPTRESULTRECEIVER_H__
