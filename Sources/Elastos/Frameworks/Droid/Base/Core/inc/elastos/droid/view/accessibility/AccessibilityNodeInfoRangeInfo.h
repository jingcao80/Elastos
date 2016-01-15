#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_ACCESSIBILITYNODEINFORANGEINFO_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_ACCESSIBILITYNODEINFORANGEINFO_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/utility/Pools.h"

using Elastos::Droid::Utility::Pools;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

class AccessibilityNodeInfoRangeInfo
    : public Object
    , public IAccessibilityNodeInfoRangeInfo
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Obtains a pooled instance that is a clone of another one.
     *
     * @param other The instance to clone.
     *
     * @hide
     */
    static CARAPI Obtain(
        /* [in] */ IAccessibilityNodeInfoRangeInfo* other,
        /* [out] */ IAccessibilityNodeInfoRangeInfo** info);

    /**
     * Obtains a pooled instance.
     *
     * @param type The type of the range.
     * @param min The min value.
     * @param max The max value.
     * @param current The current value.
     */
    static CARAPI Obtain(
        /* [in] */ Int32 type,
        /* [in] */ Float min,
        /* [in] */ Float max,
        /* [in] */ Float current,
        /* [out] */ IAccessibilityNodeInfoRangeInfo** info);

    /**
     * Gets the range type.
     *
     * @return The range type.
     *
     * @see #RANGE_TYPE_INT
     * @see #RANGE_TYPE_FLOAT
     * @see #RANGE_TYPE_PERCENT
     */
    CARAPI GetType(
        /* [out] */ Int32* type);

    /**
     * Gets the min value.
     *
     * @return The min value.
     */
    CARAPI GetMin(
        /* [out] */ Float* min);

    /**
     * Gets the max value.
     *
     * @return The max value.
     */
    CARAPI GetMax(
        /* [out] */ Float* max);

    /**
     * Gets the current value.
     *
     * @return The current value.
     */
    CARAPI GetCurrent(
        /* [out] */ Float* current);

    /**
     * Recycles this instance.
     */
    CARAPI Recycle();

private:
    AccessibilityNodeInfoRangeInfo(
        /* [in] */ Int32 type,
        /* [in] */ Float min,
        /* [in] */ Float max,
        /* [in] */ Float current);

    CARAPI_(void) Clear();

private:
    static const Int32 MAX_POOL_SIZE;
    Int32 mType;
    Float mMin;
    Float mMax;
    Float mCurrent;

    static AutoPtr< Pools::SynchronizedPool<IAccessibilityNodeInfoRangeInfo> > sPool;
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_ACCESSIBILITYNODEINFORANGEINFO_H__
