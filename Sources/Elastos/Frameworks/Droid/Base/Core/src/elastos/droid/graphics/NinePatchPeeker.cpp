
#include "elastos/droid/graphics/NinePatchPeeker.h"
#include <skia/core/SkBitmap.h>

namespace Elastos {
namespace Droid {
namespace Graphics {
bool NinePatchPeeker::peek(const char tag[], const void* data, size_t length) {
    if (!strcmp("npTc", tag) && length >= sizeof(android::Res_png_9patch)) {
        android::Res_png_9patch* patch = (android::Res_png_9patch*) data;
        size_t patchSize = patch->serializedSize();
        assert(length == patchSize);
        // You have to copy the data because it is owned by the png reader
        android::Res_png_9patch* patchNew = (android::Res_png_9patch*) malloc(patchSize);
        memcpy(patchNew, patch, patchSize);
        android::Res_png_9patch::deserialize(patchNew);
        patchNew->fileToDevice();
        free(mPatch);
        mPatch = patchNew;
        mPatchSize = patchSize;

        // now update our host to force index or 32bit config
        // 'cause we don't want 565 predithered, since as a 9patch, we know
        // we will be stretched, and therefore we want to dither afterwards.
        SkImageDecoder::PrefConfigTable table;
        table.fPrefFor_8Index_NoAlpha_src   = SkBitmap::kIndex8_Config;
        table.fPrefFor_8Index_YesAlpha_src  = SkBitmap::kIndex8_Config;
        table.fPrefFor_8Gray_src            = SkBitmap::kARGB_8888_Config;
        table.fPrefFor_8bpc_NoAlpha_src     = SkBitmap::kARGB_8888_Config;
        table.fPrefFor_8bpc_YesAlpha_src    = SkBitmap::kARGB_8888_Config;

        mHost->setPrefConfigTable(table);
    } else if (!strcmp("npLb", tag) && length == sizeof(int32_t) * 4) {
        mHasInsets = true;
        memcpy(&mOpticalInsets, data, sizeof(int32_t) * 4);
    } else if (!strcmp("npOl", tag) && length == 24) { // 4 int32_ts, 1 float, 1 int32_t sized byte
        mHasInsets = true;
        memcpy(&mOutlineInsets, data, sizeof(int32_t) * 4);
        mOutlineRadius = ((const float*)data)[4];
        mOutlineAlpha = ((const int32_t*)data)[5] & 0xff;
    }
    return true;    // keep on decoding
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elast

