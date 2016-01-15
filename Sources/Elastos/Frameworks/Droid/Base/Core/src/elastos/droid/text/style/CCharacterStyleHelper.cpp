#include "elastos/droid/text/style/CCharacterStyleHelper.h"
#include "elastos/droid/text/style/CharacterStyle.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL(CCharacterStyleHelper, Singleton, ICharacterStyleHelper)

CAR_SINGLETON_IMPL(CCharacterStyleHelper)

ECode CCharacterStyleHelper::Wrap(
    /* [in] */ ICharacterStyle* cs,
    /* [out] */ ICharacterStyle** style)
{
    VALIDATE_NOT_NULL(style);
    AutoPtr<ICharacterStyle> temp = CharacterStyle::Wrap(cs);
    *style = temp;
    REFCOUNT_ADD(*style)
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos