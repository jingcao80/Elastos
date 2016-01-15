#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/text/CBidiFormatterBuilder.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CBidiFormatterBuilder, Object, IBidiFormatterBuilder)

CAR_OBJECT_IMPL(CBidiFormatterBuilder)

ECode CBidiFormatterBuilder::constructor()
{
    return NOERROR;
}

ECode CBidiFormatterBuilder::constructor(
    /* [in] */ Boolean rtlContext)
{
    return NOERROR;
}

ECode CBidiFormatterBuilder::constructor(
    /* [in] */ ILocale* locale)
{
    return NOERROR;
}


ECode CBidiFormatterBuilder::StereoReset(
    /* [in] */ Boolean stereoReset,
    /* [out] */ IBidiFormatterBuilder** ret)
{
    return NOERROR;
}

ECode CBidiFormatterBuilder::SetTextDirectionHeuristic(
    /* [in] */ ITextDirectionHeuristic* heuristic,
    /* [out] */ IBidiFormatterBuilder** ret)
{
    return NOERROR;
}

ECode CBidiFormatterBuilder::Build(
    /* [out] */ IBidiFormatter** ret)
{
    return NOERROR;
}


} // namespace Text
} // namespace Droid
} // namespace Elastos