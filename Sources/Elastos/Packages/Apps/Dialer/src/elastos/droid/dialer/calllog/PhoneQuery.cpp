
#include "elastos/droid/dialer/calllog/PhoneQuery.h"
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractPhoneLookup;
using Elastos::Droid::Provider::IContactsContractPhoneLookupColumns;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

static AutoPtr<ArrayOf<String> > InitProjection()
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(9);
    (*projection)[0] = IBaseColumns::ID;
    (*projection)[1] = IContactsContractContactsColumns::DISPLAY_NAME;
    (*projection)[2] = IContactsContractPhoneLookupColumns::TYPE;
    (*projection)[3] = IContactsContractPhoneLookupColumns::LABEL;
    (*projection)[4] = IContactsContractPhoneLookupColumns::NUMBER;
    (*projection)[5] = IContactsContractPhoneLookupColumns::NORMALIZED_NUMBER;
    (*projection)[6] = IContactsContractContactsColumns::PHOTO_ID;
    (*projection)[7] = IContactsContractContactsColumns::LOOKUP_KEY;
    (*projection)[8] = IContactsContractContactsColumns::PHOTO_URI;
    return projection;
}
const AutoPtr<ArrayOf<String> > PhoneQuery::_PROJECTION = InitProjection();
const Int32 PhoneQuery::PERSON_ID;
const Int32 PhoneQuery::NAME;
const Int32 PhoneQuery::PHONE_TYPE;
const Int32 PhoneQuery::LABEL;
const Int32 PhoneQuery::MATCHED_NUMBER;
const Int32 PhoneQuery::NORMALIZED_NUMBER;
const Int32 PhoneQuery::PHOTO_ID;
const Int32 PhoneQuery::LOOKUP_KEY;
const Int32 PhoneQuery::PHOTO_URI;

} // CallLog
} // Dialer
} // Droid
} // Elastos
