#ifndef __ELASTOS_DROID_SERVER_ACCOUNTS_ACCOUNTAUTHENTICATORCACHE_H__
#define __ELASTOS_DROID_SERVER_ACCOUNTS_ACCOUNTAUTHENTICATORCACHE_H__

#include "elastos/droid/content/pm/RegisteredServicesCache.h"
#include "accounts/IAccountAuthenticatorCache.h"

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Org::Xmlpull::V1::IXmlSerializer;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Droid::Content::Pm::RegisteredServicesCache;
using Elastos::Droid::Content::Pm::IRegisteredServicesCacheListener;
using Elastos::Droid::Content::Pm::IXmlSerializerAndParser;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Accounts::IAccountAuthenticatorCache;
using Elastos::Droid::Accounts::IAuthenticatorDescription;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

/**
 * A cache of services that export the {@link IAccountAuthenticator} interface. This cache
 * is built by interrogating the {@link PackageManager} and is updated as packages are added,
 * removed and changed. The authenticators are referred to by their account type and
 * are made available via the {@link RegisteredServicesCache#getServiceInfo} method.
 * @hide
 */
class AccountAuthenticatorCache
    : public RegisteredServicesCache
    , public IAccountAuthenticatorCache
{
private:
    class MySerializer
        : public ElRefBase
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
    AccountAuthenticatorCache(
        /* [in] */ IContext* context);

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI ParseServiceAttributes(
        /* [in] */ IResources* res,
        /* [in] */ const String& packageName,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IInterface** attributes);

    /**
     * Accessor for the {@link android.content.pm.RegisteredServicesCache.ServiceInfo} that
     * matched the specified {@link android.accounts.AuthenticatorDescription} or null
     * if none match.
     * @param type the authenticator type to return
     * @return the {@link android.content.pm.RegisteredServicesCache.ServiceInfo} that
     * matches the account type or null if none is present
     */
    CARAPI_(AutoPtr<IRegisteredServicesCacheServiceInfo>) GetServiceInfo(
        /* [in] */ IAuthenticatorDescription* type,
        /* [in] */ Int32 userId);

    /**
     * @return A copy of a Collection of all the current Authenticators.
     */
    CARAPI_(AutoPtr< List<AutoPtr<IRegisteredServicesCacheServiceInfo> > >) GetAllServices(
        /* [in] */ Int32 userId);

    /**
     * Dumps the state of the cache. See
     * {@link android.os.Binder#dump(java.io.FileDescriptor, java.io.PrintWriter, String[])}
     */
    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* fout,
        /* [in] */ const ArrayOf<String>& args,
        /* [in] */ Int32 userId);

    /**
     * Sets a listener that will be notified whenever the authenticator set changes
     * @param listener the listener to notify, or null
     * @param handler the {@link Handler} on which the notification will be posted. If null
     * the notification will be posted on the main thread.
     */
    CARAPI_(void) SetListener(
        /* [in] */ IRegisteredServicesCacheListener* listener,
        /* [in] */ IHandler* handler);

    CARAPI_(void) InvalidateCache(
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
