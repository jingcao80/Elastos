
#ifndef __ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBBACKFORWARDLISTCHROMIUM_H__
#define __ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBBACKFORWARDLISTCHROMIUM_H__

// import org.chromium.content.browser.NavigationHistory;

// import android.webkit.WebBackForwardList;
// import android.webkit.WebHistoryItem;

// import java.util.ArrayList;
// import java.util.List;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
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
    /* package */
    WebBackForwardListChromium(
        /* [in] */ NavigationHistory* nav_history);

    CAR_INTERFACE_DECL();

    /**
     * See {@link android.webkit.WebBackForwardList#getCurrentItem}.
     */
    //@Override
    //synchronized
    CARAPI GetCurrentItem(
        /* [out] */ IWebHistoryItem** item);

    /**
     * See {@link android.webkit.WebBackForwardList#getCurrentIndex}.
     */
    //@Override
    //synchronized
    CARAPI GetCurrentIndex(
        /* [out] */ Int32* index);

    /**
     * See {@link android.webkit.WebBackForwardList#getItemAtIndex}.
     */
    //@Override
    //synchronized
    CARAPI GetItemAtIndex(
        /* [in] */ Int32 index,
        /* [out] */ IWebHistoryItem** item);

    /**
     * See {@link android.webkit.WebBackForwardList#getSize}.
     */
    //@Override
    //synchronized
    CARAPI GetSize(
        /* [out] */ Int32* size);

protected:
    /**
     * See {@link android.webkit.WebBackForwardList#clone}.
     */
    //@Override
    //synchronized
    CARAPI Clone(
        /* [out] */ IWebBackForwardListChromium** chromium);

private:
    // Clone constructor.
    WebBackForwardListChromium(
        /* [in] */ List<WebHistoryItemChromium> list,
        /* [in] */ Int32 currentIndex);

private:
    const List<WebHistoryItemChromium> mHistroryItemList;
    const Int32 mCurrentIndex;
};

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBBACKFORWARDLISTCHROMIUM_H__
