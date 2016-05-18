
#ifndef __ELASTOS_DROID_VIEW_CVIEWOUTLINEPROVIDERHELPER_H__
#define __ELASTOS_DROID_VIEW_CVIEWOUTLINEPROVIDERHELPER_H__

#include "_Elastos_Droid_View_CViewOutlineProviderHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CViewOutlineProviderHelper)
    , public Singleton
    , public IViewOutlineProviderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI GetBACKGROUND(
        /* [out] */ IViewOutlineProvider** background);

    CARAPI GetBOUNDS(
        /* [out] */ IViewOutlineProvider** bounds);

    CARAPI GetPADDEDBOUNDS(
        /* [out] */ IViewOutlineProvider** bounds);
};

}// namespace View
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_VIEW_CVIEWOUTLINEPROVIDERHELPER_H__
