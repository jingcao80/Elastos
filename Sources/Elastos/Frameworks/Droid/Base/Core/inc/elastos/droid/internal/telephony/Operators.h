
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_ATRESOPNSEPARSER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_ATRESOPNSEPARSER_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::Object;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class Operators: public Object
{
public:
    static CARAPI_(String) OperatorReplace(
        /* [in] */ const String& response);

    // unoptimized version of operatorreplace for responseOperatorInfos
    // this will provide a little more flexiblilty  in a loop like sisuation
    // same numbers of checks like before
    // this is for the search network functionality
    CARAPI_(String) UnOptimizedOperatorReplace(
        /* [in] */ const String& response);

private:
    // Initialize list of Operator codes
    // this will be taken care of when garbage collection starts.
    AutoPtr<HashMap<String, String> > InitList();

private:
    //this will stay persistant in memory when called
    static String sStored;
    static String sStoredOperators;

    // this will not stay persistant in memory, this will be taken care of
    // iin garbage collection routiene.
    AutoPtr<HashMap<String, String> > mUnOptOperators;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_ATRESOPNSEPARSER_H__
