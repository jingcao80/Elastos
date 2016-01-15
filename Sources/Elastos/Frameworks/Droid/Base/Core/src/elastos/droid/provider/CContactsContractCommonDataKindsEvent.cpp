#include "elastos/droid/provider/CContactsContractCommonDataKindsEvent.h"
#include "elastos/droid/R.h"
#include <elastos/coredef.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractCommonDataKindsEvent)

CAR_INTERFACE_IMPL_3(CContactsContractCommonDataKindsEvent, Singleton
    , IContactsContractCommonDataKindsEvent
    , IContactsContractDataColumnsWithJoins
    , IContactsContractCommonDataKindsCommonColumns)

ECode CContactsContractCommonDataKindsEvent::GetTypeResource(
    /* [in] */ IInteger32* type,
    /* [out] */ Int32* resource)
{
    VALIDATE_NOT_NULL(resource);

    if (type == NULL) {
        *resource = Elastos::Droid::R::string::eventTypeOther;
        return NOERROR;
    }
    Int32 _type;
    FAIL_RETURN(type->GetValue(&_type))
    switch (_type) {
        case TYPE_ANNIVERSARY:
            *resource = Elastos::Droid::R::string::eventTypeAnniversary;
            break;
        case TYPE_BIRTHDAY:
            *resource = Elastos::Droid::R::string::eventTypeBirthday;
            break;
        case TYPE_OTHER:
            *resource = Elastos::Droid::R::string::eventTypeOther;
            break;
        default:
            *resource = Elastos::Droid::R::string::eventTypeCustom;
            break;
    }
     return NOERROR;
}

} //Provider
} //Droid
} //Elastos