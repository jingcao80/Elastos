
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CNinePatchHelper.h"
#include "elastos/droid/graphics/NinePatch.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_SINGLETON_IMPL(CNinePatchHelper);
CAR_INTERFACE_IMPL(CNinePatchHelper, Singleton, INinePatchHelper);
ECode CNinePatchHelper::IsNinePatchChunk(
    /* [in] */ ArrayOf<Byte>* chunk,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NinePatch::IsNinePatchChunk(chunk);
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
