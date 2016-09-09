
#include "elastos/droid/server/usb/UsbDebuggingManager.h"
#include "elastos/droid/server/FgThread.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Net::CLocalSocket;
using Elastos::Droid::Net::ILocalSocket;
using Elastos::Droid::Net::CLocalSocketAddress;
using Elastos::Droid::Net::ILocalSocketAddress;
using Elastos::Droid::Net::LocalSocketAddressNamespace_RESERVED;
using Elastos::Droid::Utility::IBase64;
using Elastos::Droid::Utility::CBase64;
using Elastos::Security::IMessageDigest;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Security::CMessageDigestHelper;
using Elastos::Core::CString;
using Elastos::Core::CThread;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Usb {

const String UsbDebuggingManager::TAG("UsbDebuggingManager");
const Boolean UsbDebuggingManager::DEBUG = FALSE;
const String UsbDebuggingManager::ADBD_SOCKET = String("adbd");
const String UsbDebuggingManager::ADB_DIRECTORY = String("misc/adb");
const String UsbDebuggingManager::ADB_KEYS_FILE = String("adb_keys");
const Int32 UsbDebuggingManager::BUFFER_SIZE = 4096;

const Int32 UsbDebuggingManager::UsbDebuggingHandler::MESSAGE_ADB_ENABLED;// = 1;
const Int32 UsbDebuggingManager::UsbDebuggingHandler::MESSAGE_ADB_DISABLED;// = 2;
const Int32 UsbDebuggingManager::UsbDebuggingHandler::MESSAGE_ADB_ALLOW;// = 3;
const Int32 UsbDebuggingManager::UsbDebuggingHandler::MESSAGE_ADB_DENY;// = 4;
const Int32 UsbDebuggingManager::UsbDebuggingHandler::MESSAGE_ADB_CONFIRM;// = 5;
const Int32 UsbDebuggingManager::UsbDebuggingHandler::MESSAGE_ADB_CLEAR;// = 6;

UsbDebuggingManager::UsbDebuggingHandler::UsbDebuggingHandler(
    /* [in] */ UsbDebuggingManager* host,
    /* [in] */ ILooper* looper)
    : Handler(looper)
    , mHost(host)
{
}

ECode UsbDebuggingManager::UsbDebuggingHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case MESSAGE_ADB_ENABLED:
            HandleAdbEnable();
            break;
        case MESSAGE_ADB_DISABLED:
            HandleAdbDisable();
            break;
        case MESSAGE_ADB_ALLOW: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ICharSequence* seq = ICharSequence::Probe(obj);
            String key;
            seq->ToString(&key);
            Int32 arg1;
            msg->GetArg1(&arg1);
            HandleAdbAllow(arg1 == 1, key);
            break;
        }
        case MESSAGE_ADB_DENY:
            HandleAdbDeny();
            break;
        case MESSAGE_ADB_CONFIRM: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ICharSequence* seq = ICharSequence::Probe(obj);
            String key;
            seq->ToString(&key);
            HandleAdbConfirm(key);
            break;
        }
        case MESSAGE_ADB_CLEAR:
            mHost->DeleteKeyFile();
            break;
    }

    return NOERROR;
}

void UsbDebuggingManager::UsbDebuggingHandler::HandleAdbEnable()
{
    if (mHost->mAdbEnabled) {
        return;
    }

    mHost->mAdbEnabled = TRUE;
    CThread::New(mHost, TAG, (IThread**)&mHost->mThread);
    mHost->mThread->Start();
}

void UsbDebuggingManager::UsbDebuggingHandler::HandleAdbDisable()
{
    if (!mHost->mAdbEnabled) {
        return;
    }

    mHost->mAdbEnabled = FALSE;
    mHost->CloseSocket();

    //try {
    mHost->mThread->Join();
    //} catch (Exception ex) {

    mHost->mThread = NULL;
    mHost->mOutputStream = NULL;
    mHost->mSocket = NULL;
}

void UsbDebuggingManager::UsbDebuggingHandler::HandleAdbAllow(
    /* [in] */ Boolean alwaysAllow,
    /* [in] */ const String& key)
{
    String fingerprints = mHost->GetFingerprints(key);

    if (!fingerprints.Equals(mHost->mFingerprints)) {
        Slogger::E(TAG, "Fingerprints do not match. Got %s, expected ", (const char*) mHost->mFingerprints);
        return;
    }

    if (alwaysAllow) {
        mHost->WriteKey(key);
    }

    mHost->SendResponse(String("OK"));
}

void UsbDebuggingManager::UsbDebuggingHandler::HandleAdbDeny()
{
    mHost->SendResponse(String("NO"));
}

void UsbDebuggingManager::UsbDebuggingHandler::HandleAdbConfirm(
    /* [in] */ const String& key)
{
    mHost->mFingerprints = mHost->GetFingerprints(key);
    mHost->StartConfirmation(key, mHost->mFingerprints);
}

UsbDebuggingManager::UsbDebuggingManager(
    /* [in] */ IContext* context)
    : mContext(context)
    , mAdbEnabled(FALSE)
{
    AutoPtr<ILooper> looper;
    FgThread::Get()->GetLooper((ILooper**)&looper);
    mHandler = new UsbDebuggingHandler(this, looper);
}

ECode UsbDebuggingManager::ListenToSocket()
{
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(BUFFER_SIZE);
    AutoPtr<ILocalSocketAddress> address;
    CLocalSocketAddress::New(ADBD_SOCKET, LocalSocketAddressNamespace_RESERVED,
        (ILocalSocketAddress**)&address);

    CLocalSocket::New((ILocalSocket**)&mSocket);
    mSocket->Connect(address);

    mSocket->GetOutputStream((IOutputStream**)&mOutputStream);
    AutoPtr<IInputStream> inputStream;
    mSocket->GetInputStream((IInputStream**)&inputStream);

    while (TRUE) {
        Int32 count;
        if (FAILED(inputStream->Read(buffer, &count)))
            break;

        if (count < 0) {
            break;
        }

        if ((*buffer)[0] == 'p' && (*buffer)[1] == 'k') {
            String key(*buffer, 2, count - 2);
            Slogger::D(TAG, "Received public key: %s", (const char*) key);

            AutoPtr<ICharSequence> seq;
            CString::New(key, (ICharSequence**)&seq);
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(UsbDebuggingHandler::MESSAGE_ADB_CONFIRM, (IMessage**)&msg);
            msg->SetObj(seq);
            Boolean result;
            mHandler->SendMessage(msg, &result);
        }
        else {
            String key(*buffer, 0, 2);
            Slogger::E(TAG, "Wrong message: %s", (const char*) key);
            break;
        }
    }

    CloseSocket();
    return NOERROR;
}

CARAPI UsbDebuggingManager::Run()
{
    while (mAdbEnabled) {
        if (FAILED(ListenToSocket())) {
            SystemClock::Sleep(1000);
        }
    }

    return NOERROR;
}

void UsbDebuggingManager::CloseSocket()
{
    if (mOutputStream != NULL) {
        if (FAILED(ICloseable::Probe(mOutputStream)->Close())) {
            Slogger::E(TAG, "Failed closing output stream.");
        }
    }

    if (mSocket != NULL) {
        if (FAILED(ICloseable::Probe(mSocket)->Close())) {
            Slogger::E(TAG, "Failed closing socket.");
        }
    }
}

void UsbDebuggingManager::SendResponse(
    /* [in] */ const String& msg)
{
    if (mOutputStream == NULL) {
        return;
    }

    if (FAILED(mOutputStream->Write(msg.GetBytes())))
        Slogger::E(TAG, "Failed to write response.");
}

String UsbDebuggingManager::GetFingerprints(
    /* [in] */ const String& key)
{
    String hex = String("0123456789ABCDEF");
    StringBuilder sb;
    AutoPtr<IMessageDigestHelper> digesterHelper;
    CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&digesterHelper);

    AutoPtr<IMessageDigest> digester;
    ECode ec = digesterHelper->GetInstance(String("MD5"), (IMessageDigest**)&digester);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Error getting digester.");
        return String("");
    }

    AutoPtr<ArrayOf<String> > base64_strData;
    StringUtils::Split(key, String("\\s+"), (ArrayOf<String>**)&base64_strData);
    AutoPtr< ArrayOf<Byte> > base64_data = (*base64_strData)[0].GetBytes();

    AutoPtr<IBase64> base64;
    CBase64::AcquireSingleton((IBase64**)&base64);
    AutoPtr<ArrayOf<Byte> > decodedResult;
    base64->Decode(base64_data, IBase64::DEFAULT, (ArrayOf<Byte>**)&decodedResult);
    AutoPtr<ArrayOf<Byte> > digest;
    digester->Digest((ArrayOf<Byte>*)decodedResult, (ArrayOf<Byte>**)&digest);

    for (Int32 i = 0, size = digest->GetLength(); i < size; ++i) {
        sb += hex.GetChar((((*digest)[i] >> 4) & 0xf));
        sb += hex.GetChar(((*digest)[i] & 0xf));

        if (i < (digest->GetLength() - 1)) {
            sb += ":";
        }
    }

    return sb.ToString();
}

void UsbDebuggingManager::StartConfirmation(
    /* [in] */ const String& key,
    /* [in] */ const String& fingerprints)
{
    AutoPtr<IResourcesHelper> rHelper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&rHelper);
    AutoPtr<IResources> sr;
    rHelper->GetSystem((IResources**)&sr);
    String nameString;
    sr->GetString(R::string::config_customAdbPublicKeyConfirmationComponent, &nameString);
    AutoPtr<IComponentNameHelper> cnHelper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&cnHelper);
    AutoPtr<IComponentName> componentName;
    cnHelper->UnflattenFromString(nameString, (IComponentName**)&componentName);
    if (StartConfirmationActivity(componentName, key, fingerprints)
        || StartConfirmationService(componentName, key, fingerprints)) {
        return;
    }
    Slogger::E(TAG, "unable to start customAdbPublicKeyConfirmationComponent "
        "%s as an Activity or a Service", nameString.string());
}

/**
 * @returns true if the componentName led to an Activity that was started.
 */
Boolean UsbDebuggingManager::StartConfirmationActivity(
    /* [in] */ IComponentName* componentName,
    /* [in] */ const String& key,
    /* [in] */ const String& fingerprints)
{
    AutoPtr<IPackageManager> packageManager;
    mContext->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IIntent> intent = CreateConfirmationIntent(componentName, key, fingerprints);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    AutoPtr<IResolveInfo> ri;
    packageManager->ResolveActivity(intent, IPackageManager::MATCH_DEFAULT_ONLY, (IResolveInfo**)&ri);
    if (ri != NULL) {
        if (SUCCEEDED(mContext->StartActivity(intent))) {
            return TRUE;
        }

        Slogger::E(TAG, "unable to start adb whitelist activity: %s", TO_CSTR(componentName));
    }
    return FALSE;
}

/**
 * @returns true if the componentName led to a Service that was started.
 */
Boolean UsbDebuggingManager::StartConfirmationService(
    /* [in] */ IComponentName* componentName,
    /* [in] */ const String& key,
    /* [in] */ const String& fingerprints)
{
    AutoPtr<IIntent> intent = CreateConfirmationIntent(componentName, key, fingerprints);
    AutoPtr<IComponentName> cn;
    if (SUCCEEDED(mContext->StartService(intent, (IComponentName**)&cn)) && cn != NULL) {
        return TRUE;
    }

    Slogger::E(TAG, "unable to start adb whitelist service: %s", TO_CSTR(componentName));
    return FALSE;
}

AutoPtr<IIntent> UsbDebuggingManager::CreateConfirmationIntent(
    /* [in] */ IComponentName* componentName,
    /* [in] */ const String& key,
    /* [in] */ const String& fingerprints)
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    String pkgName, className;
    componentName->GetPackageName(&pkgName);
    componentName->GetClassName(&className);
    intent->SetClassName(pkgName, className);
    intent->PutExtra(String("key"), key);
    intent->PutExtra(String("fingerprints"), fingerprints);
    return intent;
}

AutoPtr<IFile> UsbDebuggingManager::GetUserKeyFile()
{
    AutoPtr<IFile> dataDir = Environment::GetDataDirectory();
    AutoPtr<IFile> adbDir;
    CFile::New(dataDir, ADB_DIRECTORY, (IFile**)&adbDir);

    Boolean isAdbDirExist;
    adbDir->Exists(&isAdbDirExist);
    if (isAdbDirExist == FALSE) {
        Slogger::E(TAG, "ADB data directory does not exist");
        return NULL;
    }

    AutoPtr<IFile> keyFile;
    CFile::New(adbDir, ADB_KEYS_FILE, (IFile**)&keyFile);
    return keyFile;
}

void UsbDebuggingManager::WriteKey(
    /* [in] */ const String& key)
{
    AutoPtr<IFile> keyFile = GetUserKeyFile();
    if (keyFile == NULL) {
        return;
    }

    ECode ec;
    do {
        Boolean isKeyFileExist;
        keyFile->Exists(&isKeyFileExist);
        if (!isKeyFileExist) {
            Boolean createdResult;
            ec = keyFile->CreateNewFile(&createdResult);
            if (FAILED(ec))
                break;

            String path;
            ec = keyFile->ToString(&path);
            FileUtils::SetPermissions(path, FileUtils::sS_IRUSR | FileUtils::sS_IWUSR | FileUtils::sS_IRGRP, -1, -1);
        }

        AutoPtr<IOutputStream> fo;
        ec = CFileOutputStream::New(keyFile, TRUE, (IOutputStream**)&fo);
        if (FAILED(ec))
            break;

        ec = fo->Write(key.GetBytes());
        if (FAILED(ec))
            break;

        ec = fo->Write('\n');
        if (FAILED(ec))
            break;

        ec = fo->Close();
    } while (0);

    if (FAILED(ec)) {
        Slogger::E(TAG, "Error writing key: ec = 0x%08x", ec);
    }
}

void UsbDebuggingManager::DeleteKeyFile()
{
    AutoPtr<IFile> keyFile = GetUserKeyFile();
    if (keyFile != NULL) {
        Boolean res;
        keyFile->Delete(&res);
    }
}

void UsbDebuggingManager::SetAdbEnabled(
    /* [in] */ Boolean enabled)
{
    Boolean result;
    mHandler->SendEmptyMessage(
        enabled ? UsbDebuggingHandler::MESSAGE_ADB_ENABLED
            : UsbDebuggingHandler::MESSAGE_ADB_DISABLED,
        &result);
}

void UsbDebuggingManager::AllowUsbDebugging(
    /* [in] */ Boolean alwaysAllow,
    /* [in] */ const String& publicKey)
{
    AutoPtr<ICharSequence> seq;
    CString::New(publicKey, (ICharSequence**)&seq);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(UsbDebuggingHandler::MESSAGE_ADB_ALLOW, (IMessage**)&msg);
    msg->SetArg1(alwaysAllow ? 1 : 0);
    msg->SetObj(seq);
    Boolean result;
    mHandler->SendMessage(msg, &result);
}

void UsbDebuggingManager::DenyUsbDebugging()
{
    Boolean result;
    mHandler->SendEmptyMessage(UsbDebuggingHandler::MESSAGE_ADB_DENY, &result);
}

void UsbDebuggingManager::ClearUsbDebuggingKeys()
{
    Boolean result;
    mHandler->SendEmptyMessage(UsbDebuggingHandler::MESSAGE_ADB_CLEAR, &result);
}

} // namespace Usb
} // namespace Server
} // namepsace Droid
} // namespace Elastos
