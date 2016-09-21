// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#ifdef SKIP_STATIC_CONSTRUCTORS_ON_GCC
#define XMLNS_NAMES_HIDE_GLOBALS 1
#else
#define QNAME_DEFAULT_CONSTRUCTOR 1
#endif

#include "XMLNSNames.h"

#include "wtf/StaticConstructors.h"

namespace WebCore {
namespace XMLNSNames {

using namespace WebCore;

DEFINE_GLOBAL(AtomicString, xmlnsNamespaceURI)

// Attributes
DEFINE_GLOBAL(QualifiedName, xmlnsAttr)

PassOwnPtr<const QualifiedName*[]> getXMLNSAttrs()
{
    OwnPtr<const QualifiedName*[]> attrs = adoptArrayPtr(new const QualifiedName*[XMLNSAttrsCount]);
    attrs[0] = reinterpret_cast<const WebCore::QualifiedName*>(&xmlnsAttr);
    return attrs.release();
}

void init()
{
    // Use placement new to initialize the globals.
    AtomicString xmlnsNS("http://www.w3.org/2000/xmlns/", AtomicString::ConstructFromLiteral);

    // Namespace
    new ((void*)&xmlnsNamespaceURI) AtomicString(xmlnsNS);
    StringImpl* xmlnsImpl = StringImpl::createStatic("xmlns", 5, 171597);

    // Tags

    // Attrs
    QualifiedName::createStatic((void*)&xmlnsAttr, xmlnsImpl, xmlnsNS);
}

} // XMLNS
} // WebCore
