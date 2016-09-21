// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#ifdef SKIP_STATIC_CONSTRUCTORS_ON_GCC
#define HTML_TOKENIZER_NAMES_HIDE_GLOBALS 1
#endif

#include "HTMLTokenizerNames.h"

#include "wtf/StaticConstructors.h"

namespace WebCore {
namespace HTMLTokenizerNames {

using namespace WTF;

DEFINE_GLOBAL(AtomicString, doctype)
DEFINE_GLOBAL(AtomicString, system)
DEFINE_GLOBAL(AtomicString, cdata)
DEFINE_GLOBAL(AtomicString, dashDash)
DEFINE_GLOBAL(AtomicString, publicString)

void init()
{
    StringImpl* doctypeImpl = StringImpl::createStatic("doctype", 7, 10540185);
    StringImpl* systemImpl = StringImpl::createStatic("system", 6, 7974370);
    StringImpl* cdataImpl = StringImpl::createStatic("[CDATA[", 7, 11402496);
    StringImpl* dashDashImpl = StringImpl::createStatic("--", 2, 7376722);
    StringImpl* publicStringImpl = StringImpl::createStatic("public", 6, 14399889);

    new ((void*)&doctype) AtomicString(doctypeImpl);
    new ((void*)&system) AtomicString(systemImpl);
    new ((void*)&cdata) AtomicString(cdataImpl);
    new ((void*)&dashDash) AtomicString(dashDashImpl);
    new ((void*)&publicString) AtomicString(publicStringImpl);
}

} // HTMLTokenizerNames
} // WebCore
