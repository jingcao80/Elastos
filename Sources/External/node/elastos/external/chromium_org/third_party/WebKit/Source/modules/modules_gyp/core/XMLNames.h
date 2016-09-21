// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XMLNames_h
#define XMLNames_h

#include "core/dom/QualifiedName.h"
#include "wtf/PassOwnPtr.h"

namespace WebCore {
namespace XMLNames {

#ifndef XML_NAMES_HIDE_GLOBALS
// Namespace
extern const WTF::AtomicString xmlNamespaceURI;

// Tags

// Attributes
extern const WebCore::QualifiedName baseAttr;
extern const WebCore::QualifiedName langAttr;
extern const WebCore::QualifiedName spaceAttr;

#endif // XML_NAMES_HIDE_GLOBALS
const unsigned XMLAttrsCount = 3;
PassOwnPtr<const QualifiedName*[]> getXMLAttrs();

void init();

} // XMLNames
} // WebCore

#endif
