
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_PKSTATUS_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_PKSTATUS_H__

#include "Elastos.CoreLibrary.Apache.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

class PKIStatus
    : public Object
    , public IPKIStatus
{
public:
    CAR_INTERFACE_DECL()

    PKIStatus(
        /* [in] */ Int32 status);

    /**
     * @return int value of the status
     */
    CARAPI GetStatus(
        /* [out] */ Int32* status);

    /**
     * @param value
     * @return
     */
    static CARAPI GetInstance(
        /* [in] */ Int32 status,
        /* [out] */ IPKIStatus** instance);

private:
    Int32 mStatus;
};

} //namespace Tsp
} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_PKSTATUS_H__
