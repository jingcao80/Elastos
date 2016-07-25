
#ifndef __ELASTOS_DROID_SERVER_TELECOM_PHONEACCOUNTREGISTRAR_H__
#define __ELASTOS_DROID_SERVER_TELECOM_PHONEACCOUNTREGISTRAR_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Utility::IList;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Handles writing and reading PhoneAccountHandle registration entries. This is a simple verbatim
 * delegate for all the account handling methods on {@link android.telecom.TelecomManager} as implemented in
 * {@link TelecomServiceImpl}, with the notable exception that {@link TelecomServiceImpl} is
 * responsible for security checking to make sure that the caller has proper authority over
 * the {@code ComponentName}s they are declaring in their {@code PhoneAccountHandle}s.
 */
class PhoneAccountRegistrar
    : public Object
{
public:
    class Listener
        : public Object
    {
    public:
        CARAPI OnAccountsChanged(
            /* [in] */ PhoneAccountRegistrar* registrar);

        CARAPI OnDefaultOutgoingChanged(
            /* [in] */ PhoneAccountRegistrar* registrar);

        CARAPI OnSimCallManagerChanged(
            /* [in] */ PhoneAccountRegistrar* registrar);
    };

    /**
     * The state of this {@code PhoneAccountRegistrar}.
     */
    // @VisibleForTesting
    class State
        : public Object
    {
    public:
        State();

        /**
         * The account selected by the user to be employed by default for making outgoing calls.
         * If the user has not made such a selection, then this is null.
         */
        AutoPtr<IPhoneAccountHandle> mDefaultOutgoing;

        /**
         * A {@code PhoneAccount} having {@link PhoneAccount#CAPABILITY_CONNECTION_MANAGER} which
         * manages and optimizes a user's PSTN SIM connections.
         */
        AutoPtr<IPhoneAccountHandle> mSimCallManager;

        /**
         * The complete list of {@code PhoneAccount}s known to the Telecom subsystem.
         */
        AutoPtr<IList> mAccounts;

        /**
         * The version number of the State data.
         */
        Int32 mVersionNumber;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // XML serialization
    //

    // @VisibleForTesting
    class XmlSerialization
        : public Object
    {
    public:
        /**
         * Write the supplied object to XML
         */
        virtual CARAPI WriteToXml(
            /* [in] */ IInterface* o,
            /* [in] */ IXmlSerializer* serializer) = 0;

        /**
         * Read from the supplied XML into a new object, returning null in case of an
         * unrecoverable schema mismatch or other data error. 'parser' must be already
         * positioned at the first tag that is expected to have been emitted by this
         * object's writeToXml(). This object tries to fail early without modifying
         * 'parser' if it does not recognize the data it sees.
         */
        virtual CARAPI ReadFromXml(
            /* [in] */ IXmlPullParser* parser,
            /* [in] */ Int32 version,
            /* [in] */ IContext* context,
            /* [out] */ IInterface** result) = 0;

        CARAPI WriteTextSafely(
            /* [in] */ const String& tagName,
            /* [in] */ IInterface* value,
            /* [in] */ IXmlSerializer* serializer);

        /**
         * Serializes a string array.
         *
         * @param tagName The tag name for the string array.
         * @param values The string values to serialize.
         * @param serializer The serializer.
         * @throws IOException
         */
        CARAPI WriteStringList(
            /* [in] */ const String& tagName,
            /* [in] */ IList* values,
            /* [in] */ IXmlSerializer* serializer);

        /**
         * Reads a string array from the XML parser.
         *
         * @param parser The XML parser.
         * @return String array containing the parsed values.
         * @throws IOException Exception related to IO.
         * @throws XmlPullParserException Exception related to parsing.
         */
        CARAPI ReadStringList(
            /* [in] */ IXmlPullParser* parser,
            /* [out] */ IList** result);

    private:
        static const String LENGTH_ATTRIBUTE;

        static const String VALUE_TAG;
    };

private:
    class SubStateXmlSerialization
        : public XmlSerialization
    {
    public:
        // @Override
        CARAPI WriteToXml(
            /* [in] */ IInterface* o,
            /* [in] */ IXmlSerializer* serializer);

        // @Override
        CARAPI ReadFromXml(
            /* [in] */ IXmlPullParser* parser,
            /* [in] */ Int32 version,
            /* [in] */ IContext* context,
            /* [out] */ IInterface** result);

    private:
        static const String CLASS_STATE;

        static const String DEFAULT_OUTGOING;

        static const String SIM_CALL_MANAGER;

        static const String ACCOUNTS;

        static const String VERSION;
    };

    class SubPhoneAccountXmlSerialization
        : public XmlSerialization
    {
    public:
        // @Override
        CARAPI WriteToXml(
            /* [in] */ IInterface* o,
            /* [in] */ IXmlSerializer* serializer);

        CARAPI ReadFromXml(
            /* [in] */ IXmlPullParser* parser,
            /* [in] */ Int32 version,
            /* [in] */ IContext* context,
            /* [out] */ IInterface** result);//IPhoneAccount

    private:
        /**
         * Determines if the SIP call settings specify to use SIP for all calls, including PSTN calls.
         *
         * @param context The context.
         * @return {@code True} if SIP should be used for all calls.
         */
        CARAPI UseSipForPstnCalls(
            /* [in] */ IContext* context,
            /* [out] */ Boolean* result);

        static const String CLASS_PHONE_ACCOUNT;

        static const String ACCOUNT_HANDLE;

        static const String ADDRESS;

        static const String SUBSCRIPTION_ADDRESS;

        static const String CAPABILITIES;

        static const String ICON_RES_ID;

        static const String LABEL;

        static const String SHORT_DESCRIPTION;

        static const String SUPPORTED_URI_SCHEMES;

        static const String sTRUE;

        static const String sFALSE;
    };

    class SubPhoneAccountHandleXmlSerialization
        : public XmlSerialization
    {
    public:
        // @Override
        CARAPI WriteToXml(
            /* [in] */ IInterface* o,
            /* [in] */ IXmlSerializer* serializer);

        // @Override
        CARAPI ReadFromXml(
            /* [in] */ IXmlPullParser* parser,
            /* [in] */ Int32 version,
            /* [in] */ IContext* context,
            /* [out] */ IInterface** result);// IPhoneAccountHandle

    private:
        static const String CLASS_PHONE_ACCOUNT_HANDLE;

        static const String COMPONENT_NAME;

        static const String ID;
    };

public:
    PhoneAccountRegistrar();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @VisibleForTesting
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& fileName);

    /**
     * Retrieves the default outgoing phone account supporting the specified uriScheme.
     * @param uriScheme The URI scheme for the outgoing call.
     * @return The {@link PhoneAccountHandle} to use.
     */
    CARAPI GetDefaultOutgoingPhoneAccount(
        /* [in] */ const String& uriScheme,
        /* [out] */ IPhoneAccountHandle** result);

    CARAPI GetUserSelectedOutgoingPhoneAccount(
        /* [out] */ IPhoneAccountHandle** result);

    CARAPI GetUserSelectedVoicePhoneAccount(
        /* [out] */ IPhoneAccountHandle** result);

    CARAPI SetUserSelectedOutgoingPhoneAccount(
        /* [in] */ IPhoneAccountHandle* accountHandle);

    CARAPI SetSimCallManager(
        /* [in] */ IPhoneAccountHandle* callManager);

    CARAPI GetSimCallManager(
        /* [out] */ IPhoneAccountHandle** result);

    /**
     * Retrieves a list of all {@link PhoneAccountHandle}s registered.
     *
     * @return The list of {@link PhoneAccountHandle}s.
     */
    CARAPI GetAllPhoneAccountHandles(
        /* [out] */ IList** result);

    CARAPI GetAllPhoneAccounts(
        /* [out] */ IList** result);

    /**
     * Determines the number of all {@link PhoneAccount}s.
     *
     * @return The total number {@link PhoneAccount}s.
     */
    CARAPI GetAllPhoneAccountsCount(
        /* [out] */ Int32* result);

    /**
     * Retrieves a list of all call provider phone accounts.
     *
     * @return The phone account handles.
     */
    CARAPI GetCallCapablePhoneAccounts(
        /* [out] */ IList** result);

    /**
     * Retrieves a list of all phone account call provider phone accounts supporting the
     * specified URI scheme.
     *
     * @param uriScheme The URI scheme.
     * @return The phone account handles.
     */
    CARAPI GetCallCapablePhoneAccounts(
        /* [in] */ const String& uriScheme,
        /* [out] */ IList** result);

    /**
     * Retrieves a list of all phone accounts registered by a specified package.
     *
     * @param packageName The name of the package that registered the phone accounts.
     * @return The phone account handles.
     */
    CARAPI GetPhoneAccountsForPackage(
        /* [in] */ const String& packageName,
        /* [out] */ IList** result);

    /**
     * Retrieves a list of all phone account handles with the connection manager capability.
     *
     * @return The phone account handles.
     */
    CARAPI GetConnectionManagerPhoneAccounts(
        /* [out] */ IList** result);

    CARAPI GetPhoneAccount(
        /* [in] */ IPhoneAccountHandle* handle,
        /* [out] */ IPhoneAccount** result);

    // ComponentName?
    CARAPI RegisterPhoneAccount(
        /* [in] */ IPhoneAccount* account);

    CARAPI UnregisterPhoneAccount(
        /* [in] */ IPhoneAccountHandle* accountHandle);

    /**
     * Un-registers all phone accounts associated with a specified package.
     *
     * @param packageName The package for which phone accounts will be removed.
     */
    CARAPI ClearAccounts(
        /* [in] */ const String& packageName);

    CARAPI AddListener(
        /* [in] */ Listener* l);

    CARAPI RemoveListener(
        /* [in] */ Listener* l);

    /**
     * Determines if the connection service specified by a {@link PhoneAccountHandle} has the
     * {@link Manifest.permission#BIND_CONNECTION_SERVICE} permission.
     *
     * @param phoneAccountHandle The phone account to check.
     * @return {@code True} if the phone account has permission.
     */
    CARAPI PhoneAccountHasPermission(
        /* [in] */ IPhoneAccountHandle* phoneAccountHandle,
        /* [out] */ Boolean* result);

private:
    CARAPI SetDefaultVoicePhoneAccount(
        /* [in] */ IPhoneAccountHandle* accountHandle);

    /**
     * Adds a {@code PhoneAccount}, replacing an existing one if found.
     *
     * @param account The {@code PhoneAccount} to add or replace.
     */
    CARAPI AddOrReplacePhoneAccount(
        /* [in] */ IPhoneAccount* account);

    CARAPI FireAccountsChanged();

    CARAPI FireDefaultOutgoingChanged();

    CARAPI FireSimCallManagerChanged();

    /**
     * Returns a list of phone account handles with the specified flag.
     *
     * @param flags Flags which the {@code PhoneAccount} must have.
     */
    CARAPI GetPhoneAccountHandles(
        /* [in] */ Int32 flags,
        /* [out] */ IList** result);

    /**
     * Returns a list of phone account handles with the specified flag, supporting the specified
     * URI scheme.
     *
     * @param flags Flags which the {@code PhoneAccount} must have.
     * @param uriScheme URI schemes the PhoneAccount must handle.  {@code Null} bypasses the
     *                  URI scheme check.
     */
    CARAPI GetPhoneAccountHandles(
        /* [in] */ Int32 flags,
        /* [in] */ const String& uriScheme,
        /* [out] */ IList** result);

    //
    CARAPI Write();

    CARAPI Read();

    static CARAPI WriteToXml(
        /* [in] */ State* state,
        /* [in] */ IXmlSerializer* serializer);

    static CARAPI ReadFromXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IContext* context,
        /* [out] */ State** result);

    static CARAPI_(AutoPtr<IPhoneAccountHandle>) InitNO_ACCOUNT_SELECTED();

public:
    static AutoPtr<IPhoneAccountHandle> NO_ACCOUNT_SELECTED;

    // @VisibleForTesting
    static const Int32 EXPECTED_STATE_VERSION;

    // @VisibleForTesting
    static AutoPtr<XmlSerialization> sStateXml;

    // @VisibleForTesting
    static AutoPtr<XmlSerialization> sPhoneAccountXml;

    // @VisibleForTesting
    static AutoPtr<XmlSerialization> sPhoneAccountHandleXml;

private:
    static const String FILE_NAME;

    /** Keep in sync with the same in SipSettings.java */
    static const String SIP_SHARED_PREFERENCES;

    AutoPtr<IList> mListeners;

    AutoPtr<IAtomicFile> mAtomicFile;

    AutoPtr<IContext> mContext;

    AutoPtr<State> mState;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_PHONEACCOUNTREGISTRAR_H__
