
#include "elastos/droid/dialer/calllog/CPhoneQuery.h"
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractPhoneLookup;
using Elastos::Droid::Provider::IContactsContractPhoneLookupColumns;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

const String CPhoneQuery::_PROJECTION[] = {
    IBaseColumns::ID,
    IContactsContractContactsColumns::DISPLAY_NAME,
    IContactsContractPhoneLookupColumns::TYPE,
    IContactsContractPhoneLookupColumns::LABEL,
    IContactsContractPhoneLookupColumns::NUMBER,
    IContactsContractPhoneLookupColumns::NORMALIZED_NUMBER,
    IContactsContractContactsColumns::PHOTO_ID,
    IContactsContractContactsColumns::LOOKUP_KEY,
    IContactsContractContactsColumns::PHOTO_URI
};

CAR_INTERFACE_IMPL(CPhoneQuery, Singleton, IPhoneQuery);

CAR_SINGLETON_IMPL(CPhoneQuery);

ECode CPhoneQuery::Get_PROJECTION(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);

    // TODO:
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(
        (String*)_PROJECTION, 9);
    *result = projection;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
