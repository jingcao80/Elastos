
#ifndef __ELASTOS_DROID_GRAPHICS_AUTODECODERCANCEL_H__
#define __ELASTOS_DROID_GRAPHICS_AUTODECODERCANCEL_H__

#include "Elastos.Droid.Graphics.h"
#include <skia/core/SkImageDecoder.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

class AutoDecoderCancel
{
public:
    AutoDecoderCancel(
        /* [in] */ IBitmapFactoryOptions* options,
        /* [in] */ SkImageDecoder* decoder);

    ~AutoDecoderCancel();

    static CARAPI_(Boolean) RequestCancel(
        /* [in] */ IBitmapFactoryOptions* options);

private:
    AutoDecoderCancel* mNext;
    AutoDecoderCancel* mPrev;
    AutoPtr<IBitmapFactoryOptions> mOptions;
    SkImageDecoder* mDecoder;

#ifdef SK_DEBUG
    static void Validate();
#else
    static void Validate() {}
#endif
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_AUTODECODERCANCEL_H__
