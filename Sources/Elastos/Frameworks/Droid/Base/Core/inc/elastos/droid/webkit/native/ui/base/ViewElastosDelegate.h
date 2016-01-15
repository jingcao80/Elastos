// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_UI_BASE_VIEWANDROIDDELEGATE_H_
#define _ELASTOS_DROID_WEBKIT_UI_BASE_VIEWANDROIDDELEGATE_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.ui.base;
// import android.view.View;

using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {
namespace Base {

/**
  * Interface to acquire and release anchor views from the implementing View.
  */
class ViewElastosDelegate
    : public Object
{
public:
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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_UI_BASE_VIEWANDROIDDELEGATE_H_

