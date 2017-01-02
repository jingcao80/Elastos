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

#include <elastos/droid/server/EntropyMixer.h>
#include <elastos/droid/server/RandomBlock.h>
#include <elastos/droid/os/Environment.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::IO::IOutputStream;
using Elastos::IO::CFile;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CPrintWriter;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

static Int64 InitSTART_TIME()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 value;
    system->GetCurrentTimeMillis(&value);
    return value;
}

static Int64 InitSTART_NANOTIME()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 value;
    system->GetNanoTime(&value);
    return value;
}

const String EntropyMixer::TAG("EntropyMixer");
const Int32 EntropyMixer::ENTROPY_WHAT = 1;
const Int32 EntropyMixer::ENTROPY_WRITE_PERIOD = 3 * 60 * 60 * 1000;  // 3 hrs
const Int64 EntropyMixer::START_TIME = InitSTART_TIME();
const Int64 EntropyMixer::START_NANOTIME = InitSTART_NANOTIME();

//============================================================
// EntropyMixer::MyHandler
//============================================================
EntropyMixer::MyHandler::MyHandler(
    /* [in] */ EntropyMixer* host)
    : mHost(host)
{}

//@Override
ECode EntropyMixer::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    if (what != EntropyMixer::ENTROPY_WHAT) {
        Slogger::E("EntropyMixer", "Will not process invalid message");
        return NOERROR;
    }

    mHost->AddHwRandomEntropy();
    mHost->WriteEntropy();
    mHost->ScheduleEntropyWriter();
    return NOERROR;
}

//============================================================
// EntropyMixer::MyBroadcastReceiver
//============================================================
EntropyMixer::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ EntropyMixer* host)
    : mHost(host)
{}

//@Override
ECode EntropyMixer::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->WriteEntropy();
    return NOERROR;
}

//============================================================
// EntropyMixer
//============================================================
CAR_INTERFACE_IMPL_2(EntropyMixer, Object, IEntropyMixer, IBinder)

ECode EntropyMixer::constructor(
    /* [in] */ IContext* context)
{
    StringBuilder sb(GetSystemDir());
    sb += "/entropy.dat";
    return Init(context, sb.ToString(),
        String("/dev/urandom"), String("/dev/hw_random"));
}

ECode EntropyMixer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& entropyFile,
    /* [in] */ const String& randomDevice,
    /* [in] */ const String& hwRandomDevice)
{
    return Init(context, entropyFile, randomDevice, hwRandomDevice);
}

ECode EntropyMixer::Init(
    /* [in] */ IContext* context,
    /* [in] */ const String& entropyFile,
    /* [in] */ const String& randomDevice,
    /* [in] */ const String& hwRandomDevice)
{
    if (entropyFile == NULL) {
        Slogger::E(TAG, "NullPointerException: entropyFile");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (randomDevice == NULL) {
        Slogger::E(TAG, "NullPointerException: randomDevice");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (hwRandomDevice == NULL) {
        Slogger::E(TAG, "NullPointerException: hwRandomDevice");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<MyHandler> mh = new MyHandler(this);
    mh->constructor();
    mHandler = (IHandler*)mh.Get();
    mRandomDevice = randomDevice;
    mHwRandomDevice = hwRandomDevice;
    mEntropyFile = entropyFile;
    LoadInitialEntropy();
    AddDeviceSpecificEntropy();
    AddHwRandomEntropy();
    WriteEntropy();
    ScheduleEntropyWriter();
    AutoPtr<IIntentFilter> broadcastFilter;
    CIntentFilter::New(IIntent::ACTION_SHUTDOWN, (IIntentFilter**)&broadcastFilter);
    broadcastFilter->AddAction(IIntent::ACTION_POWER_CONNECTED);
    broadcastFilter->AddAction(IIntent::ACTION_REBOOT);
    AutoPtr<IIntent> stickyIntent;
    return context->RegisterReceiver(mBroadcastReceiver, broadcastFilter,
        (IIntent**)&stickyIntent);
}

void EntropyMixer::ScheduleEntropyWriter()
{
    mHandler->RemoveMessages(ENTROPY_WHAT);
    Boolean bval;
    mHandler->SendEmptyMessageDelayed(ENTROPY_WHAT, ENTROPY_WRITE_PERIOD, &bval);
}

void EntropyMixer::LoadInitialEntropy()
{
    AutoPtr<RandomBlock> block;
    ECode ec = RandomBlock::FromFile(mEntropyFile, (RandomBlock**)&block);
    FAIL_GOTO(ec, _Exit_)

    ec = block->ToFile(mRandomDevice, FALSE);
    FAIL_GOTO(ec, _Exit_)

_Exit_:
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        Slogger::W(TAG, "No existing entropy file -- first boot?");
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::W(TAG, "Failure loading existing entropy file");
    }
}

void EntropyMixer::WriteEntropy()
{
    Slogger::I(TAG, "Writing entropy...");

    AutoPtr<RandomBlock> block;
    ECode ec = RandomBlock::FromFile(mRandomDevice, (RandomBlock**)&block);
    FAIL_GOTO(ec, _Exit_)

    ec = block->ToFile(mEntropyFile, TRUE);
    FAIL_GOTO(ec, _Exit_)

_Exit_:
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::W(TAG, "Unable to write entropy");
    }
}

void EntropyMixer::AddDeviceSpecificEntropy()
{
    AutoPtr<ISystemProperties> systemProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&systemProp);
    String str;

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 value;

    AutoPtr<IPrintWriter> out;
    AutoPtr<IOutputStream> os;
    ECode ec = CFileOutputStream::New(mRandomDevice, (IOutputStream**)&os);
    FAIL_GOTO(ec, _Exit_)

    ec = CPrintWriter::New(os, (IPrintWriter**)&out);
    FAIL_GOTO(ec, _Exit_)

    out->Println(String("Copyright (C) 2009 The Android Open Source Project"));
    out->Println(String("All Your Randomness Are Belong To Us"));
    out->Println(START_TIME);
    out->Println(START_NANOTIME);

    systemProp->Get(String("ro.serialno"), &str);
    out->Println(str);
    systemProp->Get(String("ro.bootmode"), &str);
    out->Println(str);
    systemProp->Get(String("ro.baseband"), &str);
    out->Println(str);
    systemProp->Get(String("ro.carrier"), &str);
    out->Println(str);
    systemProp->Get(String("ro.bootloader"), &str);
    out->Println(str);
    systemProp->Get(String("ro.hardware"), &str);
    out->Println(str);
    systemProp->Get(String("ro.revision"), &str);
    out->Println(str);
    systemProp->Get(String("ro.build.fingerprint"), &str);
    out->Println(str);

    system->GetCurrentTimeMillis(&value);
    out->Println(value);
    system->GetNanoTime(&value);
    out->Println(value);

_Exit_:
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::W(TAG, "Unable to add device specific data to the entropy pool");
    }

    if (out != NULL) {
        ICloseable::Probe(out)->Close();
    }
}

void EntropyMixer::AddHwRandomEntropy()
{
    AutoPtr<RandomBlock> block;
    ECode ec = RandomBlock::FromFile(mHwRandomDevice, (RandomBlock**)&block);
    FAIL_GOTO(ec, _Exit_)

    ec = block->ToFile(mRandomDevice, FALSE);
    FAIL_GOTO(ec, _Exit_)

_Exit_:
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        Slogger::W(TAG, "Added HW RNG output to entropy pool");
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::W(TAG, "Failed to add HW RNG output to entropy pool");
    }
}

String EntropyMixer::GetSystemDir()
{
    AutoPtr<IFile> dataDir = Environment::GetDataDirectory();
    AutoPtr<IFile> systemDir;
    CFile::New(dataDir, String("system"), (IFile**)&systemDir);
    Boolean bval;
    systemDir->Mkdirs(&bval);
    return Object::ToString(systemDir);
}

ECode EntropyMixer::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "EntropyMixer";
    return NOERROR;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos