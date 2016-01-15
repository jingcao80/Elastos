
#include "elastos/droid/packages/systemui/FontSizeUtils.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::ITypedValue;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {

const Float FontSizeUtils::LARGE_TEXT_SCALE = 1.3f;
void FontSizeUtils::UpdateFontSize(
    /* [in] */ IView* parent,
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 dimensId)
{
    AutoPtr<IView> view;
    parent->FindViewById(viewId, (IView**)&view);
    UpdateFontSize(ITextView::Probe(view), dimensId);
}

void FontSizeUtils::UpdateFontSize(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 dimensId)
{
    if (v != NULL) {
        AutoPtr<IResources> res;
        IView::Probe(v)->GetResources((IResources**)&res);
        Int32 size = 0;
        res->GetDimensionPixelSize(dimensId, &size);
        v->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, size);
    }
}

} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos
