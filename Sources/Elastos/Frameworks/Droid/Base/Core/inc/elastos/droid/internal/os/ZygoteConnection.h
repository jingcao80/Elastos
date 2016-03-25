
#ifndef __ELASTOS_DROID_INTERNAL_OS_ZYGOTECONNECTION_H__
#define __ELASTOS_DROID_INTERNAL_OS_ZYGOTECONNECTION_H__

#include "Elastos.Droid.Core.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Net::ILocalSocket;
using Elastos::Droid::Net::ICredentials;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::IBufferedReader;
using Elastos::IO::IPrintStream;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

class ZygoteConnection : public Object
{
public:
    /**
     * Handles argument parsing for args related to the zygote spawner.
     *
     * Current recognized args:
     * <ul>
     *   <li> --setuid=<i>uid of child process, defaults to 0</i>
     *   <li> --setgid=<i>gid of child process, defaults to 0</i>
     *   <li> --setgroups=<i>comma-separated list of supplimentary gid's</i>
     *   <li> --capabilities=<i>a pair of comma-separated integer strings
     * indicating Linux capabilities(2) set for child. The first string
     * represents the <code>permitted</code> set, and the second the
     * <code>effective</code> set. Precede each with 0 or
     * 0x for octal or hexidecimal value. If unspecified, both default to 0.
     * This parameter is only applied if the uid of the new process will
     * be non-0. </i>
     *   <li> --rlimit=r,c,m<i>tuple of values for setrlimit() call.
     *    <code>r</code> is the resource, <code>c</code> and <code>m</code>
     *    are the settings for current and max value.</i>
     *   <li> --classpath=<i>colon-separated classpath</i> indicates
     * that the specified class (which must b first non-flag argument) should
     * be loaded from jar files in the specified classpath. Incompatible with
     * --runtime-init
     *   <li> --runtime-init indicates that the remaining arg list should
     * be handed off to com.android.internal.os.RuntimeInit, rather than
     * processed directly
     * Android runtime startup (eg, Binder initialization) is also eschewed.
     *   <li> --nice-name=<i>nice name to appear in ps</i>
     *   <li> If <code>--runtime-init</code> is present:
     *      [--] &lt;args for RuntimeInit &gt;
     *   <li> If <code>--runtime-init</code> is absent:
     *      [--] &lt;classname&gt; [args...]
     *   <li> --instruction-set=<i>instruction-set-string</i> which instruction set to use/emulate.
     * </ul>
     */
    class Arguments : public Object
    {
    public:
        Arguments(
            /* [in] */ const ArrayOf<String>& args);

    private:
        /**
         * Parses the commandline arguments intended for the Zygote spawner
         * (such as "--setuid=" and "--setgid=") and creates an array
         * containing the remaining args.
         *
         * Per security review bug #1112214, duplicate args are disallowed in
         * critical cases to make injection harder.
         */
        CARAPI ParseArgs(
            /* [in] */ const ArrayOf<String>& args);

    public:
        /** from --setuid */
        Int32 mUid;
        Boolean mUidSpecified;

        /** from --setgid */
        Int32 mGid;
        Boolean mGidSpecified;

        /** from --setgroups */
        AutoPtr< ArrayOf<Int32> > mGids;

        /**
         * From --enable-debugger, --enable-checkjni, --enable-assert,
         * --enable-safemode, and --enable-jni-logging.
         */
        Int32 mDebugFlags;

        /** From --mount-external */
        Int32 mMountExternal;

        /** from --target-sdk-version. */
        Int32 mTargetSdkVersion;
        Boolean mTargetSdkVersionSpecified;

        /** from --classpath */
        String mClasspath;

        /** from --runtime-init */
        Boolean mRuntimeInit;

        /** from --nice-name */
        String mNiceName;

        /** from --capabilities */
        Boolean mCapabilitiesSpecified;
        Int64 mPermittedCapabilities;
        Int64 mEffectiveCapabilities;

        /** from --seinfo */
        Boolean mSeInfoSpecified;
        String mSeInfo;

        /** from all --rlimit=r,c,m */
        AutoPtr<List< Int32Array > > mRlimits;

        /** from --invoke-with */
        String mInvokeWith;

        /**
         * Any args after and including the first non-option arg
         * (or after a '--')
         */
        AutoPtr< ArrayOf<String> > mRemainingArgs;

        /**
         * Whether the current arguments constitute an ABI list query.
         */
        Boolean mAbiListQuery;

        /**
         * The instruction set to use, or null when not important.
         */
        String mInstructionSet;

        /**
         * The app data directory. May be null, e.g., for the system server. Note that this might
         * not be reliable in the case of process-sharing apps.
         */
        String mAppDataDir;

        /** from --refresh_theme */
        Boolean mRefreshTheme;
    };

public:
    ZygoteConnection();

    /**
     * Constructs instance from connected socket.
     *
     * @param socket non-null; connected socket
     * @throws IOException
     */
    CARAPI constructor(
        /* [in] */ ILocalSocket* socket,
        /* [in] */ const String& abiList);

    /**
     * Returns the file descriptor of the associated socket.
     *
     * @return null-ok; file descriptor
     */
    CARAPI GetFileDescriptor(
        /* [out] */ IFileDescriptor** fd);

    /**
     * Reads one start command from the command socket. If successful,
     * a child is forked and a {@link ZygoteInit.MethodAndArgsCaller}
     * exception is thrown in that child while in the parent process,
     * the method returns normally. On failure, the child is not
     * spawned and messages are printed to the log and stderr. Returns
     * a boolean status value indicating whether an end-of-file on the command
     * socket has been encountered.
     *
     * @return false if command socket should continue to be read from, or
     * true if an end-of-file has been encountered.
     * @throws ZygoteInit.MethodAndArgsCaller trampoline to invoke main()
     * method in child process
     */
    CARAPI_(Boolean) RunOnce(
        /* [out] */ IRunnable** task);

    /**
     * Closes socket associated with this connection.
     */
    CARAPI_(void) CloseSocket();

    /**
     * Applies debugger system properties to the zygote arguments.
     *
     * If "ro.debuggable" is "1", all apps are debuggable. Otherwise,
     * the debugger state is specified via the "--enable-debugger" flag
     * in the spawn request.
     *
     * @param args non-null; zygote spawner args
     */
    static CARAPI_(void) ApplyDebuggerSystemProperty(
        /* [in] */ Arguments* args);

    /**
     * Applies invoke-with system properties to the zygote arguments.
     *
     * @param parsedArgs non-null; zygote args
     */
    static CARAPI_(void) ApplyInvokeWithSystemProperty(
        /* [in] */ Arguments* args);

private:
    /**
     * Applies zygote security policy per bugs #875058 and #1082165.
     * Based on the credentials of the process issuing a zygote command:
     * <ol>
     * <li> uid 0 (root) may specify any uid, gid, and setgroups() list
     * <li> uid 1000 (Process.SYSTEM_UID) may specify any uid &gt; 1000 in normal
     * operation. It may also specify any gid and setgroups() list it chooses.
     * In factory test mode, it may specify any UID.
     * <li> Any other uid may not specify any uid, gid, or setgroups list. The
     * uid and gid will be inherited from the requesting process.
     * </ul>
     *
     * @param args non-null; zygote spawner arguments
     * @param peer non-null; peer credentials
     * @throws ZygoteSecurityException
     */
    static CARAPI ApplyUidSecurityPolicy(
        /* [in] */ Arguments* args,
        /* [in] */ ICredentials* peer,
        /* [in] */ const String& peerSecurityContext);

    /**
     * Applies zygote security policy per bug #1042973. Based on the credentials
     * of the process issuing a zygote command:
     * <ol>
     * <li> peers of  uid 0 (root) and uid 1000 (Process.SYSTEM_UID)
     * may specify any rlimits.
     * <li> All other uids may not specify rlimits.
     * </ul>
     * @param args non-null; zygote spawner arguments
     * @param peer non-null; peer credentials
     * @throws ZygoteSecurityException
     */
    static CARAPI ApplyRlimitSecurityPolicy(
        /* [in] */ Arguments* args,
        /* [in] */ ICredentials* peer,
        /* [in] */ const String& peerSecurityContext);

    /**
     * Applies zygote security policy.
     * Based on the credentials of the process issuing a zygote command:
     * <ol>
     * <li> uid 0 (root) may specify --invoke-with to launch Zygote with a
     * wrapper command.
     * <li> Any other uid may not specify any invoke-with argument.
     * </ul>
     *
     * @param args non-null; zygote spawner arguments
     * @param peer non-null; peer credentials
     * @throws ZygoteSecurityException
     */
    static CARAPI ApplyInvokeWithSecurityPolicy(
        /* [in] */ Arguments* args,
        /* [in] */ ICredentials* peer,
        /* [in] */ const String& peerSecurityContext);

    /**
     * Applies zygote security policy for SELinux information.
     *
     * @param args non-null; zygote spawner arguments
     * @param peer non-null; peer credentials
     * @throws ZygoteSecurityException
     */
    static CARAPI ApplyseInfoSecurityPolicy(
        /* [in] */ Arguments* args,
        /* [in] */ ICredentials* peer,
        /* [in] */ const String& peerSecurityContext);

    /**
     * Logs an error message and prints it to the specified stream, if
     * provided
     *
     * @param newStderr null-ok; a standard error stream
     * @param message non-null; error message
     * @param ex null-ok an exception
     */
    static void LogAndPrintError (
        /* [in] */ IPrintStream* newStderr,
        /* [in] */ const String& message);

    /**
     * Temporary hack: check time since start time and log if over a fixed threshold.
     *
     */
    CARAPI_(void) CheckTime(
        /* [in] */ Int64 startTime,
        /* [in] */ const String& where);

    CARAPI_(Boolean) HandleAbiListQuery();

    /**
     * Reads an argument list from the command socket/
     * @return Argument list or null if EOF is reached
     * @throws IOException passed straight through
     */
    CARAPI ReadArgumentList(
        /* [out] */ ArrayOf<String>** args);

    /**
     * Handles post-fork setup of child proc, closing sockets as appropriate,
     * reopen stdio as appropriate, and ultimately throwing MethodAndArgsCaller
     * if successful or returning if failed.
     *
     * @param parsedArgs non-null; zygote args
     * @param descriptors null-ok; new file descriptors for stdio if available.
     * @param pipeFd null-ok; pipe for communication back to Zygote.
     * @param newStderr null-ok; stream to use for stderr until stdio
     * is reopened.
     *
     * @throws ZygoteInit.MethodAndArgsCaller on success to
     * trampoline to code that invokes static main.
     */
    CARAPI HandleChildProc(
        /* [in] */ Arguments* parsedArgs,
        /* [in] */ ArrayOf<IFileDescriptor*>* descriptors,
        /* [in] */ IFileDescriptor* pipeFd,
        /* [in] */ IPrintStream* newStderr,
        /* [out] */ IRunnable** task);

    /**
     * Handles post-fork cleanup of parent proc
     *
     * @param pid != 0; pid of child if &gt; 0 or indication of failed fork
     * if &lt; 0;
     * @param descriptors null-ok; file descriptors for child's new stdio if
     * specified.
     * @param pipeFd null-ok; pipe for communication with child.
     * @param parsedArgs non-null; zygote args
     * @return true for "exit command loop" and false for "continue command
     * loop"
     */
    CARAPI_(Boolean) HandleParentProc(
        /* [in] */ Int32 pid,
        /* [in] */ ArrayOf<IFileDescriptor*>* descriptors,
        /* [in] */ IFileDescriptor* pipeFd,
        /* [in] */ Arguments* parsedArgs);

    CARAPI_(void) SetChildPgid(
        /* [in] */ Int32 pid);

private:
    static const String TAG;

    /**
     * {@link android.net.LocalSocket#setSoTimeout} value for connections.
     * Effectively, the amount of time a requestor has between the start of
     * the request and the completed request. The select-loop mode Zygote
     * doesn't have the logic to return to the select loop in the middle of
     * a request, so we need to time out here to avoid being denial-of-serviced.
     */
    static const Int32 CONNECTION_TIMEOUT_MILLIS = 1000;

    /** max number of arguments that a connection can specify */
    static const Int32 MAX_ZYGOTE_ARGC = 1024;

    /**
     * The command socket.
     *
     * mSocket is retained in the child process in "peer wait" mode, so
     * that it closes when the child process terminates. In other cases,
     * it is closed in the peer.
     */
    AutoPtr<ILocalSocket> mSocket;
    AutoPtr<IDataOutputStream> mSocketOutStream;
    AutoPtr<IBufferedReader> mSocketReader;
    AutoPtr<ICredentials> mPeer;
    String mPeerSecurityContext;
    String mAbiList;

    /**
     * A long-lived reference to the original command socket used to launch
     * this peer. If "peer wait" mode is specified, the process that requested
     * the new VM instance intends to track the lifetime of the spawned instance
     * via the command socket. In this case, the command socket is closed
     * in the Zygote and placed here in the spawned instance so that it will
     * not be collected and finalized. This field remains null at all times
     * in the original Zygote process, and in all spawned processes where
     * "peer-wait" mode was not requested.
     */
    static AutoPtr<ILocalSocket> sPeerWaitSocket;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_ZYGOTECONNECTION_H__
