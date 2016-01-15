
#include "elastos/droid/text/CAlteredCharSequenceHelper.h"
#include "elastos/droid/text/AlteredCharSequence.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CAlteredCharSequenceHelper, Object, IAlteredCharSequenceHelper)

CAR_SINGLETON_IMPL(CAlteredCharSequenceHelper)

ECode CAlteredCharSequenceHelper::Make(
    /* [in] */ ICharSequence* source,
    /* [in] */ ArrayOf<Char32>* sub,
    /* [in] */ Int32 substart,
    /* [in] */ Int32 subend,
    /* [out] */ IAlteredCharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IAlteredCharSequence> acs = AlteredCharSequence::Make(source, sub, substart, subend);
    *ret = acs;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos

