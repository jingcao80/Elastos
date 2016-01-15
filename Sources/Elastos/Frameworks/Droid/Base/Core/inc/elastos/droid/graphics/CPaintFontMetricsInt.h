#ifndef __ELASTOS_DROID_GRAPHICS_CPAINTFONTMETRICSINT_H__
#define __ELASTOS_DROID_GRAPHICS_CPAINTFONTMETRICSINT_H__

#include "_Elastos_Droid_Graphics_CPaintFontMetricsInt.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CPaintFontMetricsInt)
    , public Object
    , public IPaintFontMetricsInt
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CPaintFontMetricsInt();

    CARAPI GetTop(
        /* [out] */ Int32* top);

    CARAPI SetTop(
        /* [in] */ Int32 top);

    CARAPI GetAscent(
        /* [out] */ Int32* ascent);

    CARAPI SetAscent(
        /* [in] */ Int32 ascent);

    CARAPI GetDescent(
        /* [out] */ Int32* descent);

    CARAPI SetDescent(
        /* [in] */ Int32 descent);

    CARAPI GetBottom(
        /* [out] */ Int32* bottom);

    CARAPI SetBottom(
        /* [in] */ Int32 bottom);

    CARAPI GetLeading(
        /* [out] */ Int32* leading);

    CARAPI SetLeading(
        /* [in] */ Int32 leading);

private:
    Int32 mTop;
    Int32 mAscent;
    Int32 mDescent;
    Int32 mBottom;
    Int32 mLeading;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CPAINTFONTMETRICSINT_H__
