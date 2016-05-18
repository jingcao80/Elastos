
#ifndef __ELASTOS_DROID_GRAPHICS_RASTERIZER_H__
#define __ELASTOS_DROID_GRAPHICS_RASTERIZER_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

extern const InterfaceID EIID_Rasterizer;

class Rasterizer
    : public Object
    , public IRasterizer
{
    friend class Paint;

public:
    CAR_INTERFACE_DECL()

    ~Rasterizer();

private:
    static CARAPI_(void) Finalizer(
        /* [in] */ Int64 nativeInstance);

protected:
    Int64 mNativeInstance;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_RASTERIZER_H__
