// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EventTargetNames_h
#define EventTargetNames_h

#include "wtf/text/AtomicString.h"

namespace WebCore {
namespace EventTargetNames {

#ifndef EVENT_TARGET_NAMES_HIDE_GLOBALS

extern const WTF::AtomicString AnimationPlayer;
extern const WTF::AtomicString FontFaceSet;
extern const WTF::AtomicString MessagePort;
extern const WTF::AtomicString Node;
extern const WTF::AtomicString FileReader;
extern const WTF::AtomicString MediaController;
extern const WTF::AtomicString InputMethodContext;
extern const WTF::AtomicString AudioTrackList;
extern const WTF::AtomicString TextTrack;
extern const WTF::AtomicString TextTrackCue;
extern const WTF::AtomicString TextTrackList;
extern const WTF::AtomicString VideoTrackList;
extern const WTF::AtomicString ApplicationCache;
extern const WTF::AtomicString EventSource;
extern const WTF::AtomicString Performance;
extern const WTF::AtomicString DedicatedWorkerGlobalScope;
extern const WTF::AtomicString SharedWorker;
extern const WTF::AtomicString SharedWorkerGlobalScope;
extern const WTF::AtomicString Worker;
extern const WTF::AtomicString XMLHttpRequest;
extern const WTF::AtomicString XMLHttpRequestUpload;
extern const WTF::AtomicString LocalDOMWindow;

#endif // EVENT_TARGET_NAMES_HIDE_GLOBALS

 void init();

} // EventTargetNames
} // WebCore

#endif
