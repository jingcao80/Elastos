
#ifndef __ELASTOS_DROID_WIDGET_CGLES30_H__
#define __ELASTOS_DROID_WIDGET_CGLES30_H__

#include "_Elastos_Droid_Opengl_CGLES30.h"
#include "elastos/droid/opengl/GLES30.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLES30)
    , public GLES30
{
public:
    CAR_SINGLETON_DECL()
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CGLES30_H__

