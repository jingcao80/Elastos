// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HTMLTokenizerNames_h
#define HTMLTokenizerNames_h

#include "wtf/text/AtomicString.h"

namespace WebCore {
namespace HTMLTokenizerNames {

#ifndef HTML_TOKENIZER_NAMES_HIDE_GLOBALS

extern const WTF::AtomicString doctype;
extern const WTF::AtomicString system;
extern const WTF::AtomicString cdata;
extern const WTF::AtomicString dashDash;
extern const WTF::AtomicString publicString;

#endif // HTML_TOKENIZER_NAMES_HIDE_GLOBALS

 void init();

} // HTMLTokenizerNames
} // WebCore

#endif
