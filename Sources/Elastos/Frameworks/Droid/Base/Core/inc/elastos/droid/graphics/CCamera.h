
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
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CCamera();

    ~CCamera();

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
