// wuweizuo automatic build .h file from .java file.
// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_NAVIGATIONENTRY_H_
#define _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_NAVIGATIONENTRY_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace Webkit {
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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_NAVIGATIONENTRY_H_
