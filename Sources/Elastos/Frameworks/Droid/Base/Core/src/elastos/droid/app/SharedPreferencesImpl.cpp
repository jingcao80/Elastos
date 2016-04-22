
#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/app/SharedPreferencesImpl.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/app/CContextImpl.h"
#include "elastos/droid/app/QueuedWork.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/droid/system/Os.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::App::CActivityThread;
using Elastos::Droid::App::QueuedWork;
using Elastos::Droid::Content::EIID_ISharedPreferences;
using Elastos::Droid::Content::EIID_ISharedPreferencesEditor;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Internal::Utility::XmlUtils;

using Elastos::Core::CBoolean;
using Elastos::Core::CObject;
using Elastos::Core::CFloat;
using Elastos::Core::CInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::CString;
using Elastos::Core::IBoolean;
using Elastos::Core::IFloat;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::IBlockGuard;
using Elastos::Core::CBlockGuard;
using Elastos::Core::IBlockGuardPolicy;
using Elastos::Core::CThread;
using Elastos::Utility::Concurrent::IExecutor;
using Elastos::Utility::Concurrent::IExecutorService;
using Elastos::Utility::Concurrent::CCountDownLatch;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IHashSet;
using Elastos::Utility::ICollection;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CWeakHashMap;
using Elastos::IO::ICloseable;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::ICloseable;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;
using Libcore::IO::CLibcore;
using Libcore::IO::ILibcore;
using Libcore::IO::IOs;
using Elastos::Droid::System::IStructStat;
using Elastos::Droid::System::CStructStat;


namespace Elastos {
namespace Droid {
namespace App {


//==============================================================================
// SharedPreferencesImpl::MemoryCommitResult
//==============================================================================

SharedPreferencesImpl::MemoryCommitResult::MemoryCommitResult()
    : mChangesMade(FALSE)
    , mWriteToDiskResult(FALSE)
{
    CCountDownLatch::New(1,(ICountDownLatch**)&mWrittenToDiskLatch);
}

void SharedPreferencesImpl::MemoryCommitResult::SetDiskWriteResult(
    /* [in] */ Boolean result)
{
    mWriteToDiskResult = result;
    mWrittenToDiskLatch->CountDown();
}

SharedPreferencesImpl::EditorImpl::AwaitCommitRunnable::AwaitCommitRunnable(
    /* [in] */ MemoryCommitResult* mcr)
    : mMcr(mcr)
{
}

ECode SharedPreferencesImpl::EditorImpl::AwaitCommitRunnable::Run()
{
    mMcr->mWrittenToDiskLatch->Await();
    return NOERROR;
}

SharedPreferencesImpl::EditorImpl::PostWriteRunnable::PostWriteRunnable(
    /* [in] */ AwaitCommitRunnable* awaitCommitRunnable)
    : mAwaitCommitRunnable(awaitCommitRunnable)
{
}

ECode SharedPreferencesImpl::EditorImpl::PostWriteRunnable::Run()
{
    mAwaitCommitRunnable->Run();
    //TODO
    Logger::W(TAG, "[TODO] PostWriteRunnable::Run == There need the class QueuedWork...");
    QueuedWork::Remove(mAwaitCommitRunnable);
    return NOERROR;
}

SharedPreferencesImpl::EditorImpl::NotifyListenersRunnable::NotifyListenersRunnable(
    /* [in] */ EditorImpl* host,
    /* [in] */ MemoryCommitResult* mcr)
    : mHost(host)
    , mMcr(mcr)
{
}

ECode SharedPreferencesImpl::EditorImpl::NotifyListenersRunnable::Run()
{
    mHost->NotifyListeners(mMcr);
    return NOERROR;
}

//==============================================================================
// SharedPreferencesImpl::EditorImpl
//==============================================================================

CAR_INTERFACE_IMPL(SharedPreferencesImpl::EditorImpl, Object, ISharedPreferencesEditor);

SharedPreferencesImpl::EditorImpl::EditorImpl(
    /* [in] */ SharedPreferencesImpl* host)
    : mClear(FALSE)
    , mHost(host)
{
    CHashMap::New((IMap**)&mModified);
}

ECode SharedPreferencesImpl::EditorImpl::PutString(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    AutoLock lock(this);
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    AutoPtr<ICharSequence> valueObj = CoreUtils::Convert(value);
    mModified->Put(keyObj.Get(), valueObj.Get());
    return NOERROR;
}

ECode SharedPreferencesImpl::EditorImpl::PutStringSet(
    /* [in] */ const String& key,
    /* [in] */ ISet* values)
{
    AutoLock lock(this);
    AutoPtr<ISet> valueObj;
    if (values != NULL) {
        CHashSet::New(ICollection::Probe(values), (ISet**)&valueObj);
    }
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    mModified->Put(keyObj.Get(), valueObj.Get());
    return NOERROR;
}

ECode SharedPreferencesImpl::EditorImpl::PutInt32(
    /* [in] */ const String& key,
    /* [in] */ Int32 value)
{
    AutoLock lock(this);
    AutoPtr<IInteger32> valueObj;
    CInteger32::New(value, (IInteger32**)&valueObj);
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    mModified->Put(keyObj.Get(), valueObj.Get());
    return NOERROR;
}

ECode SharedPreferencesImpl::EditorImpl::PutInt64(
    /* [in] */ const String& key,
    /* [in] */ Int64 value)
{
    AutoLock lock(this);
    AutoPtr<IInteger64> valueObj;
    CInteger64::New(value, (IInteger64**)&valueObj);
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    mModified->Put(keyObj.Get(), valueObj.Get());
    return NOERROR;
}

ECode SharedPreferencesImpl::EditorImpl::PutFloat(
    /* [in] */ const String& key,
    /* [in] */ Float value)
{
    AutoLock lock(this);
    AutoPtr<IFloat> valueObj;
    CFloat::New(value, (IFloat**)&valueObj);
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    mModified->Put(keyObj.Get(), valueObj.Get());
    return NOERROR;
}

ECode SharedPreferencesImpl::EditorImpl::PutBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean value)
{
    AutoLock lock(this);
    AutoPtr<IBoolean> valueObj;
    CBoolean::New(value, (IBoolean**)&valueObj);
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    mModified->Put(keyObj.Get(), valueObj.Get());
    return NOERROR;
}

ECode SharedPreferencesImpl::EditorImpl::Remove(
    /* [in] */ const String& key)
{
    AutoLock lock(this);
    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    mModified->Put(keyObj.Get(), TO_IINTERFACE(this));
    return NOERROR;
}

ECode SharedPreferencesImpl::EditorImpl::Clear()
{
    AutoLock lock(this);
    mClear = TRUE;
    return NOERROR;
}

ECode SharedPreferencesImpl::EditorImpl::Apply()
{
    AutoPtr<MemoryCommitResult> mcr = CommitToMemory();
    AutoPtr<AwaitCommitRunnable> awaitCommitRunnable = new AwaitCommitRunnable(mcr);

    QueuedWork::Add(awaitCommitRunnable);
    Logger::W(TAG, "[TODO] Apply == There need the class QueuedWork...");
    AutoPtr<PostWriteRunnable> postWriteRunnable = new PostWriteRunnable(awaitCommitRunnable);
    mHost->EnqueueDiskWrite(mcr, postWriteRunnable);

    // Okay to notify the listeners before it's hit disk
    // because the listeners should always get the same
    // SharedPreferences instance back, which has the
    // changes reflected in memory.
    NotifyListeners(mcr);
    return NOERROR;
}

AutoPtr<SharedPreferencesImpl::MemoryCommitResult> SharedPreferencesImpl::EditorImpl::CommitToMemory()
{
    AutoPtr<SharedPreferencesImpl::MemoryCommitResult> mcr = new MemoryCommitResult();
    {
        AutoLock lock(mHost);
        // We optimistically don't make a deep copy until
        // a memory commit comes in when we're already
        // writing to disk.
        if (mHost->mDiskWritesInFlight > 0) {
            // We can't modify our mMap as a currently
            // in-flight write owns it.  Clone it before
            // modifying it.
            // noinspection unchecked
            mHost->mMap = NULL;
            CHashMap::New((IHashMap**)&mHost->mMap);
        }

        mcr->mMapToWriteToDisk = IMap::Probe(mHost->mMap);
        mHost->mDiskWritesInFlight++;

        Boolean hasListeners;
        mHost->mListeners->IsEmpty(&hasListeners);
        hasListeners = !hasListeners;
        if (hasListeners) {
            mcr->mKeysModified = NULL;
            CArrayList::New((IList**)&mcr->mKeysModified);

            AutoPtr<ISet> keySet;
            mHost->mListeners->GetKeySet((ISet**)&keySet);
            CHashSet::New(ICollection::Probe(keySet), (ISet**)&mcr->mListeners);
        }

        {
            AutoLock lock(this);
            if (mClear) {
                Boolean isEmpty;
                mHost->mMap->IsEmpty(&isEmpty);
                if (!isEmpty) {
                    mcr->mChangesMade = TRUE;
                    mHost->mMap->Clear();
                }
                mClear = FALSE;
            }

            AutoPtr<ISet> entrySet;
            mModified->GetEntrySet((ISet**)&entrySet);
            AutoPtr<IIterator> it;
            entrySet->GetIterator((IIterator**)&it);
            AutoPtr<IInterface> thisObj = TO_IINTERFACE(this);
            Boolean hasNext, contains;
            IMapEntry* entry;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj, k, v;
                it->GetNext((IInterface**)&obj);
                entry = IMapEntry::Probe(obj);
                entry->GetKey((IInterface**)&k);
                entry->GetValue((IInterface**)&v);

                // "this" is the magic value for a removal mutation. In addition,
                // setting a value to "null" for a given key is specified to be
                // equivalent to calling remove on that key.
                mHost->mMap->ContainsKey(k, &contains);
                if (v.Get() == thisObj || v.Get() == NULL) {
                    if (!contains) {
                        continue;
                    }
                    mHost->mMap->Remove(k);
                }
                else {
                    if (contains) {
                        AutoPtr<IInterface> existingValue;
                        mHost->mMap->Get(k, (IInterface**)&existingValue);
                        if (existingValue != NULL && Object::Equals(existingValue, v)) {
                            continue;
                        }
                    }
                    mHost->mMap->Put(k, v);
                }

                mcr->mChangesMade = TRUE;
                if (hasListeners) {
                    mcr->mKeysModified->Add(k);
                }
            }

            mModified->Clear();
        }
    }
    return mcr;
}

ECode SharedPreferencesImpl::EditorImpl::Commit(
    /* [out] */ Boolean* succeded)
{
    VALIDATE_NOT_NULL(succeded);
    AutoPtr<MemoryCommitResult> mcr = CommitToMemory();
    mHost->EnqueueDiskWrite(
        mcr, NULL /* sync write on this thread okay */);

    ECode ec = mcr->mWrittenToDiskLatch->Await();
    if (FAILED(ec)) {
        *succeded = FALSE;
        return NOERROR;
    }
    NotifyListeners(mcr);
    *succeded = mcr->mWriteToDiskResult;
    return NOERROR;
}

void SharedPreferencesImpl::EditorImpl::NotifyListeners(
    /* [in] */ MemoryCommitResult* mcr)
{
    Int32 size = 0;
    if (mcr->mListeners == NULL || mcr->mKeysModified == NULL
        || (mcr->mKeysModified->GetSize(&size), size) == 0) {
        return;
    }

    if (Looper::GetMyLooper() == Looper::GetMainLooper()) {
        ISharedPreferencesOnSharedPreferenceChangeListener* listener = NULL;
        Boolean hasNext;
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> keyObj;
            mcr->mKeysModified->Get(i, (IInterface**)&keyObj);
            String key = Object::ToString(keyObj);

            AutoPtr<IIterator> it;
            mcr->mListeners->GetIterator((IIterator**)&it);
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> listenerObj;
                it->GetNext((IInterface**)&listenerObj);
                listener = ISharedPreferencesOnSharedPreferenceChangeListener::Probe(listenerObj);
                if (listener != NULL) {
                    listener->OnSharedPreferenceChanged(mHost, key);
                }
            }
        }
    }
    else {
        AutoPtr<NotifyListenersRunnable> notifyListenersRunnable = new NotifyListenersRunnable(this, mcr);
        // Run this function on the main thread.
        Boolean isSuccess = FALSE;
        CActivityThread::sMainThreadHandler->Post(notifyListenersRunnable, &isSuccess);
    }
}

//==============================================================================
// SharedPreferencesImpl::LoadFromDiskLockedRunnable
//==============================================================================

SharedPreferencesImpl::LoadFromDiskLockedRunnable::LoadFromDiskLockedRunnable(
    /* [in] */ SharedPreferencesImpl* host)
    : mHost(host)
{
}

ECode SharedPreferencesImpl::LoadFromDiskLockedRunnable::Run()
{
    AutoLock lock(mHost);
    mHost->LoadFromDiskLocked();
    return NOERROR;
}

//==============================================================================
// SharedPreferencesImpl::WriteToDiskRunnable
//==============================================================================
SharedPreferencesImpl::WriteToDiskRunnable::WriteToDiskRunnable(
    /* [in] */ MemoryCommitResult* mcr,
    /* [in] */ Runnable* postWriteRunnable,
    /* [in] */ SharedPreferencesImpl* host)
    : mHost(host)
    , mMcr(mcr)
    , mPostWriteRunnable(postWriteRunnable)
{
}

ECode SharedPreferencesImpl::WriteToDiskRunnable::Run()
{
    {
        AutoLock lock(mHost->mWritingToDiskLock);
        mHost->WriteToFile(mMcr);
    }
    {
        AutoLock lock(mHost);
        mHost->mDiskWritesInFlight--;
    }
    if (mPostWriteRunnable != NULL) {
        mPostWriteRunnable->Run();
    }
    return NOERROR;
}

//==============================================================================
// SharedPreferencesImpl
//==============================================================================
static AutoPtr<IInterface> InitContent()
{
    AutoPtr<IObject> obj;
    Elastos::Core::CObject::New((IObject**)&obj);
    return (IInterface*)obj.Get();
}

const String SharedPreferencesImpl::TAG("SharedPreferencesImpl");
const Boolean SharedPreferencesImpl::DEBUG = FALSE;
AutoPtr<IInterface> SharedPreferencesImpl::mContent = InitContent();

CAR_INTERFACE_IMPL(SharedPreferencesImpl, Object, ISharedPreferences);

SharedPreferencesImpl::SharedPreferencesImpl()
    : mDiskWritesInFlight(0)
    , mLoaded(FALSE)
    , mStatTimestamp(0)
    , mStatSize(0)
{}

SharedPreferencesImpl::~SharedPreferencesImpl()
{}

ECode SharedPreferencesImpl::constructor(
    /* [in] */ IFile* file,
    /* [in] */ Int32 mode)
{
    CWeakHashMap::New((IWeakHashMap**)&mListeners);
    mFile = file;
    mBackupFile = MakeBackupFile(file);
    mMode = mode;

    StartLoadFromDisk();
    return NOERROR;
}

void SharedPreferencesImpl::StartLoadFromDisk()
{
    {
        AutoLock lock(this);
        mLoaded = FALSE;
    }

    AutoPtr<IRunnable> runnable = (IRunnable*)new LoadFromDiskLockedRunnable(this);
    AutoPtr<IThread> thread;
    CThread::New(runnable, String("SharedPreferencesImpl-load"), (IThread**)&thread);
    thread->Start();
}

void SharedPreferencesImpl::LoadFromDiskLocked()
{
    if (mLoaded) {
        return;
    }
    Boolean isExist = FALSE;
    mBackupFile->Exists(&isExist);
    if (isExist) {
        Boolean value = FALSE;
        mFile->Delete(&value);
        mBackupFile->RenameTo(mFile, &value);
    }

    // Debugging
    Boolean exists, isCanRead = FALSE;
    if ((mFile->Exists(&exists), exists) && !(mFile->CanRead(&isCanRead), isCanRead)) {
        Logger::W(TAG, "Attempt to read preferences file %p without permission", mFile.Get());
    }

    AutoPtr<IHashMap> map;
    AutoPtr<IStructStat> stat;
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);

    do {
        ECode ec = NOERROR;
        String path;
        mFile->GetPath(&path);
        ec = Elastos::Droid::System::Os::Stat(path, (IStructStat**)&stat);
        if (FAILED(ec)) {
            break;
        }

        mFile->CanRead(&isCanRead);
        if (isCanRead) {
            AutoPtr<IInputStream> fileStr;
            ec = CFileInputStream::New(mFile, (IInputStream**)&fileStr);
            if (FAILED(ec)) {
                break;
            }

            AutoPtr<IInputStream> str;
            ec = CBufferedInputStream::New(fileStr, 16*1024, (IInputStream**)&str);
            AutoPtr<ICloseable> closeable = ICloseable::Probe(str);
            if (FAILED(ec)) {
                ioUtils->CloseQuietly(closeable);
                break;
            }

            XmlUtils::ReadMapXml(str, (IHashMap**)&map);
            ioUtils->CloseQuietly(closeable);
        }
    } while (FALSE);

    mLoaded = TRUE;
    if (map != NULL) {
        mMap = map;
        stat->GetMtime(&mStatTimestamp);
        stat->GetSize(&mStatSize);
    }
    else {
        mMap = NULL;
        CHashMap::New((IHashMap**)&mMap);
    }

    NotifyAll();
}

AutoPtr<IFile> SharedPreferencesImpl::MakeBackupFile(
    /* [in] */ IFile* prefsFile)
{
    AutoPtr<IFile> file;
    String fileName;
    prefsFile->GetPath(&fileName);
    fileName = fileName + ".bak";
    CFile::New(fileName, (IFile**)&file);
    return file;
}

void SharedPreferencesImpl::StartReloadIfChangedUnexpectedly()
{
    AutoLock lock(this);
    // TODO: wait for any pending writes to disk?
    if (!HasFileChangedUnexpectedly()) {
        return;
    }
    StartLoadFromDisk();
}

Boolean SharedPreferencesImpl::HasFileChangedUnexpectedly()
{
    {
        AutoLock lock(this);
        if (mDiskWritesInFlight > 0) {
            // If we know we caused it, it's not unexpected.
            //if (DEBUG) Log.d(TAG, "disk write in flight, not unexpected.");
            return FALSE;
        }
    }

    AutoPtr<IStructStat> stat;
    /*
     * Metadata operations don't usually count as a block guard
     * violation, but we explicitly want this one.
     */
    AutoPtr<IBlockGuard> helper;
    CBlockGuard::AcquireSingleton((IBlockGuard**)&helper);
    AutoPtr<IBlockGuardPolicy> policy;
    helper->GetThreadPolicy((IBlockGuardPolicy**)&policy);
    policy->OnReadFromDisk();
    String path;
    mFile->GetPath(&path);

    ECode ec = Elastos::Droid::System::Os::Stat(path, (IStructStat**)&stat);
    if(FAILED(ec)) {
        return TRUE;
    }

    {
        AutoLock lock(this);
        Int64 st_mtime = 0;
        Int64 st_size = 0;
        stat->GetMtime(&st_mtime);
        stat->GetSize(&st_size);
        return mStatTimestamp != st_mtime || mStatSize != st_size;
    }
}

ECode SharedPreferencesImpl::RegisterOnSharedPreferenceChangeListener(
    /* [in] */ ISharedPreferencesOnSharedPreferenceChangeListener* listener)
{
    AutoLock lock(this);
    return mListeners->Put(listener, mContent.Get());
}

ECode SharedPreferencesImpl::UnregisterOnSharedPreferenceChangeListener(
    /* [in] */ ISharedPreferencesOnSharedPreferenceChangeListener* listener)
{
    AutoLock lock(this);
    return mListeners->Remove(listener);
}

void SharedPreferencesImpl::AwaitLoadedLocked()
{
    if (!mLoaded) {
        // Raise an explicit StrictMode onReadFromDisk for this
        // thread, since the real read will be in a different
        // thread and otherwise ignored by StrictMode.

        AutoPtr<IBlockGuard> helper;
        CBlockGuard::AcquireSingleton((IBlockGuard**)&helper);
        AutoPtr<IBlockGuardPolicy> policy;
        helper->GetThreadPolicy((IBlockGuardPolicy**)&policy);
        policy->OnReadFromDisk();
    }
    while (!mLoaded) {
        Wait();
    }
}

ECode SharedPreferencesImpl::GetAll(
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(this);
    AwaitLoadedLocked();
    //noinspection unchecked
    AutoPtr<IMap> map;
    CHashMap::New(IMap::Probe(mMap), (IMap**)&map);
    *result = map;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SharedPreferencesImpl::GetString(
    /* [in] */ const String& key,
    /* [in] */ const String& defValue,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = defValue;

    AutoLock lock(this);
    AwaitLoadedLocked();

    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    AutoPtr<IInterface> valueObj;
    mMap->Get(keyObj.Get(), (IInterface**)&valueObj);

    if (valueObj != NULL) {
        *value = Object::ToString(valueObj);
    }
    return NOERROR;
}

ECode SharedPreferencesImpl::GetStringSet(
    /* [in] */ const String& key,
    /* [in] */ ISet* defValue,
    /* [out] */ ISet** value)
{
    VALIDATE_NOT_NULL(value);

    AutoLock lock(this);
    AwaitLoadedLocked();

    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    AutoPtr<IInterface> valueObj;
    mMap->Get(keyObj.Get(), (IInterface**)&valueObj);

    if (valueObj != NULL) {
        *value = ISet::Probe(valueObj);
        REFCOUNT_ADD(*value);
    }
    else {
        *value = defValue;
        REFCOUNT_ADD(*value);
    }
    return NOERROR;
}

ECode SharedPreferencesImpl::GetInt32(
    /* [in] */ const String& key,
    /* [in] */ Int32 defValue,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = defValue;

    AutoLock lock(this);
    AwaitLoadedLocked();

    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    AutoPtr<IInterface> valueObj;
    mMap->Get(keyObj.Get(), (IInterface**)&valueObj);

    if (valueObj != NULL) {
        IInteger32::Probe(valueObj)->GetValue(value);
    }

    return NOERROR;
}

ECode SharedPreferencesImpl::GetInt64(
    /* [in] */ const String& key,
    /* [in] */ Int64 defValue,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);
    *value = defValue;

    AutoLock lock(this);
    AwaitLoadedLocked();

    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    AutoPtr<IInterface> valueObj;
    mMap->Get(keyObj.Get(), (IInterface**)&valueObj);

    if (valueObj != NULL) {
        IInteger64::Probe(valueObj)->GetValue(value);
    }

    return NOERROR;
}

ECode SharedPreferencesImpl::GetFloat(
    /* [in] */ const String& key,
    /* [in] */ Float defValue,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = defValue;

    AutoLock lock(this);
    AwaitLoadedLocked();

    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    AutoPtr<IInterface> valueObj;
    mMap->Get(keyObj.Get(), (IInterface**)&valueObj);

    if (valueObj != NULL) {
        IFloat::Probe(valueObj)->GetValue(value);
    }

    return NOERROR;
}

ECode SharedPreferencesImpl::GetBoolean(
    /* [in] */ const String& key,
    /* [in] */ Boolean defValue,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = defValue;

    AutoLock lock(this);
    AwaitLoadedLocked();

    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    AutoPtr<IInterface> valueObj;
    mMap->Get(keyObj.Get(), (IInterface**)&valueObj);

    if (valueObj != NULL) {
        IBoolean::Probe(valueObj)->GetValue(value);
    }

    return NOERROR;
}

ECode SharedPreferencesImpl::Contains(
    /* [in] */ const String& key,
    /* [out] */ Boolean* isContain)
{
    VALIDATE_NOT_NULL(isContain);

    AutoLock lock(this);
    AwaitLoadedLocked();

    AutoPtr<ICharSequence> keyObj = CoreUtils::Convert(key);
    return mMap->ContainsKey(keyObj.Get(), isContain);
}

ECode SharedPreferencesImpl::Edit(
    /* [out] */ ISharedPreferencesEditor** result)
{
    // TODO: remove the need to call awaitLoadedLocked() when
    // requesting an editor.  will require some work on the
    // Editor, but then we should be able to do:
    //
    //      context.getSharedPreferences(..).edit().putString(..).apply()
    //
    // ... all without blocking.
    VALIDATE_NOT_NULL(result);
    {
        AutoLock lock(this);
        AwaitLoadedLocked();
    }
    AutoPtr<EditorImpl> editorImpl = new EditorImpl(this);
    *result = (ISharedPreferencesEditor*)editorImpl.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void SharedPreferencesImpl::EnqueueDiskWrite(
    /* [in] */ MemoryCommitResult* mcr,
    /* [in] */ Runnable* postWriteRunnable)
{
    AutoPtr<WriteToDiskRunnable> writeToDiskRunnable =
        new WriteToDiskRunnable(mcr, postWriteRunnable, this);

    const Boolean isFromSyncCommit = (postWriteRunnable == NULL);

    // Typical #commit() path with fewer allocations, doing a write on
    // the current thread.
    if (isFromSyncCommit) {
        Boolean wasEmpty = FALSE;
        {
            AutoLock lock(this);
            wasEmpty = mDiskWritesInFlight == 1;
        }
        if (wasEmpty) {
            writeToDiskRunnable->Run();
            return;
        }
    }

    AutoPtr<IExecutorService> singleThreadExecutor;
    QueuedWork::SingleThreadExecutor((IExecutorService**)&singleThreadExecutor);
    assert(singleThreadExecutor != NULL);
    IExecutor::Probe(singleThreadExecutor)->Execute(writeToDiskRunnable);
}

AutoPtr<IFileOutputStream> SharedPreferencesImpl::CreateFileOutputStream(
    /* [in] */ IFile* file)
{
    AutoPtr<IFileOutputStream> str;
    if (CFileOutputStream::New(file, (IFileOutputStream**)&str) == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        AutoPtr<IFile> parent;
        file->GetParentFile((IFile**)&parent);
        Boolean successed = FALSE;

        if (!(parent->Mkdir(&successed), successed)) {
            Logger::E(TAG, "Couldn't create directory for SharedPreferences file %p", file);
            return NULL;
        }
        String path;
        parent->GetPath(&path);
        FileUtils::SetPermissions(
            path,
            FileUtils::sS_IRWXU | FileUtils::sS_IRWXG | FileUtils::sS_IXOTH,
            -1, -1);

        str = NULL;
        if (CFileOutputStream::New(file, (IFileOutputStream**)&str) == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
            Logger::E(TAG, "Couldn't create SharedPreferences file %p", file);
        }
    }
    return str;
}

void SharedPreferencesImpl::WriteToFile(
    /* [in] */ MemoryCommitResult* mcr)
{
    // Rename the current file so it may be used as a backup during the next read
    Boolean isExist = FALSE;
    mFile->Exists(&isExist);
    if (isExist) {
        if (!mcr->mChangesMade) {
            // If the file already exists, but no changes were
            // made to the underlying map, it's wasteful to
            // re-write the file.  Return as if we wrote it
            // out.
            mcr->SetDiskWriteResult(TRUE);
            return;
        }
        Boolean isExistFile = FALSE;
        mBackupFile->Exists(&isExistFile);
        if (!isExistFile) {
            Boolean isSuccess = FALSE;
            mFile->RenameTo(mBackupFile, &isSuccess);
            if (!isSuccess) {
                //Log.e(TAG, "Couldn't rename file " + mFile
                //      + " to backup file " + mBackupFile);
                mcr->SetDiskWriteResult(FALSE);
                return;
            }
        } else {
            Boolean isDelete = FALSE;
            mFile->Delete(&isDelete);
        }
    }

    // Attempt to write the file, delete the backup and return true as atomically as
    // possible.  If any exception occurs, delete the new file; next time we will restore
    // from the backup.

    AutoPtr<IFileOutputStream> str = CreateFileOutputStream(mFile);
    if (str == NULL) {
        mcr->SetDiskWriteResult(FALSE);
        return;
    }

    Boolean isDeleteFile = FALSE;
    String path;
    AutoPtr<IStructStat> stat;
    FAIL_GOTO(XmlUtils::WriteMapXml(mcr->mMapToWriteToDisk, IOutputStream::Probe(str)), failed);
    FileUtils::Sync(str);
    FAIL_GOTO(ICloseable::Probe(str)->Close(), failed);
    FAIL_GOTO(mFile->GetPath(&path), failed);
    CContextImpl::SetFilePermissionsFromMode(path, mMode, 0);

    FAIL_GOTO(Elastos::Droid::System::Os::Stat(path, (IStructStat**)&stat), failed);
    {
        AutoLock lock(this);
        stat->GetMtime(&mStatTimestamp);
        stat->GetSize(&mStatSize);
    }

    // Writing was successful, delete the backup file if there is one.
    FAIL_GOTO(mBackupFile->Delete(&isDeleteFile), failed);
    mcr->SetDiskWriteResult(TRUE);
    return;

failed:
    // Clean up an unsuccessfully written file
    if (isExist) {
        Boolean mFileIsDelete = FALSE;
        mFile->Delete(&mFileIsDelete);
        if (mFileIsDelete) {
            //Log.e(TAG, "Couldn't clean up partially-written file " + mFile);
        }
    }
    mcr->SetDiskWriteResult(FALSE);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
