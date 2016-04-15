#ifndef __ELASTOS_DROID_SERVER_ACCOUNTS_ACCOUNTAUTHENTICATORCACHE_H__
#define __ELASTOS_DROID_SERVER_ACCOUNTS_ACCOUNTAUTHENTICATORCACHE_H__

#include <_Elastos.Droid.Server.h>
#include <elastos/core/Object.h>
#include <elastos/droid/content/pm/RegisteredServicesCache.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Accounts::IAuthenticatorDescription;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IRegisteredServicesCacheListener;
using Elastos::Droid::Content::Pm::IRegisteredServicesCacheServiceInfo;
using Elastos::Droid::Content::Pm::IXmlSerializerAndParser;
using Elastos::Droid::Content::Pm::RegisteredServicesCache;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Server::Accounts::IIAccountAuthenticatorCache;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::ICollection;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

/**
 * A cache of services that export the {@link IIAccountAuthenticator} interface. This cache
 * is built by interrogating the {@link PackageManager} and is updated as packages are added,
 * removed and changed. The authenticators are referred to by their account type and
 * are made available via the {@link RegisteredServicesCache#getServiceInfo} method.
 * @hide
 */
class AccountAuthenticatorCache
    : public RegisteredServicesCache
    , public IIAccountAuthenticatorCache
{
private:
    class MySerializer
        : public Object
        , public IXmlSerializerAndParser
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI WriteAsXml(
            /* [in] */ IInterface* item,
            /* [in] */ IXmlSerializer* serializer);

        CARAPI CreateFromXml(
            /* [in] */ IXmlPullParser* parser,
            /* [out] */ IInterface** obj);
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI ParseServiceAttributes(
        /* [in] */ IResources* res,
        /* [in] */ const String& packageName,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IInterface** attributes);

    CARAPI GetServiceInfo(
        /* [in] */ IAuthenticatorDescription* type,
        /* [in] */ Int32 userId,
        /* [out] */ IRegisteredServicesCacheServiceInfo** result);

    CARAPI GetAllServices(
        /* [in] */ Int32 userId,
        /* [out] */ ICollection** result);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* fout,
        /* [in] */ ArrayOf<String>* args,
        /* [in] */ Int32 userId);

    CARAPI SetListener(
        /* [in] */ IRegisteredServicesCacheListener* listener,
        /* [in] */ IHandler* handler);

    CARAPI InvalidateCache(
        /* [in] */ Int32 userId);

private:
    static const String TAG;
    static AutoPtr<MySerializer> sSerializer;
};

} // Accounts
} // server
} // Droid
} // Elastos

#endif //__AccountAuthenticatorCache_H__
