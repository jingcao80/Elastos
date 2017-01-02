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

// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_DROPDOWNITEM_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_DROPDOWNITEM_H_

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
  * Dropdown item interface used to access all the information needed to show the item.
  */
class DropdownItem
    : public Object
{
public:
    /**
      * Returns the label that should be shown in the dropdown.
      */
    virtual CARAPI_(String) GetLabel() = 0;

    /**
      * Returns the sublabel that should be shown in the dropdown.
      */
    virtual CARAPI_(String) GetSublabel() = 0;

    /**
      * Returns true if the item should be enabled in the dropdown.
      */
    virtual CARAPI_(Boolean) IsEnabled() = 0;

    /**
      * Returns true if the item should be a group header in the dropdown.
      */
    virtual CARAPI_(Boolean) IsGroupHeader() = 0;
};

} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_DROPDOWNITEM_H_

DEFINE_CONVERSION_FOR(Elastos::Droid::Webkit::Webview::Chromium::Ui::DropdownItem, IInterface)
