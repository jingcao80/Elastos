#ifndef __ELASTOS_DROID_PROVIDER_CCALLLOG_H__
#define __ELASTOS_DROID_PROVIDER_CCALLLOG_H__

#include "_Elastos_Droid_Provider_CCallLog.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CCallLog)
    , public Singleton
    , public ICallLog
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URL for this provider
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

public:
    static const AutoPtr<IUri> CONTENT_URI;
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCALLLOG_H__
