
#ifndef __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTETYPEANDVALUECOMPARATOR_H__
#define __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTETYPEANDVALUECOMPARATOR_H__

#include "_Org_Apache_Harmony_Security_X501_CAttributeTypeAndValueComparator.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

CarClass(CAttributeTypeAndValueComparator)
{
public:
    CARAPI Compare(
        /* [in] */ IInterface * pLhs,
        /* [in] */ IInterface * pRhs,
        /* [out] */ Int32 * pResult);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTETYPEANDVALUECOMPARATOR_H__
