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
// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_SELECTACTIONMODECALLBACK_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_SELECTACTIONMODECALLBACK_H_

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IClipboardManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
// import org.chromium.content.R;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
  * An ActionMode.Callback for in-page selection. This class handles both the editable and
  * non-editable cases.
  */
class SelectActionModeCallback
    : public Object
    , public IActionModeCallback
{
    friend class ContentViewClient;
public:
    /**
      * An interface to retrieve information about the current selection, and also to perform
      * actions based on the selection or when the action bar is dismissed.
      */
    class ActionHandler : public Object
    {
    public:
        /**
          * Perform a select all action.
          */
        virtual CARAPI SelectAll() = 0;

        /**
          * Perform a copy (to clipboard) action.
          */
        virtual CARAPI Copy() = 0;

        /**
          * Perform a cut (to clipboard) action.
          */
        virtual CARAPI Cut() = 0;

        /**
          * Perform a paste action.
          */
        virtual CARAPI Paste() = 0;

        /**
          * Perform a share action.
          */
        virtual CARAPI Share() = 0;

        /**
          * Perform a search action.
          */
        virtual CARAPI Search() = 0;

        /**
          * @return true iff the current selection is editable (e.g. text within an input field).
          */
        virtual CARAPI IsSelectionEditable(
            /* [out] */ Boolean* result) = 0;

        /**
          * Called when the onDestroyActionMode of the SelectActionmodeCallback is called.
          */
        virtual CARAPI OnDestroyActionMode() = 0;

        /**
          * @return Whether or not share is available.
          */
        virtual CARAPI IsShareAvailable(
            /* [out] */ Boolean* result) = 0;

        /**
          * @return Whether or not web search is available.
          */
        virtual CARAPI IsWebSearchAvailable(
            /* [out] */ Boolean* result) = 0;

        /**
          * @return true if the current selection is of password type.
          */
        virtual CARAPI IsSelectionPassword(
            /* [out] */ Boolean* result) = 0;
    };

public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI OnCreateActionMode(
        /* [in] */ IActionMode* mode,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnPrepareActionMode(
        /* [in] */ IActionMode* mode,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnActionItemClicked(
        /* [in] */ IActionMode* mode,
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnDestroyActionMode(
        /* [in] */ IActionMode* mode);

protected:
    SelectActionModeCallback(
        /* [in] */ IContext* context,
        /* [in] */ ActionHandler* actionHandler,
        /* [in] */ Boolean incognito);

    virtual CARAPI_(AutoPtr<IContext>) GetContext();

private:
    CARAPI CreateActionMenu(
        /* [in] */ IActionMode* mode,
        /* [in] */ IMenu* menu);

    CARAPI_(Boolean) CanPaste();

private:
    /*const*/ AutoPtr<IContext> mContext;
    /*const*/ AutoPtr<ActionHandler> mActionHandler;
    /*const*/ Boolean mIncognito;
    Boolean mEditable;
    Boolean mIsPasswordType;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_SELECTACTIONMODECALLBACK_H_
