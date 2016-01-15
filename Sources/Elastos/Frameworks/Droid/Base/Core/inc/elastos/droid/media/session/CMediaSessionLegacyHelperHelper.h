#ifndef __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONLEGACYHELPERHELPER_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONLEGACYHELPERHELPER_H__

#include "_Elastos_Droid_Media_Session_CMediaSessionLegacyHelperHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IMediaMetadata;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CarClass(CMediaSessionLegacyHelperHelper)
    , public Singleton
    , public IMediaSessionLegacyHelperHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetHelper(
        /* [in] */ IContext * context,
        /* [out] */ IMediaSessionLegacyHelper ** result);

    CARAPI GetOldMetadata(
        /* [in] */ IMediaMetadata * metadata,
        /* [in] */ Int32 artworkWidth,
        /* [in] */ Int32 artworkHeight,
        /* [out] */ IBundle ** result);
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONLEGACYHELPERHELPER_H__
