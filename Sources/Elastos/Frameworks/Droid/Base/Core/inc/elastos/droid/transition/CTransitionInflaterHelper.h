
#ifndef __ELASTOS_DROID_TRANSITION_CTRANSITIONINFLATERHELPER_H__
#define __ELASTOS_DROID_TRANSITION_CTRANSITIONINFLATERHELPER_H__

#include "_Elastos_Droid_Transition_CTransitionInflaterHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass(CTransitionInflaterHelper)
    , public Singleton
    , public ITransitionInflaterHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI From(
        /* [in] */ IContext* context,
        /* [out] */ ITransitionInflater** result);
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CTRANSITIONINFLATERHELPER_H__
