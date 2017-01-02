//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CWEBVIEWCHROMIUMFACTORYPROVIDER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CWEBVIEWCHROMIUMFACTORYPROVIDER_H_

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

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CWEBVIEWCHROMIUMFACTORYPROVIDER_H_

