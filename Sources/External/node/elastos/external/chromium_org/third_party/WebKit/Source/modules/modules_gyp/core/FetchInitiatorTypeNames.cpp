// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#ifdef SKIP_STATIC_CONSTRUCTORS_ON_GCC
#define FETCH_INITIATOR_TYPE_NAMES_HIDE_GLOBALS 1
#endif

#include "FetchInitiatorTypeNames.h"

#include "wtf/StaticConstructors.h"

namespace WebCore {
namespace FetchInitiatorTypeNames {

using namespace WTF;

DEFINE_GLOBAL(AtomicString, beacon)
DEFINE_GLOBAL(AtomicString, css)
DEFINE_GLOBAL(AtomicString, document)
DEFINE_GLOBAL(AtomicString, icon)
DEFINE_GLOBAL(AtomicString, internal)
DEFINE_GLOBAL(AtomicString, link)
DEFINE_GLOBAL(AtomicString, ping)
DEFINE_GLOBAL(AtomicString, processinginstruction)
DEFINE_GLOBAL(AtomicString, texttrack)
DEFINE_GLOBAL(AtomicString, violationreport)
DEFINE_GLOBAL(AtomicString, xml)
DEFINE_GLOBAL(AtomicString, xmlhttprequest)

void init()
{
    StringImpl* beaconImpl = StringImpl::createStatic("beacon", 6, 12791408);
    StringImpl* cssImpl = StringImpl::createStatic("css", 3, 12506144);
    StringImpl* documentImpl = StringImpl::createStatic("document", 8, 13333587);
    StringImpl* iconImpl = StringImpl::createStatic("icon", 4, 12131262);
    StringImpl* internalImpl = StringImpl::createStatic("internal", 8, 15376144);
    StringImpl* linkImpl = StringImpl::createStatic("link", 4, 7010491);
    StringImpl* pingImpl = StringImpl::createStatic("ping", 4, 9193756);
    StringImpl* processinginstructionImpl = StringImpl::createStatic("processinginstruction", 21, 8687069);
    StringImpl* texttrackImpl = StringImpl::createStatic("texttrack", 9, 5772877);
    StringImpl* violationreportImpl = StringImpl::createStatic("violationreport", 15, 9570399);
    StringImpl* xmlImpl = StringImpl::createStatic("xml", 3, 10322861);
    StringImpl* xmlhttprequestImpl = StringImpl::createStatic("xmlhttprequest", 14, 10847986);

    new ((void*)&beacon) AtomicString(beaconImpl);
    new ((void*)&css) AtomicString(cssImpl);
    new ((void*)&document) AtomicString(documentImpl);
    new ((void*)&icon) AtomicString(iconImpl);
    new ((void*)&internal) AtomicString(internalImpl);
    new ((void*)&link) AtomicString(linkImpl);
    new ((void*)&ping) AtomicString(pingImpl);
    new ((void*)&processinginstruction) AtomicString(processinginstructionImpl);
    new ((void*)&texttrack) AtomicString(texttrackImpl);
    new ((void*)&violationreport) AtomicString(violationreportImpl);
    new ((void*)&xml) AtomicString(xmlImpl);
    new ((void*)&xmlhttprequest) AtomicString(xmlhttprequestImpl);
}

} // FetchInitiatorTypeNames
} // WebCore
