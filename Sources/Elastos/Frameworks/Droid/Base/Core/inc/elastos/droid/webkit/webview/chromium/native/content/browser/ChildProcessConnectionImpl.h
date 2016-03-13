
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CHILDPROCESSCONNECTIONIMPL_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CHILDPROCESSCONNECTIONIMPL_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ChildProcessConnection.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/FileDescriptorInfo.h"
#include "elastos/droid/webkit/webview/chromium/native/content/app/ChildProcessService.h"
#include "elastos/droid/webkit/webview/chromium/native/content/app/ChromiumLinkerParams.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::IBundle;
// import android.os.DeadObjectException;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;
// import android.os.RemoteException;
// import android.util.Log;

// import com.google.common.annotations.VisibleForTesting;

// import org.chromium.base.CpuFeatures;
// import org.chromium.base.ThreadUtils;
// import org.chromium.base.TraceEvent;
// import org.chromium.base.library_loader.Linker;
using Elastos::Droid::Webkit::Webview::Chromium::Content::App::ChildProcessService;
using Elastos::Droid::Webkit::Webview::Chromium::Content::App::ChromiumLinkerParams;
// import org.chromium.content.common.IChildProcessCallback;
// import org.chromium.content.common.IChildProcessService;

// import java.io.IOException;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
 * Manages a connection between the browser activity and a child service.
 */
class ChildProcessConnectionImpl
    : public ChildProcessConnection
{
private:
    class ConnectionParams : public Object
    {
    public:
        ConnectionParams(
            /* [in] */ ArrayOf<String>* commandLine,
            /* [in] */ ArrayOf<FileDescriptorInfo*>* filesToBeMapped,
            /* [in] */ IInterface/*IChildProcessCallback*/* callback,
            /* [in] */ IBundle* sharedRelros);

    public:
        AutoPtr< ArrayOf<String> > mCommandLine;
        AutoPtr< ArrayOf<FileDescriptorInfo*> > mFilesToBeMapped;
        AutoPtr<IInterface/*IChildProcessCallback*/> mCallback;
        AutoPtr<IBundle> mSharedRelros;
    };

    class ChildServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        ChildServiceConnection(
            /* [in] */ ChildProcessConnectionImpl* owner,
            /* [in] */ Int32 bindFlags);

        CAR_INTERFACE_DECL()

        CARAPI_(Boolean) Bind(
            /* [in] */ ArrayOf<String>* commandLine);

        CARAPI_(void) Unbind();

        CARAPI_(Boolean) IsBound();

        //@Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* className,
            /* [in] */ IBinder* service);

        // Called on the main thread to notify that the child service did not disconnect gracefully.
        //@Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* className);

    private:
        CARAPI_(AutoPtr<IIntent>) CreateServiceBindIntent();

    private:
        ChildProcessConnectionImpl* mOwner;
        Boolean mBound;
        const Int32 mBindFlags;
    };

public:
    ChildProcessConnectionImpl(
        /* [in] */ IContext* context,
        /* [in] */ Int32 number,
        /* [in] */ Boolean inSandbox,
        /* [in] */ ChildProcessConnection::DeathCallback* deathCallback,
        /* [in] */ ChildProcessService* serviceClass,
        /* [in] */ ChromiumLinkerParams* chromiumLinkerParams);

    //@Override
    CARAPI_(Int32) GetServiceNumber();

    //@Override
    CARAPI_(Boolean) IsInSandbox();

    //@Override
    CARAPI_(AutoPtr<ChildProcessService/*IChildProcessService*/>) GetService();

    //@Override
    CARAPI_(Int32) GetPid();

    //@Override
    CARAPI_(void) Start(
        /* [in] */ ArrayOf<String>* commandLine);

    //@Override
    CARAPI_(void) SetupConnection(
        /* [in] */ ArrayOf<String>* commandLine,
        /* [in] */ ArrayOf<FileDescriptorInfo*>* filesToBeMapped,
        /* [in] */ IInterface/*IChildProcessCallback*/* processCallback,
        /* [in] */ ConnectionCallback* connectionCallback,
        /* [in] */ IBundle* sharedRelros);

    //@Override
    CARAPI_(void) Stop();

    //@Override
    CARAPI_(Boolean) IsInitialBindingBound();

    //@Override
    CARAPI_(Boolean) IsStrongBindingBound();

    //@Override
    CARAPI_(void) RemoveInitialBinding();

    //@Override
    CARAPI_(Boolean) IsOomProtectedOrWasWhenDied();

    //@Override
    CARAPI_(void) DropOomBindings();

    //@Override
    CARAPI_(void) AddStrongBinding();

    //@Override
    CARAPI_(void) RemoveStrongBinding();

    //@VisibleForTesting
    CARAPI_(Boolean) CrashServiceForTesting();

    //@VisibleForTesting
    CARAPI_(Boolean) IsConnected();

private:
    /**
     * Called after the connection parameters have been set (in setupConnection()) *and* a
     * connection has been established (as signaled by onServiceConnected()). These two events can
     * happen in any order. Has to be called with mLock.
     */
    CARAPI_(void) DoConnectionSetupLocked();

private:
    AutoPtr<IContext> mContext;
    const Int32 mServiceNumber;
    const Boolean mInSandbox;
    const AutoPtr<ChildProcessConnection::DeathCallback> mDeathCallback;
    const AutoPtr<ChildProcessService/*TODO IInterface*/> mServiceClass;

    // Synchronization: While most internal flow occurs on the UI thread, the public API
    // (specifically start and stop) may be called from any thread, hence all entry point methods
    // into the class are synchronized on the lock to protect access to these members.
    Object mLock;
    AutoPtr<ChildProcessService/*TODO IChildProcessService*/> mService;
    // Set to true when the service connected successfully.
    Boolean mServiceConnectComplete;
    // Set to true when the service disconnects, as opposed to being properly closed. This happens
    // when the process crashes or gets killed by the system out-of-memory killer.
    Boolean mServiceDisconnected;
    // When the service disconnects (i.e. mServiceDisconnected is set to true), the status of the
    // oom bindings is stashed here for future inspection.
    Boolean mWasOomProtected;
    Int32 mPid;  // Process ID of the corresponding child process.
    // Initial binding protects the newly spawned process from being killed before it is put to use,
    // it is maintained between calls to start() and removeInitialBinding().
    AutoPtr<ChildServiceConnection> mInitialBinding;
    // Strong binding will make the service priority equal to the priority of the activity. We want
    // the OS to be able to kill background renderers as it kills other background apps, so strong
    // bindings are maintained only for services that are active at the moment (between
    // addStrongBinding() and removeStrongBinding()).
    AutoPtr<ChildServiceConnection> mStrongBinding;
    // Low priority binding maintained in the entire lifetime of the connection, i.e. between calls
    // to start() and stop().
    AutoPtr<ChildServiceConnection> mWaivedBinding;
    // Incremented on addStrongBinding(), decremented on removeStrongBinding().
    Int32 mStrongBindingCount;

    // Linker-related parameters.
    AutoPtr<ChromiumLinkerParams> mLinkerParams;

    static const String TAG;

    // This is set in setupConnection() and is later used in doConnectionSetupLocked(), after which
    // the variable is cleared. Therefore this is only valid while the connection is being set up.
    AutoPtr<ConnectionParams> mConnectionParams;

    // Callback provided in setupConnection() that will communicate the result to the caller. This
    // has to be called exactly once after setupConnection(), even if setup fails, so that the
    // caller can free up resources associated with the setup attempt. This is set to null after the
    // call.
    AutoPtr<ChildProcessConnection::ConnectionCallback> mConnectionCallback;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CHILDPROCESSCONNECTIONIMPL_H__
