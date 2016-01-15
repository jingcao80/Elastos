#ifndef __ELASTOS_DROID_GRAPHICS_CPAINTFONTMETRICS_H__
#define __ELASTOS_DROID_GRAPHICS_CPAINTFONTMETRICS_H__

#include "_Elastos_Droid_Graphics_CPaintFontMetrics.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CPaintFontMetrics)
    , public Object
    , public IPaintFontMetrics
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CARAPI GetTop(
        /* [out] */ Float* top);

    CARAPI SetTop(
        /* [in] */ Float top);

    CARAPI GetAscent(
        /* [out] */ Float* ascent);

    CARAPI SetAscent(
        /* [in] */ Float ascent);

    CARAPI GetDescent(
        /* [out] */ Float* descent);

    CARAPI SetDescent(
        /* [in] */ Float descent);

    CARAPI GetBottom(
        /* [out] */ Float* bottom);

    CARAPI SetBottom(
        /* [in] */ Float bottom);

    CARAPI GetLeading(
        /* [out] */ Float* leading);

    CARAPI SetLeading(
        /* [in] */ Float leading);

private:
    Float mTop;
    Float mAscent;
    Float mDescent;
    Float mBottom;
    Float mLeading;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CPAINTFONTMETRICS_H__
