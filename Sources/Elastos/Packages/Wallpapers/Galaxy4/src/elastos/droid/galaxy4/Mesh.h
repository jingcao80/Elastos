
#ifndef __ELASTOS_DROID_GALAXY4_MESH_H__
#define __ELASTOS_DROID_GALAXY4_MESH_H__

#include "BaseObj.h"

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

class RenderScript;

class Mesh : public BaseObj
{
public:
    class AllocationBuilder : public Object
    {
    public:
        AllocationBuilder(
            /* [in] */ RenderScript* rs);

    public:
        RenderScript* mRS;
    };
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_MESH_H__
