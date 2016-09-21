// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MathMLNames_h
#define MathMLNames_h

#include "core/dom/QualifiedName.h"
#include "wtf/PassOwnPtr.h"

namespace WebCore {
namespace MathMLNames {

#ifndef MATH_ML_NAMES_HIDE_GLOBALS
// Namespace
extern const WTF::AtomicString mathmlNamespaceURI;

// Tags
extern const WebCore::QualifiedName annotation_xmlTag;
extern const WebCore::QualifiedName malignmarkTag;
extern const WebCore::QualifiedName mathTag;
extern const WebCore::QualifiedName mglyphTag;
extern const WebCore::QualifiedName miTag;
extern const WebCore::QualifiedName mnTag;
extern const WebCore::QualifiedName moTag;
extern const WebCore::QualifiedName msTag;
extern const WebCore::QualifiedName mtextTag;

// Attributes
extern const WebCore::QualifiedName alttextAttr;
extern const WebCore::QualifiedName backgroundAttr;
extern const WebCore::QualifiedName closeAttr;
extern const WebCore::QualifiedName colorAttr;
extern const WebCore::QualifiedName columnspanAttr;
extern const WebCore::QualifiedName definitionURLAttr;
extern const WebCore::QualifiedName denomalignAttr;
extern const WebCore::QualifiedName encodingAttr;
extern const WebCore::QualifiedName fontfamilyAttr;
extern const WebCore::QualifiedName fontsizeAttr;
extern const WebCore::QualifiedName fontstyleAttr;
extern const WebCore::QualifiedName fontweightAttr;
extern const WebCore::QualifiedName linethicknessAttr;
extern const WebCore::QualifiedName mathbackgroundAttr;
extern const WebCore::QualifiedName mathcolorAttr;
extern const WebCore::QualifiedName mathsizeAttr;
extern const WebCore::QualifiedName mathvariantAttr;
extern const WebCore::QualifiedName numalignAttr;
extern const WebCore::QualifiedName openAttr;
extern const WebCore::QualifiedName rowspanAttr;
extern const WebCore::QualifiedName separatorsAttr;
extern const WebCore::QualifiedName stretchyAttr;

#endif // MATH_ML_NAMES_HIDE_GLOBALS
const unsigned MathMLTagsCount = 9;
PassOwnPtr<const QualifiedName*[]> getMathMLTags();
const unsigned MathMLAttrsCount = 22;
PassOwnPtr<const QualifiedName*[]> getMathMLAttrs();

void init();

} // MathMLNames
} // WebCore

#endif
