
#include "elastos/droid/support/v13/app/FragmentCompatICS.h"

namespace Elastos {
namespace Droid {
namespace Support {
namespace V13 {
namespace App {

void FragmentCompatICS::SetMenuVisibility(
    /* [in] */ IFragment* f,
    /* [in] */ Boolean visible)
{
    f->SetMenuVisibility(visible);
}

} // namespace App
} // namespace V13
} // namespace Support
} // namespace Droid
} // namespace Elastos
