// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_UI_DROPDOWNITEM_H_
#define _ELASTOS_DROID_WEBKIT_UI_DROPDOWNITEM_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// package org.chromium.ui;

namespace Elastos {
namespace Droid {
namespace Webkit {
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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_UI_DROPDOWNITEM_H_

