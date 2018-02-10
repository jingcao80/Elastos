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

#include "elastos/droid/javaproxy/CMediaSessionCallbackStubNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Media::Session::EIID_IISessionCallback;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CMediaSessionCallbackStubNative::TAG("CMediaSessionCallbackStubNative");

CAR_INTERFACE_IMPL_2(CMediaSessionCallbackStubNative, Object, IISessionCallback, IBinder)

CAR_OBJECT_IMPL(CMediaSessionCallbackStubNative)

CMediaSessionCallbackStubNative::~CMediaSessionCallbackStubNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CMediaSessionCallbackStubNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::OnCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle * args,
        /* [in] */ IResultReceiver * cb)
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::OnMediaButton(
        /* [in] */ IIntent * mediaButtonIntent,
        /* [in] */ Int32 sequenceNumber,
        /* [in] */ IResultReceiver * cb)
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

    // These callbacks are for the TransportPerformer
ECode CMediaSessionCallbackStubNative::OnPlay()
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::OnPlayFromMediaId(
        /* [in] */ const String& mediaId,
        /* [in] */ IBundle * extras)
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::OnPlayFromSearch(
        /* [in] */ const String& query,
        /* [in] */ IBundle * extras)
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::OnSkipToTrack(
        /* [in] */ Int64 id)
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::OnPause()
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::OnStop()
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::OnNext()
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::OnPrevious()
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::OnFastForward()
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::OnRewind()
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::OnSeekTo(
        /* [in] */ Int64 pos)
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::OnRate(
        /* [in] */ IRating * rating)
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

    // @Override
ECode CMediaSessionCallbackStubNative::SetRemoteControlClientBrowsedPlayer()
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

    // @Override
ECode CMediaSessionCallbackStubNative::SetRemoteControlClientPlayItem(
        /* [in] */ Int64 uid,
        /* [in] */ Int32 scope)
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

    // @Override
ECode CMediaSessionCallbackStubNative::GetRemoteControlClientNowPlayingEntries()
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::OnCustomAction(
        /* [in] */ const String& action,
        /* [in] */ IBundle * args)
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

    // These callbacks are for volume handling
ECode CMediaSessionCallbackStubNative::OnAdjustVolume(
        /* [in] */ Int32 direction)
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::OnSetVolumeTo(
        /* [in] */ Int32 value)
{
    LOGGERD(TAG, "TODO leliang, no impl, line%d", __LINE__);
    return NOERROR;
}

ECode CMediaSessionCallbackStubNative::ToString(
    /* [out] */ String* str)
{
    LOGGERD(TAG, "+ CMediaSessionCallbackStubNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, TAG, "FindClass: Object %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: toString %d", __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: toString %d", __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    LOGGERD(TAG, "- CMediaSessionCallbackStubNative::ToString()");
    return NOERROR;
}

}
}
}

