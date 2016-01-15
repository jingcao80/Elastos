#ifndef __ELASTOS_DROID_PROVIDER_CMEDIASTOREVIDEO_H__
#define __ELASTOS_DROID_PROVIDER_CMEDIASTOREVIDEO_H__

#include "_Elastos_Droid_Provider_CMediaStoreVideo.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CMediaStoreVideo)
    , public Singleton
    , public IMediaStoreVideo
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Query(
        /* [in] */ IContentResolver* cr,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor);
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CMEDIASTOREVIDEO_H__
