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

#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Database.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Zip.h>
#include "elastos/droid/settingsprovider/SettingsBackupAgent.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/provider/Settings.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::ISettingsNameValueTable;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::Settings;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuffer;
using Elastos::Core::Thread;
using Elastos::IO::ICharArrayReader;
using Elastos::IO::CCharArrayReader;
using Elastos::IO::ICloseable;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::CBufferedReader;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::CFile;
using Elastos::IO::IFileReader;
using Elastos::IO::CFileReader;
using Elastos::IO::IFileWriter;
using Elastos::IO::CFileWriter;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataOutput;
using Elastos::IO::IFlushable;
using Elastos::IO::CBufferedWriter;
using Elastos::IO::IReader;
using Elastos::Utility::ISet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Zip::IChecksum;
using Elastos::Utility::Zip::ICRC32;
using Elastos::Utility::Zip::CCRC32;

namespace Elastos {
namespace Droid {
namespace SettingsProvider {

const String SettingsBackupAgent::TAG("SettingsBackupAgent");
const Boolean SettingsBackupAgent::DEBUG = TRUE;
const Boolean SettingsBackupAgent::DEBUG_BACKUP = TRUE;
const String SettingsBackupAgent::KEY_SYSTEM("system");
const String SettingsBackupAgent::KEY_SECURE("secure");
const String SettingsBackupAgent::KEY_GLOBAL("global");
const String SettingsBackupAgent::KEY_LOCALE("locale");
const Int32 SettingsBackupAgent::STATE_SIZES[] = {
    0,
    4,
    5,
    STATE_SIZE
};

static AutoPtr<ArrayOf<Byte> > InitEmptyData()
{
    AutoPtr<ArrayOf<Byte> > b = ArrayOf<Byte>::Alloc(0);
    return b;
}
const AutoPtr<ArrayOf<Byte> > SettingsBackupAgent::EMPTY_DATA = InitEmptyData();

static AutoPtr<ArrayOf<String> > InitProjection()
{
    AutoPtr<ArrayOf<String> > b = ArrayOf<String>::Alloc(3);
    b->Set(0, String("_id"));
    b->Set(1, String("name"));
    b->Set(2, String("value"));
    return b;
}
const AutoPtr<ArrayOf<String> > SettingsBackupAgent::PROJECTION = InitProjection();

const String SettingsBackupAgent::FILE_WIFI_SUPPLICANT("/data/misc/wifi/wpa_supplicant.conf");
const String SettingsBackupAgent::FILE_WIFI_SUPPLICANT_TEMPLATE("/system/etc/wifi/wpa_supplicant.conf");
const String SettingsBackupAgent::KEY_WIFI_SUPPLICANT("\uffedWIFI");
const String SettingsBackupAgent::KEY_WIFI_CONFIG("\uffedCONFIG_WIFI");
const String SettingsBackupAgent::STAGE_FILE("flattened-data");

//============================================================================
//                  SettingsBackupAgent::NetWork
//============================================================================
SettingsBackupAgent::NetWork::NetWork()
    : mSsid("")
    , mKey_mgmt("")
    , mCertUsed(FALSE)
{}

SettingsBackupAgent::NetWork::~NetWork()
{}

AutoPtr<SettingsBackupAgent::NetWork> SettingsBackupAgent::NetWork::ReadFromStream(
    /* [in] */ IBufferedReader* in)
{
    AutoPtr<NetWork> n = new NetWork();
    String line;
    //try{
    ECode ec = NOERROR;
    Boolean ready;
    while (IReader::Probe(in)->IsReady(&ready), ready) {
        FAIL_GOTO((ec = in->ReadLine(&line)), _EXIT_);
        if (line.IsNull() || line.StartWith("}")) {
            break;
        }
        n->RememberLine(line);
    }
    //} catch(IOException e) {
    //return null;
    //}
_EXIT_:
    if (FAILED(ec)) {
        Logger::E(TAG, "ReadFromStream has error");
        return NULL;
    }
    return n;
}

void SettingsBackupAgent::NetWork::RememberLine(
    /* [in] */ const String& line)
{
    // can't rely on particular whitespace patterns so strip leading/trailing
    String temp = line.Trim();
    if (temp.IsEmpty()) return;
    mRawLines.PushBack(temp);

    // remember the ssid and key_mgmt lines for duplicate culling
    if (temp.StartWith("ssid")) {
        mSsid = temp;
    }
    else if (temp.StartWith("key_mgmt")) {
        mKey_mgmt = temp;
    }
    else if (line.StartWith("client_cert=")) {
        mCertUsed = TRUE;
    }
    else if (line.StartWith("ca_cert=")) {
        mCertUsed = TRUE;
    }
    else if (line.StartWith("ca_path=")) {
        mCertUsed = TRUE;
    }
}

ECode SettingsBackupAgent::NetWork::Write(
    /* [in] */ IWriter* w)
{
    FAIL_RETURN(w->Write(String("\nnetwork={\n}")));
    List<String>::Iterator it = mRawLines.Begin();
    for ( ; it != mRawLines.End(); ++it) {
        FAIL_RETURN(w->Write(String("\t") + (*it) + String("\n")));
    }
    FAIL_RETURN(w->Write(String("}\n")));
    return NOERROR;
}

void SettingsBackupAgent::NetWork::Dump()
{
    Logger::V(TAG, "network={}");
    List<String>::Iterator it = mRawLines.Begin();
    for ( ; it != mRawLines.End(); ++it) {
        Logger::V(TAG, "\t %s ", (*it).string());
    }
    Logger::V(TAG, "}");
}

ECode SettingsBackupAgent::NetWork::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (TO_IINTERFACE(this) == IInterface::Probe(obj)) {
        *res = TRUE;
        return NOERROR;
    }

    AutoPtr<NetWork> network = (NetWork*)IObject::Probe(obj);

    *res = mSsid.Equals(network->mSsid) && mKey_mgmt.Equals(network->mKey_mgmt);
    return NOERROR;
}

ECode SettingsBackupAgent::NetWork::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);

    Int32 result = 17;
    result = 31 * result + mSsid.GetHashCode();
    result = 31 * result + mKey_mgmt.GetHashCode();

    *code = result;
    return NOERROR;
}

//============================================================================
//                  SettingsBackupAgent::WifiNetworkSettings
//============================================================================
void SettingsBackupAgent::WifiNetworkSettings::ReadNetworks(
    /* [in] */ IBufferedReader* in)
{
    //try{
    String line;
    Boolean ready;
    while (IReader::Probe(in)->IsReady(&ready), ready) {
        in->ReadLine(&line);
        if (!line.IsNull()) {
            // Parse out 'network=' decls so we can ignore duplicates
            if (line.StartWith("network")) {
                AutoPtr<NetWork> net = NetWork::ReadFromStream(in);
                if (mKnownNetworks.Find(net) == mKnownNetworks.End()) {
                    if (DEBUG_BACKUP) {
                        Logger::V(TAG, "Adding %s / %s", net->mSsid.string(),
                                net->mKey_mgmt.string());
                    }
                    mKnownNetworks.Insert(net);
                    mNetWorks.PushBack(net);
                }
                else {
                    if (DEBUG_BACKUP) {
                        Logger::V(TAG, "Dupe; skipped %s / %s", net->mSsid.string(),
                                net->mKey_mgmt.string());
                    }
                }
            }
        }
    }

    //} catch (IOException e) {
    //whatever happened, we're done now
    //}
}

ECode SettingsBackupAgent::WifiNetworkSettings::Write(
    /* [in] */ IWriter* w)
{
    List<AutoPtr<NetWork> >::Iterator it = mNetWorks.Begin();
    for (; it != mNetWorks.End(); ++it) {
        if ((*it)->mCertUsed) {
            // Networks that use certificates for authentication can't be restored
            // because the certificates they need don't get restored (because they
            // are stored in keystore, and can't be restored)
            continue;
        }

        FAIL_RETURN((*it)->Write(w));
    }
    return NOERROR;
}

void SettingsBackupAgent::WifiNetworkSettings::Dump()
{
    List<AutoPtr<NetWork> >::Iterator it = mNetWorks.Begin();
    for (; it != mNetWorks.End(); ++it) {
        (*it)->Dump();
    }
}

//============================================================================
//                  SettingsBackupAgent::WifiRestoreRunnable
//============================================================================

SettingsBackupAgent::WifiRestoreRunnable::WifiRestoreRunnable(
    /* [in] */ SettingsBackupAgent* host)
    :mHost(host)
{
}

void SettingsBackupAgent::WifiRestoreRunnable::IncorporateWifiSupplicant(
    /* [in] */ IBackupDataInput* data)
{
    Int32 size;
    data->GetDataSize(&size);
    mRestoredSupplicantData = ArrayOf<Byte>::Alloc(size);
    if (mRestoredSupplicantData->GetLength() < 0) return;
    //try{
    Int32 readSize;
    ECode ec = data->ReadEntityData(mRestoredSupplicantData, 0, size, &readSize);
    if (FAILED(ec)) {
        Logger::W(TAG, "Unable to read supplicant data ec[%p]",ec);
        mRestoredSupplicantData = NULL;
    }
    //} catch (IOException e) {
    // Log.w(TAG, "Unable to read supplicant data");
    //  mRestoredSupplicantData = null;
    //}
}

void SettingsBackupAgent::WifiRestoreRunnable::IncorporateWifiConfigFile(
    /* [in] */ IBackupDataInput* data)
{
    Int32 size;
    data->GetDataSize(&size);
    mRestoredWifiConfigFile = ArrayOf<Byte>::Alloc(size);
    if (mRestoredWifiConfigFile->GetLength() < 0) return;
    //try{
    Int32 readSize;
    ECode ec = data->ReadEntityData(mRestoredWifiConfigFile, 0, size, &readSize);
    if (FAILED(ec)) {
        Logger::W(TAG, "Unable to read config file ec[%p]",ec);
        mRestoredWifiConfigFile = NULL;
    }
    //} catch (IOException e) {
    // Log.w(TAG, "Unable to read config file");
    //  mRestoredSupplicantData = null;
    //}
}

ECode SettingsBackupAgent::WifiRestoreRunnable::Run()
{
    if (mRestoredSupplicantData != NULL || mRestoredWifiConfigFile != NULL) {
        if (DEBUG_BACKUP) {
            Logger::V(TAG, "Starting deferred restore of wifi data");
        }

        AutoPtr<IContentResolver> cr;
        assert(0 && "TODO");
        // mHost->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsGlobal> global;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
        Int32 scanAlways;
        global->GetInt32(cr, ISettingsGlobal::WIFI_SCAN_ALWAYS_AVAILABLE, 0, &scanAlways);

        Int32 retainedWifiState = mHost->EnableWifi(FALSE);
        Boolean res;
        if (scanAlways != 0) {
            global->PutInt32(cr,
                    ISettingsGlobal::WIFI_SCAN_ALWAYS_AVAILABLE, 0, &res);
        }
        // !!! Give the wifi stack a moment to quiesce
        // try {
        Thread::Sleep(1500);
        // } catch (InterruptedException e) {}
        if (mRestoredSupplicantData != NULL) {
            mHost->RestoreWifiSupplicant(FILE_WIFI_SUPPLICANT,
                    mRestoredSupplicantData, mRestoredSupplicantData->GetLength());
            FileUtils::SetPermissions(FILE_WIFI_SUPPLICANT, FileUtils::sS_IRUSR |
                    FileUtils::sS_IWUSR | FileUtils::sS_IRGRP |
                    FileUtils::sS_IWGRP, Process::MyUid(), IProcess::WIFI_UID);

        }
        if (mRestoredWifiConfigFile != NULL) {
            mHost->RestoreFileData(mHost->mWifiConfigFile, mRestoredWifiConfigFile,
                    mRestoredWifiConfigFile->GetLength());
        }

        // restore the previous WIFI state.
        if (scanAlways != 0) {
            global->PutInt32(cr,
                    ISettingsGlobal::WIFI_SCAN_ALWAYS_AVAILABLE, scanAlways, &res);
        }
        mHost->EnableWifi(retainedWifiState == IWifiManager::WIFI_STATE_ENABLED ||
                retainedWifiState == IWifiManager::WIFI_STATE_ENABLING);
    }
    return NOERROR;
}

//============================================================================
//                  SettingsBackupAgent
//============================================================================

SettingsBackupAgent::SettingsBackupAgent()
{}

SettingsBackupAgent::~SettingsBackupAgent()
{}

ECode SettingsBackupAgent::constructor()
{
    return NOERROR;
}

ECode SettingsBackupAgent::OnCreate()
{
    if (DEBUG_BACKUP) {
        Logger::D(TAG, "onCreate() invoked");
    }
    assert(0 && "TODO");
    // mSettingsHelper = new SettingsHelper(IContext::Probe(this));
    // BackupAgentHelper::OnCreate();

    AutoPtr<IInterface> service;
    // GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&service);
    AutoPtr<IWifiManager> temp = IWifiManager::Probe(service);
    if (temp != NULL) {
        temp->GetConfigFile(&mWifiConfigFile);
    }
    return NOERROR;
}

ECode SettingsBackupAgent::OnBackup(
    /* [in] */ IParcelFileDescriptor* oldState,
    /* [in] */ IBackupDataOutput* data,
    /* [in] */ IParcelFileDescriptor* newState)
{
    AutoPtr< ArrayOf<Byte> > systemSettingsData = GetSystemSettings();
    AutoPtr< ArrayOf<Byte> > secureSettingsData = GetSecureSettings();
    AutoPtr< ArrayOf<Byte> > globalSettingsData = GetGlobalSettings();
    AutoPtr< ArrayOf<Byte> > locale = mSettingsHelper->GetLocaleData();
    AutoPtr< ArrayOf<Byte> > wifiSupplicantData = GetWifiSupplicant(FILE_WIFI_SUPPLICANT);
    AutoPtr< ArrayOf<Byte> > wifiConfigData = GetFileData(mWifiConfigFile);
    AutoPtr< ArrayOf<Int64> > stateChecksums = ReadOldChecksums(oldState);

    (*stateChecksums)[STATE_SYSTEM] =
            WriteIfChanged((*stateChecksums)[STATE_SYSTEM], KEY_SYSTEM,
                systemSettingsData, data);
    (*stateChecksums)[STATE_SECURE] =
            WriteIfChanged((*stateChecksums)[STATE_SECURE], KEY_SECURE,
                secureSettingsData, data);
    (*stateChecksums)[STATE_GLOBAL] =
            WriteIfChanged((*stateChecksums)[STATE_GLOBAL], KEY_GLOBAL,
                globalSettingsData, data);
    (*stateChecksums)[STATE_LOCALE] =
            WriteIfChanged((*stateChecksums)[STATE_LOCALE], KEY_LOCALE,
                locale, data);
    (*stateChecksums)[STATE_WIFI_SUPPLICANT] =
            WriteIfChanged((*stateChecksums)[STATE_WIFI_SUPPLICANT],
                KEY_WIFI_SUPPLICANT, wifiSupplicantData, data);
    (*stateChecksums)[STATE_WIFI_CONFIG] =
            WriteIfChanged((*stateChecksums)[STATE_WIFI_CONFIG],
                KEY_WIFI_CONFIG, wifiConfigData, data);
    WriteNewChecksums(stateChecksums, newState);
    return NOERROR;
}

void SettingsBackupAgent::InitWifiRestoreIfNecessary()
{
    if (mWifiRestore == NULL) {
        mWifiRestore = new WifiRestoreRunnable(this);
    }
}

ECode SettingsBackupAgent::OnRestore(
    /* [in] */ IBackupDataInput* data,
    /* [in] */ Int32 appVersionCode,
    /* [in] */ IParcelFileDescriptor* newState)
{
    if (DEBUG_BACKUP) {
        Logger::I(TAG, "OnRestore invoked");
    }
    AutoPtr<IHashSet> movedToGlobal;
    CHashSet::New((IHashSet**)&movedToGlobal);

    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);
    sys->GetMovedKeys(movedToGlobal);

    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    secure->GetMovedKeys(movedToGlobal);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

    Boolean next = FALSE;
    while (data->ReadNextHeader(&next), next) {
        String key;
        data->GetKey(&key);
        Int32 dataSize;
        data->GetDataSize(&dataSize);

        if (KEY_SYSTEM.Equals(key)) {
            AutoPtr<IUri> uri;
            sys->GetCONTENT_URI((IUri**)&uri);
            RestoreSettings(data, uri, movedToGlobal);
            mSettingsHelper->ApplyAudioSettings();
        }
        else if (KEY_SECURE.Equals(key)) {
            AutoPtr<IUri> uri;
            secure->GetCONTENT_URI((IUri**)&uri);
            RestoreSettings(data, uri, movedToGlobal);
        }
        else if (KEY_GLOBAL.Equals(key)) {
            AutoPtr<IUri> uri;
            global->GetCONTENT_URI((IUri**)&uri);
            RestoreSettings(data, uri, NULL);
        }
        else if (KEY_WIFI_SUPPLICANT.Equals(key)) {
            InitWifiRestoreIfNecessary();
            mWifiRestore->IncorporateWifiSupplicant(data);
        }
        else if (KEY_LOCALE.Equals(key)) {
            AutoPtr<ArrayOf<Byte> > localeData = ArrayOf<Byte>::Alloc(dataSize);
            Int32 readSize;
            data->ReadEntityData(localeData, 0, dataSize, &readSize);
            mSettingsHelper->SetLocaleData(localeData, dataSize);
        }
        else if (KEY_WIFI_CONFIG.Equals(key)) {
            InitWifiRestoreIfNecessary();
            mWifiRestore->IncorporateWifiConfigFile(data);
        }
        else {
            data->SkipEntityData();
        }
    }

    // If we have wifi data to restore, post a runnable to perform the
    // bounce-and-update operation a little ways in the future.
    if (mWifiRestore != NULL) {
        AutoPtr<IContentResolver> resolver;
        assert(0 && "TODO");
        // GetContentResolver((IContentResolver**)&resolver);
        Int64 wifiBounceDelayMillis;
        global->GetInt64(resolver,
                ISettingsGlobal::WIFI_BOUNCE_DELAY_OVERRIDE_MS,
                WIFI_BOUNCE_DELAY_MILLIS, &wifiBounceDelayMillis);

        AutoPtr<ILooper> looper;
        assert(0 && "TODO");
        // GetMainLooper((ILooper**)&looper);
        AutoPtr<IHandler> handler;
        CHandler::New(looper, (IHandler**)&handler);
        Boolean result;
        handler->PostDelayed(mWifiRestore, wifiBounceDelayMillis, &result);
    }

    return NOERROR;
}

ECode SettingsBackupAgent::OnFullBackup(
    /* [in] */ IFullBackupDataOutput* data)
{
    AutoPtr< ArrayOf<Byte> > systemSettingsData = GetSystemSettings();
    AutoPtr< ArrayOf<Byte> > secureSettingsData = GetSecureSettings();
    AutoPtr< ArrayOf<Byte> > globalSettingsData = GetGlobalSettings();
    AutoPtr< ArrayOf<Byte> > locale = mSettingsHelper->GetLocaleData();
    AutoPtr< ArrayOf<Byte> > wifiSupplicantData = GetWifiSupplicant(FILE_WIFI_SUPPLICANT);
    AutoPtr< ArrayOf<Byte> > wifiConfigData = GetFileData(mWifiConfigFile);

    // Write the data to the staging file, then emit that as our tarfile
    // representation of the backed-up settings.
    AutoPtr<IFile> dir;
        assert(0 && "TODO");
    // GetFilesDir((IFile**)&dir);
    String root;
    dir->GetAbsolutePath(&root);

    AutoPtr<IFile> stage;
    CFile::New(root, STAGE_FILE, (IFile**)&stage);
    //try{
    AutoPtr<IFileOutputStream> filestream;
    CFileOutputStream::New(stage, (IFileOutputStream**)&filestream);
    AutoPtr<IBufferedOutputStream> bufstream;
    CBufferedOutputStream::New(IOutputStream::Probe(filestream), (IBufferedOutputStream**)&bufstream);
    AutoPtr<IDataOutputStream>  out;
    CDataOutputStream::New(IOutputStream::Probe(bufstream), (IDataOutputStream**)&out);

    AutoPtr<IDataOutput> dataOutput = IDataOutput::Probe(out);

    if (DEBUG_BACKUP) Logger::D(TAG, "Writing flattened data version %d",
            FULL_BACKUP_VERSION);
    dataOutput->WriteInt32(FULL_BACKUP_VERSION);

    if (DEBUG_BACKUP) Logger::D(TAG, "%d bytes of settings data",
            systemSettingsData->GetLength());
    dataOutput->WriteInt32(systemSettingsData->GetLength());
    dataOutput->Write(systemSettingsData);

    if (DEBUG_BACKUP) Logger::D(TAG, "%d bytes of secure settings data",
            secureSettingsData->GetLength());
    dataOutput->WriteInt32(secureSettingsData->GetLength());
    dataOutput->Write(secureSettingsData);

    if (DEBUG_BACKUP) Logger::D(TAG, "%d bytes of global settings data",
            globalSettingsData->GetLength());
    dataOutput->WriteInt32(globalSettingsData->GetLength());
    dataOutput->Write(globalSettingsData);

    if (DEBUG_BACKUP) Logger::D(TAG, "%d bytes of locale data",
            locale->GetLength());
    dataOutput->WriteInt32(locale->GetLength());
    dataOutput->Write(locale);

    if (DEBUG_BACKUP) Logger::D(TAG, "%d bytes of wifi supplicant data",
            wifiSupplicantData->GetLength());
    dataOutput->WriteInt32(wifiSupplicantData->GetLength());
    dataOutput->Write(wifiSupplicantData);

    if (DEBUG_BACKUP) Logger::D(TAG, "%d bytes of wifi config data",
            wifiConfigData->GetLength());
    dataOutput->WriteInt32(wifiConfigData->GetLength());
    dataOutput->Write(wifiConfigData);

    IFlushable::Probe(out)->Flush();

    // now we're set to emit the tar stream
    assert(0 && "TODO");
    // FullBackupFile(stage, data);

    Boolean bTemp;
    stage->Delete(&bTemp);
    //}finally {
    // stage.delete();
    //}

    return NOERROR;
}

ECode SettingsBackupAgent::OnRestoreFile(
    /* [in] */ IParcelFileDescriptor* data,
    /* [in] */ Int64 size,
    /* [in] */ IFile* destination,
    /* [in] */ Int32 type,
    /* [in] */ Int64 mode,
    /* [in] */ Int64 mtime)
{
    if (DEBUG_BACKUP) Logger::D(TAG, "onRestoreFile() invoked");
    // Our data is actually a blob of flattened settings data identical to that
    // produced during incremental backups.  Just unpack and apply it all in
    // turn.
    AutoPtr<IFileDescriptor> fd;
    data->GetFileDescriptor((IFileDescriptor**)&fd);
    AutoPtr<IFileInputStream> instream;
    CFileInputStream::New(fd, (IFileInputStream**)&instream);
    AutoPtr<IDataInputStream> in;
    CDataInputStream::New(IInputStream::Probe(instream), (IDataInputStream**)&in);

    Int32 version;
    IDataInput::Probe(in)->ReadInt32(&version);
    if (DEBUG_BACKUP) Logger::D(TAG, "Flattened data version %d", version);
    if (version <= FULL_BACKUP_VERSION) {
        // Generate the moved-to-global lookup table
        AutoPtr<IHashSet> movedToGlobal;
        CHashSet::New((IHashSet**)&movedToGlobal);

        AutoPtr<ISettingsSystem> sys;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);
        sys->GetMovedKeys(movedToGlobal);

        AutoPtr<ISettingsSecure> secure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
        secure->GetMovedKeys(movedToGlobal);

        AutoPtr<ISettingsGlobal> global;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

        // system settings data first
        Int32 nBytes;
        IDataInput::Probe(in)->ReadInt32(&nBytes);
        if (DEBUG_BACKUP) Logger::D(TAG, "%d bytes of settings data", nBytes);
        AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(nBytes);
        IDataInput::Probe(in)->ReadFully(buffer, 0, nBytes);
        AutoPtr<IUri> uri;
        sys->GetCONTENT_URI((IUri**)&uri);
        RestoreSettings(buffer, nBytes, uri, movedToGlobal);

        // secure settings
        IDataInput::Probe(in)->ReadInt32(&nBytes);
        if (DEBUG_BACKUP) Logger::D(TAG, "%d  bytes of secure settings data", nBytes);
        if (nBytes > buffer->GetLength()) {
            buffer = NULL;
            buffer = ArrayOf<Byte>::Alloc(nBytes);
        }
        IDataInput::Probe(in)->ReadFully(buffer, 0, nBytes);
        uri = NULL;
        secure->GetCONTENT_URI((IUri**)&uri);
        RestoreSettings(buffer, nBytes, uri, movedToGlobal);

        // Global only if sufficiently new
        if (version >= FULL_BACKUP_ADDED_GLOBAL) {
            IDataInput::Probe(in)->ReadInt32(&nBytes);
            if (DEBUG_BACKUP) Logger::D(TAG, "%d bytes of global settings data", nBytes);
            if (nBytes > buffer->GetLength()) {
                buffer = NULL;
                buffer = ArrayOf<Byte>::Alloc(nBytes);
            }
            IDataInput::Probe(in)->ReadFully(buffer, 0, nBytes);
            movedToGlobal->Clear();
            uri = NULL;
            global->GetCONTENT_URI((IUri**)&uri);
            RestoreSettings(buffer, nBytes, uri, movedToGlobal);
        }

        // locale
        IDataInput::Probe(in)->ReadInt32(&nBytes);
        if (DEBUG_BACKUP) Logger::D(TAG, "%d bytes of locale data", nBytes);
        if (nBytes > buffer->GetLength()) {
            buffer = NULL;
            buffer = ArrayOf<Byte>::Alloc(nBytes);
        }
        IDataInput::Probe(in)->ReadFully(buffer, 0, nBytes);
        mSettingsHelper->SetLocaleData(buffer, nBytes);

        // wifi supplicant
        IDataInput::Probe(in)->ReadInt32(&nBytes);
        if (DEBUG_BACKUP) Logger::D(TAG, "%d bytes of wifi supplicant data", nBytes);
        if (nBytes > buffer->GetLength()) {
            buffer = NULL;
            buffer = ArrayOf<Byte>::Alloc(nBytes);
        }
        IDataInput::Probe(in)->ReadFully(buffer, 0, nBytes);
        Int32 retainedWifiState = EnableWifi(FALSE);
        RestoreWifiSupplicant(FILE_WIFI_SUPPLICANT, buffer, nBytes);
        FileUtils::SetPermissions(FILE_WIFI_SUPPLICANT,
                FileUtils::sS_IRUSR | FileUtils::sS_IWUSR |
                FileUtils::sS_IRGRP | FileUtils::sS_IWGRP,
                Process::MyUid(), IProcess::WIFI_UID);
        // retain the previous WIFI state.
        EnableWifi(retainedWifiState == IWifiManager::WIFI_STATE_ENABLED ||
                retainedWifiState == IWifiManager::WIFI_STATE_ENABLING);

        // wifi config
        IDataInput::Probe(in)->ReadInt32(&nBytes);
        if (DEBUG_BACKUP) Logger::D(TAG, "%d bytes of wifi config data", nBytes);
        if (nBytes > buffer->GetLength()) {
            buffer = NULL;
            buffer = ArrayOf<Byte>::Alloc(nBytes);
        }
        IDataInput::Probe(in)->ReadFully(buffer, 0, nBytes);
        RestoreFileData(mWifiConfigFile, buffer, nBytes);

        if (DEBUG_BACKUP) Logger::D(TAG, "Full restore complete.");
    }
    else {
        data->Close();
        Logger::E(TAG, "Invalid file schema");
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

AutoPtr<ArrayOf<Int64> > SettingsBackupAgent::ReadOldChecksums(
    /* [in] */ IParcelFileDescriptor* oldState)
{
    AutoPtr<ArrayOf<Int64> > stateChecksums = ArrayOf<Int64>::Alloc(STATE_SIZE);
    AutoPtr<IFileDescriptor> fd;
    oldState->GetFileDescriptor((IFileDescriptor**)&fd);
    AutoPtr<IFileInputStream> fileInput;
    CFileInputStream::New(fd, (IFileInputStream**)&fileInput);
    AutoPtr<IDataInputStream> dataInput;
    CDataInputStream::New(IInputStream::Probe(fileInput), (IDataInputStream**)&dataInput);

    //try{
    Int32 stateVersion;
    ECode ec = NOERROR;
    FAIL_GOTO((ec = IDataInput::Probe(dataInput)->ReadInt32(&stateVersion)), _EXIT_);
    for (Int32 i = 0; i < STATE_SIZES[stateVersion]; ++i) {
        FAIL_GOTO((ec = IDataInput::Probe(dataInput)->ReadInt64(&((*stateChecksums)[i]))), _EXIT_);
    }
    //}catch (EOFException eof) {
    //With the default 0 checksum we'll wind up forcing a backup of
    //any unhandled data sets, which is appropriate.
    //}
_EXIT_:
    ICloseable::Probe(dataInput)->Close();
    return stateChecksums;
}

void SettingsBackupAgent::WriteNewChecksums(
    /* [in] */ ArrayOf<Int64>* checksums,
    /* [in] */ IParcelFileDescriptor* newState)
{
    AutoPtr<IFileDescriptor> fd;
    newState->GetFileDescriptor((IFileDescriptor**)&fd);
    AutoPtr<IFileOutputStream> fileOutput;
    CFileOutputStream::New(fd, (IFileOutputStream**)&fileOutput);
    AutoPtr<IDataOutputStream> dataOutput;
    CDataOutputStream::New(IOutputStream::Probe(fileOutput), (IDataOutputStream**)&dataOutput);
    IDataOutput::Probe(dataOutput)->WriteInt32(STATE_VERSION);
    for (Int32 i = 0; i < STATE_SIZE; ++i) {
        IDataOutput::Probe(dataOutput)->WriteInt64((*checksums)[i]);
    }
    ICloseable::Probe(dataOutput)->Close();
}

Int64 SettingsBackupAgent::WriteIfChanged(
    /* [in] */ Int64 oldChecksums,
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IBackupDataOutput* output)
{
    AutoPtr<ICRC32> checkSummer;
    CCRC32::New((ICRC32**)&checkSummer);
    IChecksum::Probe(checkSummer)->Update(data);
    Int64 newChecksum;
    IChecksum::Probe(checkSummer)->GetValue(&newChecksum);
    if (oldChecksums == newChecksum) {
        return oldChecksums;
    }
    Int32 writtenSize;
    output->WriteEntityHeader(key, data->GetLength(), &writtenSize);
    output->WriteEntityData(data, data->GetLength(), &writtenSize);
    return newChecksum;
}

AutoPtr<ArrayOf<Byte> > SettingsBackupAgent::GetSystemSettings()
{
    AutoPtr<IContentResolver> resolver;
    assert(0 && "TODO");
    // GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);
    AutoPtr<IUri> uri;
    sys->GetCONTENT_URI((IUri**)&uri);
    AutoPtr<ICursor> cursor;
    resolver->Query(uri, PROJECTION, String(NULL),
            NULL, String(NULL), (ICursor**)&cursor);

    AutoPtr<ArrayOf<Byte> > result;
    ECode ec = NOERROR;
    ec = ExtractRelevantValues(cursor, Settings::System::SETTINGS_TO_BACKUP, (ArrayOf<Byte>**)&result);
    if (FAILED(ec)) {
        Logger::E(TAG, "GetSystemSettings has error:%p",ec);
    }
    ICloseable::Probe(cursor)->Close();
    return result;
}

AutoPtr<ArrayOf<Byte> > SettingsBackupAgent::GetSecureSettings()
{
    AutoPtr<IContentResolver> resolver;
    assert(0 && "TODO");
    // GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    AutoPtr<IUri> uri;
    secure->GetCONTENT_URI((IUri**)&uri);
    AutoPtr<ICursor> cursor;
    resolver->Query(uri, PROJECTION, String(NULL),
            NULL, String(NULL), (ICursor**)&cursor);
    AutoPtr<ArrayOf<Byte> > result;
    ECode ec = NOERROR;
    AutoPtr<ArrayOf<String> > backups;
    secure->GetSETTINGS_TO_BACKUP((ArrayOf<String>**)&backups);
    ec = ExtractRelevantValues(cursor, backups, (ArrayOf<Byte>**)&result);
    if (FAILED(ec)) {
        Logger::E(TAG, "GetSecureSettings has error:%p",ec);
    }
    ICloseable::Probe(cursor)->Close();
    return result;
}

AutoPtr<ArrayOf<Byte> > SettingsBackupAgent::GetGlobalSettings()
{
    AutoPtr<IContentResolver> resolver;
    assert(0 && "TODO");
    // GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IUri> uri;
    global->GetCONTENT_URI((IUri**)&uri);
    AutoPtr<ICursor> cursor;
    resolver->Query(uri, PROJECTION, String(NULL),
            NULL, String(NULL), (ICursor**)&cursor);
    AutoPtr<ArrayOf<Byte> > result;
    ECode ec = NOERROR;
    AutoPtr<ArrayOf<String> > backups;
    global->GetSETTINGS_TO_BACKUP((ArrayOf<String>**)&backups);
    ec = ExtractRelevantValues(cursor, backups, (ArrayOf<Byte>**)&result);
    if (FAILED(ec)) {
        Logger::E(TAG, "GetGlobalSettings has error:%p",ec);
    }
    ICloseable::Probe(cursor)->Close();
    return result;
}

void SettingsBackupAgent::RestoreSettings(
    /* [in] */ IBackupDataInput* data,
    /* [in] */ IUri* contentUri,
    /* [in] */ IHashSet* movedToGlobal)
{
    Int32 size;
    data->GetDataSize(&size);
    AutoPtr<ArrayOf<Byte> > settins = ArrayOf<Byte>::Alloc(size);
    //try{
    Int32 readSize;
    ECode ec = data->ReadEntityData(settins, 0, size, &readSize);
    if (FAILED(ec)) {
        Logger::E(TAG, "Couldn't read entity data:%p", ec);
        return;
    }
    RestoreSettings(settins, size, contentUri, movedToGlobal);

    //}  catch (IOException ioe) {
    //
    //}
}

ECode SettingsBackupAgent::RestoreSettings(
    /* [in] */ ArrayOf<Byte>* settings,
    /* [in] */ Int32 bytes,
    /* [in] */ IUri* contentUri,
    /* [in] */ IHashSet* movedToGlobal)
{
    if (DEBUG) {
        String str;
        IObject::Probe(contentUri)->ToString(&str);
        Logger::I(TAG, "restoreSettings:%s", str.string());
    }

    // Figure out the white list and redirects to the global table.
    AutoPtr<ArrayOf<String> > whitelist;
    Boolean isEquals;
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    AutoPtr<IUri> secureUri;
    secure->GetCONTENT_URI((IUri**)&secureUri);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IUri> globalUri;
    global->GetCONTENT_URI((IUri**)&globalUri);

    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);
    AutoPtr<IUri> systemUri;
    sys->GetCONTENT_URI((IUri**)&systemUri);
    if (IObject::Probe(contentUri)->Equals(secureUri, &isEquals), isEquals) {
        secure->GetSETTINGS_TO_BACKUP((ArrayOf<String>**)&whitelist);
    }
    else if (IObject::Probe(contentUri)->Equals(systemUri, &isEquals), isEquals) {
        whitelist = Settings::System::SETTINGS_TO_BACKUP;
    }
    else if (IObject::Probe(contentUri)->Equals(globalUri, &isEquals), isEquals) {
        global->GetSETTINGS_TO_BACKUP((ArrayOf<String>**)&whitelist);
    }
    else {
        String str;
        IObject::Probe(contentUri)->ToString(&str);
        Logger::E(TAG, "Unknown URI: %s", str.string());
        return  E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Restore only the white list data.
    Int32 pos = 0;
    AutoPtr<HashMap<String, String> > cacheEntries = new HashMap<String, String>();
    AutoPtr<IContentValues> contentValues;
    CContentValues::New(2, (IContentValues**)&contentValues);
    AutoPtr<SettingsHelper> settingsHelper = mSettingsHelper;

    Int32 whiteListSize = whitelist->GetLength();
    for (Int32 i = 0; i < whiteListSize; ++i) {
        String key = (*whitelist)[i];
        String value = (*cacheEntries)[key];
        cacheEntries->Erase(key);

        // If the value not cached, let us look it up.
        if (value.IsNull()) {
            while (pos < bytes) {
                Int32 length  = ReadInt32(settings, pos);
                pos += INTEGER_BYTE_COUNT;
                String dataKey(NULL);
                if (length > 0) {
                    const char* temp = (const char*)(&(*settings)[pos]);
                    dataKey = String(temp, length);
                }
                pos += length;
                length = ReadInt32(settings, pos);
                pos += INTEGER_BYTE_COUNT;
                String dataValue(NULL);
                if (length > 0) {
                    const char* temp = (const char*)(&(*settings)[pos]);
                    dataValue = String(temp, length);
                }
                pos += length;
                if (key.Equals(dataKey)) {
                    value = dataValue;
                    break;
                }
                (*cacheEntries)[dataKey] = dataValue;
            }
        }
        if (value.IsNull()) continue;

        AutoPtr<IUri> destination;
        Boolean res;
        if (movedToGlobal != NULL &&
                (ISet::Probe(movedToGlobal)->Contains(CoreUtils::Convert(key), &res), res)) {
            destination = globalUri;
        }
        else {
            destination = contentUri;
        }

        // The helper doesn't care what namespace the keys are in
        if (settingsHelper->RestoreValue(key, value)) {
            contentValues->Clear();
            contentValues->Put(ISettingsNameValueTable::NAME,
                    CoreUtils::Convert(key));
            contentValues->Put(ISettingsNameValueTable::VALUE,
                    CoreUtils::Convert(value));

            AutoPtr<IContentResolver> resolver;
            assert(0 && "TODO");
            // GetContentResolver((IContentResolver**)&resolver);
            AutoPtr<IUri> newUri;
            resolver->Insert(destination, contentValues, (IUri**)&newUri);
        }
        if (DEBUG) {
            String str;
            IObject::Probe(destination)->ToString(&str);
            Logger::D(TAG, "Restored setting: %s:%s=%s", str.string(),
                    key.string(), value.string());
        }
    }
    return NOERROR;
}

ECode SettingsBackupAgent::ExtractRelevantValues(
    /* [in] */ ICursor* cursor,
    /* [in] */ ArrayOf<String>* settings,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 settingsCount = settings->GetLength();
    AutoPtr<ArrayOf<ByteArray> > values = ArrayOf<ByteArray>::Alloc(settingsCount*2);
    Boolean moveToFirst = FALSE;
    if (cursor->MoveToFirst(&moveToFirst), !moveToFirst) {
        Logger::E(TAG, "Couldn't read from the cursor");
        *result = ArrayOf<Byte>::Alloc(0);
        return NOERROR;
    }

    // Obtain the relevant data in a temporary array.
    Int32 totalSize = 0;
    Int32 backedUpSettingIndex = 0;
    AutoPtr<HashMap< String, String> > cacheEntries = new HashMap<String, String>();
    for (Int32 i = 0; i < settingsCount; ++i) {
        String key = (*settings)[i];
        String value = (*cacheEntries)[key];
        cacheEntries->Erase(key);

        // If the value not cached, let us look it up.
        if (value.IsNull()) {
            Boolean isAfterLast = FALSE;
            while (cursor->IsAfterLast(&isAfterLast), !isAfterLast) {
                String cursorKey;
                String cursorValue;
                cursor->GetString(COLUMN_NAME, &cursorKey);
                cursor->GetString(COLUMN_VALUE, &cursorValue);
                Boolean bSuc;
                cursor->MoveToNext(&bSuc);
                if (key.Equals(cursorKey)) {
                    value = cursorValue;
                    break;
                }
                (*cacheEntries)[cursorKey] = cursorValue;
            }
        }

        // Intercept the keys and see if they need special handling
        value = mSettingsHelper->OnBackupValue(key, value);

        if (value.IsNull()) {
            continue;
        }
        // Write the key and value in the intermediary array.
        AutoPtr<ArrayOf<Byte> > keyBytes = key.GetBytes();
        totalSize += INTEGER_BYTE_COUNT + keyBytes->GetLength();
        values->Set(backedUpSettingIndex*2, keyBytes);

        AutoPtr<ArrayOf<Byte> > valueBytes = value.GetBytes();
        totalSize += INTEGER_BYTE_COUNT + valueBytes->GetLength();
        values->Set(backedUpSettingIndex*2 +1 , valueBytes);
        if (DEBUG) {
            Logger::D(TAG, "Backed up setting[%d]:index[%d] %s = %s keylen[%d] valuelen[%d]",
                    i, backedUpSettingIndex, key.string(), value.string(),
                    ((*values)[backedUpSettingIndex*2])->GetLength(),
                    ((*values)[backedUpSettingIndex*2 +1])->GetLength());
        }
        backedUpSettingIndex++;
    }

    // Aggregate the result.
    AutoPtr<ArrayOf<Byte> > temp = ArrayOf<Byte>::Alloc(totalSize);
    Int32 pos = 0;
    Int32 keyValuePairsCount = backedUpSettingIndex * 2;
    for (Int32 i = 0; i < keyValuePairsCount; ++i) {
        pos = WriteInt32(temp, pos, ((*values)[i])->GetLength());
        pos = WriteBytes(temp, pos, (*values)[i]);
    }
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > SettingsBackupAgent::GetFileData(
    /* [in] */ const String& filename)
{
    //try{
    ECode ec = NOERROR;
    AutoPtr<IFile> file;
    CFile::New(filename, (IFile**)&file);
    AutoPtr<IFileInputStream> input;
    CFileInputStream::New(file, (IFileInputStream**)&input);
    AutoPtr<IInputStream> is = IInputStream::Probe(input);

    //Will truncate read on a very long file,
    //should not happen for a config file
    Int64 len;
    file->GetLength(&len);
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc((Int32)len);
    Int32 offset = 0;
    Int32 numRead = 0;
    while (offset < bytes->GetLength() &&
            (ec = is->Read(bytes, offset, bytes->GetLength() - offset, &numRead), numRead >= 0)) {
        if (FAILED(ec)) {
            Logger::W(TAG, "Couldn't backup %s", filename.string());
            if (is != NULL) {
                is->Close();
            }
            return EMPTY_DATA;
        }
        offset += numRead;
    }

    //read failure
    if (offset < bytes->GetLength()) {
        Logger::W(TAG, "Couldn't backup %s", filename.string());
        if (is != NULL) {
            is->Close();
        }
        return EMPTY_DATA;
    }
    //}  catch (IOException ioe) {
    //
    //}
    if (is != NULL) {
        is->Close();
    }
    return  bytes;
}

void SettingsBackupAgent::RestoreFileData(
    /* [in] */ const String& filename,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 size)
{
    //try{
    AutoPtr<IFile> file;
    CFile::New(filename, (IFile**)&file);
    Boolean bExist = FALSE;
    if (file->Exists(&bExist), bExist) {
        Boolean temp;
        file->Delete(&temp);
    }
    AutoPtr<IFileOutputStream> fileoutput;
    CFileOutputStream::New(filename, true,(IFileOutputStream**)&fileoutput);
    AutoPtr<IBufferedOutputStream> bufferoutput;
    CBufferedOutputStream::New(IOutputStream::Probe(fileoutput), (IBufferedOutputStream**)&bufferoutput);
    AutoPtr<IOutputStream> os = IOutputStream::Probe(bufferoutput);
    os->Write(bytes, 0, size);
    os->Close();
    //}
}

AutoPtr<ArrayOf<Byte> > SettingsBackupAgent::GetWifiSupplicant(
    /* [in] */ const String& filename)
{
    //try{
    AutoPtr<IBufferedReader> br;
    ECode ec = NOERROR;
    AutoPtr<IFile> file;
    FAIL_GOTO((ec = CFile::New(filename, (IFile**)&file)), _Exit_);
    Boolean bExist;
    if (file->Exists(&bExist), bExist) {
        AutoPtr<IFileReader> fileReader;
        CFileReader::New(filename, (IFileReader**)&fileReader);
        CBufferedReader::New(IReader::Probe(fileReader), (IBufferedReader**)&br);
        StringBuffer relevantLines;
        Boolean started = FALSE;
        String line;
        while (ec = br->ReadLine(&line), !line.IsNull()) {
            if (FAILED(ec)) {
                goto _Exit_;
            }
            if (!started && line.StartWith("network")) {
                started = TRUE;
            }
            if (started) {
                relevantLines += line;
                relevantLines += "\n";
            }
        }
        Int32 len;
        if (relevantLines.GetLength(&len), len > 0) {
            String str;
            relevantLines.ToString(&str);
            if (br != NULL) {
                ICloseable::Probe(br)->Close();
            }
            return str.GetBytes();
        }
        else {
            if (br != NULL) {
                ICloseable::Probe(br)->Close();
            }
            return EMPTY_DATA;
        }
    }
    //} catch (IOException ioe) {
    //Log.w(TAG, "Couldn't backup " + filename);
    //return EMPTY_DATA;
    //}
_Exit_:
    if (br != NULL) {
        ICloseable::Probe(br)->Close();
    }
    if (FAILED(ec)) {
        Logger::W(TAG, "Couldn't backup %s", filename.string());
        return EMPTY_DATA;
    }
    if (DEBUG_BACKUP) {
        Logger::W(TAG, "GetWifiSupplicant has no file");
    }
    return EMPTY_DATA;
}

void SettingsBackupAgent::RestoreWifiSupplicant(
    /* [in] */ const String& filename,
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 size)
{
    //try{
    ECode ec = NOERROR;
    AutoPtr<WifiNetworkSettings> supplicantImage = new WifiNetworkSettings();
    AutoPtr<IFile> supplicantFile;
    AutoPtr<IFileWriter> filew;
    AutoPtr<IBufferedWriter> bw;
    FAIL_GOTO((ec = CFile::New(FILE_WIFI_SUPPLICANT, (IFile**)&supplicantFile)), _Exit_);
    Boolean bExist;
    if (supplicantFile->Exists(&bExist), bExist) {
        // Retain the existing APs; we'll append the restored ones to them
        AutoPtr<IBufferedReader> in;
        AutoPtr<IFileReader> fileReader;
        CFileReader::New(FILE_WIFI_SUPPLICANT, (IFileReader**)&fileReader);
        CBufferedReader::New(IReader::Probe(fileReader), (IBufferedReader**)&in);
        supplicantImage->ReadNetworks(in);
        ICloseable::Probe(in)->Close();
        Boolean temp;
        supplicantFile->Delete(&temp);
    }

    // Incorporate the restore AP information
    if (size > 0) {
        AutoPtr<ArrayOf<Char32> > restoredAsBytes = ArrayOf<Char32>::Alloc(size);
        for (Int32 i = 0; i < size; ++i) {
            (*restoredAsBytes)[i] = (Char32)(*bytes)[i];
        }
        AutoPtr<ICharArrayReader> charArray;
        CCharArrayReader::New(restoredAsBytes, (ICharArrayReader**)&charArray);
        AutoPtr<IBufferedReader> in;
        CBufferedReader::New(IReader::Probe(charArray), (IBufferedReader**)&in);
        supplicantImage->ReadNetworks(in);
        if (DEBUG_BACKUP) {
            Logger::V(TAG, "Final AP list:");
            supplicantImage->Dump();
        }
    }

    // Install the correct default template
    CFileWriter::New(FILE_WIFI_SUPPLICANT, (IFileWriter**)&filew);
    CBufferedWriter::New(IWriter::Probe(filew), (IBufferedWriter**)&bw);

    // Write the restored supplicant config and we're done
    supplicantImage->Write(IWriter::Probe(bw));
    ICloseable::Probe(bw)->Close();

    //} catch (IOException ioe) {
    //Log.w(TAG, "Couldn't restore " + filename);
    //}
_Exit_:
    if (FAILED(ec)) {
        Logger::W(TAG, "Couldn't restore %s", filename.string());
    }
}

void SettingsBackupAgent::CopyWifiSupplicantTemplate(
    /* [in] */ IBufferedWriter* bw)
{
    //try{
    ECode ec = NOERROR;
    AutoPtr<IFileReader> filer;
    CFileReader::New(FILE_WIFI_SUPPLICANT_TEMPLATE, (IFileReader**)&filer);
    AutoPtr<IBufferedReader> br;
    CBufferedReader::New(IReader::Probe(filer), (IBufferedReader**)&br);
    AutoPtr<ArrayOf<Char32> > temp = ArrayOf<Char32>::Alloc(1024);
    Int32 size;
    while ((ec = IReader::Probe(br)->Read(temp, &size)), (ec == NOERROR) && size > 0){
        FAIL_GOTO((ec = IWriter::Probe(bw)->Write(temp, 0, size)), _Exit_);
    }

    //}  catch (IOException ioe) {
    //Log.w(TAG, "Couldn't copy wpa_supplicant file");
    //}
_Exit_:
    if (br != NULL) {
        ICloseable::Probe(br)->Close();
    }
    if (FAILED(ec)) {
        Logger::W(TAG, "Couldn't copy wpa_supplicant file");
    }
}

Int32 SettingsBackupAgent::WriteInt32(
    /* [in] */ ArrayOf<Byte>* out,
    /* [in] */ Int32 pos,
    /* [in] */ Int32 value)
{
    (*out)[pos + 0] = (Byte)((value >> 24) & 0xFF);
    (*out)[pos + 1] = (Byte)((value >> 16) & 0xFF);
    (*out)[pos + 2] = (Byte)((value >> 8) & 0xFF);
    (*out)[pos + 3] = (Byte)((value >> 0) & 0xFF);
    return pos + INTEGER_BYTE_COUNT;
}

Int32 SettingsBackupAgent::WriteBytes(
    /* [in] */ ArrayOf<Byte>* out,
    /* [in] */ Int32 pos,
    /* [in] */ ArrayOf<Byte>* value)
{
    out->Copy(pos, value, value->GetLength());
    return pos + value->GetLength();
}

Int32 SettingsBackupAgent::ReadInt32(
    /* [in] */ ArrayOf<Byte>* in,
    /* [in] */ Int32 pos)
{
    Int32 result =
            (((*in)[pos    ] & 0xFF) << 24) |
            (((*in)[pos + 1] & 0xFF) << 16) |
            (((*in)[pos + 2] & 0xFF) << 8) |
            (((*in)[pos + 3] & 0xFF) << 0);
    return result;
}

Int32 SettingsBackupAgent::EnableWifi(
    /* [in] */ Boolean enable)
{
    if (mWfm == NULL) {
        AutoPtr<IInterface> service;
        assert(0 && "TODO");
        // GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&service);
        mWfm = IWifiManager::Probe(service);
    }
    if (mWfm != NULL) {
        Int32 state;
        mWfm->GetWifiState(&state);
        Boolean bTemp;
        mWfm->SetWifiEnabled(enable, &bTemp);
        return state;
    }
    else {
        Logger::E(TAG, "Failed to fetch WifiManager instance");
    }

    return IWifiManager::WIFI_STATE_UNKNOWN;
}

} // namespace SettingsProvider
} // namespace Droid
} // namespace Elastos
