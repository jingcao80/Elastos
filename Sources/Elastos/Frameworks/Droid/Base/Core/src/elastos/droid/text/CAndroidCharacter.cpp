
#include "elastos/droid/text/CAndroidCharacter.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CAndroidCharacter, Object, IAndroidCharacter)

CAR_SINGLETON_IMPL(CAndroidCharacter)

ECode CAndroidCharacter::GetDirectionalities(
    /* [in] */ ArrayOf<Char32>* src,
    /* [in] */ ArrayOf<Byte>* dest,
    /* [in] */ Int32 count)
{
    return E_NOT_IMPLEMENTED;
}

ECode CAndroidCharacter::GetEastAsianWidth(
    /* [in] */ Char32 input,
    /* [out] */ Int32* result)
{
    return NOERROR;
}

ECode CAndroidCharacter::GetEastAsianWidths(
    /* [in] */ ArrayOf<Char32>* src,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ ArrayOf<Byte>* dest)
{
    return NOERROR;
}

ECode CAndroidCharacter::Mirror(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [out] */ Boolean* result)
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CAndroidCharacter::GetMirror(
    /* [in] */ Char32 ch,
    /* [out] */ Char32* result)
{
    // TODO: Add your code here
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos