
#ifndef __ELASTOS_DROID_GRAPHICS_CREGIONITERATOR_H__
#define __ELASTOS_DROID_GRAPHICS_CREGIONITERATOR_H__

#include "_Elastos_Droid_Graphics_CRegionIterator.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CRegionIterator)
    , public Object
    , public IRegionIterator
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    ~CRegionIterator();

    /**
     * Construct an iterator for all of the rectangles in a region. This
     * effectively makes a private copy of the region, so any subsequent edits
     * to region will not affect the iterator.
     *
     * @param region the region that will be iterated
     */
    CARAPI constructor(
        /* [in] */ IRegion* region);

    /**
     * Return the next rectangle in the region. If there are no more rectangles
     * this returns false and r is unchanged. If there is at least one more,
     * this returns true and r is set to that rectangle.
     */
    CARAPI Next(
        /* [in] */ IRect* r,
        /* [out] */ Boolean* result);

private:
    static CARAPI_(Int64) NativeConstructor(
        /* [in] */ Int64 nativeRegion);

    static CARAPI_(void) NativeDestructor(
        /* [in] */ Int64 nativeIter);

    static CARAPI_(Boolean) NativeNext(
        /* [in] */ Int64 nativeIter,
        /* [in] */ IRect* r);

private:
    Int64 mNativeIter;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CREGIONITERATOR_H__
