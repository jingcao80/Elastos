
#ifndef __ELASTOS_DROID_GALAXY4_BASEOBJ_H__
#define __ELASTOS_DROID_GALAXY4_BASEOBJ_H__

#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

class RenderScript;

class BaseObj : public Object
{
public:
    CARAPI constructor(
        /* [in] */ Int64 id,
        /* [in] */ RenderScript* rs);

    CARAPI SetID(
        /* [in] */ Int64 id);

    CARAPI GetID(
        /* [in] */ RenderScript* rs,
        /* [out] */ Int64* id);

public:
    RenderScript* mRS;

private:
    Int64 mID;
    Boolean mDestroyed;
    String mName;
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_BASEOBJ_H__
