//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
