#include "elastos/droid/text/CSpannableStringBuilderHelper.h"
#include "elastos/droid/text/SpannableStringBuilder.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_SINGLETON_IMPL(CSpannableStringBuilderHelper)

CAR_INTERFACE_IMPL(CSpannableStringBuilderHelper, Singleton, ISpannableStringBuilderHelper)

ECode  CSpannableStringBuilderHelper::ValueOf(
    /* [in] */ ICharSequence* source,
    /* [out] */ ISpannableStringBuilder** builder)
{
    VALIDATE_NOT_NULL(builder)
    AutoPtr<ISpannableStringBuilder> b = SpannableStringBuilder::ValueOf(source);
    *builder= b;
    REFCOUNT_ADD(*builder)
    return NOERROR;
}


} // namespace Text
} // namepsace Droid
} // namespace Elastos