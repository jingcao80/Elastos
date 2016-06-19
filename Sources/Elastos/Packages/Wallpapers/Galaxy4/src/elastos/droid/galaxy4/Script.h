
#ifndef __ELASTOS_DROID_GALAXY4_SCRIPT_H__
#define __ELASTOS_DROID_GALAXY4_SCRIPT_H__

#include "BaseObj.h"

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

class Script : public BaseObj
{
public:
    CARAPI constructor(
        /* [in] */ Int64 id,
        /* [in] */ RenderScript* rs);

    CARAPI SetVar(
        /* [in] */ Int32 index,
        /* [in] */ BaseObj* o);
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_SCRIPT_H__
