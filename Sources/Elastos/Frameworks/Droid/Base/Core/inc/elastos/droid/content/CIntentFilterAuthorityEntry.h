
#ifndef __ELASTOS_DROID_CONTENT_CINTENTFILTERAUTHORITYENTRY_H__
#define __ELASTOS_DROID_CONTENT_CINTENTFILTERAUTHORITYENTRY_H__

#include "_Elastos_Droid_Content_CIntentFilterAuthorityEntry.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CIntentFilterAuthorityEntry)
    , public Object
    , public IIntentFilterAuthorityEntry
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CIntentFilterAuthorityEntry();

    virtual ~CIntentFilterAuthorityEntry();

    CARAPI GetHost(
        /* [out] */ String* host);

    CARAPI GetPort(
        /* [out] */ Int32* port);

    CARAPI GetWild(
        /* [out] */ Boolean* wild);

    CARAPI Match(
        /* [in] */ IIntentFilterAuthorityEntry* other,
        /* [out] */ Boolean* result);

    /**
     * Determine whether this AuthorityEntry matches the given data Uri.
     * <em>Note that this comparison is case-sensitive, unlike formal
     * RFC host names.  You thus should always normalize to lower-case.</em>
     *
     * @param data The Uri to match.
     * @return Returns either {@link IntentFilter#NO_MATCH_DATA},
     * {@link IntentFilter#MATCH_CATEGORY_PORT}, or
     * {@link IntentFilter#MATCH_CATEGORY_HOST}.
     */
    CARAPI Match(
        /* [in] */ IUri* data,
        /* [out] */ Int32* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ const String& port);

private:
    String mOrigHost;
    String mHost;
    Boolean mWild;
    Int32 mPort;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CINTENTFILTERAUTHORITYENTRY_H__
