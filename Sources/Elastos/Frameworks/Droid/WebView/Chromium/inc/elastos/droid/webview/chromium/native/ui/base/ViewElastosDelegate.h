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

// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_VIEWANDROIDDELEGATE_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_VIEWANDROIDDELEGATE_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.ui.base;
// import android.view.View;

using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Base {

/**
  * Interface to acquire and release anchor views from the implementing View.
  */
class ViewElastosDelegate
{
public:
    virtual CARAPI_(UInt32) AddRef() = 0;

    virtual CARAPI_(UInt32) Release() = 0;

    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid) = 0;

    virtual CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid) = 0;

    /**
      * @return An anchor view that can be used to anchor decoration views like Autofill popup.
      */
    virtual CARAPI_(AutoPtr<IView>) AcquireAnchorView() = 0;

    /**
      * Set the anchor view to specified position and width (all units in dp).
      * @param view The anchor view that needs to be positioned.
      * @param x X coordinate of the top left corner of the anchor view.
      * @param y Y coordinate of the top left corner of the anchor view.
      * @param width The width of the anchor view.
      * @param height The height of the anchor view.
      */
    virtual CARAPI SetAnchorViewPosition(
        /* [in] */ IView* view,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float width,
        /* [in] */ Float height) = 0;

    /**
      * Release given anchor view.
      * @param anchorView The anchor view that needs to be released.
      */
    virtual CARAPI ReleaseAnchorView(
        /* [in] */ IView* anchorView) = 0;
};

} // namespace Base
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_VIEWANDROIDDELEGATE_H_

