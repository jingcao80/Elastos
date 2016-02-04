#ifndef __ELASTOS_DROID_GRAPHICS_COLORFILTER_H__
#define __ELASTOS_DROID_GRAPHICS_COLORFILTER_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

class ColorFilter
    : public Object
    , public IColorFilter
{
    friend class Paint;

public:
    CAR_INTERFACE_DECL();

    virtual ~ColorFilter();

protected:
    static CARAPI_(void) DestroyFilter(
        /* [in] */ Int64 native_instance);

public:
    /**
     * Holds the pointer to the native SkColorFilter instance.
     *
     * @hide
     */
    Int64 mNativeInstance;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_COLORFILTER_H__
