
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWFILTER_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWFILTER_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

extern const InterfaceID EIID_DrawFilter;

/**
 * A DrawFilter subclass can be installed in a Canvas. When it is present, it
 * can modify the paint that is used to draw (temporarily). With this, a filter
 * can disable/enable antialiasing, or change the color for everything this is
 * drawn.
 */
class DrawFilter
    : public Object
    , public IDrawFilter
{
public:
    CAR_INTERFACE_DECL();

    virtual ~DrawFilter();

private:
    static CARAPI_(void) NativeDestructor(
        /* [in] */ Int64 nativeDrawFilter);

public:
    Int64 mNativeInstance;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_DRAWFILTER_H__
