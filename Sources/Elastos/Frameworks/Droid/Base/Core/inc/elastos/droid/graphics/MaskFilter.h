#ifndef __ELASTOS_DROID_GRAPHICS_MASKFILTER_H__
#define __ELASTOS_DROID_GRAPHICS_MASKFILTER_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

extern const InterfaceID EIID_MaskFilter;

/**
 * MaskFilter is the base class for object that perform transformations on
 * an alpha-channel mask before drawing it. A subclass of MaskFilter may be
 * installed into a Paint. Blur and emboss are implemented as subclasses of MaskFilter.
 */
class MaskFilter
    : public Object
    , public IMaskFilter
{
    friend class Paint;

public:
    CAR_INTERFACE_DECL();

    virtual ~MaskFilter();

private:
    static CARAPI_(void) NativeDestructor(
        /* [in] */ Int64 nativeInstance);

protected:
    Int64 mNativeInstance;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_MASKFILTER_H__
