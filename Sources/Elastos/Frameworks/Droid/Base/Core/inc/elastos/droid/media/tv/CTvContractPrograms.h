#ifndef __ELASTOS_DROID_MEDIA_TV_CTVCONTRACTPROGRAMS_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVCONTRACTPROGRAMS_H__

#include "_Elastos_Droid_Media_Tv_CTvContractPrograms.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/** Column definitions for the TV programs table. */
CarClass(CTvContractPrograms)
    , public Singleton
    , public ITvContractPrograms
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL()

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVCONTRACTPROGRAMS_H__
