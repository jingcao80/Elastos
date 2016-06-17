
#include "elastos/droid/galaxy4/CGalaxy4.h"

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

ECode CGalaxy4::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Activity::OnCreate(savedInstanceState));
    CGalaxyView::NewByFriend((IContext*)this, (CGalaxyView**)&mView);
    SetContentView(mView);
    return NOERROR;
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
