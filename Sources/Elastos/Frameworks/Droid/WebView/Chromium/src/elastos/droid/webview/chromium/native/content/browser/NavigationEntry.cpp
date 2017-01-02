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

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/NavigationEntry.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
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
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
