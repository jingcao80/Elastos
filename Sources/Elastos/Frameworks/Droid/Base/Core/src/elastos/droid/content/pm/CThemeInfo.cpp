
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/CThemeInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(CThemeInfo, BaseThemeInfo, IThemeInfo)

CAR_OBJECT_IMPL(CThemeInfo)

ECode CThemeInfo::constructor(
    /* [in] */ IBundle* bundle)
{
    BaseThemeInfo::constructor();
    bundle->GetString(META_TAG_NAME, &mName);
    mThemeId = mName;
    bundle->GetString(META_TAG_AUTHOR, &mAuthor);
    return NOERROR;
}

ECode CThemeInfo::constructor()
{
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
