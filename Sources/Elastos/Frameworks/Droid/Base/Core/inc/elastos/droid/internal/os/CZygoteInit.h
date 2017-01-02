//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_INTERNAL_OS_CZYGOTEINIT_H__
#define __ELASTOS_DROID_INTERNAL_OS_CZYGOTEINIT_H__

#include "_Elastos_Droid_Internal_Os_CZygoteInit.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/internal/os/ZygoteConnection.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Net::ILocalServerSocket;
using Elastos::Core::Runnable;
using Elastos::Core::Singleton;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CarClass(CZygoteInit)
    , public Singleton
    , public IZygoteInit
{
public:
    /**
     * Helper exception class which holds a method and arguments and
     * can call them. This is used as part of a trampoline to get rid of
     * the initial process setup stack frames.
     */
    class MethodAndArgsCaller
        : public Runnable
    {
    public:
        MethodAndArgsCaller(
            /* [in] */ IInterface* object,
            /* [in] */ IMethodInfo* method,
            /* [in] */ ArrayOf<String>* args);

        CARAPI Run();

    private:
        AutoPtr<IInterface> mObject;
        AutoPtr<IMethodInfo> mMethod;
        AutoPtr< ArrayOf<String> > mArgs;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Close and clean up zygote sockets. Called on shutdown and on the
     * child's exit path.
     */
    static CARAPI_(void) CloseServerSocket();

    /**
     * Return the server socket's underlying file descriptor, so that
     * ZygoteConnection can pass it to the native code for proper
     * closure after a child process is forked off.
     */
    static CARAPI_(AutoPtr<IFileDescriptor>) GetServerSocketFileDescriptor();

    /**
    * Creates a file descriptor from an int fd.
    */
    static CARAPI_(AutoPtr<IFileDescriptor>) CreateFileDescriptor(
        /* [in] */ Int32 fd);

    CARAPI Main(
        /* [in] */ const ArrayOf<String>& argv);

    /**
     * The Linux syscall "setreuid()"
     * @param ruid real uid
     * @param euid effective uid
     * @return 0 on success, non-zero errno on fail
     */
    static CARAPI_(Int32) Setreuid(
        /* [in] */ Int32 ruid,
        /* [in] */ Int32 euid);

    /**
     * The Linux syscall "setregid()"
     * @param rgid real gid
     * @param egid effective gid
     * @return 0 on success, non-zero errno on fail
     */
    static CARAPI_(Int32) Setregid(
        /* [in] */ Int32 rgid,
        /* [in] */ Int32 egid);

    /**
     * Invokes the linux syscall "setpgid"
     *
     * @param pid pid to change
     * @param pgid new process group of pid
     * @return 0 on success or non-zero errno on fail
     */
    static CARAPI_(Int32) Setpgid(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 pgid);

    /**
     * Invokes the linux syscall "getpgid"
     *
     * @param pid pid to query
     * @return pgid of pid in question
     * @throws IOException on error
     */
    static CARAPI Getpgid(
        /* [in] */ Int32 pid,
        /* [out] */ Int32* pgid);

    /**
     * Invokes the syscall dup2() to copy the specified descriptors into
     * stdin, stdout, and stderr. The existing stdio descriptors will be
     * closed and errors during close will be ignored. The specified
     * descriptors will also remain open at their original descriptor numbers,
     * so the caller may want to close the original descriptors.
     *
     * @param in new stdin
     * @param out new stdout
     * @param err new stderr
     * @throws IOException
     */
    static CARAPI ReopenStdio(
        /* [in] */ IFileDescriptor* in,
        /* [in] */ IFileDescriptor* out,
        /* [in] */ IFileDescriptor* err);

    /**
     * Toggles the close-on-exec flag for the specified file descriptor.
     *
     * @param fd non-null; file descriptor
     * @param flag desired close-on-exec flag state
     * @throws IOException
     */
    static CARAPI SetCloseOnExec(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Boolean flag);

    /**
     * Invokes select() on the provider array of file descriptors (selecting
     * for readability only). Array elements of null are ignored.
     *
     * @param fds non-null; array of readable file descriptors
     * @return index of descriptor that is now readable or -1 for empty array.
     * @throws IOException if an error occurs
     */
    static CARAPI SelectReadable(
        /* [in] */ ArrayOf<IFileDescriptor*>* fds,
        /* [out] */ Int32* index);

private:
    /**
     * Registers a server socket for zygote command connections
     *
     * @throws RuntimeException when open fails
     */
    static CARAPI RegisterZygoteSocket(
        /* [in] */ const String& socketName);

    /**
     * Waits for and accepts a single command connection. Throws
     * RuntimeException on failure.
     */
    static CARAPI AcceptCommandPeer(
        /* [in] */ const String& abiList,
        /* [out] */ ZygoteConnection** peer);

    /**
     * Finish remaining work for the newly forked system server process.
     */
    static CARAPI HandleSystemServerProcess(
        /* [in] */ ZygoteConnection::Arguments* parsedArgs,
        /* [out] */ IRunnable** task);

    /**
     * Prepare the arguments and fork for the system server process.
     */
    static CARAPI StartSystemServer(
        /* [in] */ const String& abiList,
        /* [in] */ const String& socketName,
        /* [out] */ IRunnable** task);

    /**
     * Return {@code true} if this device configuration has another zygote.
     *
     * We determine this by comparing the device ABI list with this zygotes
     * list. If this zygote supports all ABIs this device supports, there won't
     * be another zygote.
     */
    static CARAPI_(Boolean) HasSecondZygote(
        /* [in] */ const String& abiList);

    static CARAPI_(void) WaitForSecondaryZygote(
        /* [in] */ const String& socketName);

    /**
     * Runs the zygote process's select loop. Accepts new connections as
     * they happen, and reads commands from connections one spawn-request's
     * worth at a time.
     *
     * @throws MethodAndArgsCaller in a child process when a main() should
     * be executed.
     */
    static CARAPI RunSelectLoop(
        /* [in] */ const String& abiList,
        /* [in] */ IRunnable** task);

    static CARAPI_(void) Preload();

    static CARAPI_(void) PreloadOpenGL();

    /**
     * Load in commonly used resources, so they can be shared across
     * processes.
     *
     * These tend to be a few Kbytes, but are frequently in the 20-40K
     * range, and occasionally even larger.
     */
    static CARAPI_(void) PreloadResources();

    static CARAPI_(Int32) PreloadColorStateLists(
        /* [in] */ ITypedArray* ar);

    static CARAPI_(Int32) PreloadDrawables(
        /* [in] */ ITypedArray* ar);

    static CARAPI PosixCapabilitiesAsBits(
        /* [in] */ ArrayOf<Int32>* capabilities,
        /* [out] */ Int64* result);

public:
    /**
     * The number of times that the main Zygote loop
     * should run before calling gc() again.
     */
    static const Int32 GC_LOOP_COUNT = 10;

private:
    friend class WrapperInit;

    static const String TAG;

    static const String PROPERTY_DISABLE_OPENGL_PRELOADING;

    static const String ANDROID_SOCKET_PREFIX;

    static const Int32 LOG_BOOT_PROGRESS_PRELOAD_START;
    static const Int32 LOG_BOOT_PROGRESS_PRELOAD_END;

    /** when preloading, GC after allocating this many bytes */
    static const Int32 PRELOAD_GC_THRESHOLD;

    static const String ABI_LIST_ARG;

    static const String SOCKET_NAME_ARG;

    static AutoPtr<ILocalServerSocket> sServerSocket;

    /**
     * Used to pre-load resources.  We hold a global reference on it so it
     * never gets destroyed.
     */
    static AutoPtr<IResources> mResources;

    /**
     * The name of a resource file that contains classes to preload.
     */
    static const String PRELOADED_CLASSES;

    /** Controls whether we should preload resources during zygote init. */
    static const Boolean PRELOAD_RESOURCES;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_CZYGOTEINIT_H__
