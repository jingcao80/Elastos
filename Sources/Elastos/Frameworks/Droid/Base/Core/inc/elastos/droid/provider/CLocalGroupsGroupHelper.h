#ifndef __ELASTOS_DROID_PROVIDER_CLOCALGROUPSGROUPHELPER_H__
#define __ELASTOS_DROID_PROVIDER_CLOCALGROUPSGROUPHELPER_H__

#include "_Elastos_Droid_Provider_CLocalGroupsGroupHelper.h"
#include "_Elastos.Droid.Database.h"
#include "_Elastos.Droid.Content.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CLocalGroupsGroupHelper)
    , public Singleton
    , public ILocalGroupsGroupHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI RestoreGroup(
        /* [in] */ ICursor* cursor,
        /* [out] */ ILocalGroupsGroup** group);

    CARAPI RestoreGroupById(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int64 groupId,
        /* [out] */ ILocalGroupsGroup** group);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CLOCALGROUPSGROUPHELPER_H__
