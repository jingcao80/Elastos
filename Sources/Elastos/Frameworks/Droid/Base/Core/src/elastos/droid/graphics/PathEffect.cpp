
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/PathEffect.h"
#include <SkPathEffect.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

// {29CC1A9D-F188-4165-AD7D-CAB9A43079EC}
extern const InterfaceID EIID_PathEffect =
    { 0x29cc1a9d, 0xf188, 0x4165, { 0xad, 0x7d, 0xca, 0xb9, 0xa4, 0x30, 0x79, 0xec } };

CAR_INTERFACE_IMPL(PathEffect, Object, IPathEffect);
PathEffect::~PathEffect()
{
    NativeDestructor(mNativeInstance);
}

void PathEffect::NativeDestructor(
    /* [in] */ Int64 nativeInstance)
{
    SkPathEffect* obj = reinterpret_cast<SkPathEffect*>(nativeInstance);
    SkSafeUnref(obj);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
