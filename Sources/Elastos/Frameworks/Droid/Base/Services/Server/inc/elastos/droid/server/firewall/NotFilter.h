#ifndef __ELASTOS_DROID_Server_Firewall_NotFilter_H__
#define __ELASTOS_DROID_Server_Firewall_NotFilter_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/firewall/FilterFactory.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

class NotFilter
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

    NotFilter(
        /* [in] */ IFilter* child);

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
    AutoPtr<IFilter> mChild;
};

} // Firewall
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Firewall_NotFilter_H__
