#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/CLayoutHelper.h"
#include "elastos/droid/text/Layout.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CLayoutHelper, Singleton, ILayoutHelper)

CAR_SINGLETON_IMPL(CLayoutHelper)

ECode CLayoutHelper::GetDesiredWidth(
    /* [in] */ ICharSequence* source,
    /* [in] */ ITextPaint* paint,
    /* [out] */ Float* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = Layout::GetDesiredWidth(source, paint);
    return NOERROR;
}

ECode CLayoutHelper::GetDesiredWidth(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ITextPaint* paint,
    /* [out] */ Float* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = Layout::GetDesiredWidth(source, start, end, paint);
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos