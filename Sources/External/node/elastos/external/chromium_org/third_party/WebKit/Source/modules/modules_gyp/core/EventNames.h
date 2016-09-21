// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EventNames_h
#define EventNames_h

#include "wtf/text/AtomicString.h"

namespace WebCore {
namespace EventNames {

#ifndef EVENT_NAMES_HIDE_GLOBALS

extern const WTF::AtomicString ApplicationCacheErrorEvent;
extern const WTF::AtomicString AutocompleteErrorEvent;
extern const WTF::AtomicString BeforeUnloadEvent;
extern const WTF::AtomicString CompositionEvent;
extern const WTF::AtomicString CustomEvent;
extern const WTF::AtomicString ErrorEvent;
extern const WTF::AtomicString Event;
extern const WTF::AtomicString FocusEvent;
extern const WTF::AtomicString HashChangeEvent;
extern const WTF::AtomicString KeyboardEvent;
extern const WTF::AtomicString MessageEvent;
extern const WTF::AtomicString MouseEvent;
extern const WTF::AtomicString MutationEvent;
extern const WTF::AtomicString OverflowEvent;
extern const WTF::AtomicString PageTransitionEvent;
extern const WTF::AtomicString PopStateEvent;
extern const WTF::AtomicString ProgressEvent;
extern const WTF::AtomicString ResourceProgressEvent;
extern const WTF::AtomicString TextEvent;
extern const WTF::AtomicString TouchEvent;
extern const WTF::AtomicString TransitionEvent;
extern const WTF::AtomicString UIEvent;
extern const WTF::AtomicString WebKitAnimationEvent;
extern const WTF::AtomicString WheelEvent;
extern const WTF::AtomicString WebGLContextEvent;
extern const WTF::AtomicString TrackEvent;
extern const WTF::AtomicString StorageEvent;
extern const WTF::AtomicString SVGZoomEvent;
extern const WTF::AtomicString XMLHttpRequestProgressEvent;
extern const WTF::AtomicString SecurityPolicyViolationEvent;
extern const WTF::AtomicString CSSFontFaceLoadEvent;
extern const WTF::AtomicString MediaKeyEvent;
extern const WTF::AtomicString AnimationPlayerEvent;

#endif // EVENT_NAMES_HIDE_GLOBALS

 void init();

} // EventNames
} // WebCore

#endif
