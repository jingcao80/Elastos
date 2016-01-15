
#include "elastos/droid/content/res/CColorStateListHelper.h"
#include "elastos/droid/content/res/CColorStateList.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CColorStateListHelper, Singleton, IColorStateListHelper)

CAR_SINGLETON_IMPL(CColorStateListHelper)

ECode CColorStateListHelper::ValueOf(
    /* [in] */ Int32 color,
    /* [out] */ IColorStateList** list)
{
    return CColorStateList::ValueOf(color, list);
}

ECode CColorStateListHelper::CreateFromXml(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IColorStateList** list)
{
    return CColorStateList::CreateFromXml(r, parser, list);
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

