#ifndef __ELASTOS_DROID_INTERNAL_VIEW_CACTIONBARPOLICYHELPER_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_CACTIONBARPOLICYHELPER_H__

#include "_Elastos_Droid_Internal_View_CActionBarPolicyHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

CarClass(CActionBarPolicyHelper)
    , public Singleton
    , public IActionBarPolicyHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Get(
        /* [in] */ IContext* context,
        /* [out] */ IActionBarPolicy** policy);
};

} // namespace View
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_CACTIONBARPOLICYHELPER_H__
