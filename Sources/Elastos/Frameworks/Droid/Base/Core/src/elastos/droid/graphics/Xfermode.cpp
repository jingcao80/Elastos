
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/Xfermode.h"
#include <SkXfermode.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

// {5DFFFEB0-3161-4BFE-9596-F50C528C69E7}
extern const InterfaceID EIID_Xfermode =
    { 0x5dfffeb0, 0x3161, 0x4bfe, { 0x95, 0x96, 0xf5, 0xc, 0x52, 0x8c, 0x69, 0xe7 } };

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
