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

#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_LISTMENUPRESENTER_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_LISTMENUPRESENTER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

class ListMenuPresenter
    : public Object
    , public IListMenuPresenter
    , public IMenuPresenter
{
private:
    class MenuAdapter
        : public BaseAdapter
    {
    public:
        MenuAdapter(
            /* [in] */ ListMenuPresenter* host);

        CARAPI GetCount(
            /* [out] */ Int32* count);

        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* id);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        CARAPI_(void) FindExpandedIndex();

        //@Override
        CARAPI NotifyDataSetChanged();

    protected:
        Int32 mExpandedIndex;
        ListMenuPresenter*  mHost;
    };

    class OnItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        OnItemClickListener(
            /* [in] */ ListMenuPresenter* owner)
            : mOwner(owner)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* v,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        ListMenuPresenter* mOwner;
    };

public:
    ListMenuPresenter();

    ~ListMenuPresenter();

    CAR_INTERFACE_DECL()

    /**
     * Construct a new ListMenuPresenter.
     * @param context Context to use for theming. This will supersede the context provided
     *                to initForMenu when this presenter is added.
     * @param itemLayoutRes Layout resource for individual item views.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 itemLayoutRes);

    /**
     * Construct a new ListMenuPresenter.
     * @param itemLayoutRes Layout resource for individual item views.
     * @param themeRes Resource ID of a theme to use for views.
     */
    CARAPI constructor(
        /* [in] */ Int32 itemLayoutRes,
        /* [in] */ Int32 themeRes);

    //@Override
    CARAPI InitForMenu(
        /* [in] */ IContext* context,
        /* [in] */ IMenuBuilder* menu);

    //@Override
    CARAPI GetMenuView(
        /* [in] */ IViewGroup* root,
        /* [out] */ IMenuView** view);

    /**
     * Call this instead of getMenuView if you want to manage your own ListView.
     * For proper operation, the ListView hosting this adapter should add
     * this presenter as an OnItemClickListener.
     *
     * @return A ListAdapter containing the items in the menu.
     */
    CARAPI GetAdapter(
        /* [out] */ IListAdapter** adapter);

    //@Override
    CARAPI UpdateMenuView(
        /* [in] */ Boolean cleared);

    //@Override
    CARAPI SetCallback(
        /* [in] */ IMenuPresenterCallback* cb);

    //@Override
    CARAPI OnSubMenuSelected(
        /* [in] */ ISubMenuBuilder* subMenu,
        /* [out] */ Boolean* selected);

    //@Override
    CARAPI OnCloseMenu(
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ Boolean allMenusAreClosing);

    CARAPI SetItemIndexOffset(
        /* [in] */ Int32 offset);

    //@Override
    CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    //@Override
    CARAPI FlagActionItems(
        /* [out] */ Boolean* result);

    CARAPI ExpandItemActionView(
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ IMenuItemImpl* item,
        /* [out] */ Boolean* result);

    CARAPI CollapseItemActionView(
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ IMenuItemImpl* item,
        /* [out] */ Boolean* result);

    CARAPI SaveHierarchyState(
        /* [in] */ IBundle* outState);

    CARAPI RestoreHierarchyState(
        /* [in] */ IBundle* inState);

    CARAPI SetId(
        /* [in] */ Int32 id);

    //@Override
    CARAPI GetId(
        /* [out] */ Int32* id);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable** parcel);

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

protected:
    CARAPI_(Int32) GetItemIndexOffset();

public:
    IContext* mContext;
    Boolean mHolderContext;
    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<IMenuBuilder> mMenu;
    AutoPtr<IExpandedMenuView> mMenuView;
    Int32 mThemeRes;
    Int32 mItemLayoutRes;
    AutoPtr<MenuAdapter> mAdapter;

private:
    static const String TAG;

    Int32 mItemIndexOffset;
    AutoPtr<IMenuPresenterCallback> mCallback;
    Int32 mId;
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INTERNAL_VIEW_MENU_LISTMENUPRESENTER_H__
