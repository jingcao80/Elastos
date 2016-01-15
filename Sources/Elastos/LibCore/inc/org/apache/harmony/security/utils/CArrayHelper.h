
#ifndef __ORG_APACHE_HARMONY_SECURITY_UTILS_CARRAYHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_UTILS_CARRAYHELPER_H__

#include "_Org_Apache_Harmony_Security_Utils_CArrayHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

CarClass(CArrayHelper)
{
public:
    CARAPI GetBytesAsString(
        /* [in] */ ArrayOf<Byte> * pData,
        /* [out] */ String * pStr);

    CARAPI ToString(
        /* [in] */ ArrayOf<Byte> * pArray,
        /* [in] */ const String& prefix,
        /* [out] */ String * pStr);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_UTILS_CARRAYHELPER_H__
