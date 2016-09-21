// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#ifdef SKIP_STATIC_CONSTRUCTORS_ON_GCC
#define XML_NAMES_HIDE_GLOBALS 1
#else
#define QNAME_DEFAULT_CONSTRUCTOR 1
#endif

#include "XMLNames.h"

#include "wtf/StaticConstructors.h"

namespace WebCore {
namespace XMLNames {

using namespace WebCore;

DEFINE_GLOBAL(AtomicString, xmlNamespaceURI)

// Attributes
DEFINE_GLOBAL(QualifiedName, baseAttr)
DEFINE_GLOBAL(QualifiedName, langAttr)
DEFINE_GLOBAL(QualifiedName, spaceAttr)

PassOwnPtr<const QualifiedName*[]> getXMLAttrs()
{
    OwnPtr<const QualifiedName*[]> attrs = adoptArrayPtr(new const QualifiedName*[XMLAttrsCount]);
    attrs[0] = reinterpret_cast<const WebCore::QualifiedName*>(&baseAttr);
    attrs[1] = reinterpret_cast<const WebCore::QualifiedName*>(&langAttr);
    attrs[2] = reinterpret_cast<const WebCore::QualifiedName*>(&spaceAttr);
    return attrs.release();
}

void init()
{
    // Use placement new to initialize the globals.
    AtomicString xmlNS("http://www.w3.org/XML/1998/namespace", AtomicString::ConstructFromLiteral);

    // Namespace
    new ((void*)&xmlNamespaceURI) AtomicString(xmlNS);
    StringImpl* baseImpl = StringImpl::createStatic("base", 4, 4376626);
    StringImpl* langImpl = StringImpl::createStatic("lang", 4, 3702417);
    StringImpl* spaceImpl = StringImpl::createStatic("space", 5, 531440);

    // Tags

    // Attrs
    QualifiedName::createStatic((void*)&baseAttr, baseImpl, xmlNS);
    QualifiedName::createStatic((void*)&langAttr, langImpl, xmlNS);
    QualifiedName::createStatic((void*)&spaceAttr, spaceImpl, xmlNS);
}

} // XML
} // WebCore
