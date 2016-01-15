#ifndef __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONTOKEN_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONTOKEN_H__

#include "_Elastos_Droid_Media_Session_CMediaSessionToken.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Represents an ongoing session. This may be passed to apps by the session
 * owner to allow them to create a {@link MediaController} to communicate with
 * the session.
 */
CarClass(CMediaSessionToken)
    , public Object
    , public IMediaSessionToken
    , public IParcelable
{
public:
    CMediaSessionToken();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CMediaSessionToken();

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IISessionController * binder);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean * result);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI GetBinder(
        /* [out] */ IISessionController ** result);

public:
    AutoPtr<IISessionController> mBinder;
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONTOKEN_H__
