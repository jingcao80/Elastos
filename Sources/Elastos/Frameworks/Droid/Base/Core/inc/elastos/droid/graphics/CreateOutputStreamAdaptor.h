#ifndef __ELASTOS_DROID_GRAPHICS_CREATEOUTPUTSTREAMADAPTOR_H__
#define __ELASTOS_DROID_GRAPHICS_CREATEOUTPUTSTREAMADAPTOR_H__

#include <Elastos.CoreLibrary.h>
#include <skia/core/SkStream.h>

using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Droid {
namespace Graphics {

SkStream* CreateInputStreamAdaptor(
    /* [in] */ IInputStream* stream,
    /* [in] */ ArrayOf<Byte>* storage);

SkStreamRewindable* CopyJavaInputStream(
    /* [in] */ IInputStream* stream,
    /* [in] */ ArrayOf<Byte>* storage);

SkWStream* CreateOutputStreamAdaptor(
    /* [in] */ IOutputStream* stream,
    /* [in] */ ArrayOf<Byte>* storage);

} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CREATEOUTPUTSTREAMADAPTOR_H__
