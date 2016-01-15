
#ifndef __ELASTOS_DROID_CONTENT_CMUTABLECONTEXTWRAPPER_H__
#define __ELASTOS_DROID_CONTENT_CMUTABLECONTEXTWRAPPER_H__

#include "_Elastos_Droid_Content_CMutableContextWrapper.h"
#include "ContextWrapper.h"

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CMutableContextWrapper)
    , public ContextWrapper
    , public IMutableContextWrapper
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetBaseContext(
        /* [out] */ IContext** context);

    /**
     * Change the base context for this ContextWrapper. All calls will then be
     * delegated to the base context.  Unlike ContextWrapper, the base context
     * can be changed even after one is already set.
     *
     * @param base The new base context for this wrapper.
     */
    CARAPI SetBaseContext(
        /* [in] */ IContext* base);

    CARAPI constructor(
        /* [in] */ IContext* base);

private:
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CMUTABLECONTEXTWRAPPER_H__
