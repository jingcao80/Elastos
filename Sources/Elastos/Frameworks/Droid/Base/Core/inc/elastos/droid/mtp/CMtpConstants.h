
#ifndef __ELASTOS_DROID_MTP_CMTPCONSTANTS_H__
#define __ELASTOS_DROID_MTP_CMTPCONSTANTS_H__

#include "_Elastos_Droid_Mtp_CMtpConstants.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Mtp {

CarClass(CMtpConstants)
    , public Singleton
    , public IMtpConstants
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsAbstractObject(
        /* [in] */ Int32 format,
        /* [out] */ Boolean* result);
};
} // namespace Mtp
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MTP_CMTPCONSTANTS_H__
