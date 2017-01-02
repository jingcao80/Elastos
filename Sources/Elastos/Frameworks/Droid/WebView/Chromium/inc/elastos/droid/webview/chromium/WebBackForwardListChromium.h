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

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBBACKFORWARDLISTCHROMIUM_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBBACKFORWARDLISTCHROMIUM_H_

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/NavigationHistory.h"

using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::NavigationHistory;
using Elastos::Droid::Webkit::IWebBackForwardList;
using Elastos::Droid::Webkit::IWebHistoryItem;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

/**
  * WebView Chromium implementation of WebBackForwardList. Simple immutable
  * wrapper around NavigationHistory.
  */
class WebBackForwardListChromium
    : public Object
    , public IWebBackForwardList
{
public:
    CAR_INTERFACE_DECL()

    /* package */
    WebBackForwardListChromium(
        /* [in] */ NavigationHistory* nav_history);

    /**
      * See {@link android.webkit.WebBackForwardList#getCurrentItem}.
      */
    // @Override
    // synchronized
    CARAPI GetCurrentItem(
        /* [out] */ IWebHistoryItem** item);

    /**
      * See {@link android.webkit.WebBackForwardList#getCurrentIndex}.
      */
    // @Override
    // synchronized
    CARAPI GetCurrentIndex(
        /* [out] */ Int32* index);

    /**
      * See {@link android.webkit.WebBackForwardList#getItemAtIndex}.
      */
    // @Override
    // synchronized
    CARAPI GetItemAtIndex(
        /* [in] */ Int32 index,
        /* [out] */ IWebHistoryItem** item);

    /**
      * See {@link android.webkit.WebBackForwardList#getSize}.
      */
    // @Override
    // synchronized
    CARAPI GetSize(
        /* [out] */ Int32* size);

protected:
    /**
      * See {@link android.webkit.WebBackForwardList#clone}.
      */
    // @Override
    // synchronized
    CARAPI Clone(
        /* [out] */ WebBackForwardListChromium** chromium);

private:
    // Clone constructor.
    WebBackForwardListChromium(
        /* [in] */ IList* list,
        /* [in] */ Int32 currentIndex);

private:
    /*const*/ AutoPtr<IList> mHistroryItemList;
    /*const*/ Int32 mCurrentIndex;
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBBACKFORWARDLISTCHROMIUM_H_

