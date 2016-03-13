#ifndef __ELASTOS_DROID_Server_Firewall_SenderFilter_H__
#define __ELASTOS_DROID_Server_Firewall_SenderFilter_H__

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

class SenderFilter
    : public Object
{
public:
    class FACTORY_FilterFactory
        : public FilterFactory
    {
    public:
        CARAPI_(Filter*) NewFilter(
            /* in */ IXmlPullParser* parser);
    };

    class SIGNATURE_Filter
        : public Filter
    {
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
    };

    class SYSTEM_Filter
        : public Filter
    {
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
    };

    class SYSTEM_OR_SIGNATURE_Filter
        : public Filter
    {
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
    };

    class USER_ID_Filter
        : public Filter
    {
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
    static Boolean IsPrivilegedApp(
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid);

public:
    static const AutoPtr<FACTORY_FilterFactory> FACTORY = new FACTORY_FilterFactory(String("sender"));

    static const AutoPtr<SIGNATURE_Filter> SIGNATURE = new SIGNATURE_Filter();
    static const AutoPtr<SYSTEM_Filter> SYSTEM = new SYSTEM_Filter();
    static const AutoPtr<SYSTEM_OR_SIGNATURE_Filter> SYSTEM_OR_SIGNATURE = new SYSTEM_OR_SIGNATURE_Filter();
    static const AutoPtr<USER_ID_Filter> USER_ID = new USER_ID_Filter();

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
