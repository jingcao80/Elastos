// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#ifdef SKIP_STATIC_CONSTRUCTORS_ON_GCC
#define EVENT_TARGET_NAMES_HIDE_GLOBALS 1
#endif

#include "EventTargetNames.h"

#include "wtf/StaticConstructors.h"

namespace WebCore {
namespace EventTargetNames {

using namespace WTF;

DEFINE_GLOBAL(AtomicString, AnimationPlayer)
DEFINE_GLOBAL(AtomicString, FontFaceSet)
DEFINE_GLOBAL(AtomicString, MessagePort)
DEFINE_GLOBAL(AtomicString, Node)
DEFINE_GLOBAL(AtomicString, FileReader)
DEFINE_GLOBAL(AtomicString, MediaController)
DEFINE_GLOBAL(AtomicString, InputMethodContext)
DEFINE_GLOBAL(AtomicString, AudioTrackList)
DEFINE_GLOBAL(AtomicString, TextTrack)
DEFINE_GLOBAL(AtomicString, TextTrackCue)
DEFINE_GLOBAL(AtomicString, TextTrackList)
DEFINE_GLOBAL(AtomicString, VideoTrackList)
DEFINE_GLOBAL(AtomicString, ApplicationCache)
DEFINE_GLOBAL(AtomicString, EventSource)
DEFINE_GLOBAL(AtomicString, Performance)
DEFINE_GLOBAL(AtomicString, DedicatedWorkerGlobalScope)
DEFINE_GLOBAL(AtomicString, SharedWorker)
DEFINE_GLOBAL(AtomicString, SharedWorkerGlobalScope)
DEFINE_GLOBAL(AtomicString, Worker)
DEFINE_GLOBAL(AtomicString, XMLHttpRequest)
DEFINE_GLOBAL(AtomicString, XMLHttpRequestUpload)
DEFINE_GLOBAL(AtomicString, LocalDOMWindow)

void init()
{
    StringImpl* AnimationPlayerImpl = StringImpl::createStatic("AnimationPlayer", 15, 8316901);
    StringImpl* FontFaceSetImpl = StringImpl::createStatic("FontFaceSet", 11, 15992392);
    StringImpl* MessagePortImpl = StringImpl::createStatic("MessagePort", 11, 7675160);
    StringImpl* NodeImpl = StringImpl::createStatic("Node", 4, 7481749);
    StringImpl* FileReaderImpl = StringImpl::createStatic("FileReader", 10, 1122872);
    StringImpl* MediaControllerImpl = StringImpl::createStatic("MediaController", 15, 5103076);
    StringImpl* InputMethodContextImpl = StringImpl::createStatic("InputMethodContext", 18, 6735798);
    StringImpl* AudioTrackListImpl = StringImpl::createStatic("AudioTrackList", 14, 4289573);
    StringImpl* TextTrackImpl = StringImpl::createStatic("TextTrack", 9, 6141630);
    StringImpl* TextTrackCueImpl = StringImpl::createStatic("TextTrackCue", 12, 12570487);
    StringImpl* TextTrackListImpl = StringImpl::createStatic("TextTrackList", 13, 15622086);
    StringImpl* VideoTrackListImpl = StringImpl::createStatic("VideoTrackList", 14, 12436801);
    StringImpl* ApplicationCacheImpl = StringImpl::createStatic("ApplicationCache", 16, 14896278);
    StringImpl* EventSourceImpl = StringImpl::createStatic("EventSource", 11, 10057717);
    StringImpl* PerformanceImpl = StringImpl::createStatic("Performance", 11, 16508941);
    StringImpl* DedicatedWorkerGlobalScopeImpl = StringImpl::createStatic("DedicatedWorkerGlobalScope", 26, 16739179);
    StringImpl* SharedWorkerImpl = StringImpl::createStatic("SharedWorker", 12, 5095866);
    StringImpl* SharedWorkerGlobalScopeImpl = StringImpl::createStatic("SharedWorkerGlobalScope", 23, 14997459);
    StringImpl* WorkerImpl = StringImpl::createStatic("Worker", 6, 14821068);
    StringImpl* XMLHttpRequestImpl = StringImpl::createStatic("XMLHttpRequest", 14, 5139743);
    StringImpl* XMLHttpRequestUploadImpl = StringImpl::createStatic("XMLHttpRequestUpload", 20, 16716896);
    StringImpl* LocalDOMWindowImpl = StringImpl::createStatic("LocalDOMWindow", 14, 12069601);

    new ((void*)&AnimationPlayer) AtomicString(AnimationPlayerImpl);
    new ((void*)&FontFaceSet) AtomicString(FontFaceSetImpl);
    new ((void*)&MessagePort) AtomicString(MessagePortImpl);
    new ((void*)&Node) AtomicString(NodeImpl);
    new ((void*)&FileReader) AtomicString(FileReaderImpl);
    new ((void*)&MediaController) AtomicString(MediaControllerImpl);
    new ((void*)&InputMethodContext) AtomicString(InputMethodContextImpl);
    new ((void*)&AudioTrackList) AtomicString(AudioTrackListImpl);
    new ((void*)&TextTrack) AtomicString(TextTrackImpl);
    new ((void*)&TextTrackCue) AtomicString(TextTrackCueImpl);
    new ((void*)&TextTrackList) AtomicString(TextTrackListImpl);
    new ((void*)&VideoTrackList) AtomicString(VideoTrackListImpl);
    new ((void*)&ApplicationCache) AtomicString(ApplicationCacheImpl);
    new ((void*)&EventSource) AtomicString(EventSourceImpl);
    new ((void*)&Performance) AtomicString(PerformanceImpl);
    new ((void*)&DedicatedWorkerGlobalScope) AtomicString(DedicatedWorkerGlobalScopeImpl);
    new ((void*)&SharedWorker) AtomicString(SharedWorkerImpl);
    new ((void*)&SharedWorkerGlobalScope) AtomicString(SharedWorkerGlobalScopeImpl);
    new ((void*)&Worker) AtomicString(WorkerImpl);
    new ((void*)&XMLHttpRequest) AtomicString(XMLHttpRequestImpl);
    new ((void*)&XMLHttpRequestUpload) AtomicString(XMLHttpRequestUploadImpl);
    new ((void*)&LocalDOMWindow) AtomicString(LocalDOMWindowImpl);
}

} // EventTargetNames
} // WebCore
