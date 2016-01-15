#include "elastos/droid/text/CSpannedStringHelper.h"
#include "elastos/droid/text/CSpannedString.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CSpannedStringHelper, Singleton, ISpannedStringHelper)

CAR_SINGLETON_IMPL(CSpannedStringHelper)

ECode CSpannedStringHelper::ValueOf(
    /* [in] */ ICharSequence* source,
    /* [out] */ ISpannedString** spannedString)
{
    AutoPtr<ISpannedString> span = CSpannedString::ValueOf(source);
    *spannedString = span;
    REFCOUNT_ADD(*spannedString);
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
