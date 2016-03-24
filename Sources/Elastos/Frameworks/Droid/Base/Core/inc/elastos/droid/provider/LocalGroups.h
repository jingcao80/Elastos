#ifndef __ELASTOS_DROID_PROVIDER_LOCALGROUPS_H__
#define __ELASTOS_DROID_PROVIDER_LOCALGROUPS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Net.h"
#include "_Elastos.Droid.Database.h"
#include "_Elastos.Droid.Content.h"
#include "_Elastos.Droid.Provider.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Content::IContentResolver;

namespace Elastos {
namespace Droid {
namespace Provider {

class LocalGroups
{
public:
    class Group
    {
    public:
        static CARAPI RestoreGroup(
            /* [in] */ ICursor* cursor,
            /* [out] */ ILocalGroupsGroup** group);

        static CARAPI RestoreGroupById(
            /* [in] */ IContentResolver* cr,
            /* [in] */ Int64 groupId,
            /* [out] */ ILocalGroupsGroup** group);
    };

public:
    static const AutoPtr<IUri> AUTHORITY_URI;

    static const AutoPtr<IUri> CONTENT_URI;
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_LOCALGROUPS_H__
