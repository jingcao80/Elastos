#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsGroupMembership.h"

using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsGroupMembership)

CAR_INTERFACE_IMPL_3(CContactsGroupMembership, Singleton, IContactsGroupMembership, IBaseColumns, IContactsGroupsColumns)

ECode CContactsGroupMembership::constructor()
{
    return NOERROR;
}

ECode CContactsGroupMembership::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/groupmembership"), uri);
}

ECode CContactsGroupMembership::GetRAW_CONTENT_URL(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/groupmembershipraw"), uri);
}

} // Provider
} // Droid
} // Elastos