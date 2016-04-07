#include "elastos/droid/server/CDropBoxManagerService.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Zip.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IDropBoxManager;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::CStatFs;
using Elastos::Droid::Os::UserHandle;
// using Elastos::Droid::Os::CDropBoxManagerEntry;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Internal::Os::EIID_IIDropBoxManagerService;

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IComparable;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IFlushable;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::Utility::Zip::IGZIPOutputStream;
using Elastos::Utility::Zip::CGZIPOutputStream;
using Elastos::Utility::Logging::Slogger;

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::CDropBoxManagerService::EntryFile, IInterface)

namespace Elastos {
namespace Droid {
namespace Server {

const String CDropBoxManagerService::TAG("DropBoxManagerService");
const Int32 CDropBoxManagerService::DEFAULT_AGE_SECONDS = 3 * 86400;
const Int32 CDropBoxManagerService::DEFAULT_MAX_FILES = 1000;
const Int32 CDropBoxManagerService::DEFAULT_QUOTA_KB = 5 * 1024;
const Int32 CDropBoxManagerService::DEFAULT_QUOTA_PERCENT = 10;
const Int32 CDropBoxManagerService::DEFAULT_RESERVE_PERCENT = 10;
const Int32 CDropBoxManagerService::QUOTA_RESCAN_MILLIS = 5000;
const Int32 CDropBoxManagerService::MSG_SEND_BROADCAST = 1;
const Boolean CDropBoxManagerService::PROFILE_DUMP = FALSE;

ECode CDropBoxManagerService::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case CDropBoxManagerService::MSG_SEND_BROADCAST:
            // mContext.sendBroadcastAsUser((Intent)msg.obj, UserHandle.OWNER,
            //     android.Manifest.permission.READ_LOGS);
        break;
    }

    return NOERROR;
}

//====================================================================
// CDropBoxManagerService::FileList
//====================================================================
Int32 CDropBoxManagerService::FileList::CompareTo(
    /* [in] */ FileList* another)
{
    AutoPtr<FileList> o = another;
    if (mBlocks != o->mBlocks) {
        return o->mBlocks - mBlocks;
    }
    if (this == o) {
        return 0;
    }
    // TODO:
    //if (hashCode() < o.hashCode()) return -1;
    //if (hashCode() > o.hashCode()) return 1;
    return 0;
}


//====================================================================
// CDropBoxManagerService::EntryFile
//====================================================================
CDropBoxManagerService::EntryFile::EntryFile()
    : mTimestampMillis(0)
    , mFlags(0)
    , mBlocks(0)
{
}

CDropBoxManagerService::EntryFile::Init(
    /* [in] */ IFile* temp,
    /* [in] */ IFile* dir,
    /* [in] */ const String& tag,
    /* [in] */ Int64 timestampMillis,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 blockSize)
{
    if ((flags & IDropBoxManager::IS_EMPTY) != 0) {
        //throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mTag = tag;
    mTimestampMillis = timestampMillis;
    mFlags = flags;

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    String encoded;
    helper->Encode(tag, &encoded);
    StringBuilder sb(encoded);
    sb += "@";
    sb += timestampMillis;
    sb += (flags & IDropBoxManager::IS_TEXT) != 0 ? ".txt" : ".dat";
    sb += (flags & IDropBoxManager::IS_GZIPPED) != 0 ? ".gz" : "";
    CFile::New(dir, sb.ToString(), (IFile**)&mFile);

    Boolean result;
    temp->RenameTo(this->mFile, &result);
    if (!result) {
        //throw new IOException("Can't rename " + temp + " to " + this.file);
        Slogger::E("CDropBoxManagerService::EntryFile", "Can't rename to %s, tag: %s",
          sb.ToString().string(), tag.string());
        return E_IO_EXCEPTION;
    }
    Int64 length;
    mFile->GetLength(&length);
    mBlocks = (Int32)((length + blockSize - 1) / blockSize);
    return NOERROR;
}

CDropBoxManagerService::EntryFile::Init(
    /* [in] */ IFile* dir,
    /* [in] */ const String& tag,
    /* [in] */ Int64 timestampMillis)
{
    mTag = tag;
    mTimestampMillis = timestampMillis;
    mFlags = IDropBoxManager::IS_EMPTY;
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    String encoded;
    helper->Encode(tag, &encoded);
    StringBuilder sb(encoded);
    sb += "@";
    sb += timestampMillis;
    sb += ".lost";
    CFile::New(dir, sb.ToString(), (IFile**)&mFile);
    mBlocks = 0;
    AutoPtr<ICloseable> fos;
    CFileOutputStream::New(mFile, (ICloseable**)&fos);
    fos->Close();
    return NOERROR;
}

CDropBoxManagerService::EntryFile::Init(
    /* [in] */ IFile* file,
    /* [in] */ Int32 blockSize)
{
    mFile = file;
    Int64 length;
    mFile->GetLength(&length);
    mBlocks = (Int32)((length + blockSize - 1) / blockSize);

    String name;
    file->GetName(&name);
    Int32 at = name.LastIndexOf('@');
    if (at < 0) {
        mTag = NULL;
        mTimestampMillis = 0;
        mFlags = IDropBoxManager::IS_EMPTY;
        return NOERROR;
    }

    Int32 flags = 0;
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    helper->Decode(name.Substring(0, at), &mTag);
    if (name.EndWith(".gz")) {
        flags |= IDropBoxManager::IS_GZIPPED;
        name = name.Substring(0, name.GetLength() - 3);
    }
    if (name.EndWith(".lost")) {
        flags |= IDropBoxManager::IS_EMPTY;
        name = name.Substring(at + 1, name.GetLength() - 5);
    }
    else if (name.EndWith(".txt")) {
        flags |= IDropBoxManager::IS_TEXT;
        name = name.Substring(at + 1, name.GetLength() - 4);
    }
    else if (name.EndWith(".dat")) {
        name = name.Substring(at + 1, name.GetLength() - 4);
    }
    else {
        mFlags = IDropBoxManager::IS_EMPTY;
        mTimestampMillis = 0;
        return NOERROR;
    }
    mFlags = flags;

    Int64 millis;
    //try {
    ECode ec = StringUtils::Parse(name, &millis);
    if (FAILED(ec)) millis = 0;
    //} catch (NumberFormatException e) { millis = 0; }
    mTimestampMillis = millis;
    return NOERROR;
}

/**
 * Creates a EntryFile object with only a timestamp for comparison purposes.
 * @param timestampMillis to compare with.
 */
CDropBoxManagerService::EntryFile::Init(
    /* [in] */ Int64 millis)
{
    mTag = NULL;
    mTimestampMillis = millis;
    mFlags = IDropBoxManager::IS_EMPTY;
    mFile = NULL;
    mBlocks = 0;
    return NOERROR;
}

Int32 CDropBoxManagerService::EntryFile::CompareTo(
    /* [in] */ EntryFile* another)
{
    AutoPtr<EntryFile> o = another;
    if (mTimestampMillis < o->mTimestampMillis) {
        return -1;
    }
    if (mTimestampMillis > o->mTimestampMillis) {
        return 1;
    }
    if (mFile != NULL && o->mFile != NULL) {
        // TODO:
        //IComparable::Probe(mFile)->CompareTo(o->mFile, result);
        return NOERROR;
    }
    if (o->mFile != NULL) {
        return -1;
    }
    if (mFile != NULL) {
        return 1;
    }
    if (this == o) {
        return 0;
    }
    // TODO:
    //if (hashCode() < o.hashCode()) return -1;
    //if (hashCode() > o.hashCode()) return 1;
    return 0;
}


//====================================================================
// CDropBoxManagerService::CleanupBroadcastReceiver::MyThread
//====================================================================
CDropBoxManagerService::CleanupBroadcastReceiver::MyThread::MyThread(
    /* [in] */ CDropBoxManagerService* owner)
    : mHost(owner)
{
}

ECode CDropBoxManagerService::CleanupBroadcastReceiver::MyThread::Run()
{
   mHost->Init();
   mHost->TrimToFit();
   return NOERROR;
}


//====================================================================
// CDropBoxManagerService::CleanupBroadcastReceiver
//====================================================================
ECode CDropBoxManagerService::CleanupBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    if (intent != NULL && (intent->GetAction(&action), IIntent::ACTION_BOOT_COMPLETED.Equals(action))) {
        mHost->mBooted = TRUE;
        return NOERROR;
    }

    // Else, for ACTION_DEVICE_STORAGE_LOW:
    mHost->mCachedQuotaUptimeMillis = 0;  // Force a re-check of quota size

    // Run the initialization in the background (not this main thread).
    // The init() and trimToFit() methods are synchronized, so they still
    // block other users -- but at least the onReceive() call can finish.
    AutoPtr<MyThread> thread = new MyThread(mHost);
    thread->constructor();

    thread->Start();
    return NOERROR;
}

//====================================================================
// CDropBoxManagerService::MyContentObserver
//====================================================================
ECode CDropBoxManagerService::MyContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    return mHost->mReceiver->OnReceive(mContext, NULL);
}

//====================================================================
// CDropBoxManagerService
//====================================================================
CAR_INTERFACE_IMPL_2(CDropBoxManagerService, Object, IIDropBoxManagerService, IBinder)

CAR_OBJECT_IMPL(CDropBoxManagerService)

CDropBoxManagerService::CDropBoxManagerService()
    : mBlockSize(0)
    , mCachedQuotaBlocks(0)
    , mCachedQuotaUptimeMillis(0)
    , mBooted(FALSE)
{
}

CDropBoxManagerService::~CDropBoxManagerService()
{
    mFilesByTag = NULL;
}

ECode CDropBoxManagerService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IFile* path)
{
    mDropBoxDir = path;

    mReceiver = new CleanupBroadcastReceiver(this);

    // Set up intent receivers
    mContext = context;
    context->GetContentResolver((IContentResolver**)&mContentResolver);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_DEVICE_STORAGE_LOW);
    filter->AddAction(IIntent::ACTION_BOOT_COMPLETED);
    AutoPtr<IIntent> result;
    context->RegisterReceiver(mReceiver, filter, (IIntent**)&result);

    AutoPtr<MyContentObserver> observer = new MyContentObserver(this, context);
    AutoPtr<ISettingsGlobal> settings;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settings);
    AutoPtr<IUri> uri;
    settings->GetCONTENT_URI((IUri**)&uri);
    mContentResolver->RegisterContentObserver(uri, TRUE, observer);

    mHandler = new MyHandler(this);
    return NOERROR;
}

ECode CDropBoxManagerService::Stop()
{
    return mContext->UnregisterReceiver(mReceiver);
}

ECode CDropBoxManagerService::Add(
    /* [in] */ IDropBoxManagerEntry* entry)
{
    ECode ec;
    AutoPtr<IFile> temp;
    AutoPtr<IOutputStream> output;
    Boolean isTagEnabled;
    Int64 max;
    Int64 lastTrim;
    AutoPtr<ArrayOf<Byte> > buffer;
    AutoPtr<IInputStream> input;
    Int32 read;
    String tag;
    entry->GetTag(&tag);
    //try {
    Int32 flags;
    entry->GetFlags(&flags);
    AutoPtr<ISystem> system;
    if ((flags & IDropBoxManager::IS_EMPTY) != 0) {
       ec = E_ILLEGAL_ARGUMENT_EXCEPTION;
       goto EXIT;
    }

    FAIL_GOTO(Init(), EXIT);
    IsTagEnabled(tag, &isTagEnabled);
    if (!isTagEnabled) {
       ec = NOERROR;
       goto EXIT;
    }
    max = TrimToFit();
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&lastTrim);

    buffer = ArrayOf<Byte>::Alloc(mBlockSize);
    entry->GetInputStream((IInputStream**)&input);

    // First, accumulate up to one block worth of data in memory before
    // deciding whether to compress the data or not.

    read = 0;
    while (read < buffer->GetLength()) {
       Int32 n;
       FAIL_GOTO(input->Read(buffer, read, buffer->GetLength() - read, &n), EXIT);
       if (n <= 0) {
           break;
       }
       read += n;
    }

    {
       // If we have at least one block, compress it -- otherwise, just write
       // the data in uncompressed form.
       Int64 id = 0;
       Thread::GetCurrentThread()->GetId(&id);
       StringBuilder sb("drop");
       sb += id;
       sb += ".tmp";
       CFile::New(mDropBoxDir, sb.ToString(), (IFile**)&temp);
       Int32 bufferSize = mBlockSize;
       if (bufferSize > 4096) {
           bufferSize = 4096;
       }
       if (bufferSize < 512) {
           bufferSize = 512;
       }
       AutoPtr<IFileOutputStream> foutput;
       CFileOutputStream::New(temp, (IFileOutputStream**)&foutput);
       CBufferedOutputStream::New(IOutputStream::Probe(foutput), bufferSize, (IOutputStream**)&output);
       if (read == buffer->GetLength() && ((flags & IDropBoxManager::IS_GZIPPED) == 0)) {
           AutoPtr<IOutputStream> goutput;
           CGZIPOutputStream::New(output, (IOutputStream**)&goutput);
           output = goutput;
           flags = flags | IDropBoxManager::IS_GZIPPED;
       }

       do {
           FAIL_GOTO(output->Write(buffer, 0, read), EXIT);

           Int64 now;
           system->GetCurrentTimeMillis(&now);
           if (now - lastTrim > 30 * 1000) {
               max = TrimToFit();  // In case data dribbles in slowly
               lastTrim = now;
           }

           FAIL_GOTO(input->Read(buffer, &read), EXIT);
           if (read <= 0) {
               Boolean result = FileUtils::Sync(foutput);
               ICloseable::Probe(output)->Close();  // Get a final size measurement
               output = NULL;
           }
           else {
               IFlushable::Probe(output)->Flush();// So the size measurement is pseudo-reasonable
           }

           Int64 len;
           temp->GetLength(&len);
           if (len > max) {
               StringBuilder sb("Dropping: ");
               sb += tag;
               sb += " (";
               sb += len;
               sb += " > ";
               sb += max;
               sb += " bytes)";
               Slogger::W(TAG, sb.ToString());
               Boolean isDelete;
               temp->Delete(&isDelete);
               temp = NULL;  // Pass temp = null to createEntry() to leave a tombstone
               break;
           }
       } while (read > 0);

       Int64 time;
       CreateEntry(temp, tag, flags, &time);
       temp = NULL;

       const AutoPtr<IIntent> dropboxIntent;
       CIntent::New(IDropBoxManager::ACTION_DROPBOX_ENTRY_ADDED, (IIntent**)&dropboxIntent);
       dropboxIntent->PutExtra(IDropBoxManager::EXTRA_TAG, tag);
       dropboxIntent->PutExtra(IDropBoxManager::EXTRA_TIME, time);
       if (!mBooted) {
           dropboxIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
       }
       // Call sendBroadcast after returning from this call to avoid deadlock. In particular
       // the caller may be holding the WindowManagerService lock but sendBroadcast requires a
       // lock in ActivityManagerService. ActivityManagerService has been caught holding that
       // very lock while waiting for the WindowManagerService lock.
       AutoPtr<IMessage> msg;
       Boolean rst;
       mHandler->ObtainMessage(MSG_SEND_BROADCAST, dropboxIntent, (IMessage**)&msg);
       mHandler->SendMessage(msg, &rst);
    }
    //} catch (IOException e) {
    //    Slog.e(TAG, "Can't write: " + tag, e);
    //} finally {
       //try {
    EXIT:
    if (output != NULL) {
       output->Close();
    }
       //} catch (IOException e) {}
    ICloseable::Probe(entry)->Close();
    if (temp != NULL) {
       Boolean isDelete;
       temp->Delete(&isDelete);
    }
    //}
    return ec;
}

ECode CDropBoxManagerService::IsTagEnabled(
    /* [in] */ const String& tag,
    /* [out] */ Boolean* isTagEnabled)
{
    VALIDATE_NOT_NULL(isTagEnabled);

    StringBuilder sb(ISettingsGlobal::DROPBOX_TAG_PREFIX);
    sb += tag;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    String value;
    settingsGlobal->GetString(mContentResolver, sb.ToString(), &value);
    *isTagEnabled = !value.Equals("disabled");
    return NOERROR;
}

ECode CDropBoxManagerService::GetNextEntry(
    /* [in] */ const String& tag,
    /* [in] */ Int64 millis,
    /* [out] */ IDropBoxManagerEntry** nextEntry)
{
    VALIDATE_NOT_NULL(nextEntry);
    *nextEntry = NULL;

    AutoLock lock(this);
    Int32 result;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::READ_LOGS, &result));
    if (result != IPackageManager::PERMISSION_GRANTED) {
        Slogger::E(TAG, String("READ_LOGS permission required"));
        return E_SECURITY_EXCEPTION;
    }

    //try {
    ECode ec = Init();
    if (FAILED(ec)) {
        return NOERROR;
    }
    //} catch (IOException e) {
    //    Slog.e(TAG, "Can't init", e);
    //    return null;
    //}

    AutoPtr<FileList> list;
    if (tag == NULL) {
        list = mAllFiles;
    }
    else {
        HashMap<String, AutoPtr<FileList> >::Iterator it = mFilesByTag->Find(tag);
        if (it != mFilesByTag->End()) list = it->mSecond;
    }
    if (list == NULL) {
        return NOERROR;
    }

    // TODO: list.contents.tailSet(new EntryFile(millis + 1))
    AutoPtr<EntryFile> compEntry = new EntryFile();
    FAIL_RETURN(compEntry->Init(millis + 1))

    Set<AutoPtr<EntryFile> >::Iterator iter;
    for (iter = list->mContents.Begin(); iter != list->mContents.End(); ++iter) {
        AutoPtr<EntryFile> entry = *iter;
        if (entry->CompareTo(compEntry) < 0) continue;
        if (entry->mTag.IsNull()) {
            continue;
        }

        if ((entry->mFlags & IDropBoxManager::IS_EMPTY) != 0) {
        assert(0 && "TODO");
            // return CDropBoxManagerEntry::New(entry->mTag, entry->mTimestampMillis, nextEntry);
        }
        //try {
        assert(0 && "TODO");
        // return CDropBoxManagerEntry::New(entry->mTag, entry->mTimestampMillis,
                // entry->mFile, entry->mFlags, nextEntry);
        //} catch (IOException e) {
        //    Slog.e(TAG, "Can't read: " + entry.file, e);
        //    // Continue to next file
        //}
    }

    return NOERROR;
}

ECode CDropBoxManagerService::Init()
{
    AutoLock lock(this);
    if (mStatFs == NULL) {
        Boolean isDirectory;
        Boolean succeed;
        if ((mDropBoxDir->IsDirectory(&isDirectory), !isDirectory) &&
                (mDropBoxDir->Mkdirs(&succeed), !succeed)) {
            //throw new IOException("Can't mkdir: " + mDropBoxDir);
            return E_IO_EXCEPTION;
        }
        //try {
        String path;
        mDropBoxDir->GetPath(&path);
        ECode ec = CStatFs::New(path, (IStatFs**)&mStatFs);
        if (FAILED(ec)) {
            return E_IO_EXCEPTION;
        }
        mStatFs->GetBlockSize(&mBlockSize);
        //} catch (IllegalArgumentException e) {  // StatFs throws this on error
        //    throw new IOException("Can't statfs: " + mDropBoxDir);
        //}
    }

    if (mAllFiles == NULL) {
        AutoPtr<ArrayOf<IFile*> > files;
        mDropBoxDir->ListFiles((ArrayOf<IFile*>**)(&files));
        if (files == NULL) {
            //throw new IOException("Can't list files: " + mDropBoxDir);
            return E_IO_EXCEPTION;
        }

        mAllFiles = new FileList();
        mFilesByTag = new HashMap<String, AutoPtr<FileList> >();

        // Scan pre-existing files.
        for (Int32 i = 0; i < files->GetLength(); ++i) {
            AutoPtr<IFile> file = (*files)[i];
            String name;
            file->GetName(&name);
            if (name.EndWith(String(".tmp"))) {
                //Slog.i(TAG, "Cleaning temp file: " + file);
                Boolean isDelete;
                file->Delete(&isDelete);
                continue;
            }

            AutoPtr<EntryFile> entry = new EntryFile();
            FAIL_RETURN(entry->Init(file, mBlockSize));
            if ((entry->mTag).IsNull()) {
                //Slog.w(TAG, "Unrecognized file: " + file);
                continue;
            }
            else if (entry->mTimestampMillis == 0) {
                //Slog.w(TAG, "Invalid filename: " + file);
                Boolean isDelete;
                file->Delete(&isDelete);
                continue;
            }

            EnrollEntry(entry);
        }
    }
    return NOERROR;
}

ECode CDropBoxManagerService::EnrollEntry(
    /* [in] */ EntryFile* entry)
{
    AutoLock lock(this);
    mAllFiles->mContents.Insert(entry);
    mAllFiles->mBlocks += entry->mBlocks;

    // mFilesByTag is used for trimming, so don't list empty files.
    // (Zero-length/lost files are trimmed by date from mAllFiles.)
    if (!entry->mTag.IsNull() && entry->mFile != NULL && entry->mBlocks > 0) {
        AutoPtr<FileList> tagFiles = (*mFilesByTag)[entry->mTag];
        if (tagFiles == NULL) {
            tagFiles = new FileList();
            (*mFilesByTag)[entry->mTag] = tagFiles;
        }
        tagFiles->mContents.Insert(entry);
        tagFiles->mBlocks += entry->mBlocks;
    }
    return NOERROR;
}

ECode CDropBoxManagerService::CreateEntry(
    /* [in] */ IFile* temp,
    /* [in] */ const String& tag,
    /* [in] */ Int32 flags,
    /* [out] */ Int64* value)
{
    AutoLock lock(this);
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 t;
    system->GetCurrentTimeMillis(&t);

    // Require each entry to have a unique timestamp; if there are entries
    // >10sec in the future (due to clock skew), drag them back to avoid
    // keeping them around forever.

    // TODO:
    //SortedSet<EntryFile> tail = mAllFiles.contents.tailSet(new EntryFile(t + 10000));
    AutoPtr<EntryFile> compEntry = new EntryFile();
    FAIL_RETURN(compEntry->Init(t + 10000));
    Set< AutoPtr<EntryFile> > tail;
    Set< AutoPtr<EntryFile> >::Iterator it;
    for (it = mAllFiles->mContents.Begin(); it != mAllFiles->mContents.End(); ++it) {
        AutoPtr<EntryFile> entry = *it;
        if (entry->CompareTo(compEntry) >= 0) {
            tail.Insert(entry);
        }
    }

    AutoPtr<ArrayOf<EntryFile*> > future;
    if (!tail.IsEmpty()) {
        future = ArrayOf<EntryFile*>::Alloc(tail.GetSize());
        Int32 i = 0;
        Set< AutoPtr<EntryFile> >::Iterator it;
        for (it = tail.Begin(); it != tail.End(); ++it, ++i) {
            future->Set(i, *it);
        }
        tail.Clear();  // Remove from mAllFiles
    }

    if (!mAllFiles->mContents.IsEmpty()) {
        // TODO: TreeSet is a sorted set
        t = Elastos::Core::Math::Max(t, (*mAllFiles->mContents.RBegin())->mTimestampMillis + 1);
    }

    if (future != NULL) {
        for (Int32 i = 0; i < future->GetLength(); ++i) {
            AutoPtr<EntryFile> late = (*future)[i];
            mAllFiles->mBlocks -= late->mBlocks;
            AutoPtr<FileList> tagFiles;
            HashMap<String, AutoPtr<FileList> >::Iterator it = mFilesByTag->Find(late->mTag);
            if (it != mFilesByTag->End()) tagFiles = it->mSecond;
            if (tagFiles != NULL && tagFiles->mContents.Erase(late) > 0) {
                tagFiles->mBlocks -= late->mBlocks;
            }
            if ((late->mFlags & IDropBoxManager::IS_EMPTY) == 0) {
                AutoPtr<EntryFile> ef = new EntryFile();
                FAIL_RETURN(ef->Init(late->mFile, mDropBoxDir, late->mTag, t++, late->mFlags, mBlockSize))
                EnrollEntry(ef);
            }
            else {
                AutoPtr<EntryFile> ef = new EntryFile();
                FAIL_RETURN(ef->Init(mDropBoxDir, late->mTag, t++));
                EnrollEntry(ef);
            }
        }
    }

    if (temp == NULL) {
        AutoPtr<EntryFile> entry = new EntryFile();
        FAIL_RETURN(entry->Init(mDropBoxDir, tag, t));
        EnrollEntry(entry);
    }
    else {
        AutoPtr<EntryFile> entry = new EntryFile();
        FAIL_RETURN(entry->Init(temp, mDropBoxDir, tag, t, flags, mBlockSize));
        EnrollEntry(entry);
    }
    *value = t;
    return NOERROR;
}

Int64 CDropBoxManagerService::TrimToFit()
{
    AutoLock lock(this);

    // Expunge aged items (including tombstones marking deleted data).
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    Int32 ageSeconds;
    settingsGlobal->GetInt32(mContentResolver,
            ISettingsGlobal::DROPBOX_AGE_SECONDS, DEFAULT_AGE_SECONDS, &ageSeconds);
    Int32 maxFiles;
    settingsGlobal->GetInt32(mContentResolver,
            ISettingsGlobal::DROPBOX_MAX_FILES, DEFAULT_MAX_FILES, &maxFiles);
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);
    Int64 cutoffMillis = millis - ageSeconds * 1000;
    while (!mAllFiles->mContents.IsEmpty()) {
        AutoPtr<EntryFile> entry = *(mAllFiles->mContents.Begin());
        if (entry->mTimestampMillis > cutoffMillis && mAllFiles->mContents.GetSize() < (UInt32)maxFiles) break;

        AutoPtr<FileList> tag;
        HashMap<String, AutoPtr<FileList> >::Iterator it = mFilesByTag->Find(entry->mTag);
        if (it != mFilesByTag->End()) tag = it->mSecond;
        if (tag != NULL && tag->mContents.Erase(entry) > 0) tag->mBlocks -= entry->mBlocks;
        if (mAllFiles->mContents.Erase(entry) > 0) mAllFiles->mBlocks -= entry->mBlocks;
        if (entry->mFile != NULL) {
            Boolean isDeleted;
            entry->mFile->Delete(&isDeleted);
        }
    }

    // Compute overall quota (a fraction of available free space) in blocks.
    // The quota changes dynamically based on the amount of free space;
    // that way when lots of data is available we can use it, but we'll get
    // out of the way if storage starts getting tight.

    Int64 uptimeMillis = SystemClock::GetUptimeMillis();
    if (uptimeMillis > mCachedQuotaUptimeMillis + QUOTA_RESCAN_MILLIS) {
        Int32 quotaPercent;
        settingsGlobal->GetInt32(mContentResolver,
                ISettingsGlobal::DROPBOX_QUOTA_PERCENT, DEFAULT_QUOTA_PERCENT, &quotaPercent);
        Int32 reservePercent;
        settingsGlobal->GetInt32(mContentResolver,
                ISettingsGlobal::DROPBOX_RESERVE_PERCENT, DEFAULT_RESERVE_PERCENT, &reservePercent);
        Int32 quotaKb;
        settingsGlobal->GetInt32(mContentResolver,
                ISettingsGlobal::DROPBOX_QUOTA_KB, DEFAULT_QUOTA_KB, &quotaKb);

        String path;
        mDropBoxDir->GetPath(&path);
        mStatFs->Restat(path);
        Int32 available;
        mStatFs->GetAvailableBlocks(&available);
        Int32 count;
        mStatFs->GetBlockCount(&count);
        Int32 nonreserved = available - count * reservePercent / 100;
        Int32 maximum = quotaKb * 1024 / mBlockSize;
        mCachedQuotaBlocks = Elastos::Core::Math::Min(maximum, Elastos::Core::Math::Max(0, nonreserved * quotaPercent / 100));
        mCachedQuotaUptimeMillis = uptimeMillis;
    }

    // If we're using too much space, delete old items to make room.
    //
    // We trim each tag independently (this is why we keep per-tag lists).
    // Space is "fairly" shared between tags -- they are all squeezed
    // equally until enough space is reclaimed.
    //
    // A single circular buffer (a la logcat) would be simpler, but this
    // way we can handle fat/bursty data (like 1MB+ bugreports, 300KB+
    // kernel crash dumps, and 100KB+ ANR reports) without swamping small,
    // well-behaved data streams (event statistics, profile data, etc).
    //
    // Deleted files are replaced with zero-length tombstones to mark what
    // was lost.  Tombstones are expunged by age (see above).

    if (mAllFiles->mBlocks > mCachedQuotaBlocks) {
        // Find a fair share amount of space to limit each tag
        Int32 unsqueezed = mAllFiles->mBlocks, squeezed = 0;

        Set<AutoPtr<FileList> > tags;
        HashMap<String, AutoPtr<FileList> >::Iterator it;
        for (it = mFilesByTag->Begin(); it != mFilesByTag->End(); ++it) {
            tags.Insert(it->mSecond);
        }

        Set<AutoPtr<FileList> >::Iterator sit;
        for (sit = tags.Begin(); sit != tags.End(); ++sit) {
            AutoPtr<FileList> tag = *sit;
            if (squeezed > 0 && tag->mBlocks <= (mCachedQuotaBlocks - unsqueezed) / squeezed) {
                break;
            }
            unsqueezed -= tag->mBlocks;
            squeezed++;
        }
        Int32 tagQuota = (mCachedQuotaBlocks - unsqueezed) / squeezed;

        // Remove old items from each tag until it meets the per-tag quota.
        for (sit = tags.Begin(); sit != tags.End(); ++sit) {
            AutoPtr<FileList> tag = *sit;
            if (mAllFiles->mBlocks < mCachedQuotaBlocks) break;
            while (tag->mBlocks > tagQuota && !tag->mContents.IsEmpty()) {
                AutoPtr<EntryFile> entry = *tag->mContents.Begin();
                if (tag->mContents.Erase(entry) > 0) tag->mBlocks -= entry->mBlocks;
                if (mAllFiles->mContents.Erase(entry) > 0) mAllFiles->mBlocks -= entry->mBlocks;

                //try {
                if (entry->mFile != NULL) {
                    Boolean isDeleted;
                    (entry->mFile)->Delete(&isDeleted);
                }
                AutoPtr<EntryFile> entryFile = new EntryFile();
                FAIL_RETURN(entryFile->Init(mDropBoxDir, entry->mTag, entry->mTimestampMillis));
                EnrollEntry(entryFile);
                //} catch (IOException e) {
                //    Slog.e(TAG, "Can't write tombstone file", e);
                //}
            }
        }
    }
    return mCachedQuotaBlocks * mBlockSize;
}

ECode CDropBoxManagerService::HandleMessage(
    /* [in] */ IIntent* obj,
    /* [in] */ Int32 what)
{
    if (what == MSG_SEND_BROADCAST) {
        mContext->SendBroadcastAsUser(obj, UserHandle::OWNER, Elastos::Droid::Manifest::permission::READ_LOGS);
    }
    return NOERROR;
}

ECode CDropBoxManagerService::ToString(
    /* [in] */ String* result)
{
    return E_NOT_IMPLEMENTED;
}

} //namespace Server
} //namespace Droid
} //namespace Elastos
