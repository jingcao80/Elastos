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

#ifndef __ELASTOS_DROID_GRAPHICS_CCAMERA_H__
#define __ELASTOS_DROID_GRAPHICS_CCAMERA_H__

#include "_Elastos_Droid_Graphics_CCamera.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CCamera)
    , public Object
    , public ICamera
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CCamera();

    ~CCamera();

    CARAPI constructor();

    CARAPI Save();

    CARAPI Restore();

    CARAPI Translate(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI RotateX(
        /* [in] */ Float deg);

    CARAPI RotateY(
        /* [in] */ Float deg);

    CARAPI RotateZ(
        /* [in] */ Float deg);

    CARAPI Rotate(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI GetLocationX(
        /* [out] */ Float* locationX);

    CARAPI GetLocationY(
        /* [out] */ Float* locationY);

    CARAPI GetLocationZ(
        /* [out] */ Float* locationZ);

    CARAPI SetLocation(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float z);

    CARAPI GetMatrix(
        /* [in] */ IMatrix* matrix);

    CARAPI ApplyToCanvas(
        /* [in] */ ICanvas* canvas);

    CARAPI DotWithNormal(
        /* [in] */ Float dx,
        /* [in] */ Float dy,
        /* [in] */ Float dz,
        /* [out] */ Float* result);

private:
    CARAPI_(void) NativeConstructor();

    CARAPI_(void) NativeDestructor();

    CARAPI_(void) NativeGetMatrix(
        /* [in] */ Int64 nativeMatrix);

    CARAPI_(void) NativeApplyToCanvas(
        /* [in] */ Int64 nativeCanvas);

private:
    Int64 mNativeInstance;
    AutoPtr<IMatrix> mMatrix;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CCAMERA_H__
