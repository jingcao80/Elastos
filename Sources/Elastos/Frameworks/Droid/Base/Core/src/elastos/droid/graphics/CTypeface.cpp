
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CTypeface.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CTypeface);
ECode CTypeface::constructor(
    /* [in] */ Int64 ni)
{
    return Typeface::constructor(ni);
}

PInterface CTypeface::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_Typeface) {
        return reinterpret_cast<PInterface>((Typeface*)this);
    }
    else if (riid == EIID_ITypeface) {
        return (ITypeface*)this;
    }
    return Typeface::Probe(riid);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
