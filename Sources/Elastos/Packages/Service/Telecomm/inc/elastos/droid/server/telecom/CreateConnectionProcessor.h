
#ifndef __ELASTOS_DROID_SERVER_TELECOM_CREATECONNECTIONPROCESSOR_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CREATECONNECTIONPROCESSOR_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/ConnectionServiceRepository.h"
#include "elastos/droid/server/telecom/PhoneAccountRegistrar.h"
#include <Elastos.Droid.Os.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Telecom::IDisconnectCause;
using Elastos::Droid::Telecom::IParcelableConnection;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * This class creates connections to place new outgoing calls or to attach to an existing incoming
 * call. In either case, this class cycles through a set of connection services until:
 *   - a connection service returns a newly created connection in which case the call is displayed
 *     to the user
 *   - a connection service cancels the process, in which case the call is aborted
 */
class CreateConnectionProcessor
    : public Object
{
private:
    // Describes information required to attempt to make a phone call
    class CallAttemptRecord
        : public Object
        , public ICreateConnectionProcessorCallAttemptRecord
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
            /* [in] */ IPhoneAccountHandle* targetPhoneAccount);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

        /**
         * Determines if this instance of {@code CallAttemptRecord} has the same underlying
         * {@code PhoneAccountHandle}s as another instance.
         *
         * @param obj The other instance to compare against.
         * @return {@code True} if the {@code CallAttemptRecord}s are equal.
         */
        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result);

        // The PhoneAccount describing the target connection service which we will
        // contact in order to process an attempt
        AutoPtr<IPhoneAccountHandle> mConnectionManagerPhoneAccount;
        // The PhoneAccount which we will tell the target connection service to use
        // for attempting to make the actual phone call
        AutoPtr<IPhoneAccountHandle> mTargetPhoneAccount;
    };

    class Response
        : public Object
        , public ICreateConnectionResponse
    {
    public:
        CAR_INTERFACE_DECL()
        TO_STRING_IMPL("CreateConnectionProcessor::Response");

        Response(
            /* [in] */ CreateConnectionProcessor* host);

        CARAPI constructor(
            /* [in] */ IConnectionServiceWrapper* service);

        // @Override
        CARAPI HandleCreateConnectionSuccess(
            /* [in] */ ICallIdMapper* idMapper,
            /* [in] */ IParcelableConnection* connection);

        // @Override
        CARAPI HandleCreateConnectionFailure(
            /* [in] */ IDisconnectCause* errorDisconnectCause);

    private:
        CreateConnectionProcessor* mHost;
        AutoPtr<IConnectionServiceWrapper> mService;
    };

public:
    CARAPI constructor(
        /* [in] */ ICall* call,
        /* [in] */ ConnectionServiceRepository* repository,
        /* [in] */ ICreateConnectionResponse* response,
        /* [in] */ PhoneAccountRegistrar* phoneAccountRegistrar,
        /* [in] */ IContext* context);

    CARAPI Process();

    CARAPI Abort();

private:
    CARAPI AttemptNextPhoneAccount();

    CARAPI ShouldSetConnectionManager(
        /* [out] */ Boolean* result);

    // If there exists a registered connection manager then use it.
    CARAPI AdjustAttemptsForConnectionManager();

    // plain PSTN connection services are listed, and nothing else.
    CARAPI AdjustAttemptsForEmergency();

private:
    AutoPtr<ICall> mCall;

    AutoPtr<ConnectionServiceRepository> mRepository;

    AutoPtr<IList> mAttemptRecords;

    AutoPtr<IIterator> mAttemptRecordIterator;

    AutoPtr<ICreateConnectionResponse> mResponse;

    AutoPtr<IDisconnectCause> mLastErrorDisconnectCause;

    AutoPtr<PhoneAccountRegistrar> mPhoneAccountRegistrar;

    AutoPtr<IContext> mContext;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CREATECONNECTIONPROCESSOR_H__
