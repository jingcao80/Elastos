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

#include "elastos/droid/incallui/InCallVideoCallListenerNotifier.h"
#include "Elastos.Droid.Internal.h"

using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Internal::Utility::CPreconditions;

namespace Elastos {
namespace Droid {
namespace InCallUI {

AutoPtr<InCallVideoCallListenerNotifier> InCallVideoCallListenerNotifier::InitNotifier()
{
    return new InCallVideoCallListenerNotifier();
}

AutoPtr<InCallVideoCallListenerNotifier> InCallVideoCallListenerNotifier::sInstance
        = InCallVideoCallListenerNotifier::InitNotifier();

AutoPtr<InCallVideoCallListenerNotifier> InCallVideoCallListenerNotifier::GetInstance()
{
    return sInstance;
}

void InCallVideoCallListenerNotifier::AddSessionModificationListener(
    /* [in] */ ISessionModificationListener* listener)
{
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    if (FAILED(preconditions->CheckNotNull(listener))) return;
    mSessionModificationListeners.Insert(listener);
}

void InCallVideoCallListenerNotifier::RemoveSessionModificationListener(
    /* [in] */ ISessionModificationListener* listener)
{
    if (listener != NULL) {
        mSessionModificationListeners.Erase(listener);
    }
}

void InCallVideoCallListenerNotifier::AddVideoEventListener(
    /* [in] */ IVideoEventListener* listener)
{
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    if (FAILED(preconditions->CheckNotNull(listener))) return;
    mVideoEventListeners.Insert(listener);
}

void InCallVideoCallListenerNotifier::RemoveVideoEventListener(
    /* [in] */ IVideoEventListener* listener)
{
    if (listener != NULL) {
        mVideoEventListeners.Erase(listener);
    }
}

void InCallVideoCallListenerNotifier::AddSurfaceChangeListener(
    /* [in] */ ISurfaceChangeListener* listener)
{
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    if (FAILED(preconditions->CheckNotNull(listener))) return;
    mSurfaceChangeListeners.Insert(listener);
}

void InCallVideoCallListenerNotifier::RemoveSurfaceChangeListener(
    /* [in] */ ISurfaceChangeListener* listener)
{
    if (listener != NULL) {
        mSurfaceChangeListeners.Erase(listener);
    }
}

void InCallVideoCallListenerNotifier::UpgradeToVideoRequest(
    /* [in] */ Call* call)
{
    Set<AutoPtr<ISessionModificationListener> >::Iterator it = mSessionModificationListeners.Begin();
    for (; it != mSessionModificationListeners.End(); ++it) {
        (*it)->OnUpgradeToVideoRequest((ICall*)call);
    }
}

void InCallVideoCallListenerNotifier::UpgradeToVideoSuccess(
    /* [in] */ Call* call)
{
    Set<AutoPtr<ISessionModificationListener> >::Iterator it = mSessionModificationListeners.Begin();
    for (; it != mSessionModificationListeners.End(); ++it) {
        (*it)->OnUpgradeToVideoSuccess((ICall*)call);
    }
}

void InCallVideoCallListenerNotifier::UpgradeToVideoFail(
    /* [in] */ Call* call)
{
    Set<AutoPtr<ISessionModificationListener> >::Iterator it = mSessionModificationListeners.Begin();
    for (; it != mSessionModificationListeners.End(); ++it) {
        (*it)->OnUpgradeToVideoFail((ICall*)call);
    }
}

void InCallVideoCallListenerNotifier::DowngradeToAudio(
    /* [in] */ Call* call)
{
    Set<AutoPtr<ISessionModificationListener> >::Iterator it = mSessionModificationListeners.Begin();
    for (; it != mSessionModificationListeners.End(); ++it) {
        (*it)->OnDowngradeToAudio((ICall*)call);
    }
}

void InCallVideoCallListenerNotifier::PeerPausedStateChanged(
    /* [in] */ Call* call,
    /* [in] */ Boolean paused)
{
    Set<AutoPtr<IVideoEventListener> >::Iterator it = mVideoEventListeners.Begin();
    for (; it != mVideoEventListeners.End(); ++it) {
        (*it)->OnPeerPauseStateChanged((ICall*)call, paused);
    }
}

void InCallVideoCallListenerNotifier::PeerDimensionsChanged(
    /* [in] */ Call* call,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Set<AutoPtr<ISurfaceChangeListener> >::Iterator it = mSurfaceChangeListeners.Begin();
    for (; it != mSurfaceChangeListeners.End(); ++it) {
        (*it)->OnUpdatePeerDimensions((ICall*)call, width, height);
    }
}

void InCallVideoCallListenerNotifier::CameraDimensionsChanged(
    /* [in] */ Call* call,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Set<AutoPtr<ISurfaceChangeListener> >::Iterator it = mSurfaceChangeListeners.Begin();
    for (; it != mSurfaceChangeListeners.End(); ++it) {
        (*it)->OnCameraDimensionsChange((ICall*)call, width, height);
    }
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
