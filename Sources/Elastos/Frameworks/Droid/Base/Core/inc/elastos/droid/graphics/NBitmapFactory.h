#ifndef __ELASTOS_DROID_GRAPHICS_NBITMAPFACTORY_H__
#define __ELASTOS_DROID_GRAPHICS_NBITMAPFACTORY_H__

#include <elastos.h>
#include <skia/core/SkImageDecoder.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

class NBitmapFactory
{
public:
    static CARAPI_(String) GetMimeTypeString(
        /* [in] */ SkImageDecoder::Format format);
};

} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_NBITMAPFACTORY_H__
