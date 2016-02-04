
#ifndef __ELASTOS_DROID_WIDGET_CGLES31Ext_H__
#define __ELASTOS_DROID_WIDGET_CGLES31Ext_H__

#include "_Elastos_Droid_Opengl_CGLES31Ext.h"
#include "elastos/droid/opengl/GLES31Ext.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLES31Ext)
    , public GLES31Ext
{
public:
    CAR_SINGLETON_DECL()
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CGLES31Ext_H__

