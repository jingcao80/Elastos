
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTSTINFO_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTSTINFO_H__

#include "_Org_Apache_Harmony_Security_X509_Tsp_CTSTInfo.h"
#include "org/apache/harmony/security/x509/tsp/TSTInfo.h"

using Org::Apache::Harmony::Security::X509::Tsp::TSTInfo;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CarClass(CTSTInfo)
    , public TSTInfo
{
public:
    CAR_OBJECT_DECL()
};

} //namespace Tsp
} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_TSP_CTSTINFO_H__
