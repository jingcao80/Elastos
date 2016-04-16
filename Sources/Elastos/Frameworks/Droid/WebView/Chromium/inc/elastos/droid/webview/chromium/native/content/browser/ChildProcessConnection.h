
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CHILDPROCESSCONNECTION_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CHILDPROCESSCONNECTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "elastos/droid/webkit/webview/chromium/native/content/browser/FileDescriptorInfo.h"
#include "elastos/droid/webkit/webview/chromium/native/content/app/ChildProcessService.h"

using Elastos::Droid::Os::IBundle;

// import org.chromium.content.common.IChildProcessCallback;
// import org.chromium.content.common.IChildProcessService;
using Elastos::Droid::Webkit::Webview::Chromium::Content::App::ChildProcessService;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
 * Manages a connection between the browser activity and a child service. ChildProcessConnection is
 * responsible for estabilishing the connection (start()), closing it (stop()) and manipulating the
 * bindings held onto the service (addStrongBinding(), removeStrongBinding(),
 * removeInitialBinding()).
 */
class ChildProcessConnection : public Object
{
public:
    /**
     * Used to notify the consumer about disconnection of the service. This callback is provided
     * earlier than ConnectionCallbacks below, as a child process might die before the connection is
     * fully set up.
     */
    class DeathCallback : public Object
    {
    public:
        virtual CARAPI_(void) OnChildProcessDied(
            /* [in] */ ChildProcessConnection* connection) = 0;
    };

    /**
     * Used to notify the consumer about the connection being established.
     */
    class ConnectionCallback : public Object
    {
    public:
        /**
         * Called when the connection to the service is established.
         * @param pid the pid of the child process
         */
        virtual CARAPI_(void) OnConnected(
            /* [in] */ Int32 pid) = 0;
    };

public:
    // Names of items placed in the bind intent or connection bundle.
    static const String EXTRA_COMMAND_LINE;
    // Note the FDs may only be passed in the connection bundle.
    static const String EXTRA_FILES_PREFIX;

    static const String EXTRA_FILES_ID_SUFFIX;

    static const String EXTRA_FILES_FD_SUFFIX;

    // Used to pass the CPU core count to child processes.
    static const String EXTRA_CPU_COUNT;

    // Used to pass the CPU features mask to child processes.
    static const String EXTRA_CPU_FEATURES;

public:
    virtual CARAPI_(Int32) GetServiceNumber() = 0;

    virtual CARAPI_(Boolean) IsInSandbox() = 0;

    virtual CARAPI_(AutoPtr<ChildProcessService>) GetService() = 0;  /*TODO IChildProcessService*/

    /**
     * @return the connection pid, or 0 if not yet connected
     */
    virtual CARAPI_(Int32) GetPid() = 0;

    /**
     * Starts a connection to an IChildProcessService. This must be followed by a call to
     * setupConnection() to setup the connection parameters. start() and setupConnection() are
     * separate to allow to pass whatever parameters are available in start(), and complete the
     * remainder later while reducing the connection setup latency.
     * @param commandLine (optional) command line for the child process. If omitted, then
     *                    the command line parameters must instead be passed to setupConnection().
     */
    virtual CARAPI_(void) Start(
        /* [in] */ ArrayOf<String>* commandLine) = 0;

    /**
     * Setups the connection after it was started with start().
     * @param commandLine (optional) will be ignored if the command line was already sent in start()
     * @param filesToBeMapped a list of file descriptors that should be registered
     * @param processCallback used for status updates regarding this process connection
     * @param connectionCallback will be called exactly once after the connection is set up or the
     *                           setup fails
     */
    virtual CARAPI_(void) SetupConnection(
        /* [in] */ ArrayOf<String>* commandLine,
        /* [in] */ ArrayOf<FileDescriptorInfo*>* filesToBeMapped,
        /* [in] */ IInterface/*TODO IChildProcessCallback*/* processCallback,
        /* [in] */ ConnectionCallback* connectionCallback,
        /* [in] */ IBundle* sharedRelros) = 0;

    /**
     * Terminates the connection to IChildProcessService, closing all bindings. It is safe to call
     * this multiple times.
     */
    virtual CARAPI_(void) Stop() = 0;

    /** @return true iff the initial oom binding is currently bound. */
    virtual CARAPI_(Boolean) IsInitialBindingBound() = 0;

    /** @return true iff the strong oom binding is currently bound. */
    virtual CARAPI_(Boolean) IsStrongBindingBound() = 0;

    /**
     * Called to remove the strong binding estabilished when the connection was started. It is safe
     * to call this multiple times.
     */
    virtual CARAPI_(void) RemoveInitialBinding() = 0;

    /**
     * For live connections, this returns true iff either the initial or the strong binding is
     * bound, i.e. the connection has at least one oom binding. For connections that disconnected
     * (did not exit properly), this returns true iff the connection had at least one oom binding
     * when it disconnected.
     */
    virtual CARAPI_(Boolean) IsOomProtectedOrWasWhenDied() = 0;

    /**
     * Unbinds the bindings that protect the process from oom killing. It is safe to call this
     * multiple times, before as well as after stop().
     */
    virtual CARAPI_(void) DropOomBindings() = 0;

    /**
     * Attaches a strong binding that will make the service as important as the main process. Each
     * call should be succeeded by removeStrongBinding(), but multiple strong bindings can be
     * requested and released independently.
     */
    virtual CARAPI_(void) AddStrongBinding() = 0;

    /**
     * Called when the service is no longer in active use of the consumer.
     */
    virtual CARAPI_(void) RemoveStrongBinding() = 0;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
DEFINE_CONVERSION_FOR(Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ChildProcessConnection, IInterface)

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CHILDPROCESSCONNECTION_H__
