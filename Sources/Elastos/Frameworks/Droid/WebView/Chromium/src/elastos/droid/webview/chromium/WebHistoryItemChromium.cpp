
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/webkit/webview/chromium/WebHistoryItemChromium.h"
#include "elastos/core/AutoLock.h"

using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

//=====================================================================
//                        WebHistoryItemChromium
//=====================================================================
CAR_INTERFACE_IMPL(WebHistoryItemChromium, Object, IWebHistoryItem)

WebHistoryItemChromium::WebHistoryItemChromium(
    /* [in] */ NavigationEntry* entry)
    : mUrl(entry->GetUrl())
    , mOriginalUrl(entry->GetOriginalUrl())
    , mTitle(entry->GetTitle())
    , mFavicon(entry->GetFavicon())
{
    // ==================before translated======================
    // mUrl = entry.getUrl();
    // mOriginalUrl = entry.getOriginalUrl();
    // mTitle = entry.getTitle();
    // mFavicon = entry.getFavicon();
}

ECode WebHistoryItemChromium::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    // ==================before translated======================
    // // This method is deprecated in superclass. Returning constant -1 now.
    // return -1;

    *id = -1;
    return NOERROR;
}

ECode WebHistoryItemChromium::GetUrl(
    /* [out] */ String* url)
{
    VALIDATE_NOT_NULL(url);
    // ==================before translated======================
    // return mUrl;

    *url = mUrl;
    return NOERROR;
}

ECode WebHistoryItemChromium::GetOriginalUrl(
    /* [out] */ String* url)
{
    VALIDATE_NOT_NULL(url);
    // ==================before translated======================
    // return mOriginalUrl;

    *url = mOriginalUrl;
    return NOERROR;
}

ECode WebHistoryItemChromium::GetTitle(
    /* [out] */ String* title)
{
    VALIDATE_NOT_NULL(title);
    // ==================before translated======================
    // return mTitle;

    *title = mTitle;
    return NOERROR;
}

ECode WebHistoryItemChromium::GetFavicon(
    /* [out] */ IBitmap** bitMap)
{
    VALIDATE_NOT_NULL(bitMap);
    // ==================before translated======================
    // return mFavicon;

    *bitMap = mFavicon;
    REFCOUNT_ADD(*bitMap);
    return NOERROR;
}

// synchronized
ECode WebHistoryItemChromium::Clone(
    /* [out] */ WebHistoryItemChromium** chromium)
{
    VALIDATE_NOT_NULL(chromium);
    // ==================before translated======================
    // return new WebHistoryItemChromium(mUrl, mOriginalUrl, mTitle, mFavicon);

    AutoLock lock(this);
    *chromium = new WebHistoryItemChromium(mUrl, mOriginalUrl, mTitle, mFavicon);
    REFCOUNT_ADD(*chromium);
    return NOERROR;
}

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
    // ==================before translated======================
    // mUrl = url;
    // mOriginalUrl = originalUrl;
    // mTitle = title;
    // mFavicon = favicon;
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


