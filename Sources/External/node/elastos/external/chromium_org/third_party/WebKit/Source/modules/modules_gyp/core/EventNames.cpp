// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#ifdef SKIP_STATIC_CONSTRUCTORS_ON_GCC
#define EVENT_NAMES_HIDE_GLOBALS 1
#endif

#include "EventNames.h"

#include "wtf/StaticConstructors.h"

namespace WebCore {
namespace EventNames {

using namespace WTF;

DEFINE_GLOBAL(AtomicString, ApplicationCacheErrorEvent)
DEFINE_GLOBAL(AtomicString, AutocompleteErrorEvent)
DEFINE_GLOBAL(AtomicString, BeforeUnloadEvent)
DEFINE_GLOBAL(AtomicString, CompositionEvent)
DEFINE_GLOBAL(AtomicString, CustomEvent)
DEFINE_GLOBAL(AtomicString, ErrorEvent)
DEFINE_GLOBAL(AtomicString, Event)
DEFINE_GLOBAL(AtomicString, FocusEvent)
DEFINE_GLOBAL(AtomicString, HashChangeEvent)
DEFINE_GLOBAL(AtomicString, KeyboardEvent)
DEFINE_GLOBAL(AtomicString, MessageEvent)
DEFINE_GLOBAL(AtomicString, MouseEvent)
DEFINE_GLOBAL(AtomicString, MutationEvent)
DEFINE_GLOBAL(AtomicString, OverflowEvent)
DEFINE_GLOBAL(AtomicString, PageTransitionEvent)
DEFINE_GLOBAL(AtomicString, PopStateEvent)
DEFINE_GLOBAL(AtomicString, ProgressEvent)
DEFINE_GLOBAL(AtomicString, ResourceProgressEvent)
DEFINE_GLOBAL(AtomicString, TextEvent)
DEFINE_GLOBAL(AtomicString, TouchEvent)
DEFINE_GLOBAL(AtomicString, TransitionEvent)
DEFINE_GLOBAL(AtomicString, UIEvent)
DEFINE_GLOBAL(AtomicString, WebKitAnimationEvent)
DEFINE_GLOBAL(AtomicString, WheelEvent)
DEFINE_GLOBAL(AtomicString, WebGLContextEvent)
DEFINE_GLOBAL(AtomicString, TrackEvent)
DEFINE_GLOBAL(AtomicString, StorageEvent)
DEFINE_GLOBAL(AtomicString, SVGZoomEvent)
DEFINE_GLOBAL(AtomicString, XMLHttpRequestProgressEvent)
DEFINE_GLOBAL(AtomicString, SecurityPolicyViolationEvent)
DEFINE_GLOBAL(AtomicString, CSSFontFaceLoadEvent)
DEFINE_GLOBAL(AtomicString, MediaKeyEvent)
DEFINE_GLOBAL(AtomicString, AnimationPlayerEvent)

void init()
{
    StringImpl* ApplicationCacheErrorEventImpl = StringImpl::createStatic("ApplicationCacheErrorEvent", 26, 10025690);
    StringImpl* AutocompleteErrorEventImpl = StringImpl::createStatic("AutocompleteErrorEvent", 22, 11231409);
    StringImpl* BeforeUnloadEventImpl = StringImpl::createStatic("BeforeUnloadEvent", 17, 6032602);
    StringImpl* CompositionEventImpl = StringImpl::createStatic("CompositionEvent", 16, 9205103);
    StringImpl* CustomEventImpl = StringImpl::createStatic("CustomEvent", 11, 13370215);
    StringImpl* ErrorEventImpl = StringImpl::createStatic("ErrorEvent", 10, 3070522);
    StringImpl* EventImpl = StringImpl::createStatic("Event", 5, 14343017);
    StringImpl* FocusEventImpl = StringImpl::createStatic("FocusEvent", 10, 8217765);
    StringImpl* HashChangeEventImpl = StringImpl::createStatic("HashChangeEvent", 15, 15010276);
    StringImpl* KeyboardEventImpl = StringImpl::createStatic("KeyboardEvent", 13, 6959608);
    StringImpl* MessageEventImpl = StringImpl::createStatic("MessageEvent", 12, 3580588);
    StringImpl* MouseEventImpl = StringImpl::createStatic("MouseEvent", 10, 12822602);
    StringImpl* MutationEventImpl = StringImpl::createStatic("MutationEvent", 13, 3855658);
    StringImpl* OverflowEventImpl = StringImpl::createStatic("OverflowEvent", 13, 1066761);
    StringImpl* PageTransitionEventImpl = StringImpl::createStatic("PageTransitionEvent", 19, 14165289);
    StringImpl* PopStateEventImpl = StringImpl::createStatic("PopStateEvent", 13, 13550868);
    StringImpl* ProgressEventImpl = StringImpl::createStatic("ProgressEvent", 13, 457487);
    StringImpl* ResourceProgressEventImpl = StringImpl::createStatic("ResourceProgressEvent", 21, 3169626);
    StringImpl* TextEventImpl = StringImpl::createStatic("TextEvent", 9, 9996370);
    StringImpl* TouchEventImpl = StringImpl::createStatic("TouchEvent", 10, 8776614);
    StringImpl* TransitionEventImpl = StringImpl::createStatic("TransitionEvent", 15, 4011983);
    StringImpl* UIEventImpl = StringImpl::createStatic("UIEvent", 7, 9523113);
    StringImpl* WebKitAnimationEventImpl = StringImpl::createStatic("WebKitAnimationEvent", 20, 3635677);
    StringImpl* WheelEventImpl = StringImpl::createStatic("WheelEvent", 10, 580237);
    StringImpl* WebGLContextEventImpl = StringImpl::createStatic("WebGLContextEvent", 17, 8713913);
    StringImpl* TrackEventImpl = StringImpl::createStatic("TrackEvent", 10, 14136022);
    StringImpl* StorageEventImpl = StringImpl::createStatic("StorageEvent", 12, 5246637);
    StringImpl* SVGZoomEventImpl = StringImpl::createStatic("SVGZoomEvent", 12, 13387139);
    StringImpl* XMLHttpRequestProgressEventImpl = StringImpl::createStatic("XMLHttpRequestProgressEvent", 27, 740569);
    StringImpl* SecurityPolicyViolationEventImpl = StringImpl::createStatic("SecurityPolicyViolationEvent", 28, 945681);
    StringImpl* CSSFontFaceLoadEventImpl = StringImpl::createStatic("CSSFontFaceLoadEvent", 20, 9053401);
    StringImpl* MediaKeyEventImpl = StringImpl::createStatic("MediaKeyEvent", 13, 7226908);
    StringImpl* AnimationPlayerEventImpl = StringImpl::createStatic("AnimationPlayerEvent", 20, 11993449);

    new ((void*)&ApplicationCacheErrorEvent) AtomicString(ApplicationCacheErrorEventImpl);
    new ((void*)&AutocompleteErrorEvent) AtomicString(AutocompleteErrorEventImpl);
    new ((void*)&BeforeUnloadEvent) AtomicString(BeforeUnloadEventImpl);
    new ((void*)&CompositionEvent) AtomicString(CompositionEventImpl);
    new ((void*)&CustomEvent) AtomicString(CustomEventImpl);
    new ((void*)&ErrorEvent) AtomicString(ErrorEventImpl);
    new ((void*)&Event) AtomicString(EventImpl);
    new ((void*)&FocusEvent) AtomicString(FocusEventImpl);
    new ((void*)&HashChangeEvent) AtomicString(HashChangeEventImpl);
    new ((void*)&KeyboardEvent) AtomicString(KeyboardEventImpl);
    new ((void*)&MessageEvent) AtomicString(MessageEventImpl);
    new ((void*)&MouseEvent) AtomicString(MouseEventImpl);
    new ((void*)&MutationEvent) AtomicString(MutationEventImpl);
    new ((void*)&OverflowEvent) AtomicString(OverflowEventImpl);
    new ((void*)&PageTransitionEvent) AtomicString(PageTransitionEventImpl);
    new ((void*)&PopStateEvent) AtomicString(PopStateEventImpl);
    new ((void*)&ProgressEvent) AtomicString(ProgressEventImpl);
    new ((void*)&ResourceProgressEvent) AtomicString(ResourceProgressEventImpl);
    new ((void*)&TextEvent) AtomicString(TextEventImpl);
    new ((void*)&TouchEvent) AtomicString(TouchEventImpl);
    new ((void*)&TransitionEvent) AtomicString(TransitionEventImpl);
    new ((void*)&UIEvent) AtomicString(UIEventImpl);
    new ((void*)&WebKitAnimationEvent) AtomicString(WebKitAnimationEventImpl);
    new ((void*)&WheelEvent) AtomicString(WheelEventImpl);
    new ((void*)&WebGLContextEvent) AtomicString(WebGLContextEventImpl);
    new ((void*)&TrackEvent) AtomicString(TrackEventImpl);
    new ((void*)&StorageEvent) AtomicString(StorageEventImpl);
    new ((void*)&SVGZoomEvent) AtomicString(SVGZoomEventImpl);
    new ((void*)&XMLHttpRequestProgressEvent) AtomicString(XMLHttpRequestProgressEventImpl);
    new ((void*)&SecurityPolicyViolationEvent) AtomicString(SecurityPolicyViolationEventImpl);
    new ((void*)&CSSFontFaceLoadEvent) AtomicString(CSSFontFaceLoadEventImpl);
    new ((void*)&MediaKeyEvent) AtomicString(MediaKeyEventImpl);
    new ((void*)&AnimationPlayerEvent) AtomicString(AnimationPlayerEventImpl);
}

} // EventNames
} // WebCore
