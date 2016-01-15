
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/style/MaskFilterSpan.h"

using Elastos::Droid::Graphics::IPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_2(MaskFilterSpan, CharacterStyle, IMaskFilterSpan, IUpdateAppearance)

MaskFilterSpan::MaskFilterSpan()
{}

MaskFilterSpan::~MaskFilterSpan()
{}

ECode MaskFilterSpan::constructor(
    /* [in] */ IMaskFilter* filter)
{
    mFilter = filter;
    return NOERROR;
}

ECode MaskFilterSpan::GetMaskFilter(
    /* [out] */ IMaskFilter** filter)
{
    VALIDATE_NOT_NULL(filter)
    *filter = mFilter;
    REFCOUNT_ADD(*filter)
    return NOERROR;
}

ECode MaskFilterSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    IPaint::Probe(ds)->SetMaskFilter( mFilter );
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos
