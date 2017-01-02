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

#ifndef __ELASTOS_DROID_MEDIA_CFACEDETECTOR_H__
#define __ELASTOS_DROID_MEDIA_CFACEDETECTOR_H__

#include "_Elastos_Droid_Media_CFaceDetector.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IPointF;
using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Identifies the faces of people in a
 * {@link android.graphics.Bitmap} graphic object.
 */
CarClass(CFaceDetector)
    , public Object
    , public IFaceDetector
{
public:
    CFaceDetector();

    ~CFaceDetector();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Creates a FaceDetector, configured with the size of the images to
     * be analysed and the maximum number of faces that can be detected.
     * These parameters cannot be changed once the object is constructed.
     * Note that the width of the image must be even.
     *
     * @param width  the width of the image
     * @param height the height of the image
     * @param maxFaces the maximum number of faces to identify
     *
     */
    CARAPI constructor(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 maxFaces);

    /**
     * Finds all the faces found in a given {@link android.graphics.Bitmap}.
     * The supplied array is populated with {@link FaceDetector.Face}s for each
     * face found. The bitmap must be in 565 format (for now).
     *
     * @param bitmap the {@link android.graphics.Bitmap} graphic to be analyzed
     * @param faces  an array in which to place all found
     *               {@link FaceDetector.Face}s. The array must be sized equal
     *               to the <var>maxFaces</var> value set at initialization
     * @return the number of faces found
     * @throws IllegalArgumentException if the Bitmap dimensions don't match
     *               the dimensions defined at initialization or the given array
     *               is not sized equal to the <var>maxFaces</var> value defined
     *               at initialization
     */
    CARAPI FindFaces(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ ArrayOf<IFace*>* faces,
        /* [out] */ Int32* num);

protected:
    /* no user serviceable parts here ... */
    CARAPI_(void) Finalize();

private:

    CARAPI  fft_initialize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 maxFaces,
        /* [out] */ Int32* status);

    CARAPI_(Int32)  fft_detect(
        /* [in] */ IBitmap* bitmap);

    CARAPI_(void) fft_get_face(
        /* [in] */ IFace* face,
        /* [in] */ Int32 i);

    CARAPI_(void) fft_destroy();

private:
    Int64  mFD;
    Int64  mSDK;
    Int64  mDCR;
    Int32  mWidth;
    Int32  mHeight;
    Int32  mMaxFaces;
    AutoPtr<ArrayOf<Byte> > mBWBuffer;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CFACEDETECTOR_H__

