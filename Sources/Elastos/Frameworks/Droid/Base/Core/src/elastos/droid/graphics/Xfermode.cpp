
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/Xfermode.h"
#include <SkXfermode.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_INTERFACE_IMPL(Xfermode, Object, IXfermode);
Xfermode::~Xfermode()
{
    Finalizer(mNativeInstance);
}

void Xfermode::Finalizer(
    /* [in] */ Int64 nativeInstance)
{
    SkSafeUnref(reinterpret_cast<SkXfermode*>(nativeInstance));
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
