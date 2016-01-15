#include "CollationKey.h"

using Elastos::Core::EIID_IComparable;

namespace Elastos {
namespace Text {

CAR_INTERFACE_IMPL_2(CollationKey, Object, ICollationKey, IComparable)

CollationKey::CollationKey()
{}

CollationKey:: ~CollationKey()
{}

ECode CollationKey::constructor(
    /* [in] */ const String& source)
{
    mSource = String(source);
    return NOERROR;
}

ECode CollationKey::GetSourceString(
    /* [out] */ String* sourceString)
{
    VALIDATE_NOT_NULL(sourceString);
    *sourceString = mSource;
    return NOERROR;
}

} // namespace Text
} // namespace Elastos
