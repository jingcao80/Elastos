// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MediaTypeNames_h
#define MediaTypeNames_h

#include "wtf/text/AtomicString.h"

namespace WebCore {
namespace MediaTypeNames {

#ifndef MEDIA_TYPE_NAMES_HIDE_GLOBALS

extern const WTF::AtomicString all;
extern const WTF::AtomicString braille;
extern const WTF::AtomicString embossed;
extern const WTF::AtomicString handheld;
extern const WTF::AtomicString print;
extern const WTF::AtomicString projection;
extern const WTF::AtomicString screen;
extern const WTF::AtomicString speech;
extern const WTF::AtomicString tty;
extern const WTF::AtomicString tv;

#endif // MEDIA_TYPE_NAMES_HIDE_GLOBALS

 void init();

} // MediaTypeNames
} // WebCore

#endif
