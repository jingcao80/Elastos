
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CNinePatch.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CNinePatch);
ECode CNinePatch::constructor(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ArrayOf<Byte>* chunk)
{
    return NinePatch::constructor(bitmap, chunk);
}

ECode CNinePatch::constructor(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ArrayOf<Byte>* chunk,
    /* [in] */ const String& srcName)
{
    return NinePatch::constructor(bitmap, chunk, srcName);
}

ECode CNinePatch::constructor(
    /* [in] */ INinePatch* patch)
{
    return NinePatch::constructor((NinePatch*)(CNinePatch*)patch);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
