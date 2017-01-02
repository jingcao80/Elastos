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

#ifndef __ELASTOS_DROID_SERVER_CDROPBOXMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CDROPBOXMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_CDropBoxManagerService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/Set.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Internal.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::IDropBoxManagerEntry;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IStatFs;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Internal::Os::IIDropBoxManagerService;
using Elastos::Core::Thread;
using Elastos::Core::IComparable;
using Elastos::IO::IFile;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::Set;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CDropBoxManagerService)
    , public Object
    , public IIDropBoxManagerService
    , public IBinder
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CDropBoxManagerService::MyHandler")

        MyHandler(
            /* [in] */ CDropBoxManagerService* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CDropBoxManagerService* mHost;
    };

    /** Metadata describing an on-disk log file. */
    class EntryFile
        : public Object
    {
    public:
        EntryFile();

        /**
         * Moves an existing temporary file to a new log filename.
         * @param temp file to rename
         * @param dir to store file in
         * @param tag to use for new log file name
         * @param timestampMillis of log entry
         * @param flags for the entry data
         * @param blockSize to use for space accounting
         * @throws IOException if the file can't be moved
         */
        CARAPI Init(
            /* [in] */ IFile* temp,
            /* [in] */ IFile* dir,
            /* [in] */ const String& tag,
            /* [in] */ Int64 timestampMillis,
            /* [in] */ Int32 flags,
            /* [in] */ Int32 blockSize);

        /**
         * Creates a zero-length tombstone for a file whose contents were lost.
         * @param dir to store file in
         * @param tag to use for new log file name
         * @param timestampMillis of log entry
         * @throws IOException if the file can't be created.
         */
        CARAPI Init(
            /* [in] */ IFile* dir,
            /* [in] */ const String& tag,
            /* [in] */ Int64 timestampMillis);

        /**
         * Extracts metadata from an existing on-disk log filename.
         * @param file name of existing log file
         * @param blockSize to use for space accounting
         */
        CARAPI Init(
            /* [in] */ IFile* file,
            /* [in] */ Int32 blockSize);

        /**
         * Creates a EntryFile object with only a timestamp for comparison purposes.
         * @param timestampMillis to compare with.
         */
        CARAPI Init(
            Int64 millis);

        /** Sorts earlier EntryFile instances before later ones. */
        CARAPI_(Int32) CompareTo(
            /* [in] */ EntryFile* another);

    public:
        String mTag;
        Int64 mTimestampMillis;
        Int32 mFlags;
        AutoPtr<IFile> mFile;
        Int32 mBlocks;
    };

    /** Chronologically sorted list of {@link #EntryFile} */
    class FileList
        : public Object
    {
    public:
        FileList() : mBlocks(0) {}

        /* Sorts bigger FileList instances before smaller ones.*/
        CARAPI_(Int32) CompareTo(
            /* [in] */ FileList* another);

    public:
        Int32 mBlocks;
        // TODO: should be a sorted set
        Set< AutoPtr<EntryFile> > mContents;
    };

    /** Receives events that might indicate a need to clean up files. */
    class CleanupBroadcastReceiver : public BroadcastReceiver
    {
    public:
        CleanupBroadcastReceiver(
            /* [in] */ CDropBoxManagerService* owner) : mHost(owner)
        {}

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CDropBoxManagerService::CleanupBroadcastReceiver: ")
    private:
        class MyThread
            : public Thread
        {
        public:
            MyThread(
                /* [in] */ CDropBoxManagerService* owner);

            CARAPI Run();

        private:
            CDropBoxManagerService* mHost;
        };

    public:
        CDropBoxManagerService* mHost;

    };

    class MyContentObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CDropBoxManagerService::MyContentObserver")

        CARAPI constructor(
            /* [in] */ CDropBoxManagerService* owner,
            /* [in] */ IHandler* handler,
            /* [in] */ IContext* context);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        CDropBoxManagerService* mHost;
        AutoPtr<IContext> mContext;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDropBoxManagerService();

    ~CDropBoxManagerService();

    /**
     * Creates an instance of managed drop box storage.  Normally there is one of these
     * run by the system, but others can be created for testing and other purposes.
     *
     * @param context to use for receiving free space & gservices intents
     * @param path to store drop box entries in
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IFile* path);

    /** Unregisters broadcast receivers and any other hooks -- for test instances */
    CARAPI Stop();

    //@Override
    CARAPI Add(
        /* [in] */ IDropBoxManagerEntry* entry);

    CARAPI IsTagEnabled(
        /* [in] */ const String& tag,
        /* [out] */ Boolean* isTagEnabled);

    CARAPI GetNextEntry(
        /* [in] */ const String& tag,
        /* [in] */ Int64 millis,
        /* [out] */ IDropBoxManagerEntry** nextEntry);

    // public synchronized void dump(FileDescriptor fd, PrintWriter pw, String[] args)

    CARAPI ToString(
        /* [out] */ String* result);

private:
    ///////////////////////////////////////////////////////////////////////////

    /** If never run before, scans disk contents to build in-memory tracking data. */
    CARAPI Init();

    /** Adds a disk log file to in-memory tracking for accounting and enumeration. */
    CARAPI EnrollEntry(
        /* [in] */ EntryFile* entry);

    /** Moves a temporary file to a final log filename and enrolls it. */
    CARAPI CreateEntry(
        /* [in] */ IFile* temp,
        /* [in] */ const String& tag,
        /* [in] */ Int32 flags,
        /* [out] */ Int64* value);

    /**
     * Trims the files on disk to make sure they aren't using too much space.
     * @return the overall quota for storage (in bytes)
     */
    CARAPI_(Int64) TrimToFit();

    CARAPI HandleMessage(
        /* [in] */ IIntent* obj,
        /* [in] */ Int32 what);

private:
    const static String TAG;
    const static Int32 DEFAULT_AGE_SECONDS;
    const static Int32 DEFAULT_MAX_FILES;
    const static Int32 DEFAULT_QUOTA_KB;
    const static Int32 DEFAULT_QUOTA_PERCENT;
    const static Int32 DEFAULT_RESERVE_PERCENT;
    const static Int32 QUOTA_RESCAN_MILLIS;

    // mHandler 'what' value.
    const static Int32 MSG_SEND_BROADCAST;

    const static Boolean PROFILE_DUMP;

    // TODO: This implementation currently uses one file per entry, which is
    // inefficient for smallish entries -- consider using a single queue file
    // per tag (or even globally) instead.

    // The cached context and derived objects

    AutoPtr<IContext> mContext;
    AutoPtr<IContentResolver> mContentResolver;
    AutoPtr<IFile> mDropBoxDir;

    // Accounting of all currently written log files (set in init()).

    AutoPtr<FileList> mAllFiles;
    AutoPtr< HashMap<String, AutoPtr<FileList> > > mFilesByTag;

    // Various bits of disk information

    AutoPtr<IStatFs> mStatFs;
    Int32 mBlockSize;
    Int32 mCachedQuotaBlocks;  // Space we can use: computed from free space, etc.
    Int64 mCachedQuotaUptimeMillis;

    Boolean mBooted;

    // Provide a way to perform sendBroadcast asynchronously to avoid deadlocks.
    AutoPtr<IHandler> mHandler;

    AutoPtr<IBroadcastReceiver> mReceiver;
};

} //namespace Server
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CDROPBOXMANAGERSERVICE_H__
