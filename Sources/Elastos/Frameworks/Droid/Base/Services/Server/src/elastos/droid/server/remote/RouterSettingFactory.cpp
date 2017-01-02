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

#include "remote/RouterSettingFactory.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::IInputStream;
using Elastos::Net::CServerSocket;
using Elastos::Net::ISocket;
using Elastos::Net::IURLDecoder;
using Elastos::Net::CURLDecoder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

RouterSettingFactory::SendIntentThread::SendIntentThread(
    /* [in] */ RouterSettingFactory* host)
    : mHost(host)
{
    ThreadBase::Init();
}

ECode RouterSettingFactory::SendIntentThread::Run()
{
    ThreadBase::Run();
    if (DBG) Slogger::D(TAG, "SendIntentThread");
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    AutoPtr<IComponentName> comp;
    CComponentName::New(String("com.kortide.routersettings"), String("com.kortide.routersettings.RemoteControlService"),
        (IComponentName**)&comp);
    intent->SetComponent(comp);

    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    if (DBG) Slogger::D(TAG, "cmd =  %s", mHost->mRemoteCmd.string());
    bundle->PutString(String("cmd"), mHost->mRemoteCmd);
    bundle->PutInt64(String("port"), mHost->mPort);
    intent->PutExtras(bundle);

    AutoPtr<IComponentName> name;
    mHost->mContext->StartService(intent, (IComponentName**)&name);

    return NOERROR;
}

const String RouterSettingFactory::TAG("RouterSettingFactory");
const Boolean RouterSettingFactory::DBG = TRUE;

RouterSettingFactory::RouterSettingFactory(
    /* [in] */ IContext* ctx)
    : AbstractDeviceFactory(ctx)
    , mCmdCheckSN(0)
{
    if (DBG) Slogger::D(TAG, "Router constructor");
    // try {
        CServerSocket::New(0, (IServerSocket**)&mServersocket);
        mServersocket->SetSoTimeout(10000);
        mServersocket->GetLocalPort(&mPort);
        if (DBG) Slogger::D(TAG, " mServersocket.getLocalPort() = %d", mPort);
    // } catch (java.io.IOException e) {
    //     if (DBG) Slogger::D(TAG, "Socket ERROR !IOException :" + e.toString());
    // }
}

Boolean RouterSettingFactory::IsCorrectDevice()
{
    // try {
    //     if (DBG) Slogger::D(TAG, "IsCorrectDevice()");
    //     String command("cat system/build.prop"); // for print content of build.prop
    //     AutoPtr<IProcess> proc;
    //     proc = Runtime.getRuntime().exec(command);
    //     BufferedReader br = new BufferedReader(new InputStreamReader(proc.getInputStream()));

    //     // read the content of build.prop
    //     char charBuffer[] = new char[9192]; // 9192 is an arbitrary enough magic number
    //     Int32 buffLen = br.read(charBuffer);
    //     String content = new String(charBuffer, 0, buffLen);
    //     if (DBG) Slogger::D(TAG, "[IsCorrectDevice] content = " + content);
    //     if (content.contains("R1S")) return true;
    //     else if (content.contains("R1")) return true;
    //     else if(content.contains("M1")) return false;
    //     else return false;

    // } catch (IOException e) {
    //     if (DBG) Slogger::D(TAG, "[IsCorrectDevice] IOException");
    //     e.printStackTrace();
    // }
    // return false;
    AutoPtr<ISystemProperties> systemProperties;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&systemProperties);
    String prop;
    systemProperties->Get(String("ro.product.name"), &prop);
    if (DBG) Slogger::D(TAG, "[IsCorrectDevice] ro.product.name = %s", prop.string());
    if (prop.Contains("R1S"))
        return TRUE;
    else if (prop.Contains("R1"))
        return TRUE;
    else if(prop.Contains("M1"))
        return FALSE;

    return FALSE;
}

String RouterSettingFactory::SocketCmd_Task()
{
    if (DBG) Slogger::D(TAG, "RouterSettingFactory SocketCmd_Task");
    String result("fail");

    if(IsCorrectDevice()) {
        if (DBG) Slogger::D(TAG, "[SocketCmd_Task]IsCorrectDevice = TRUE");
        AutoPtr<ISocket> socket;
        ECode ec;
        do {
            AutoPtr<SendIntentThread> sendIntent = new SendIntentThread(this);
            sendIntent->Start();

            if (DBG) Slogger::D(TAG, "ceate socket");
            ec = mServersocket->Accept((ISocket**)&socket);
            if (FAILED(ec))
                break;
            if (DBG) Slogger::D(TAG, "accept");

            AutoPtr<IInputStream> inputStream;
            ec = socket->GetInputStream((IInputStream**)&inputStream);
            if (FAILED(ec))
                break;

            AutoPtr<IBufferedInputStream> in;
            ec = CBufferedInputStream::New(inputStream, (IBufferedInputStream**)&in);
            if (FAILED(ec))
                break;

            AutoPtr<ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(2048);
            String rspData("");
            Int32 length = 0;
            while ((in->ReadBytes(buf, &length), length) > 0) {
                if (DBG) Slogger::D(TAG, "in while");
                rspData += String((char*)buf->GetPayload(), length);
            }

            AutoPtr<IURLDecoder> urlDecoder;
            CURLDecoder::AcquireSingleton((IURLDecoder**)&urlDecoder);
            String temp;
            if (FAILED(urlDecoder->Decode(rspData, String("UTF-8"), &temp)))
                Slogger::E(TAG, "UnsupportedEncodingException!!");
            rspData = temp;

            if (DBG) Slogger::D(TAG, "rspData = %s", rspData.string());
            result = rspData;
        } while (0);

        mServersocket->Close();
        if (socket != NULL)
            socket->Close();

        if (E_SOCKET_TIMEOUT_EXCEPTION == ec){
            if (DBG) Slogger::D(TAG, "Socket ERROR ! SocketTimeoutException : 0x%08x", ec);
        }
        else if (E_IO_EXCEPTION == ec) {
            if (DBG) Slogger::D(TAG, "Socket ERROR !IOException : 0x%08x", ec);
        }
    }
    else{
        if (DBG) Slogger::D(TAG, "[SocketCmd_Task]IsCorrectDevice = FALSE");
        result = "Error_Device";
    }
    return result;
}

ECode RouterSettingFactory::RunForResult(
    /* [in] */ const String& cmd,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Slogger::D(TAG, "RouterSettingFactory RunForResult");
    mRemoteCmd = cmd;
    *result = SocketCmd_Task();
    return NOERROR;
}

} // Remote
} // Server
} // Droid
} // Elastos
