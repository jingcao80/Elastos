#ifndef  __ELASTOS_DROID_PHONE_CALLGATEWAYMANAGER_H__
#define  __ELASTOS_DROID_PHONE_CALLGATEWAYMANAGER_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Net::IUri;
using Elastos::Core::Object;
using Elastos::Utility::Concurrent::IConcurrentHashMap;

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * This class manages gateway information for outgoing calls. When calls are made, they may contain
 * gateway information for services which route phone calls through their own service/numbers.
 * The data consists of a number to call and the package name of the service. This data is used in
 * two ways:<br/>
 * 1. Call the appropriate routing number<br/>
 * 2. Display information about the routing to the user<br/>
 *
 * <p>When an outgoing call is finally placed in PhoneUtils.placeCall, it uses this class to get the
 * proper number to dial. It also saves an association between the connection object and the gateway
 * data into this class.
 */
class CallGatewayManager
    : public Object
{
public:
    class RawGatewayInfo
        : public Object
    {
    public:
        TO_STRING_IMPL("CallGatewayManager::RawGatewayInfo")

        RawGatewayInfo(
            /* [in] */ const String& packageName,
            /* [in] */ IUri* gatewayUri,
            /* [in] */ const String& trueNumber)
            : mPackageName(packageName)
            , mGatewayUri(gatewayUri)
            , mTrueNumber(trueNumber)
        {}

        CARAPI_(String) GetFormattedGatewayNumber();

        CARAPI_(Boolean) IsEmpty();

    public:
        String mPackageName;
        AutoPtr<IUri> mGatewayUri;
        String mTrueNumber;
    };

public:
    TO_STRING_IMPL("CallGatewayManager")

    static CARAPI_(AutoPtr<CallGatewayManager>) GetInstance();

    /**
     * Static method returns an object containing the gateway data stored in the extras of the
     * Intent parameter.  If no such data exists, returns a Null-Object RawGatewayInfo.
     * @param intent The intent from which to read gateway data.
     * @return A populated or empty RawGatewayInfo object.
     */
    static CARAPI_(AutoPtr<RawGatewayInfo>) GetRawGatewayInfo(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& number);

    /**
     * This function sets the current mapping from connection to gatewayInfo.
     * @param connection The connection object for the placed outgoing call.
     * @param gatewayInfo Gateway info gathered using getRawGatewayInfo.
     */
    CARAPI SetGatewayInfoForConnection(
        /* [in] */ IConnection* connection,
        /* [in] */ RawGatewayInfo* gatewayInfo);

    /**
     * Clears the gateway information previously stored via setGatewayInfoForConnection.
     */
    CARAPI ClearGatewayData(
        /* [in] */ IConnection* connection);

    /**
     * If the parameter matches the connection object we previously saved through
     * setGatewayInfoForConnection, return the associated raw gateway info data. If not, then
     * return an empty raw gateway info.
     */
    CARAPI_(AutoPtr<RawGatewayInfo>) GetGatewayInfo(
        /* [in] */ IConnection* connection);

    /**
     * Check if all the provider's info is present in the intent.
     * @param intent Expected to have the provider's extra.
     * @return true if the intent has all the extras to build the
     * in-call screen's provider info overlay.
     */
    static CARAPI_(Boolean) HasPhoneProviderExtras(
        /* [in] */ IIntent* intent);

    /**
     * Copy all the expected extras set when a 3rd party provider is
     * used from the source intent to the destination one.  Checks all
     * the required extras are present, if any is missing, none will
     * be copied.
     * @param src Intent which may contain the provider's extras.
     * @param dst Intent where a copy of the extras will be added if applicable.
     */
    static CARAPI CheckAndCopyPhoneProviderExtras(
        /* [in] */ IIntent* src,
        /* [in] */ IIntent* dst);

    /**
     * Return the gateway uri from the intent.
     * @param intent With the gateway uri extra.
     * @return The gateway URI or null if not found.
     */
    static CARAPI_(AutoPtr<IUri>) GetProviderGatewayUri(
        /* [in] */ IIntent* intent);

    /**
     * Return a formatted version of the uri's scheme specific
     * part. E.g for 'tel:12345678', return '1-234-5678'.
     * @param uri A 'tel:' URI with the gateway phone number.
     * @return the provider's address (from the gateway uri) formatted
     * for user display. null if uri was null or its scheme was not 'tel:'.
     */
    static CARAPI_(String) FormatProviderUri(
        /* [in] */ IUri* uri);

private:
    CallGatewayManager();

public:
    /**
     * Intent extra to specify the package name of the gateway
     * provider.  Used to get the name displayed in the in-call screen
     * during the call setup. The value is a string.
     */
    // TODO: This extra is currently set by the gateway application as
    // a temporary measure. Ultimately, the framework will securely
    // set it.
    static const String EXTRA_GATEWAY_PROVIDER_PACKAGE;

    /**
     * Intent extra to specify the URI of the provider to place the
     * call. The value is a string. It holds the gateway address
     * (phone gateway URL should start with the 'tel:' scheme) that
     * will actually be contacted to call the number passed in the
     * intent URL or in the EXTRA_PHONE_NUMBER extra.
     */
    // TODO: Should the value be a Uri (Parcelable)? Need to make sure
    // MMI code '#' don't get confused as URI fragments.
    static const String EXTRA_GATEWAY_URI;

    static AutoPtr<RawGatewayInfo> EMPTY_INFO;

private:
    static const String TAG;

    AutoPtr<IConcurrentHashMap> mMap;

    static AutoPtr<CallGatewayManager> sSingleton;
    static Object THIS;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CALLGATEWAYMANAGER_H__