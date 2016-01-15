
#ifndef __ELASTOS_DROID_GRAPHICS_SKIAIOSTREAMADAPTOR_H__
#define __ELASTOS_DROID_GRAPHICS_SKIAIOSTREAMADAPTOR_H__

#include <skia/core/SkStream.h>

using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Droid {
namespace Graphics {

CARAPI_(SkStream*) CreateSkiaInputStreamAdaptor(
    /* [in] */ IInputStream* stream,
    /* [in] */ ArrayOf<Byte>* storage,
    /* [in] */ Int32 markSize = 0);

CARAPI_(SkWStream*) CreateSkiaOutputStreamAdaptor(
    /* [in] */ IOutputStream* stream,
    /* [in] */ ArrayOf<Byte>* storage);

} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_SKIAIOSTREAMADAPTOR_H__
