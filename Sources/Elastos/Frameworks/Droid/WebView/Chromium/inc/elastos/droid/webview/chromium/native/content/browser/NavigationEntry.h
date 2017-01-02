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

// wuweizuo automatic build .h file from .java file.
// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_NAVIGATIONENTRY_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_NAVIGATIONENTRY_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * Represents one entry in the navigation history of a page.
  */
class NavigationEntry : public Object
{
  friend class ContentViewCore;
public:
    /**
      * @return The index into the navigation history that this entry represents.
      */
    virtual CARAPI_(Int32) GetIndex();

    /**
      * @return The actual URL of the page. For some about pages, this may be a
      *         scary data: URL or something like that. Use GetVirtualURL() for
      *         showing to the user.
      */
    virtual CARAPI_(String) GetUrl();

    /**
      * @return The virtual URL, when nonempty, will override the actual URL of
      *         the page when we display it to the user. This allows us to have
      *         nice and friendly URLs that the user sees for things like about:
      *         URLs, but actually feed the renderer a data URL that results in
      *         the content loading.
      *         <p/>
      *         GetVirtualURL() will return the URL to display to the user in all
      *         cases, so if there is no overridden display URL, it will return
      *         the actual one.
      */
    virtual CARAPI_(String) GetVirtualUrl();

    /**
      * @return The URL that caused this NavigationEntry to be created.
      */
    virtual CARAPI_(String) GetOriginalUrl();

    /**
      * @return The title as set by the page. This will be empty if there is no
      *         title set. The caller is responsible for detecting when there is
      *         no title and displaying the appropriate "Untitled" label if this
      *         is being displayed to the user.
      */
    virtual CARAPI_(String) GetTitle();

    /**
      * @return The favicon of the page. This may be null.
      */
    virtual CARAPI_(AutoPtr<IBitmap>) GetFavicon();

    /**
      * @param favicon The updated favicon to replace the existing one with.
      */
    virtual CARAPI UpdateFavicon(
        /* [in] */ IBitmap* favicon);

protected:
    /**
      * Default constructor.
      */
    NavigationEntry(
        /* [in] */ Int32 index,
        /* [in] */ const String& url,
        /* [in] */ const String& virtualUrl,
        /* [in] */ const String& originalUrl,
        /* [in] */ const String& title,
        /* [in] */ IBitmap* favicon);

private:
    const Int32 mIndex;
    const String mUrl;
    const String mOriginalUrl;
    const String mVirtualUrl;
    const String mTitle;
    AutoPtr<IBitmap> mFavicon;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_NAVIGATIONENTRY_H_
