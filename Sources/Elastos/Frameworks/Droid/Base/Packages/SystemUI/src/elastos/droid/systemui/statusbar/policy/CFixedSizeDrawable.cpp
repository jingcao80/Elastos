#include "elastos/droid/systemui/statusbar/policy/CFixedSizeDrawable.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

IDRAWABLE_METHODS_IMPL(CFixedSizeDrawable, FixedSizeDrawable)

ECode CFixedSizeDrawable::constructor(
    /* [in] */ IDrawable* that)
{
    return FixedSizeDrawable::Init(that);
}

PInterface CFixedSizeDrawable::Probe(
    /* [in] */ REIID riid)
{
    return _CFixedSizeDrawable::Probe(riid);
}

ECode CFixedSizeDrawable::SetFixedBounds(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    return FixedSizeDrawable::SetFixedBounds(l, t, r, b);
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
