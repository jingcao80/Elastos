
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/style/RasterizerSpan.h"

using Elastos::Droid::Graphics::IPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_2(RasterizerSpan, CharacterStyle, IRasterizerSpan, IUpdateAppearance)

RasterizerSpan::RasterizerSpan()
{}

RasterizerSpan::~RasterizerSpan()
{}

ECode RasterizerSpan::constructor(
    /* [in] */ IRasterizer* r)
{
    mRasterizer = r;
    return NOERROR;
}

ECode RasterizerSpan::GetRasterizer(
    /* [out] */ IRasterizer** rastersizer)
{
    VALIDATE_NOT_NULL(rastersizer)
    *rastersizer = mRasterizer;
    REFCOUNT_ADD(*rastersizer)
    return NOERROR;
}

ECode RasterizerSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    IPaint::Probe(ds)->SetRasterizer(mRasterizer);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos
