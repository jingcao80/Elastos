
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Handler.h"
#include "usb/UsbDebuggingManager.h"
#include <elastos/utility/logging/Logger.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringUtils.h>

using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::StringBuffer;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::CThread;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Net::CLocalSocket;
using Elastos::Droid::Net::ILocalSocket;
using Elastos::Droid::Net::CLocalSocketAddress;
using Elastos::Droid::Net::ILocalSocketAddress;
using Elastos::Droid::Net::LocalSocketAddressNamespace_RESERVED;
//using Elastos::Droid::Utility::IBase64;
//using Elastos::Droid::Utility::CBase64;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Security::IMessageDigest;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Security::CMessageDigestHelper;

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
    }

    return NOERROR;
}

UsbDebuggingManager::UsbDebuggingHandler::UsbDebuggingHandler(
    /* [in] */ UsbDebuggingManager* host,
    /* [in] */ ILooper* looper)
    : HandlerBase(looper)
    , mHost(host)
{
}

void UsbDebuggingManager::UsbDebuggingHandler::HandleAdbEnable()
{
    if (mHost->mAdbEnabled) {
        return;
    }

    mHost->mAdbEnabled = TRUE;
    CThread::New(mHost, (IThread**)&mHost->mThread);
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
        Slogger::E(UsbDebuggingManager::TAG, "Fingerprints do not match. Got %s, expected ", (const char*) mHost->mFingerprints);
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
    mHost->ShowConfirmationDialog(key, mHost->mFingerprints);
}

UsbDebuggingManager::UsbDebuggingManager(
    /* [in] */ IContext* context)
{
    CHandlerThread::New(String("UsbDebuggingHandler"), (IHandlerThread**)&mHandlerThread);
    mHandlerThread->Start();
    AutoPtr<ILooper> looper;
    mHandlerThread->GetLooper((ILooper**)&looper);
    mUsbDebuggingHandler = new UsbDebuggingHandler(this, looper);
    mContext = context;
    mAdbEnabled = FALSE;
}

UInt32 UsbDebuggingManager::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 UsbDebuggingManager::Release()
{
    return ElRefBase::Release();
}

PInterface UsbDebuggingManager::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IRunnable) {
        return this;
    }

    return NULL;
}

CARAPI UsbDebuggingManager::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [out] */ InterfaceID* pIID)
{
    return E_NOT_IMPLEMENTED;
}

ECode UsbDebuggingManager::ListenToSocket()
{
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(UsbDebuggingManager::BUFFER_SIZE);
    AutoPtr<ILocalSocketAddress> address;

    ECode ec = CLocalSocketAddress::New(UsbDebuggingManager::ADBD_SOCKET,
                LocalSocketAddressNamespace_RESERVED,
                (ILocalSocketAddress**)&address);

    if (FAILED(ec)) {
        CloseSocket();
        return ec;
    }

    AutoPtr<IInputStream> inputStream;

    ec = CLocalSocket::New((ILocalSocket**)&mSocket);
    if (FAILED(ec)) {
        CloseSocket();
        return ec;
    }

    ec = mSocket->Connect(address);
    if (FAILED(ec)) {
        CloseSocket();
        return ec;
    }

    ec = mSocket->GetOutputStream((IOutputStream**)&mOutputStream);
    if (FAILED(ec)) {
        CloseSocket();
        return ec;
    }

    ec = mSocket->GetInputStream((IInputStream**)&inputStream);
    if (FAILED(ec)) {
        CloseSocket();
        return ec;
    }

    while (TRUE) {
        Int32 count;
        inputStream->ReadBytes(buffer, &count);

        if (count < 0) {
            StringBuffer buf;
            buf += "got ";
            buf += count;
            buf += " reading";
            Logger::E(UsbDebuggingManager::TAG, buf.ToString());
            break;
        }

        Byte p = 'P';
        Byte k = 'K';

        if ((*buffer)[0] == p && (*buffer)[1] == k) {
            String key((const char*)(buffer->GetPayload() + 2), count - 2);
            Slogger::D(TAG, "Received public key: %s", (const char*) key);

            AutoPtr<ICharSequence> seq;
            CString::New(key, (ICharSequence**)&seq);
            AutoPtr<IMessage> msg;
            mUsbDebuggingHandler->ObtainMessage(UsbDebuggingHandler::MESSAGE_ADB_CONFIRM, (IMessage**)&msg);
            msg->SetObj(seq);
            Boolean result;
            mUsbDebuggingHandler->SendMessage(msg, &result);
        }
        else {
            String key((const char*)(buffer->GetPayload() + 2), count-2);
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
        ECode ec = ListenToSocket();

        if (!FAILED(ec)) {
            continue;
        }

        SystemClock::Sleep(1000);
    }

    return NOERROR;
}

void UsbDebuggingManager::CloseSocket()
{
    if (mOutputStream != NULL) {
        ECode ec = mOutputStream->Close();

        if (FAILED(ec)) {
            Logger::E(UsbDebuggingManager::TAG, "Failed closing output stream.");
        }
    }

    if (mSocket != NULL) {
        ECode ec = mSocket->Close();

        if (FAILED(ec)) {
            Logger::E(UsbDebuggingManager::TAG, "Failed closing socket.");
        }
    }
}

void UsbDebuggingManager::SendResponse(
    /* [in] */ const String& msg)
{
    if (mOutputStream == NULL) {
        return;
    }

    ECode ec = mOutputStream->WriteBytes(ArrayOf<Byte>((Byte*)msg.string(), msg.GetByteLength()));

    if (!FAILED(ec)) {
        return;
    }

    Logger::E(UsbDebuggingManager::TAG, "Failed to write response.");
    return;
}

String UsbDebuggingManager::GetFingerprints(
    /* [in] */ const String& key)
{
    String hex = String("0123456789ABCDEF");

    AutoPtr<IMessageDigestHelper> digesterHelper;

    CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&digesterHelper);

    AutoPtr<IMessageDigest> digester;
    ECode ec = digesterHelper->GetInstance(String("MD5"), (IMessageDigest**)&digester);

    if (FAILED(ec)) {
        Logger::E(UsbDebuggingManager::TAG, "Error getting digester.");
        return String("");
    }

    AutoPtr<ArrayOf<String> > base64_strData;
    StringUtils::Split(key, String("\\s+"), (ArrayOf<String>**)&base64_strData);
    String strData((*base64_strData)[0]);

    AutoPtr< ArrayOf<Byte> > base64_data = ArrayOf<Byte>::Alloc(strData.GetByteLength());
    base64_data->Copy((Byte*)strData.string(), strData.GetByteLength());
//[Vince]+
    assert(0 && "TODO");
//    AutoPtr<IBase64> base64;
//    CBase64::AcquireSingleton((IBase64**)&base64);
    AutoPtr<ArrayOf<Byte> > decodedResult;
//    base64->Decode(base64_data, IBase64::DEFAULT, (ArrayOf<Byte>**)&decodedResult);
    AutoPtr<ArrayOf<Byte> > digest;
//    digester->Digest((ArrayOf<Byte>*)decodedResult, (ArrayOf<Byte>**)&digest);
//[Vince]-
    StringBuffer buf;

    for (Int32 i = 0, size = digest->GetLength(); i < size; ++i) {
        buf += hex.GetChar((((*digest)[i] >> 4) & 0xf));
        buf += hex.GetChar(((*digest)[i] & 0xf));

        if (i < (digest->GetLength() - 1)) {
            buf += ":";
        }
    }

    return buf.ToString();
}

void UsbDebuggingManager::ShowConfirmationDialog(
    /* [in] */ const String &key,
    /* [in] */ const String &fingerprints)
{
    AutoPtr<IIntent> dialogIntent;
    CIntent::New((IIntent**)&dialogIntent);

    dialogIntent->SetClassName(String("com.android.systemui"),
        String("com.android.systemui.usb.UsbDebuggingActivity"));
    dialogIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    dialogIntent->PutExtra(String("key"), key);
    dialogIntent->PutExtra(String("fingerprints"), fingerprints);

    ECode ec = mContext->StartActivity(dialogIntent);

    if (FAILED(ec)) {
        Logger::E(UsbDebuggingManager::TAG, "unable to start UsbDebuggingActivity");
    }
}

void UsbDebuggingManager::WriteKey(
    /* [in] */ const String& key)
{
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dataDir;
    env->GetDataDirectory((IFile**)&dataDir);
    AutoPtr<IFile> adbDir;
    CFile::New(dataDir, UsbDebuggingManager::ADB_DIRECTORY, (IFile**)&adbDir);

    Boolean isAdbDirExist;
    adbDir->Exists(&isAdbDirExist);

    if (isAdbDirExist == FALSE) {
        Logger::E(UsbDebuggingManager::TAG, "ADB data directory does not exist");
        return;
    }

    AutoPtr<IFile> keyFile;
    CFile::New(adbDir, UsbDebuggingManager::ADB_KEYS_FILE, (IFile**)&keyFile);

    Boolean isKeyFileExist;
    keyFile->Exists(&isKeyFileExist);

    ECode ec;

    if (isKeyFileExist == FALSE) {
        Boolean createdResult;
        ec = keyFile->CreateNewFile(&createdResult);
        if (FAILED(ec)) {
            Logger::E(UsbDebuggingManager::TAG, "Error creating FileOutputStream");
            return;
        }

        String path;
        ec = keyFile->ToString(&path);
        if (FAILED(ec)) {
            Logger::E(UsbDebuggingManager::TAG, "Error creating FileOutputStream");
            return;
        }

        FileUtils::SetPermissions(path, FileUtils::sS_IRUSR | FileUtils::sS_IWUSR | FileUtils::sS_IRGRP, -1, -1);
    }

    AutoPtr<IFileOutputStream> fo;
    ec = CFileOutputStream::New(keyFile, TRUE, (IFileOutputStream**)&fo);
    if (FAILED(ec)) {
        Logger::E(UsbDebuggingManager::TAG, "Error creating FileOutputStream");
        return;
    }

    ec = fo->WriteBytes(ArrayOf<Byte>((Byte*)key.string(), key.GetByteLength()));
    if (FAILED(ec)) {
        Logger::E(UsbDebuggingManager::TAG, "Error writing key");
        return;
    }

    ec = fo->Write('\n');
    if (FAILED(ec)) {
        Logger::E(UsbDebuggingManager::TAG, "Error writing \\n for key");
        return;
    }

    ec = fo->Close();
    if (FAILED(ec)) {
        Logger::E(UsbDebuggingManager::TAG, "Error closing FileOutputStream after writing key");
        return;
    }
}

void UsbDebuggingManager::SetAdbEnabled(
    /* [in] */ Boolean enabled)
{
    if (mHandler == NULL) {
        return;
    }

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
    if (mHandler == NULL) {
        return;
    }

    AutoPtr<ICharSequence> seq;
    CString::New(publicKey, (ICharSequence**)&seq);
    AutoPtr<IMessage> msg;
    mUsbDebuggingHandler->ObtainMessage(UsbDebuggingHandler::MESSAGE_ADB_ALLOW, (IMessage**)&msg);
    msg->SetArg1(alwaysAllow ? 1 : 0);
    msg->SetObj(seq);
    Boolean result;
    mUsbDebuggingHandler->SendMessage(msg, &result);
}

void UsbDebuggingManager::DenyUsbDebugging()
{
    if (mHandler == NULL) {
        return;
    }

    Boolean result;
    mHandler->SendEmptyMessage(UsbDebuggingHandler::MESSAGE_ADB_DENY, &result);
}

} // namespace Usb
} // namespace Server
} // namepsace Droid
} // namespace Elastos
