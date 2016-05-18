#ifndef __ELASTOS_DROID_VIEW_CWINDOWINFOHELPER_H__
#define __ELASTOS_DROID_VIEW_CWINDOWINFOHELPER_H__

#include "_Elastos_Droid_View_CWindowInfoHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CWindowInfoHelper)
    , public Singleton
    , public IWindowInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI Obtain(
        /* [out] */ IWindowInfo** info);

    CARAPI Obtain(
        /* [in] */ IWindowInfo *other,
        /* [out] */ IWindowInfo** info);
};

}// namespace View
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_VIEW_CWINDOWINFOHELPER_H__
