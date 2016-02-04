
#ifndef __ELASTOS_DROID_WIDGET_CGLES31_H__
#define __ELASTOS_DROID_WIDGET_CGLES31_H__

#include "_Elastos_Droid_Opengl_CGLES31.h"
#include "elastos/droid/opengl/GLES31.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLES31)
    , public GLES31
{
public:
    CAR_SINGLETON_DECL()
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CGLES31_H__

