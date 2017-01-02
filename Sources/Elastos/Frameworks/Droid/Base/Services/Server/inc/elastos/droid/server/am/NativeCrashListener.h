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

#ifndef __ELASTOS_DROID_SERVER_AM_NATIVECRASHLISTENER_H__
#define __ELASTOS_DROID_SERVER_AM_NATIVECRASHLISTENER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/core/Thread.h"

using Elastos::Core::Thread;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

/**
  * Set up a Unix domain socket that debuggerd will connect() to in
  * order to write a description of a native crash.  The crash info is
  * then parsed and forwarded to the ActivityManagerService's normal
  * crash handling code.
  *
  * Note that this component runs in a separate thread.
  */
class NativeCrashListener
    : public Thread
{
public:
    /*
      * Spin the actual work of handling a debuggerd crash report into a
      * separate thread so that the listener can go immediately back to
      * accepting incoming connections.
      */
    class NativeCrashReporter
        : public Thread
    {
    public:
        NativeCrashReporter(
            /* [in] */ ProcessRecord* app,
            /* [in] */ Int32 signal,
            /* [in] */ const String& report,
            /* [in] */ NativeCrashListener* owner);

        // @Override
        CARAPI Run();

    public:
        AutoPtr<ProcessRecord> mApp;
        Int32 mSignal;
        String mCrashReport;
        NativeCrashListener* mOwner;
    };

public:
    /*
      * Daemon thread that accept()s incoming domain socket connections from debuggerd
      * and processes the crash dump that is passed through.
      */
    NativeCrashListener(
        /* [in] */ CActivityManagerService* am);

    // @Override
    CARAPI Run();

    static CARAPI_(Int32) UnpackInt(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset);

    static CARAPI_(Int32) ReadExactly(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 numBytes);

    // Read the crash report from the debuggerd connection
    CARAPI ConsumeNativeCrashData(
        /* [in] */ IFileDescriptor* fd);

public:
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean MORE_DEBUG;
    // Must match the path defined in debuggerd.c.
    static const String DEBUGGERD_SOCKET_PATH;
    // Use a short timeout on socket operations and abandon the connection
    // on hard errors
    static const Int64 SOCKET_TIMEOUT_MILLIS = 2000;
    // 2 seconds
    AutoPtr<CActivityManagerService> mAm;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_NATIVECRASHLISTENER_H__

