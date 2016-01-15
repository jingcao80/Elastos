// wuweizuo automatic build .h file from .java file.
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_SMARTCLIPPROVIDER_H_
#define _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_SMARTCLIPPROVIDER_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

/**
  * An interface to provide smart clip data when requested.
  *
  * NOTE: Some platforms may call these functions to extract smart clip data.
  * Please make sure implementation of them is somewhere in the view
  * hierarchy.
  */
class SmartClipProvider : public Object
{
public:
    /**
      * Initiate extraction of text, HTML, and other information for clipping puposes (smart clip)
      * from the rectangle area defined by starting positions (x and y), and width and height.
      */
    virtual CARAPI ExtractSmartClipData(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height) = 0;

    /**
      * Register a handler to handle smart clip data once extraction is done.
      */
    virtual CARAPI SetSmartClipResultHandler(
        /* [in] */ const IHandler* resultHandler) = 0;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_SMARTCLIPPROVIDER_H_
