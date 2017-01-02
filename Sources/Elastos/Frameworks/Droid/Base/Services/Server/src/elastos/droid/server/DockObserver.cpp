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

#include "elastos/droid/server/DockObserver.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.CoreLibrary.IO.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Media::IRingtoneManagerHelper;
// using Elastos::Droid::Media::CRingtoneManagerHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::IO::CFileReader;
using Elastos::IO::IReader;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String DockObserver::TAG("DockObserver");
const String DockObserver::DOCK_UEVENT_MATCH("DEVPATH=/devices/virtual/switch/dock");
const String DockObserver::DOCK_STATE_PATH("/sys/class/switch/dock/state");

const Int32 DockObserver::MSG_DOCK_STATE_CHANGED = 0;

//===================================================================
// DockObserver::BinderService
//===================================================================

CAR_INTERFACE_IMPL(DockObserver::BinderService, Object, IBinder)

//@Override
ECode DockObserver::BinderService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // AutoPtr<IContext> context;
    // mHost->GetContext((IContext**)&context);

    // if (getContext().checkCallingOrSelfPermission(android.Manifest.permission.DUMP)
    //         != PackageManager.PERMISSION_GRANTED) {
    //     pw.println("Permission Denial: can't dump dock observer service from from pid="
    //             + Binder.getCallingPid()
    //             + ", uid=" + Binder.getCallingUid());
    //     return;
    // }

    // long ident = Binder.clearCallingIdentity();
    // try {
    //     {    AutoLock syncLock(mLock);
    //         if (args == NULL || args.length == 0 || "-a".equals(args[0])) {
    //             pw.println("Current Dock Observer Service state:");
    //             if (mUpdatesStopped) {
    //                 pw.println("  (UPDATES STOPPED -- use 'reset' to restart)");
    //             }
    //             pw.println("  reported state: " + mReportedDockState);
    //             pw.println("  previous state: " + mPreviousDockState);
    //             pw.println("  actual state: " + mActualDockState);
    //         } else if (args.length == 3 && "set".equals(args[0])) {
    //             String key = args[1];
    //             String value = args[2];
    //             try {
    //                 if ("state".equals(key)) {
    //                     mUpdatesStopped = true;
    //                     setDockStateLocked(Integer.parseInt(value));
    //                 } else {
    //                     pw.println("Unknown set option: " + key);
    //                 }
    //             } catch (NumberFormatException ex) {
    //                 pw.println("Bad value: " + value);
    //             }
    //         } else if (args.length == 1 && "reset".equals(args[0])) {
    //             mUpdatesStopped = false;
    //             setDockStateLocked(mActualDockState);
    //         } else {
    //             pw.println("Dump current dock state, or:");
    //             pw.println("  set state <value>");
    //             pw.println("  reset");
    //         }
    //     }
    // } finally {
    //     Binder.restoreCallingIdentity(ident);
    // }
    assert(0 && "TODO");
    return NOERROR;
}

ECode DockObserver::BinderService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "DockObserver::BinderService";
    return NOERROR;
}

//===================================================================
// DockObserver::MyHandler
//===================================================================
DockObserver::MyHandler::MyHandler(
    /* [in] */ DockObserver* host)
    : Handler(FALSE)
{
}

//@Override
ECode DockObserver::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case DockObserver::MSG_DOCK_STATE_CHANGED:
            mHost->HandleDockStateChange();
            mHost->mWakeLock->ReleaseLock();
            break;
    }
    return NOERROR;
}

//===================================================================
// DockObserver::MyUEventObserver
//===================================================================

DockObserver::MyUEventObserver::MyUEventObserver(
    /* [in] */ DockObserver* host)
{}

//@Override
ECode DockObserver::MyUEventObserver::OnUEvent(
    /* [in] */ IUEvent* event)
{
    // if (Log.isLoggable(TAG, Log.VERBOSE)) {
    //     Slog.v(TAG, "Dock UEVENT: " + event.toString());
    // }

    AutoLock lock(mHost->mLock);
    String str;
    event->Get(String("SWITCH_STATE"), &str);
    Int32 ival;
    ECode ec = StringUtils::Parse(str, &ival);
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        Slogger::E("DockObserver", "Could not parse switch state from event %s",
            TO_CSTR(event));
        ec = NOERROR;
    }
    else {
        ec = mHost->SetActualDockStateLocked(ival);
    }
    return ec;
}

//===================================================================
// DockObserver
//===================================================================

DockObserver::DockObserver()
    : mSystemReady(FALSE)
    , mActualDockState(IIntent::EXTRA_DOCK_STATE_UNDOCKED)
    , mReportedDockState(IIntent::EXTRA_DOCK_STATE_UNDOCKED)
    , mPreviousDockState(IIntent::EXTRA_DOCK_STATE_UNDOCKED)
    , mUpdatesStopped(FALSE)
{}

ECode DockObserver::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(SystemService::constructor(context))

    mObserver = new MyUEventObserver(this);
    mHandler = new MyHandler(this);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);

    mPowerManager = IPowerManager::Probe(obj);
    mPowerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, TAG,
        (IPowerManagerWakeLock**)&mWakeLock);

    Init();  // set initial status

    return mObserver->StartObserving(DOCK_UEVENT_MATCH);
}

//@Override
ECode DockObserver::OnStart()
{
    AutoPtr<IBinder> service = (IBinder*)new BinderService();
    PublishBinderService(TAG, service);
    return NOERROR;
}

//@Override
ECode DockObserver::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == PHASE_ACTIVITY_MANAGER_READY) {
        {    AutoLock syncLock(mLock);
            mSystemReady = TRUE;

            // don't bother broadcasting undocked here
            if (mReportedDockState != IIntent::EXTRA_DOCK_STATE_UNDOCKED) {
                UpdateLocked();
            }
        }
    }
    return NOERROR;
}

ECode DockObserver::Init()
{
    {    AutoLock syncLock(mLock);

        AutoPtr<ArrayOf<Char32> > buffer = ArrayOf<Char32>::Alloc(1024);
        Int32 len, ival;
        String str;

        AutoPtr<IFileReader> file;
        ECode ec = CFileReader::New(DOCK_STATE_PATH, (IFileReader**)&file);
        FAIL_GOTO(ec, _EXIT_)

        ec = IReader::Probe(file)->Read(buffer, 0, 1024, &len);
        FAIL_GOTO(ec, _EXIT_)

        str = String(*buffer, 0, len);
        str = str.Trim();
        ival = StringUtils::ParseInt32(str);
        SetActualDockStateLocked(ival);
        mPreviousDockState = mActualDockState;

_EXIT_:
        ICloseable::Probe(file)->Close();

        if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
            Slogger::W(TAG, "This kernel does not have dock station support");
        }
        else if (FAILED(ec)) {
            Slogger::E(TAG, "DockObserver::Init() failed: ec=%08x", ec);
        }
    }
    return NOERROR;
}

ECode DockObserver::SetActualDockStateLocked(
    /* [in] */ Int32 newState)
{
    mActualDockState = newState;
    if (!mUpdatesStopped) {
        SetDockStateLocked(newState);
    }
    return NOERROR;
}

ECode DockObserver::SetDockStateLocked(
    /* [in] */ Int32 newState)
{
    if (newState != mReportedDockState) {
        mReportedDockState = newState;
        if (mSystemReady) {
            // Wake up immediately when docked or undocked.
            mPowerManager->WakeUp(SystemClock::GetUptimeMillis());
            UpdateLocked();
        }
    }
    return NOERROR;
}

ECode DockObserver::UpdateLocked()
{
    mWakeLock->AcquireLock();
    Boolean bval;
    return mHandler->SendEmptyMessage(MSG_DOCK_STATE_CHANGED, &bval);
}

ECode DockObserver::HandleDockStateChange()
{
    {    AutoLock syncLock(mLock);
        Slogger::I(TAG, "Dock state changed from %d to %d",
            mPreviousDockState, mReportedDockState);
        Int32 previousDockState = mPreviousDockState;
        mPreviousDockState = mReportedDockState;

        // Skip the dock intent if not yet provisioned.
        AutoPtr<IContentResolver> cr;
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        context->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsGlobal> sg;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
        Int32 ival;
        sg->GetInt32(cr, ISettingsGlobal::DEVICE_PROVISIONED, 0, &ival);
        if (ival == 0) {
            Slogger::I(TAG, "Device not provisioned, skipping dock broadcast");
            return NOERROR;
        }

        // Pack up the values and broadcast them to everyone
        AutoPtr<IIntent> intent;
        CIntent::New(IIntent::ACTION_DOCK_EVENT, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        intent->PutExtra(IIntent::EXTRA_DOCK_STATE, mReportedDockState);

        // Play a sound to provide feedback to confirm dock connection.
        // Particularly useful for flaky contact pins...
        sg->GetInt32(cr, ISettingsGlobal::DOCK_SOUNDS_ENABLED, 1, &ival);
        if (ival == 1) {
            String whichSound;
            if (mReportedDockState == IIntent::EXTRA_DOCK_STATE_UNDOCKED) {
                if ((previousDockState == IIntent::EXTRA_DOCK_STATE_DESK) ||
                    (previousDockState == IIntent::EXTRA_DOCK_STATE_LE_DESK) ||
                    (previousDockState == IIntent::EXTRA_DOCK_STATE_HE_DESK)) {
                    whichSound = ISettingsGlobal::DESK_UNDOCK_SOUND;
                }
                else if (previousDockState == IIntent::EXTRA_DOCK_STATE_CAR) {
                    whichSound = ISettingsGlobal::CAR_UNDOCK_SOUND;
                }
            }
            else {
                if ((mReportedDockState == IIntent::EXTRA_DOCK_STATE_DESK) ||
                    (mReportedDockState == IIntent::EXTRA_DOCK_STATE_LE_DESK) ||
                    (mReportedDockState == IIntent::EXTRA_DOCK_STATE_HE_DESK)) {
                    whichSound = ISettingsGlobal::DESK_DOCK_SOUND;
                }
                else if (mReportedDockState == IIntent::EXTRA_DOCK_STATE_CAR) {
                    whichSound = ISettingsGlobal::CAR_DOCK_SOUND;
                }
            }

            if (whichSound != NULL) {
                String soundPath;
                sg->GetString(cr, whichSound, &soundPath);
                if (soundPath != NULL) {
                    AutoPtr<IUriHelper> uriHelper;
                    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
                    StringBuilder sb("file://");
                    sb += soundPath;
                    AutoPtr<IUri> soundUri;
                    uriHelper->Parse(sb.ToString(), (IUri**)&soundUri);
                    if (soundUri != NULL) {
                        AutoPtr<IRingtone> sfx;
                        AutoPtr<IRingtoneManagerHelper> rmh;
                        assert(0 && "TODO");
                        // CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&rmh);
                        rmh->GetRingtone(context, soundUri, (IRingtone**)&sfx);
                        if (sfx != NULL) {
                            sfx->SetStreamType(IAudioManager::STREAM_SYSTEM);
                            sfx->Play();
                        }
                    }
                }
            }
        }

        // Send the dock event intent.
        // There are many components in the system watching for this so as to
        // adjust audio routing, screen orientation, etc.
        context->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
    }
    return NOERROR;
}


}// Server
}// Droid
}// Elastos
