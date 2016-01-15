#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFOCOLLECTIONINFOHELPER_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFOCOLLECTIONINFOHELPER_H__

#include "_Elastos_Droid_View_Accessibility_CAccessibilityNodeInfoCollectionInfoHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityNodeInfoCollectionInfoHelper)
    , public Singleton
    , public IAccessibilityNodeInfoCollectionInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Obtain(
        /* [in] */ IAccessibilityNodeInfoCollectionInfo* other,
        /* [out] */ IAccessibilityNodeInfoCollectionInfo** info);

    CARAPI Obtain(
        /* [in] */ Int32 rowCount,
        /* [in] */ Int32 columnCount,
        /* [in] */ Boolean hierarchical,
        /* [out] */ IAccessibilityNodeInfoCollectionInfo** info);

    CARAPI Obtain(
        /* [in] */ Int32 rowCount,
        /* [in] */ Int32 columnCount,
        /* [in] */ Boolean hierarchical,
        /* [in] */ Int32 selectionMode,
        /* [out] */ IAccessibilityNodeInfoCollectionInfo** info);
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFOCOLLECTIONINFOHELPER_H__
