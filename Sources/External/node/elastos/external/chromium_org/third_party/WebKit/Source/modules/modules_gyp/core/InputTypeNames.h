// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InputTypeNames_h
#define InputTypeNames_h

#include "wtf/text/AtomicString.h"

namespace WebCore {
namespace InputTypeNames {

#ifndef INPUT_TYPE_NAMES_HIDE_GLOBALS

extern const WTF::AtomicString button;
extern const WTF::AtomicString checkbox;
extern const WTF::AtomicString color;
extern const WTF::AtomicString date;
extern const WTF::AtomicString datetime;
extern const WTF::AtomicString datetime_local;
extern const WTF::AtomicString email;
extern const WTF::AtomicString file;
extern const WTF::AtomicString hidden;
extern const WTF::AtomicString image;
extern const WTF::AtomicString month;
extern const WTF::AtomicString number;
extern const WTF::AtomicString password;
extern const WTF::AtomicString radio;
extern const WTF::AtomicString range;
extern const WTF::AtomicString reset;
extern const WTF::AtomicString search;
extern const WTF::AtomicString submit;
extern const WTF::AtomicString tel;
extern const WTF::AtomicString text;
extern const WTF::AtomicString time;
extern const WTF::AtomicString url;
extern const WTF::AtomicString week;

#endif // INPUT_TYPE_NAMES_HIDE_GLOBALS

 void init();

} // InputTypeNames
} // WebCore

#endif
