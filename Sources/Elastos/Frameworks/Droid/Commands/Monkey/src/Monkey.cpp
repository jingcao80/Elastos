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

#include "Monkey.h"
#include <unistd.h>
#include "elastos/droid/os/Process.h"
#include <elastos/core/StringUtils.h>
#include "elastos/droid/app/ActivityManagerNative.h"
#include <stdlib.h>
#include "MonkeyUtils.h"
#include <elastos/core/Math.h>
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/StringBuilder.h>

using Elastos::IO::IWriter;
using Elastos::IO::CFile;
using Elastos::IO::CBufferedWriter;
using Elastos::IO::CFileReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::IFileWriter;
using Elastos::IO::CFileWriter;
using Elastos::IO::IBufferedReader;
using Elastos::IO::IFileReader;
using Elastos::Core::ISystem;
using Elastos::Utility::CRandom;
using Elastos::Core::Math;
using Elastos::Core::CObjectContainer;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::App::EIID_IActivityController;
using Elastos::Droid::App::ActivityManagerNative;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

static AutoPtr<IFile> InitStatics()
{
    AutoPtr<IFile> file;
    ASSERT_SUCCEEDED(CFile::New(String("/data/tombstones"), (IFile**)&file));
    return file;
}

static const Int32 ONELINE_MAX_LENGTH = 1024;
AutoPtr<IIntent> Monkey::currentIntent;
String Monkey::currentPackage;
AutoPtr<IFile> Monkey::TOMBSTONES_PATH = InitStatics();

Monkey::Monkey()
    : mThrottle(0)
    , mRandomizeThrottle(FALSE)
    , mCount(1000)
    , mSeed(0)
    , mDroppedKeyEvents(0)
    , mDroppedPointerEvents(0)
    , mDroppedTrackballEvents(0)
    , mDroppedFlipEvents(0)
    , mDroppedRotationEvents(0)
    , mProfileWaitTime(5000)
    , mDeviceSleepTime(30000)
    , mRandomizeScript(FALSE)
    , mScriptLog(FALSE)
    , mFactors(ArrayOf<Float>::Alloc(IMonkeySourceRandom::FACTORZ_COUNT))
    , mNextArg(0)
    , mCurArgData()
    , mVerbose(0)
    , mIgnoreCrashes(FALSE)
    , mIgnoreTimeouts(FALSE)
    , mIgnoreSecurityExceptions(FALSE)
    , mMonitorNativeCrashes(FALSE)
    , mIgnoreNativeCrashes(FALSE)
    , mSendNoEvents(FALSE)
    , mAbort(FALSE)
    , mCountEvents(TRUE)
    , mRequestAnrTraces(FALSE)
    , mRequestDumpsysMemInfo(FALSE)
    , mRequestAnrBugreport(FALSE)
    , mRequestAppCrashBugreport(FALSE)
    , mGetPeriodicBugreport(FALSE)
    , mRequestPeriodicBugreport(FALSE)
    , mBugreportFrequency(10)
    , mReportProcessName()
    , mRequestProcRank(FALSE)
    , mKillProcessAfterError(FALSE)
    , mGenerateHprof(FALSE)
    , mPkgBlacklistFile()
    , mPkgWhitelistFile()
    , mValidPackages(new HashSet<String>())
    , mInvalidPackages(new HashSet<String>())
    , mMainCategories(new List<String>())
    , mMainApps(new List<AutoPtr<IComponentName> >())
    , mRequestBugreport(FALSE)
    , mSetupFileName()
    , mScriptFileNames(ArrayOf<String>::Alloc(0))
    , mServerPort(-1)
{
    ASSERT_SUCCEEDED(CMonkeyNetworkMonitor::New((IMonkeyNetworkMonitor**)&mNetworkMonitor));
}

Monkey::~Monkey()
{
}

/**
 * Check whether we should run against the givn package.
 *
 * @param pkg The package name.
 * @return Returns TRUE if we should run against pkg.
 */
Boolean Monkey::CheckEnteringPackage(
    /* [in] */ const String& pkg)
{
    if (mInvalidPackages->IsEmpty() == FALSE) {
        if (mInvalidPackages->Find(pkg) != mInvalidPackages->End()) {
            return FALSE;
        }
    } else if (mValidPackages->IsEmpty() == FALSE) {
        if (mValidPackages->Find(pkg) == mValidPackages->End()) {
            return FALSE;
        }
    }
    return TRUE;
}

Int32 Monkey::GetVerbose()
{
    return mVerbose;
}

/**
     * Run the procrank tool to insert system status information into the debug
     * report.
     */
void Monkey::ReportProcRank()
{
    CommandLineReport(String("procrank"), String("procrank"));
}

/**
     * Run "cat /data/anr/traces.txt". Wait about 5 seconds first, to let the
     * asynchronous report writing complete.
     */
void Monkey::ReportAnrTraces()
{
        usleep(5 * 1000 * 1000);
        CommandLineReport(String("anr traces"), String("cat /data/anr/traces.txt"));
}

/**
     * Run "dumpsys meminfo"
     * <p>
     * NOTE: You cannot perform a dumpsys call from the ActivityController
     * callback, as it will deadlock. This should only be called from the main
     * loop of the monkey.
     */
void Monkey::ReportDumpsysMemInfo()
{
        CommandLineReport(String("meminfo"), String("dumpsys meminfo"));
}

void Monkey::CommandLineReport(
    /* [in] */ const String& reportName,
    /* [in] */ const String& command)
{
    PFL_EX("%s:", reportName.string());
    FILE *pFile = fopen(command, "r");
    if (pFile == NULL) {
        return;
    }
    AutoPtr<IWriter> logOutput;
    if (mRequestBugreport) {
        AutoPtr<IBufferedWriter> bufferWriter;
        AutoPtr<IFileWriter> fileWriter;
        AutoPtr<IEnvironment> env;
        CEnvironment::AcquireSingleton((IEnvironment**)&env);
        AutoPtr<IFile> file;
        env->GetLegacyExternalStorageDirectory((IFile**)&file);
        AutoPtr<IFile> file2;
        CFile::New(file, reportName, (IFile**)&file2);
        CFileWriter::New(file2, TRUE, (IFileWriter**)&fileWriter);
        CBufferedWriter::New(fileWriter, (IBufferedWriter**)&bufferWriter);
        logOutput = IWriter::Probe(bufferWriter);
    }
    char oneLine[ONELINE_MAX_LENGTH];
    while(fgets(oneLine, sizeof(oneLine), pFile)) {
        if(mRequestBugreport) {
            logOutput->WriteString(String(oneLine) + "\n");
        } else {
            logOutput->WriteString(String(oneLine));
        }
    }

    Int32 status = pclose(pFile);
    PFL_EX("// %s status was %d", reportName.string(), status);
}

// Write the numbe of iteration to the log
void Monkey::WriteScriptLog(
    /* [in] */ Int32 count)
{
    AutoPtr<IBufferedWriter> bufferWriter;
    AutoPtr<IFileWriter> fileWriter;
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> file;
    env->GetLegacyExternalStorageDirectory((IFile**)&file);
    AutoPtr<IFile> file2;
    CFile::New(file, String("scriptlog.txt"), (IFile**)&file2);
    CFileWriter::New(file2, TRUE, (IFileWriter**)&fileWriter);
    CBufferedWriter::New(fileWriter, (IBufferedWriter**)&bufferWriter);
    AutoPtr<IWriter> output = IWriter::Probe(bufferWriter);
    output->WriteString(String("iteration: "));
    output->Write(count);
    output->WriteString(String(" time: "));
    String calTime;
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    MonkeyUtils::ToCalendarTime(now, &calTime);
    output->WriteString(calTime);
    output->WriteString(String("\n"));
}

    // Write the bugreport to the sdcard.
void Monkey::GetBugreport(
    /* [in] */ const String& reportName)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    String calTime;
    MonkeyUtils::ToCalendarTime(now, &calTime);
    String bugreportName = reportName + calTime;
    StringUtils::ReplaceAll(bugreportName, String("[ ,:]"), String("_"), &bugreportName);
    CommandLineReport(bugreportName + ".txt", String("bugreport"));
}

/**
     * Command-line entry point32.
     *
     * @param args The command-line arguments
     */
void Monkey::Main(
    /* [in] */ const ArrayOf<String>& args)
{
    // Set the process name showing in "ps" or "top"
    Process::SetArgV0(String("elastos.droid.commands.monkey"));
    AutoPtr<Monkey> mk(new Monkey());
    Int32 resultCode = mk->Run(&const_cast< ArrayOf<String>& >(args));
}

Int32 Monkey::Run(
    /* [in] */ ArrayOf<String>* args)
{
    // Super-early debugger wait
    Int32 length = args->GetLength();
    for(Int32 i = 0; i < length; ++i) {
        if((*args)[i].Equals("--wait-dbg")) {
            //Debug.waitForDebugger();
        }
    }

    // Default values for some command-line options
    mVerbose = 0;
    mCount = 1000;
    mSeed = 0;
    mThrottle = 0;

    // prepare for command-line processing
    mArgs = args;
    mNextArg = 0;

    // set a positive value, indicating none of the factors is provided yet
    for (Int32 i = 0; i < IMonkeySourceRandom::FACTORZ_COUNT; i++) {
        mFactors->Set(i, 1.0f);
    }

    (*mFactors)[IMonkeySourceRandom::FACTOR_SYSOPS] = 0.0f;

    if (!ProcessOptions()) {
        return -1;
    }

    if (!LoadPackageLists()) {
        return -1;
    }

    // now set up additional data in preparation for launch
    if (mMainCategories->IsEmpty()) {
        mMainCategories->PushBack(IIntent::CATEGORY_LAUNCHER);
        mMainCategories->PushBack(IIntent::CATEGORY_MONKEY);
    }

    if (mSeed == 0) {
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 now;
        system->GetCurrentTimeMillis(&now);
        mSeed = now + (Int32)this;// + this->GetHashCode();
    }

    if (mVerbose > 0) {
        PFL_EX(":Monkey: seed = %lld count= %d", mSeed, mCount);
        if (!mValidPackages->IsEmpty()) {
            HashSet<String>::Iterator it = mValidPackages->Begin();
            for(; it != mValidPackages->End(); ++it) {
                PFL_EX(":AllowPackage: %s", it->string());
            }
        }
        if (!mInvalidPackages->IsEmpty()) {
            HashSet<String>::Iterator it = mInvalidPackages->Begin();
            for(; it != mInvalidPackages->End(); ++it) {
                PFL_EX(":DisallowPackage: %s", it->string());
            }
        }
        if (!mMainCategories->IsEmpty()) {
            List<String>::Iterator it = mMainCategories->Begin();
            for(;it != mMainCategories->End(); ++it) {
                PFL_EX(":IncludeCategory: %s", it->string());
            }
        }
    }

    if (!CheckInternalConfiguration()) {
        return -2;
    }

    if (!GetSystemInterfaces()) {
        return -3;
    }

    if (!GetMainApps()) {
        return -4;
    }

    mRandom = NULL;
    CRandom::New(mSeed, (IRandom**)&mRandom);

    if (mScriptFileNames != NULL && mScriptFileNames->GetLength() == 1) {
        // script mode, ignore other options
        AutoPtr<IMonkeySourceScript> source;
        CMonkeySourceScript::New(mRandom, (*mScriptFileNames)[0], mThrottle,
            mRandomizeThrottle, mProfileWaitTime, mDeviceSleepTime, (IMonkeySourceScript**)&source);
        mEventSource = IMonkeyEventSource::Probe(source);
        mEventSource->SetVerbose(mVerbose);
        mCountEvents = FALSE;
    }
    else if (mScriptFileNames != NULL && mScriptFileNames->GetLength() > 1) {
        AutoPtr<IMonkeySourceRandomScript> source;
        if (!mSetupFileName.IsNull()) {
            CMonkeySourceRandomScript::New(mSetupFileName, mScriptFileNames
                , mThrottle, mRandomizeThrottle, mRandom, mProfileWaitTime
                , mDeviceSleepTime, mRandomizeScript, (IMonkeySourceRandomScript**)&source);
            mEventSource = IMonkeyEventSource::Probe(source);
            mCount++;
        } else {
            CMonkeySourceRandomScript::New(mScriptFileNames, mThrottle, mRandomizeThrottle
                , mRandom, mProfileWaitTime, mDeviceSleepTime, mRandomizeScript, (IMonkeySourceRandomScript**)&source);
            mEventSource = IMonkeyEventSource::Probe(source);
        }
        mEventSource->SetVerbose(mVerbose);
        mCountEvents = FALSE;
    }
    else if (mServerPort != -1) {
        AutoPtr<IMonkeySourceNetwork> source;
        CMonkeySourceNetwork::New(mServerPort, (IMonkeySourceNetwork**)&source);
        mEventSource = IMonkeyEventSource::Probe(source);
        if(!mEventSource) {
            PFL_EX("Error binding to network socket.");
            return -5;
        }
        mCount = Elastos::Core::Math::INT32_MAX_VALUE;
    }
    else {
        // random source by default
        if (mVerbose >= 2) { // check seeding performance
            PFL_EX("// Seeded: %lld", mSeed);
        }

        AutoPtr<IObjectContainer> mainApp;
        CObjectContainer::New((IObjectContainer**)&mainApp);

        List<AutoPtr<IComponentName> >::Iterator it = mMainApps->Begin();
        for (; it != mMainApps->End(); ++it) {
            mainApp->Add(*it);
        }

        AutoPtr<IMonkeySourceRandom> source;
        CMonkeySourceRandom::New(mRandom, mainApp, mThrottle, mRandomizeThrottle,
            (IMonkeySourceRandom**)&source);

        mEventSource = IMonkeyEventSource::Probe(source);
        mEventSource->SetVerbose(mVerbose);
        // set any of the factors that has been set
        for (Int32 i = 0; i < IMonkeySourceRandom::FACTORZ_COUNT; i++) {
            if ((*mFactors)[i] <= 0.0f) {
                source->SetFactors(i, (*mFactors)[i]);
            }
        }

        // in random mode, we start with a random activity
        source->GenerateActivity();
    }

    // validate source generator
    Boolean validated;
    if(mEventSource->Validate(&validated), !validated) {
        return -5;
    }

    // If we're profiling, do it immediately before/after the main monkey
    // loop
    if (mGenerateHprof) {
        SignalPersistentProcesses();
    }

    mNetworkMonitor->Start();
    Int32 crashedAtCycle = 0;
    crashedAtCycle = RunMonkeyCycles();

    // Release the rotation lock if it's still held and restore the
    // original orientation.
    AutoPtr<IMonkeyRotationEvent> event;
    Int32 eventReturnVal;
    CMonkeyRotationEvent::New(ISurface::ROTATION_0, FALSE, (IMonkeyRotationEvent**)&event);
    event->InjectEvent(mWm, mAm, mVerbose, &eventReturnVal);
    mNetworkMonitor->Stop();

    {
        AutoLock lock(mLockLock);
        if (mRequestAnrTraces) {
            ReportAnrTraces();
            mRequestAnrTraces = FALSE;
        }
        if (mRequestAnrBugreport){
            PFL_EX("Print the anr report");
            GetBugreport(String("anr_") + mReportProcessName + "_");
            mRequestAnrBugreport = FALSE;
        }
        if (mRequestAppCrashBugreport){
            GetBugreport(String("app_crash") + mReportProcessName + "_");
            mRequestAppCrashBugreport = FALSE;
        }
        if (mRequestDumpsysMemInfo) {
            ReportDumpsysMemInfo();
            mRequestDumpsysMemInfo = FALSE;
        }
        if (mRequestPeriodicBugreport){
            GetBugreport(String("Bugreport_"));
            mRequestPeriodicBugreport = FALSE;
        }
    }

    if (mGenerateHprof) {
        SignalPersistentProcesses();
        if (mVerbose > 0) {
            PFL_EX("// Generated profiling reports in /data/misc");
        }
    }

    ECode ec = mAm->SetActivityController(NULL);
    if (SUCCEEDED(ec)) {
        ec = mNetworkMonitor->Unregister(mAm);
    }

ERROR_CODE:
    // just in case this was latent (after mCount cycles), make sure
    // we report it
    if (crashedAtCycle >= mCount) {
        crashedAtCycle = mCount - 1;
    }

    // report dropped event stats
    if (mVerbose > 0) {
        PFL_EX(":Dropped: keys = %lld, pointers = %lld, trackballs = %lld, flips = %lld, rotations = %lld", mDroppedKeyEvents,
            mDroppedPointerEvents, mDroppedTrackballEvents, mDroppedFlipEvents, mDroppedRotationEvents);
    }

    // report network stats
    mNetworkMonitor->Dump();

    if (crashedAtCycle < mCount - 1) {
        PFL_EX("** System appears to have crashed at event %d of %d using seed %lld",
            crashedAtCycle, mCount, mSeed);
        return crashedAtCycle;
    } else {
        if (mVerbose > 0) {
            PFL_EX("// Monkey finished");
        }
        return 0;
    }
}

/**
 * Process the command-line options
 *
 * @return Returns TRUE if options were parsed with no apparent errors.
 */
Boolean Monkey::ProcessOptions()
{
    // quick (throwaway) check for unadorned command
    if (mArgs->GetLength() < 1) {
        ShowUsage();
        return FALSE;
    }

    String opt;
    while (!((opt = NextOption()).IsNull())) {
        if (opt.Equals("-s")) {
            mSeed = NextOptionLong("Seed");
        } else if (opt.Equals("-p")) {
            mValidPackages->Insert(NextOptionData());
        } else if (opt.Equals("-c")) {
            mMainCategories->PushBack(NextOptionData());
        } else if (opt.Equals("-v")) {
            mVerbose += 1;
        } else if (opt.Equals("--ignore-crashes")) {
            mIgnoreCrashes = TRUE;
        } else if (opt.Equals("--ignore-timeouts")) {
            mIgnoreTimeouts = TRUE;
        } else if (opt.Equals("--ignore-security-exceptions")) {
            mIgnoreSecurityExceptions = TRUE;
        } else if (opt.Equals("--monitor-native-crashes")) {
            mMonitorNativeCrashes = TRUE;
        } else if (opt.Equals("--ignore-native-crashes")) {
            mIgnoreNativeCrashes = TRUE;
        } else if (opt.Equals("--kill-process-after-error")) {
            mKillProcessAfterError = TRUE;
        } else if (opt.Equals("--hprof")) {
            mGenerateHprof = TRUE;
        } else if (opt.Equals("--pct-touch")) {
            Int32 i = IMonkeySourceRandom::FACTOR_TOUCH;
            (*mFactors)[i] = -NextOptionLong("touch events percentage");
        } else if (opt.Equals("--pct-motion")) {
            Int32 i = IMonkeySourceRandom::FACTOR_MOTION;
            (*mFactors)[i] = -NextOptionLong("motion events percentage");
        } else if (opt.Equals("--pct-trackball")) {
            Int32 i = IMonkeySourceRandom::FACTOR_TRACKBALL;
            (*mFactors)[i] = -NextOptionLong("trackball events percentage");
        } else if (opt.Equals("--pct-rotation")) {
            Int32 i = IMonkeySourceRandom::FACTOR_ROTATION;
            (*mFactors)[i] = -NextOptionLong("screen rotation events percentage");
        } else if (opt.Equals("--pct-syskeys")) {
            Int32 i = IMonkeySourceRandom::FACTOR_SYSOPS;
            (*mFactors)[i] = -NextOptionLong("system (key) operations percentage");
        } else if (opt.Equals("--pct-nav")) {
            Int32 i = IMonkeySourceRandom::FACTOR_NAV;
            (*mFactors)[i] = -NextOptionLong("nav events percentage");
        } else if (opt.Equals("--pct-majornav")) {
            Int32 i = IMonkeySourceRandom::FACTOR_MAJORNAV;
            (*mFactors)[i] = -NextOptionLong("major nav events percentage");
        } else if (opt.Equals("--pct-appswitch")) {
            Int32 i = IMonkeySourceRandom::FACTOR_APPSWITCH;
            (*mFactors)[i] = -NextOptionLong("app switch events percentage");
        } else if (opt.Equals("--pct-flip")) {
            Int32 i = IMonkeySourceRandom::FACTOR_FLIP;
            (*mFactors)[i] = -NextOptionLong("keyboard flip percentage");
        } else if (opt.Equals("--pct-anyevent")) {
            Int32 i = IMonkeySourceRandom::FACTOR_ANYTHING;
            (*mFactors)[i] = -NextOptionLong("any events percentage");
        } else if (opt.Equals("--pct-pinchzoom")) {
            Int32 i = IMonkeySourceRandom::FACTOR_PINCHZOOM;
            (*mFactors)[i] = -NextOptionLong("pinch zoom events percentage");
        } else if (opt.Equals("--pkg-blacklist-file")) {
            mPkgBlacklistFile = NextOptionData();
        } else if (opt.Equals("--pkg-whitelist-file")) {
            mPkgWhitelistFile = NextOptionData();
        } else if (opt.Equals("--throttle")) {
            mThrottle = NextOptionLong("delay (in milliseconds) to wait between events");
        } else if (opt.Equals("--randomize-throttle")) {
            mRandomizeThrottle = TRUE;
        } else if (opt.Equals("--wait-dbg")) {
            // do nothing - it's caught at the very start of run()
        } else if (opt.Equals("--dbg-no-events")) {
            mSendNoEvents = TRUE;
        } else if (opt.Equals("--port")) {
            mServerPort = (Int32) NextOptionLong("Server port to listen on for commands");
        } else if (opt.Equals("--setup")) {
            mSetupFileName = NextOptionData();
        } else if (opt.Equals("-f")) {
            mScriptFileNames->Set(mScriptFileNames->GetLength(), NextOptionData());
        } else if (opt.Equals("--profile-wait")) {
            mProfileWaitTime = NextOptionLong("Profile delay" \
                        " (in milliseconds) to wait between user action");
        } else if (opt.Equals("--device-sleep-time")) {
            mDeviceSleepTime = NextOptionLong("Device sleep time" \
                                              "(in milliseconds)");
        } else if (opt.Equals("--randomize-script")) {
            mRandomizeScript = TRUE;
        } else if (opt.Equals("--script-log")) {
            mScriptLog = TRUE;
        } else if (opt.Equals("--bugreport")) {
            mRequestBugreport = TRUE;
        } else if (opt.Equals("--periodic-bugreport")){
            mGetPeriodicBugreport = TRUE;
            mBugreportFrequency = NextOptionLong("Number of iterations");
        } else if (opt.Equals("-h")) {
            ShowUsage();
            return FALSE;
        } else {
            PFL_EX("** Error: Unknown option: %s", opt.string());
            ShowUsage();
            return FALSE;
        }
    } //end while

    // If a server port hasn't been specified, we need to specify
    // a count
    if (mServerPort == -1) {
        String countStr = NextArg();
        if (countStr.IsNull()) {
            PFL_EX("** Error: Count not specified");
            ShowUsage();
            return FALSE;
        }
        StringUtils::ParseInt32(countStr, &mCount);
    }

    return TRUE;
}

/**
 * Load a list of package names from a file.
 *
 * @param fileName The file name, with package names separated by new line.
 * @param list The destination list.
 * @return Returns FALSE if any error occurs.
 */
Boolean Monkey::LoadPackageListFromFile(
    /* [in] */ const String& fileName,
    /* [in] */ HashSet<String>* list)
{
    AutoPtr<IBufferedReader> reader;
    AutoPtr<IFileReader> fileReader;
    CFileReader::New(fileName, (IFileReader**)&fileReader);
    CBufferedReader::New(fileReader, (IBufferedReader**)&reader);

    String s;
    while(reader->ReadLine(&s), !s.IsNull()) {
        s = s.Trim();
        if(!s.IsNullOrEmpty() && !s.StartWith("#")) {
            list->Insert(s);
        }
    }

    if(!reader.Get()) {
        //reader->Close();
    }

    return TRUE;
}

/**
 * Load package blacklist or whitelist (if specified).
 *
 * @return Returns FALSE if any error occurs.
 */
Boolean Monkey::LoadPackageLists()
{
    if (((!mPkgWhitelistFile.IsNull()) || (mValidPackages->IsEmpty() == FALSE))
            && (!mPkgBlacklistFile.IsNull())) {
        PFL_EX("** Error: you can not specify a package blacklist " \
                 "together with a whitelist or individual packages (via -p).");
        return FALSE;
    }
    if ((!mPkgWhitelistFile.IsNull())
            && (!LoadPackageListFromFile(mPkgWhitelistFile, mValidPackages))) {
        return FALSE;
    }
    if ((!mPkgBlacklistFile.IsNull())
            && (!LoadPackageListFromFile(mPkgBlacklistFile, mInvalidPackages))) {
        return FALSE;
    }
    return TRUE;
}

/**
 * Check for any internal configuration (primarily build-time) errors.
 *
 * @return Returns TRUE if ready to rock.
 */
Boolean Monkey::CheckInternalConfiguration()
{
    return TRUE;
}

/**
 * Attach to the required system interfaces.
 *
 * @return Returns TRUE if all system interfaces were available.
 */
Boolean Monkey::GetSystemInterfaces()
{
    mAm = ActivityManagerNative::GetDefault();
    if (!mAm) {
        PFL_EX("** Error: Unable to connect to activity manager; is the system " \
                 "running?");
        return FALSE;
    }

    AutoPtr<IServiceManager> sm;
    AutoPtr<IInterface> base;
    ASSERT_SUCCEEDED(CServiceManager::AcquireSingleton((IServiceManager**)&sm));
    sm->GetService(String("window"), (IInterface**)&base);
    mWm = IIWindowManager::Probe(base);

    if (mWm == NULL) {
        PFL_EX("** Error: Unable to connect to window manager; is the system " \
                 "running?");
        return FALSE;
    }

    base = NULL;
    sm->GetService(String("package"), (IInterface**)&base);
    mPm = IIPackageManager::Probe(base);
    if(mPm == NULL) {
        PFL_EX("** Error: Unable to connect to package manager; is the system " \
                 "running?");
        return FALSE;
    }

    AutoPtr<IIActivityController> iac;
    CActivityController::New((HANDLE)this, (IIActivityController**)&iac);
    ECode ec = mAm->SetActivityController(iac.Get());
    FAIL_GOTO(ec, _Exit_)

    ec = mNetworkMonitor->Register(mAm);

_Exit_:
    if (FAILED(ec)) {
        PFL_EX("** Failed talking with activity manager!");
        return FALSE;
    }
    return TRUE;
}

/**
 * Using the restrictions provided (categories & packages), generate a list
 * of activities that we can actually switch to.
 *
 * @return Returns TRUE if it could successfully build a list of target
 *         activities
 */
Boolean Monkey::GetMainApps()
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 myUserId;
    helper->GetMyUserId(&myUserId);

    List<String>::Iterator it = mMainCategories->Begin();
    const Int32 N = mMainCategories->GetSize();
    for (; it != mMainCategories->End(); ++it) {
        String category = *it;
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
        if (!category.IsNullOrEmpty()) {
            intent->AddCategory(category);
        }

        AutoPtr<IObjectContainer> mainApps;
        mPm->QueryIntentActivities(intent, String(), 0, myUserId, (IObjectContainer**)&mainApps);
        Int32 count;
        if (!mainApps || (mainApps->GetObjectCount(&count), count == 0)) {
            PFL_EX("// Warning: no activities found for category %s", category.string());
            continue;
        }
        if (mVerbose >= 2) { // very verbose
            PFL_EX("// Selecting main activities from category %s", category.string());
        }

        AutoPtr<IObjectEnumerator> enumerator;
        mainApps->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        Boolean hasNext = FALSE;
        while (enumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> element;
            enumerator->Current((IInterface**)&element);
            AutoPtr<IResolveInfo> r = IResolveInfo::Probe(element);
            String packageName;
            AutoPtr<IActivityInfo> act;
            r->GetActivityInfo((IActivityInfo**)&act);
            AutoPtr<IApplicationInfo> appInfo;
            act->GetApplicationInfo((IApplicationInfo**)&appInfo);
            appInfo->GetPackageName(&packageName);
            String actName;
            act->GetName(&actName);

            if (CheckEnteringPackage(packageName)) {
                if (mVerbose >= 2) { // very verbose
                    PFL_EX("//   + Using main activity %s (from package %s)", actName.string(), packageName.string());
                }
                AutoPtr<IComponentName> ele;
                CComponentName::New(packageName, actName, (IComponentName**)&ele);
                mMainApps->PushBack(ele);
            }
            else {
                if (mVerbose >= 3) { // very very verbose
                    PFL_EX("//   - NOT USING main activity %s (from package %s)",
                            actName.string(), packageName.string());
                }
            }
        } //end while
    }

    if (mMainApps->IsEmpty()) {
        PFL_EX("** No activities found to run, monkey aborted.");
        return FALSE;
    }

    return TRUE;
}

/**
 * Run mCount cycles and see if we hit any crashers.
 * <p>
 * TODO: Meta state on keys
 *
 * @return Returns the last cycle which executed. If the value == mCount, no
 *         errors detected.
 */
Int32 Monkey::RunMonkeyCycles()
{
    Int32 eventCounter = 0;
    Int32 cycleCounter = 0;

    Boolean shouldReportAnrTraces = FALSE;
    Boolean shouldReportDumpsysMemInfo = FALSE;
    Boolean shouldAbort = FALSE;
    Boolean systemCrashed = FALSE;
    AutoPtr<ISystem> systemObj;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&systemObj);

    // TO DO : The count should apply to each of the script file.
    while (!systemCrashed && cycleCounter < mCount) {
        {
            if ( -1 == system("cd /data && ./getsysinfo.sh")) {
                PFL_EX("please check whether the shell script file is correct or not")
            }
            AutoLock lock(mLockLock);
            if (mRequestProcRank) {
                ReportProcRank();
                mRequestProcRank = FALSE;
            }
            if (mRequestAnrTraces) {
                mRequestAnrTraces = FALSE;
                shouldReportAnrTraces = TRUE;
            }
            if (mRequestAnrBugreport){
                GetBugreport(String("anr_") + mReportProcessName + "_");
                mRequestAnrBugreport = FALSE;
            }
            if (mRequestAppCrashBugreport){
                GetBugreport(String("app_crash") + mReportProcessName + "_");
                mRequestAppCrashBugreport = FALSE;
            }
            if (mRequestPeriodicBugreport){
                GetBugreport(String("Bugreport_"));
                mRequestPeriodicBugreport = FALSE;
            }
            if (mRequestDumpsysMemInfo) {
                mRequestDumpsysMemInfo = FALSE;
                shouldReportDumpsysMemInfo = TRUE;
            }
            if (mMonitorNativeCrashes) {
                // first time through, when eventCounter == 0, just set up
                // the watcher (ignore the error)
                if (CheckNativeCrashes() && (eventCounter > 0)) {
                    PFL_EX("** New native crash detected.");
                    if (mRequestBugreport) {
                        GetBugreport(String("native_crash_"));
                    }
                    mAbort = mAbort || !mIgnoreNativeCrashes || mKillProcessAfterError;
                }
            }
            if (mAbort) {
                shouldAbort = TRUE;
            }
        }

        // Report ANR, dumpsys after releasing lock on this.
        // This ensures the availability of the lock to Activity controller's appNotResponding
        if (shouldReportAnrTraces) {
           shouldReportAnrTraces = FALSE;
           ReportAnrTraces();
        }

        if (shouldReportDumpsysMemInfo) {
           shouldReportDumpsysMemInfo = FALSE;
           ReportDumpsysMemInfo();
        }

        if (shouldAbort) {
           shouldAbort = FALSE;
           PFL_EX("** Monkey aborted due to error.");
           PFL_EX("Events injected: %d", eventCounter);
           return eventCounter;
        }

        // In this debugging mode, we never send any events. This is
        // primarily here so you can manually test the package or category
        // limits, while manually exercising the system.
        if (mSendNoEvents) {
            eventCounter++;
            cycleCounter++;
            continue;
        }

        if ((mVerbose > 0) && (eventCounter % 100) == 0 && eventCounter != 0) {
            String calendarTime;
            Int64 now;
            systemObj->GetCurrentTimeMillis(&now);
            MonkeyUtils::ToCalendarTime(now, &calendarTime);


            Int64 systemUpTime = SystemClock::GetElapsedRealtime();
            PFL_EX("    //[calendar_time:%s system_uptime:%lld]", calendarTime.string(),
                               systemUpTime);
            PFL_EX("    // Sending event #%d", eventCounter);
        }

        AutoPtr<IMonkeyEvent> ev;
        mEventSource->GetNextEvent((IMonkeyEvent**)&ev);
        if (ev != NULL) {
            Int32 injectCode;
            ev->InjectEvent(mWm, mAm, mVerbose, &injectCode);
            if (injectCode == IMonkeyEvent::INJECT_FAIL) {
                if (IMonkeyKeyEvent::Probe(ev) != NULL) {
                    mDroppedKeyEvents++;
                }
                else if (IMonkeyMotionEvent::Probe(ev) != NULL) {
                    mDroppedPointerEvents++;
                }
                else if (IMonkeyFlipEvent::Probe(ev) != NULL) {
                    mDroppedFlipEvents++;
                }
                else if (IMonkeyRotationEvent::Probe(ev) != NULL) {
                    mDroppedRotationEvents++;
                }
            }
            else if (injectCode == IMonkeyEvent::INJECT_ERROR_REMOTE_EXCEPTION) {
                systemCrashed = TRUE;
                PFL_EX("** Error: RemoteException while injecting event.");
            }
            else if (injectCode == IMonkeyEvent::INJECT_ERROR_SECURITY_EXCEPTION) {
                systemCrashed = !mIgnoreSecurityExceptions;
                if (systemCrashed) {
                    PFL_EX("** Error: SecurityException while injecting event.");
                }
            }

            // Don't count throttling as an event.
            if (IMonkeyThrottleEvent::Probe(ev) == NULL) {
                eventCounter++;
                if (mCountEvents) {
                    cycleCounter++;
                }
            }
        }
        else {
            if (!mCountEvents) {
                cycleCounter++;
                WriteScriptLog(cycleCounter);
                //Capture the bugreport after n iteration
                if (mGetPeriodicBugreport) {
                    if ((cycleCounter % mBugreportFrequency) == 0) {
                        mRequestPeriodicBugreport = TRUE;
                    }
                }
            } else {
                // Event Source has signaled that we have no more events to process
                break;
            }
        }
    }
    PFL_EX("Events injected: %d", eventCounter);
    return eventCounter;
}

/**
 * Send SIGNAL_USR1 to all processes. This will generate large (5mb)
 * profiling reports in data/misc, so use with care.
 */
void Monkey::SignalPersistentProcesses()
{
    mAm->SignalPersistentProcesses(IProcess::SIGNAL_USR1);
    AutoLock lock(mLockLock);
    mLock.Wait(2000);
}

/**
 * Watch for appearance of new tombstone files, which indicate native
 * crashes.
 *
 * @return Returns true if new files have appeared in the list
 */
Boolean Monkey::CheckNativeCrashes()
{
   AutoPtr<ArrayOf<String> > tombstones;
   TOMBSTONES_PATH->List((ArrayOf<String>**)&tombstones);

    // shortcut path for usually empty directory, so we don't waste even
    // more objects
    if (tombstones == NULL || (tombstones->GetLength() == 0)) {
        mTombstones = NULL;
        return FALSE;
    }

    // use set logic to look for new files
    AutoPtr< HashSet<String> > newStones = new HashSet<String>();
    Int32 length  = tombstones->GetLength();
    for(Int32 i = 0; i < length; ++i) {
        newStones->Insert((*tombstones)[i]);
    }

    Boolean result = FALSE;
    HashSet<String>::Iterator it = newStones->Begin();
    for(; it != newStones->End(); ++it) {
        if (mTombstones->Find(*it) == mTombstones->End()) {
            result = TRUE;
            break;
        }
    }

    // keep the new list for the next time
    mTombstones = newStones;
    return result;
}

/**
 * Return the next command line option. This has a number of special cases
 * which closely, but not exactly, follow the POSIX command line options
 * patterns:
 *
 * <pre>
 * -- means to stop processing additional options
 * -z means option z
 * -z ARGS means option z with (non-optional) arguments ARGS
 * -zARGS means option z with (optional) arguments ARGS
 * --zz means option zz
 * --zz ARGS means option zz with (non-optional) arguments ARGS
 * </pre>
 *
 * Note that you cannot combine single letter options; -abc != -a -b -c
 *
 * @return Returns the option string, or null if there are no more options.
 */
String Monkey::NextOption()
{
    if (mNextArg >= mArgs->GetLength()) {
        return String();
    }
    String arg = (*mArgs)[mNextArg];
    if (!arg.StartWith("-")) {
        return String();
    }
    mNextArg++;
    if (arg.Equals("--")) {
        return String();
    }
    if (arg.GetLength() > 1 && arg.GetChar(1) != '-') {
        if (arg.GetLength() > 2) {
            mCurArgData = arg.Substring(2);
            return arg.Substring(0, 2);
        }
        else {
            mCurArgData = String();
            return arg;
        }
    }
    mCurArgData = String();
    return arg;
}

/**
 * Return the next data associated with the current option.
 *
 * @return Returns the data string, or null of there are no more arguments.
 */
String Monkey::NextOptionData()
{
    if (!mCurArgData.IsNull()) {
        return mCurArgData;
    }
    if (mNextArg >= mArgs->GetLength()) {
        return String();
    }
    String data = (*mArgs)[mNextArg++];
    return data;
}

/**
 * Returns a long converted from the next data argument, with error handling
 * if not available.
 *
 * @param opt The name of the option.
 * @return Returns a long converted from the argument.
 */
Int64 Monkey::NextOptionLong(
    /* [in] */ const char *opt)
{
    Int64 result;
    ECode e = StringUtils::ParseInt64(NextOptionData(), 10, &result);
    if(FAILED(e)) {
        PFL_EX("** Error: %s is not a number", opt);
    }
    return result;
}

/**
 * Return the next argument on the command line.
 *
 * @return Returns the argument string, or null if we have reached the end.
 */
String Monkey::NextArg()
{
    if (mNextArg >= mArgs->GetLength()) {
        return String();
    }
    String arg = (*mArgs)[mNextArg];
    mNextArg++;
    return arg;
}

/**
 * Print how to use this command.
 */
void Monkey::ShowUsage()
{
    StringBuilder usage;
    usage.Append("usage: monkey [-p ALLOWED_PACKAGE [-p ALLOWED_PACKAGE] ...]\n");
    usage.Append("              [-c MAIN_CATEGORY [-c MAIN_CATEGORY] ...]\n");
    usage.Append("              [--ignore-crashes] [--ignore-timeouts]\n");
    usage.Append("              [--ignore-security-exceptions]\n");
    usage.Append("              [--monitor-native-crashes] [--ignore-native-crashes]\n");
    usage.Append("              [--kill-process-after-error] [--hprof]\n");
    usage.Append("              [--pct-touch PERCENT] [--pct-motion PERCENT]\n");
    usage.Append("              [--pct-trackball PERCENT] [--pct-syskeys PERCENT]\n");
    usage.Append("              [--pct-nav PERCENT] [--pct-majornav PERCENT]\n");
    usage.Append("              [--pct-appswitch PERCENT] [--pct-flip PERCENT]\n");
    usage.Append("              [--pct-anyevent PERCENT] [--pct-pinchzoom PERCENT]\n");
    usage.Append("              [--pkg-blacklist-file PACKAGE_BLACKLIST_FILE]\n");
    usage.Append("              [--pkg-whitelist-file PACKAGE_WHITELIST_FILE]\n");
    usage.Append("              [--wait-dbg] [--dbg-no-events]\n");
    usage.Append("              [--setup scriptfile] [-f scriptfile [-f scriptfile] ...]\n");
    usage.Append("              [--port port]\n");
    usage.Append("              [-s SEED] [-v [-v] ...]\n");
    usage.Append("              [--throttle MILLISEC] [--randomize-throttle]\n");
    usage.Append("              [--profile-wait MILLISEC]\n");
    usage.Append("              [--device-sleep-time MILLISEC]\n");
    usage.Append("              [--randomize-script]\n");
    usage.Append("              [--script-log]\n");
    usage.Append("              [--bugreport]\n");
    usage.Append("              [--periodic-bugreport]\n");
    usage.Append("              COUNT\n");
    PFL_EX(usage.ToString().string());
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
