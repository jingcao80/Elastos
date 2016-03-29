
#include "elastos/droid/provider/CLocalGroupsGroupHelper.h"
#include "elastos/droid/provider/LocalGroups.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CLocalGroupsGroupHelper)

CAR_INTERFACE_IMPL(CLocalGroupsGroupHelper, Singleton, ILocalGroupsGroupHelper)

ECode CLocalGroupsGroupHelper::RestoreGroup(
    /* [in] */ ICursor* cursor,
    /* [out] */ ILocalGroupsGroup** group)
{
    return LocalGroups::Group::RestoreGroup(cursor, group);
}


ECode CLocalGroupsGroupHelper::RestoreGroupById(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int64 groupId,
    /* [out] */ ILocalGroupsGroup** group)
{
    return LocalGroups::Group::RestoreGroupById(cr, groupId, group);
}

} //Provider
} //Droid
} //Elastos
