
#ifndef __ORG_ALLJOYN_BUS_ALLJOYN_DAEMONINIT_H__
#define __ORG_ALLJOYN_BUS_ALLJOYN_DAEMONINIT_H__

namespace Org {
namespace Alljoyn {
namespace Bus {
namespace Alljoyn {

/**
 * This class is to ensure that AllJoyn daemon is initialized to be ready for accepting connections from clients.
 * Before invoking Connect() to BusAttachment, the application should call PrepareDaemon()/PrepareDaemonAsync()
 * if it expects to use the bundled daemon no other daemons are available. PrepareDaemon() waits until the daemon
 * initialization is done. PrepareDaemonAsync() is the async version of PrepareDaemon().
 *
 */
class DaemonInit
{
public:
    static CARAPI_(AutoPtr<IContext>) GetContext();

    /**
     * Initialize daemon service if needed.
     * First it checks whether any daemon is running; if no daemon is running, then it starts the APK daemon if it is installed;
     * If no APK daemon is installed, then starts the bundled daemon. This is a blocking call; it blocks until the daemon is ready or
     * no daemon is found. Thus only non-UI thread is allowed to call PrepareDaemon().
     * @param context The application context
     * @return true  if the daemon is ready for connection
     *         false if no daemon is available
     */
    static CARAPI_(Boolean) PrepareDaemon(
        /* [in] */ IContext* context);

public:
    static AutoPtr<IBroadcastReceiver> sReceiver;

private:
    const static String TAG;
    static AutoPtr<IContext> sContext;
};

} // namespace Alljoyn
} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_ALLJOYN_DAEMONINIT_H__
