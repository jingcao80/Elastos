
#include "elastos/droid/text/CEditableFactory.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CEditableFactory, Singleton, IEditableFactory)

CAR_SINGLETON_IMPL(CEditableFactory)

ECode CEditableFactory::NewEditable(
    /* [in] */ ICharSequence* source,
    /* [out] */ IEditable** editable)
{
    VALIDATE_NOT_NULL(editable);
    AutoPtr<ISpannableStringBuilder> ssb;
    CSpannableStringBuilder::New(source, (ISpannableStringBuilder**)&ssb);
    *editable = IEditable::Probe(ssb);
    REFCOUNT_ADD(*editable)
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
