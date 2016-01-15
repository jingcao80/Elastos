
#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBBACKFORWARDLISTCHROMIUM_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBBACKFORWARDLISTCHROMIUM_H_

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/content/browser/NavigationHistory.h"

using Elastos::Droid::Webkit::Content::Browser::NavigationHistory;
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

