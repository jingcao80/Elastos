#ifndef  __ELASTOS_DROID_LAUNCHER2_FOCUSHELPER_H__
#define  __ELASTOS_DROID_LAUNCHER2_FOCUSHELPER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Core::Object;
using Elastos::Core::IComparator;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * A keyboard listener we set on all the workspace icons.
 */
class IconKeyEventListener
    : public Object
    , public IViewOnKeyListener
{
public:
    CAR_INTERFACE_DECL();

    CARAPI OnKey(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);
};

/**
 * A keyboard listener we set on all the workspace icons.
 */
class FolderKeyEventListener
    : public Object
    , public IViewOnKeyListener
{
public:
    CAR_INTERFACE_DECL();

    CARAPI OnKey(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);
};

/**
 * A keyboard listener we set on all the hotseat buttons.
 */
class HotseatIconKeyEventListener
    : public Object
    , public IViewOnKeyListener
{
public:
    CAR_INTERFACE_DECL();

    CARAPI OnKey(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);
};

/**
 * A keyboard listener we set on the last tab button in AppsCustomize to jump to then
 * market icon and vice versa.
 */
class AppsCustomizeTabKeyEventListener
    : public Object
    , public IViewOnKeyListener
{
public:
    CAR_INTERFACE_DECL();

    CARAPI OnKey(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);
};


class FocusHelper
{
private:
    class MyComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL();

        MyComparator(
            /* [in] */ Int32 cellCountX);

        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);

    private:
        Int32 mCellCountX;
    };

public:
    /**
     * Handles key events in a AppsCustomize tab between the last tab view and the shop button.
     */
    static CARAPI_(Boolean) HandleAppsCustomizeTabKeyEvent(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* e);

    /**
     * Handles key events in a PageViewExtendedLayout containing PagedViewWidgets.
     */
    static CARAPI_(Boolean) HandlePagedViewGridLayoutWidgetKeyEvent(
        /* [in] */ IPagedViewWidget* w,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* e);

    /**
     * Handles key events in a PageViewCellLayout containing PagedViewIcons.
     */
    static CARAPI_(Boolean) HandleAppsCustomizeKeyEvent(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* e);

    /**
     * Handles key events in the tab widget.
     */
    static CARAPI_(Boolean) HandleTabKeyEvent(
        /* [in] */ IAccessibleTabView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* e);

    /**
     * Handles key events in the workspace hotseat (bottom of the screen).
     */
    static CARAPI_(Boolean) HandleHotseatButtonKeyEvent(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* e,
        /* [in] */ Int32 orientation);

    /**
     * Handles key events in a Workspace containing.
     */
    static CARAPI_(Boolean) HandleIconKeyEvent(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* e);

    /**
     * Handles key events for items in a Folder.
     */
    static CARAPI_(Boolean) HandleFolderKeyEvent(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* e);

private:
    /**
     * Private helper to get the parent TabHost in the view hiearchy.
     */
    static CARAPI_(AutoPtr<ITabHost>) FindTabHostParent(
        /* [in] */ IView* v);

    /**
     * Returns the Viewgroup containing page contents for the page at the index specified.
     */
    static CARAPI_(AutoPtr<IViewGroup>) GetAppsCustomizePage(
        /* [in] */ IViewGroup* container,
        /* [in] */ Int32 index);

    /**
     * Private helper method to get the CellLayoutChildren given a CellLayout index.
     */
    static CARAPI_(AutoPtr<IShortcutAndWidgetContainer>) GetCellLayoutChildrenForIndex(
        /* [in] */ IViewGroup* container,
        /* [in] */ Int32 i);

    /**
     * Private helper method to sort all the CellLayout children in order of their (x,y) spatially
     * from top left to bottom right.
     */
    static CARAPI_(AutoPtr<IArrayList>) GetCellLayoutChildrenSortedSpatially(
        /* [in] */ ICellLayout* layout,
        /* [in] */ IViewGroup* parent);

    /**
     * Private helper method to find the index of the next BubbleTextView or FolderIcon in the
     * direction delta.
     *
     * @param delta either -1 or 1 depending on the direction we want to search
     */
    static CARAPI_(AutoPtr<IView>) FindIndexOfIcon(
        /* [in] */ IArrayList* views,
        /* [in] */ Int32 i,
        /* [in] */ Int32 delta);

    static CARAPI_(AutoPtr<IView>) GetIconInDirection(
        /* [in] */ ICellLayout* layout,
        /* [in] */ IViewGroup* parent,
        /* [in] */ Int32 i,
        /* [in] */ Int32 delta);

    static CARAPI_(AutoPtr<IView>) GetIconInDirection(
        /* [in] */ ICellLayout* layout,
        /* [in] */ IViewGroup* parent,
        /* [in] */ IView* v,
        /* [in] */ Int32 delta);
    /**
     * Private helper method to find the next closest BubbleTextView or FolderIcon in the direction
     * delta on the next line.
     *
     * @param delta either -1 or 1 depending on the line and direction we want to search
     */
    static CARAPI_(AutoPtr<IView>) GetClosestIconOnLine(
        /* [in] */ ICellLayout* layout,
        /* [in] */ IViewGroup* parent,
        /* [in] */ IView* v,
        /* [in] */ Int32 lineDelta);
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_FOCUSHELPER_H__