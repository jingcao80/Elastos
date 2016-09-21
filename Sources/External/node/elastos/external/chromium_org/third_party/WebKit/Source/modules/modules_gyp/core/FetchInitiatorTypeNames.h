// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FetchInitiatorTypeNames_h
#define FetchInitiatorTypeNames_h

#include "wtf/text/AtomicString.h"

namespace WebCore {
namespace FetchInitiatorTypeNames {

#ifndef FETCH_INITIATOR_TYPE_NAMES_HIDE_GLOBALS

extern const WTF::AtomicString beacon;
extern const WTF::AtomicString css;
extern const WTF::AtomicString document;
extern const WTF::AtomicString icon;
extern const WTF::AtomicString internal;
extern const WTF::AtomicString link;
extern const WTF::AtomicString ping;
extern const WTF::AtomicString processinginstruction;
extern const WTF::AtomicString texttrack;
extern const WTF::AtomicString violationreport;
extern const WTF::AtomicString xml;
extern const WTF::AtomicString xmlhttprequest;

#endif // FETCH_INITIATOR_TYPE_NAMES_HIDE_GLOBALS

 void init();

} // FetchInitiatorTypeNames
} // WebCore

#endif
