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

#ifndef __ELASTOS_DROID_GRAPHICS_NATIVEPICTURE_H__
#define __ELASTOS_DROID_GRAPHICS_NATIVEPICTURE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "SkPicture.h"
#include "SkPictureRecorder.h"
#include "SkRefCnt.h"
#include "SkTemplates.h"

class SkStream;
class SkWStream;

namespace Elastos {
namespace Droid {
namespace Graphics {

class NativeCanvas;

// Skia's SkPicture class has been split into an SkPictureRecorder
// and an SkPicture. AndroidPicture recreates the functionality
// of the old SkPicture interface by flip-flopping between the two
// new classes.
class NativePicture
{
public:
    explicit NativePicture(
        /* [in] */ const NativePicture* src = NULL);

    CARAPI_(NativeCanvas*) BeginRecording(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI_(void) EndRecording();

    CARAPI_(Int32) GetWidth() const;

    CARAPI_(Int32) GetHeight() const;

    static NativePicture* CreateFromStream(
        /* [in] */ SkStream* stream);

    CARAPI_(void) Serialize(
        /* [in] */ SkWStream* stream) const;

    CARAPI_(void) Draw(
        /* [in] */ NativeCanvas* canvas);

private:
    // Make a copy of a picture that is in the midst of being recorded. The
    // resulting picture will have balanced saves and restores.
    CARAPI_(SkPicture*) MakePartialCopy() const;

private:
    Int32 mWidth;
    Int32 mHeight;
    SkAutoTUnref<const SkPicture> mPicture;
    SkAutoTDelete<SkPictureRecorder> mRecorder;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_NATIVEPICTURE_H__
