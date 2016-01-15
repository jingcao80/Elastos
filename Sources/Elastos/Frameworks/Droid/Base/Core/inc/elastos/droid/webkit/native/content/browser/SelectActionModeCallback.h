// wuweizuo automatic build .h file from .java file.
// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_SELECTACTIONMODECALLBACK_H_
#define _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_SELECTACTIONMODECALLBACK_H_

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
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_SELECTACTIONMODECALLBACK_H_
