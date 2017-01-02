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

#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_ACTIONMENUITEM_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_ACTIONMENUITEM_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IActionProvider;
using Elastos::Droid::View::IContextMenuInfo;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IOnActionExpandListener;
using Elastos::Droid::View::IOnMenuItemClickListener;
using Elastos::Droid::View::ISubMenu;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

class ActionMenuItem
    : public Object
    , public IActionMenuItem
    , public IMenuItem
{
public:
    ActionMenuItem();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 group,
        /* [in] */ Int32 id,
        /* [in] */ Int32 categoryOrder,
        /* [in] */ Int32 ordering,
        /* [in] */ ICharSequence* title);

    CARAPI GetAlphabeticShortcut(
        /* [out] */ Char32* shortcut);

    CARAPI GetGroupId(
        /* [out] */ Int32* id);

    CARAPI GetIcon(
        /* [out] */ IDrawable** icon);

    CARAPI GetIntent(
        /* [out] */ IIntent** intent);

    CARAPI GetItemId(
        /* [out] */ Int32* id);

    CARAPI GetMenuInfo(
        /* [out] */ IContextMenuInfo** info);

    CARAPI GetNumericShortcut(
        /* [out] */ Char32* shortcut);

    CARAPI GetOrder(
        /* [out] */ Int32* ordering);

    CARAPI GetSubMenu(
        /* [out] */ ISubMenu** menu);

    CARAPI GetTitle(
        /* [out] */ ICharSequence** title);

    CARAPI GetTitleCondensed(
        /* [out] */ ICharSequence** title);

    CARAPI HasSubMenu(
        /* [out] */ Boolean* has);

    CARAPI IsCheckable(
        /* [out] */ Boolean* checkable);

    CARAPI IsChecked(
        /* [out] */ Boolean* checked);

    CARAPI IsEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI IsVisible(
        /* [out] */ Boolean* visible);

    CARAPI SetAlphabeticShortcut(
        /* [in] */ Char32 alphaChar);

    CARAPI SetCheckable(
        /* [in] */ Boolean checkable);

    CARAPI SetExclusiveCheckable(
        /* [in] */ Boolean exclusive);

    CARAPI SetChecked(
        /* [in] */ Boolean checked);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    CARAPI SetIcon(
        /* [in] */ Int32 iconRes);

    CARAPI SetIntent(
        /* [in] */ IIntent* intent);

    CARAPI SetNumericShortcut(
        /* [in] */ Char32 numericChar);

    CARAPI SetOnMenuItemClickListener(
        /* [in] */ IOnMenuItemClickListener* menuItemClickListener);

    CARAPI SetShortcut(
        /* [in] */ Char32 numericChar,
        /* [in] */ Char32 alphaChar);

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    CARAPI SetTitle(
        /* [in] */ Int32 title);

    CARAPI SetTitleCondensed(
        /* [in] */ ICharSequence* title);

    CARAPI SetVisible(
        /* [in] */ Boolean visible);

    CARAPI Invoke(
        /* [out] */ Boolean* res);

    CARAPI SetShowAsAction(
        /* [in] */ Int32 show);

    CARAPI SetActionView(
        /* [in] */ IView* actionView);

    CARAPI GetActionView(
        /* [out] */ IView** view);

    //@Override
    CARAPI SetActionView(
        /* [in] */ Int32 resId);

    //@Override
    CARAPI GetActionProvider(
        /* [out] */ IActionProvider** provider);

    //@Override
    CARAPI SetActionProvider(
        /* [in] */ IActionProvider* actionProvider);

    //@Override
    CARAPI SetShowAsActionFlags(
        /* [in] */ Int32 actionEnum);

    //@Override
    CARAPI ExpandActionView(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI CollapseActionView(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsActionViewExpanded(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetOnActionExpandListener(
        /* [in] */ IOnActionExpandListener* listener);

private:
    static const Int32 NO_ICON;
    static const Int32 CHECKABLE;
    static const Int32 CHECKED;
    static const Int32 EXCLUSIVE;
    static const Int32 HIDDEN;
    static const Int32 ENABLED;

    Int32 mId;
    Int32 mGroup;
    Int32 mCategoryOrder;
    Int32 mOrdering;

    AutoPtr<ICharSequence> mTitle;
    AutoPtr<ICharSequence> mTitleCondensed;
    AutoPtr<IIntent> mIntent;
    Char32 mShortcutNumericChar;
    Char32 mShortcutAlphabeticChar;

    AutoPtr<IDrawable> mIconDrawable;
    Int32 mIconResId;

    IContext* mContext;

    AutoPtr<IOnMenuItemClickListener> mClickListener;


    Int32 mFlags;
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_MENU_ACTIONMENUITEM_H__
