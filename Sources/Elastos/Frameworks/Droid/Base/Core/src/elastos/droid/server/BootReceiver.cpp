
#include "elastos/droid/server/BootReceiver.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/Build.h"
// #include "elastos/droid/os/RecoverySystem.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

// using Elastos::Droid::Os::RecoverySystem;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

static Int32 InitLOG_SIZE()
{
    Int32 ival;
    SystemProperties::GetInt32(String("ro.debuggable"), 0, &ival);
    return ival == 1 ? 98304 : 65536;
}

static AutoPtr<IFile> InitTOMBSTONE_DIR()
{
    AutoPtr<IFile> file;
    CFile::New(String("/data/tombstones"), (IFile**)&file);
    return file;
}

const String BootReceiver::TAG("BootReceiver");

const Int32 BootReceiver::LOG_SIZE = InitLOG_SIZE();
const AutoPtr<IFile> BootReceiver::TOMBSTONE_DIR = InitTOMBSTONE_DIR();

const String BootReceiver::OLD_UPDATER_PACKAGE("com.google.android.systemupdater");
const String BootReceiver::OLD_UPDATER_CLASS("com.google.android.systemupdater.SystemUpdateReceiver");

AutoPtr<IFileObserver> BootReceiver::sTombstoneObserver;

//=================================================================================
// BootReceiver::MyThread
//=================================================================================

BootReceiver::MyThread::MyThread(
    /* [in] */ BootReceiver* host,
    /* [in] */ IContext* ctx)
    : mHost(host)
    , mContext(ctx)
{
}

ECode BootReceiver::MyThread::Run()
{
    ECode ec = mHost->LogBootEvents(mContext);
    if (FAILED(ec)) {
        Slogger::E(BootReceiver::TAG, "Can't log boot events %08x", ec);
        return ec;
    }
    Boolean onlyCore = FALSE;
    AutoPtr<IInterface> obj = ServiceManager::GetService(String("package"));
    IIPackageManager* pkgMgr = IIPackageManager::Probe(obj);
    ec = pkgMgr->IsOnlyCoreApps(&onlyCore);
    if (FAILED(ec)) {
        return E_REMOTE_EXCEPTION;
    }
    if (!onlyCore) {
        ec = mHost->RemoveOldUpdatePackages(mContext);
        if (FAILED(ec)) {
            Slogger::E(BootReceiver::TAG, "Can't remove old update packages %08x", ec);
            return ec;
        }
    }
    return NOERROR;
}

//=================================================================================
// BootReceiver::TombstoneObserver
//=================================================================================
BootReceiver::TombstoneObserver::TombstoneObserver(
    /* [in] */ const String& path,
    /* [in] */ Int32 mask,
    /* [in] */ IDropBoxManager* mgr,
    /* [in] */ ISharedPreferences* pref,
    /* [in] */ const String& headers,
    /* [in] */ BootReceiver* host)
    : FileObserver(path, mask)
    , mHost(host)
    , mDropBoxManager(mgr)
    , mSharedPreferences(pref)
    , mHeaders(headers)
{
}

ECode BootReceiver::TombstoneObserver::OnEvent(
    /* [in] */ Int32 event,
    /* [in] */ const String& path)
{
     Boolean isFile;
    AutoPtr<IFile> file;
    ECode ec = CFile::New(BootReceiver::TOMBSTONE_DIR, path, (IFile**)&file);
    if (FAILED(ec)) return E_IO_EXCEPTION;

    ec = file->IsFile(&isFile);
    if (FAILED(ec)) return E_IO_EXCEPTION;

    if (isFile) {
        String filePath;
        file->GetPath(&filePath);
        ec = mHost->AddFileToDropBox(mDropBoxManager.Get(), mSharedPreferences.Get(),
            mHeaders, filePath, BootReceiver::LOG_SIZE, String("SYSTEM_TOMBSTONE"));
    }

    if (FAILED(ec)) {
        Slogger::E(BootReceiver::TAG, "Can't log tombstone %08x", ec);
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

//=================================================================================
// BootReceiver
//=================================================================================

BootReceiver::BootReceiver()
{
}

ECode BootReceiver::constructor()
{
    return NOERROR;
}

ECode BootReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<MyThread> mythread = new MyThread(this, context);
    mythread->constructor();
    mythread->Start();
    return NOERROR;
}

ECode BootReceiver::RemoveOldUpdatePackages(
    /* [in] */ IContext* context)
{
    assert(0 && "TODO");
    // AutoPtr<IDownloads> downloads;
    // CDownloads::AcquireSingleton((IDownloads**)&downloads);
    // downloads->RemoveAllDownloadsByPackage(context, OLD_UPDATER_PACKAGE, OLD_UPDATER_CLASS);
    return NOERROR;
}

ECode BootReceiver::LogBootEvents(
    /* [in] */ IContext* ctx)
{
    AutoPtr<IInterface> obj;
    ctx->GetSystemService(IContext::DROPBOX_SERVICE, (IInterface**)&obj);
    AutoPtr<IDropBoxManager> db = IDropBoxManager::Probe(obj);
    AutoPtr<ISharedPreferences> prefs;
    ctx->GetSharedPreferences(String("log_files"), IContext::MODE_PRIVATE, (ISharedPreferences**)&prefs);

    StringBuilder sb(512);
    sb.Append("Build: ");
    sb.Append(Build::FINGERPRINT);
    sb.Append("\n");
    sb.Append("Hardware: ");
    sb.Append(Build::BOARD);
    sb.Append("\n");
    sb.Append("Revision: ");
    String revision;
    SystemProperties::Get(String("ro.revision"), String(""), &revision);
    sb.Append(revision);
    sb.Append("\n");
    sb.Append("Bootloader: ");
    sb.Append(Build::BOOTLOADER);
    sb.Append("\n");
    sb.Append("Radio: ");
    sb.Append(Build::RADIO);
    sb.Append("\n");
    sb.Append("Kernel: ");
    AutoPtr<IFile> file;
    CFile::New(String("/proc/version"), (IFile**)&file);
    FileUtils::ReadTextFile(file, 1024, String("...\n"), &revision);
    sb.Append(revision);
    sb.Append("\n");

    String headers(sb.ToString());
    String bootReason;
    SystemProperties::Get(String("ro.boot.bootreason"), String(NULL), &bootReason);

    assert(0 && "TODO");
    // String recovery = RecoverySystem::HandleAftermath();
    // if (!recovery.IsNull() && db != NULL) {
    //     db->AddText(String("SYSTEM_RECOVERY_LOG"), headers + recovery);
    // }

    String lastKmsgFooter("");
    if (bootReason != NULL) {
        StringBuilder sb(512);
        sb.Append("\n");
        sb.Append("Boot info:\n");
        sb.Append("Last boot reason: ");
        sb.Append(bootReason);
        sb.Append("\n");
        lastKmsgFooter = sb.ToString();
    }

    Int64 ival;
    SystemProperties::GetInt64(String("ro.runtime.firstboot"), 0, &ival);
    if (ival == 0) {
        String str1, str2;
        SystemProperties::Get(String("ro.crypto.state"), &str1);
        SystemProperties::Get(String("vold.decrypt"), &str2);
        if (str1.Equals("encrypted")
            && str2.Equals("trigger_restart_min_framework")) {
            // Encrypted, first boot to get PIN/pattern/password so data is tmpfs
            // Don't set ro.runtime.firstboot so that we will do this again
            // when data is properly mounted
        }
        else {
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            system->GetCurrentTimeMillis(&ival);
            String now = StringUtils::ToString(ival);
            SystemProperties::Set(String("ro.runtime.firstboot"), now);
        }
        if (db != NULL) db->AddText(String("SYSTEM_BOOT"), headers);

        // Negative sizes mean to take the *tail* of the file (see FileUtils.readTextFile())
        AddFileWithFootersToDropBox(
            db, prefs, headers, lastKmsgFooter,
            String("/proc/last_kmsg"), -LOG_SIZE, String("SYSTEM_LAST_KMSG"));
        AddFileWithFootersToDropBox(
            db, prefs, headers, lastKmsgFooter,
            String("/sys/fs/pstore/console-ramoops"), -LOG_SIZE,
            String("SYSTEM_LAST_KMSG"));
        AddFileToDropBox(
            db, prefs, headers, String("/cache/recovery/log"),
            -LOG_SIZE, String("SYSTEM_RECOVERY_LOG"));
        AddFileToDropBox(
            db, prefs, headers, String("/data/dontpanic/apanic_console"),
            -LOG_SIZE, String("APANIC_CONSOLE"));
        AddFileToDropBox(db, prefs, headers,
            String("/data/dontpanic/apanic_threads"),
            -LOG_SIZE, String("APANIC_THREADS"));
        AddAuditErrorsToDropBox(db, prefs, headers, -LOG_SIZE, String("SYSTEM_AUDIT"));
        AddFsckErrorsToDropBox(db, prefs, headers, -LOG_SIZE, String("SYSTEM_FSCK"));
    } else {
        if (db != NULL) db->AddText(String("SYSTEM_RESTART"), headers);
    }

    // Scan existing tombstones (in case any new ones appeared)
    AutoPtr< ArrayOf<IFile*> > tombstoneFiles;
    TOMBSTONE_DIR->ListFiles((ArrayOf<IFile*>**)&tombstoneFiles);
    Boolean isFile;
    for (Int32 i = 0; tombstoneFiles != NULL && i < tombstoneFiles->GetLength(); i++) {
        (*tombstoneFiles)[i]->IsFile(&isFile);
        if (isFile) {
            String path;
            (*tombstoneFiles)[i]->GetPath(&path);
            AddFileToDropBox(db, prefs, headers, path, LOG_SIZE, String("SYSTEM_TOMBSTONE"));
        }
    }

    // Start watching for new tombstone files; will record them as they occur.
    // This gets registered with the singleton file observer thread.
    String dir;
    TOMBSTONE_DIR->GetPath(&dir);
    sTombstoneObserver = new TombstoneObserver(dir, IFileObserver::CLOSE_WRITE, db, prefs, headers, this);

    sTombstoneObserver->StartWatching();
    return NOERROR;
}

ECode BootReceiver::AddFileToDropBox(
    /* [in] */ IDropBoxManager* db,
    /* [in] */ ISharedPreferences* prefs,
    /* [in] */ const String& headers,
    /* [in] */ const String& filename,
    /* [in] */ Int32 maxSize,
    /* [in] */ const String& tag)
{
    return AddFileWithFootersToDropBox(db, prefs, headers, String(""), filename, maxSize, tag);
}

ECode BootReceiver::AddFileWithFootersToDropBox(
    /* [in] */ IDropBoxManager* db,
    /* [in] */ ISharedPreferences* prefs,
    /* [in] */ const String& headers,
    /* [in] */ const String& footers,
    /* [in] */ const String& filename,
    /* [in] */ Int32 maxSize,
    /* [in] */ const String& tag)
{
    Boolean bval;
    if (db == NULL || (db->IsTagEnabled(tag, &bval), !bval)) return E_NULL_POINTER_EXCEPTION;  // Logging disabled

    AutoPtr<IFile> file;
    CFile::New(filename, (IFile**)&file);
    Int64 fileTime;
    file->GetLastModified(&fileTime);
    if (fileTime <= 0) return E_NULL_POINTER_EXCEPTION;  // File does not exist

    if (prefs != NULL) {
        Int64 lastTime;
        prefs->GetInt64(filename, 0, &lastTime);
        if (lastTime == fileTime) return E_NULL_POINTER_EXCEPTION;  // Already logged this particular file
        // TODO: move all these SharedPreferences Editor commits
        // outside this function to the end of logBootEvents
        AutoPtr<ISharedPreferencesEditor> editor;
        prefs->Edit((ISharedPreferencesEditor**)&editor);
        editor->PutInt64(filename, fileTime);
        editor->Apply();
    }

    Slogger::I(TAG, "Copying %s to DropBox (%s)", filename.string(), tag.string());
    String str;
    FileUtils::ReadTextFile(file, maxSize, String("[[TRUNCATED]]\n"), &str);
    return db->AddText(tag, headers + str + footers);
}

ECode BootReceiver::AddAuditErrorsToDropBox(
    /* [in] */ IDropBoxManager* db,
    /* [in] */ ISharedPreferences* prefs,
    /* [in] */ const String& headers,
    /* [in] */ Int32 maxSize,
    /* [in] */ const String& tag)
{
    Boolean bval;
    if (db == NULL || (db->IsTagEnabled(tag, &bval), !bval)) return E_NULL_POINTER_EXCEPTION;  // Logging disabled
    Slogger::I(TAG, "Copying audit failures to DropBox");

    AutoPtr<IFile> file;
    CFile::New(String("/proc/last_kmsg"), (IFile**)&file);
    Int64 fileTime;
    file->GetLastModified(&fileTime);

    if (fileTime <= 0) {
        file = NULL;
        CFile::New(String("/sys/fs/pstore/console-ramoops"), (IFile**)&file);
        file->GetLastModified(&fileTime);
    }

    if (fileTime <= 0) return E_NULL_POINTER_EXCEPTION;  // File does not exist

    if (prefs != NULL) {
        Int64 lastTime;
        prefs->GetInt64(tag, 0, &lastTime);
        if (lastTime == fileTime) return E_NULL_POINTER_EXCEPTION;  // Already logged this particular file
        // TODO: move all these SharedPreferences Editor commits
        // outside this function to the end of logBootEvents
        AutoPtr<ISharedPreferencesEditor> editor;
        prefs->Edit((ISharedPreferencesEditor**)&editor);
        editor->PutInt64(tag, fileTime);
        editor->Apply();
    }

    String log;
    FileUtils::ReadTextFile(file, maxSize, String("[[TRUNCATED]]\n"), &log);
    AutoPtr<ArrayOf<String> > splits;
    StringUtils::Split(log, "\n", (ArrayOf<String>**)&splits);
    StringBuilder sb;
    for (Int32 i = 0; i < splits->GetLength(); ++i) {
        if ((*splits)[i].Contains("audit")) {
            sb.Append((*splits)[i]);
            sb.Append("\n");
        }
    }

    String info(sb.ToString());
    Slogger::I(TAG, "Copied %d worth of audits to DropBox", info.GetLength());
    return db->AddText(tag, headers + info);
}

ECode BootReceiver::AddFsckErrorsToDropBox(
    /* [in] */ IDropBoxManager* db,
    /* [in] */ ISharedPreferences* prefs,
    /* [in] */ const String& headers,
    /* [in] */ Int32 maxSize,
    /* [in] */ const String& tag)
{
    Boolean bval;
    if (db == NULL || (db->IsTagEnabled(tag, &bval), !bval)) return E_NULL_POINTER_EXCEPTION;  // Logging disabled

    Slogger::I(TAG, "Checking for fsck errors");

    Boolean upload_needed = FALSE;

    AutoPtr<IFile> file;
    CFile::New(String("/dev/fscklogs/log"), (IFile**)&file);
    Int64 fileTime;
    file->GetLastModified(&fileTime);
    if (fileTime <= 0) return E_NULL_POINTER_EXCEPTION;  // File does not exist

    String log;
    FileUtils::ReadTextFile(file, maxSize, String("[[TRUNCATED]]\n"), &log);
    AutoPtr<ArrayOf<String> > splits;
    StringUtils::Split(log, "\n", (ArrayOf<String>**)&splits);
    StringBuilder sb;
    for (Int32 i = 0; i < splits->GetLength(); ++i) {
        if ((*splits)[i].Contains("FILE SYSTEM WAS MODIFIED")) {
            upload_needed = TRUE;
            break;
        }
    }


    if (upload_needed) {
        AddFileToDropBox(db, prefs, headers, String("/dev/fscklogs/log"), maxSize, tag);
    }

    // Remove the file so we don't re-upload if the runtime restarts.
    return file->Delete();
}

} // namespace Server
} // namespace Droid
} // namespace Elastos
