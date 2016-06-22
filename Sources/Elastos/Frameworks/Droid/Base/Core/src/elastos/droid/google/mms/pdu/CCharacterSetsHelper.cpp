#include "elastos/droid/google/mms/pdu/CCharacterSetsHelper.h"
#include "elastos/droid/google/mms/pdu/CharacterSets.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

CAR_INTERFACE_IMPL(CCharacterSetsHelper, Singleton, ICharacterSetsHelper)
CAR_SINGLETON_IMPL(CCharacterSetsHelper)

ECode CCharacterSetsHelper::GetMimeName(
    /* [in] */ Int32 mibEnumValue,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CharacterSets::GetMimeName(mibEnumValue);
    return NOERROR;
}

ECode CCharacterSetsHelper::GetMibEnumValue(
    /* [in] */ const String& mimeName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CharacterSets::GetMibEnumValue(mimeName);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
