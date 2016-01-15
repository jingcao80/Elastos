
#include "elastos/droid/wifi/WifiApConfigStore.h"
#include "elastos/droid/wifi/WifiStateMachine.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>
#ifdef DROID_CORE
#include "elastos/droid/wifi/CWifiConfiguration.h"
#include "elastos/droid/os/CMessenger.h"
#include "elastos/droid/os/CEnvironment.h"
#endif
#include "elastos/droid/utility/AsyncChannel.h"

using Elastos::Core::CThread;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::CDataInputStreamHelper;
using Elastos::IO::IDataInputStreamHelper;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataOutput;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::IUUID;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;
using Elastos::Droid::R;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Internal::Utility::AsyncChannel;

namespace Elastos {
namespace Droid {
namespace Wifi {

static String InitAP_CONFIG_FILE()
{
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dataDir;
    env->GetDataDirectory((IFile**)&dataDir);
    String dataPath;
    dataDir->ToString(&dataPath);
    String path = dataPath + "/misc/wifi/softap.conf";
    return path;
}

String WifiApConfigStore::TAG("WifiApConfigStore");
String WifiApConfigStore::AP_CONFIG_FILE = InitAP_CONFIG_FILE();
const Int32 WifiApConfigStore::AP_CONFIG_FILE_VERSION = 1;

//=======================================================================
// WifiApConfigStore::DefaultState
//=======================================================================
ECode WifiApConfigStore::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = HANDLED;
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case WifiStateMachine::CMD_SET_AP_CONFIG:
        case WifiStateMachine::CMD_SET_AP_CONFIG_COMPLETED:
            Logger::E(WifiApConfigStore::TAG, "Unexpected message: %d", what);
            break;
        case WifiStateMachine::CMD_REQUEST_AP_CONFIG: {
            AutoPtr<IInterface> obj;
            if (mOwner->mWifiApConfig != NULL) {
                obj = mOwner->mWifiApConfig->Probe(EIID_IInterface);
            }
            mOwner->mReplyChannel->ReplyToMessage(msg,
                WifiStateMachine::CMD_RESPONSE_AP_CONFIG, obj);
            break;
        }
        default:
            Logger::E(WifiApConfigStore::TAG, "Failed to handle %d", what);
            break;
    }

    return NOERROR;
}

//=======================================================================
// WifiApConfigStore::InactiveState
//=======================================================================
ECode WifiApConfigStore::InactiveState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NOT_HANDLED;
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
        case WifiStateMachine::CMD_SET_AP_CONFIG: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mOwner->mWifiApConfig = IWifiConfiguration::Probe(obj);
            mOwner->TransitionTo(mOwner->mActiveState);
            *result = HANDLED;
            break;
        }
        default:
            break;
    }

    return NOERROR;
}

ECode WifiApConfigStore::ActiveStateRunnable::Run()
{
    mOwner->WriteApConfiguration(mOwner->mWifiApConfig);
    mOwner->SendMessage(WifiStateMachine::CMD_SET_AP_CONFIG_COMPLETED);
    return NOERROR;
}

//=======================================================================
// WifiApConfigStore::ActiveState
//=======================================================================
ECode WifiApConfigStore::ActiveState::Enter()
{
    AutoPtr<IRunnable> runnable = new ActiveStateRunnable(mOwner);
    AutoPtr<IThread> thread;
    CThread::New(runnable, (IThread**)&thread);
    thread->Start();
    return NOERROR;
}

ECode WifiApConfigStore::ActiveState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = HANDLED;
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
        //TODO: have feedback to the user when we do this
        //to indicate the write is currently in progress
        case WifiStateMachine::CMD_SET_AP_CONFIG:
            mOwner->DeferMessage(msg);
            break;
        case WifiStateMachine::CMD_SET_AP_CONFIG_COMPLETED:
            mOwner->TransitionTo(mOwner->mInactiveState);
            break;
        default:
            *result = NOT_HANDLED;
            break;
    }

    return NOERROR;
}

//=======================================================================
// WifiApConfigStore
//=======================================================================
WifiApConfigStore::WifiApConfigStore(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* target)
{
    AutoPtr<ILooper> looper;
    target->GetLooper((ILooper**)&looper);
    StateMachine::InitStateMachine(TAG, looper);

    mContext = context;
    mDefaultState = new DefaultState(this);
    mInactiveState = new InactiveState(this);
    mActiveState = new ActiveState(this);

    mReplyChannel = new AsyncChannel();

    AddState(mDefaultState);
    AddState(mInactiveState, mDefaultState);
    AddState(mActiveState, mDefaultState);

    SetInitialState(mInactiveState);
}

AutoPtr<WifiApConfigStore> WifiApConfigStore::MakeWifiApConfigStore(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* target)
{
    AutoPtr<WifiApConfigStore> s = new WifiApConfigStore(context, target);
    s->Start();
    return s;
}

void WifiApConfigStore::LoadApConfiguration()
{
    AutoPtr<IDataInputStream> in;
    // try {
    AutoPtr<IWifiConfiguration> config;
    CWifiConfiguration::New((IWifiConfiguration**)&config);

    AutoPtr<IFileInputStream> fis;
    CFileInputStream::New(AP_CONFIG_FILE, (IFileInputStream**)&fis);
    if (fis == NULL) {
        SetDefaultApConfiguration();
        return;
    }

    AutoPtr<IBufferedInputStream> bis;
    CBufferedInputStream::New(fis, (IBufferedInputStream**)&bis);
    if (bis == NULL) {
        SetDefaultApConfiguration();
        return;
    }

    CDataInputStream::New(bis, (IDataInputStream**)&in);
    if (in == NULL) {
        SetDefaultApConfiguration();
        return;
    }

    Int32 version = 0;
    in->Read(&version);
    if (version != 1) {
        in->Close();

        Logger::E(TAG, "Bad version on hotspot configuration file, set defaults");
        SetDefaultApConfiguration();
        return;
    }

    AutoPtr<IDataInputStreamHelper> helper;
    assert(IDataInput::Probe(in) != NULL);
    CDataInputStreamHelper::AcquireSingleton((IDataInputStreamHelper**)&helper);
    String ssid;
    helper->ReadUTF(IDataInput::Probe(in), &ssid);
    config->SetSSID(ssid);

    Int32 authType = 0;
    in->Read(&authType);
    AutoPtr<IBitSet> bs;
    config->GetAllowedKeyManagement((IBitSet**)&bs);
    bs->Set(authType);

    if (authType != IWifiConfigurationKeyMgmt::NONE) {
        String psk;
        helper->ReadUTF(IDataInput::Probe(in), &psk);
        config->SetPreSharedKey(psk);
    }

    mWifiApConfig = config;
    if (in != NULL) {
        in->Close();
    }
}

ECode WifiApConfigStore::GetMessenger(
    /* [out] */ IMessenger** msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IHandler> handler = GetHandler();
    return CMessenger::New(handler, msg);
}

void WifiApConfigStore::WriteApConfiguration(
    /* [in] */ IWifiConfiguration* config)
{
    assert(config != NULL);
    AutoPtr<IFileOutputStream> fos;
    CFileOutputStream::New(AP_CONFIG_FILE, (IFileOutputStream**)&fos);
    AutoPtr<IBufferedOutputStream> bos;
    CBufferedOutputStream::New(fos, (IBufferedOutputStream**)&bos);
    if (bos == NULL) {
        Logger::E(TAG, "Error writing hotspot configuration");
        return;
    }

    AutoPtr<IDataOutputStream> out;
    CDataOutputStream::New(bos, (IDataOutputStream**)&out);
    if (out == NULL) {
        Logger::E(TAG, "Error writing hotspot configuration");
        return;
    }

    IDataOutput* output = IDataOutput::Probe(out);
    assert(output != NULL);

    output->WriteInt32(AP_CONFIG_FILE_VERSION);
    String temp;
    config->GetSSID(&temp);
    output->WriteUTF(temp);
    Int32 authType = 0;
    config->GetAuthType(&authType);
    output->WriteInt32(authType);
    if(authType != IWifiConfigurationKeyMgmt::NONE) {
        config->GetPreSharedKey(&temp);
        output->WriteUTF(temp);
    }

    out->Close();
}

void WifiApConfigStore::SetDefaultApConfiguration()
{
    AutoPtr<IWifiConfiguration> config;
    CWifiConfiguration::New((IWifiConfiguration**)&config);

    String temp;
    mContext->GetString(R::string::wifi_tether_configure_ssid_default, &temp);
    config->SetSSID(temp);

    AutoPtr<IBitSet> bs;
    config->GetAllowedKeyManagement((IBitSet**)&bs);
    bs->Set(IWifiConfigurationKeyMgmt::WPA2_PSK);

    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    AutoPtr<IUUID> uuid;
    helper->RandomUUID((IUUID**)&uuid);

    String randomUUID("3290c984-e86b-4e1a-y7fb-a51b99b815e0");
    if (uuid) uuid->ToString(&randomUUID);

    //first 12 chars from xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
    String psk(randomUUID.Substring(0, 8));
    psk.AppendFormat("%s", randomUUID.Substring(9, 13).string());
    config->SetPreSharedKey(psk);
    SendMessage(WifiStateMachine::CMD_SET_AP_CONFIG, config.Get());
}


} // namespace Wifi
} // namespace Droid
} // namespace Elastos
