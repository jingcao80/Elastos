
#include "elastos/droid/server/am/TaskPersister.h"
#include "elastos/droid/server/am/ActivityRecord.h"
#include "elastos/droid/server/am/ActivityStack.h"
#include "elastos/droid/server/am/ActivityStackSupervisor.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/am/TaskRecord.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/droid/internal/utility/XmlUtils.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/utility/Xml.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Graphics::BitmapCompressFormat_PNG;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Utility::Xml;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CBufferedReader;
using Elastos::IO::CFile;
using Elastos::IO::CFileReader;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CStringWriter;
using Elastos::IO::ICloseable;
using Elastos::IO::IFlushable;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFileReader;
using Elastos::IO::IOutputStream;
using Elastos::IO::IReader;
using Elastos::IO::IWriter;
using Elastos::Utility::Logging::Slogger;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

const Int32 TaskPersister::WriteQueueItem::TYPE_TASK;
const Int32 TaskPersister::WriteQueueItem::TYPE_IMAGE;

const String TaskPersister::TAG("TaskPersister");
const Boolean TaskPersister::DEBUG;
const String TaskPersister::IMAGE_EXTENSION(".png");
AutoPtr<IFile> TaskPersister::sImagesDir;
AutoPtr<IFile> TaskPersister::sTasksDir;
const Int64 TaskPersister::INTER_WRITE_DELAY_MS;
const Int64 TaskPersister::PRE_TASK_DELAY_MS;
const Int32 TaskPersister::MAX_WRITE_QUEUE_LENGTH;
const Int64 TaskPersister::FLUSH_QUEUE;
const String TaskPersister::RECENTS_FILENAME("_task");
const String TaskPersister::TASKS_DIRNAME("recent_tasks");
const String TaskPersister::TASK_EXTENSION(".xml");
const String TaskPersister::IMAGES_DIRNAME("recent_images");
const String TaskPersister::TAG_TASK("task");

TaskPersister::LazyTaskWriterThread::LazyTaskWriterThread(
    /* [in] */ const String& name,
    /* [in] */ TaskPersister* host)
    : mHost(host)
{
    constructor(name);
}

ECode TaskPersister::LazyTaskWriterThread::Run()
{
    AutoPtr<Set<Int32> > persistentTaskIds = new Set<Int32>();
    while (TRUE) {
        // We can't lock mService while holding mHost, but we don't want to
        // call RemoveObsoleteFiles every time through the loop, only the last time before
        // going to sleep. The risk is that we call RemoveObsoleteFiles() successively.
        Boolean probablyDone;
        synchronized (mHost) {
            probablyDone = mHost->mWriteQueue.IsEmpty();
        }
        CActivityManagerService* service = mHost->mService;
        if (probablyDone) {
            if (DEBUG) Slogger::D(TAG, "Looking for obsolete files.");
            persistentTaskIds->Clear();
            synchronized (service) {
                assert(0);
                AutoPtr<List<AutoPtr<TaskRecord> > > tasks;// = service->mRecentTasks;
                // if (DEBUG) Slogger::D(TAG, "mRecents=" + tasks);
                List<AutoPtr<TaskRecord> >::ReverseIterator riter;
                for (riter = tasks->RBegin(); riter != tasks->REnd(); ++riter) {
                    AutoPtr<TaskRecord> task = *riter;
                    if (DEBUG) Slogger::D(TAG, "LazyTaskWriter: task=%s persistable=%d",
                            task->ToString().string(), task->mIsPersistable);
                    if (task->mIsPersistable && !task->mStack->IsHomeStack()) {
                        if (DEBUG) Slogger::D(TAG, "adding to persistentTaskIds task=%s",
                                task->ToString().string());
                        persistentTaskIds->Insert(task->mTaskId);
                    }
                    else {
                        if (DEBUG) Slogger::D(TAG, "omitting from persistentTaskIds task=%s",
                                task->ToString().string());
                    }
                }
            }
            mHost->RemoveObsoleteFiles(persistentTaskIds);
        }

        // If mNextWriteTime, then don't delay between each call to saveToXml().
        AutoPtr<WriteQueueItem> item;
        synchronized (mHost) {
            if (mHost->mNextWriteTime != FLUSH_QUEUE) {
                // The next write we don't have to wait so Int64.
                mHost->mNextWriteTime = SystemClock::GetUptimeMillis() + INTER_WRITE_DELAY_MS;
                if (DEBUG) Slogger::D(TAG, "Next write time may be in %lld msec. (%lld)",
                    INTER_WRITE_DELAY_MS, mHost->mNextWriteTime);
            }


            while (mHost->mWriteQueue.IsEmpty()) {
                if (mHost->mNextWriteTime != 0) {
                    mHost->mNextWriteTime = 0; // idle.
                    mHost->NotifyAll(); // wake up flush() if needed.
                }

                if (DEBUG) Slogger::D(TAG, "LazyTaskWriter: waiting indefinitely.");
                mHost->Wait();

                // Invariant: mNextWriteTime is either FLUSH_QUEUE or PRE_WRITE_DELAY_MS
                // from now.
            }
            item = *mHost->mWriteQueue.Begin();
            mHost->mWriteQueue.PopFront();

            Int64 now = SystemClock::GetUptimeMillis();
            if (DEBUG) Slogger::D(TAG, "LazyTaskWriter: now=%lld mNextWriteTime=%lld mWriteQueue.size=%d",
                now, mHost->mNextWriteTime, mHost->mWriteQueue.GetSize());
            while (now < mHost->mNextWriteTime) {
                if (DEBUG) Slogger::D(TAG, "LazyTaskWriter: waiting %lld",
                        (mHost->mNextWriteTime - now));
                mHost->Wait(mHost->mNextWriteTime - now);
                now = SystemClock::GetUptimeMillis();
            }
            // Got something to do.
        }

        if (item->mType == WriteQueueItem::TYPE_IMAGE) {
            ImageWriteQueueItem* imageWriteQueueItem = (ImageWriteQueueItem*)item.Get();
            String filename = imageWriteQueueItem->mFilename;
            AutoPtr<IBitmap> bitmap = imageWriteQueueItem->mImage;
            if (DEBUG) Slogger::D(TAG, "writing bitmap: filename=%s", filename.string());
            AutoPtr<IOutputStream> imageFile;
            ECode ec;
            do {
                AutoPtr<IFile> file;
                ec = CFile::New(sImagesDir, filename, (IFile**)&file);
                if (FAILED(ec))
                    break;
                ec = CFileOutputStream::New(file, (IOutputStream**)&imageFile);
                if (FAILED(ec))
                    break;
                Boolean res;
                ec = bitmap->Compress(BitmapCompressFormat_PNG, 100, imageFile, &res);
            } while(0);

            if (FAILED(ec)) {
                Slogger::E(TAG, "saveImage: unable to save %s, 0x%08x", filename.string(), ec);
            }

            if (imageFile != NULL) {
                ICloseable::Probe(imageFile)->Close();
            }
        }
        else if (item->mType == WriteQueueItem::TYPE_TASK) {
            // Write out one task.
            AutoPtr<IStringWriter> stringWriter;
            TaskRecord* task = ((TaskWriteQueueItem*)item.Get())->mTask;
            if (DEBUG) Slogger::D(TAG, "Writing task=%s", task->ToString().string());
            synchronized (service) {
                if (task->mInRecents) {
                    // Still there.
                    if (DEBUG) Slogger::D(TAG, "Saving task=%s", task->ToString().string());
                    mHost->SaveToXml(task, (IStringWriter**)&stringWriter);
                }
            }
            if (stringWriter != NULL) {
                // Write out xml file while not holding mService lock.
                AutoPtr<IFileOutputStream> file;
                AutoPtr<IAtomicFile> atomicFile;
                ECode ec;
                do {
                    StringBuilder sb;
                    sb += task->mTaskId;
                    sb += RECENTS_FILENAME;
                    sb += TASK_EXTENSION;
                    AutoPtr<IFile> f;
                    ec = CFile::New(sTasksDir, sb.ToString(), (IFile**)&f);
                    if (FAILED(ec))
                        break;
                    ec = CAtomicFile::New(f, (IAtomicFile**)&atomicFile);
                    if (FAILED(ec))
                        break;
                    ec = atomicFile->StartWrite((IFileOutputStream**)&file);
                    if (FAILED(ec))
                        break;
                    String str;
                    IObject::Probe(stringWriter)->ToString(&str);
                    ec = IOutputStream::Probe(file)->Write(str.GetBytes());
                    if (FAILED(ec))
                        break;
                    ec = IOutputStream::Probe(file)->Write('\n');
                    if (FAILED(ec))
                        break;
                    ec = atomicFile->FinishWrite(file);
                } while(0);

                if (FAILED(ec)) {
                    if (file != NULL) {
                        atomicFile->FailWrite(file);
                    }
                    String str;
                    IObject::Probe(atomicFile)->ToString(&str);
                    Slogger::E(TAG, "Unable to open %s for persisting. 0x%08x",
                        str.string(), ec);
                }
            }
        }
    }
}

TaskPersister::TaskPersister(
    /* [in] */ IFile* systemDir,
    /* [in] */ ActivityStackSupervisor* stackSupervisor)
    : mStackSupervisor(stackSupervisor)
    , mNextWriteTime(0)
{
    sTasksDir = NULL;
    CFile::New(systemDir, TASKS_DIRNAME, (IFile**)&sTasksDir);
    Boolean exists;
    sTasksDir->Exists(&exists);
    if (!exists) {
        String str;
        IObject::Probe(sTasksDir)->ToString(&str);
        if (DEBUG) Slogger::D(TAG, "Creating tasks directory %s", str.string());
        Boolean res;
        sTasksDir->Mkdir(&res);
        if (!res) {
            Slogger::E(TAG, "Failure creating tasks directory %s", str.string());
        }
    }

    sImagesDir = NULL;
    CFile::New(systemDir, IMAGES_DIRNAME, (IFile**)&sImagesDir);
    sImagesDir->Exists(&exists);
    if (!exists) {
        String str;
        IObject::Probe(sImagesDir)->ToString(&str);
        if (DEBUG) Slogger::D(TAG, "Creating images directory %s", str.string());
        Boolean res;
        sImagesDir->Mkdir(&res);
        if (!res) {
            Slogger::E(TAG, "Failure creating images directory %s", str.string());
        }
    }

    mService = stackSupervisor->mService;

    mLazyTaskWriterThread = new LazyTaskWriterThread(String("LazyTaskWriterThread"), this);
}

void TaskPersister::StartPersisting()
{
    mLazyTaskWriterThread->Start();
}

void TaskPersister::RemoveThumbnails(
    /* [in] */ TaskRecord* task)
{
    String taskString = StringUtils::ToString(task->mTaskId);
    List<AutoPtr<WriteQueueItem> >::ReverseIterator riter;
    for (riter = mWriteQueue.RBegin(); riter != mWriteQueue.REnd();) {
        AutoPtr<WriteQueueItem> item = *riter;
        if (item->mType == WriteQueueItem::TYPE_IMAGE &&
            ((ImageWriteQueueItem*)item.Get())->mFilename.StartWith(taskString)) {
            if (DEBUG) Slogger::D(TAG, "Removing %s from write queue",
                ((ImageWriteQueueItem*)item.Get())->mFilename.string());
            List<AutoPtr<WriteQueueItem> >::ReverseIterator tmpIt(riter);
            mWriteQueue.Erase((++tmpIt).GetBase());
        }
        else {
            ++riter;
        }
    }
}

void TaskPersister::YieldIfQueueTooDeep()
{
    Boolean stall = FALSE;
    synchronized (this) {
        if (mNextWriteTime == FLUSH_QUEUE) {
            stall = TRUE;
        }
    }
    if (stall) {
        Thread::Yield();
    }
}

void TaskPersister::Wakeup(
    /* [in] */ TaskRecord* task,
    /* [in] */ Boolean flush)
{
    synchronized (this) {
        if (task != NULL) {
            List<AutoPtr<WriteQueueItem> >::ReverseIterator riter;
            for (riter = mWriteQueue.RBegin(); riter != mWriteQueue.REnd(); ++riter) {
                AutoPtr<WriteQueueItem> item = *riter;
                if (item->mType == WriteQueueItem::TYPE_TASK &&
                    ((TaskWriteQueueItem*)item.Get())->mTask.Get() == task) {
                    if (!task->mInRecents) {
                        // This task is being removed.
                        RemoveThumbnails(task);
                    }
                    break;
                }
            }
            if (riter == mWriteQueue.REnd()) {
                mWriteQueue.PushBack(new TaskWriteQueueItem(task));
            }
        }
        else {
            // Dummy.
            mWriteQueue.PushBack(new WriteQueueItem(-1));
        }
        if (flush || mWriteQueue.GetSize() > MAX_WRITE_QUEUE_LENGTH) {
            mNextWriteTime = FLUSH_QUEUE;
        }
        else if (mNextWriteTime == 0) {
            mNextWriteTime = SystemClock::GetUptimeMillis() + PRE_TASK_DELAY_MS;
        }
        if (DEBUG) Slogger::D(TAG, "wakeup: task=%s flush=%d mNextWriteTime=%lld"
            " mWriteQueue.size=%d Callers=", task->ToString().string(), flush,
            mNextWriteTime, mWriteQueue.GetSize()/*, Debug::GetCallers(4)*/);
        NotifyAll();
    }

    YieldIfQueueTooDeep();
}

void TaskPersister::Flush()
{
    synchronized (this) {
        mNextWriteTime = FLUSH_QUEUE;
        NotifyAll();
        do {
            Wait();
        } while (mNextWriteTime == FLUSH_QUEUE);
    }
}

void TaskPersister::SaveImage(
    /* [in] */ IBitmap* image,
    /* [in] */ const String& filename)
{
    synchronized (this) {
        Int32 queueNdx;
        List<AutoPtr<WriteQueueItem> >::ReverseIterator riter;
        for (riter = mWriteQueue.RBegin(); riter != mWriteQueue.REnd(); ++riter) {
            AutoPtr<WriteQueueItem> item = *riter;
            if (item->mType == WriteQueueItem::TYPE_IMAGE) {
                ImageWriteQueueItem* imageWriteQueueItem = (ImageWriteQueueItem*)item.Get();
                if (imageWriteQueueItem->mFilename.Equals(filename)) {
                    // replace the Bitmap with the new one.
                    imageWriteQueueItem->mImage = image;
                    break;
                }
            }
        }
        if (riter == mWriteQueue.REnd()) {
            mWriteQueue.PushBack(new ImageWriteQueueItem(filename, image));
        }
        if (mWriteQueue.GetSize() > MAX_WRITE_QUEUE_LENGTH) {
            mNextWriteTime = FLUSH_QUEUE;
        }
        else if (mNextWriteTime == 0) {
            mNextWriteTime = SystemClock::GetUptimeMillis() + PRE_TASK_DELAY_MS;
        }
        if (DEBUG) Slogger::D(TAG, "saveImage: filename=%s now=%lld mNextWriteTime=%lld Callers=",
            filename.string(), SystemClock::GetUptimeMillis(), mNextWriteTime/*, Debug::GetCallers(4)*/);
        NotifyAll();
    }

    YieldIfQueueTooDeep();
}

AutoPtr<IBitmap> TaskPersister::GetTaskDescriptionIcon(
    /* [in] */ const String& filename)
{
    // See if it is in the write queue
    AutoPtr<IBitmap> icon = GetImageFromWriteQueue(filename);
    if (icon != NULL) {
        return icon;
    }
    return RestoreImage(filename);
}

AutoPtr<IBitmap> TaskPersister::GetImageFromWriteQueue(
    /* [in] */ const String& filename)
{
    synchronized (this) {
        List<AutoPtr<WriteQueueItem> >::ReverseIterator riter;
        for (riter = mWriteQueue.RBegin(); riter != mWriteQueue.REnd(); ++riter) {
            AutoPtr<WriteQueueItem> item = *riter;
            if (item->mType == WriteQueueItem::TYPE_IMAGE) {
                ImageWriteQueueItem* imageWriteQueueItem = (ImageWriteQueueItem*)item.Get();
                if (imageWriteQueueItem->mFilename.Equals(filename)) {
                    return imageWriteQueueItem->mImage;
                }
            }
        }
        return NULL;
    }
}

ECode TaskPersister::SaveToXml(
    /* [in] */ TaskRecord* task,
    /* [out] */ IStringWriter** writer)
{
    VALIDATE_NOT_NULL(writer)
    if (DEBUG) Slogger::D(TAG, "saveToXml: task=%s", task->ToString().string());
    AutoPtr<IXmlSerializer> xmlSerializer;
    CFastXmlSerializer::New((IXmlSerializer**)&xmlSerializer);
    CStringWriter::New(writer);
    xmlSerializer->SetOutput(IWriter::Probe(*writer));

    if (DEBUG) xmlSerializer->SetFeature(
            String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE);

    // save task
    FAIL_RETURN(xmlSerializer->StartDocument(String(NULL), TRUE));

    FAIL_RETURN(xmlSerializer->WriteStartTag(String(NULL), TAG_TASK));
    FAIL_RETURN(task->SaveToXml(xmlSerializer));
    FAIL_RETURN(xmlSerializer->WriteEndTag(String(NULL), TAG_TASK));

    FAIL_RETURN(xmlSerializer->EndDocument());
    FAIL_RETURN(IFlushable::Probe(xmlSerializer)->Flush());

    return NOERROR;
}

String TaskPersister::FileToString(
    /* [in] */ IFile* file)
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String newline;
    system->GetLineSeparator(&newline);
    do {
        AutoPtr<IReader> freader;
        if (FAILED(CFileReader::New(file, (IReader**)&freader)))
            break;
        AutoPtr<IBufferedReader> reader;
        if (FAILED(CBufferedReader::New(freader, (IBufferedReader**)&reader)))
            break;
        Int64 length;
        file->GetLength(&length);
        StringBuffer sb(length * 2);
        String line;
        while (reader->ReadLine(&line), line != NULL) {
            sb.Append(line);
            sb.Append(newline);
        }
        ICloseable::Probe(reader)->Close();
        return sb.ToString();
    } while(0);

    String name;
    file->GetName(&name);
    Slogger::E(TAG, "Couldn't read file %s", name.string());
    return String(NULL);
}

AutoPtr<TaskRecord> TaskPersister::TaskIdToTask(
    /* [in] */ Int32 taskId,
    /* [in] */ List<AutoPtr<TaskRecord> >* tasks)
{
    if (taskId < 0 || tasks == NULL) {
        return NULL;
    }
    List<AutoPtr<TaskRecord> >::ReverseIterator riter;
    for (riter = tasks->RBegin(); riter != tasks->REnd(); ++riter) {
        AutoPtr<TaskRecord> task = *riter;
        if (task->mTaskId == taskId) {
            return task;
        }
    }
    Slogger::E(TAG, "Restore affiliation error looking for taskId=%d", taskId);
    return NULL;
}

AutoPtr<List<AutoPtr<TaskRecord> > > TaskPersister::RestoreTasksLocked()
{
    AutoPtr<List<AutoPtr<TaskRecord> > > tasks = new List<AutoPtr<TaskRecord> >();
    AutoPtr<Set<Int32> > recoveredTaskIds = new Set<Int32>();

    AutoPtr<ArrayOf<IFile*> > recentFiles;
    sTasksDir->ListFiles((ArrayOf<IFile*>**)&recentFiles);
    if (recentFiles == NULL) {
        String str;
        IObject::Probe(sTasksDir)->ToString(&str);
        Slogger::E(TAG, "Unable to list files from %s", str.string());
        return tasks;
    }

    for (Int32 taskNdx = 0; taskNdx < recentFiles->GetLength(); ++taskNdx) {
        AutoPtr<IFile> taskFile = (*recentFiles)[taskNdx];
        if (DEBUG) {
            String name;
            taskFile->GetName(&name);
            Slogger::D(TAG, "restoreTasksLocked: taskFile=%s", name.string());
        }
        AutoPtr<IBufferedReader> reader;
        Boolean deleteFile = FALSE;
        ECode ec;
        do {
            AutoPtr<IReader> freader;
            ec = CFileReader::New(taskFile, (IReader**)&freader);
            if (FAILED(ec))
                break;
            ec = CBufferedReader::New(freader, (IBufferedReader**)&reader);
            if (FAILED(ec))
                break;

            AutoPtr<IXmlPullParser> in;
            Xml::NewPullParser((IXmlPullParser**)&in);
            in->SetInput(IReader::Probe(reader));

            Int32 event;
            while (((in->Next(&event), event) != IXmlPullParser::END_DOCUMENT) &&
                    event != IXmlPullParser::END_TAG) {
                String name;
                ec = in->GetName(&name);
                if (FAILED(ec))
                    break;
                if (event == IXmlPullParser::START_TAG) {
                    if (DEBUG) Slogger::D(TAG, "restoreTasksLocked: START_TAG name=%s", name.string());
                    if (TAG_TASK.Equals(name)) {
                        AutoPtr<TaskRecord> task;
                        ec = TaskRecord::RestoreFromXml(in, mStackSupervisor, (TaskRecord**)&task);
                        if (FAILED(ec))
                            break;
                        if (DEBUG) Slogger::D(TAG, "restoreTasksLocked: restored task=%s",
                                        task->ToString().string());
                        if (task != NULL) {
                            task->mIsPersistable = TRUE;
                            // XXX Don't add to write queue... there is no reason to write
                            // out the stuff we just read, if we don't write it we will
                            // read the same thing again.
                            //mWriteQueue.PushBack(new TaskWriteQueueItem(task));
                            tasks->PushBack(task);
                            Int32 taskId = task->mTaskId;
                            recoveredTaskIds->Insert(taskId);
                            assert(0);
                            // mStackSupervisor->SetNextTaskId(taskId);
                        }
                        else {
                            String str;
                            IObject::Probe(taskFile)->ToString(&str);
                            Slogger::E(TAG, "Unable to restore taskFile=%s: %s",
                                str.string(), FileToString(taskFile).string());
                        }
                    }
                    else {
                        Slogger::W/*Wtf*/(TAG, "restoreTasksLocked Unknown xml event=%d name=%s",
                            event, name.string());
                    }
                }
                ec = XmlUtils::SkipCurrentTag(in);
            }
        } while (0);
        if (FAILED(ec)) {
            String str;
            IObject::Probe(taskFile)->ToString(&str);
            Slogger::W/*Wtf*/(TAG, "Unable to parse%s. Error 0x%08x", str.string(), ec);
            Slogger::E(TAG, "Failing file: %s", FileToString(taskFile).string());
            deleteFile = TRUE;
        }

        if (reader != NULL) {
            ICloseable::Probe(reader)->Close();
        }
        if (!DEBUG && deleteFile) {
            String name;
            taskFile->GetName(&name);
            if (TRUE || DEBUG) Slogger::D(TAG, "Deleting file=%s", name.string());
            taskFile->Delete();
        }
    }

    if (!DEBUG) {
        RemoveObsoleteFiles(recoveredTaskIds);
    }

    // Fixup task affiliation from taskIds
    List<AutoPtr<TaskRecord> >::ReverseIterator riter;
    for (riter = tasks->RBegin(); riter != tasks->REnd(); ++riter) {
        AutoPtr<TaskRecord> task = *riter;
        task->SetPrevAffiliate(TaskIdToTask(task->mPrevAffiliateTaskId, tasks));
        task->SetNextAffiliate(TaskIdToTask(task->mNextAffiliateTaskId, tasks));
    }

    tasks->Sort(CompareTaskRecord);
    return tasks;
}

Int32 TaskPersister::CompareTaskRecord(
    /* [in] */ TaskRecord* lhs,
    /* [in] */ TaskRecord* rhs)
{
    Int64 diff = rhs->mLastTimeMoved - lhs->mLastTimeMoved;
    if (diff < 0) {
        return -1;
    }
    else if (diff > 0) {
        return +1;
    }
    else {
        return 0;
    }
}

void TaskPersister::RemoveObsoleteFiles(
    /* [in] */ Set<Int32>* persistentTaskIds,
    /* [in] */ ArrayOf<IFile*>* files)
{
    // if (DEBUG) Slogger::D(TAG, "removeObsoleteFile: persistentTaskIds=" + persistentTaskIds +
    //         " files=" + files);
    if (files == NULL) {
        Slogger::E(TAG, "File error accessing recents directory (too many files open?).");
        return;
    }
    for (Int32 fileNdx = 0; fileNdx < files->GetLength(); ++fileNdx) {
        AutoPtr<IFile> file = (*files)[fileNdx];
        String filename;
        file->GetName(&filename);
        Int32 taskIdEnd = filename.IndexOf('_');
        if (taskIdEnd > 0) {
            Int32 taskId;
            // try {
                taskId = StringUtils::ParseInt32(filename.Substring(0, taskIdEnd));
                if (DEBUG) Slogger::D(TAG, "removeObsoleteFile: Found taskId=%d", taskId);
            // } catch (Exception e) {
            //     Slogger::W/*Wtf*/(TAG, "removeObsoleteFile: Can't parse file=" + file.getName());
            //     file.delete();
            //     continue;
            // }
            if (persistentTaskIds->Find(taskId) == persistentTaskIds->End()) {
                if (TRUE || DEBUG) Slogger::D(TAG, "removeObsoleteFile: deleting file=%s", filename.string());
                file->Delete();
            }
        }
    }
}

void TaskPersister::RemoveObsoleteFiles(
    /* [in] */ Set<Int32>* persistentTaskIds)
{
    AutoPtr<ArrayOf<IFile*> > files;
    sTasksDir->ListFiles((ArrayOf<IFile*>**)&files);
    RemoveObsoleteFiles(persistentTaskIds, files);
    files = NULL;
    sImagesDir->ListFiles((ArrayOf<IFile*>**)&files);
    RemoveObsoleteFiles(persistentTaskIds, files);
}

AutoPtr<IBitmap> TaskPersister::RestoreImage(
    /* [in] */ const String& filename)
{
    if (DEBUG) Slogger::D(TAG, "restoreImage: restoring %s", filename.string());
    AutoPtr<IBitmapFactory> factory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
    StringBuilder sb;
    sb += sImagesDir;
    sb += "/"; //IFile::separator;
    sb += filename;
    AutoPtr<IBitmap> bitmap;
    factory->DecodeFile(sb.ToString(), (IBitmap**)&bitmap);
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
