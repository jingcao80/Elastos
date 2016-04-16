// wuweizuo automatic build .h file from .java file.
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_POSITIONOBSERVER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_POSITIONOBSERVER_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * Used to register listeners that can be notified of changes to the position of a view.
  */
class PositionObserver : public Object
{
public:
    class Listener : public Object
    {
    public:
        /**
          * Called during predraw if the position of the underlying view has changed.
          */
        virtual CARAPI OnPositionChanged(
            /* [in] */ Int32 positionX,
            /* [in] */ Int32 positionY) = 0;
    };

public:
    /**
      * @return The current x position of the observed view.
      */
    virtual CARAPI_(Int32) GetPositionX() = 0;

    /**
      * @return The current y position of the observed view.
      */
    virtual CARAPI_(Int32) GetPositionY() = 0;

    /**
      * Register a listener to be called when the position of the underlying view changes.
      */
    virtual CARAPI AddListener(
        /* [in] */ Listener* listener) = 0;

    /**
      * Remove a previously installed listener.
      */
    virtual CARAPI RemoveListener(
        /* [in] */ Listener* listener) = 0;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_POSITIONOBSERVER_H_

