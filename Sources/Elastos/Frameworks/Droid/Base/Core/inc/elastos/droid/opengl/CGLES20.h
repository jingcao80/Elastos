
#ifndef __ELASTOS_DROID_WIDGET_CGLES20_H__
#define __ELASTOS_DROID_WIDGET_CGLES20_H__

#include "_Elastos_Droid_Opengl_CGLES20.h"
#include "elastos/droid/opengl/GLES20.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLES20)
    , public GLES20
{
public:
    CAR_SINGLETON_DECL()
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CGLES20_H__

