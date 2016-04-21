#include "ViewUtil.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

const AutoPtr<IViewOutlineProvider> ViewUtil::OVAL_OUTLINE_PROVIDER = new OVALViewOutlineProvide();
const AutoPtr<IViewOutlineProvider> ViewUtil::RECT_OUTLINE_PROVIDER = new RECTViewOutlineProvide();

//=================================================================
// ViewUtil::OVALViewOutlineProvide
//=================================================================
CAR_INTERFACE_IMPL(ViewUtil::OVALViewOutlineProvide, Object, IViewOutlineProvider)

ECode ViewUtil::OVALViewOutlineProvide::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    Int32 width, height;
    view->GetWidth(&width);
    view->GetHeight(&height)
    outline->SetOval(0, 0, width, height);

    return NOERROR;
}

//=================================================================
// ViewUtil::RECTViewOutlineProvide
//=================================================================
CAR_INTERFACE_IMPL(ViewUtil::RECTViewOutlineProvide, Object, IViewOutlineProvider)

ECode ViewUtil::RECTViewOutlineProvide::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    Int32 width, height;
    view->GetWidth(&width);
    view->GetHeight(&height)
    outline->SetOval(0, 0, width, height);

    return NOERROR;
}

//=================================================================
// ViewUtil
//=================================================================
ECode ViewUtil::GetConstantPreLayoutWidth(
    /* [in] */ IView* view,
    /* [out] */ Int32* result)
{
    VALUE_NOT_NULL(result);

    // We haven't been layed out yet, so get the size from the LayoutParams
    AutoPtr<IViewGroupLayoutParams> p;
    view->GetLayoutParams((IViewGroupLayoutParams**)&p);
    Int32 width;
    p->GetWidth(&width);
    if (width < 0) {
        Logger::E(String("ViewUtil"), "Expecting view's width to be a constant rather " \
                "than a result of the layout pass");
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Expecting view's width to be a constant rather " +
        //         "than a result of the layout pass");
    }
    *result = width;

    return NOERROR;
}

Boolean ViewUtil::IsViewLayoutRtl(
    /* [in] */ IView* view)
{
    Int32 direction;
    view->GetLayoutDirection(&direction);
    return direction == IView::LAYOUT_DIRECTION_RTL;
}

void ViewUtil::AddRectangularOutlineProvider(
    /* [in] */ IView* view,
    /* [in] */ IResources* res)
{
    view->SetOutlineProvider((IViewOutlineProvider*)RECT_OUTLINE_PROVIDER);
}

void ViewUtil::SetupFloatingActionButton(
    /* [in] */ IView* view,
    /* [in] */ IResources* res)
{
    view->SetOutlineProvider((IViewOutlineProvider*)OVAL_OUTLINE_PROVIDER);
    Int32 size;
    res->GetDimensionPixelSize(R::dimen::floating_action_button_translation_z, &size)
    view.setTranslationZ(size);
}

void ViewUtil::AddBottomPaddingToListViewForFab(
    /* [in] */ IListView* listView,
    /* [in] */ IResources* res)
{
    Int32 fabPadding;
    res->GetDimensionPixelSize(
            R::dimen::floating_action_button_list_bottom_padding, &fabPadding);
    Int32 paddingStart, paddingTop, paddingEnd, paddingBottom;
    listView->GetPaddingStart(&paddingStart);
    listView->GetPaddingTop(&paddingTop);
    listView->GetPaddingEnd(&paddingEnd);
    listView->GetPaddingBottom(&paddingBottom);
    listView.setPaddingRelative(paddingStart, paddingTop,
            paddingEnd, paddingBottom + fabPadding);
    listView->SetClipToPadding(FALSE);
}

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos
