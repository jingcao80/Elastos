
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_MENUPOPUPHELPER_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_MENUPOPUPHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IOnGlobalLayoutListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnAttachStateChangeListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::Widget::IPopupWindowOnDismissListener;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IListPopupWindow;
using Elastos::Droid::Widget::ISpinnerAdapter;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

/**
 * Presents a menu as a small, simple popup anchored to another view.
 * @hide
 */
class MenuPopupHelper
    : public Object
    , public IMenuPopupHelper
    , public IAdapterViewOnItemClickListener
    , public IViewOnKeyListener
    , public IOnGlobalLayoutListener
    , public IPopupWindowOnDismissListener
    , public IViewOnAttachStateChangeListener
    , public IMenuPresenter
{
private:
    class MenuAdapter
        : public BaseAdapter
    {
    public:
        MenuAdapter(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ MenuPopupHelper* host);

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

        // /@Override
        CARAPI NotifyDataSetChanged();

    private:
        // AdapterMenu usually holds adapter, we use weak-reference here.
        AutoPtr<IWeakReference> mWeakAdapterMenu;
        // AutoPtr<IMenuBuilder> mAdapterMenu;
        Int32 mExpandedIndex;
        MenuPopupHelper*  mHost;

        friend class MenuPopupHelper;
    };

    class MyListener
        : public Object
        , public IAdapterViewOnItemClickListener
        , public IViewOnKeyListener
        , public IPopupWindowOnDismissListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ MenuPopupHelper* owner)
            : mOwner(owner)
        {}

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* v,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        CARAPI OnKey(
            /* [in] */ IView* v,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        CARAPI OnDismiss();

    private:
        MenuPopupHelper* mOwner;
    };

public:
    MenuPopupHelper();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IMenuBuilder* menu);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ IView* anchorView);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ IView* anchorView,
        /* [in] */ Boolean overflowOnly,
        /* [in] */ Int32 popupStyleAttr);

    CARAPI SetAnchorView(
        /* [in] */ IView* anchor);

    CARAPI SetForceShowIcon(
        /* [in] */ Boolean forceShow);

    CARAPI SetGravity(
        /* [in] */ Int32 gravity);

    CARAPI Show();

    CARAPI GetPopup(
        /* [out] */ IListPopupWindow** popup);

    CARAPI TryShow(
        /* [out] */ Boolean* result);

    CARAPI Dismiss();

    CARAPI OnDismiss();

    CARAPI IsShowing(
        /* [out] */ Boolean* result);

    CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    CARAPI OnKey(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnGlobalLayout();

    //@Override
    CARAPI OnViewAttachedToWindow(
        /* [in] */ IView* v);

    //@Override
    CARAPI OnViewDetachedFromWindow(
        /* [in] */ IView* v);

    //@Override
    CARAPI InitForMenu(
        /* [in] */ IContext* context,
        /* [in] */ IMenuBuilder* menu);

    //@Override
    CARAPI GetMenuView(
        /* [in] */ IViewGroup* root,
        /* [out] */ IMenuView** view);

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

    //@Override
    CARAPI GetId(
        /* [out] */ Int32* id);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable** parcel);

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

private:
    CARAPI_(Int32) MeasureContentWidth();

private:
    static const String TAG;
    static const Int32 ITEM_LAYOUT;

    AutoPtr<IContext> mContext;
    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<IMenuBuilder> mMenu;
    AutoPtr<MenuAdapter> mAdapter;
    Boolean mOverflowOnly;
    Int32 mPopupMaxWidth;
    Int32 mPopupStyleAttr;

    AutoPtr<IView> mAnchorView;
    AutoPtr<IListPopupWindow> mPopup;
    AutoPtr<IViewTreeObserver> mTreeObserver;
    AutoPtr<IMenuPresenterCallback> mPresenterCallback;

    Boolean mForceShowIcon;
    AutoPtr<IViewGroup> mMeasureParent;

    /** Whether the cached content width value is valid. */
    Boolean mHasContentWidth;

    /** Cached content width from {@link #measureContentWidth}. */
    Int32 mContentWidth;

    Int32 mDropDownGravity;
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INTERNAL_VIEW_MENU_MENUPOPUPHELPER_H__
