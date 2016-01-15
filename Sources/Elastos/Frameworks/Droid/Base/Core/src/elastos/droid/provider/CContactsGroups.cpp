#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsGroups.h"

using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsGroups)

CAR_INTERFACE_IMPL_4(CContactsGroups, Singleton, IContactsGroups, IBaseColumns, ISyncConstValue, IContactsGroupsColumns)

ECode CContactsGroups::constructor()
{
    return NOERROR;
}

ECode CContactsGroups::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/groups"), uri);
}

ECode CContactsGroups::GetDELETED_CONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return Uri::Parse(String("content://contacts/deleted_groups"), uri);
}

} // Provider
} // Droid
} // Elastos