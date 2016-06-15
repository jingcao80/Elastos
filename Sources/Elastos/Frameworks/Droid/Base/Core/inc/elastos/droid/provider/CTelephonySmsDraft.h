#ifndef __ELASTOS_DROID_PROVIDER_CTELEPHONYSMSDRAFT_H__
#define __ELASTOS_DROID_PROVIDER_CTELEPHONYSMSDRAFT_H__

#include "_Elastos_Droid_Provider_CTelephonySmsDraft.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContentResolver;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CTelephonySmsDraft)
    , public Singleton
    , public ITelephonySmsDraft
    , public IBaseColumns
    , public ITelephonyTextBasedSmsColumns
{
public:
    CAR_INTERFACE_DECL();
    CAR_SINGLETON_DECL();

    /**
      * Add an SMS to the Draft box.
      *
      * @param resolver the content resolver to use
      * @param address the address of the sender
      * @param body the body of the message
      * @param subject the pseudo-subject of the message
      * @param date the timestamp for the message
      * @param read TRUE if the message has been read, FALSE if not
      * @return the URI for the new message
      * @hide
      */
    CARAPI AddMessage(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& address,
        /* [in] */ const String& body,
        /* [in] */ const String& subject,
        /* [in] */ Int64 date,
        /* [out] */ IUri** uri);

    /**
      * Add an SMS to the Draft box.
      *
      * @param resolver the content resolver to use
      * @param address the address of the sender
      * @param body the body of the message
      * @param subject the psuedo-subject of the message
      * @param date the timestamp for the message
      * @param read TRUE if the message has been read, FALSE if not
      * @param subId the sub_id which the message belongs to
      * @return the URI for the new message
      * @hide
      */
    CARAPI AddMessage(
        /* [in] */ Int64 subId,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& address,
        /* [in] */ const String& body,
        /* [in] */ const String& subject,
        /* [in] */ Int64 date,
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CTELEPHONYSMSDRAFT_H__
