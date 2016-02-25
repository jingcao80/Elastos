#include "elastos/droid/systemui/statusbar/phone/CFastColorDrawable.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {


IDRAWABLE_METHODS_IMPL(CFastColorDrawable, FastColorDrawable)

PInterface CFastColorDrawable::Probe(
    /* [in] */ REIID riid)
{
    return _CFastColorDrawable::Probe(riid);
}

ECode CFastColorDrawable::constructor(
    /* [in] */ Int32 color)
{
    return FastColorDrawable::Init(color);
}


}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
