
#include "elastos/droid/text/CSpannableFactory.h"
#include "elastos/droid/text/CSpannableString.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CSpannableFactory, Singleton, ISpannableFactory)

CAR_SINGLETON_IMPL(CSpannableFactory)

ECode CSpannableFactory::NewSpannable(
    /* [in] */ ICharSequence* source,
    /* [out] */ ISpannable** spannable)
{
    VALIDATE_NOT_NULL(spannable);
    AutoPtr<ISpannableString> ss;
    CSpannableString::New(source, (ISpannableString**)&ss);
    *spannable = ISpannable::Probe(ss);
    REFCOUNT_ADD(*spannable)
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
