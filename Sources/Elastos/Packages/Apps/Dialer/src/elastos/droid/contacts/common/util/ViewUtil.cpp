
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/contacts/common/util/ViewUtil.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewOutlineProvider;
using Elastos::Droid::Dialer::R;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

const AutoPtr<ViewOutlineProvider> ViewUtil::OVAL_OUTLINE_PROVIDER = new OVALViewOutlineProvide();
const AutoPtr<ViewOutlineProvider> ViewUtil::RECT_OUTLINE_PROVIDER = new RECTViewOutlineProvide();

//=================================================================
// ViewUtil::OVALViewOutlineProvide
//=================================================================
ECode ViewUtil::OVALViewOutlineProvide::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    Int32 width, height;
    view->GetWidth(&width);
    view->GetHeight(&height);
    outline->SetOval(0, 0, width, height);
    return NOERROR;
}


//=================================================================
// ViewUtil::RECTViewOutlineProvide
//=================================================================
ECode ViewUtil::RECTViewOutlineProvide::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    Int32 width, height;
    view->GetWidth(&width);
    view->GetHeight(&height);
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
    VALIDATE_NOT_NULL(result);

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
    res->GetDimensionPixelSize(R::dimen::floating_action_button_translation_z, &size);
    view->SetTranslationZ(size);
}

void ViewUtil::AddBottomPaddingToListViewForFab(
    /* [in] */ IListView* listView,
    /* [in] */ IResources* res)
{
    Int32 fabPadding;
    res->GetDimensionPixelSize(
            R::dimen::floating_action_button_list_bottom_padding, &fabPadding);
    Int32 paddingStart, paddingTop, paddingEnd, paddingBottom;
    IView* view = IView::Probe(listView);
    view->GetPaddingStart(&paddingStart);
    view->GetPaddingTop(&paddingTop);
    view->GetPaddingEnd(&paddingEnd);
    view->GetPaddingBottom(&paddingBottom);
    view->SetPaddingRelative(paddingStart, paddingTop,
            paddingEnd, paddingBottom + fabPadding);
    IViewGroup::Probe(listView)->SetClipToPadding(FALSE);
}

} // Util
} // Common
} // Contacts
} // Droid
} // Elastos
