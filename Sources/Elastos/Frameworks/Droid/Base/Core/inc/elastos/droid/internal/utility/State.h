#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_STATE_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_STATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

/**
 * {@hide}
 *
 * The class for implementing states in a StateMachine
 */
class ECO_PUBLIC State
    : public Object
    , public IState
{
public:
    CAR_INTERFACE_DECL()

    /* (non-Javadoc)
     * @see com.android.internal.util.IState#enter()
     */
    // @Override
    CARAPI Enter();

    /* (non-Javadoc)
     * @see com.android.internal.util.IState#exit()
     */
    // @Override
    CARAPI Exit();

    /* (non-Javadoc)
     * @see com.android.internal.util.IState#processMessage(android.os.Message)
     */
    // @Override
    CARAPI ProcessMessage(
        /* [in] */ IMessage* msg,
        /* [out] */ Boolean* result);

    /**
     * Name of State for debugging purposes.
     *
     * This default implementation returns the class name, returning
     * the instance name would better in cases where a State class
     * is used for multiple states. But normally there is one class per
     * state and the class name is sufficient and easy to get. You may
     * want to provide a setName or some other mechanism for setting
     * another name if the class name is not appropriate.
     *
     * @see com.android.internal.util.IState#processMessage(android.os.Message)
     */
    // @Override
    CARAPI GetName(
        /* [out] */ String* name);

    virtual CARAPI_(String) GetName() = 0;

protected:
    State() {}
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#ifndef HASH_EQUALTO_FUNC_FOR_AUTOPTR_STATE
#define HASH_EQUALTO_FUNC_FOR_AUTOPTR_STATE
DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Internal::Utility::State)
#endif

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_STATE_H__
