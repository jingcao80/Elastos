#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFOCOLLECTIONITEMINFOHELPER_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFOCOLLECTIONITEMINFOHELPER_H__

#include "_Elastos_Droid_View_Accessibility_CAccessibilityNodeInfoCollectionItemInfoHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityNodeInfoCollectionItemInfoHelper)
    , public Singleton
    , public IAccessibilityNodeInfoCollectionItemInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Obtain(
        /* [in] */ IAccessibilityNodeInfoCollectionItemInfo* other,
        /* [out] */ IAccessibilityNodeInfoCollectionItemInfo** info);

    CARAPI Obtain(
        /* [in] */ Int32 rowIndex,
        /* [in] */ Int32 rowSpan,
        /* [in] */ Int32 columnIndex,
        /* [in] */ Int32 columnSpan,
        /* [in] */ Boolean heading,
        /* [out] */ IAccessibilityNodeInfoCollectionItemInfo** info);

    CARAPI Obtain(
        /* [in] */ Int32 rowIndex,
        /* [in] */ Int32 rowSpan,
        /* [in] */ Int32 columnIndex,
        /* [in] */ Int32 columnSpan,
        /* [in] */ Boolean heading,
        /* [in] */ Boolean selected,
        /* [out] */ IAccessibilityNodeInfoCollectionItemInfo** info);
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFOCOLLECTIONITEMINFOHELPER_H__
