#ifndef __ELASTOS_DROID_VIEW_CMAGNIFICATIONSPECHELPER_H__
#define __ELASTOS_DROID_VIEW_CMAGNIFICATIONSPECHELPER_H__

#include "_Elastos_Droid_View_CMagnificationSpecHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CMagnificationSpecHelper)
    , public Singleton
    , public IMagnificationSpecHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI Obtain(
        /* [in] */ IMagnificationSpec* other,
        /* [out] */ IMagnificationSpec** spec);

    CARAPI Obtain(
        /* [out] */ IMagnificationSpec** spec);

};

}// namespace View
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_VIEW_CMAGNIFICATIONSPECHELPER_H__
