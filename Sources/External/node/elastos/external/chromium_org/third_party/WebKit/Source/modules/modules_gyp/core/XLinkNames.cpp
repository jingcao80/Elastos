// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#ifdef SKIP_STATIC_CONSTRUCTORS_ON_GCC
#define X_LINK_NAMES_HIDE_GLOBALS 1
#else
#define QNAME_DEFAULT_CONSTRUCTOR 1
#endif

#include "XLinkNames.h"

#include "wtf/StaticConstructors.h"

namespace WebCore {
namespace XLinkNames {

using namespace WebCore;

DEFINE_GLOBAL(AtomicString, xlinkNamespaceURI)

// Attributes
DEFINE_GLOBAL(QualifiedName, actuateAttr)
DEFINE_GLOBAL(QualifiedName, arcroleAttr)
DEFINE_GLOBAL(QualifiedName, hrefAttr)
DEFINE_GLOBAL(QualifiedName, roleAttr)
DEFINE_GLOBAL(QualifiedName, showAttr)
DEFINE_GLOBAL(QualifiedName, titleAttr)
DEFINE_GLOBAL(QualifiedName, typeAttr)

PassOwnPtr<const QualifiedName*[]> getXLinkAttrs()
{
    OwnPtr<const QualifiedName*[]> attrs = adoptArrayPtr(new const QualifiedName*[XLinkAttrsCount]);
    attrs[0] = reinterpret_cast<const WebCore::QualifiedName*>(&actuateAttr);
    attrs[1] = reinterpret_cast<const WebCore::QualifiedName*>(&arcroleAttr);
    attrs[2] = reinterpret_cast<const WebCore::QualifiedName*>(&hrefAttr);
    attrs[3] = reinterpret_cast<const WebCore::QualifiedName*>(&roleAttr);
    attrs[4] = reinterpret_cast<const WebCore::QualifiedName*>(&showAttr);
    attrs[5] = reinterpret_cast<const WebCore::QualifiedName*>(&titleAttr);
    attrs[6] = reinterpret_cast<const WebCore::QualifiedName*>(&typeAttr);
    return attrs.release();
}

void init()
{
    // Use placement new to initialize the globals.
    AtomicString xlinkNS("http://www.w3.org/1999/xlink", AtomicString::ConstructFromLiteral);

    // Namespace
    new ((void*)&xlinkNamespaceURI) AtomicString(xlinkNS);
    StringImpl* actuateImpl = StringImpl::createStatic("actuate", 7, 5578769);
    StringImpl* arcroleImpl = StringImpl::createStatic("arcrole", 7, 11561565);
    StringImpl* hrefImpl = StringImpl::createStatic("href", 4, 5797448);
    StringImpl* roleImpl = StringImpl::createStatic("role", 4, 16084934);
    StringImpl* showImpl = StringImpl::createStatic("show", 4, 3191658);
    StringImpl* titleImpl = StringImpl::createStatic("title", 5, 2337488);
    StringImpl* typeImpl = StringImpl::createStatic("type", 4, 1916283);

    // Tags

    // Attrs
    QualifiedName::createStatic((void*)&actuateAttr, actuateImpl, xlinkNS);
    QualifiedName::createStatic((void*)&arcroleAttr, arcroleImpl, xlinkNS);
    QualifiedName::createStatic((void*)&hrefAttr, hrefImpl, xlinkNS);
    QualifiedName::createStatic((void*)&roleAttr, roleImpl, xlinkNS);
    QualifiedName::createStatic((void*)&showAttr, showImpl, xlinkNS);
    QualifiedName::createStatic((void*)&titleAttr, titleImpl, xlinkNS);
    QualifiedName::createStatic((void*)&typeAttr, typeImpl, xlinkNS);
}

} // XLink
} // WebCore
