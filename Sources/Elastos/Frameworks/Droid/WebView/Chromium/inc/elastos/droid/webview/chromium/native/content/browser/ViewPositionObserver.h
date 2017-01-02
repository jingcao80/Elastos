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
// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_VIEWPOSITIONOBSERVER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_VIEWPOSITIONOBSERVER_H_

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/PositionObserver.h"

using Elastos::Droid::View::IView;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Utility::IArrayList;

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
class ViewPositionObserver
    : public PositionObserver
{
public:
    class InnerViewTreeObserverOnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerViewTreeObserverOnPreDrawListener(
            /* [in] */ ViewPositionObserver* owner);

        // @Override
        CARAPI OnPreDraw(
          /* [out] */ Boolean* result);

    private:
        ViewPositionObserver* mOwner;
    };

public:
    /**
      * @param view The view to observe.
      */
    ViewPositionObserver(
        /* [in] */ IView* view);

    /**
      * @return The current x position of the observed view.
      */
    // @Override
    CARAPI_(Int32) GetPositionX();

    /**
      * @return The current y position of the observed view.
      */
    // @Override
    CARAPI_(Int32) GetPositionY();

    /**
      * Register a listener to be called when the position of the underlying view changes.
      */
    // @Override
    CARAPI AddListener(
        /* [in] */ Listener* listener);

    /**
      * Remove a previously installed listener.
      */
    // @Override
    CARAPI RemoveListener(
        /* [in] */ Listener* listener);

private:
    CARAPI_(AutoPtr< ArrayOf<Int32> >) MiddleInitMposition();

    CARAPI NotifyListeners();

    CARAPI UpdatePosition();

private:
    AutoPtr<IView> mView;
    // Absolute position of the container view relative to its parent window.
    AutoPtr< ArrayOf<Int32> > mPosition;
    ///*const*/ AutoPtr< IArrayList<Listener> > mListeners;
    AutoPtr<IArrayList> mListeners;
    AutoPtr<IOnPreDrawListener> mPreDrawListener;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_VIEWPOSITIONOBSERVER_H_
