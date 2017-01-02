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

#include "elastos/droid/webkit/webview/chromium/native/components/NavigationParams.h"
#include "elastos/droid/webkit/webview/chromium/native/components/api/NavigationParams_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Components {

//=====================================================================
//                           NavigationParams
//=====================================================================
NavigationParams::NavigationParams(
    /* [in] */ const String& url1,
    /* [in] */ Boolean isPost1,
    /* [in] */ Boolean hasUserGesture1,
    /* [in] */ Int32 pageTransitionType1,
    /* [in] */ Boolean isRedirect1)
    : url(url1)
    , isPost(isPost1)
    , hasUserGesture(hasUserGesture1)
    , pageTransitionType(pageTransitionType1)
    , isRedirect(isRedirect1)
{
    // ==================before translated======================
    // this.url = url;
    // this.isPost = isPost;
    // this.hasUserGesture = hasUserGesture;
    // this.pageTransitionType = pageTransitionType;
    // this.isRedirect = isRedirect;
}

AutoPtr<IInterface> NavigationParams::Create(
    /* [in] */ const String& url,
    /* [in] */ Boolean isPost,
    /* [in] */ Boolean hasUserGesture,
    /* [in] */ Int32 pageTransitionType,
    /* [in] */ Boolean isRedirect)
{
    // ==================before translated======================
    // return new NavigationParams(url, isPost, hasUserGesture, pageTransitionType,
    //         isRedirect);

    AutoPtr<NavigationParams> result = new NavigationParams(url, isPost, hasUserGesture, pageTransitionType, isRedirect);;
    return TO_IINTERFACE(result);
}

} // namespace Components
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

