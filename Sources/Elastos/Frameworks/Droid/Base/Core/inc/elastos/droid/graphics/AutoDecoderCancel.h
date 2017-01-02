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
