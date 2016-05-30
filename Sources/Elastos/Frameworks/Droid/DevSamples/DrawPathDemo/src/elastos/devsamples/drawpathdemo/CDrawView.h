
#ifndef __Elastos_DevSamples_DrawPathDemo_CDrawView_H__
#define __Elastos_DevSamples_DrawPathDemo_CDrawView_H__

#include "_Elastos_DevSamples_DrawPathDemo_CDrawView.h"
#include <elastos/droid/view/View.h>

namespace Elastos {
namespace DevSamples {
namespace DrawPathDemo {

CarClass(CDrawView)
    , public Elastos::Droid::View::View
    , public IDrawView
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);
};

}// namespace DrawPathDemo
}// namespace DevSamples
}// namespace Elastos

#endif // __Elastos_DevSamples_DrawPathDemo_CDrawPathDemo_H__
