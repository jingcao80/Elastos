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

#ifndef __ELASTOS_DROID_WIDGET_TOOLBAR__
#define __ELASTOS_DROID_WIDGET_TOOLBAR__

#include "Elastos.Droid.Widget.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/view/ViewGroup.h"
#include "elastos/droid/widget/RtlSpacingHelper.h"
#include "elastos/droid/app/ActionBarLayoutParams.h"
#include "elastos/droid/R.h"

using Elastos::Droid::App::ActionBarLayoutParams;
using Elastos::Droid::Internal::View::Menu::IMenuPresenter;
using Elastos::Droid::Internal::View::Menu::IMenuBuilder;
using Elastos::Droid::Internal::View::Menu::IMenuView;
using Elastos::Droid::Internal::View::Menu::IMenuPresenterCallback;
using Elastos::Droid::Internal::View::Menu::IMenuBuilderCallback;
using Elastos::Droid::Internal::View::Menu::ISubMenuBuilder;
using Elastos::Droid::Internal::View::Menu::IMenuItemImpl;
using Elastos::Droid::Internal::Widget::IDecorToolbar;
using Elastos::Droid::Internal::Widget::IToolbarWidgetWrapper;
using Elastos::Core::Runnable;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::RtlSpacingHelper;

namespace Elastos {
namespace Droid {
namespace Widget {

class Toolbar
    : public ViewGroup
    , public IToolbar
{
public:
    class ToolbarLayoutParams
        : public ActionBarLayoutParams
        , public IToolbarLayoutParams
    {
    public:
        CAR_INTERFACE_DECL()

        ToolbarLayoutParams();

        CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 gravity);

        CARAPI constructor(
            /* [in] */ Int32 gravity);

        CARAPI constructor(
            /* [in] */ IToolbarLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IActionBarLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IViewGroupMarginLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        CARAPI SetViewType(
            /* [in] */ Int32 type);

        CARAPI GetViewType(
            /* [out] */ Int32* type);

    public:
        Int32 mViewType;// = CUSTOM;
    };

    class ToolbarSavedState
        : public View::BaseSavedState
        , public IToolbarSavedState
    {
    public:
        CAR_INTERFACE_DECL()

        ToolbarSavedState();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IParcelable* superState);

        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        CARAPI GetExpandedMenuItemId(
            /* [out] */ Int32* itemId);

        CARAPI SetExpandedMenuItemId(
            /* [in] */ Int32 itemId);

        CARAPI IsOverflowOpen(
            /* [out] */ Boolean* isOverflowOpen);

        CARAPI SetOverflowOpen(
            /* [in] */ Boolean isOverflowOpen);

    public:
        Int32 mExpandedMenuItemId;
        Boolean mIsOverflowOpen;
    };

private:
    class MenuItemClickListener
        : public Object
        , public IActionMenuViewOnMenuItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MenuItemClickListener(
            /* [in] */ Toolbar* host);

        CARAPI OnMenuItemClick(
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* rst);
    private:
        Toolbar* mHost;
    };

    class ExpandedActionViewMenuPresenter
        : public Object
        , public IMenuPresenter
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("Toolbar::ExpandedActionViewMenuPresenter")

        ExpandedActionViewMenuPresenter(
            /* [in] */ Toolbar* host);

        ~ExpandedActionViewMenuPresenter();

        CARAPI InitForMenu(
            /* [in] */ IContext* ctx,
            /* [in] */ IMenuBuilder* menu);

        CARAPI GetMenuView(
            /* [in] */ IViewGroup* root,
            /* [out] */ IMenuView** view);

        CARAPI UpdateMenuView(
            /* [in] */ Boolean cleared);

        CARAPI SetCallback(
            /* [in] */ IMenuPresenterCallback* cb);

        CARAPI OnSubMenuSelected(
            /* [in] */ ISubMenuBuilder* subMenu,
            /* [out] */ Boolean* handled);

        CARAPI OnCloseMenu(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ Boolean allMenusAreClosing);

        CARAPI FlagActionItems(
            /* [out] */ Boolean* shown);

        CARAPI ExpandItemActionView(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ IMenuItemImpl* item,
            /* [out] */ Boolean* expanded);

        CARAPI CollapseItemActionView(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ IMenuItemImpl* item,
            /* [out] */ Boolean* collapsed);

        CARAPI GetId(
            /* [out] */ Int32* id);

        CARAPI OnSaveInstanceState(
            /* [out] */ IParcelable** pa);

        CARAPI OnRestoreInstanceState(
            /* [in] */ IParcelable* state);

    private:
        friend class Toolbar;
        Toolbar* mHost;
        AutoPtr<IMenuBuilder> mMenu;
        AutoPtr<IMenuItemImpl> mCurrentExpandedItem;
    };

    class ToolbarRunnable
        : public Runnable
    {
    public:
        ToolbarRunnable(
            /* [in] */ Toolbar* host);

        CARAPI Run();

    private:
        Toolbar* mHost;
    };

    class CollapseOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        CollapseOnClickListener(
            /* [in] */ Toolbar* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        Toolbar* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    Toolbar();

    ~Toolbar();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL,
        /* [in] */ Int32 defStyleAttr = R::attr::toolbarStyle,
        /* [in] */ Int32 defStyleRes = 0);

    CARAPI SetPopupTheme(
        /* [in] */ Int32 resId);

    CARAPI GetPopupTheme(
        /* [out] */ Int32* theme);

    CARAPI OnRtlPropertiesChanged(
        /* [in] */ Int32 layoutDirection);

    CARAPI SetLogo(
        /* [in] */ Int32 resId);

    CARAPI CanShowOverflowMenu(
        /* [out] */ Boolean* menu);

    CARAPI IsOverflowMenuShowing(
        /* [out] */ Boolean* isShowing);

    CARAPI IsOverflowMenuShowPending(
        /* [out] */ Boolean* is);

    CARAPI ShowOverflowMenu(
        /* [out] */ Boolean* isShowing);

    CARAPI HideOverflowMenu(
        /* [out] */ Boolean* isHide);

    CARAPI SetMenu(
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ IActionMenuPresenter* outerPresenter);

    CARAPI DismissPopupMenus();

    CARAPI IsTitleTruncated(
        /* [out] */ Boolean* is);

    CARAPI SetLogo(
        /* [in] */ IDrawable* drawable);

    CARAPI GetLogo(
        /* [out] */ IDrawable** logo);

    CARAPI SetLogoDescription(
        /* [in] */ Int32 resId);

    CARAPI SetLogoDescription(
        /* [in] */ ICharSequence* description);

    CARAPI GetLogoDescription(
        /* [out] */ ICharSequence** description);

    CARAPI HasExpandedActionView(
        /* [out] */ Boolean* has);

    CARAPI CollapseActionView();

    CARAPI GetTitle(
        /* [out] */ ICharSequence** title);

    CARAPI SetTitle(
        /* [in] */ Int32 resId);

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    CARAPI GetSubtitle(
        /* [out] */ ICharSequence** description);

    CARAPI SetSubtitle(
        /* [in] */ Int32 resId);

    CARAPI SetSubtitle(
        /* [in] */ ICharSequence* title);

    CARAPI SetTitleTextAppearance(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resId);

    CARAPI SetSubtitleTextAppearance(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resId);

    CARAPI SetTitleTextColor(
        /* [in] */ Int32 color);

    CARAPI SetSubtitleTextColor(
        /* [in] */ Int32 color);

    CARAPI GetNavigationContentDescription(
        /* [out] */ ICharSequence** description);

    CARAPI SetNavigationContentDescription(
        /* [in] */ Int32 resId);

    CARAPI SetNavigationContentDescription(
        /* [in] */ ICharSequence* description);

    CARAPI SetNavigationIcon(
        /* [in] */ Int32 resId);

    CARAPI SetNavigationIcon(
        /* [in] */ IDrawable* icon);

    CARAPI GetNavigationIcon(
        /* [out] */ IDrawable** icon);

    CARAPI SetNavigationOnClickListener(
        /* [in] */ IViewOnClickListener* listener);

    CARAPI GetMenu(
        /* [out] */ IMenu** menu);

    CARAPI InflateMenu(
        /* [in] */ Int32 resId);

    CARAPI SetOnMenuItemClickListener(
        /* [in] */ IToolbarOnMenuItemClickListener* listener);

    CARAPI SetContentInsetsRelative(
        /* [in] */ Int32 contentInsetStart,
        /* [in] */ Int32 contentInsetEnd);

    CARAPI GetContentInsetStart(
        /* [out] */ Int32* start);

    CARAPI GetContentInsetEnd(
        /* [out] */ Int32* end);

    CARAPI SetContentInsetsAbsolute(
        /* [in] */ Int32 contentInsetLeft,
        /* [in] */ Int32 contentInsetRight);

    CARAPI GetContentInsetLeft(
        /* [out] */ Int32* left);

    CARAPI GetContentInsetRight(
        /* [out] */ Int32* right);

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    CARAPI_(void) OnSetLayoutParams(
        /* [in] */ IView* child,
        /* [in] */ IViewGroupLayoutParams* layoutParams);

    // @Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** layoutParams);

    // @Override
    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    CARAPI GetWrapper(
        /* [out] */ IDecorToolbar** bar);

    CARAPI SetCollapsible(
        /* [in] */ Boolean collapsible);

    CARAPI SetMenuCallbacks(
        /* [in] */ IMenuPresenterCallback* pcb,
        /* [in] */ IMenuBuilderCallback* mcb);

protected:
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    CARAPI OnDetachedFromWindow();

    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params);

    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

private:
    CARAPI EnsureLogoView();

    CARAPI EnsureMenu();

    CARAPI EnsureMenuView();

    CARAPI_(AutoPtr<IMenuInflater>) GetMenuInflater();

    CARAPI EnsureNavButtonView();

    CARAPI EnsureCollapseButtonView();

    CARAPI AddSystemView(
        /* [in] */ IView* v);

    CARAPI PostShowOverflowMenu();

    CARAPI MeasureChildConstrained(
        /* [in] */ IView* child,
        /* [in] */ Int32 parentWidthSpec,
        /* [in] */ Int32 widthUsed,
        /* [in] */ Int32 parentHeightSpec,
        /* [in] */ Int32 heightUsed,
        /* [in] */ Int32 heightConstraint);

    CARAPI_(Int32) MeasureChildCollapseMargins(
        /* [in] */ IView* child,
        /* [in] */ Int32 parentWidthMeasureSpec,
        /* [in] */ Int32 widthUsed,
        /* [in] */ Int32 parentHeightMeasureSpec,
        /* [in] */ Int32 heightUsed,
        /* [in] */ ArrayOf<Int32> * collapsingMargins);

    CARAPI_(Boolean) ShouldCollapse();

    CARAPI_(Int32) GetViewListMeasuredWidth(
        /* [in] */ IList* views,
        /* [in] */ ArrayOf<Int32> * collapsingMargins);

    CARAPI_(Int32) LayoutChildLeft(
        /* [in] */IView* child,
        /* [in] */ Int32 left,
        /* [in] */ ArrayOf<Int32> * collapsingMargins,
        /* [in] */ Int32 alignmentHeight);

    CARAPI_(Int32) LayoutChildRight(
        /* [in] */ IView* child,
        /* [in] */ Int32 right,
        /* [in] */ ArrayOf<Int32> * collapsingMargins,
        /* [in] */ Int32 alignmentHeight);

    CARAPI_(Int32) GetChildTop(
        /* [in] */ IView* child,
        /* [in] */ Int32 alignmentHeight);

    CARAPI_(Int32) GetChildVerticalGravity(
        /* [in] */ Int32 gravity);

    CARAPI_(void) AddCustomViewsWithGravity(
        /* [in] */ IList* views,
        /* [in] */ Int32 gravity);

    CARAPI_(Int32) GetChildHorizontalGravity(
        /* [in] */ Int32 gravity);

    CARAPI_(Boolean) ShouldLayout(
        /* [in] */ IView* view);

    CARAPI_(Int32) GetHorizontalMargins(
        /* [in] */ IView* v);

    CARAPI_(Int32) GetVerticalMargins(
        /* [in] */ IView* v);

    static CARAPI_(Boolean) IsCustomView(
        /* [in] */ IView* child);

    CARAPI SetChildVisibilityForExpandedActionView(
        /* [in] */ Boolean expand);

    CARAPI UpdateChildVisibilityForExpandedActionView(
        /* [in] */ IView* child);

private:
    static const String TAG;

    AutoPtr<IActionMenuView> mMenuView;
    AutoPtr<ITextView> mTitleTextView;
    AutoPtr<ITextView> mSubtitleTextView;
    AutoPtr<IImageButton> mNavButtonView;
    AutoPtr<IImageView> mLogoView;

    AutoPtr<IDrawable> mCollapseIcon;
    AutoPtr<IImageButton> mCollapseButtonView;
    AutoPtr<IView> mExpandedActionView;

    /** Context against which to inflate popup menus. */
    IContext* mPopupContext;
    Boolean mHolderPopupContext;

    /** Theme resource against which to inflate popup menus. */
    Int32 mPopupTheme;

    Int32 mTitleTextAppearance;
    Int32 mSubtitleTextAppearance;
    Int32 mNavButtonStyle;

    Int32 mButtonGravity;

    Int32 mMaxButtonHeight;

    Int32 mTitleMarginStart;
    Int32 mTitleMarginEnd;
    Int32 mTitleMarginTop;
    Int32 mTitleMarginBottom;

    AutoPtr<RtlSpacingHelper> mContentInsets;// = new RtlSpacingHelper();

    Int32 mGravity;// = Gravity.START | Gravity.CENTER_VERTICAL;

    AutoPtr<ICharSequence> mTitleText;
    AutoPtr<ICharSequence> mSubtitleText;

    Int32 mTitleTextColor;
    Int32 mSubtitleTextColor;

    Boolean mEatingTouch;

    // Clear me after use.
    AutoPtr<IArrayList> mTempViews;// = new ArrayList<View>();

    AutoPtr< ArrayOf<Int32> > mTempMargins;// = new int[2];

    AutoPtr<IToolbarOnMenuItemClickListener> mOnMenuItemClickListener;

    AutoPtr<IActionMenuViewOnMenuItemClickListener> mMenuViewItemClickListener;//

    AutoPtr<IWeakReference> mWeakWrapper;
    AutoPtr<IActionMenuPresenter> mOuterActionMenuPresenter;
    AutoPtr<ExpandedActionViewMenuPresenter> mExpandedMenuPresenter;
    AutoPtr<IMenuPresenterCallback> mActionMenuPresenterCallback;
    AutoPtr<IMenuBuilderCallback> mMenuBuilderCallback;

    Boolean mCollapsible;

    AutoPtr<IRunnable> mShowOverflowMenuRunnable;
};

} // namespace Widget
} // namespace Droid
} // namaspace Elastos

#endif //__ELASTOS_DROID_WIDGET_TOOLBAR__