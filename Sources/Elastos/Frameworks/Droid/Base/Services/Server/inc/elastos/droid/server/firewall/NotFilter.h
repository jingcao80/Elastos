#ifndef __ELASTOS_DROID_Server_Firewall_NotFilter_H__
#define __ELASTOS_DROID_Server_Firewall_NotFilter_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>
#include <elastos/droid/internal/utility/XmlUtils.h>
#include "elastos/droid/server/firewall/FilterFactory.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Utility::IArrayList;
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

        CARAPI_(AutoPtr<IFilter>) NewFilter(
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
        /* [in] */ Int32 receivingUid,
        /* [out] */ Boolean *ret);

public:
    CAR_INTERFACE_DECL()

    NotFilter(
        /* in */ IFilter* child);

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
