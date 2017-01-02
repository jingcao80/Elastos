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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_PUBLIC_REFERRER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_PUBLIC_REFERRER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content_Public {

/**
 * Container that holds together a referrer URL along with the referrer policy set on the
 * originating frame. This corresponds to native content/public/common/referrer.h.
 */
class Referrer : public Object
{
public:
    Referrer(
        /* [in] */ const String& url,
        /* [in] */ Int32 policy);

    CARAPI_(String) GetUrl();

    CARAPI_(Int32) GetPolicy();

private:
    const String mUrl;
    const Int32 mPolicy;
};

} // namespace Content_Public
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_PUBLIC_REFERRER_H__
