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
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_SMARTCLIPPROVIDER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_SMARTCLIPPROVIDER_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * An interface to provide smart clip data when requested.
  *
  * NOTE: Some platforms may call these functions to extract smart clip data.
  * Please make sure implementation of them is somewhere in the view
  * hierarchy.
  */
class SmartClipProvider
{
public:
    CAR_INTERFACE_DECL()
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
        /* [in] */ IHandler* resultHandler) = 0;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_SMARTCLIPPROVIDER_H_
