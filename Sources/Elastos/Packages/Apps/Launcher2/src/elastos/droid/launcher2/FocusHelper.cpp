
#include "elastos/droid/launcher2/FocusHelper.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(IconKeyEventListener, Object, IViewOnKeyListener);

ECode IconKeyEventListener::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FocusHelper::HandleIconKeyEvent(v, keyCode, event);
    return NOERROR;
}

CAR_INTERFACE_IMPL(FolderKeyEventListener, Object, IViewOnKeyListener);

ECode FolderKeyEventListener::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FocusHelper::HandleFolderKeyEvent(v, keyCode, event);
    return NOERROR;
}

CAR_INTERFACE_IMPL(HotseatIconKeyEventListener, Object, IViewOnKeyListener);

ECode HotseatIconKeyEventListener::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IResources> resources;
    v->GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> configuration;
    resources->GetConfiguration((IConfiguration**)&configuration);
    Int32 orientation;
    configuration->GetOrientation(&orientation);
    *result = FocusHelper::HandleHotseatButtonKeyEvent(v, keyCode, event, orientation);
    return NOERROR;
}

CAR_INTERFACE_IMPL(AppsCustomizeTabKeyEventListener, Object, IViewOnKeyListener);

ECode AppsCustomizeTabKeyEventListener::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FocusHelper::HandleAppsCustomizeTabKeyEvent(v, keyCode, event);
    return NOERROR;
}

CAR_INTERFACE_IMPL(FocusHelper::MyComparator, Object, IComparator);

FocusHelper::MyComparator::MyComparator(
    /* [in] */ Int32 cellCountX)
    : mCellCountX(cellCountX)
{
}

ECode FocusHelper::MyComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IView> lhsView = IView::Probe(lhs);
    AutoPtr<IView> rhsView = IView::Probe(rhs);

    AutoPtr<IViewGroupLayoutParams> lhsParams;
    lhsView->GetLayoutParams((IViewGroupLayoutParams**)&lhsParams);
    AutoPtr<ICellLayoutLayoutParams> llp = ICellLayoutLayoutParams::Probe(lhsParams);

    AutoPtr<IViewGroupLayoutParams> rhsParams;
    rhsView->GetLayoutParams((IViewGroupLayoutParams**)&rhsParams);
    AutoPtr<ICellLayoutLayoutParams> rlp = ICellLayoutLayoutParams::Probe(rhsParams);

    Int32 llpCellY;
    llp->GetCellY(&llpCellY);
    Int32 llpCellX;
    llp->GetCellX(&llpCellX);
    Int32 lvIndex = llpCellY * cellCountX) + llpCellX;

    Int32 rlpCellY;
    rlp->GetCellY(&rlpCellY);
    Int32 rlpCellX;
    rlp->GetCellX(&rlpCellX);
    Int32 rvIndex = (rlpCellY * cellCountX) + rlpCellX;
    *result = lvIndex - rvIndex;
    return NOERROR;
}

AutoPtr<ITabHost> FocusHelper::FindTabHostParent(
    /* [in] */ IView* v)
{
    AutoPtr<IViewParent> p;
    v->GetParent((IViewParent**)&p);
    while (p != NULL && !(ITabHost::Probe(p) != NULL)) {
        AutoPtr<IViewParent> tmp;
        p->GetParent((IViewParent**)&tmp);
        p = NULL;
        p = tmp;
    }
    AutoPtr<ITabHost> host = ITabHost::Probe(p);
    return host;
}

Boolean FocusHelper::HandleAppsCustomizeTabKeyEvent(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* e)
{
    AutoPtr<ITabHost> tabHost = FindTabHostParent(v);
    AutoPtr<IViewGroup> contents;
    tabHost->GetTabContentView((IViewGroup**)&contents);
    AutoPtr<IView> shop;
    tabHost->FindViewById(
            Elastos::Droid::Launcher2::R::id::market_button, (IView**)&shop);

    Int32 action;
    e->GetAction(&action);
    const Boolean handleKeyEvent = (action != IKeyEvent::ACTION_UP);
    Boolean wasHandled = FALSE;
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_RIGHT:
            if (handleKeyEvent) {
                // Select the shop button if we aren't on it
                if (TO_IINTERFACE(v) != TO_IINTERFACE(shop)) {
                    shop->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        case IKeyEvent::KEYCODE_DPAD_DOWN:
            if (handleKeyEvent) {
                // Select the content view (down is handled by the tab key handler otherwise)
                if (TO_IINTERFACE(v) == TO_IINTERFACE(shop)) {
                    contents->RequestFocus();
                    wasHandled = TRUE;
                }
            }
            break;
        default: break;
    }
    return wasHandled;
}

AutoPtr<IViewGroup> FocusHelper::GetAppsCustomizePage(
    /* [in] */ IViewGroup* container,
    /* [in] */ Int32 index)
{
    AutoPtr<IPagedView> pview = IPagedView::Probe(container);
    AutoPtr<IView> view;
    pview->GetPageAt(index, (IView**)&view);
    AutoPtr<IViewGroup> page = IViewGroup::Probe(view);
    if (IPagedViewCellLayout::Probe(page) != NULL) {
        // There are two layers, a PagedViewCellLayout and PagedViewCellLayoutChildren
        AutoPtr<IView> child;
        page->GetChildAt(0, (IView**)&child);
        page = IViewGroup::Probe(child);
    }
    return page;
}

Boolean FocusHelper::HandlePagedViewGridLayoutWidgetKeyEvent(
    /* [in] */ IPagedViewWidget* w,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* e)
{
    AutoPtr<IViewParent> vparent;
    IViewGroup::Probe(w)->GetParent((IViewParent**)&vparent);
    AutoPtr<IPagedViewGridLayout> parent = IPagedViewGridLayout::Probe(vparent);

    AutoPtr<IViewParent> vparent2;
    IViewGroup::Probe(parent)->GetParent((IViewParent**)&vparent2);
    AutoPtr<IPagedView> container = IPagedView::Probe(vparent2);

    AutoPtr<ITabHost> tabHost = FindTabHostParent(container);
    AutoPtr<ITabWidget> tabs;
    tabHost->GetTabWidget((ITabWidget**)&tabs);
    Int32 widgetIndex;
    parent->IndexOfChild(w, &widgetIndex);
    Int32 widgetCount;
    parent->GetChildCount(&widgetCount);

    Int32 index;
    container->IndexOfChild(IView::Probe(parent), &index);
    Int32 pageIndex;
    container->IndexToPage(index, &pageIndex);

    Int32 pageCount;
    container->GetChildCount(&pageCount);
    Int32 cellCountX;
    parent->GetCellCountX(&cellCountX);
    Int32 cellCountY;
    parent->GetCellCountY(&cellCountY);
    Int32 x = widgetIndex % cellCountX;
    Int32 y = widgetIndex / cellCountX;

    Int32 action;
    e->GetAction(&action);
    Boolean handleKeyEvent = (action != IKeyEvent::ACTION_UP);
    AutoPtr<IViewGroup> newParent;
    // Now that we load items in the bg asynchronously, we can't just focus
    // child siblings willy-nilly
    AutoPtr<IView> child;
    Boolean wasHandled = FALSE;
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_LEFT:
        {
            if (handleKeyEvent) {
                // Select the previous widget or the last widget on the previous page
                if (widgetIndex > 0) {
                    AutoPtr<IView> view;
                    IViewGroup::Probe(parent)->GetChildAt(widgetIndex - 1, (IView**)&view);
                    view->RequestFocus();
                }
                else {
                    if (pageIndex > 0) {
                        newParent = GetAppsCustomizePage(container, pageIndex - 1);
                        if (newParent != NULL) {
                            Int32 count;
                            newParent->GetChildCount(&count);
                            newParent->GetChildAt(count - 1, (IView**)&child);
                            if (child != NULL) child->RequestFocus();
                        }
                    }
                }
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_DPAD_RIGHT:
        {
            if (handleKeyEvent) {
                // Select the next widget or the first widget on the next page
                if (widgetIndex < (widgetCount - 1)) {
                    AutoPtr<IView> view;
                    parent->GetChildAt(widgetIndex + 1, (IView**)&view);
                    view->RequestFocus();
                }
                else {
                    if (pageIndex < (pageCount - 1)) {
                        newParent = GetAppsCustomizePage(container, pageIndex + 1);
                        if (newParent != NULL) {
                            newParent->GetChildAt(0, (IView**)&child);
                            if (child != NULL) child->RequestFocus();
                        }
                    }
                }
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_DPAD_UP:
        {
            if (handleKeyEvent) {
                // Select the closest icon in the previous row, otherwise select the tab bar
                if (y > 0) {
                    Int32 newWidgetIndex = ((y - 1) * cellCountX) + x;
                    parent->GetChildAt(newWidgetIndex, (IView**)&child);
                    if (child != NULL) child->RequestFocus();
                }
                else {
                    tabs->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_DPAD_DOWN:
        {
            if (handleKeyEvent) {
                // Select the closest icon in the previous row, otherwise do nothing
                if (y < (cellCountY - 1)) {
                    Int32 newWidgetIndex = Math::Min(widgetCount - 1, ((y + 1) * cellCountX) + x);
                    parent->GetChildAt(newWidgetIndex, (IView**)&child);
                    if (child != NULL) child->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_ENTER:
        case IKeyEvent::KEYCODE_DPAD_CENTER:
        {
            if (handleKeyEvent) {
                // Simulate a click on the widget
                AutoPtr<IViewOnClickListener> clickListener =
                        IViewOnClickListener::Probe(container);
                clickListener->OnClick(w);
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_PAGE_UP:
        {
            if (handleKeyEvent) {
                // Select the first item on the previous page, or the first item on this page
                // if there is no previous page
                if (pageIndex > 0) {
                    newParent = GetAppsCustomizePage(container, pageIndex - 1);
                    if (newParent != NULL) {
                        newParent->GetChildAt(0, (IView**)&child);
                    }
                }
                else {
                    parent->GetChildAt(0, (IView**)&child);
                }
                if (child != NULL) child->RequestFocus();
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_PAGE_DOWN:
        {
            if (handleKeyEvent) {
                // Select the first item on the next page, or the last item on this page
                // if there is no next page
                if (pageIndex < (pageCount - 1)) {
                    newParent = GetAppsCustomizePage(container, pageIndex + 1);
                    if (newParent != NULL) {
                        newParent->GetChildAt(0, (IView**)&child);
                    }
                }
                else {
                    parent->GetChildAt(widgetCount - 1, (IView**)&child);
                }
                if (child != NULL) child->RequestFocus();
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_MOVE_HOME:
        {
            if (handleKeyEvent) {
                // Select the first item on this page
                parent->GetChildAt(0, (IView**)&child);
                if (child != NULL) child->RequestFocus();
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_MOVE_END:
        {
            if (handleKeyEvent) {
                // Select the last item on this page
                AutoPtr<IView> view;
                parent->GetChildAt(widgetCount - 1, (IView**)&view);
                view->RequestFocus();
            }
            wasHandled = TRUE;
            break;
        }
        default: break;
    }
    return wasHandled;
}

Boolean FocusHelper::HandleAppsCustomizeKeyEvent(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* e)
{
    AutoPtr<IViewGroup> parentLayout;
    AutoPtr<IViewGroup> itemContainer;
    Int32 countX;
    Int32 countY;

    AutoPtr<IViewParent> parent;
    v->GetParent((IViewParent**)&parent);
    if (IPagedViewCellLayoutChildren::Probe(parent) != NULL) {
        itemContainer = IViewGroup::Probe(parent);
        AutoPtr<IViewParent> parent2;
        itemContainer->GetParent((IViewParent**)&parent2);
        parentLayout = IViewGroup::Probe(parent2);
        IPagedViewCellLayout::Probe(parentLayout)->GetCellCountX(&countX);
        IPagedViewCellLayout::Probe(parentLayout)->GetCellCountY(&countY);
    }
    else {
        parentLayout = IViewGroup::Probe(parent);
        itemContainer = parentLayout;
        IPagedViewGridLayout::Probe(parentLayout)->GetCellCountX(&countX);
        IPagedViewGridLayout::Probe(parentLayout)->GetCellCountY(&countY);
    }

    // Note we have an extra parent because of the
    // PagedViewCellLayout/PagedViewCellLayoutChildren relationship
    AutoPtr<IViewParent> _parent;
    parentLayout->GetParent((IViewParent**)&_parent);
    AutoPtr<IPagedView> container = IPagedView::Probe(_parent);
    AutoPtr<ITabHost> tabHost = FindTabHostParent(container);
    AutoPtr<ITabWidget> tabs;
    tabHost->GetTabWidget((ITabWidget**)&tabs);
    Int32 iconIndex;
    itemContainer->IndexOfChild(v, &iconIndex);
    Int32 itemCount;
    itemContainer->GetChildCount(&itemCount);
    Int32 index;
    IViewGroup::Probe(container)->IndexOfChild(parentLayout, &index);
    Int32 pageIndex;
    container->IndexToPage(index, &pageIndex);
    Int32 pageCount;
    container->GetChildCount(&pageCount);

    Int32 x = iconIndex % countX;
    Int32 y = iconIndex / countX;

    Int32 action;
    e->GetAction(&action);
    Boolean handleKeyEvent = (action != IKeyEvent::ACTION_UP);
    AutoPtr<IViewGroup> newParent;
    // Side pages do not always load synchronously, so check before focusing child siblings
    // willy-nilly
    AutoPtr<IView> child;
    Boolean wasHandled = FALSE;
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_LEFT:
        {
            if (handleKeyEvent) {
                // Select the previous icon or the last icon on the previous page
                if (iconIndex > 0) {
                    AutoPtr<IView> view;
                    itemContainer->GetChildAt(iconIndex - 1, (IView**)&view);
                    view->RequestFocus();
                }
                else {
                    if (pageIndex > 0) {
                        newParent = GetAppsCustomizePage(container, pageIndex - 1);
                        if (newParent != NULL) {
                            container->SnapToPage(pageIndex - 1);
                            Int32 count;
                            newParent->GetChildCount(&count);
                            newParent->GetChildAt(count - 1, (IView**)&child);
                            if (child != NULL) child->RequestFocus();
                        }
                    }
                }
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_DPAD_RIGHT:
        {
            if (handleKeyEvent) {
                // Select the next icon or the first icon on the next page
                if (iconIndex < (itemCount - 1)) {
                    AutoPtr<IView> view;
                    itemContainer->GetChildAt(iconIndex + 1, (IView**)&view);
                    view->RequestFocus();
                }
                else {
                    if (pageIndex < (pageCount - 1)) {
                        newParent = GetAppsCustomizePage(container, pageIndex + 1);
                        if (newParent != NULL) {
                            container->SnapToPage(pageIndex + 1);
                            newParent->GetChildAt(0, (IView**)&child);
                            if (child != NULL) child->RequestFocus();
                        }
                    }
                }
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_DPAD_UP:
        {
            if (handleKeyEvent) {
                // Select the closest icon in the previous row, otherwise select the tab bar
                if (y > 0) {
                    Int32 newiconIndex = ((y - 1) * countX) + x;
                    AutoPtr<IView> view;
                    itemContainer->GetChildAt(newiconIndex, (IView**)&view);
                    view->RequestFocus();
                }
                else {
                    tabs->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_DPAD_DOWN:
        {
            if (handleKeyEvent) {
                // Select the closest icon in the previous row, otherwise do nothing
                if (y < (countY - 1)) {
                    Int32 newiconIndex = Math::Min(itemCount - 1, ((y + 1) * countX) + x);
                    AutoPtr<IView> view;
                    itemContainer->GetChildAt(newiconIndex, (IView**)&view);
                    view->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_ENTER:
        case IKeyEvent::KEYCODE_DPAD_CENTER:
        {
            if (handleKeyEvent) {
                // Simulate a click on the icon
                AutoPtr<IViewOnClickListener> clickListener =
                        IViewOnClickListener::Probe(container);
                clickListener->OnClick(v);
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_PAGE_UP:
        {
            if (handleKeyEvent) {
                // Select the first icon on the previous page, or the first icon on this page
                // if there is no previous page
                if (pageIndex > 0) {
                    newParent = GetAppsCustomizePage(container, pageIndex - 1);
                    if (newParent != NULL) {
                        container->SnapToPage(pageIndex - 1);
                        newParent->GetChildAt(0, (IView**)&child);
                        if (child != NULL) child->RequestFocus();
                    }
                }
                else {
                    AutoPtr<IView> view;
                    itemContainer->GetChildAt(0, (IView**)&view);
                    view->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_PAGE_DOWN:
        {
            if (handleKeyEvent) {
                // Select the first icon on the next page, or the last icon on this page
                // if there is no next page
                if (pageIndex < (pageCount - 1)) {
                    newParent = GetAppsCustomizePage(container, pageIndex + 1);
                    if (newParent != NULL) {
                        container->SnapToPage(pageIndex + 1);
                        newParent.getChildAt(0, (IView**)&child);
                        if (child != NULL) child->RequestFocus();
                    }
                }
                else {
                    AutoPtr<IView> view;
                    itemContainer->GetChildAt(itemCount - 1, (IView**)&view);
                    view->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_MOVE_HOME:
        {
            if (handleKeyEvent) {
                // Select the first icon on this page
                AutoPtr<IView> view;
                itemContainer->GetChildAt(0, (IView**)&view);
                view->RequestFocus();
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_MOVE_END:
        {
            if (handleKeyEvent) {
                // Select the last icon on this page
                AutoPtr<IView> view;
                itemContainer->GetChildAt(itemCount - 1, (IView**)&view);
                view->RequestFocus();
            }
            wasHandled = TRUE;
            break;
        }
        default: break;
    }
    return wasHandled;
}

Boolean FocusHelper::HandleTabKeyEvent(
    /* [in] */ IAccessibleTabView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* e)
{
    if (!LauncherApplication::IsScreenLarge()) return FALSE;

    AutoPtr<IViewParent> _parent;
    v->GetParent((IViewParent**)&_parent);
    AutoPtr<IFocusOnlyTabWidget> parent = IFocusOnlyTabWidget::Probe(_parent);
    AutoPtr<ITabHost> tabHost = FindTabHostParent(parent);
    AutoPtr<IViewGroup> contents;
    tabHost->GetTabContentView((IViewGroup**)&contents);
    Int32 tabCount;
    parent->GetTabCount(&tabCount);
    Int32 tabIndex;
    parent->GetChildTabIndex(v, &tabIndex);

    Int32 action;
    e->GetAction(&action);
    Boolean handleKeyEvent = (action != IKeyEvent::ACTION_UP);
    Boolean wasHandled = FALSE;
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_LEFT:
        {
            if (handleKeyEvent) {
                // Select the previous tab
                if (tabIndex > 0) {
                    AutoPtr<IView> view;
                    ITabWidget::Probe(parent)->GetChildTabViewAt(tabIndex - 1, (IView**)&view);
                    view->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_DPAD_RIGHT:
        {
            if (handleKeyEvent) {
                // Select the next tab, or if the last tab has a focus right id, select that
                if (tabIndex < (tabCount - 1)) {
                    AutoPtr<IView> view;
                    ITabWidget::Probe(parent)->GetChildTabViewAt(tabIndex + 1, (IView**)&view);
                    view->RequestFocus();
                }
                else {
                    Int32 id;
                    IView::Probe(v)->GetNextFocusRightId(&id);
                    if (id != IView::NO_ID) {
                        AutoPtr<IView> view;
                        tabHost->FindViewById(id, (IView**)&view);
                        view->RequestFocus();
                    }
                }
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_DPAD_UP:
        {
            // Do nothing
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_DPAD_DOWN:
        {
            if (handleKeyEvent) {
                // Select the content view
                contents->RequestFocus();
            }
            wasHandled = TRUE;
            break;
        }
        default: break;
    }
    return wasHandled;
}

Boolean FocusHelper::HandleHotseatButtonKeyEvent(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* e,
    /* [in] */ Int32 orientation)
{
    AutoPtr<IViewParent> viewParent;
    v->GetParent((IViewParent**)&viewParent);
    AutoPtr<IViewGroup> parent = IViewGroup::Probe(viewParent);

    AutoPtr<IViewParent> viewParent2;
    parent->GetParent((IViewParent**)&viewParent2);
    AutoPtr<IViewGroup> launcher = IViewGroup::Probe(viewParent2);

    AutoPtr<IView> view;
    launcher->FindViewById(
            Elastos::Droid::Launcher2::R::id::workspace,
            (IView**)&view);
    AutoPtr<IWorkspace> workspace = IWorkspace::Probe(view);
    Int32 buttonIndex;
    parent->IndexOfChild(v, &buttonIndex);
    Int32 buttonCount;
    parent->GetChildCount(&buttonCount);
    Int32 pageIndex;
    workspace->GetCurrentPage(&pageIndex);

    // NOTE: currently we don't special case for the phone UI in different
    // orientations, even though the hotseat is on the side in landscape mode.  This
    // is to ensure that accessibility consistency is maintained across rotations.

    Int32 action;
    e->GetAction(&action);
    const Boolean handleKeyEvent = (action != IKeyEvent::ACTION_UP);
    Boolean wasHandled = FALSE;
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_LEFT:
            if (handleKeyEvent) {
                // Select the previous button, otherwise snap to the previous page
                if (buttonIndex > 0) {
                    AutoPtr<IView> view;
                    parent->GetChildAt(buttonIndex - 1, (IView**)&view);
                    view->RequestFocus();
                }
                else {
                    workspace->SnapToPage(pageIndex - 1);
                }
            }
            wasHandled = TRUE;
            break;
        case IKeyEvent::KEYCODE_DPAD_RIGHT:
            if (handleKeyEvent) {
                // Select the next button, otherwise snap to the next page
                if (buttonIndex < (buttonCount - 1)) {
                    AutoPtr<IView> view;
                    parent->GetChildAt(buttonIndex + 1, (IView**)&view);
                    view->RequestFocus();
                }
                else {
                    workspace->SnapToPage(pageIndex + 1);
                }
            }
            wasHandled = TRUE;
            break;
        case IKeyEvent::KEYCODE_DPAD_UP:
            if (handleKeyEvent) {
                // Select the first bubble text view in the current page of the workspace
                AutoPtr<IView> view;
                workspace->GetChildAt(pageIndex, (IView**)&view);
                AutoPtr<ICellLayout> layout = ICellLayout::Probe(view);
                AutoPtr<IShortcutAndWidgetContainer> children;
                layout->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&children);
                AutoPtr<IView> newIcon = GetIconInDirection(layout, children, -1, 1);
                if (newIcon != NULL) {
                    newIcon->RequestFocus();
                }
                else {
                    workspace->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        case IKeyEvent::KEYCODE_DPAD_DOWN:
            // Do nothing
            wasHandled = TRUE;
            break;
        default: break;
    }
    return wasHandled;
}

AutoPtr<IShortcutAndWidgetContainer> FocusHelper::GetCellLayoutChildrenForIndex(
    /* [in] */ IViewGroup* container,
    /* [in] */ Int32 i)
{
    AutoPtr<IView> view;
    container->GetChildAt(i, (IView**)&view);
    AutoPtr<IViewGroup> parent = IViewGroup::Probe(view);

    AutoPtr<IView> view2;
    parent->GetChildAt(0, (IView**)&view2);
    AutoPtr<IShortcutAndWidgetContainer> container =
            IShortcutAndWidgetContainer::Probe(view2);
    return container;
}

AutoPtr<IArrayList> FocusHelper::GetCellLayoutChildrenSortedSpatially(
    /* [in] */ ICellLayout* layout,
    /* [in] */ IViewGroup* parent)
{
    // First we order each the CellLayout children by their x,y coordinates
    Int32 cellCountX;
    layout->GetCountX(&cellCountX);
    Int32 count;
    parent->GetChildCount(&count);
    AutoPtr<IArrayList> views;
    CArrayList::New((IArrayList**)&views);
    for (Int32 j = 0; j < count; ++j) {
        AutoPtr<IView> view;
        parent->GetChildAt(j, (IView**)&view);
        views->Add(TO_IINTERFACE(view));
    }
    AutoPtr<IComparator> cmp = new MyComparator(cellCountX);
    Collections::Sort(views, cmp);
    return views;
}

AutoPtr<IView> FocusHelper::FindIndexOfIcon(
    /* [in] */ IArrayList* views,
    /* [in] */ Int32 i,
    /* [in] */ Int32 delta)
{
    // Then we find the next BubbleTextView offset by delta from i
    Int32 count;
    views->GetSize(&count);
    Int32 newI = i + delta;
    while (0 <= newI && newI < count) {
        AutoPtr<IInterface> obj;
        views->Get(newI, (IInterface**)&obj);
        AutoPtr<IView> newV = IView::Probe(obj);
        if (IBubbleTextView::Probe(newV) != NULL
                || IFolderIcon::Probe(newV) != NULL) {
            return newV;
        }
        newI += delta;
    }
    return NULL;
}

AutoPtr<IView> FocusHelper::GetIconInDirection(
    /* [in] */ ICellLayout* layout,
    /* [in] */ IViewGroup* parent,
    /* [in] */ Int32 i,
    /* [in] */ Int32 delta)
{
    AutoPtr<IArrayList> views = GetCellLayoutChildrenSortedSpatially(layout, parent);
    return FindIndexOfIcon(views, i, delta);
}

AutoPtr<IView> FocusHelper::GetIconInDirection(
    /* [in] */ ICellLayout* layout,
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* v,
    /* [in] */ Int32 delta)
{
    AutoPtr<IArrayList> views = GetCellLayoutChildrenSortedSpatially(layout, parent);
    Int32 i;
    views->IndexOf(v, &i);
    return FindIndexOfIcon(views, i, delta);
}

AutoPtr<IView> FocusHelper::GetClosestIconOnLine(
    /* [in] */ ICellLayout* layout,
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* v,
    /* [in] */ Int32 lineDelta)
{
    AutoPtr<IArrayList> views = GetCellLayoutChildrenSortedSpatially(layout, parent);
    AutoPtr<IViewGroupLayoutParams> params;
    v->GetLayoutParams((IViewGroupLayoutParams**)&params);
    AutoPtr<ICellLayoutLayoutParams> lp = ICellLayoutLayoutParams::Probe(params);
    Int32 cellCountY;
    layout->GetCountY(&cellCountY);
    Int32 row;
    lp->GetCellY(&row);
    Int32 newRow = row + lineDelta;
    if (0 <= newRow && newRow < cellCountY) {
        Float closestDistance = Float.MAX_VALUE;
        Int32 closestIndex = -1;
        Int32 index;
        views->IndexOf(v, &index);
        Int32 size;
        views->GetSize(&size);
        Int32 endIndex = (lineDelta < 0) ? -1 : size;
        while (index != endIndex) {
            AutoPtr<IInterface> obj;
            views->Get(index, (IInterface**)&obj);
            AutoPtr<IView> newV = IView::Probe(obj);
            AutoPtr<IViewGroupLayoutParams> _params;
            newV->GetLayoutParams((IViewGroupLayoutParams**)&_params);
            AutoPtr<ICellLayoutLayoutParams> tmpLp = ICellLayoutLayoutParams::Probe(_params);
            Int32 tmpLpY;
            tmpLp->GetCellY(&tmpLpY);
            Boolean satisfiesRow = (lineDelta < 0) ? (tmpLpY < row) : (tmpLpY > row);
            if (satisfiesRow && (IBubbleTextView::Probe(newV) != NULL
                        || IFolderIcon::Probe(newV) != NULL)) {
                Int32 tmpLpX;
                tmpLp->GetCellX(&tmpLpX);
                Int32 lpX;
                lp->GetCellX(&lpX);
                Int32 lpY;
                lp->GetCellY(&lpY);
                Float tmpDistance = (Float)Math::Sqrt(Math::Pow(tmpLpX - lpX, 2) +
                        Math.pow(tmpLpY - lpY, 2));
                if (tmpDistance < closestDistance) {
                    closestIndex = index;
                    closestDistance = tmpDistance;
                }
            }
            if (index <= endIndex) {
                ++index;
            }
            else {
                --index;
            }
        }
        if (closestIndex > -1) {
            AutoPtr<IInterface> obj;
            views->Get(closestIndex, (IInterface**)&obj);
            AutoPtr<IView> tmp = IView::Probe(obj);
            return tmp;
        }
    }
    return NULL;
}

Boolean FocusHelper::HandleIconKeyEvent(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* e)
{
    AutoPtr<IViewParent> viewParent;
    v->GetParent((IViewParent**)&viewParent);
    AutoPtr<IShortcutAndWidgetContainer> parent =
            IShortcutAndWidgetContainer::Probe(viewParent);

    AutoPtr<IViewParent> viewParent2;
    parent->GetParent((IViewParent**)&viewParent2);
    AutoPtr<ICellLayout> layout = ICellLayout::Probe(viewParent2);

    AutoPtr<IViewParent> viewParent3;
    layout->GetParent((IViewParent**)&viewParent3);
    AutoPtr<IWorkspace> workspace = IWorkspace::Probe(viewParent3);

    AutoPtr<IViewParent> viewParent4;
    workspace->GetParent((IViewParent**)&viewParent4);
    AutoPtr<IViewGroup> launcher = IViewGroup::Probe(viewParent4);

    AutoPtr<IView> view;
    launcher->FindViewById(
            Elastos::Droid::Launcher2::R::id::qsb_bar, (IView**)&view);
    AutoPtr<IViewGroup> tabs = IViewGroup::Probe(view);

    AutoPtr<IView> view2;
    launcher->FindViewById(
            Elastos::Droid::Launcher2::R::id::hotseat, (IView**)&view2);
    AutoPtr<IViewGroup> hotseat = IViewGroup::Probe(view2);

    Int32 pageIndex;
    workspace->IndexOfChild(layout, &pageIndex);
    Int32 pageCount;
    workspace->GetChildCount(&pageCount);

    Int32 action;
    e->GetAction(&action);
    const Boolean handleKeyEvent = (action != IKeyEvent::ACTION_UP);
    Boolean wasHandled = FALSE;
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_LEFT:
        {
            if (handleKeyEvent) {
                // Select the previous icon or the last icon on the previous page if possible
                AutoPtr<IView> newIcon = GetIconInDirection(layout, parent, v, -1);
                if (newIcon != NULL) {
                    newIcon->RequestFocus();
                }
                else {
                    if (pageIndex > 0) {
                        parent = GetCellLayoutChildrenForIndex(workspace, pageIndex - 1);
                        Int32 count;
                        parent->GetChildCount(&count);
                        newIcon = GetIconInDirection(layout, parent, count, -1);
                        if (newIcon != NULL) {
                            newIcon->RequestFocus();
                        }
                        else {
                            // Snap to the previous page
                            workspace->SnapToPage(pageIndex - 1);
                        }
                    }
                }
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_DPAD_RIGHT:
        {
            if (handleKeyEvent) {
                // Select the next icon or the first icon on the next page if possible
                AutoPtr<IView> newIcon = GetIconInDirection(layout, parent, v, 1);
                if (newIcon != NULL) {
                    newIcon->RequestFocus();
                }
                else {
                    if (pageIndex < (pageCount - 1)) {
                        parent = GetCellLayoutChildrenForIndex(workspace, pageIndex + 1);
                        newIcon = GetIconInDirection(layout, parent, -1, 1);
                        if (newIcon != NULL) {
                            newIcon->RequestFocus();
                        }
                        else {
                            // Snap to the next page
                            workspace->SnapToPage(pageIndex + 1);
                        }
                    }
                }
            }
            wasHandled = TRUE;
            break;
        }
        case IKeyEvent::KEYCODE_DPAD_UP:
        {
            if (handleKeyEvent) {
                // Select the closest icon in the previous line, otherwise select the tab bar
                AutoPtr<IView> newIcon = GetClosestIconOnLine(layout, parent, v, -1);
                if (newIcon != NULL) {
                    newIcon->RequestFocus();
                    wasHandled = TRUE;
                }
                else {
                    tabs0>RequestFocus();
                }
            }
            break;
        }
        case IKeyEvent::KEYCODE_DPAD_DOWN:
            if (handleKeyEvent) {
                // Select the closest icon in the next line, otherwise select the button bar
                AutoPtr<IView> newIcon = GetClosestIconOnLine(layout, parent, v, 1);
                if (newIcon != NULL) {
                    newIcon->RequestFocus();
                    wasHandled = TRUE;
                } else if (hotseat != NULL) {
                    hotseat->RequestFocus();
                }
            }
            break;
        case IKeyEvent::KEYCODE_PAGE_UP:
            if (handleKeyEvent) {
                // Select the first icon on the previous page or the first icon on this page
                // if there is no previous page
                if (pageIndex > 0) {
                    parent = GetCellLayoutChildrenForIndex(workspace, pageIndex - 1);
                    AutoPtr<IView> newIcon = GetIconInDirection(layout, parent, -1, 1);
                    if (newIcon != NULL) {
                        newIcon->RequestFocus();
                    }
                    else {
                        // Snap to the previous page
                        workspace->SnapToPage(pageIndex - 1);
                    }
                }
                else {
                    AutoPtr<IView> newIcon = GetIconInDirection(layout, parent, -1, 1);
                    if (newIcon != NULL) {
                        newIcon->RequestFocus();
                    }
                }
            }
            wasHandled = TRUE;
            break;
        case IKeyEvent::KEYCODE_PAGE_DOWN:
            if (handleKeyEvent) {
                // Select the first icon on the next page or the last icon on this page
                // if there is no previous page
                if (pageIndex < (pageCount - 1)) {
                    parent = GetCellLayoutChildrenForIndex(workspace, pageIndex + 1);
                    AutoPtr<IView> newIcon = GetIconInDirection(layout, parent, -1, 1);
                    if (newIcon != NULL) {
                        newIcon->RequestFocus();
                    }
                    else {
                        // Snap to the next page
                        workspace->SnapToPage(pageIndex + 1);
                    }
                }
                else {
                    Int32 count;
                    parent->GetChildCount(&count);
                    AutoPtr<IView> newIcon = GetIconInDirection(layout, parent,
                            count, -1);
                    if (newIcon != NULL) {
                        newIcon->RequestFocus();
                    }
                }
            }
            wasHandled = TRUE;
            break;
        case IKeyEvent::KEYCODE_MOVE_HOME:
            if (handleKeyEvent) {
                // Select the first icon on this page
                AutoPtr<IView> newIcon = getIconInDirection(layout, parent, -1, 1);
                if (newIcon != NULL) {
                    newIcon->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        case IKeyEvent::KEYCODE_MOVE_END:
            if (handleKeyEvent) {
                // Select the last icon on this page
                Int32 count;
                parent->GetChildCount(&count);
                AutoPtr<IView> newIcon = GetIconInDirection(layout, parent,
                        count, -1);
                if (newIcon != NULL) {
                    newIcon->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        default: break;
    }
    return wasHandled;
}

Boolean FocusHelper::HandleFolderKeyEvent(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* e)
{
    AutoPtr<IViewParent> viewParent;
    v->GetParent((IViewParent**)&viewParent);
    AutoPtr<IShortcutAndWidgetContainer> parent =
            IShortcutAndWidgetContainer::Probe(viewParent);

    AutoPtr<IViewParent> viewParent2;
    parent->GetParent((IViewParent**)&viewParent2);
    AutoPtr<ICellLayout> layout = ICellLayout::Probe(viewParent2);

    AutoPtr<IViewParent> viewParent3;
    layout->GetParent((IViewParent**)&viewParent3);
    AutoPtr<IFolder> folder = IFolder::Probe(viewParent3);

    AutoPtr<IView> title;
    folder->GetFolderName((IView**)&title);

    Int32 action;
    e->GetAction(&action);
    const Boolean handleKeyEvent = (action != IKeyEvent::ACTION_UP);
    Boolean wasHandled = FALSE;
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_LEFT:
            if (handleKeyEvent) {
                // Select the previous icon
                AutoPtr<IView> newIcon = GetIconInDirection(layout, parent, v, -1);
                if (newIcon != NULL) {
                    newIcon->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        case IKeyEvent::KEYCODE_DPAD_RIGHT:
            if (handleKeyEvent) {
                // Select the next icon
                AutoPtr<IView> newIcon = GetIconInDirection(layout, parent, v, 1);
                if (newIcon != NULL) {
                    newIcon->RequestFocus();
                }
                else {
                    title->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        case IKeyEvent::KEYCODE_DPAD_UP:
            if (handleKeyEvent) {
                // Select the closest icon in the previous line
                AutoPtr<IView> newIcon = GetClosestIconOnLine(layout, parent, v, -1);
                if (newIcon != NULL) {
                    newIcon->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        case IKeyEvent::KEYCODE_DPAD_DOWN:
            if (handleKeyEvent) {
                // Select the closest icon in the next line
                AutoPtr<IView> newIcon = GetClosestIconOnLine(layout, parent, v, 1);
                if (newIcon != NULL) {
                    newIcon->RequestFocus();
                }
                else {
                    title->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        case IKeyEvent::KEYCODE_MOVE_HOME:
            if (handleKeyEvent) {
                // Select the first icon on this page
                AutoPtr<IView> newIcon = GetIconInDirection(layout, parent, -1, 1);
                if (newIcon != NULL) {
                    newIcon->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        case IKeyEvent::KEYCODE_MOVE_END:
            if (handleKeyEvent) {
                // Select the last icon on this page
                Int32 count;
                parent->GetChildCount(&count);
                AutoPtr<IView> newIcon = GetIconInDirection(layout, parent,
                        count, -1);
                if (newIcon != NULL) {
                    newIcon->RequestFocus();
                }
            }
            wasHandled = TRUE;
            break;
        default: break;
    }
    return wasHandled;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos