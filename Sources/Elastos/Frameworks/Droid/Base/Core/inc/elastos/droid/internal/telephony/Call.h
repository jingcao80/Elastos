
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CALL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CALL_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * {@hide}
 */
class Call
    : public Object
    , public ICall
{
public:
    enum SrvccState {
        NONE, STARTED, COMPLETED, FAILED, CANCELED
    };

public:
    CAR_INTERFACE_DECL()

    Call();

    virtual ~Call();

    static CARAPI_(ICallState) StateFromDCState(
        /* [in] */ IDriverCallState dcState);

    /* Instance Methods */

    /**
     * hasConnection
     *
     * @param c a Connection object
     * @return true if the call contains the connection object passed in
     */
    CARAPI HasConnection(
        /* [in] */ IConnection* c,
        /* [out] */ Boolean* result);

    /**
     * hasConnections
     * @return true if the call contains one or more connections
     */
    CARAPI HasConnections(
        /* [out] */ Boolean* result);

    /**
     * getState
     * @return state of class call
     */
    CARAPI GetState(
        /* [out] */ ICallState* result);

    /**
    * getExtras
    * @return Call Extras. Subclasses of Call that support call extras need
    *         to override this method to return the extras.
    */
    CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    /**
     * isIdle
     *
     * FIXME rename
     * @return true if the call contains only disconnected connections (if any)
     */
    CARAPI IsIdle(
        /* [out] */ Boolean* result);

    /**
     * Returns the Connection associated with this Call that was created
     * first, or NULL if there are no Connections in this Call
     */
    CARAPI GetEarliestConnection(
        /* [out] */ IConnection** result);

    CARAPI GetEarliestCreateTime(
        /* [out] */ Int64* result);

    CARAPI GetEarliestConnectTime(
        /* [out] */ Int64* result);

    CARAPI IsDialingOrAlerting(
        /* [out] */ Boolean* result);

    CARAPI IsRinging(
        /* [out] */ Boolean* result);

    /**
     * Returns the Connection associated with this Call that was created
     * last, or NULL if there are no Connections in this Call
     */
    CARAPI GetLatestConnection(
        /* [out] */ IConnection** result);

    /**
     * To indicate if the connection information is accurate
     * or not. FALSE means accurate. Only used for CDMA.
     */
    CARAPI IsGeneric(
        /* [out] */ Boolean* result);

    /**
     * Set the generic instance variable
     */
    CARAPI SetGeneric(
        /* [in] */ Boolean generic);

    /**
     * Hangup call if it is alive
     */
    CARAPI HangupIfAlive();

public:
    /* Instance Variables */
    ICallState mState;

    AutoPtr<IArrayList> mConnections;

protected:
    static const String TAG;

    // Flag to indicate if the current calling/caller information
    // is accurate. If FALSE the information is known to be accurate.
    //
    // For CDMA, during call waiting/3 way, there is no network response
    // if call waiting is answered, network timed out, dropped, 3 way
    // merged, etc.
    Boolean mIsGeneric;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CALL_H__
