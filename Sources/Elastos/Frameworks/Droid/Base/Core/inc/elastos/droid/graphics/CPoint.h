
#ifndef __ELASTOS_DROID_GRAPHICS_CPOINT_H__
#define __ELASTOS_DROID_GRAPHICS_CPOINT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Graphics_CPoint.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CPoint)
    , public Object
    , public IPoint
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI constructor(
        /* [in] */ IPoint* src);

    /**
     * Set the point's x and y coordinates
     */
    CARAPI Set(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI Get(
        /* [out] */ Int32* x,
        /* [out] */ Int32* y);

    CARAPI GetX(
        /* [out] */ Int32* x);

    CARAPI GetY(
        /* [out] */ Int32* x);

    /**
     * Negate the point's coordinates
     */
    CARAPI Negate();

    /**
     * Offset the point's coordinates by dx, dy
     */
    CARAPI Offset(
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy);

    /**
     * Returns true if the point's coordinates equal (x,y)
     */
    CARAPI Equals(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IPoint* p,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IInterface* p,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [out] */ IParcel* dest);

public:
    Int32 mX;
    Int32 mY;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CPOINT_H__
