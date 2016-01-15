#ifndef __ELASTOS_DROID_APP_SHAREDPREFERENCES_H__
#define __ELASTOS_DROID_APP_SHAREDPREFERENCES_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::ISharedPreferencesOnSharedPreferenceChangeListener;
using Elastos::Droid::Os::Runnable;

using Elastos::IO::IFile;
using Elastos::IO::IFileOutputStream;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IMap;
using Elastos::Utility::IWeakHashMap;
using Elastos::Utility::Concurrent::ICountDownLatch;

namespace Elastos {
namespace Droid {
namespace App {

class SharedPreferencesImpl
    : public Object
    , public ISharedPreferences
{
private:
    class MemoryCommitResult
        : public Object
    {
    public:
        MemoryCommitResult();

        CARAPI_(void) SetDiskWriteResult(
            /* [in] */ Boolean result);

    public:
        Boolean mChangesMade;  // any keys different?
        AutoPtr<IList> mKeysModified;  // may be null
        AutoPtr<ISet> mListeners; //Set<OnSharedPreferenceChangeListener> // may be null
        AutoPtr<IMap> mMapToWriteToDisk;
        AutoPtr<ICountDownLatch> mWrittenToDiskLatch;
        volatile Boolean mWriteToDiskResult;
    };

public:
    class EditorImpl
        : public Object
        , public ISharedPreferencesEditor
    {
    private:
        class NotifyListenersRunnable
            : public Runnable
        {
        public:
            NotifyListenersRunnable(
                /* [in] */ EditorImpl* host,
                /* [in] */ MemoryCommitResult* mcr);

            CARAPI Run();

        public:
            EditorImpl* mHost;
            AutoPtr<MemoryCommitResult> mMcr;
        };

        class AwaitCommitRunnable
            : public Runnable
        {
        public:
            AwaitCommitRunnable(
                /* [in] */ MemoryCommitResult* mcr);

            CARAPI Run();

        public:
            AutoPtr<MemoryCommitResult> mMcr;
        };

        class PostWriteRunnable
            : public Runnable
        {
        public:
            PostWriteRunnable(
                /* [in] */ AwaitCommitRunnable* awaitCommitRunnable);

            CARAPI Run();

        public:
            AutoPtr<AwaitCommitRunnable> mAwaitCommitRunnable;
        };

    public:
        CAR_INTERFACE_DECL();

        EditorImpl(
            /* [in] */ SharedPreferencesImpl* host);

        CARAPI PutString(
            /* [in] */ const String& key,
            /* [in] */ const String& value);

        CARAPI PutStringSet(
            /* [in] */ const String& key,
            /* [in] */ ISet* values);

        CARAPI PutInt32(
            /* [in] */ const String& key,
            /* [in] */ Int32 value);

        CARAPI PutInt64(
            /* [in] */ const String& key,
            /* [in] */ Int64 value);

        CARAPI PutFloat(
            /* [in] */ const String& key,
            /* [in] */ Float value);

        CARAPI PutBoolean(
            /* [in] */ const String& key,
            /* [in] */ Boolean value);

        CARAPI Remove(
            /* [in] */ const String& key);

        CARAPI Clear();

        CARAPI Apply();

        CARAPI Commit(
            /* [out] */ Boolean* succeded);

    private:
        // Returns true if any changes were made
        CARAPI_(AutoPtr<SharedPreferencesImpl::MemoryCommitResult>) CommitToMemory();

        CARAPI_(void) NotifyListeners(
            /* [in] */ MemoryCommitResult* mcr);

    private:
        AutoPtr<IMap> mModified;//new Map<String, Object>
        Boolean mClear;
        SharedPreferencesImpl* mHost;
    };

private:
    class LoadFromDiskLockedRunnable
        : public Runnable
    {
    public:
        LoadFromDiskLockedRunnable(
        /* [in] */ SharedPreferencesImpl* host);

        virtual CARAPI Run();

    public:
        SharedPreferencesImpl* mHost;
    };

    class WriteToDiskRunnable
        : public Runnable
    {
    public:
        WriteToDiskRunnable(
            /* [in] */ MemoryCommitResult* mcr,
            /* [in] */ Runnable* postWriteRunnable,
            /* [in] */ SharedPreferencesImpl* host);

        virtual CARAPI Run();

    public:
        SharedPreferencesImpl* mHost;
        AutoPtr<MemoryCommitResult> mMcr;
        AutoPtr<IRunnable> mPostWriteRunnable;
    };

public:
    CAR_INTERFACE_DECL();

    SharedPreferencesImpl();

    virtual ~SharedPreferencesImpl();

    CARAPI constructor(
        /* [in] */ IFile* file,
        /* [in] */ Int32 mode);

    CARAPI RegisterOnSharedPreferenceChangeListener(
        /* [in] */ ISharedPreferencesOnSharedPreferenceChangeListener* listener);

    CARAPI UnregisterOnSharedPreferenceChangeListener(
        /* [in] */ ISharedPreferencesOnSharedPreferenceChangeListener* listener);

    CARAPI GetAll(
        /* [out] */ IMap** result);

    CARAPI GetString(
        /* [in] */ const String& key,
        /* [in] */ const String& defValue,
        /* [out] */ String* value);

    CARAPI GetStringSet(
        /* [in] */ const String& key,
        /* [in] */ ISet* defValues,
        /* [out] */ ISet** values);

    CARAPI GetInt32(
        /* [in] */ const String& key,
        /* [in] */ Int32 defValue,
        /* [out] */ Int32* value);

    CARAPI GetInt64(
        /* [in] */ const String& key,
        /* [in] */ Int64 defValue,
        /* [out] */ Int64* value);

    CARAPI GetFloat(
        /* [in] */ const String& key,
        /* [in] */ Float defValue,
        /* [out] */ Float* value);

    CARAPI GetBoolean(
        /* [in] */ const String& key,
        /* [in] */ Boolean defValue,
        /* [out] */ Boolean* value);

    CARAPI Contains(
        /* [in] */ const String& key,
        /* [out] */ Boolean* isContain);

    CARAPI Edit(
        /* [out] */ ISharedPreferencesEditor** result);

protected:
    CARAPI_(void) StartReloadIfChangedUnexpectedly();

private:
    CARAPI_(void) StartLoadFromDisk();

    CARAPI_(void) LoadFromDiskLocked();

    static CARAPI_(AutoPtr<IFile>) MakeBackupFile(
        /* [in] */ IFile* prefsFile);

    // Has the file changed out from under us?  i.e. writes that
    // we didn't instigate.
    CARAPI_(Boolean) HasFileChangedUnexpectedly();

    CARAPI_(void) AwaitLoadedLocked();

    /**
     * Enqueue an already-committed-to-memory result to be written
     * to disk.
     *
     * They will be written to disk one-at-a-time in the order
     * that they're enqueued.
     *
     * @param postWriteRunnable if non-null, we're being called
     *   from apply() and this is the runnable to run after
     *   the write proceeds.  if null (from a regular commit()),
     *   then we're allowed to do this disk write on the main
     *   thread (which in addition to reducing allocations and
     *   creating a background thread, this has the advantage that
     *   we catch them in userdebug StrictMode reports to convert
     *   them where possible to apply() ...)
     */
    CARAPI_(void) EnqueueDiskWrite(
        /* [in] */ MemoryCommitResult* mcr,
        /* [in] */ Runnable* postWriteRunnable);

    static CARAPI_(AutoPtr<IFileOutputStream>) CreateFileOutputStream(
        /* [in] */ IFile* file);

        // Note: must hold mWritingToDiskLock
    CARAPI_(void) WriteToFile(
        /* [in] */ MemoryCommitResult *mcr);

private:
    static const String TAG;
    static const Boolean DEBUG;

    // Lock ordering rules:
    //  - acquire SharedPreferencesImpl.this before EditorImpl.this
    //  - acquire mWritingToDiskLock before EditorImpl.this

    AutoPtr<IFile> mFile;
    AutoPtr<IFile> mBackupFile;
    Int32 mMode;

    AutoPtr<IHashMap> mMap;//Map<String, Object> ;     // guarded by 'this'
    Int32 mDiskWritesInFlight;  // guarded by 'this'
    Boolean mLoaded;      // guarded by 'this'
    Int64 mStatTimestamp;          // guarded by 'this'
    Int64 mStatSize;               // guarded by 'this'

    Object mWritingToDiskLock;
    static AutoPtr<IInterface> mContent;
    AutoPtr<IWeakHashMap> mListeners;//new WeakHashMap<OnSharedPreferenceChangeListener, Object>();

    friend class CContextImpl;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::App::SharedPreferencesImpl::EditorImpl, IInterface)

#endif // __ELASTOS_DROID_APP_SHAREDPREFERENCES_H__
