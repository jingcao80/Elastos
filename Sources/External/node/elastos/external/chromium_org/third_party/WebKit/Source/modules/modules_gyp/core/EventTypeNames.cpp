// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#ifdef SKIP_STATIC_CONSTRUCTORS_ON_GCC
#define EVENT_TYPE_NAMES_HIDE_GLOBALS 1
#endif

#include "EventTypeNames.h"

#include "wtf/StaticConstructors.h"

namespace WebCore {
namespace EventTypeNames {

using namespace WTF;

DEFINE_GLOBAL(AtomicString, DOMActivate)
DEFINE_GLOBAL(AtomicString, DOMCharacterDataModified)
DEFINE_GLOBAL(AtomicString, DOMContentLoaded)
DEFINE_GLOBAL(AtomicString, DOMFocusIn)
DEFINE_GLOBAL(AtomicString, DOMFocusOut)
DEFINE_GLOBAL(AtomicString, DOMNodeInserted)
DEFINE_GLOBAL(AtomicString, DOMNodeInsertedIntoDocument)
DEFINE_GLOBAL(AtomicString, DOMNodeRemoved)
DEFINE_GLOBAL(AtomicString, DOMNodeRemovedFromDocument)
DEFINE_GLOBAL(AtomicString, DOMSubtreeModified)
DEFINE_GLOBAL(AtomicString, abort)
DEFINE_GLOBAL(AtomicString, activate)
DEFINE_GLOBAL(AtomicString, addsourcebuffer)
DEFINE_GLOBAL(AtomicString, addstream)
DEFINE_GLOBAL(AtomicString, addtrack)
DEFINE_GLOBAL(AtomicString, animationend)
DEFINE_GLOBAL(AtomicString, animationiteration)
DEFINE_GLOBAL(AtomicString, animationstart)
DEFINE_GLOBAL(AtomicString, audioend)
DEFINE_GLOBAL(AtomicString, audioprocess)
DEFINE_GLOBAL(AtomicString, audiostart)
DEFINE_GLOBAL(AtomicString, autocomplete)
DEFINE_GLOBAL(AtomicString, autocompleteerror)
DEFINE_GLOBAL(AtomicString, beforecopy)
DEFINE_GLOBAL(AtomicString, beforecut)
DEFINE_GLOBAL(AtomicString, beforepaste)
DEFINE_GLOBAL(AtomicString, beforeunload)
DEFINE_GLOBAL(AtomicString, beginEvent)
DEFINE_GLOBAL(AtomicString, blocked)
DEFINE_GLOBAL(AtomicString, blur)
DEFINE_GLOBAL(AtomicString, boundary)
DEFINE_GLOBAL(AtomicString, cached)
DEFINE_GLOBAL(AtomicString, cancel)
DEFINE_GLOBAL(AtomicString, candidatewindowhide)
DEFINE_GLOBAL(AtomicString, candidatewindowshow)
DEFINE_GLOBAL(AtomicString, candidatewindowupdate)
DEFINE_GLOBAL(AtomicString, canplay)
DEFINE_GLOBAL(AtomicString, canplaythrough)
DEFINE_GLOBAL(AtomicString, change)
DEFINE_GLOBAL(AtomicString, chargingchange)
DEFINE_GLOBAL(AtomicString, chargingtimechange)
DEFINE_GLOBAL(AtomicString, checking)
DEFINE_GLOBAL(AtomicString, click)
DEFINE_GLOBAL(AtomicString, close)
DEFINE_GLOBAL(AtomicString, complete)
DEFINE_GLOBAL(AtomicString, compositionend)
DEFINE_GLOBAL(AtomicString, compositionstart)
DEFINE_GLOBAL(AtomicString, compositionupdate)
DEFINE_GLOBAL(AtomicString, connect)
DEFINE_GLOBAL(AtomicString, connecting)
DEFINE_GLOBAL(AtomicString, contextlost)
DEFINE_GLOBAL(AtomicString, contextmenu)
DEFINE_GLOBAL(AtomicString, contextrestored)
DEFINE_GLOBAL(AtomicString, copy)
DEFINE_GLOBAL(AtomicString, cuechange)
DEFINE_GLOBAL(AtomicString, cut)
DEFINE_GLOBAL(AtomicString, datachannel)
DEFINE_GLOBAL(AtomicString, dblclick)
DEFINE_GLOBAL(AtomicString, devicelight)
DEFINE_GLOBAL(AtomicString, devicemotion)
DEFINE_GLOBAL(AtomicString, deviceorientation)
DEFINE_GLOBAL(AtomicString, dischargingtimechange)
DEFINE_GLOBAL(AtomicString, disconnect)
DEFINE_GLOBAL(AtomicString, display)
DEFINE_GLOBAL(AtomicString, downloading)
DEFINE_GLOBAL(AtomicString, drag)
DEFINE_GLOBAL(AtomicString, dragend)
DEFINE_GLOBAL(AtomicString, dragenter)
DEFINE_GLOBAL(AtomicString, dragleave)
DEFINE_GLOBAL(AtomicString, dragover)
DEFINE_GLOBAL(AtomicString, dragstart)
DEFINE_GLOBAL(AtomicString, drop)
DEFINE_GLOBAL(AtomicString, durationchange)
DEFINE_GLOBAL(AtomicString, emptied)
DEFINE_GLOBAL(AtomicString, end)
DEFINE_GLOBAL(AtomicString, endEvent)
DEFINE_GLOBAL(AtomicString, ended)
DEFINE_GLOBAL(AtomicString, enter)
DEFINE_GLOBAL(AtomicString, error)
DEFINE_GLOBAL(AtomicString, exit)
DEFINE_GLOBAL(AtomicString, fetch)
DEFINE_GLOBAL(AtomicString, finish)
DEFINE_GLOBAL(AtomicString, focus)
DEFINE_GLOBAL(AtomicString, focusin)
DEFINE_GLOBAL(AtomicString, focusout)
DEFINE_GLOBAL(AtomicString, gamepadconnected)
DEFINE_GLOBAL(AtomicString, gamepaddisconnected)
DEFINE_GLOBAL(AtomicString, gesturescrollend)
DEFINE_GLOBAL(AtomicString, gesturescrollstart)
DEFINE_GLOBAL(AtomicString, gesturescrollupdate)
DEFINE_GLOBAL(AtomicString, gestureshowpress)
DEFINE_GLOBAL(AtomicString, gesturetap)
DEFINE_GLOBAL(AtomicString, gesturetapdown)
DEFINE_GLOBAL(AtomicString, gesturetapunconfirmed)
DEFINE_GLOBAL(AtomicString, hashchange)
DEFINE_GLOBAL(AtomicString, icecandidate)
DEFINE_GLOBAL(AtomicString, iceconnectionstatechange)
DEFINE_GLOBAL(AtomicString, input)
DEFINE_GLOBAL(AtomicString, install)
DEFINE_GLOBAL(AtomicString, invalid)
DEFINE_GLOBAL(AtomicString, keydown)
DEFINE_GLOBAL(AtomicString, keypress)
DEFINE_GLOBAL(AtomicString, keyup)
DEFINE_GLOBAL(AtomicString, languagechange)
DEFINE_GLOBAL(AtomicString, levelchange)
DEFINE_GLOBAL(AtomicString, load)
DEFINE_GLOBAL(AtomicString, loadeddata)
DEFINE_GLOBAL(AtomicString, loadedmetadata)
DEFINE_GLOBAL(AtomicString, loadend)
DEFINE_GLOBAL(AtomicString, loading)
DEFINE_GLOBAL(AtomicString, loadingdone)
DEFINE_GLOBAL(AtomicString, loadingerror)
DEFINE_GLOBAL(AtomicString, loadstart)
DEFINE_GLOBAL(AtomicString, mark)
DEFINE_GLOBAL(AtomicString, message)
DEFINE_GLOBAL(AtomicString, midimessage)
DEFINE_GLOBAL(AtomicString, mousedown)
DEFINE_GLOBAL(AtomicString, mouseenter)
DEFINE_GLOBAL(AtomicString, mouseleave)
DEFINE_GLOBAL(AtomicString, mousemove)
DEFINE_GLOBAL(AtomicString, mouseout)
DEFINE_GLOBAL(AtomicString, mouseover)
DEFINE_GLOBAL(AtomicString, mouseup)
DEFINE_GLOBAL(AtomicString, mousewheel)
DEFINE_GLOBAL(AtomicString, mute)
DEFINE_GLOBAL(AtomicString, needkey)
DEFINE_GLOBAL(AtomicString, negotiationneeded)
DEFINE_GLOBAL(AtomicString, nomatch)
DEFINE_GLOBAL(AtomicString, noupdate)
DEFINE_GLOBAL(AtomicString, obsolete)
DEFINE_GLOBAL(AtomicString, offline)
DEFINE_GLOBAL(AtomicString, online)
DEFINE_GLOBAL(AtomicString, open)
DEFINE_GLOBAL(AtomicString, orientationchange)
DEFINE_GLOBAL(AtomicString, overflowchanged)
DEFINE_GLOBAL(AtomicString, pagehide)
DEFINE_GLOBAL(AtomicString, pageshow)
DEFINE_GLOBAL(AtomicString, paste)
DEFINE_GLOBAL(AtomicString, pause)
DEFINE_GLOBAL(AtomicString, play)
DEFINE_GLOBAL(AtomicString, playing)
DEFINE_GLOBAL(AtomicString, pointerlockchange)
DEFINE_GLOBAL(AtomicString, pointerlockerror)
DEFINE_GLOBAL(AtomicString, popstate)
DEFINE_GLOBAL(AtomicString, progress)
DEFINE_GLOBAL(AtomicString, push)
DEFINE_GLOBAL(AtomicString, ratechange)
DEFINE_GLOBAL(AtomicString, ready)
DEFINE_GLOBAL(AtomicString, readystatechange)
DEFINE_GLOBAL(AtomicString, removesourcebuffer)
DEFINE_GLOBAL(AtomicString, removestream)
DEFINE_GLOBAL(AtomicString, removetrack)
DEFINE_GLOBAL(AtomicString, repeatEvent)
DEFINE_GLOBAL(AtomicString, reset)
DEFINE_GLOBAL(AtomicString, resize)
DEFINE_GLOBAL(AtomicString, result)
DEFINE_GLOBAL(AtomicString, resume)
DEFINE_GLOBAL(AtomicString, scroll)
DEFINE_GLOBAL(AtomicString, search)
DEFINE_GLOBAL(AtomicString, securitypolicyviolation)
DEFINE_GLOBAL(AtomicString, seeked)
DEFINE_GLOBAL(AtomicString, seeking)
DEFINE_GLOBAL(AtomicString, select)
DEFINE_GLOBAL(AtomicString, selectionchange)
DEFINE_GLOBAL(AtomicString, selectstart)
DEFINE_GLOBAL(AtomicString, show)
DEFINE_GLOBAL(AtomicString, signalingstatechange)
DEFINE_GLOBAL(AtomicString, soundend)
DEFINE_GLOBAL(AtomicString, soundstart)
DEFINE_GLOBAL(AtomicString, sourceclose)
DEFINE_GLOBAL(AtomicString, sourceended)
DEFINE_GLOBAL(AtomicString, sourceopen)
DEFINE_GLOBAL(AtomicString, speechend)
DEFINE_GLOBAL(AtomicString, speechstart)
DEFINE_GLOBAL(AtomicString, stalled)
DEFINE_GLOBAL(AtomicString, start)
DEFINE_GLOBAL(AtomicString, statechange)
DEFINE_GLOBAL(AtomicString, storage)
DEFINE_GLOBAL(AtomicString, submit)
DEFINE_GLOBAL(AtomicString, success)
DEFINE_GLOBAL(AtomicString, suspend)
DEFINE_GLOBAL(AtomicString, sync)
DEFINE_GLOBAL(AtomicString, textInput)
DEFINE_GLOBAL(AtomicString, timeout)
DEFINE_GLOBAL(AtomicString, timeupdate)
DEFINE_GLOBAL(AtomicString, toggle)
DEFINE_GLOBAL(AtomicString, tonechange)
DEFINE_GLOBAL(AtomicString, touchcancel)
DEFINE_GLOBAL(AtomicString, touchend)
DEFINE_GLOBAL(AtomicString, touchmove)
DEFINE_GLOBAL(AtomicString, touchstart)
DEFINE_GLOBAL(AtomicString, transitionend)
DEFINE_GLOBAL(AtomicString, typechange)
DEFINE_GLOBAL(AtomicString, unload)
DEFINE_GLOBAL(AtomicString, unmute)
DEFINE_GLOBAL(AtomicString, update)
DEFINE_GLOBAL(AtomicString, updateend)
DEFINE_GLOBAL(AtomicString, updateready)
DEFINE_GLOBAL(AtomicString, updatestart)
DEFINE_GLOBAL(AtomicString, upgradeneeded)
DEFINE_GLOBAL(AtomicString, versionchange)
DEFINE_GLOBAL(AtomicString, visibilitychange)
DEFINE_GLOBAL(AtomicString, voiceschanged)
DEFINE_GLOBAL(AtomicString, volumechange)
DEFINE_GLOBAL(AtomicString, waiting)
DEFINE_GLOBAL(AtomicString, webglcontextcreationerror)
DEFINE_GLOBAL(AtomicString, webglcontextlost)
DEFINE_GLOBAL(AtomicString, webglcontextrestored)
DEFINE_GLOBAL(AtomicString, webkitAnimationEnd)
DEFINE_GLOBAL(AtomicString, webkitAnimationIteration)
DEFINE_GLOBAL(AtomicString, webkitAnimationStart)
DEFINE_GLOBAL(AtomicString, webkitBeforeTextInserted)
DEFINE_GLOBAL(AtomicString, webkitEditableContentChanged)
DEFINE_GLOBAL(AtomicString, webkitTransitionEnd)
DEFINE_GLOBAL(AtomicString, webkitaddsourcebuffer)
DEFINE_GLOBAL(AtomicString, webkitdeviceproximity)
DEFINE_GLOBAL(AtomicString, webkitfullscreenchange)
DEFINE_GLOBAL(AtomicString, webkitfullscreenerror)
DEFINE_GLOBAL(AtomicString, webkitkeyadded)
DEFINE_GLOBAL(AtomicString, webkitkeyerror)
DEFINE_GLOBAL(AtomicString, webkitkeymessage)
DEFINE_GLOBAL(AtomicString, webkitneedkey)
DEFINE_GLOBAL(AtomicString, webkitnetworkinfochange)
DEFINE_GLOBAL(AtomicString, webkitpointerlockchange)
DEFINE_GLOBAL(AtomicString, webkitpointerlockerror)
DEFINE_GLOBAL(AtomicString, webkitprerenderdomcontentloaded)
DEFINE_GLOBAL(AtomicString, webkitprerenderload)
DEFINE_GLOBAL(AtomicString, webkitprerenderstart)
DEFINE_GLOBAL(AtomicString, webkitprerenderstop)
DEFINE_GLOBAL(AtomicString, webkitremovesourcebuffer)
DEFINE_GLOBAL(AtomicString, webkitresourcetimingbufferfull)
DEFINE_GLOBAL(AtomicString, webkitsourceclose)
DEFINE_GLOBAL(AtomicString, webkitsourceended)
DEFINE_GLOBAL(AtomicString, webkitsourceopen)
DEFINE_GLOBAL(AtomicString, webkitspeechchange)
DEFINE_GLOBAL(AtomicString, webkitvisibilitychange)
DEFINE_GLOBAL(AtomicString, wheel)
DEFINE_GLOBAL(AtomicString, write)
DEFINE_GLOBAL(AtomicString, writeend)
DEFINE_GLOBAL(AtomicString, writestart)
DEFINE_GLOBAL(AtomicString, zoom)

void init()
{
    StringImpl* DOMActivateImpl = StringImpl::createStatic("DOMActivate", 11, 1580932);
    StringImpl* DOMCharacterDataModifiedImpl = StringImpl::createStatic("DOMCharacterDataModified", 24, 1747391);
    StringImpl* DOMContentLoadedImpl = StringImpl::createStatic("DOMContentLoaded", 16, 10217403);
    StringImpl* DOMFocusInImpl = StringImpl::createStatic("DOMFocusIn", 10, 13717889);
    StringImpl* DOMFocusOutImpl = StringImpl::createStatic("DOMFocusOut", 11, 14725020);
    StringImpl* DOMNodeInsertedImpl = StringImpl::createStatic("DOMNodeInserted", 15, 3305523);
    StringImpl* DOMNodeInsertedIntoDocumentImpl = StringImpl::createStatic("DOMNodeInsertedIntoDocument", 27, 9765719);
    StringImpl* DOMNodeRemovedImpl = StringImpl::createStatic("DOMNodeRemoved", 14, 15338285);
    StringImpl* DOMNodeRemovedFromDocumentImpl = StringImpl::createStatic("DOMNodeRemovedFromDocument", 26, 16284219);
    StringImpl* DOMSubtreeModifiedImpl = StringImpl::createStatic("DOMSubtreeModified", 18, 6084203);
    StringImpl* abortImpl = StringImpl::createStatic("abort", 5, 15390287);
    StringImpl* activateImpl = StringImpl::createStatic("activate", 8, 8463533);
    StringImpl* addsourcebufferImpl = StringImpl::createStatic("addsourcebuffer", 15, 7452490);
    StringImpl* addstreamImpl = StringImpl::createStatic("addstream", 9, 15110493);
    StringImpl* addtrackImpl = StringImpl::createStatic("addtrack", 8, 3675244);
    StringImpl* animationendImpl = StringImpl::createStatic("animationend", 12, 2356336);
    StringImpl* animationiterationImpl = StringImpl::createStatic("animationiteration", 18, 4399371);
    StringImpl* animationstartImpl = StringImpl::createStatic("animationstart", 14, 1171773);
    StringImpl* audioendImpl = StringImpl::createStatic("audioend", 8, 4005439);
    StringImpl* audioprocessImpl = StringImpl::createStatic("audioprocess", 12, 7432340);
    StringImpl* audiostartImpl = StringImpl::createStatic("audiostart", 10, 12162534);
    StringImpl* autocompleteImpl = StringImpl::createStatic("autocomplete", 12, 14667434);
    StringImpl* autocompleteerrorImpl = StringImpl::createStatic("autocompleteerror", 17, 412664);
    StringImpl* beforecopyImpl = StringImpl::createStatic("beforecopy", 10, 15623218);
    StringImpl* beforecutImpl = StringImpl::createStatic("beforecut", 9, 6886552);
    StringImpl* beforepasteImpl = StringImpl::createStatic("beforepaste", 11, 5049674);
    StringImpl* beforeunloadImpl = StringImpl::createStatic("beforeunload", 12, 16009443);
    StringImpl* beginEventImpl = StringImpl::createStatic("beginEvent", 10, 3946429);
    StringImpl* blockedImpl = StringImpl::createStatic("blocked", 7, 7188319);
    StringImpl* blurImpl = StringImpl::createStatic("blur", 4, 3880931);
    StringImpl* boundaryImpl = StringImpl::createStatic("boundary", 8, 8113195);
    StringImpl* cachedImpl = StringImpl::createStatic("cached", 6, 2984957);
    StringImpl* cancelImpl = StringImpl::createStatic("cancel", 6, 9877073);
    StringImpl* candidatewindowhideImpl = StringImpl::createStatic("candidatewindowhide", 19, 3029572);
    StringImpl* candidatewindowshowImpl = StringImpl::createStatic("candidatewindowshow", 19, 11030621);
    StringImpl* candidatewindowupdateImpl = StringImpl::createStatic("candidatewindowupdate", 21, 6897811);
    StringImpl* canplayImpl = StringImpl::createStatic("canplay", 7, 10609174);
    StringImpl* canplaythroughImpl = StringImpl::createStatic("canplaythrough", 14, 3645779);
    StringImpl* changeImpl = StringImpl::createStatic("change", 6, 3980998);
    StringImpl* chargingchangeImpl = StringImpl::createStatic("chargingchange", 14, 12089746);
    StringImpl* chargingtimechangeImpl = StringImpl::createStatic("chargingtimechange", 18, 2233255);
    StringImpl* checkingImpl = StringImpl::createStatic("checking", 8, 12242986);
    StringImpl* clickImpl = StringImpl::createStatic("click", 5, 10125525);
    StringImpl* closeImpl = StringImpl::createStatic("close", 5, 3222970);
    StringImpl* completeImpl = StringImpl::createStatic("complete", 8, 6079763);
    StringImpl* compositionendImpl = StringImpl::createStatic("compositionend", 14, 5906550);
    StringImpl* compositionstartImpl = StringImpl::createStatic("compositionstart", 16, 16133899);
    StringImpl* compositionupdateImpl = StringImpl::createStatic("compositionupdate", 17, 7385090);
    StringImpl* connectImpl = StringImpl::createStatic("connect", 7, 12649850);
    StringImpl* connectingImpl = StringImpl::createStatic("connecting", 10, 1432354);
    StringImpl* contextlostImpl = StringImpl::createStatic("contextlost", 11, 9876436);
    StringImpl* contextmenuImpl = StringImpl::createStatic("contextmenu", 11, 14578063);
    StringImpl* contextrestoredImpl = StringImpl::createStatic("contextrestored", 15, 81548);
    StringImpl* copyImpl = StringImpl::createStatic("copy", 4, 16438426);
    StringImpl* cuechangeImpl = StringImpl::createStatic("cuechange", 9, 3496656);
    StringImpl* cutImpl = StringImpl::createStatic("cut", 3, 6810636);
    StringImpl* datachannelImpl = StringImpl::createStatic("datachannel", 11, 3404486);
    StringImpl* dblclickImpl = StringImpl::createStatic("dblclick", 8, 9054393);
    StringImpl* devicelightImpl = StringImpl::createStatic("devicelight", 11, 9022337);
    StringImpl* devicemotionImpl = StringImpl::createStatic("devicemotion", 12, 1024162);
    StringImpl* deviceorientationImpl = StringImpl::createStatic("deviceorientation", 17, 12165871);
    StringImpl* dischargingtimechangeImpl = StringImpl::createStatic("dischargingtimechange", 21, 16138730);
    StringImpl* disconnectImpl = StringImpl::createStatic("disconnect", 10, 13405069);
    StringImpl* displayImpl = StringImpl::createStatic("display", 7, 16245385);
    StringImpl* downloadingImpl = StringImpl::createStatic("downloading", 11, 576780);
    StringImpl* dragImpl = StringImpl::createStatic("drag", 4, 1532976);
    StringImpl* dragendImpl = StringImpl::createStatic("dragend", 7, 11982793);
    StringImpl* dragenterImpl = StringImpl::createStatic("dragenter", 9, 10351130);
    StringImpl* dragleaveImpl = StringImpl::createStatic("dragleave", 9, 11297414);
    StringImpl* dragoverImpl = StringImpl::createStatic("dragover", 8, 2973271);
    StringImpl* dragstartImpl = StringImpl::createStatic("dragstart", 9, 15174021);
    StringImpl* dropImpl = StringImpl::createStatic("drop", 4, 13413407);
    StringImpl* durationchangeImpl = StringImpl::createStatic("durationchange", 14, 4541505);
    StringImpl* emptiedImpl = StringImpl::createStatic("emptied", 7, 15126627);
    StringImpl* endImpl = StringImpl::createStatic("end", 3, 1590106);
    StringImpl* endEventImpl = StringImpl::createStatic("endEvent", 8, 10314310);
    StringImpl* endedImpl = StringImpl::createStatic("ended", 5, 54732);
    StringImpl* enterImpl = StringImpl::createStatic("enter", 5, 3509546);
    StringImpl* errorImpl = StringImpl::createStatic("error", 5, 6654137);
    StringImpl* exitImpl = StringImpl::createStatic("exit", 4, 15251726);
    StringImpl* fetchImpl = StringImpl::createStatic("fetch", 5, 10988466);
    StringImpl* finishImpl = StringImpl::createStatic("finish", 6, 15149124);
    StringImpl* focusImpl = StringImpl::createStatic("focus", 5, 1849182);
    StringImpl* focusinImpl = StringImpl::createStatic("focusin", 7, 13489365);
    StringImpl* focusoutImpl = StringImpl::createStatic("focusout", 8, 13124807);
    StringImpl* gamepadconnectedImpl = StringImpl::createStatic("gamepadconnected", 16, 3715619);
    StringImpl* gamepaddisconnectedImpl = StringImpl::createStatic("gamepaddisconnected", 19, 3966662);
    StringImpl* gesturescrollendImpl = StringImpl::createStatic("gesturescrollend", 16, 5471896);
    StringImpl* gesturescrollstartImpl = StringImpl::createStatic("gesturescrollstart", 18, 16516609);
    StringImpl* gesturescrollupdateImpl = StringImpl::createStatic("gesturescrollupdate", 19, 11683597);
    StringImpl* gestureshowpressImpl = StringImpl::createStatic("gestureshowpress", 16, 111807);
    StringImpl* gesturetapImpl = StringImpl::createStatic("gesturetap", 10, 1938166);
    StringImpl* gesturetapdownImpl = StringImpl::createStatic("gesturetapdown", 14, 8907849);
    StringImpl* gesturetapunconfirmedImpl = StringImpl::createStatic("gesturetapunconfirmed", 21, 793435);
    StringImpl* hashchangeImpl = StringImpl::createStatic("hashchange", 10, 3959696);
    StringImpl* icecandidateImpl = StringImpl::createStatic("icecandidate", 12, 16268761);
    StringImpl* iceconnectionstatechangeImpl = StringImpl::createStatic("iceconnectionstatechange", 24, 9120814);
    StringImpl* inputImpl = StringImpl::createStatic("input", 5, 10365436);
    StringImpl* installImpl = StringImpl::createStatic("install", 7, 7743126);
    StringImpl* invalidImpl = StringImpl::createStatic("invalid", 7, 2059736);
    StringImpl* keydownImpl = StringImpl::createStatic("keydown", 7, 5301006);
    StringImpl* keypressImpl = StringImpl::createStatic("keypress", 8, 13845508);
    StringImpl* keyupImpl = StringImpl::createStatic("keyup", 5, 1054547);
    StringImpl* languagechangeImpl = StringImpl::createStatic("languagechange", 14, 3401938);
    StringImpl* levelchangeImpl = StringImpl::createStatic("levelchange", 11, 4445458);
    StringImpl* loadImpl = StringImpl::createStatic("load", 4, 8207817);
    StringImpl* loadeddataImpl = StringImpl::createStatic("loadeddata", 10, 13874190);
    StringImpl* loadedmetadataImpl = StringImpl::createStatic("loadedmetadata", 14, 9550378);
    StringImpl* loadendImpl = StringImpl::createStatic("loadend", 7, 15911784);
    StringImpl* loadingImpl = StringImpl::createStatic("loading", 7, 13228638);
    StringImpl* loadingdoneImpl = StringImpl::createStatic("loadingdone", 11, 6190750);
    StringImpl* loadingerrorImpl = StringImpl::createStatic("loadingerror", 12, 5585167);
    StringImpl* loadstartImpl = StringImpl::createStatic("loadstart", 9, 5495169);
    StringImpl* markImpl = StringImpl::createStatic("mark", 4, 16508213);
    StringImpl* messageImpl = StringImpl::createStatic("message", 7, 11017471);
    StringImpl* midimessageImpl = StringImpl::createStatic("midimessage", 11, 3928734);
    StringImpl* mousedownImpl = StringImpl::createStatic("mousedown", 9, 5740218);
    StringImpl* mouseenterImpl = StringImpl::createStatic("mouseenter", 10, 632853);
    StringImpl* mouseleaveImpl = StringImpl::createStatic("mouseleave", 10, 1144826);
    StringImpl* mousemoveImpl = StringImpl::createStatic("mousemove", 9, 7356044);
    StringImpl* mouseoutImpl = StringImpl::createStatic("mouseout", 8, 685204);
    StringImpl* mouseoverImpl = StringImpl::createStatic("mouseover", 9, 12064397);
    StringImpl* mouseupImpl = StringImpl::createStatic("mouseup", 7, 8467273);
    StringImpl* mousewheelImpl = StringImpl::createStatic("mousewheel", 10, 15891108);
    StringImpl* muteImpl = StringImpl::createStatic("mute", 4, 15335886);
    StringImpl* needkeyImpl = StringImpl::createStatic("needkey", 7, 9610812);
    StringImpl* negotiationneededImpl = StringImpl::createStatic("negotiationneeded", 17, 7270370);
    StringImpl* nomatchImpl = StringImpl::createStatic("nomatch", 7, 8258898);
    StringImpl* noupdateImpl = StringImpl::createStatic("noupdate", 8, 8885664);
    StringImpl* obsoleteImpl = StringImpl::createStatic("obsolete", 8, 12364910);
    StringImpl* offlineImpl = StringImpl::createStatic("offline", 7, 8847130);
    StringImpl* onlineImpl = StringImpl::createStatic("online", 6, 9492271);
    StringImpl* openImpl = StringImpl::createStatic("open", 4, 13703631);
    StringImpl* orientationchangeImpl = StringImpl::createStatic("orientationchange", 17, 14664026);
    StringImpl* overflowchangedImpl = StringImpl::createStatic("overflowchanged", 15, 9695864);
    StringImpl* pagehideImpl = StringImpl::createStatic("pagehide", 8, 16640617);
    StringImpl* pageshowImpl = StringImpl::createStatic("pageshow", 8, 12320166);
    StringImpl* pasteImpl = StringImpl::createStatic("paste", 5, 369957);
    StringImpl* pauseImpl = StringImpl::createStatic("pause", 5, 5413740);
    StringImpl* playImpl = StringImpl::createStatic("play", 4, 10707022);
    StringImpl* playingImpl = StringImpl::createStatic("playing", 7, 12380358);
    StringImpl* pointerlockchangeImpl = StringImpl::createStatic("pointerlockchange", 17, 7047632);
    StringImpl* pointerlockerrorImpl = StringImpl::createStatic("pointerlockerror", 16, 16321958);
    StringImpl* popstateImpl = StringImpl::createStatic("popstate", 8, 12585208);
    StringImpl* progressImpl = StringImpl::createStatic("progress", 8, 11461517);
    StringImpl* pushImpl = StringImpl::createStatic("push", 4, 4758933);
    StringImpl* ratechangeImpl = StringImpl::createStatic("ratechange", 10, 9044681);
    StringImpl* readyImpl = StringImpl::createStatic("ready", 5, 12275238);
    StringImpl* readystatechangeImpl = StringImpl::createStatic("readystatechange", 16, 11011948);
    StringImpl* removesourcebufferImpl = StringImpl::createStatic("removesourcebuffer", 18, 13594278);
    StringImpl* removestreamImpl = StringImpl::createStatic("removestream", 12, 4604809);
    StringImpl* removetrackImpl = StringImpl::createStatic("removetrack", 11, 6131613);
    StringImpl* repeatEventImpl = StringImpl::createStatic("repeatEvent", 11, 7182823);
    StringImpl* resetImpl = StringImpl::createStatic("reset", 5, 13674204);
    StringImpl* resizeImpl = StringImpl::createStatic("resize", 6, 11716975);
    StringImpl* resultImpl = StringImpl::createStatic("result", 6, 15954886);
    StringImpl* resumeImpl = StringImpl::createStatic("resume", 6, 3214184);
    StringImpl* scrollImpl = StringImpl::createStatic("scroll", 6, 7626286);
    StringImpl* searchImpl = StringImpl::createStatic("search", 6, 6906057);
    StringImpl* securitypolicyviolationImpl = StringImpl::createStatic("securitypolicyviolation", 23, 7930261);
    StringImpl* seekedImpl = StringImpl::createStatic("seeked", 6, 16049377);
    StringImpl* seekingImpl = StringImpl::createStatic("seeking", 7, 10757715);
    StringImpl* selectImpl = StringImpl::createStatic("select", 6, 210571);
    StringImpl* selectionchangeImpl = StringImpl::createStatic("selectionchange", 15, 8861459);
    StringImpl* selectstartImpl = StringImpl::createStatic("selectstart", 11, 5847864);
    StringImpl* showImpl = StringImpl::createStatic("show", 4, 3191658);
    StringImpl* signalingstatechangeImpl = StringImpl::createStatic("signalingstatechange", 20, 14107818);
    StringImpl* soundendImpl = StringImpl::createStatic("soundend", 8, 5959322);
    StringImpl* soundstartImpl = StringImpl::createStatic("soundstart", 10, 14237399);
    StringImpl* sourcecloseImpl = StringImpl::createStatic("sourceclose", 11, 10570717);
    StringImpl* sourceendedImpl = StringImpl::createStatic("sourceended", 11, 16675086);
    StringImpl* sourceopenImpl = StringImpl::createStatic("sourceopen", 10, 7526202);
    StringImpl* speechendImpl = StringImpl::createStatic("speechend", 9, 15574659);
    StringImpl* speechstartImpl = StringImpl::createStatic("speechstart", 11, 10183406);
    StringImpl* stalledImpl = StringImpl::createStatic("stalled", 7, 4001267);
    StringImpl* startImpl = StringImpl::createStatic("start", 5, 1021290);
    StringImpl* statechangeImpl = StringImpl::createStatic("statechange", 11, 9232255);
    StringImpl* storageImpl = StringImpl::createStatic("storage", 7, 3034291);
    StringImpl* submitImpl = StringImpl::createStatic("submit", 6, 12328646);
    StringImpl* successImpl = StringImpl::createStatic("success", 7, 15544773);
    StringImpl* suspendImpl = StringImpl::createStatic("suspend", 7, 2237992);
    StringImpl* syncImpl = StringImpl::createStatic("sync", 4, 14643608);
    StringImpl* textInputImpl = StringImpl::createStatic("textInput", 9, 12906447);
    StringImpl* timeoutImpl = StringImpl::createStatic("timeout", 7, 5983938);
    StringImpl* timeupdateImpl = StringImpl::createStatic("timeupdate", 10, 10406103);
    StringImpl* toggleImpl = StringImpl::createStatic("toggle", 6, 8404466);
    StringImpl* tonechangeImpl = StringImpl::createStatic("tonechange", 10, 3007488);
    StringImpl* touchcancelImpl = StringImpl::createStatic("touchcancel", 11, 5916858);
    StringImpl* touchendImpl = StringImpl::createStatic("touchend", 8, 2956176);
    StringImpl* touchmoveImpl = StringImpl::createStatic("touchmove", 9, 9499787);
    StringImpl* touchstartImpl = StringImpl::createStatic("touchstart", 10, 13231026);
    StringImpl* transitionendImpl = StringImpl::createStatic("transitionend", 13, 9031405);
    StringImpl* typechangeImpl = StringImpl::createStatic("typechange", 10, 5883421);
    StringImpl* unloadImpl = StringImpl::createStatic("unload", 6, 4411490);
    StringImpl* unmuteImpl = StringImpl::createStatic("unmute", 6, 1582770);
    StringImpl* updateImpl = StringImpl::createStatic("update", 6, 5546488);
    StringImpl* updateendImpl = StringImpl::createStatic("updateend", 9, 13034123);
    StringImpl* updatereadyImpl = StringImpl::createStatic("updateready", 11, 3196224);
    StringImpl* updatestartImpl = StringImpl::createStatic("updatestart", 11, 4286012);
    StringImpl* upgradeneededImpl = StringImpl::createStatic("upgradeneeded", 13, 492799);
    StringImpl* versionchangeImpl = StringImpl::createStatic("versionchange", 13, 1334380);
    StringImpl* visibilitychangeImpl = StringImpl::createStatic("visibilitychange", 16, 11206348);
    StringImpl* voiceschangedImpl = StringImpl::createStatic("voiceschanged", 13, 3637255);
    StringImpl* volumechangeImpl = StringImpl::createStatic("volumechange", 12, 16288075);
    StringImpl* waitingImpl = StringImpl::createStatic("waiting", 7, 10489636);
    StringImpl* webglcontextcreationerrorImpl = StringImpl::createStatic("webglcontextcreationerror", 25, 2718866);
    StringImpl* webglcontextlostImpl = StringImpl::createStatic("webglcontextlost", 16, 16605358);
    StringImpl* webglcontextrestoredImpl = StringImpl::createStatic("webglcontextrestored", 20, 8624464);
    StringImpl* webkitAnimationEndImpl = StringImpl::createStatic("webkitAnimationEnd", 18, 11845628);
    StringImpl* webkitAnimationIterationImpl = StringImpl::createStatic("webkitAnimationIteration", 24, 9040977);
    StringImpl* webkitAnimationStartImpl = StringImpl::createStatic("webkitAnimationStart", 20, 4844440);
    StringImpl* webkitBeforeTextInsertedImpl = StringImpl::createStatic("webkitBeforeTextInserted", 24, 5393700);
    StringImpl* webkitEditableContentChangedImpl = StringImpl::createStatic("webkitEditableContentChanged", 28, 1212374);
    StringImpl* webkitTransitionEndImpl = StringImpl::createStatic("webkitTransitionEnd", 19, 16021356);
    StringImpl* webkitaddsourcebufferImpl = StringImpl::createStatic("webkitaddsourcebuffer", 21, 3799888);
    StringImpl* webkitdeviceproximityImpl = StringImpl::createStatic("webkitdeviceproximity", 21, 13182349);
    StringImpl* webkitfullscreenchangeImpl = StringImpl::createStatic("webkitfullscreenchange", 22, 5579857);
    StringImpl* webkitfullscreenerrorImpl = StringImpl::createStatic("webkitfullscreenerror", 21, 12520378);
    StringImpl* webkitkeyaddedImpl = StringImpl::createStatic("webkitkeyadded", 14, 3238929);
    StringImpl* webkitkeyerrorImpl = StringImpl::createStatic("webkitkeyerror", 14, 1430383);
    StringImpl* webkitkeymessageImpl = StringImpl::createStatic("webkitkeymessage", 16, 61860);
    StringImpl* webkitneedkeyImpl = StringImpl::createStatic("webkitneedkey", 13, 7858771);
    StringImpl* webkitnetworkinfochangeImpl = StringImpl::createStatic("webkitnetworkinfochange", 23, 4917586);
    StringImpl* webkitpointerlockchangeImpl = StringImpl::createStatic("webkitpointerlockchange", 23, 37170);
    StringImpl* webkitpointerlockerrorImpl = StringImpl::createStatic("webkitpointerlockerror", 22, 4907338);
    StringImpl* webkitprerenderdomcontentloadedImpl = StringImpl::createStatic("webkitprerenderdomcontentloaded", 31, 14089645);
    StringImpl* webkitprerenderloadImpl = StringImpl::createStatic("webkitprerenderload", 19, 15469365);
    StringImpl* webkitprerenderstartImpl = StringImpl::createStatic("webkitprerenderstart", 20, 4709816);
    StringImpl* webkitprerenderstopImpl = StringImpl::createStatic("webkitprerenderstop", 19, 7661225);
    StringImpl* webkitremovesourcebufferImpl = StringImpl::createStatic("webkitremovesourcebuffer", 24, 15764645);
    StringImpl* webkitresourcetimingbufferfullImpl = StringImpl::createStatic("webkitresourcetimingbufferfull", 30, 14547041);
    StringImpl* webkitsourcecloseImpl = StringImpl::createStatic("webkitsourceclose", 17, 11711321);
    StringImpl* webkitsourceendedImpl = StringImpl::createStatic("webkitsourceended", 17, 13059969);
    StringImpl* webkitsourceopenImpl = StringImpl::createStatic("webkitsourceopen", 16, 9562429);
    StringImpl* webkitspeechchangeImpl = StringImpl::createStatic("webkitspeechchange", 18, 11755315);
    StringImpl* webkitvisibilitychangeImpl = StringImpl::createStatic("webkitvisibilitychange", 22, 440449);
    StringImpl* wheelImpl = StringImpl::createStatic("wheel", 5, 5389519);
    StringImpl* writeImpl = StringImpl::createStatic("write", 5, 10020319);
    StringImpl* writeendImpl = StringImpl::createStatic("writeend", 8, 6300360);
    StringImpl* writestartImpl = StringImpl::createStatic("writestart", 10, 2198394);
    StringImpl* zoomImpl = StringImpl::createStatic("zoom", 4, 6082914);

    new ((void*)&DOMActivate) AtomicString(DOMActivateImpl);
    new ((void*)&DOMCharacterDataModified) AtomicString(DOMCharacterDataModifiedImpl);
    new ((void*)&DOMContentLoaded) AtomicString(DOMContentLoadedImpl);
    new ((void*)&DOMFocusIn) AtomicString(DOMFocusInImpl);
    new ((void*)&DOMFocusOut) AtomicString(DOMFocusOutImpl);
    new ((void*)&DOMNodeInserted) AtomicString(DOMNodeInsertedImpl);
    new ((void*)&DOMNodeInsertedIntoDocument) AtomicString(DOMNodeInsertedIntoDocumentImpl);
    new ((void*)&DOMNodeRemoved) AtomicString(DOMNodeRemovedImpl);
    new ((void*)&DOMNodeRemovedFromDocument) AtomicString(DOMNodeRemovedFromDocumentImpl);
    new ((void*)&DOMSubtreeModified) AtomicString(DOMSubtreeModifiedImpl);
    new ((void*)&abort) AtomicString(abortImpl);
    new ((void*)&activate) AtomicString(activateImpl);
    new ((void*)&addsourcebuffer) AtomicString(addsourcebufferImpl);
    new ((void*)&addstream) AtomicString(addstreamImpl);
    new ((void*)&addtrack) AtomicString(addtrackImpl);
    new ((void*)&animationend) AtomicString(animationendImpl);
    new ((void*)&animationiteration) AtomicString(animationiterationImpl);
    new ((void*)&animationstart) AtomicString(animationstartImpl);
    new ((void*)&audioend) AtomicString(audioendImpl);
    new ((void*)&audioprocess) AtomicString(audioprocessImpl);
    new ((void*)&audiostart) AtomicString(audiostartImpl);
    new ((void*)&autocomplete) AtomicString(autocompleteImpl);
    new ((void*)&autocompleteerror) AtomicString(autocompleteerrorImpl);
    new ((void*)&beforecopy) AtomicString(beforecopyImpl);
    new ((void*)&beforecut) AtomicString(beforecutImpl);
    new ((void*)&beforepaste) AtomicString(beforepasteImpl);
    new ((void*)&beforeunload) AtomicString(beforeunloadImpl);
    new ((void*)&beginEvent) AtomicString(beginEventImpl);
    new ((void*)&blocked) AtomicString(blockedImpl);
    new ((void*)&blur) AtomicString(blurImpl);
    new ((void*)&boundary) AtomicString(boundaryImpl);
    new ((void*)&cached) AtomicString(cachedImpl);
    new ((void*)&cancel) AtomicString(cancelImpl);
    new ((void*)&candidatewindowhide) AtomicString(candidatewindowhideImpl);
    new ((void*)&candidatewindowshow) AtomicString(candidatewindowshowImpl);
    new ((void*)&candidatewindowupdate) AtomicString(candidatewindowupdateImpl);
    new ((void*)&canplay) AtomicString(canplayImpl);
    new ((void*)&canplaythrough) AtomicString(canplaythroughImpl);
    new ((void*)&change) AtomicString(changeImpl);
    new ((void*)&chargingchange) AtomicString(chargingchangeImpl);
    new ((void*)&chargingtimechange) AtomicString(chargingtimechangeImpl);
    new ((void*)&checking) AtomicString(checkingImpl);
    new ((void*)&click) AtomicString(clickImpl);
    new ((void*)&close) AtomicString(closeImpl);
    new ((void*)&complete) AtomicString(completeImpl);
    new ((void*)&compositionend) AtomicString(compositionendImpl);
    new ((void*)&compositionstart) AtomicString(compositionstartImpl);
    new ((void*)&compositionupdate) AtomicString(compositionupdateImpl);
    new ((void*)&connect) AtomicString(connectImpl);
    new ((void*)&connecting) AtomicString(connectingImpl);
    new ((void*)&contextlost) AtomicString(contextlostImpl);
    new ((void*)&contextmenu) AtomicString(contextmenuImpl);
    new ((void*)&contextrestored) AtomicString(contextrestoredImpl);
    new ((void*)&copy) AtomicString(copyImpl);
    new ((void*)&cuechange) AtomicString(cuechangeImpl);
    new ((void*)&cut) AtomicString(cutImpl);
    new ((void*)&datachannel) AtomicString(datachannelImpl);
    new ((void*)&dblclick) AtomicString(dblclickImpl);
    new ((void*)&devicelight) AtomicString(devicelightImpl);
    new ((void*)&devicemotion) AtomicString(devicemotionImpl);
    new ((void*)&deviceorientation) AtomicString(deviceorientationImpl);
    new ((void*)&dischargingtimechange) AtomicString(dischargingtimechangeImpl);
    new ((void*)&disconnect) AtomicString(disconnectImpl);
    new ((void*)&display) AtomicString(displayImpl);
    new ((void*)&downloading) AtomicString(downloadingImpl);
    new ((void*)&drag) AtomicString(dragImpl);
    new ((void*)&dragend) AtomicString(dragendImpl);
    new ((void*)&dragenter) AtomicString(dragenterImpl);
    new ((void*)&dragleave) AtomicString(dragleaveImpl);
    new ((void*)&dragover) AtomicString(dragoverImpl);
    new ((void*)&dragstart) AtomicString(dragstartImpl);
    new ((void*)&drop) AtomicString(dropImpl);
    new ((void*)&durationchange) AtomicString(durationchangeImpl);
    new ((void*)&emptied) AtomicString(emptiedImpl);
    new ((void*)&end) AtomicString(endImpl);
    new ((void*)&endEvent) AtomicString(endEventImpl);
    new ((void*)&ended) AtomicString(endedImpl);
    new ((void*)&enter) AtomicString(enterImpl);
    new ((void*)&error) AtomicString(errorImpl);
    new ((void*)&exit) AtomicString(exitImpl);
    new ((void*)&fetch) AtomicString(fetchImpl);
    new ((void*)&finish) AtomicString(finishImpl);
    new ((void*)&focus) AtomicString(focusImpl);
    new ((void*)&focusin) AtomicString(focusinImpl);
    new ((void*)&focusout) AtomicString(focusoutImpl);
    new ((void*)&gamepadconnected) AtomicString(gamepadconnectedImpl);
    new ((void*)&gamepaddisconnected) AtomicString(gamepaddisconnectedImpl);
    new ((void*)&gesturescrollend) AtomicString(gesturescrollendImpl);
    new ((void*)&gesturescrollstart) AtomicString(gesturescrollstartImpl);
    new ((void*)&gesturescrollupdate) AtomicString(gesturescrollupdateImpl);
    new ((void*)&gestureshowpress) AtomicString(gestureshowpressImpl);
    new ((void*)&gesturetap) AtomicString(gesturetapImpl);
    new ((void*)&gesturetapdown) AtomicString(gesturetapdownImpl);
    new ((void*)&gesturetapunconfirmed) AtomicString(gesturetapunconfirmedImpl);
    new ((void*)&hashchange) AtomicString(hashchangeImpl);
    new ((void*)&icecandidate) AtomicString(icecandidateImpl);
    new ((void*)&iceconnectionstatechange) AtomicString(iceconnectionstatechangeImpl);
    new ((void*)&input) AtomicString(inputImpl);
    new ((void*)&install) AtomicString(installImpl);
    new ((void*)&invalid) AtomicString(invalidImpl);
    new ((void*)&keydown) AtomicString(keydownImpl);
    new ((void*)&keypress) AtomicString(keypressImpl);
    new ((void*)&keyup) AtomicString(keyupImpl);
    new ((void*)&languagechange) AtomicString(languagechangeImpl);
    new ((void*)&levelchange) AtomicString(levelchangeImpl);
    new ((void*)&load) AtomicString(loadImpl);
    new ((void*)&loadeddata) AtomicString(loadeddataImpl);
    new ((void*)&loadedmetadata) AtomicString(loadedmetadataImpl);
    new ((void*)&loadend) AtomicString(loadendImpl);
    new ((void*)&loading) AtomicString(loadingImpl);
    new ((void*)&loadingdone) AtomicString(loadingdoneImpl);
    new ((void*)&loadingerror) AtomicString(loadingerrorImpl);
    new ((void*)&loadstart) AtomicString(loadstartImpl);
    new ((void*)&mark) AtomicString(markImpl);
    new ((void*)&message) AtomicString(messageImpl);
    new ((void*)&midimessage) AtomicString(midimessageImpl);
    new ((void*)&mousedown) AtomicString(mousedownImpl);
    new ((void*)&mouseenter) AtomicString(mouseenterImpl);
    new ((void*)&mouseleave) AtomicString(mouseleaveImpl);
    new ((void*)&mousemove) AtomicString(mousemoveImpl);
    new ((void*)&mouseout) AtomicString(mouseoutImpl);
    new ((void*)&mouseover) AtomicString(mouseoverImpl);
    new ((void*)&mouseup) AtomicString(mouseupImpl);
    new ((void*)&mousewheel) AtomicString(mousewheelImpl);
    new ((void*)&mute) AtomicString(muteImpl);
    new ((void*)&needkey) AtomicString(needkeyImpl);
    new ((void*)&negotiationneeded) AtomicString(negotiationneededImpl);
    new ((void*)&nomatch) AtomicString(nomatchImpl);
    new ((void*)&noupdate) AtomicString(noupdateImpl);
    new ((void*)&obsolete) AtomicString(obsoleteImpl);
    new ((void*)&offline) AtomicString(offlineImpl);
    new ((void*)&online) AtomicString(onlineImpl);
    new ((void*)&open) AtomicString(openImpl);
    new ((void*)&orientationchange) AtomicString(orientationchangeImpl);
    new ((void*)&overflowchanged) AtomicString(overflowchangedImpl);
    new ((void*)&pagehide) AtomicString(pagehideImpl);
    new ((void*)&pageshow) AtomicString(pageshowImpl);
    new ((void*)&paste) AtomicString(pasteImpl);
    new ((void*)&pause) AtomicString(pauseImpl);
    new ((void*)&play) AtomicString(playImpl);
    new ((void*)&playing) AtomicString(playingImpl);
    new ((void*)&pointerlockchange) AtomicString(pointerlockchangeImpl);
    new ((void*)&pointerlockerror) AtomicString(pointerlockerrorImpl);
    new ((void*)&popstate) AtomicString(popstateImpl);
    new ((void*)&progress) AtomicString(progressImpl);
    new ((void*)&push) AtomicString(pushImpl);
    new ((void*)&ratechange) AtomicString(ratechangeImpl);
    new ((void*)&ready) AtomicString(readyImpl);
    new ((void*)&readystatechange) AtomicString(readystatechangeImpl);
    new ((void*)&removesourcebuffer) AtomicString(removesourcebufferImpl);
    new ((void*)&removestream) AtomicString(removestreamImpl);
    new ((void*)&removetrack) AtomicString(removetrackImpl);
    new ((void*)&repeatEvent) AtomicString(repeatEventImpl);
    new ((void*)&reset) AtomicString(resetImpl);
    new ((void*)&resize) AtomicString(resizeImpl);
    new ((void*)&result) AtomicString(resultImpl);
    new ((void*)&resume) AtomicString(resumeImpl);
    new ((void*)&scroll) AtomicString(scrollImpl);
    new ((void*)&search) AtomicString(searchImpl);
    new ((void*)&securitypolicyviolation) AtomicString(securitypolicyviolationImpl);
    new ((void*)&seeked) AtomicString(seekedImpl);
    new ((void*)&seeking) AtomicString(seekingImpl);
    new ((void*)&select) AtomicString(selectImpl);
    new ((void*)&selectionchange) AtomicString(selectionchangeImpl);
    new ((void*)&selectstart) AtomicString(selectstartImpl);
    new ((void*)&show) AtomicString(showImpl);
    new ((void*)&signalingstatechange) AtomicString(signalingstatechangeImpl);
    new ((void*)&soundend) AtomicString(soundendImpl);
    new ((void*)&soundstart) AtomicString(soundstartImpl);
    new ((void*)&sourceclose) AtomicString(sourcecloseImpl);
    new ((void*)&sourceended) AtomicString(sourceendedImpl);
    new ((void*)&sourceopen) AtomicString(sourceopenImpl);
    new ((void*)&speechend) AtomicString(speechendImpl);
    new ((void*)&speechstart) AtomicString(speechstartImpl);
    new ((void*)&stalled) AtomicString(stalledImpl);
    new ((void*)&start) AtomicString(startImpl);
    new ((void*)&statechange) AtomicString(statechangeImpl);
    new ((void*)&storage) AtomicString(storageImpl);
    new ((void*)&submit) AtomicString(submitImpl);
    new ((void*)&success) AtomicString(successImpl);
    new ((void*)&suspend) AtomicString(suspendImpl);
    new ((void*)&sync) AtomicString(syncImpl);
    new ((void*)&textInput) AtomicString(textInputImpl);
    new ((void*)&timeout) AtomicString(timeoutImpl);
    new ((void*)&timeupdate) AtomicString(timeupdateImpl);
    new ((void*)&toggle) AtomicString(toggleImpl);
    new ((void*)&tonechange) AtomicString(tonechangeImpl);
    new ((void*)&touchcancel) AtomicString(touchcancelImpl);
    new ((void*)&touchend) AtomicString(touchendImpl);
    new ((void*)&touchmove) AtomicString(touchmoveImpl);
    new ((void*)&touchstart) AtomicString(touchstartImpl);
    new ((void*)&transitionend) AtomicString(transitionendImpl);
    new ((void*)&typechange) AtomicString(typechangeImpl);
    new ((void*)&unload) AtomicString(unloadImpl);
    new ((void*)&unmute) AtomicString(unmuteImpl);
    new ((void*)&update) AtomicString(updateImpl);
    new ((void*)&updateend) AtomicString(updateendImpl);
    new ((void*)&updateready) AtomicString(updatereadyImpl);
    new ((void*)&updatestart) AtomicString(updatestartImpl);
    new ((void*)&upgradeneeded) AtomicString(upgradeneededImpl);
    new ((void*)&versionchange) AtomicString(versionchangeImpl);
    new ((void*)&visibilitychange) AtomicString(visibilitychangeImpl);
    new ((void*)&voiceschanged) AtomicString(voiceschangedImpl);
    new ((void*)&volumechange) AtomicString(volumechangeImpl);
    new ((void*)&waiting) AtomicString(waitingImpl);
    new ((void*)&webglcontextcreationerror) AtomicString(webglcontextcreationerrorImpl);
    new ((void*)&webglcontextlost) AtomicString(webglcontextlostImpl);
    new ((void*)&webglcontextrestored) AtomicString(webglcontextrestoredImpl);
    new ((void*)&webkitAnimationEnd) AtomicString(webkitAnimationEndImpl);
    new ((void*)&webkitAnimationIteration) AtomicString(webkitAnimationIterationImpl);
    new ((void*)&webkitAnimationStart) AtomicString(webkitAnimationStartImpl);
    new ((void*)&webkitBeforeTextInserted) AtomicString(webkitBeforeTextInsertedImpl);
    new ((void*)&webkitEditableContentChanged) AtomicString(webkitEditableContentChangedImpl);
    new ((void*)&webkitTransitionEnd) AtomicString(webkitTransitionEndImpl);
    new ((void*)&webkitaddsourcebuffer) AtomicString(webkitaddsourcebufferImpl);
    new ((void*)&webkitdeviceproximity) AtomicString(webkitdeviceproximityImpl);
    new ((void*)&webkitfullscreenchange) AtomicString(webkitfullscreenchangeImpl);
    new ((void*)&webkitfullscreenerror) AtomicString(webkitfullscreenerrorImpl);
    new ((void*)&webkitkeyadded) AtomicString(webkitkeyaddedImpl);
    new ((void*)&webkitkeyerror) AtomicString(webkitkeyerrorImpl);
    new ((void*)&webkitkeymessage) AtomicString(webkitkeymessageImpl);
    new ((void*)&webkitneedkey) AtomicString(webkitneedkeyImpl);
    new ((void*)&webkitnetworkinfochange) AtomicString(webkitnetworkinfochangeImpl);
    new ((void*)&webkitpointerlockchange) AtomicString(webkitpointerlockchangeImpl);
    new ((void*)&webkitpointerlockerror) AtomicString(webkitpointerlockerrorImpl);
    new ((void*)&webkitprerenderdomcontentloaded) AtomicString(webkitprerenderdomcontentloadedImpl);
    new ((void*)&webkitprerenderload) AtomicString(webkitprerenderloadImpl);
    new ((void*)&webkitprerenderstart) AtomicString(webkitprerenderstartImpl);
    new ((void*)&webkitprerenderstop) AtomicString(webkitprerenderstopImpl);
    new ((void*)&webkitremovesourcebuffer) AtomicString(webkitremovesourcebufferImpl);
    new ((void*)&webkitresourcetimingbufferfull) AtomicString(webkitresourcetimingbufferfullImpl);
    new ((void*)&webkitsourceclose) AtomicString(webkitsourcecloseImpl);
    new ((void*)&webkitsourceended) AtomicString(webkitsourceendedImpl);
    new ((void*)&webkitsourceopen) AtomicString(webkitsourceopenImpl);
    new ((void*)&webkitspeechchange) AtomicString(webkitspeechchangeImpl);
    new ((void*)&webkitvisibilitychange) AtomicString(webkitvisibilitychangeImpl);
    new ((void*)&wheel) AtomicString(wheelImpl);
    new ((void*)&write) AtomicString(writeImpl);
    new ((void*)&writeend) AtomicString(writeendImpl);
    new ((void*)&writestart) AtomicString(writestartImpl);
    new ((void*)&zoom) AtomicString(zoomImpl);
}

} // EventTypeNames
} // WebCore
