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

#include "elastos/droid/server/am/CUsageStatsService.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Binder.h"
#include "util/Xml.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <binder/Parcel.h>

using Elastos::Core::CBoolean;
using Elastos::Core::IBoolean;
using Elastos::Core::CInteger64;
using Elastos::Core::IInteger64;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Internal::App::EIID_IIUsageStats;
using Elastos::Droid::Internal::App::IIUsageStats;
using Elastos::Droid::Internal::Os::CPkgUsageStats;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::CFastXmlSerializer;
using Elastos::Droid::Utility::IFastXmlSerializer;
using Elastos::Droid::Utility::Xml;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger64;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::IObjectStringMap;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::Logging::Slogger;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {


//===============================================================
// CUsageStatsService::TimeStats
//===============================================================
CUsageStatsService::TimeStats::TimeStats()
    : mCount(0)
{
    mTimes = ArrayOf<Int32>::Alloc(NUM_LAUNCH_TIME_BINS);
}

CUsageStatsService::TimeStats::TimeStats(
    /* [in] */ IParcel* in)
{
    TimeStats();
    in->ReadInt32(&mCount);
    AutoPtr<ArrayOf<Int32> > localTimes = mTimes;
    for (Int32 i = 0; i < NUM_LAUNCH_TIME_BINS; i++) {
        in->ReadInt32(&(*localTimes)[i]);
    }
}

void CUsageStatsService::TimeStats::IncCount()
{
    mCount++;
}

void CUsageStatsService::TimeStats::Add(
    /* [in] */ Int32 val)
{
    AutoPtr<ArrayOf<Int32> > bins = LAUNCH_TIME_BINS;
    for (Int32 i = 0; i < NUM_LAUNCH_TIME_BINS - 1; i++) {
        if (val < (*bins)[i]) {
            (*mTimes)[i]++;
            return;
        }
    }
    (*mTimes)[NUM_LAUNCH_TIME_BINS - 1]++;
}

ECode CUsageStatsService::TimeStats::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteInt32(mCount);
    AutoPtr<ArrayOf<Int32> > localTimes = mTimes;
    for (Int32 i = 0; i < NUM_LAUNCH_TIME_BINS; i++) {
        out->WriteInt32((*localTimes)[i]);
    }
    return NOERROR;
}


//===============================================================
// CUsageStatsService::PkgUsageStatsExtended
//===============================================================
CUsageStatsService::PkgUsageStatsExtended::PkgUsageStatsExtended(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mLaunchCount);
    in->ReadInt64(&mUsageTime);
    if (mLocalLOGV) Slogger::V(TAG, "Launch count: %d, Usage time:%d", mLaunchCount,
            mUsageTime);

    Int32 numTimeStats;
    in->ReadInt32(&numTimeStats);
    if (mLocalLOGV) Slogger::V(TAG, "Reading comps: %d", numTimeStats);
    for (Int32 i = 0; i < numTimeStats; i++) {
        String comp;
        in->ReadString(&comp);
        if (mLocalLOGV) Slogger::V(TAG, "Component: %s", comp.string());
        AutoPtr<TimeStats> times = new TimeStats(in);
        mLaunchTimes[comp] = times;
    }
}

void CUsageStatsService::PkgUsageStatsExtended::UpdateResume(
    /* [in] */ const String& comp,
    /* [in] */ Boolean launched)
{
    if (launched) {
        mLaunchCount ++;
    }
    mResumedTime = SystemClock::GetElapsedRealtime();
}

void CUsageStatsService::PkgUsageStatsExtended::UpdatePause()
{
    mPausedTime =  SystemClock::GetElapsedRealtime();
    mUsageTime += (mPausedTime - mResumedTime);
}

void CUsageStatsService::PkgUsageStatsExtended::AddLaunchCount(
    /* [in] */ const String& comp)
{
    AutoPtr<TimeStats> times = mLaunchTimes[comp];
    if (times == NULL) {
        times = new TimeStats();
        mLaunchTimes[comp] = times;
    }
    times->IncCount();
}

void CUsageStatsService::PkgUsageStatsExtended::AddLaunchTime(
    /* [in] */ const String& comp,
    /* [in] */ Int32 millis)
{
    AutoPtr<TimeStats> times = mLaunchTimes[comp];
    if (times == NULL) {
        times = new TimeStats();
        mLaunchTimes[comp] = times;
    }
    times->Add(millis);
}

void CUsageStatsService::PkgUsageStatsExtended::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteInt32(mLaunchCount);
    out->WriteInt64(mUsageTime);
    const Int32 numTimeStats = mLaunchTimes.GetSize();
    out->WriteInt32(numTimeStats);
    if (numTimeStats > 0) {
        HashMap<String, AutoPtr<TimeStats> >::Iterator it;
        for (it = mLaunchTimes.Begin(); it != mLaunchTimes.End(); ++it) {
            out->WriteString(it->mFirst);
            AutoPtr<TimeStats> times = it->mSecond;
            times->WriteToParcel(out);
        }
    }
}

void CUsageStatsService::PkgUsageStatsExtended::Clear()
{
    mLaunchTimes.Clear();
    mLaunchCount = 0;
    mUsageTime = 0;
}


//===============================================================
// CUsageStatsService::WriteStatsToFileThread
//===============================================================
CUsageStatsService::WriteStatsToFileThread::WriteStatsToFileThread(
    /* [in] */ CUsageStatsService* host)
    : mHost(host)
{
    Thread::constructor(String("CUsageStatsService_DiskWriter"));
}

ECode CUsageStatsService::WriteStatsToFileThread::Run()
{
    // try {
    if (mHost->mLocalLOGV) Slogger::D(TAG, "Disk writer thread starting.");
    mHost->WriteStatsToFile(TRUE, FALSE);
    // } finally {
    mHost->mUnforcedDiskWriteRunning->Set(FALSE);
    if (mHost->mLocalLOGV) Slogger::D(TAG, "Disk writer thread ending.");
    // }
    return NOERROR;
}


//===============================================================
// CUsageStatsService::LocalPackageMonitor
//===============================================================
ECode CUsageStatsService::LocalPackageMonitor::OnPackageRemovedAllUsers(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    AutoLock lock(mHost->mStatsLock);
    mHost->mLastResumeTimes.Erase(packageName);
    return NOERROR;
}


//===============================================================
// CUsageStatsService
//===============================================================
static AutoPtr< ArrayOf<Int32> > InitLaunchTimeBins()
{
    AutoPtr< ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(9);
    (*array)[0] = 250;
    (*array)[1] = 500;
    (*array)[2] = 750;
    (*array)[3] = 1000;
    (*array)[4] = 1500;
    (*array)[5] = 2000;
    (*array)[6] = 3000;
    (*array)[7] = 4000;
    (*array)[8] = 5000;
    return array;
}

const String CUsageStatsService::SERVICE_NAME = String("usagestats");
const Boolean CUsageStatsService::mLocalLOGV;
const Boolean CUsageStatsService::REPORT_UNEXPECTED;
const String CUsageStatsService::TAG("UsageStats");

const Int32 CUsageStatsService::VERSION;
const Int32 CUsageStatsService::CHECKIN_VERSION;
const String CUsageStatsService::FILE_PREFIX = String("usage-");
const String CUsageStatsService::FILE_HISTORY = String("usage-history.xml");
const Int32 CUsageStatsService::FILE_WRITE_INTERVAL;
const Int32 CUsageStatsService::MAX_NUM_FILES;
const Int32 CUsageStatsService::NUM_LAUNCH_TIME_BINS;

const AutoPtr< ArrayOf<Int32> > CUsageStatsService::LAUNCH_TIME_BINS = InitLaunchTimeBins();

AutoPtr<IIUsageStats> CUsageStatsService::sService;

CUsageStatsService::~CUsageStatsService()
{
    mStats.Clear();
    mLastResumeTimes.Clear();
}

ECode CUsageStatsService::constructor(
    /* [in] */ const String& dir)
{
    CAtomicInteger32::New(-1, (IAtomicInteger32**)&mLastWriteDay);
    CAtomicInteger64::New(0, (IAtomicInteger64**)&mLastWriteElapsedTime);
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mUnforcedDiskWriteRunning);

    CFile::New(dir, (IFile**)&mDir);
    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    AutoPtr<ITimeZone> tz;
    tzHelper->GetTimeZone(String("GMT+0"), (ITimeZone**)&tz);
    AutoPtr<ICalendarHelper> cHelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&cHelper);
    cHelper->GetInstance(tz, (ICalendar**)&mCal);

    Boolean maked;
    mDir->Mkdir(&maked);

    // Remove any old usage files from previous versions.
    AutoPtr<IFile> parentDir;
    mDir->GetParentFile((IFile**)&parentDir);
    AutoPtr<ArrayOf<String> > fList;
    parentDir->List((ArrayOf<String>**)&fList);
    if (fList != NULL) {
        String dirName;
        mDir->GetName(&dirName);
        String prefix = dirName + ".";
        Int32 i = fList->GetLength();
        while (i > 0) {
            i--;
            if ((*fList)[i].StartWith(prefix)) {
                Slogger::I(TAG, "Deleting old usage file: %s", (*fList)[i].string());
                AutoPtr<IFile> file;
                CFile::New(parentDir, (*fList)[i], (IFile**)&file);
                Boolean deleted;
                file->Delete(&deleted);
            }
        }
    }

    // Update current stats which are binned by date
    mFileLeaf = GetCurrentDateStr(FILE_PREFIX);
    CFile::New(mDir, mFileLeaf, (IFile**)&mFile);
    AutoPtr<IFile> hFile;
    CFile::New(mDir, FILE_HISTORY, (IFile**)&hFile);
    CAtomicFile::New(hFile, (IAtomicFile**)&mHistoryFile);
    ReadStatsFromFile();
    ReadHistoryStatsFromFile();

    mLastWriteElapsedTime->Set(SystemClock::GetElapsedRealtime());
    // mCal was set by getCurrentDateStr(), want to use that same time.
    Int32 value;
    mCal->Get(ICalendar::DAY_OF_YEAR, &value);
    mLastWriteDay->Set(value);
    return NOERROR;
}

String CUsageStatsService::GetCurrentDateStr(
        /* [in] */ const String& prefix)
{
    StringBuilder sb;
    {
        AutoLock lock(mCalLock);
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 now;
        system->GetCurrentTimeMillis(&now);
//        mCal->SetTimeInMillis(now);
        if (!prefix.IsNull()) {
            sb.Append(prefix);
        }
        Int32 year = 0;
//        mCal->Get(ICalendar::YEAR, &year);
        sb.Append(year);
        Int32 month = 0;
//        mCal->Get(ICalendar::MONTH, &month);
        Int32 mm = month - ICalendar::JANUARY +1;
        if (mm < 10) {
            sb.Append("0");
        }
        sb.Append(mm);
        Int32 dd = 0;
//        mCal->Get(ICalendar::DAY_OF_MONTH, &dd);
        if (dd < 10) {
            sb.Append("0");
        }
        sb.Append(dd);
    }
    return sb.ToString();
}

AutoPtr<IParcel> CUsageStatsService::GetParcelForFile(
    /* [in] */ IFile* file)
{
    AutoPtr<IFileInputStream> stream;
    CFileInputStream::New(file, (IFileInputStream**)&stream);
    AutoPtr<ArrayOf<Byte> > raw = ReadFully(stream);
    AutoPtr<IParcel> in;
    CParcel::New((IParcel**)&in);
    if (raw != NULL) {
        android::Parcel* nativeParcel;
        in->GetElementPayload((Handle32*)&nativeParcel);
        Int32 length = raw->GetLength();
        nativeParcel->setDataSize(length);
        nativeParcel->setDataPosition(0);

        void* r = nativeParcel->writeInplace(length);
        memcpy(r, raw->GetPayload(), length);
    }
    in->SetDataPosition(0);
    stream->Close();
    return in;
}

void CUsageStatsService::ReadStatsFromFile()
{
    AutoPtr<IFile> newFile = mFile;
    {
        AutoLock lock(mFileLock);
    //     try {
        Boolean exist;
        newFile->Exists(&exist);
        if (exist) {
            ReadStatsFLOCK(newFile);
        }
        else {
            // Check for file limit before creating a new file
            CheckFileLimitFLOCK();
            Boolean created;
            newFile->CreateNewFile(&created);
        }
    //     } catch (IOException e) {
    //         Slog.w(TAG,"Error : " + e + " reading data from file:" + newFile);
    //     }
    }
}

void CUsageStatsService::ReadStatsFLOCK(
    /* [in] */ IFile* file)
{
    AutoPtr<IParcel> in = GetParcelForFile(file);
    if (in == NULL) {
        Slogger::E(TAG, "Failed to read parcel from file.");
        return;
    }

    Int32 vers;
    in->ReadInt32(&vers);
    if (vers != VERSION) {
        Slogger::W(TAG, "Usage stats version changed; dropping");
        return;
    }
    Int32 N;
    in->ReadInt32(&N);
    while (N > 0) {
        N--;
        String pkgName;
        in->ReadString(&pkgName);
        if (pkgName.IsNull()) {
            break;
        }
        if (mLocalLOGV) Slogger::V(TAG, "Reading package #%d: %s", N, pkgName.string());
        AutoPtr<PkgUsageStatsExtended> pus = new PkgUsageStatsExtended(in);
        {
            AutoLock lock(mStatsLock);
            mStats[pkgName] = pus;
        }
    }
}

void CUsageStatsService::ReadHistoryStatsFromFile()
{
    AutoLock lock(mFileLock);
    AutoPtr<IFile> bFile;
    mHistoryFile->GetBaseFile((IFile**)&bFile);
    Boolean exist;
    bFile->Exists(&exist);
    if (exist) {
        ReadHistoryStatsFLOCK(mHistoryFile);
    }
}

ECode CUsageStatsService::ReadHistoryStatsFLOCK(
    /* [in] */ IAtomicFile* file)
{
    AutoPtr<IFileInputStream> fis;
    // try {
    FAIL_RETURN(mHistoryFile->OpenRead((IFileInputStream**)&fis));
    AutoPtr<IXmlPullParser> parser = Xml::NewPullParser();
    parser->SetInput(IInputStream::Probe(fis), String(NULL));
    Int32 eventType;
    parser->GetEventType(&eventType);
    Int32 cycleCount = 10000;
    while ((eventType != IXmlPullParser::START_TAG)&& --cycleCount>0) {
        parser->Next(&eventType);
    }
    String tagName;
    parser->GetName(&tagName);
    if (tagName.Equals("usage-history")) {
        String pkg = String(NULL);
        do {
            parser->Next(&eventType);
            if (eventType == IXmlPullParser::START_TAG) {
                parser->GetName(&tagName);
                Int32 depth;
                parser->GetDepth(&depth);
                if (String("pkg").Equals(tagName) && depth == 2) {
                    parser->GetAttributeValue(String(NULL), String("name"), &pkg);
                } else if (String("comp").Equals(tagName) && depth == 3 && pkg != NULL) {
                    String comp;
                    parser->GetAttributeValue(String(NULL), String("name"), &comp);
                    String lastResumeTimeStr;
                    parser->GetAttributeValue(String(NULL), String("lrt"), &lastResumeTimeStr);
                    if (!comp.IsNull() && !lastResumeTimeStr.IsNull()) {
                        // try {
                        Int64 lastResumeTime = StringUtils::ParseInt64(lastResumeTimeStr);
                        {
                            AutoLock lock(mStatsLock);
                            LastResumeTimeMapIterator it = mLastResumeTimes.Find(pkg);
                            AutoPtr<StringInt64Map> lrt;
                            if (it == mLastResumeTimes.End()) {
                                lrt = new StringInt64Map();
                                mLastResumeTimes[pkg] = lrt;
                            } else {
                                lrt = it->mSecond;
                            }
                            (*lrt)[comp] = lastResumeTime;
                        }
                        // } catch (NumberFormatException e) {
                        // }
                    }
                }
            } else if (eventType == IXmlPullParser::END_TAG) {
                String pName;
                parser->GetName(&pName);
                if (pName.Equals("pkg")) {
                    pkg = String(NULL);
                }
            }
        } while (eventType != IXmlPullParser::END_DOCUMENT);
    }
    // } catch (XmlPullParserException e) {
    //     Slog.w(TAG,"Error reading history stats: " + e);
    // } catch (IOException e) {
    //     Slog.w(TAG,"Error reading history stats: " + e);
    // } finally {
    if (fis != NULL) {
        // try {
        fis->Close();
        // } catch (IOException e) {
        // }
    }
    // }
    return NOERROR;
}

AutoPtr<ArrayOf<String> > CUsageStatsService::GetUsageStatsFileListFLOCK()
{
    // Check if there are too many files in the system and delete older files
    AutoPtr<ArrayOf<String> > fList;
    mDir->List((ArrayOf<String>**)&fList);
    if (fList == NULL) {
        return NULL;
    }
    AutoPtr<ArrayOf<String> > fileList = ArrayOf<String>::Alloc(fList->GetLength());
    Int32 count = 0;
    for (Int32 i = 0; i < fList->GetLength(); ++i) {
        String file = (*fList)[i];
        if (!file.StartWith(FILE_PREFIX)) {
            continue;
        }
        if (file.EndWith(".bak")) {
            AutoPtr<IFile> f;
            CFile::New(mDir, file, (IFile**)&f);
            Boolean deleted;
            f->Delete(&deleted);
            continue;
        }
        fileList->Set(count, file);
        ++count;
    }
    AutoPtr<ArrayOf<String> > temp = ArrayOf<String>::Alloc(count);
    temp->Copy(fileList, count);
    return temp;
}

void CUsageStatsService::CheckFileLimitFLOCK()
{
    // Get all usage stats output files
    AutoPtr<ArrayOf<String> > fileList = GetUsageStatsFileListFLOCK();
    if (fileList == NULL) {
        // Strange but we dont have to delete any thing
        return;
    }
    Int32 count = fileList->GetLength();
    if (count <= MAX_NUM_FILES) {
        return;
    }
    // // Sort files
    // Collections.sort(fileList);
    count -= MAX_NUM_FILES;
    // Delete older files
    for (Int32 i = 0; i < count; i++) {
        String fileName = (*fileList)[i];
        AutoPtr<IFile> file;
        CFile::New(mDir, fileName, (IFile**)&file);
        Slogger::I(TAG, "Deleting usage file : %s", fileName.string());
        Boolean deleted;
        file->Delete(&deleted);
    }
}

void CUsageStatsService::WriteStatsToFile(
    /* [in] */ Boolean force,
    /* [in] */ Boolean forceWriteHistoryStats)
{
    Int32 curDay;
    {
        AutoLock lock(mCalLock);
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 millis;
        system->GetCurrentTimeMillis(&millis);
        mCal->SetTimeInMillis(millis);
        mCal->Get(ICalendar::DAY_OF_YEAR, &curDay);
    }
    Int32 day;
    mLastWriteDay->Get(&day);
    const Boolean dayChanged = curDay != day;

    // Determine if the day changed...  note that this will be wrong
    // if the year has changed but we are in the same day of year...
    // we can probably live with this.
    const Int64 currElapsedTime = SystemClock::GetElapsedRealtime();

    // Fast common path, without taking the often-contentious
    // mFileLock.
    if (!force) {
        Int64 timeValue;
        mLastWriteElapsedTime->Get(&timeValue);
        if (!dayChanged &&
            (currElapsedTime - timeValue) < FILE_WRITE_INTERVAL) {
            // wait till the next update
            return;
        }
        Boolean compare;
        mUnforcedDiskWriteRunning->CompareAndSet(FALSE, TRUE, &compare);
        if (compare) {
            AutoPtr<WriteStatsToFileThread> thread = new WriteStatsToFileThread(this);
            thread->Start();
        }
        return;
    }

    {
        AutoLock lock(mFileLock);
        // Get the most recent file
        mFileLeaf = GetCurrentDateStr(FILE_PREFIX);
        // Copy current file to back up
        AutoPtr<IFile> backupFile;
        Boolean exist = FALSE;
        if (mFile != NULL && (mFile->Exists(&exist), exist)) {
            String path;
            mFile->GetPath(&path);
            CFile::New(path + ".bak", (IFile**)&backupFile);
            Boolean backupExist;
            backupFile->Exists(&backupExist);
            if (!backupExist) {
                Boolean renamed;
                mFile->RenameTo(backupFile, &renamed);
                if (!renamed) {
                    Slogger::W(TAG, "Failed to persist new stats");
                    return;
                }
            } else {
                Boolean deleted;
                mFile->Delete(&deleted);
            }
        }

        // try {
            // Write mStats to file
            WriteStatsFLOCK(mFile);
            mLastWriteElapsedTime->Set(currElapsedTime);
            if (dayChanged) {
                mLastWriteDay->Set(curDay);
                // clear stats
                {
                    AutoLock lock(mStatsLock);
                    mStats.Clear();
                }
                mFile = NULL;
                CFile::New(mDir, mFileLeaf, (IFile**)&mFile);
                CheckFileLimitFLOCK();
            }

            if (dayChanged || forceWriteHistoryStats) {
                // Write history stats daily, or when forced (due to shutdown).
                WriteHistoryStatsFLOCK(mHistoryFile);
            }

            // Delete the backup file
            if (backupFile != NULL) {
                Boolean deleted;
                backupFile->Delete(&deleted);
            }
        // } catch (IOException e) {
        //     Slog.w(TAG, "Failed writing stats to file:" + mFile);
        //     if (backupFile != null) {
        //         mFile.delete();
        //         backupFile.renameTo(mFile);
        //     }
        // }
    }
    if (mLocalLOGV) Slogger::D(TAG, "Dumped usage stats.");
}

void CUsageStatsService::WriteStatsFLOCK(
    /* [in] */ IFile* file)
{
    AutoPtr<IFileOutputStream> stream;
    CFileOutputStream::New(file, (IFileOutputStream**)&stream);
    // try {
    // TODO:
    // AutoPtr<IParcel> out = Parcel.obtain();
    // WriteStatsToParcelFLOCK(out);
    // AutoPtr<ArrayOf<Byte> > value;
    // out->Marshall((ArrayOf<Byte>**)&value);
    // stream->Write(value);
    // out->Recycle();
    // IFlushable::Probe(stream)->Flush();
    // } finally {
    Boolean synced;
    FileUtils::Sync(stream, &synced);
    stream->Close();
    // }
}

void CUsageStatsService::WriteStatsToParcelFLOCK(
    /* [in] */ IParcel* out)
{
    AutoLock lock(mStatsLock);
    out->WriteInt32(VERSION);
    out->WriteInt32(mStats.GetSize());
    HashMap<String, AutoPtr<PkgUsageStatsExtended> >::Iterator it;
    for (it = mStats.Begin(); it != mStats.End(); ++it) {
        String key = it->mFirst;
        AutoPtr<PkgUsageStatsExtended> pus = it->mSecond;
        out->WriteString(key);
        pus->WriteToParcel(out);
    }
}

void CUsageStatsService::FilterHistoryStats()
{
    AutoLock lock(mStatsLock);
    // Copy and clear the last resume times map, then copy back stats
    // for all installed packages.
    LastResumeTimeMap tmpLastResumeTimes(mLastResumeTimes.Begin(), mLastResumeTimes.End());
    mLastResumeTimes.Clear();
    AutoPtr<IPackageManager> pkgManager;
    mContext->GetPackageManager((IPackageManager**)&pkgManager);
    AutoPtr<IObjectContainer> pkgInfos;
    pkgManager->GetInstalledPackages(0, (IObjectContainer**)&pkgInfos);
    AutoPtr<IObjectEnumerator> enumerator;
    pkgInfos->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    Boolean hasNext = FALSE;
    while (enumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IInterface> item;
        enumerator->Current((IInterface**)&item);
        AutoPtr<IPackageInfo> info = IPackageInfo::Probe(item);
        String infoPkgName;
        info->GetPackageName(&infoPkgName);
        LastResumeTimeMapIterator it = tmpLastResumeTimes.Find(infoPkgName);
        if (it != tmpLastResumeTimes.End()) {
            mLastResumeTimes[infoPkgName] = it->mSecond;
            it->mSecond = NULL;
        }
    }
}

void CUsageStatsService::WriteHistoryStatsFLOCK(
    /* [in] */ IAtomicFile* historyFile)
{
    AutoPtr<IFileOutputStream> fos;
    AutoPtr<IFastXmlSerializer> serializer;
    AutoPtr<IXmlSerializer> out;
    AutoPtr<IBoolean> b;
    // try {
    FAIL_GOTO(historyFile->StartWrite((IFileOutputStream**)&fos), EXCEPTION);
    CFastXmlSerializer::New((IFastXmlSerializer**)&serializer);
    out = IXmlSerializer::Probe(serializer);
    FAIL_GOTO(out->SetOutput(fos, String("utf-8")), EXCEPTION);
    CBoolean::New(TRUE, (IBoolean**)&b);
    FAIL_GOTO(out->StartDocument(String(NULL), b), EXCEPTION);
    FAIL_GOTO(out->SetFeature(String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE), EXCEPTION);
    FAIL_GOTO(out->WriteStartTag(String(NULL), String("usage-history")), EXCEPTION);
    {
        AutoLock lock(mStatsLock);
        LastResumeTimeMapIterator it;
        for (it = mLastResumeTimes.Begin(); it != mLastResumeTimes.End(); ++it) {
            FAIL_GOTO(out->WriteStartTag(String(NULL), String("pkg")), EXCEPTION);
            FAIL_GOTO(out->WriteAttribute(String(NULL), String("name"), it->mFirst), EXCEPTION);
            StringInt64MapIterator compIt;
            for (compIt = it->mSecond->Begin(); compIt != it->mSecond->End(); ++compIt) {
                FAIL_GOTO(out->WriteStartTag(String(NULL), String("comp")), EXCEPTION);
                FAIL_GOTO(out->WriteAttribute(String(NULL), String("name"), compIt->mFirst), EXCEPTION);
                FAIL_GOTO(out->WriteAttribute(String(NULL), String("lrt"), StringUtils::Int64ToString(compIt->mSecond)), EXCEPTION);
                FAIL_GOTO(out->WriteEndTag(String(NULL), String("comp")), EXCEPTION);
            }
            FAIL_GOTO(out->WriteEndTag(String(NULL), String("pkg")), EXCEPTION);
        }
    }
    FAIL_GOTO(out->WriteEndTag(String(NULL), String("usage-history")), EXCEPTION);
    FAIL_GOTO(out->EndDocument(), EXCEPTION);

    FAIL_GOTO(historyFile->FinishWrite(fos), EXCEPTION);
    return;

EXCEPTION:
    // } catch (IOException e) {
    Slogger::W(TAG,"Error writing history stats");
    if (fos != NULL) {
        historyFile->FailWrite(fos);
    }
}

ECode CUsageStatsService::Publish(
    /* [in] */ IContext* context)
{
    mContext = context;
    ServiceManager::AddService(SERVICE_NAME, this);
    return NOERROR;
}

ECode CUsageStatsService::MonitorPackages()
{
    mPackageMonitor = new LocalPackageMonitor(this);
    mPackageMonitor->Register(mContext, NULL, TRUE);
    FilterHistoryStats();
    return NOERROR;
}

ECode CUsageStatsService::Shutdown()
{
    if (mPackageMonitor != NULL) {
        mPackageMonitor->Unregister();
    }
    Slogger::I(TAG, "Writing usage stats before shutdown...");
    WriteStatsToFile(TRUE, TRUE);
    return NOERROR;
}

AutoPtr<IIUsageStats> CUsageStatsService::GetService()
{
    if (sService != NULL) {
        return sService;
    }
    AutoPtr<IServiceManager> sm;
    ASSERT_SUCCEEDED(CServiceManager::AcquireSingleton((IServiceManager**)&sm));
    assert(sm != NULL);
    AutoPtr<IInterface> b;
    sm->GetService(SERVICE_NAME, (IInterface**)&b);
    sService = IIUsageStats::Probe(b);
    return sService;
}

ECode CUsageStatsService::NoteResumeComponent(
    /* [in] */ IComponentName* componentName)
{
    FAIL_RETURN(EnforceCallingPermission());
    String pkgName;
    {
        AutoLock lock(mStatsLock);
        if ((componentName == NULL) ||
                (componentName->GetPackageName(&pkgName), pkgName.IsNull())) {
            return NOERROR;
        }

        const Boolean samePackage = pkgName.Equals(mLastResumedPkg);
        if (mIsResumed) {
            if (!mLastResumedPkg.IsNull()) {
                // We last resumed some other package...  just pause it now
                // to recover.
                if (REPORT_UNEXPECTED) Slogger::I(TAG, "Unexpected resume of %s while already resumed in %s",
                    pkgName.string(), mLastResumedPkg.string());
                AutoPtr<PkgUsageStatsExtended> pus;
                HashMap<String, AutoPtr<PkgUsageStatsExtended> >::Iterator it = mStats.Find(mLastResumedPkg);
                if (it != mStats.End()) pus = it->mSecond;
                if (pus != NULL) {
                    pus->UpdatePause();
                }
            }
        }

        String compClassName;
        componentName->GetClassName(&compClassName);
        const Boolean sameComp = samePackage && compClassName.Equals(mLastResumedComp);

        mIsResumed = TRUE;
        mLastResumedPkg = pkgName;
        mLastResumedComp = compClassName;

        if (mLocalLOGV) Slogger::I(TAG, "started component:%s", pkgName.string());
        AutoPtr<PkgUsageStatsExtended> pus = mStats[pkgName];
        if (pus == NULL) {
            pus = new PkgUsageStatsExtended();
            mStats[pkgName] = pus;
        }
        pus->UpdateResume(mLastResumedComp, !samePackage);
        if (!sameComp) {
            pus->AddLaunchCount(mLastResumedComp);
        }

        AutoPtr<StringInt64Map> componentResumeTimes = mLastResumeTimes[pkgName];
        if (componentResumeTimes == NULL) {
            componentResumeTimes = new StringInt64Map();
            mLastResumeTimes[pkgName] = componentResumeTimes;
        }

        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 now;
        system->GetCurrentTimeMillis(&now);
        (*componentResumeTimes)[mLastResumedComp] = now;
    }
    return NOERROR;
}

ECode CUsageStatsService::NotePauseComponent(
    /* [in] */ IComponentName* componentName)
{
    FAIL_RETURN(EnforceCallingPermission());

    {
        AutoLock lock(mStatsLock);
        String pkgName;
        if ((componentName == NULL) ||
                (componentName->GetPackageName(&pkgName), pkgName.IsNull())) {
            return NOERROR;
        }
        if (!mIsResumed) {
            if (REPORT_UNEXPECTED) Slogger::I(TAG, "Something wrong here, didn't expect %s to be paused",
                    pkgName.string());
            return NOERROR;
        }
        mIsResumed = FALSE;

        if (mLocalLOGV) Slogger::I(TAG, "paused component:%s", pkgName.string());

        AutoPtr<PkgUsageStatsExtended> pus;
        HashMap<String, AutoPtr<PkgUsageStatsExtended> >::Iterator it = mStats.Find(pkgName);
        if (it != mStats.End()) pus = it->mSecond;
        if (pus == NULL) {
            // Weird some error here
            Slogger::I(TAG, "No package stats for pkg:%s", pkgName.string());
            return NOERROR;
        }
        pus->UpdatePause();
    }

    // Persist current data to file if needed.
    WriteStatsToFile(FALSE, FALSE);
    return NOERROR;
}

ECode CUsageStatsService::NoteLaunchTime(
    /* [in] */ IComponentName* componentName,
    /* [in] */ Int32 millis)
{
    FAIL_RETURN(EnforceCallingPermission());
    String pkgName;
    if ((componentName == NULL) ||
            (componentName->GetPackageName(&pkgName), pkgName.IsNull())) {
        return NOERROR;
    }

    // Persist current data to file if needed.
    WriteStatsToFile(FALSE, FALSE);

    AutoLock lock(mStatsLock);
    HashMap<String, AutoPtr<PkgUsageStatsExtended> >::Iterator it = mStats.Find(pkgName);
    AutoPtr<PkgUsageStatsExtended> pus;
    if (it != mStats.End()) pus = it->mSecond;
    if (pus != NULL) {
        String clsName;
        componentName->GetClassName(&clsName);
        pus->AddLaunchTime(clsName, millis);
    }

    return NOERROR;
}

ECode CUsageStatsService::EnforceCallingPermission()
{
    if (Binder::GetCallingPid() == Process::MyPid()) {
        return NOERROR;
    }
    return mContext->EnforcePermission(Elastos::Droid::Manifest::permission::UPDATE_DEVICE_STATS,
            Binder::GetCallingPid(), Binder::GetCallingUid(), String(NULL));
}

ECode CUsageStatsService::GetPkgUsageStats(
    /* [in] */ IComponentName* componentName,
    /* [out] */ IPkgUsageStats** pkgUsageStats)
{
    VALIDATE_NOT_NULL(pkgUsageStats);
    *pkgUsageStats = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::PACKAGE_USAGE_STATS, String(NULL)));
    String pkgName;
    if ((componentName == NULL) ||
            (componentName->GetPackageName(&pkgName), pkgName.IsNull())) {
        return NOERROR;
    }

    AutoLock lock(mStatsLock);
    HashMap<String, AutoPtr<PkgUsageStatsExtended> >::Iterator it = mStats.Find(pkgName);
    AutoPtr<PkgUsageStatsExtended> pus;
    if (it != mStats.End()) pus = it->mSecond;
    LastResumeTimeMapIterator it2 = mLastResumeTimes.Find(pkgName);
    AutoPtr<StringInt64Map> lastResumeTimes;
    if (it2 != mLastResumeTimes.End()) lastResumeTimes = it2->mSecond;
    if (pus == NULL && lastResumeTimes == NULL) {
        return NOERROR;
    }
    Int32 launchCount = pus != NULL ? pus->mLaunchCount : 0;
    Int64 usageTime = pus != NULL ? pus->mUsageTime : 0;
    AutoPtr<IObjectStringMap> map;
    StringInt64Map::Iterator timeIt;
    for (timeIt = lastResumeTimes->Begin(); timeIt != lastResumeTimes->End(); ++timeIt) {
        AutoPtr<IInteger64> value;
        CInteger64::New(timeIt->mSecond, (IInteger64**)&value);
        map->Put(timeIt->mFirst, value);
    }

    CPkgUsageStats::New(pkgName, launchCount, usageTime, map, pkgUsageStats);
    return NOERROR;
}

ECode CUsageStatsService::GetAllPkgUsageStats(
    /* [out, callee] */ ArrayOf<IPkgUsageStats*>** stats)
{
    VALIDATE_NOT_NULL(stats);
    *stats = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::PACKAGE_USAGE_STATS, String(NULL)));

    AutoLock lock(mStatsLock);
    Int32 size = mLastResumeTimes.GetSize();
    if (size <= 0) {
        return NOERROR;
    }
    AutoPtr<ArrayOf<IPkgUsageStats*> > retArr = ArrayOf<IPkgUsageStats*>::Alloc(size);
    Int32 i = 0;
    LastResumeTimeMapIterator it;
    for (it = mLastResumeTimes.Begin(); it != mLastResumeTimes.End(); ++it) {
        String pkg = it->mFirst;
        Int64 usageTime = 0;
        Int32 launchCount = 0;

        HashMap<String, AutoPtr<PkgUsageStatsExtended> >::Iterator pusIt = mStats.Find(pkg);
        AutoPtr<PkgUsageStatsExtended> pus;
        if (pusIt != mStats.End()) pus = pusIt->mSecond;
        if (pus != NULL) {
            usageTime = pus->mUsageTime;
            launchCount = pus->mLaunchCount;
        }
        AutoPtr<IObjectStringMap> map;
        AutoPtr<StringInt64Map> valueMap = it->mSecond;
        StringInt64MapIterator valueIt;
        for (valueIt = valueMap->Begin(); valueIt != valueMap->End(); ++valueIt) {
            AutoPtr<IInteger64> value;
            CInteger64::New(valueIt->mSecond, (IInteger64**)&value);
            map->Put(valueIt->mFirst, value);
        }
        CPkgUsageStats::New(pkg, launchCount, usageTime, map, (IPkgUsageStats**)&(*retArr)[i]);
        i++;
    }
    *stats = retArr;
    REFCOUNT_ADD(*stats);
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > CUsageStatsService::ReadFully(
    /* [in] */ IFileInputStream* stream)
{
    Int32 pos = 0;
    Int32 avail;
    stream->Available(&avail);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(avail);
    while (TRUE) {
        Int32 amt;
        stream->ReadBytes(data, pos, data->GetLength()-pos, &amt);
        if (amt <= 0) {
            return data;
        }
        pos += amt;
        stream->Available(&avail);
        if (avail > data->GetLength()-pos) {
            AutoPtr<ArrayOf<Byte> > newData = ArrayOf<Byte>::Alloc(pos+avail);
            newData->Copy(data, 0, pos);
            data = newData;
        }
    }
}

void CUsageStatsService::CollectDumpInfoFLOCK(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Boolean isCompactOutput,
    /* [in] */ Boolean deleteAfterPrint,
    /* [in] */ ArrayOf<String>* packages)
{
    AutoPtr<ArrayOf<String> > fileList = GetUsageStatsFileListFLOCK();
    if (fileList == NULL) {
        return;
    }
    // TODO:
    // Collections.sort(fileList);
    for (Int32 i = 0; i < fileList->GetLength(); ++i) {
        String file = (*fileList)[i];
        if (deleteAfterPrint && file.EqualsIgnoreCase(mFileLeaf)) {
            // In this mode we don't print the current day's stats, since
            // they are incomplete.
            continue;
        }
        AutoPtr<IFile> dFile;
        CFile::New(mDir, file, (IFile**)&dFile);
        String dateStr = file.Substring(FILE_PREFIX.GetLength());
        // try {
        AutoPtr<IParcel> in = GetParcelForFile(dFile);
        CollectDumpInfoFromParcelFLOCK(in, pw, dateStr, isCompactOutput,
                packages);
        if (deleteAfterPrint) {
            // Delete old file after collecting info only for checkin requests
            Boolean deleted;
            dFile->Delete(&deleted);
        }
        // } catch (FileNotFoundException e) {
        //     Slog.w(TAG, "Failed with "+e+" when collecting dump info from file : " + file);
        //     return;
        // } catch (IOException e) {
        //     Slog.w(TAG, "Failed with "+e+" when collecting dump info from file : "+file);
        // }
    }
}

void CUsageStatsService::CollectDumpInfoFromParcelFLOCK(
    /* [in] */ IParcel* in,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& date,
    /* [in] */ Boolean isCompactOutput,
    /* [in] */ ArrayOf<String>* packages)
{
    StringBuilder sb(512);
    if (isCompactOutput) {
        sb.Append("D:");
        sb.Append(CHECKIN_VERSION);
        sb.AppendChar(',');
    } else {
        sb.Append("Date: ");
    }

    sb.Append(date);

    Int32 vers;
    in->ReadInt32(&vers);
    if (vers != VERSION) {
        sb.Append(" (old data version)");
        pw->Println(sb.ToString());
        return;
    }

    pw->Println(sb.ToString());
    Int32 N;
    in->ReadInt32(&N);

    while (N > 0) {
        N--;
        String pkgName;
        in->ReadString(&pkgName);
        if (pkgName.IsNull()) {
            break;
        }
        // TODO:
        // sb.setLength(0);
        AutoPtr<PkgUsageStatsExtended> pus = new PkgUsageStatsExtended(in);
        if (packages != NULL && !packages->Contains(pkgName)) {
            // This package has not been requested -- don't print
            // anything for it.
        } else if (isCompactOutput) {
            sb.Append("P:");
            sb.Append(pkgName);
            sb.AppendChar(',');
            sb.Append(pus->mLaunchCount);
            sb.AppendChar(',');
            sb.Append(pus->mUsageTime);
            sb.AppendChar('\n');
            const Int32 NC = pus->mLaunchTimes.GetSize();
            if (NC > 0) {
                HashMap<String, AutoPtr<TimeStats> >::Iterator it;
                for (it = pus->mLaunchTimes.Begin(); it != pus->mLaunchTimes.End(); ++it) {
                sb.Append("A:");
                String activity = it->mFirst;
                if (activity.StartWith(pkgName)) {
                    sb.AppendChar('*');
                    sb.Append(activity.Substring(
                                pkgName.GetLength(), activity.GetLength()));
                    } else {
                        sb.Append(activity);
                    }
                    AutoPtr<TimeStats> times = it->mSecond;
                    sb.AppendChar(',');
                    sb.Append(times->mCount);
                    for (Int32 i=0; i<NUM_LAUNCH_TIME_BINS; i++) {
                        sb.Append(",");
                        sb.Append((*times->mTimes)[i]);
                    }
                    sb.AppendChar('\n');
                }
            }

        } else {
            sb.Append("  ");
            sb.Append(pkgName);
            sb.Append(": ");
            sb.Append(pus->mLaunchCount);
            sb.Append(" times, ");
            sb.Append(pus->mUsageTime);
            sb.Append(" ms");
            sb.AppendChar('\n');
            const Int32 NC = pus->mLaunchTimes.GetSize();
            if (NC > 0) {
                HashMap<String, AutoPtr<TimeStats> >::Iterator it;
                for (it = pus->mLaunchTimes.Begin(); it != pus->mLaunchTimes.End(); ++it) {
                    sb.Append("    ");
                    sb.Append(it->mFirst);
                    AutoPtr<TimeStats> times = it->mSecond;
                    sb.Append(": ");
                    sb.Append(times->mCount);
                    sb.Append(" starts");
                    Int32 lastBin = 0;
                    for (Int32 i=0; i<NUM_LAUNCH_TIME_BINS-1; i++) {
                        if ((*times->mTimes)[i] != 0) {
                            sb.Append(", ");
                            sb.Append(lastBin);
                            sb.AppendChar('-');
                            sb.Append((*LAUNCH_TIME_BINS)[i]);
                            sb.Append("ms=");
                            sb.Append((*times->mTimes)[i]);
                        }
                        lastBin = (*LAUNCH_TIME_BINS)[i];
                    }
                    if ((*times->mTimes)[NUM_LAUNCH_TIME_BINS-1] != 0) {
                        sb.Append(", ");
                        sb.Append(">=");
                        sb.Append(lastBin);
                        sb.Append("ms=");
                        sb.Append((*times->mTimes)[NUM_LAUNCH_TIME_BINS-1]);
                    }
                    sb.AppendChar('\n');
                }
            }
        }

        pw->WriteString(sb.ToString());
    }
}

/**
 * Searches array of arguments for the specified string
 * @param args array of argument strings
 * @param value value to search for
 * @return true if the value is contained in the array
 */
Boolean CUsageStatsService::ScanArgs(
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ const String& value)
{
    if (args != NULL) {
        for (Int32 i = 0; i < args->GetLength(); ++i) {
            String arg = (*args)[i];
            if (value.Equals(arg)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/**
 * Searches array of arguments for the specified string's data
 * @param args array of argument strings
 * @param value value to search for
 * @return the string of data after the arg, or null if there is none
 */
String CUsageStatsService::ScanArgsData(
    /* [in] */ ArrayOf<String>* args,
    /* [in] */ const String& value)
{
    if (args != NULL) {
        const Int32 N = args->GetLength();
        for (Int32 i = 0; i < N; i++) {
            if (value.Equals((*args)[i])) {
                i++;
                return i < N ? (*args)[i] : String(NULL);
            }
        }
    }
    return String(NULL);
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
