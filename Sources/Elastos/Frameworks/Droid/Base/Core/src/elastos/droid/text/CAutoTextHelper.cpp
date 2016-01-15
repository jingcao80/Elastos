#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/CAutoTextHelper.h"
#include "elastos/droid/text/CAutoText.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CAutoTextHelper, Singleton, IAutoTextHelper)

CAR_SINGLETON_IMPL(CAutoTextHelper)

ECode CAutoTextHelper::Get(
    /* [in] */ ICharSequence* src,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ IView* view,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = CAutoText::Get(src, start, end, view);
    return NOERROR;
}

ECode CAutoTextHelper::GetSize(
    /* [in] */ IView* view,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = CAutoText::GetSize(view);
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos