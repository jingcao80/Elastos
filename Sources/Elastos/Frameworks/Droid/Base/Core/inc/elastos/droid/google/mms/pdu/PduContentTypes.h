#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUCONTENTTYPES_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUCONTENTTYPES_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

class PduContentTypes
    : public Object
{
private:
    static CARAPI_(AutoPtr<ArrayOf<String> >) MiddleInitContenttypes();

public:
    /**
      * All content types. From:
      * http://www.openmobilealliance.org/tech/omna/omna-wsp-content-type.htm
      */
    static AutoPtr<ArrayOf<String> > sContentTypes;
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_PDUCONTENTTYPES_H__

