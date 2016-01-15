#ifndef __ELASTOS_DROID_PROVIDER_CONTACTSCONTRACT_H__
#define __ELASTOS_DROID_PROVIDER_CONTACTSCONTRACT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Regex::IPattern;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Provider {

class ContactsContract
{
public:
    /**
     * This method can be used to identify whether the given ID is associated with profile
     * data.  It does not necessarily indicate that the ID is tied to valid data, merely
     * that accessing data using this ID will result in profile access checks and will only
     * return data from the profile.
     *
     * @param id The ID to check.
     * @return Whether the ID is associated with profile data.
     */
    static CARAPI IsProfileId(
        /* [in] */ Int64 id,
        /* [out] */ Boolean* res);

public:
    static const AutoPtr<IUri> AUTHORITY_URI;
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CONTACTSCONTRACT_H__
