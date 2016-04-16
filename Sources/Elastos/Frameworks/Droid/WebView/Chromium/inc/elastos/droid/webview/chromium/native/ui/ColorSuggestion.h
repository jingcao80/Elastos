// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_COLORSUGGESTION_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_COLORSUGGESTION_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.ui;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {

/**
  * Color suggestion container used to store information for each color button that will be shown in
  * the simple color picker.
  */
class ColorSuggestion
    : public Object
{
public:
    /**
      * Constructs a color suggestion container.
      * @param color The suggested color.
      * @param label The label for the suggestion.
      */
    ColorSuggestion(
        /* [in] */ Int32 color,
        /* [in] */ const String& label);

public:
    const Int32 mColor;
    const String mLabel;
};

} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

template <>
struct Conversion<Elastos::Droid::Webkit::Webview::Chromium::Ui::ColorSuggestion*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_COLORSUGGESTION_H_

