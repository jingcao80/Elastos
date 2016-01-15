
#ifndef __ELASTOS_DROID_GRAPHICS_NINEPATCHPEEKER_H__
#define __ELASTOS_DROID_GRAPHICS_NINEPATCHPEEKER_H__

#include <skia/core/SkImageDecoder.h>
#include <androidfw/ResourceTypes.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

////////////////////////////////////////////////////////////////////////////////
// NinePatchPeeker

class NinePatchPeeker : public SkImageDecoder::Peeker {
private:
    // the host lives longer than we do, so a raw ptr is safe
    SkImageDecoder* mHost;
public:
    NinePatchPeeker(SkImageDecoder* host)
            : mHost(host)
            , mPatch(NULL)
            , mPatchSize(0)
            , mHasInsets(false)
            , mOutlineRadius(0)
            , mOutlineAlpha(0) {
        memset(mOpticalInsets, 0, 4 * sizeof(int32_t));
        memset(mOutlineInsets, 0, 4 * sizeof(int32_t));
    }

    ~NinePatchPeeker() {
        free(mPatch);
    }

    virtual bool peek(const char tag[], const void* data, size_t length);

    android::Res_png_9patch* mPatch;
    size_t mPatchSize;
    bool mHasInsets;
    int32_t mOpticalInsets[4];
    int32_t mOutlineInsets[4];
    float mOutlineRadius;
    uint8_t mOutlineAlpha;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elast

#endif //__ELASTOS_DROID_GRAPHICS_NINEPATCHPEEKER_H__
