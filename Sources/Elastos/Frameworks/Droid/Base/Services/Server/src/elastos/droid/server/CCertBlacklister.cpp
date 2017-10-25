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

#include "elastos/droid/server/CCertBlacklister.h"
#include <elastos/droid/os/FileUtils.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>

using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Net::IUri;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::IO::CFile;
using Elastos::IO::IFileHelper;
using Elastos::IO::CFileHelper;
using Elastos::IO::ICloseable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::Utility::Logging::Slogger;
using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;

namespace Elastos {
namespace Droid {
namespace Server {

static String InitBLACKLIST_ROOT()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String dataDir;
    system->GetEnv(String("ANDROID_DATA"), &dataDir);

    StringBuilder sb(dataDir);
    sb += "/misc/keychain/";
    return sb.ToString();
}

static String InitPath(
    /* [in] */ const char* path)
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String dataDir;
    system->GetEnv(String("ANDROID_DATA"), &dataDir);

    StringBuilder sb(dataDir);
    sb += "/misc/keychain/";
    sb += path;
    return sb.ToString();
}

const String CCertBlacklister::TAG("CertBlacklister");
const String CCertBlacklister::BLACKLIST_ROOT = InitBLACKLIST_ROOT();
const String CCertBlacklister::PUBKEY_PATH = InitPath("pubkey_blacklist.txt");
const String CCertBlacklister::SERIAL_PATH = InitPath("serial_blacklist.txt");

const String CCertBlacklister::PUBKEY_BLACKLIST_KEY("pubkey_blacklist");
const String CCertBlacklister::SERIAL_BLACKLIST_KEY("serial_blacklist");

CCertBlacklister::BlacklistObserver::WriteBlacklistThread::WriteBlacklistThread(
    /* [in] */ BlacklistObserver* host)
    : mHost(host)
{
}

ECode CCertBlacklister::BlacklistObserver::WriteBlacklistThread::Run()
{
    AutoLock lock(mHost->mTmpDir.Get());

    String blacklist = mHost->GetValue();
    if (blacklist != NULL) {
        Slogger::I("CCertBlacklister::BlacklistObserver", "Certificate blacklist changed, updating...");
        AutoPtr<IFileOutputStream> out;
        AutoPtr<ArrayOf<Byte> > bytes = blacklist.GetBytes();

        ECode ec = NOERROR;
        Boolean bval;
        // create a temporary file
        AutoPtr<IFile> tmp, newFile;
        AutoPtr<IFileHelper> helper;
        CFileHelper::AcquireSingleton((IFileHelper**)&helper);
        helper->CreateTempFile(String("journal"), String(""), mHost->mTmpDir, (IFile**)&tmp);
        // mark it -rw-r--r--
        ec = tmp->SetReadable(TRUE, FALSE, &bval);
        FAIL_GOTO(ec, _EXIT_)

        // write to it
        CFileOutputStream::New(tmp, (IFileOutputStream**)&out);

        ec = IOutputStream::Probe(out)->Write(bytes);
        FAIL_GOTO(ec, _EXIT_)

        // sync to disk
        ec = FileUtils::Sync(out);
        FAIL_GOTO(ec, _EXIT_)

        // atomic rename
        CFile::New(mHost->mPath, (IFile**)&newFile);
        ec = tmp->RenameTo(newFile, &bval);
        FAIL_GOTO(ec, _EXIT_)

        Slogger::I("CCertBlacklister::BlacklistObserver", "Certificate blacklist updated");

_EXIT_:
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::E("CCertBlacklister::BlacklistObserver", "Failed to write blacklist");
        }

        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
        ioUtils->CloseQuietly(ICloseable::Probe(out));
    }

    return NOERROR;
}

CCertBlacklister::BlacklistObserver::BlacklistObserver(
    /* [in] */ const String& key,
    /* [in] */ const String& name,
    /* [in] */ const String& path,
    /* [in] */ IContentResolver* cr)
{
    mKey = key;
    mName = name;
    mPath = path;
    AutoPtr<IFile> file;
    CFile::New(mPath, (IFile**)&file);
    file->GetParentFile((IFile**)&mTmpDir);
    mContentResolver = cr;
}

ECode CCertBlacklister::BlacklistObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    ContentObserver::OnChange(selfChange);
    WriteBlacklist();
    return NOERROR;
}

String CCertBlacklister::BlacklistObserver::GetValue()
{
    String value;
    Settings::Secure::GetString(mContentResolver, mKey, &value);
    return value;
}

void CCertBlacklister::BlacklistObserver::WriteBlacklist()
{
    AutoPtr<WriteBlacklistThread> thread = new WriteBlacklistThread(this);
    thread->constructor(String("BlacklistUpdater"));
    thread->Start();
}

CAR_OBJECT_IMPL(CCertBlacklister)

ECode CCertBlacklister::constructor(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    RegisterObservers(cr);
    return NOERROR;
}

AutoPtr<IContentObserver> CCertBlacklister::BuildPubkeyObserver(
    /* [in] */ IContentResolver* cr)
{
    AutoPtr<BlacklistObserver> bo = new BlacklistObserver(PUBKEY_BLACKLIST_KEY,
        String("pubkey"), PUBKEY_PATH, cr);
    bo->constructor(NULL);
    return (IContentObserver*)bo.Get();
}

AutoPtr<IContentObserver> CCertBlacklister::BuildSerialObserver(
    /* [in] */ IContentResolver* cr)
{
    AutoPtr<BlacklistObserver> bo =  new BlacklistObserver(SERIAL_BLACKLIST_KEY,
        String("serial"), SERIAL_PATH, cr);
    bo->constructor(NULL);
    return (IContentObserver*)bo.Get();
}

void CCertBlacklister::RegisterObservers(
    /* [in] */ IContentResolver* cr)
{
    AutoPtr<IUri> uri;
    Settings::Secure::GetUriFor(PUBKEY_BLACKLIST_KEY, (IUri**)&uri);

    // set up the public key blacklist observer
    cr->RegisterContentObserver(
        uri, TRUE, BuildPubkeyObserver(cr));

    // set up the serial number blacklist observer
    uri = NULL;
    Settings::Secure::GetUriFor(SERIAL_BLACKLIST_KEY, (IUri**)&uri);
    cr->RegisterContentObserver(
        uri, TRUE, BuildSerialObserver(cr));
}

ECode CCertBlacklister::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)
    *info = "CCertBlacklister";
    return NOERROR;
}


} // namespace Server
} // namespace Droid
} // namespace Elastos

