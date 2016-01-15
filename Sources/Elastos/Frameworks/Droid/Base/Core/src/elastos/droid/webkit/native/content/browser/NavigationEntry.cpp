
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/webkit/native/content/browser/NavigationEntry.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

//=====================================================================
//                           NavigationEntry
//=====================================================================

NavigationEntry::NavigationEntry(
    /* [in] */ Int32 index,
    /* [in] */ const String& url,
    /* [in] */ const String& virtualUrl,
    /* [in] */ const String& originalUrl,
    /* [in] */ const String& title,
    /* [in] */ IBitmap* favicon)
    : mIndex(index)
    , mUrl(url)
    , mOriginalUrl(originalUrl)
    , mVirtualUrl(virtualUrl)
    , mTitle(title)
    , mFavicon(favicon)
{
}

Int32 NavigationEntry::GetIndex()
{
    return mIndex;
}

String NavigationEntry::GetUrl()
{
    return mUrl;
}

String NavigationEntry::GetVirtualUrl()
{
    return mVirtualUrl;
}

String NavigationEntry::GetOriginalUrl()
{
    return mOriginalUrl;
}

String NavigationEntry::GetTitle()
{
    return mTitle;
}

AutoPtr<IBitmap> NavigationEntry::GetFavicon()
{
    return mFavicon;
}

ECode NavigationEntry::UpdateFavicon(
    /* [in] */ IBitmap* favicon)
{
    mFavicon = favicon;
    return NOERROR;
}

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
