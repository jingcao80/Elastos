
#ifndef __ELASTOS_DROID_SERVER_AM_GRANTURI_H__
#define __ELASTOS_DROID_SERVER_AM_GRANTURI_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Net.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

// inner class of CActivityManagerService
class GrantUri : public Object
{
public:
    GrantUri(
        /* [in] */ Int32 sourceUserId,
        /* [in] */ IUri* uri,
        /* [in] */ Boolean prefix);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI_(Boolean) Equals(
        /* [in] */ GrantUri* other);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI_(String) ToString();

    CARAPI_(String) ToSafeString();

    static CARAPI_(AutoPtr<GrantUri>) Resolve(
        /* [in] */ Int32 defaultSourceUserHandle,
        /* [in] */ IUri* uri);

public:
    const Int32 mSourceUserId;
    const AutoPtr<IUri> mUri;
    Boolean mPrefix;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Am::GrantUri)

#endif //__ELASTOS_DROID_SERVER_AM_GRANTURI_H__
