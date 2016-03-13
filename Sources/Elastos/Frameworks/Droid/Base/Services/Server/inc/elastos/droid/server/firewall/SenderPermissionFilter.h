#ifndef __ELASTOS_DROID_Server_Firewall_SenderPermissionFilter_H__
#define __ELASTOS_DROID_Server_Firewall_SenderPermissionFilter_H__

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

class SenderPermissionFilter
    : public Object
    , public IFilter;
{
public:
    class FACTORY_FilterFactory
        : public FilterFactory
    {
    public:
        CARAPI_(IFilter*) NewFilter(
            /* in */ IXmlPullParser* parser);
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
        /* [in] */ Int32 receivingUid
        /* [out] */ Boolean *ret);

public:
    SenderPermissionFilter(
        /* in */ const String& permission);

public:
    static const AutoPtr<FACTORY_FilterFactory> FACTORY;

private:
    static const String ATTR_TYPE;          // = "name";
    String mPackageName;
};

} // Firewall
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Firewall_SenderPermissionFilter_H__
