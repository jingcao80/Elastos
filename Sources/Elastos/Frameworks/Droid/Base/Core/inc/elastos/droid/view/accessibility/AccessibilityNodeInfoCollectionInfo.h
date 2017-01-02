//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_ACCESSIBILITYNODEINFOCOLLECTIONINFO_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_ACCESSIBILITYNODEINFOCOLLECTIONINFO_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/utility/Pools.h"

using Elastos::Droid::Utility::Pools;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

class AccessibilityNodeInfoCollectionInfo
    : public Object
    , public IAccessibilityNodeInfoCollectionInfo
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
        /* [in] */ IAccessibilityNodeInfoCollectionInfo* other,
        /* [out] */ IAccessibilityNodeInfoCollectionInfo** info);

    /**
     * Obtains a pooled instance.
     *
     * @param rowCount The number of rows.
     * @param columnCount The number of columns.
     * @param hierarchical Whether the collection is hierarchical.
     */
    static CARAPI Obtain(
        /* [in] */ Int32 rowCount,
        /* [in] */ Int32 columnCount,
        /* [in] */ Boolean hierarchical,
        /* [out] */ IAccessibilityNodeInfoCollectionInfo** info);

    /**
     * Obtains a pooled instance.
     *
     * @param rowCount The number of rows.
     * @param columnCount The number of columns.
     * @param hierarchical Whether the collection is hierarchical.
     * @param selectionMode The collection's selection mode, one of:
     *            <ul>
     *            <li>{@link #SELECTION_MODE_NONE}
     *            <li>{@link #SELECTION_MODE_SINGLE}
     *            <li>{@link #SELECTION_MODE_MULTIPLE}
     *            </ul>
     */
    static CARAPI Obtain(
        /* [in] */ Int32 rowCount,
        /* [in] */ Int32 columnCount,
        /* [in] */ Boolean hierarchical,
        /* [in] */ Int32 selectionMode,
        /* [out] */ IAccessibilityNodeInfoCollectionInfo** info);

    /**
     * Gets the number of rows.
     *
     * @return The row count.
     */
    CARAPI GetRowCount(
        /* [out] */ Int32* rowCount);

    /**
     * Gets the number of columns.
     *
     * @return The column count.
     */
    CARAPI GetColumnCount(
        /* [out] */ Int32* columnCount);

    /**
     * Gets if the collection is a hierarchically ordered.
     *
     * @return Whether the collection is hierarchical.
     */
    CARAPI IsHierarchical(
        /* [out] */ Boolean* result);

    /**
     * Gets the collection's selection mode.
     *
     * @return The collection's selection mode, one of:
     *         <ul>
     *         <li>{@link #SELECTION_MODE_NONE}
     *         <li>{@link #SELECTION_MODE_SINGLE}
     *         <li>{@link #SELECTION_MODE_MULTIPLE}
     *         </ul>
     */
    CARAPI GetSelectionMode(
        /* [out] */ Int32* mode);

    /**
     * Recycles this instance.
     */
    CARAPI Recycle();

private:
    AccessibilityNodeInfoCollectionInfo(
        /* [in] */ Int32 rowCount,
        /* [in] */ Int32 columnCount,
        /* [in] */ Boolean hierarchical,
        /* [in] */ Int32 selectionMode);

    CARAPI_(void) Clear();

private:
    Int32 mRowCount;
    Int32 mColumnCount;
    Boolean mHierarchical;
    Int32 mSelectionMode;

    static const Int32 MAX_POOL_SIZE;
    static AutoPtr< Pools::SynchronizedPool<AccessibilityNodeInfoCollectionInfo> > sPool;
};

} // Accessibility
} // View
} // Droid
} // Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::View::Accessibility::AccessibilityNodeInfoCollectionInfo, IInterface)

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_ACCESSIBILITYNODEINFOCOLLECTIONINFO_H__
