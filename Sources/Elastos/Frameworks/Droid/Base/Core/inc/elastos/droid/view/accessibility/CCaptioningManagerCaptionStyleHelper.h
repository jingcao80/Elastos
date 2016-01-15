#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CAPTIONINGMANAGERCAPTIONSTYLEHELPER_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CAPTIONINGMANAGERCAPTIONSTYLEHELPER_H__

#include "_Elastos_Droid_View_Accessibility_CCaptioningManagerCaptionStyleHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CCaptioningManagerCaptionStyleHelper)
    , public Singleton
    , public ICaptioningManagerCaptionStyleHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetCustomStyle(
        /* [in] */ IContentResolver* cr,
        /* [out] */ ICaptioningManagerCaptionStyle** style);
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CAPTIONINGMANAGERCAPTIONSTYLEHELPER_H__
