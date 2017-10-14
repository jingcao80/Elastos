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

#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_ACTIONMENUITEMVIEW_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_ACTIONMENUITEMVIEW_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/ListPopupWindow.h"
#include "elastos/droid/widget/TextView.h"

using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::Widget::IActionMenuChildView;
using Elastos::Droid::Widget::IListPopupWindow;
using Elastos::Droid::Widget::ListPopupWindow;
using Elastos::Droid::Widget::TextView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

class ActionMenuItemView
    : public TextView
    , public IActionMenuItemView
    , public IMenuItemView
    , public IViewOnClickListener
    , public IViewOnLongClickListener
    , public IActionMenuChildView
{
private:
    class MyListener
        : public Object
        , public IViewOnClickListener
        , public IViewOnLongClickListener
    {
    public:
        MyListener(
            /* [in] */ ActionMenuItemView* owner);

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

    private:
        ActionMenuItemView* mOwner;
    };

    class ActionMenuItemForwardingListener
        : public ListPopupWindow::ForwardingListener
    {
    public:
        ActionMenuItemForwardingListener(
            /* [in] */ ActionMenuItemView* owner);

        // @Override
        CARAPI GetPopup(
            /* [out] */ IListPopupWindow** window);

    protected:
        // @Override
        CARAPI_(Boolean) OnForwardingStarted();

        // @Override
        CARAPI_(Boolean) OnForwardingStopped();

    private:
        ActionMenuItemView* mOwner;
    };

public:
    ActionMenuItemView();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    //@Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    //@Override
    CARAPI SetPadding(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    CARAPI GetItemData(
        /* [out] */ IMenuItemImpl** itemData);

    CARAPI Initialize(
        /* [in] */ IMenuItemImpl* itemData,
        /* [in] */ Int32 menuType);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* e,
        /* [out] */ Boolean* result);

    CARAPI OnClick(
        /* [in] */ IView* v);

    CARAPI SetItemInvoker(
        /* [in] */ IMenuBuilderItemInvoker* invoker);

    CARAPI SetPopupCallback(
        /* [in] */ IPopupCallback* popupCallback);

    CARAPI PrefersCondensedTitle(
        /* [out] */ Boolean* supported);

    CARAPI SetCheckable(
        /* [in] */ Boolean checkable);

    CARAPI SetChecked(
        /* [in] */ Boolean checked);

    CARAPI SetExpandedFormat(
        /* [in] */ Boolean expandedFormat);

    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    CARAPI HasText(
        /* [out] */ Boolean* result);

    CARAPI SetShortcut(
        /* [in] */ Boolean showShortcut,
        /* [in] */ Char32 shortcutKey);

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    //@Override
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    //@Override
    CARAPI_(Boolean) DispatchHoverEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI ShowsIcon(
        /* [out] */ Boolean* shows);

    CARAPI NeedsDividerBefore(
        /* [out] */ Boolean* need);

    CARAPI NeedsDividerAfter(
        /* [out] */ Boolean* need);

    //@Override
    CARAPI OnLongClick(
        /* [in] */ IView* v,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

protected:
    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    CARAPI_(void) UpdateTextButtonVisibility();

private:
    static String TAG;

    AutoPtr<IMenuItemImpl> mItemData;
    AutoPtr<ICharSequence> mTitle;
    AutoPtr<IDrawable> mIcon;
    AutoPtr<IMenuBuilderItemInvoker> mItemInvoker;
    AutoPtr<ListPopupWindow::ForwardingListener> mForwardingListener;
    AutoPtr<IPopupCallback> mPopupCallback;

    Boolean mAllowTextWithIcon;
    Boolean mExpandedFormat;
    Int32 mMinWidth;
    Int32 mSavedPaddingLeft;

    static const Int32 MAX_ICON_SIZE; // dp
    Int32 mMaxIconSize;
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INTERNAL_VIEW_MENU_ACTIONMENUITEMVIEW_H__
