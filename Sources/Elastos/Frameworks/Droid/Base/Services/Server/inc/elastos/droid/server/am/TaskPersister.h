
#ifndef __ELASTOS_DROID_SERVER_AM_TASKPERSISTER_H__
#define __ELASTOS_DROID_SERVER_AM_TASKPERSISTER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/Set.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Core::Thread;
using Elastos::IO::IBufferedReader;
using Elastos::IO::IFile;
using Elastos::IO::IStringWriter;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Set;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ActivityStackSupervisor;
class CActivityManagerService;
class TaskRecord;

class TaskPersister : public Object
{
private:
    class WriteQueueItem : public Object
    {
    public:
        WriteQueueItem(
            /* [in] */ Int32 type)
            : mType(type)
        {}

    public:
        static const Int32 TYPE_TASK = 0;
        static const Int32 TYPE_IMAGE = 1;
        const Int32 mType;
    };

    class TaskWriteQueueItem : public WriteQueueItem
    {
    public:
        TaskWriteQueueItem(
            /* [in] */ TaskRecord* task)
            : WriteQueueItem(TYPE_TASK)
            , mTask(task)
        {
        }

    public:
        const AutoPtr<TaskRecord> mTask;
    };

    class ImageWriteQueueItem : public WriteQueueItem
    {
    public:
        ImageWriteQueueItem(
            /* [in] */ const String& filename,
            /* [in] */ IBitmap* image)
            : WriteQueueItem(TYPE_IMAGE)
            , mFilename(filename)
            , mImage(image)
        {
        }

    public:
        const String mFilename;
        AutoPtr<IBitmap> mImage;
    };

    class LazyTaskWriterThread : public Thread
    {
    public:
        LazyTaskWriterThread(
            /* [in] */ const String& name,
            /* [in] */ TaskPersister* host);

        // @Override
        CARAPI Run();

    private:
        TaskPersister* mHost;
    };

public:
    TaskPersister(
        /* [in] */ IFile* systemDir,
        /* [in] */ ActivityStackSupervisor* stackSupervisor);

    CARAPI_(void) StartPersisting();

    CARAPI_(void) Wakeup(
        /* [in] */ TaskRecord* task,
        /* [in] */ Boolean flush);

    CARAPI_(void) Flush();

    CARAPI_(void) SaveImage(
        /* [in] */ IBitmap* image,
        /* [in] */ const String& filename);

    CARAPI_(AutoPtr<IBitmap>) GetTaskDescriptionIcon(
        /* [in] */ const String& filename);

    CARAPI_(AutoPtr<IBitmap>) GetImageFromWriteQueue(
        /* [in] */ const String& filename);

    CARAPI_(AutoPtr<List<AutoPtr<TaskRecord> > >) RestoreTasksLocked();

    static CARAPI_(AutoPtr<IBitmap>) RestoreImage(
        /* [in] */ const String& filename);

private:
    CARAPI_(void) RemoveThumbnails(
        /* [in] */ TaskRecord* task);

    CARAPI_(void) YieldIfQueueTooDeep();

    CARAPI SaveToXml(
        /* [in] */ TaskRecord* task,
        /* [out] */ IStringWriter** writer);

    CARAPI_(String) FileToString(
        /* [in] */ IFile* file);

    CARAPI_(AutoPtr<TaskRecord>) TaskIdToTask(
        /* [in] */ Int32 taskId,
        /* [in] */ List<AutoPtr<TaskRecord> >* tasks);

    static CARAPI_(Int32) CompareTaskRecord(
        /* [in] */ TaskRecord* lhs,
        /* [in] */ TaskRecord* rhs);

    static CARAPI_(void) RemoveObsoleteFiles(
        /* [in] */ Set<Int32>* persistentTaskIds,
        /* [in] */ ArrayOf<IFile*>* files);

    CARAPI_(void) RemoveObsoleteFiles(
        /* [in] */ Set<Int32>* persistentTaskIds);

public:
    static const String TAG;
    static const Boolean DEBUG = FALSE;
    static const String IMAGE_EXTENSION;

    static AutoPtr<IFile> sImagesDir;
    static AutoPtr<IFile> sTasksDir;

    List<AutoPtr<WriteQueueItem> > mWriteQueue;

private:
    /** When not flushing don't write out files faster than this */
    static const Int64 INTER_WRITE_DELAY_MS = 500;

    /** When not flushing delay this Int64 before writing the first file out. This gives the next
     * task being launched a chance to load its resources without this occupying IO bandwidth. */
    static const Int64 PRE_TASK_DELAY_MS = 3000;

    /** The maximum number of entries to keep in the queue before draining it automatically. */
    static const Int32 MAX_WRITE_QUEUE_LENGTH = 6;

    /** Special value for mWriteTime to mean don't wait, just write */
    static const Int64 FLUSH_QUEUE = -1;

    static const String RECENTS_FILENAME;
    static const String TASKS_DIRNAME;
    static const String TASK_EXTENSION;
    static const String IMAGES_DIRNAME;

    static const String TAG_TASK;

    CActivityManagerService* mService;
    ActivityStackSupervisor* mStackSupervisor;

    /** Value determines write delay mode as follows:
     *    < 0 We are Flushing. No delays between writes until the image queue is drained and all
     * tasks needing persisting are written to disk. There is no delay between writes.
     *    == 0 We are Idle. Next writes will be delayed by #PRE_TASK_DELAY_MS.
     *    > 0 We are Actively writing. Next write will be at this time. Subsequent writes will be
     * delayed by #INTER_WRITE_DELAY_MS. */
    Int64 mNextWriteTime;

    AutoPtr<LazyTaskWriterThread> mLazyTaskWriterThread;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_TASKPERSISTER_H__
