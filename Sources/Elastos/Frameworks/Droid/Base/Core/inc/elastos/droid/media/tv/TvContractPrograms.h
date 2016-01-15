#ifndef __ELASTOS_DROID_MEDIA_TV_TVCONTRACTPROGRAMS_H__
#define __ELASTOS_DROID_MEDIA_TV_TVCONTRACTPROGRAMS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/** Column definitions for the TV programs table. */
class TvContractPrograms
{
public:
    static CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

private:
    TvContractPrograms();
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_TVCONTRACTPROGRAMS_H__
