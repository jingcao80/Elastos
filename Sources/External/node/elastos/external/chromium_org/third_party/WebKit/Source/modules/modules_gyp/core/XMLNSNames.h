// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XMLNSNames_h
#define XMLNSNames_h

#include "core/dom/QualifiedName.h"
#include "wtf/PassOwnPtr.h"

namespace WebCore {
namespace XMLNSNames {

#ifndef XMLNS_NAMES_HIDE_GLOBALS
// Namespace
extern const WTF::AtomicString xmlnsNamespaceURI;

// Tags

// Attributes
extern const WebCore::QualifiedName xmlnsAttr;

#endif // XMLNS_NAMES_HIDE_GLOBALS
const unsigned XMLNSAttrsCount = 1;
PassOwnPtr<const QualifiedName*[]> getXMLNSAttrs();

void init();

} // XMLNSNames
} // WebCore

#endif
