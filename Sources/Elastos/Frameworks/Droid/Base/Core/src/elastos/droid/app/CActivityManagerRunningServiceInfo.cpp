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

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/CActivityManagerRunningServiceInfo.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CActivityManagerRunningServiceInfo, Object, IActivityManagerRunningServiceInfo, IParcelable)

CAR_OBJECT_IMPL(CActivityManagerRunningServiceInfo)

CActivityManagerRunningServiceInfo::CActivityManagerRunningServiceInfo()
    : mPid(0)
    , mUid(0)
    , mForeground(FALSE)
    , mActiveSince(0)
    , mStarted(FALSE)
    , mClientCount(0)
    , mCrashCount(0)
    , mLastActivityTime(0)
    , mRestarting(0)
    , mFlags(0)
    , mClientLabel(0)
{
}

ECode CActivityManagerRunningServiceInfo::constructor()
{
    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteInterfacePtr(mService));
    FAIL_RETURN(dest->WriteInt32(mPid));
    FAIL_RETURN(dest->WriteInt32(mUid));
    FAIL_RETURN(dest->WriteString(mProcess));
    FAIL_RETURN(dest->WriteInt32(mForeground ? 1 : 0));
    FAIL_RETURN(dest->WriteInt64(mActiveSince));
    FAIL_RETURN(dest->WriteInt32(mStarted ? 1 : 0));
    FAIL_RETURN(dest->WriteInt32(mClientCount));
    FAIL_RETURN(dest->WriteInt32(mCrashCount));
    FAIL_RETURN(dest->WriteInt64(mLastActivityTime));
    FAIL_RETURN(dest->WriteInt64(mRestarting));
    FAIL_RETURN(dest->WriteInt32(mFlags));
    FAIL_RETURN(dest->WriteString(mClientPackage));
    FAIL_RETURN(dest->WriteInt32(mClientLabel));

    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    FAIL_RETURN(source->ReadInterfacePtr((Handle32*)&mService));
    FAIL_RETURN(source->ReadInt32(&mPid));
    FAIL_RETURN(source->ReadInt32(&mUid));
    FAIL_RETURN(source->ReadString(&mProcess));
    Int32 value;
    FAIL_RETURN(source->ReadInt32(&value));
    mForeground = value != 0;
    FAIL_RETURN(source->ReadInt64(&mActiveSince));
    FAIL_RETURN(source->ReadInt32(&value));
    mStarted = value != 0;
    FAIL_RETURN(source->ReadInt32(&mClientCount));
    FAIL_RETURN(source->ReadInt32(&mCrashCount));
    FAIL_RETURN(source->ReadInt64(&mLastActivityTime));
    FAIL_RETURN(source->ReadInt64(&mRestarting));
    FAIL_RETURN(source->ReadInt32(&mFlags));
    FAIL_RETURN(source->ReadString(&mClientPackage));
    FAIL_RETURN(source->ReadInt32(&mClientLabel));

    return NOERROR;
}

/**
 * The service component.
 */
ECode CActivityManagerRunningServiceInfo::GetService(
    /* [out] */ IComponentName** service)
{
    VALIDATE_NOT_NULL(service);
    *service = mService;
    REFCOUNT_ADD(*service);
    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::SetService(
    /* [in] */ IComponentName* service)
{
    mService = service;
    return NOERROR;
}

/**
 * If non-zero, this is the process the service is running in.
 */
ECode CActivityManagerRunningServiceInfo::GetPid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid);
    *pid = mPid;
    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::SetPid(
    /* [in] */ Int32 pid)
{
    mPid = pid;
    return NOERROR;
}

/**
 * The UID that owns this service.
 */
ECode CActivityManagerRunningServiceInfo::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);
    *uid = mUid;
    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::SetUid(
    /* [in] */ Int32 uid)
{
    mUid = uid;
    return NOERROR;
}

/**
 * The name of the process this service runs in.
 */
ECode CActivityManagerRunningServiceInfo::GetProcess(
    /* [out] */ String* process)
{
    VALIDATE_NOT_NULL(process);
    *process = mProcess;
    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::SetProcess(
    /* [in] */ const String& process)
{
    mProcess = process;
    return NOERROR;
}

/**
 * Set to true if the service has asked to run as a foreground process.
 */
ECode CActivityManagerRunningServiceInfo::GetForeground(
    /* [out] */ Boolean* foreground)
{
    VALIDATE_NOT_NULL(foreground);
    *foreground = mForeground;
    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::SetForeground(
    /* [in] */ Boolean foreground)
{
    mForeground = foreground;
    return NOERROR;
}

/**
 * The time when the service was first made active, either by someone
 * starting or binding to it.  This
 * is in units of {@link android.os.SystemClock#elapsedRealtime()}.
 */
ECode CActivityManagerRunningServiceInfo::GetActiveSince(
    /* [out] */ Int64* activeSince)
{
    VALIDATE_NOT_NULL(activeSince);
    *activeSince = mActiveSince;
    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::SetActiveSince(
    /* [in] */ Int64 activeSince)
{
    mActiveSince = activeSince;
    return NOERROR;
}

/**
 * Set to true if this service has been explicitly started.
 */
ECode CActivityManagerRunningServiceInfo::IsStarted(
    /* [out] */ Boolean* started)
{
    VALIDATE_NOT_NULL(started);
    *started = mStarted;
    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::SetStarted(
    /* [in] */ Boolean started)
{
    mStarted = started;
    return NOERROR;
}

/**
 * Number of clients connected to the service.
 */
ECode CActivityManagerRunningServiceInfo::GetClientCount(
    /* [out] */ Int32* clientCount)
{
    VALIDATE_NOT_NULL(clientCount);
    *clientCount = mClientCount;
    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::SetClientCount(
    /* [in] */ Int32 clientCount)
{
    mClientCount = clientCount;
    return NOERROR;
}

/**
 * Number of times the service's process has crashed while the service
 * is running.
 */
ECode CActivityManagerRunningServiceInfo::GetCrashCount(
    /* [out] */ Int32* crashCount)
{
    VALIDATE_NOT_NULL(crashCount);
    *crashCount = mCrashCount;
    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::SetCrashCount(
    /* [in] */ Int32 crashCount)
{
    mCrashCount = crashCount;
    return NOERROR;
}

/**
 * The time when there was last activity in the service (either
 * explicit requests to start it or clients binding to it).  This
 * is in units of {@link android.os.SystemClock#uptimeMillis()}.
 */
ECode CActivityManagerRunningServiceInfo::GetLastActivityTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mLastActivityTime;
    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::SetLastActivityTime(
    /* [in] */ Int64 time)
{
    mLastActivityTime = time;
    return NOERROR;
}

/**
 * If non-zero, this service is not currently running, but scheduled to
 * restart at the given time.
 */
ECode CActivityManagerRunningServiceInfo::GetRestarting(
    /* [out] */ Int64* restarting)
{
    VALIDATE_NOT_NULL(restarting);
    *restarting = mRestarting;
    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::SetRestarting(
    /* [in] */ Int64 restarting)
{
    mRestarting = restarting;
    return NOERROR;
}

/**
 * Running flags.
 */
ECode CActivityManagerRunningServiceInfo::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;
    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

/**
 * For special services that are bound to by system code, this is
 * the package that holds the binding.
 */
ECode CActivityManagerRunningServiceInfo::GetClientPackage(
    /* [out] */ String* clientPackage)
{
    VALIDATE_NOT_NULL(clientPackage);
    *clientPackage = mClientPackage;
    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::SetClientPackage(
    /* [in] */ const String& clientPackage)
{
    mClientPackage = clientPackage;
    return NOERROR;
}

/**
 * For special services that are bound to by system code, this is
 * a string resource providing a user-visible label for who the
 * client is.
 */
ECode CActivityManagerRunningServiceInfo::GetClientLabel(
    /* [out] */ Int32* clientLabel)
{
    VALIDATE_NOT_NULL(clientLabel);
    *clientLabel = mClientLabel;
    return NOERROR;
}

ECode CActivityManagerRunningServiceInfo::SetClientLabel(
    /* [in] */ Int32 clientLabel)
{
    mClientLabel = clientLabel;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
