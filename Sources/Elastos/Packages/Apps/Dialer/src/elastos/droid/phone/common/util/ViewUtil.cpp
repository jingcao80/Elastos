
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/phone/common/util/ViewUtil.h"
#include "R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Text::ITextPaint;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Util {

ECode ViewUtil::MyViewOutlineProvider::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    Int32 width, height;
    view->GetWidth(&width);
    view->GetHeight(&height);
    outline->SetOval(0, 0, width, height);
    return NOERROR;
}


const AutoPtr<ViewOutlineProvider> ViewUtil::OVAL_OUTLINE_PROVIDER = new MyViewOutlineProvider();

/**
 * Returns the width as specified in the LayoutParams
 * @throws IllegalStateException Thrown if the view's width is unknown before a layout pass
 * s
 */
ECode ViewUtil::GetConstantPreLayoutWidth(
    /* [in] */ IView* view,
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    // We haven't been layed out yet, so get the size from the LayoutParams
    AutoPtr<IViewGroupLayoutParams> p;
    view->GetLayoutParams((IViewGroupLayoutParams**)&p);
    Int32 w;
    p->GetWidth(&w);
    if (w < 0) {
        Logger::E("ViewUtil", "Expecting view's width to be a constant rather "
                "than a result of the layout pass");
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Expecting view's width to be a constant rather " +
                // "than a result of the layout pass");
    }
    *width = w;
    return NOERROR;
}

/**
 * Returns a boolean indicating whether or not the view's layout direction is RTL
 *
 * @param view - A valid view
 * @return True if the view's layout direction is RTL
 */
Boolean ViewUtil::IsViewLayoutRtl(
    /* [in] */ IView* view)
{
    Int32 direction;
    view->GetLayoutDirection(&direction);
    return direction == IView::LAYOUT_DIRECTION_RTL;
}

/**
 * Configures the floating action button, clipping it to a circle and setting its translation z.
 * @param view The float action button's view.
 * @param res The resources file.
 */
void ViewUtil::SetupFloatingActionButton(
    /* [in] */ IView* view,
    /* [in] */ IResources* res)
{
    view->SetOutlineProvider(OVAL_OUTLINE_PROVIDER);
    Int32 size;
    res->GetDimensionPixelSize(Elastos::Droid::Dialer::R::dimen::floating_action_button_translation_z, &size);
    view->SetTranslationZ(size);
}

/**
 * Adds padding to the bottom of the given {@link ListView} so that the floating action button
 * does not obscure any content.
 *
 * @param listView to add the padding to
 * @param res valid resources object
 */
void ViewUtil::AddBottomPaddingToListViewForFab(
    /* [in] */ IListView* listView,
    /* [in] */ IResources* res)
{
    Int32 fabPadding;
    res->GetDimensionPixelSize(
            Elastos::Droid::Dialer::R::dimen::floating_action_button_list_bottom_padding, &fabPadding);
    IView* view = IView::Probe(listView);
    Int32 start, top, end, bottom;
    view->GetPaddingStart(&start);
    view->GetPaddingTop(&top);
    view->GetPaddingEnd(&end);
    view->GetPaddingBottom(&bottom);
    view->SetPaddingRelative(start, top, end, bottom + fabPadding);
    IViewGroup::Probe(listView)->SetClipToPadding(FALSE);
}

void ViewUtil::ResizeText(
    /* [in] */ ITextView* textView,
    /* [in] */ Int32 originalTextSize,
    /* [in] */ Int32 minTextSize)
{
    AutoPtr<ITextPaint> paint;
    textView->GetPaint((ITextPaint**)&paint);
    Int32 width;
    IView::Probe(textView)->GetWidth(&width);
    if (width == 0) return;
    textView->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, originalTextSize);
    AutoPtr<ICharSequence> text;
    textView->GetText((ICharSequence**)&text);
    String textStr;
    text->ToString(&textStr);
    Float textWidth;
    IPaint::Probe(paint)->MeasureText(textStr, &textWidth);
    Float ratio = width / textWidth;
    if (ratio <= 1.0f) {
        textView->SetTextSize(ITypedValue::COMPLEX_UNIT_PX,
                Elastos::Core::Math::Max((Float)minTextSize, originalTextSize * ratio));
    }
}

} // namespace Util
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos
