#ifndef __ELASTOS_DROID_Server_Firewall_PortFilter_H__
#define __ELASTOS_DROID_Server_Firewall_PortFilter_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/server/firewall/FilterFactory.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Utility::IArrayList;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

class PortFilter
    : public Object
    , public IFilter
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
    CAR_INTERFACE_DECL()

    PortFilter(
        /* [in] */ Int32 lowerBound,
        /* [in] */ Int32 upperBound);

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

private:
    static const String ATTR_EQUALS;
    static const String ATTR_MIN;
    static const String ATTR_MAX;

    static const Int32 NO_BOUND;

    // both bounds are inclusive
    Int32 mLowerBound;
    Int32 mUpperBound;
};

} // Firewall
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Firewall_PortFilter_H__
