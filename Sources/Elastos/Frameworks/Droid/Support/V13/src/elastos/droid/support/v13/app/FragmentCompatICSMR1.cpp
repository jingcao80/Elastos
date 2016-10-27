
#include "elastos/droid/support/v13/app/FragmentCompatICSMR1.h"

namespace Elastos {
namespace Droid {
namespace Support {
namespace V13 {
namespace App {

void FragmentCompatICSMR1::SetUserVisibleHint(
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
