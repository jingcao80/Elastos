
#include "elastos/apps/dialer/calllog/CPhoneQuery.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

const String CPhoneQuery::_PROJECTION[] = {
    IContactsContractPhoneLookup::_ID,
    IContactsContractPhoneLookup::DISPLAY_NAME,
    IContactsContractPhoneLookup::TYPE,
    IContactsContractPhoneLookup::LABEL,
    IContactsContractPhoneLookup::NUMBER,
    IContactsContractPhoneLookup::NORMALIZED_NUMBER,
    IContactsContractPhoneLookup::PHOTO_ID,
    IContactsContractPhoneLookup::LOOKUP_KEY,
    IContactsContractPhoneLookup::PHOTO_URI
}

CAR_INTERFACE_IMPL(CPhoneQuery, Singleton, IPhoneQuery);

CAR_SINGLETON_IMPL(CPhoneQuery);

ECode CPhoneQuery::Get_PROJECTION(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALUE_NOT_NULL(result);

    // TODO:
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(
        _PROJECTION, 9);
    *result = projection;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
