
#ifndef __ELASTOS_DROID_OPENGL_CETC1UTIL_H__
#define __ELASTOS_DROID_OPENGL_CETC1UTIL_H__

#include "_Elastos_Droid_Opengl_CETC1Util.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CETC1Util)
{
public:
    CARAPI LoadTexture(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 border,
        /* [in] */ Int32 fallbackFormat,
        /* [in] */ Int32 fallbackType,
        /* [in] */ Elastos::IO::IInputStream* input);

    CARAPI LoadTexture(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 border,
        /* [in] */ Int32 fallbackFormat,
        /* [in] */ Int32 fallbackType,
        /* [in] */ IETC1Texture* texture);

    CARAPI IsETC1Supported(
        /* [out] */ Boolean* isSupported);

    CARAPI CreateTexture(
        /* [in] */ Elastos::IO::IInputStream* input,
        /* [out] */ IETC1Texture** texture);

    CARAPI CompressTexture(
        /* [in] */ Elastos::IO::IBuffer* input,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 pixelSize,
        /* [in] */ Int32 stride,
        /* [out] */ IETC1Texture** texture);

    CARAPI WriteTexture(
        /* [in] */ IETC1Texture* texture,
        /* [in] */ Elastos::IO::IOutputStream* output);

private:
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CETC1UTIL_H__
