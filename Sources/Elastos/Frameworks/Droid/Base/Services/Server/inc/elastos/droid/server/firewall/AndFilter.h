#ifndef __ELASTOS_DROID_Server_Firewall_AndFilter_H__
#define __ELASTOS_DROID_Server_Firewall_AndFilter_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/server/firewall/FilterList.h"
#include "elastos/droid/server/firewall/FilterFactory.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Server::Firewall::IFilter;
using Elastos::Utility::IArrayList;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

class AndFilter
    : public FilterList
{
public:
    class FACTORY_FilterFactory
        : public FilterFactory
    {
    public:
        FACTORY_FilterFactory(
            /* [in] */ const String& tag);

        CARAPI NewFilter(
            /* [in] */ IXmlPullParser* parser,
            /* [out] */ IFilter** result);
    };

public:
    //@Override
    CARAPI Matches(
        /* [in] */ IIntentFirewall* ifw,
        /* [in] */ IComponentName* resolvedComponent,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 receivingUid,
        /* [out] */ Boolean *ret);

public:
    static const AutoPtr<FACTORY_FilterFactory> FACTORY;
};

} // Firewall
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Firewall_AndFilter_H__
