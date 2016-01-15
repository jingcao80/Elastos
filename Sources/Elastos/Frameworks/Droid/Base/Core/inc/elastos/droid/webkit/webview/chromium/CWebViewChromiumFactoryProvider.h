
#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CCONTENTSETTINGSADAPTER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CCONTENTSETTINGSADAPTER_H_

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_Webkit_Webview_Chromium_CWebViewChromiumFactoryProvider.h"
#include "elastos/droid/webkit/webview/chromium/WebViewChromiumFactoryProvider.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

CarClass(CWebViewChromiumFactoryProvider)
    , public WebViewChromiumFactoryProvider
{
public:
    CAR_OBJECT_DECL()

    CARAPI HasStarted(
    	/* [out] */ Boolean* result);

	CARAPI StartYourEngines(
    	/* [in] */ Boolean onMainThread);

	CARAPI GetBrowserContext(
		/* [out] */ IInterface** result);
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CCONTENTSETTINGSADAPTER_H_

