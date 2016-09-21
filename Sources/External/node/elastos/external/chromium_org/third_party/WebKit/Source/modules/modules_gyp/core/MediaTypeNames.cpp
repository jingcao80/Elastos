// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#ifdef SKIP_STATIC_CONSTRUCTORS_ON_GCC
#define MEDIA_TYPE_NAMES_HIDE_GLOBALS 1
#endif

#include "MediaTypeNames.h"

#include "wtf/StaticConstructors.h"

namespace WebCore {
namespace MediaTypeNames {

using namespace WTF;

DEFINE_GLOBAL(AtomicString, all)
DEFINE_GLOBAL(AtomicString, braille)
DEFINE_GLOBAL(AtomicString, embossed)
DEFINE_GLOBAL(AtomicString, handheld)
DEFINE_GLOBAL(AtomicString, print)
DEFINE_GLOBAL(AtomicString, projection)
DEFINE_GLOBAL(AtomicString, screen)
DEFINE_GLOBAL(AtomicString, speech)
DEFINE_GLOBAL(AtomicString, tty)
DEFINE_GLOBAL(AtomicString, tv)

void init()
{
    StringImpl* allImpl = StringImpl::createStatic("all", 3, 10505778);
    StringImpl* brailleImpl = StringImpl::createStatic("braille", 7, 15044340);
    StringImpl* embossedImpl = StringImpl::createStatic("embossed", 8, 2246272);
    StringImpl* handheldImpl = StringImpl::createStatic("handheld", 8, 12743954);
    StringImpl* printImpl = StringImpl::createStatic("print", 5, 10456730);
    StringImpl* projectionImpl = StringImpl::createStatic("projection", 10, 1089243);
    StringImpl* screenImpl = StringImpl::createStatic("screen", 6, 13980269);
    StringImpl* speechImpl = StringImpl::createStatic("speech", 6, 4509099);
    StringImpl* ttyImpl = StringImpl::createStatic("tty", 3, 13331747);
    StringImpl* tvImpl = StringImpl::createStatic("tv", 2, 14748810);

    new ((void*)&all) AtomicString(allImpl);
    new ((void*)&braille) AtomicString(brailleImpl);
    new ((void*)&embossed) AtomicString(embossedImpl);
    new ((void*)&handheld) AtomicString(handheldImpl);
    new ((void*)&print) AtomicString(printImpl);
    new ((void*)&projection) AtomicString(projectionImpl);
    new ((void*)&screen) AtomicString(screenImpl);
    new ((void*)&speech) AtomicString(speechImpl);
    new ((void*)&tty) AtomicString(ttyImpl);
    new ((void*)&tv) AtomicString(tvImpl);
}

} // MediaTypeNames
} // WebCore
