#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_ACCESSIBILITYNODEINFOCOLLECTIONITEMINFO_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_ACCESSIBILITYNODEINFOCOLLECTIONITEMINFO_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/utility/Pools.h"

using Elastos::Droid::Utility::Pools;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

class AccessibilityNodeInfoCollectionItemInfo
    : public Object
    , public IAccessibilityNodeInfoCollectionItemInfo
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Obtains a pooled instance that is a clone of another one.
     *
     * @param other The instance to clone.
     * @hide
     */
    static CARAPI Obtain(
        /* [in] */ IAccessibilityNodeInfoCollectionItemInfo* other,
        /* [out] */ IAccessibilityNodeInfoCollectionItemInfo** info);

    /**
     * Obtains a pooled instance.
     *
     * @param rowIndex The row index at which the item is located.
     * @param rowSpan The number of rows the item spans.
     * @param columnIndex The column index at which the item is located.
     * @param columnSpan The number of columns the item spans.
     * @param heading Whether the item is a heading.
     */
    static CARAPI Obtain(
        /* [in] */ Int32 rowIndex,
        /* [in] */ Int32 rowSpan,
        /* [in] */ Int32 columnIndex,
        /* [in] */ Int32 columnSpan,
        /* [in] */ Boolean heading,
        /* [out] */ IAccessibilityNodeInfoCollectionItemInfo** info);

    /**
     * Obtains a pooled instance.
     *
     * @param rowIndex The row index at which the item is located.
     * @param rowSpan The number of rows the item spans.
     * @param columnIndex The column index at which the item is located.
     * @param columnSpan The number of columns the item spans.
     * @param heading Whether the item is a heading.
     * @param selected Whether the item is selected.
     */
    static CARAPI Obtain(
        /* [in] */ Int32 rowIndex,
        /* [in] */ Int32 rowSpan,
        /* [in] */ Int32 columnIndex,
        /* [in] */ Int32 columnSpan,
        /* [in] */ Boolean heading,
        /* [in] */ Boolean selected,
        /* [out] */ IAccessibilityNodeInfoCollectionItemInfo** info);


    /**
     * Gets the column index at which the item is located.
     *
     * @return The column index.
     */
    CARAPI GetColumnIndex(
        /* [out] */ Int32* columnIndex);

    /**
     * Gets the row index at which the item is located.
     *
     * @return The row index.
     */
    CARAPI GetRowIndex(
        /* [out] */ Int32* rowIndex);

    /**
     * Gets the number of columns the item spans.
     *
     * @return The column span.
     */
    CARAPI GetColumnSpan(
        /* [out] */ Int32* columnSpan);

    /**
     * Gets the number of rows the item spans.
     *
     * @return The row span.
     */
    CARAPI GetRowSpan(
        /* [out] */ Int32* rowSpan);

    /**
     * Gets if the collection item is a heading. For example, section
     * heading, table header, etc.
     *
     * @return If the item is a heading.
     */
    CARAPI IsHeading(
        /* [out] */ Boolean* heading);

    /**
     * Gets if the collection item is selected.
     *
     * @return If the item is selected.
     */
    CARAPI IsSelected(
        /* [out] */ Boolean* selected);

    /**
     * Recycles this instance.
     */
    CARAPI Recycle();

private:
    AccessibilityNodeInfoCollectionItemInfo(
        /* [in] */ Int32 rowIndex,
        /* [in] */ Int32 rowSpan,
        /* [in] */ Int32 columnIndex,
        /* [in] */ Int32 columnSpan,
        /* [in] */ Boolean heading,
        /* [in] */ Boolean selected);

    CARAPI_(void) Clear();

private:
    Boolean mHeading;
    Int32 mColumnIndex;
    Int32 mRowIndex;
    Int32 mColumnSpan;
    Int32 mRowSpan;
    Boolean mSelected;

    static const Int32 MAX_POOL_SIZE;
    static AutoPtr< Pools::SynchronizedPool<AccessibilityNodeInfoCollectionItemInfo> > sPool;
};

} // Accessibility
} // View
} // Droid
} // Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::View::Accessibility::AccessibilityNodeInfoCollectionItemInfo, IInterface)

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_ACCESSIBILITYNODEINFOCOLLECTIONITEMINFO_H__
