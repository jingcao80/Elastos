
#ifndef __ELASTOS_DROID_MEDIA_CMEDIACRYPTOHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACRYPTOHELPER_H__

#include "_Elastos_Droid_Media_CMediaCryptoHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCryptoHelper)
    , public Singleton
    , public IMediaCryptoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Query if the given scheme identified by its UUID is supported on
     * this device.
     * @param uuid The UUID of the crypto scheme.
     */
    CARAPI IsCryptoSchemeSupported(
        /* [in] */ IUUID* uuid,
        /* [out] */ Boolean* result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIACRYPTOHELPER_H__
