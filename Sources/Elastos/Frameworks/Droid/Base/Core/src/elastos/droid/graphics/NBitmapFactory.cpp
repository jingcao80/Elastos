
#include "elastos/droid/graphics/NBitmapFactory.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

String NBitmapFactory::GetMimeTypeString(
    /* [in] */ SkImageDecoder::Format format)
{
    static const struct {
        SkImageDecoder::Format fFormat;
        String fMimeType;
    } gMimeTypes[] = {
        { SkImageDecoder::kBMP_Format,  String("image/bmp") },
        { SkImageDecoder::kGIF_Format,  String("image/gif") },
        { SkImageDecoder::kICO_Format,  String("image/x-ico") },
        { SkImageDecoder::kJPEG_Format, String("image/jpeg") },
        { SkImageDecoder::kPNG_Format,  String("image/png") },
        { SkImageDecoder::kWEBP_Format, String("image/webp") },
        { SkImageDecoder::kWBMP_Format, String("image/vnd.wap.wbmp") }
    };

    String str;
    for (size_t i = 0; i < SK_ARRAY_COUNT(gMimeTypes); i++) {
        if (gMimeTypes[i].fFormat == format) {
            str = gMimeTypes[i].fMimeType;
            break;
        }
    }

    return str;
}

} // namespace Graphics
} // namespace Droid
} // namespace Elastos
