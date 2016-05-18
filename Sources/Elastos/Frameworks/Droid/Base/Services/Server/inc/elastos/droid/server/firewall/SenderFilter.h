#ifndef __ELASTOS_DROID_Server_Firewall_SenderFilter_H__
#define __ELASTOS_DROID_Server_Firewall_SenderFilter_H__

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

class SenderFilter
    : public Object
{
public:
    class FACTORY_FilterFactory
        : public FilterFactory
    {
    public:
        FACTORY_FilterFactory(
            /* [in] */ const String& tag);

        CARAPI_(IFilter*) NewFilter(
            /* in */ IXmlPullParser* parser);
    };

    class SIGNATURE_Filter
        : public Object
        , public IFilter
    {
    public:
        CAR_INTERFACE_DECL()

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
    };

    class SYSTEM_Filter
        : public Object
        , public IFilter
    {
    public:
        CAR_INTERFACE_DECL()

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
    };

    class SYSTEM_OR_SIGNATURE_Filter
        : public Object
        , public IFilter
    {
    public:
        CAR_INTERFACE_DECL()

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
    };

    class USER_ID_Filter
        : public Object
        , public IFilter
    {
    public:
        CAR_INTERFACE_DECL()

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
    };

public:
    CAR_INTERFACE_DECL()

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
    static Boolean IsPrivilegedApp(
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid);

public:
    static const AutoPtr<FACTORY_FilterFactory> FACTORY;

    static const AutoPtr<SIGNATURE_Filter> SIGNATURE;
    static const AutoPtr<SYSTEM_Filter> SYSTEM;
    static const AutoPtr<SYSTEM_OR_SIGNATURE_Filter> SYSTEM_OR_SIGNATURE;
    static const AutoPtr<USER_ID_Filter> USER_ID;

private:
    static const String ATTR_TYPE;          // = "type";

    static const String VAL_SIGNATURE;      // = "signature";
    static const String VAL_SYSTEM;         // = "system";
    static const String VAL_SYSTEM_OR_SIGNATURE;    // = "system|signature";
    static const String VAL_USER_ID;        // = "userId";
};

} // Firewall
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Firewall_SenderFilter_H__
