// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_UI_DROPDOWNADAPTER_H_
#define _ELASTOS_DROID_WEBKIT_UI_DROPDOWNADAPTER_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/utility/etl/List.h"
#include "elastos/utility/etl/Set.h"
#include "elastos/droid/widget/ArrayAdapter.h"

// package org.chromium.ui;
// import android.content.Context;
// import android.graphics.Color;
// import android.graphics.Typeface;
// import android.text.TextUtils;
// import android.view.LayoutInflater;
// import android.view.View;
// import android.view.ViewGroup;
// import android.widget.AbsListView.LayoutParams;
// import android.widget.ArrayAdapter;
// import android.widget.TextView;
// import org.chromium.base.ApiCompatibilityUtils;
// import java.util.List;
// import java.util.Set;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Core::IInteger32;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Droid::Widget::ArrayAdapter;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {

class DropdownItem;

/**
  * Dropdown item adapter for DropdownPopupWindow.
  */
class DropdownAdapter
    : public ArrayAdapter
{
public:
    DropdownAdapter(
        /* [in] */ IContext* context,
        /* [in] */ IList* items,
        /* [in] */ ISet* separators);

    DropdownAdapter(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<DropdownItem*>* items,
        /* [in] */ ISet* separators);

    // @Override
    CARAPI_(AutoPtr<IView>) GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent);

    // @Override
    CARAPI_(Boolean) AreAllItemsEnabled();

    // @Override
    CARAPI_(Boolean) IsEnabled(
        /* [in] */ Int32 position);

private:
    CARAPI_(Boolean) CheckAreAllItemsEnabled();

private:
    AutoPtr<IContext> mContext;
    AutoPtr<ISet> mSeparators;
    Boolean mAreAllItemsEnabled;
};

} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_UI_DROPDOWNADAPTER_H_

