#ifndef __ELASTOS_DROID_Server_Firewall_PortFilter_H__
#define __ELASTOS_DROID_Server_Firewall_PortFilter_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/core/Object.h"

using Elastos::Droid::internal.util.XmlUtils;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlPullParserException;
using Elastos::Utility::IArrayList;

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
        CARAPI_(Filter*) NewFilter(
            /* in */ IXmlPullParser* parser);
    };

    //@Override
    CARAPI Matches(
        /* [in] */ IIntentFirewall* ifw,
        /* [in] */ IComponentName* resolvedComponent,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 receivingUid
        /* [out] */ Boolean *ret);

public:
    static const AutoPtr<FACTORY_FilterFactory> FACTORY;

private:
    static const String ATTR_EQUALS;    // = "equals";
    static const String ATTR_MIN;       // = "min";
    static const String ATTR_MAX;       // = "max";

    static const Int32 NO_BOUND;        // = -1;

    // both bounds are inclusive
    Int32 mLowerBound;
    Int32 mUpperBound;

private:
    PortFilter(
        /* [in] */ Int32 lowerBound,
        /* [in] */ Int32 upperBound);
};

} // Firewall
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Firewall_PortFilter_H__
