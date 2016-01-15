
#ifndef __ELASTOS_DROID_VIEW_CMAGNIFICATIONSPEC_H__
#define __ELASTOS_DROID_VIEW_CMAGNIFICATIONSPEC_H__

#include "_Elastos_Droid_View_CMagnificationSpec.h"
#include "elastos/droid/utility/Pools.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::Pools;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * This class represents spec for performing screen magnification.
 *
 * @hide
 */
CarClass(CMagnificationSpec)
    , public Object
    , public IMagnificationSpec
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CMagnificationSpec();

    CARAPI constructor();

    CARAPI Initialize(
        /* [in] */ Float scale,
        /* [in] */ Float offsetX,
        /* [in] */ Float offsetY);

    CARAPI IsNop(
        /* [out] */ Boolean* nop);

    static CARAPI_(AutoPtr<IMagnificationSpec>) Obtain(
        /* [in] */ IMagnificationSpec* other);

    static CARAPI_(AutoPtr<IMagnificationSpec>) Obtain();

    CARAPI Recycle();

    CARAPI Clear();

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SetScale(
        /* [in] */ Float value);

    CARAPI GetScale(
        /* [out] */ Float* result);

    CARAPI SetOffsetX(
        /* [in] */ Float value);

    CARAPI GetOffsetX(
        /* [out] */ Float* result);

    CARAPI SetOffsetY(
        /* [in] */ Float value);

    CARAPI GetOffsetY(
        /* [out] */ Float* result);

public:
    Float mScale;
    Float mOffsetX;
    Float mOffsetY;

private:
    static const Int32 MAX_POOL_SIZE;
    static AutoPtr<Pools::SynchronizedPool<IMagnificationSpec> > sPool;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_CMAGNIFICATIONSPEC_H__
