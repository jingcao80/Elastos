#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/provider/CMediaStoreVideo.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CMediaStoreVideo)

CAR_INTERFACE_IMPL(CMediaStoreVideo, Singleton, IMediaStoreVideo)

ECode CMediaStoreVideo::Query(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    return cr->Query(uri, projection, String(NULL), NULL,
            IMediaStoreVideo::DEFAULT_SORT_ORDER, cursor);
}

} //namespace Provider
} //namespace Droid
} //namespace Elastos
