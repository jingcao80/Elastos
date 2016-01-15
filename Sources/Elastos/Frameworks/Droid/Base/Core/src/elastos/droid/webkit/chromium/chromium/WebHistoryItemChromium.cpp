
namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

/* package */
WebHistoryItemChromium::WebHistoryItemChromium(
    /* [in] */ NavigationEntry* entry)
{
    mUrl = entry->GetUrl();
    mOriginalUrl = entry->GetOriginalUrl();
    mTitle = entry->GetTitle();
    mFavicon = entry->GetFavicon();
}

// Clone constructor.
WebHistoryItemChromium::WebHistoryItemChromium(
    /* [in] */ const String& url,
    /* [in] */ const String& originalUrl,
    /* [in] */ const String& title,
    /* [in] */ IBitmap* favicon)
    : mUrl(url)
    , mOriginalUrl(originalUrl)
    , mTitle(title)
    , mFavicon(favicon)
{
}

/**
 * See {@link android.webkit.WebHistoryItem#getId}.
 */
//@Override
ECode WebHistoryItemChromium::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    // This method is deprecated in superclass. Returning constant -1 now.
    *id = -1;
    return NOERROR;
}

/**
 * See {@link android.webkit.WebHistoryItem#getUrl}.
 */
//@Override
ECode WebHistoryItemChromium::GetUrl(
    /* [out] */ String* url)
{
    VALIDATE_NOT_NULL(url);
    *url = mUrl;
    return NOERROR;
}

/**
 * See {@link android.webkit.WebHistoryItem#getOriginalUrl}.
 */
//@Override
ECode WebHistoryItemChromium::GetOriginalUrl(
    /* [out] */ String* url)
{
    VALIDATE_NOT_NULL(url);
    *url = mOriginalUrl;
    return NOERROR;
}

/**
 * See {@link android.webkit.WebHistoryItem#getTitle}.
 */
//@Override
ECode WebHistoryItemChromium::GetTitle(
    /* [out] */ String* title)
{
    VALIDATE_NOT_NULL(title);
    *title = mTitle;
    return NOERROR;
}

/**
 * See {@link android.webkit.WebHistoryItem#getFavicon}.
 */
//@Override
ECode WebHistoryItemChromium::GetFavicon(
    /* [out] */ IBitmap** bitMap)
{
    VALIDATE_NOT_NULL(bitMap);
    *bitMap = mFavicon;
    return NOERROR;
}

/**
 * See {@link android.webkit.WebHistoryItem#clone}.
 */
//@Override
//synchronized
ECode WebHistoryItemChromium::Clone(
    /* [out] */ WebHistoryItemChromium** chromium)
{
    VALIDATE_NOT_NULL(chromium);
    *chromium = new WebHistoryItemChromium(mUrl, mOriginalUrl, mTitle, mFavicon);
    return NOERROR;
}

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
