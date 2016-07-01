
#include "CPhoneNumberListAdapterPhoneQueryHelper.h"
#include "CPhoneNumberListAdapter.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace List {

CAR_INTERFACE_IMPL(CPhoneNumberListAdapterPhoneQueryHelper, Singleton, IPhoneNumberListAdapterPhoneQueryHelper)

CAR_SINGLETON_DECL(CPhoneNumberListAdapterPhoneQueryHelper)

ECode CPhoneNumberListAdapterPhoneQueryHelper::GetPROJECTION_PRIMARY(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALUE_NOT_NULL(result);

    *result = ArrayOf<String>::Alloc(
            CPhoneNumberListAdapter::PhoneQuery::PROJECTION_PRIMARY,
            sizeof(CPhoneNumberListAdapter::PhoneQuery::PROJECTION_PRIMARY) / sizeof(String));
    return NOERROR;
}

ECode CPhoneNumberListAdapterPhoneQueryHelper::GetPROJECTION_ALTERNATIVE(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALUE_NOT_NULL(result);

    *result = ArrayOf<String>::Alloc(
            CPhoneNumberListAdapter::PhoneQuery::PROJECTION_ALTERNATIVE,
            sizeof(CPhoneNumberListAdapter::PhoneQuery::PROJECTION_ALTERNATIVE) / sizeof(String));
    return NOERROR;
}

} // List
} // Common
} // Contacts
} // Apps
} // Elastos
