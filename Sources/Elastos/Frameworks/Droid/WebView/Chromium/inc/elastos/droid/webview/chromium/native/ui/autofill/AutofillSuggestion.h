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

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_AUTOFILL_AUTOFILLSUGGESTION_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_AUTOFILL_AUTOFILLSUGGESTION_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/DropdownItem.h"

// package org.chromium.ui.autofill;
// import org.chromium.ui.DropdownItem;

using Elastos::Droid::Webkit::Webview::Chromium::Ui::DropdownItem;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Autofill {

/**
  * Autofill suggestion container used to store information needed for each Autofill popup entry.
  */
class AutofillSuggestion
    : public DropdownItem
{
public:
    /**
      * Constructs a Autofill suggestion container.
      * @param name The name of the Autofill suggestion.
      * @param label The describing label of the Autofill suggestion.
      * @param uniqueId The unique id used to identify the Autofill suggestion.
      */
    AutofillSuggestion(
        /* [in] */ const String& name,
        /* [in] */ const String& label,
        /* [in] */ Int32 uniqueId);

    // @Override
    CARAPI_(String) GetLabel();

    // @Override
    CARAPI_(String) GetSublabel();

    // @Override
    CARAPI_(Boolean) IsEnabled();

    // @Override
    CARAPI_(Boolean) IsGroupHeader();

    virtual CARAPI_(Int32) GetUniqueId();

public:
    /*const*/ String mLabel;
    /*const*/ String mSublabel;
    /*const*/ Int32 mUniqueId;
};

} // namespace Autofill
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_AUTOFILL_AUTOFILLSUGGESTION_H_

