// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_UI_COLORSUGGESTIONLISTADAPTER_H_
#define _ELASTOS_DROID_WEBKIT_UI_COLORSUGGESTIONLISTADAPTER_H_

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include "Elastos.Droid.Webkit.h"

// package org.chromium.ui;
// import android.content.Context;
// import android.graphics.Color;
// import android.graphics.drawable.GradientDrawable;
// import android.graphics.drawable.LayerDrawable;
// import android.text.TextUtils;
// import android.view.View;
// import android.view.ViewGroup;
// import android.widget.AbsListView;
// import android.widget.BaseAdapter;
// import android.widget.LinearLayout;
// import org.chromium.base.ApiCompatibilityUtils;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Webkit::Ui::IOnColorSuggestionClickListener;
using Elastos::Droid::Widget::BaseAdapter;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {

class ColorSuggestion;

/**
  * The adapter used to populate ColorPickerSimple.
  */
class ColorSuggestionListAdapter
    : public BaseAdapter
    , public IViewOnClickListener
{
public:
    CAR_INTERFACE_DECL()

    ColorSuggestionListAdapter(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<IInterface*>* suggestions);// type is ColorSuggestion

    /**
      * Sets the listener that will be notified upon a click on a suggestion.
      */
    virtual CARAPI SetOnColorSuggestionClickListener(
        /* [in] */ IOnColorSuggestionClickListener* listener);

    // @Override
    CARAPI OnClick(
        /* [in] */ IView* v);

    // @Override
    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    // @Override
    CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* id);

    // @Override
    // consistent with the parent class
    CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** item);

    // @Override
    CARAPI GetCount(
      /* [out] */ Int32* count);

private:
    /**
      * Sets up the color button to represent a color suggestion.
      *
      * @param button The button view to set up.
      * @param index The index of the suggestion in mSuggestions.
      */
    CARAPI SetUpColorButton(
        /* [in] */ IView* button,
        /* [in] */ Int32 index);

private:
    AutoPtr<IContext> mContext;
    AutoPtr< ArrayOf<IInterface*> > mSuggestions;//type is ColorSuggestion
    AutoPtr<IOnColorSuggestionClickListener> mListener;
    static const Int32 COLORS_PER_ROW = 4;
};

} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_UI_COLORSUGGESTIONLISTADAPTER_H_

