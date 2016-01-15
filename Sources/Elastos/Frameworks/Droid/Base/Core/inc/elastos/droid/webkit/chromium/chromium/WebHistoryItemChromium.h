
#ifndef __ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBHISTORYITEMCHROMIUM_H__
#define __ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBHISTORYITEMCHROMIUM_H__

// import org.chromium.content.browser.NavigationEntry;

// import android.graphics.Bitmap;
// import android.webkit.WebHistoryItem;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

/**
 * WebView Chromium implementation of WebHistoryItem. Simple immutable wrapper
 * around NavigationEntry
 */
class WebHistoryItemChromium
    : public Object
    , public IWebHistoryItem
{
public:
    /* package */
    WebHistoryItemChromium(
        /* [in] */ NavigationEntry* entry);

    /**
     * See {@link android.webkit.WebHistoryItem#getId}.
     */
    //@Override
    CARAPI GetId(
        /* [out] */ Int32* id);

    /**
     * See {@link android.webkit.WebHistoryItem#getUrl}.
     */
    //@Override
    CARAPI GetUrl(
        /* [out] */ String* url);

    /**
     * See {@link android.webkit.WebHistoryItem#getOriginalUrl}.
     */
    //@Override
    CARAPI GetOriginalUrl(
        /* [out] */ String* url);

    /**
     * See {@link android.webkit.WebHistoryItem#getTitle}.
     */
    //@Override
    CARAPI GetTitle(
        /* [out] */ String* title);

    /**
     * See {@link android.webkit.WebHistoryItem#getFavicon}.
     */
    //@Override
    CARAPI GetFavicon(
        /* [out] */ IBitmap** bitMap);

    /**
     * See {@link android.webkit.WebHistoryItem#clone}.
     */
    //@Override
    //synchronized
    CARAPI Clone(
        /* [out] */ WebHistoryItemChromium** chromium);

private:
    // Clone constructor.
    WebHistoryItemChromium(
        /* [in] */ const String& url,
        /* [in] */ const String& originalUrl,
        /* [in] */ const String& title,
        /* [in] */ IBitmap* favicon);

private:
    const String mUrl;
    const String mOriginalUrl;
    const String mTitle;
    const AutoPtr<IBitmap> mFavicon;
};

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBHISTORYITEMCHROMIUM_H__
