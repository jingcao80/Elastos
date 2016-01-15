
#ifndef __ELASTOS_DROID_OS_COMMONCLOCK_H__
#define __ELASTOS_DROID_OS_COMMONCLOCK_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Net::IInetSocketAddress;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Used for accessing the android common time service's common clock and receiving notifications
 * about common time synchronization status changes.
 * @hide
 */
class CommonClock
    : public Object
{
public:
    /**
     * Handy class factory method.
     */
    static AutoPtr<CommonClock> Create();

    /**
     * Release all native resources held by this {@link android.os.CommonClock} instance.  Once
     * resources have been released, the {@link android.os.CommonClock} instance is disconnected from
     * the native service and will throw a {@link android.os.RemoteException} if any of its
     * methods are called.  Clients should always call release on their client instances before
     * releasing their last Java reference to the instance.  Failure to do this will cause
     * non-deterministic native resource reclamation and may cause the common time service to remain
     * active on the network for longer than it should.
     */
    CARAPI ReleaseResources();

    /**
     * Gets the common clock's current time.
     *
     * @return a signed 64-bit value representing the current common time in microseconds, or the
     * special value {@link #TIME_NOT_SYNCED} if the common time service is currently not
     * synchronized.
     * @throws android.os.RemoteException
     */
    CARAPI GetTime(
        /* [out] */ Int64* time);

    /**
     * Gets the current estimation of common clock's synchronization accuracy from the common time
     * service.
     *
     * @return a signed 32-bit value representing the common time service's estimation of
     * synchronization accuracy in microseconds, or the special value
     * {@link #ERROR_ESTIMATE_UNKNOWN} if the common time service is currently not synchronized.
     * Negative values indicate that the local server estimates that the nominal common time is
     * behind the local server's time (in other words, the local clock is running fast) Positive
     * values indicate that the local server estimates that the nominal common time is ahead of the
     * local server's time (in other words, the local clock is running slow)
     * @throws android.os.RemoteException
     */
    CARAPI GetEstimatedError(
        /* [out] */ Int32* error);

    /**
     * Gets the ID of the timeline the common time service is currently synchronizing its clock to.
     *
     * @return a long representing the unique ID of the timeline the common time service is
     * currently synchronizing with, or {@link #INVALID_TIMELINE_ID} if the common time service is
     * currently not synchronized.
     * @throws android.os.RemoteException
     */
    CARAPI GetTimelineId(
        /* [out] */ Int64* time);

    /**
     * Gets the current state of this clock's common time service in the the master election
     * algorithm.
     *
     * @return a integer indicating the current state of the this clock's common time service in the
     * master election algorithm or {@link #STATE_INVALID} if there is an internal error.
     * @throws android.os.RemoteException
     */
    CARAPI GetState(
        /* [out] */ Int32* state);

    /**
     * Gets the IP address and UDP port of the current timeline master.
     *
     * @return an InetSocketAddress containing the IP address and UDP port of the current timeline
     * master, or null if there is no current master.
     * @throws android.os.RemoteException
     */
    CARAPI GetMasterAddr(
        /* [out] */ IInetSocketAddress** isa);

    /**
     * Registers an OnTimelineChangedListener interface.
     * <p>Call this method with a null listener to stop receiving server death notifications.
     */
    CARAPI SetTimelineChangedListener(
        /* [in] */ ICommonClockOnTimelineChangedListener* listener);

    /**
     * Registers an OnServerDiedListener interface.
     * <p>Call this method with a null listener to stop receiving server death notifications.
     */
    CARAPI SetServerDiedListener(
        /* [in] */ ICommonClockOnServerDiedListener* listener);

private:

    CommonClock();

    ~CommonClock();

    CARAPI constructor();

    CARAPI ThrowOnDeadServer();

    CARAPI RegisterTimelineChangeListener();

    CARAPI UnregisterTimelineChangeListener();

public:
    /**
     * Sentinel value returned by {@link #getTime()} and {@link #getEstimatedError()} when the
     * common time service is not able to determine the current common time due to a lack of
     * synchronization.
     */
    static const Int64 TIME_NOT_SYNCED;// = -1;

    /**
     * Sentinel value returned by {@link #getTimelineId()} when the common time service is not
     * currently synced to any timeline.
     */
    static const Int64 INVALID_TIMELINE_ID;// = 0;

    /**
     * Sentinel value returned by {@link #getEstimatedError()} when the common time service is not
     * currently synced to any timeline.
     */
    static const Int32 ERROR_ESTIMATE_UNKNOWN;// = 0x7FFFFFFF;

    /**
     * Value used by {@link #getState()} to indicate that there was an internal error while
     * attempting to determine the state of the common time service.
     */
    static const Int32 STATE_INVALID;// = -1;

    /**
     * Value used by {@link #getState()} to indicate that the common time service is in its initial
     * state and attempting to find the current timeline master, if any.  The service will
     * transition to either {@link #STATE_CLIENT} if it finds an active master, or to
     * {@link #STATE_MASTER} if no active master is found and this client becomes the master of a
     * new timeline.
     */
    static const Int32 STATE_INITIAL;// = 0;

    /**
     * Value used by {@link #getState()} to indicate that the common time service is in its client
     * state and is synchronizing its time to a different timeline master on the network.
     */
    static const Int32 STATE_CLIENT;// = 1;

    /**
     * Value used by {@link #getState()} to indicate that the common time service is in its master
     * state and is serving as the timeline master for other common time service clients on the
     * network.
     */
    static const Int32 STATE_MASTER;// = 2;

    /**
     * Value used by {@link #getState()} to indicate that the common time service is in its Ronin
     * state.  Common time service instances in the client state enter the Ronin state after their
     * timeline master becomes unreachable on the network.  Common time services who enter the Ronin
     * state will begin a new master election for the timeline they were recently clients of.  As
     * clients detect they are not the winner and drop out of the election, they will transition to
     * the {@link #STATE_WAIT_FOR_ELECTION} state.  When there is only one client remaining in the
     * election, it will assume ownership of the timeline and transition to the
     * {@link #STATE_MASTER} state.  During the election, all clients will allow their timeline to
     * drift without applying correction.
     */
    static const Int32 STATE_RONIN;// = 3;

    /**
     * Value used by {@link #getState()} to indicate that the common time service is waiting for a
     * master election to conclude and for the new master to announce itself before transitioning to
     * the {@link #STATE_CLIENT} state.  If no new master announces itself within the timeout
     * threshold, the time service will transition back to the {@link #STATE_RONIN} state in order
     * to restart the election.
     */
    static const Int32 STATE_WAIT_FOR_ELECTION;// = 4;

    /**
     * Name of the underlying native binder service
     */
    static const String SERVICE_NAME;// = "common_time.clock";

private:
    Object mListenerLock;
    AutoPtr<ICommonClockOnTimelineChangedListener> mTimelineChangedListener;
    AutoPtr<ICommonClockOnServerDiedListener> mServerDiedListener;

    AutoPtr<IBinder> mRemote;
    String mInterfaceDesc;// = "";
    AutoPtr<ICommonTimeUtils> mUtils;

    // AutoPtr<TimelineChangedListener> mCallbackTgt;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_COMMONCLOCK_H__
