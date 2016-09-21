// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XLinkNames_h
#define XLinkNames_h

#include "core/dom/QualifiedName.h"
#include "wtf/PassOwnPtr.h"

namespace WebCore {
namespace XLinkNames {

#ifndef X_LINK_NAMES_HIDE_GLOBALS
// Namespace
extern const WTF::AtomicString xlinkNamespaceURI;

// Tags

// Attributes
extern const WebCore::QualifiedName actuateAttr;
extern const WebCore::QualifiedName arcroleAttr;
extern const WebCore::QualifiedName hrefAttr;
extern const WebCore::QualifiedName roleAttr;
extern const WebCore::QualifiedName showAttr;
extern const WebCore::QualifiedName titleAttr;
extern const WebCore::QualifiedName typeAttr;

#endif // X_LINK_NAMES_HIDE_GLOBALS
const unsigned XLinkAttrsCount = 7;
PassOwnPtr<const QualifiedName*[]> getXLinkAttrs();

void init();

} // XLinkNames
} // WebCore

#endif
