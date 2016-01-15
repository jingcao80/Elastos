
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/accessibility/CCaptioningManagerCaptionStyleHelper.h"
#include "elastos/droid/view/accessibility/CaptioningManager.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL(CCaptioningManagerCaptionStyleHelper, Singleton, ICaptioningManagerCaptionStyleHelper)

CAR_SINGLETON_IMPL(CCaptioningManagerCaptionStyleHelper)

ECode CCaptioningManagerCaptionStyleHelper::GetCustomStyle(
    /* [in] */ IContentResolver* cr,
    /* [out] */ ICaptioningManagerCaptionStyle** style)
{
    VALIDATE_NOT_NULL(style);
    return CaptioningManager::CaptionStyle::GetCustomStyle(cr, style);
}

} // Accessibility
} // View
} // Droid
} // Elastos
