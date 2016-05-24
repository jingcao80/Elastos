
#include "CContactDisplayUtils.h"
#include "ContactDisplayUtils.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

CAR_INTERFACE_IMPL(CContactDisplayUtils, Singleton, IContactDisplayUtils)

CAR_SINGLETON_IMPL(CContactDisplayUtils)

ECode CContactDisplayUtils::IsCustomPhoneType(
    /* [in] */ IInteger32* type,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    *result = ContactDisplayUtils::IsCustomPhoneType(type);
    return NOERROR;
}

ECode CContactDisplayUtils::GetLabelForCallOrSms(
    /* [in] */ IInteger32* type,
    /* [in] */ ICharSequence* customLabel,
    /* [in] */ Int32 interactionType,
    /* [in] */ IContext* context,
    /* [out] */ ICharSequence** result)
{
    VALUE_NOT_NULL(result);

    AutoPtr<ICharSequence> cs = ContactDisplayUtils::GetLabelForCallOrSms(
            type, customLabel, interactionType, context);
    *result = cs;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CContactDisplayUtils::GetPhoneLabelResourceId(
    /* [in] */ IInteger32* type,
    /* [out] */ Int32* id)
{
    VALUE_NOT_NULL(id);

    *id = ContactDisplayUtils::GetPhoneLabelResourceId(type);
    return NOERROR;
}

ECode CContactDisplayUtils::GetSmsLabelResourceId(
    /* [in] */ IInteger32* type,
    /* [out] */ Int32* id)
{
    VALUE_NOT_NULL(id);

    *id = ContactDisplayUtils::GetSmsLabelResourceId(type);
    return NOERROR;
}

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos
