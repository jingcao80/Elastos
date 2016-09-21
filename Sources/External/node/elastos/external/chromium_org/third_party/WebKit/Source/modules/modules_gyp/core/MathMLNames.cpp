// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#ifdef SKIP_STATIC_CONSTRUCTORS_ON_GCC
#define MATH_ML_NAMES_HIDE_GLOBALS 1
#else
#define QNAME_DEFAULT_CONSTRUCTOR 1
#endif

#include "MathMLNames.h"

#include "wtf/StaticConstructors.h"

namespace WebCore {
namespace MathMLNames {

using namespace WebCore;

DEFINE_GLOBAL(AtomicString, mathmlNamespaceURI)

// Tags
DEFINE_GLOBAL(QualifiedName, annotation_xmlTag)
DEFINE_GLOBAL(QualifiedName, malignmarkTag)
DEFINE_GLOBAL(QualifiedName, mathTag)
DEFINE_GLOBAL(QualifiedName, mglyphTag)
DEFINE_GLOBAL(QualifiedName, miTag)
DEFINE_GLOBAL(QualifiedName, mnTag)
DEFINE_GLOBAL(QualifiedName, moTag)
DEFINE_GLOBAL(QualifiedName, msTag)
DEFINE_GLOBAL(QualifiedName, mtextTag)


PassOwnPtr<const QualifiedName*[]> getMathMLTags()
{
    OwnPtr<const QualifiedName*[]> tags = adoptArrayPtr(new const QualifiedName*[MathMLTagsCount]);
    tags[0] = reinterpret_cast<const QualifiedName*>(&annotation_xmlTag);
    tags[1] = reinterpret_cast<const QualifiedName*>(&malignmarkTag);
    tags[2] = reinterpret_cast<const QualifiedName*>(&mathTag);
    tags[3] = reinterpret_cast<const QualifiedName*>(&mglyphTag);
    tags[4] = reinterpret_cast<const QualifiedName*>(&miTag);
    tags[5] = reinterpret_cast<const QualifiedName*>(&mnTag);
    tags[6] = reinterpret_cast<const QualifiedName*>(&moTag);
    tags[7] = reinterpret_cast<const QualifiedName*>(&msTag);
    tags[8] = reinterpret_cast<const QualifiedName*>(&mtextTag);
    return tags.release();
}

// Attributes
DEFINE_GLOBAL(QualifiedName, alttextAttr)
DEFINE_GLOBAL(QualifiedName, backgroundAttr)
DEFINE_GLOBAL(QualifiedName, closeAttr)
DEFINE_GLOBAL(QualifiedName, colorAttr)
DEFINE_GLOBAL(QualifiedName, columnspanAttr)
DEFINE_GLOBAL(QualifiedName, definitionURLAttr)
DEFINE_GLOBAL(QualifiedName, denomalignAttr)
DEFINE_GLOBAL(QualifiedName, encodingAttr)
DEFINE_GLOBAL(QualifiedName, fontfamilyAttr)
DEFINE_GLOBAL(QualifiedName, fontsizeAttr)
DEFINE_GLOBAL(QualifiedName, fontstyleAttr)
DEFINE_GLOBAL(QualifiedName, fontweightAttr)
DEFINE_GLOBAL(QualifiedName, linethicknessAttr)
DEFINE_GLOBAL(QualifiedName, mathbackgroundAttr)
DEFINE_GLOBAL(QualifiedName, mathcolorAttr)
DEFINE_GLOBAL(QualifiedName, mathsizeAttr)
DEFINE_GLOBAL(QualifiedName, mathvariantAttr)
DEFINE_GLOBAL(QualifiedName, numalignAttr)
DEFINE_GLOBAL(QualifiedName, openAttr)
DEFINE_GLOBAL(QualifiedName, rowspanAttr)
DEFINE_GLOBAL(QualifiedName, separatorsAttr)
DEFINE_GLOBAL(QualifiedName, stretchyAttr)

PassOwnPtr<const QualifiedName*[]> getMathMLAttrs()
{
    OwnPtr<const QualifiedName*[]> attrs = adoptArrayPtr(new const QualifiedName*[MathMLAttrsCount]);
    attrs[0] = reinterpret_cast<const WebCore::QualifiedName*>(&alttextAttr);
    attrs[1] = reinterpret_cast<const WebCore::QualifiedName*>(&backgroundAttr);
    attrs[2] = reinterpret_cast<const WebCore::QualifiedName*>(&closeAttr);
    attrs[3] = reinterpret_cast<const WebCore::QualifiedName*>(&colorAttr);
    attrs[4] = reinterpret_cast<const WebCore::QualifiedName*>(&columnspanAttr);
    attrs[5] = reinterpret_cast<const WebCore::QualifiedName*>(&definitionURLAttr);
    attrs[6] = reinterpret_cast<const WebCore::QualifiedName*>(&denomalignAttr);
    attrs[7] = reinterpret_cast<const WebCore::QualifiedName*>(&encodingAttr);
    attrs[8] = reinterpret_cast<const WebCore::QualifiedName*>(&fontfamilyAttr);
    attrs[9] = reinterpret_cast<const WebCore::QualifiedName*>(&fontsizeAttr);
    attrs[10] = reinterpret_cast<const WebCore::QualifiedName*>(&fontstyleAttr);
    attrs[11] = reinterpret_cast<const WebCore::QualifiedName*>(&fontweightAttr);
    attrs[12] = reinterpret_cast<const WebCore::QualifiedName*>(&linethicknessAttr);
    attrs[13] = reinterpret_cast<const WebCore::QualifiedName*>(&mathbackgroundAttr);
    attrs[14] = reinterpret_cast<const WebCore::QualifiedName*>(&mathcolorAttr);
    attrs[15] = reinterpret_cast<const WebCore::QualifiedName*>(&mathsizeAttr);
    attrs[16] = reinterpret_cast<const WebCore::QualifiedName*>(&mathvariantAttr);
    attrs[17] = reinterpret_cast<const WebCore::QualifiedName*>(&numalignAttr);
    attrs[18] = reinterpret_cast<const WebCore::QualifiedName*>(&openAttr);
    attrs[19] = reinterpret_cast<const WebCore::QualifiedName*>(&rowspanAttr);
    attrs[20] = reinterpret_cast<const WebCore::QualifiedName*>(&separatorsAttr);
    attrs[21] = reinterpret_cast<const WebCore::QualifiedName*>(&stretchyAttr);
    return attrs.release();
}

void init()
{
    // Use placement new to initialize the globals.
    AtomicString mathmlNS("http://www.w3.org/1998/Math/MathML", AtomicString::ConstructFromLiteral);

    // Namespace
    new ((void*)&mathmlNamespaceURI) AtomicString(mathmlNS);
    StringImpl* alttextImpl = StringImpl::createStatic("alttext", 7, 14960494);
    StringImpl* annotation_xmlImpl = StringImpl::createStatic("annotation-xml", 14, 3437210);
    StringImpl* backgroundImpl = StringImpl::createStatic("background", 10, 8107606);
    StringImpl* closeImpl = StringImpl::createStatic("close", 5, 3222970);
    StringImpl* colorImpl = StringImpl::createStatic("color", 5, 2734929);
    StringImpl* columnspanImpl = StringImpl::createStatic("columnspan", 10, 6827651);
    StringImpl* definitionURLImpl = StringImpl::createStatic("definitionURL", 13, 5781331);
    StringImpl* denomalignImpl = StringImpl::createStatic("denomalign", 10, 9559630);
    StringImpl* encodingImpl = StringImpl::createStatic("encoding", 8, 641915);
    StringImpl* fontfamilyImpl = StringImpl::createStatic("fontfamily", 10, 6143894);
    StringImpl* fontsizeImpl = StringImpl::createStatic("fontsize", 8, 15308314);
    StringImpl* fontstyleImpl = StringImpl::createStatic("fontstyle", 9, 2479154);
    StringImpl* fontweightImpl = StringImpl::createStatic("fontweight", 10, 4781093);
    StringImpl* linethicknessImpl = StringImpl::createStatic("linethickness", 13, 7924943);
    StringImpl* malignmarkImpl = StringImpl::createStatic("malignmark", 10, 8634190);
    StringImpl* mathImpl = StringImpl::createStatic("math", 4, 8182406);
    StringImpl* mathbackgroundImpl = StringImpl::createStatic("mathbackground", 14, 2374557);
    StringImpl* mathcolorImpl = StringImpl::createStatic("mathcolor", 9, 7904383);
    StringImpl* mathsizeImpl = StringImpl::createStatic("mathsize", 8, 6623078);
    StringImpl* mathvariantImpl = StringImpl::createStatic("mathvariant", 11, 9126505);
    StringImpl* mglyphImpl = StringImpl::createStatic("mglyph", 6, 3316076);
    StringImpl* miImpl = StringImpl::createStatic("mi", 2, 16514040);
    StringImpl* mnImpl = StringImpl::createStatic("mn", 2, 3612898);
    StringImpl* moImpl = StringImpl::createStatic("mo", 2, 16096753);
    StringImpl* msImpl = StringImpl::createStatic("ms", 2, 5264856);
    StringImpl* mtextImpl = StringImpl::createStatic("mtext", 5, 16756693);
    StringImpl* numalignImpl = StringImpl::createStatic("numalign", 8, 8165736);
    StringImpl* openImpl = StringImpl::createStatic("open", 4, 13703631);
    StringImpl* rowspanImpl = StringImpl::createStatic("rowspan", 7, 13630419);
    StringImpl* separatorsImpl = StringImpl::createStatic("separators", 10, 11231386);
    StringImpl* stretchyImpl = StringImpl::createStatic("stretchy", 8, 9937605);

    // Tags
    QualifiedName::createStatic((void*)&annotation_xmlTag, annotation_xmlImpl, mathmlNS);
    QualifiedName::createStatic((void*)&malignmarkTag, malignmarkImpl, mathmlNS);
    QualifiedName::createStatic((void*)&mathTag, mathImpl, mathmlNS);
    QualifiedName::createStatic((void*)&mglyphTag, mglyphImpl, mathmlNS);
    QualifiedName::createStatic((void*)&miTag, miImpl, mathmlNS);
    QualifiedName::createStatic((void*)&mnTag, mnImpl, mathmlNS);
    QualifiedName::createStatic((void*)&moTag, moImpl, mathmlNS);
    QualifiedName::createStatic((void*)&msTag, msImpl, mathmlNS);
    QualifiedName::createStatic((void*)&mtextTag, mtextImpl, mathmlNS);

    // Attrs
    QualifiedName::createStatic((void*)&alttextAttr, alttextImpl);
    QualifiedName::createStatic((void*)&backgroundAttr, backgroundImpl);
    QualifiedName::createStatic((void*)&closeAttr, closeImpl);
    QualifiedName::createStatic((void*)&colorAttr, colorImpl);
    QualifiedName::createStatic((void*)&columnspanAttr, columnspanImpl);
    QualifiedName::createStatic((void*)&definitionURLAttr, definitionURLImpl);
    QualifiedName::createStatic((void*)&denomalignAttr, denomalignImpl);
    QualifiedName::createStatic((void*)&encodingAttr, encodingImpl);
    QualifiedName::createStatic((void*)&fontfamilyAttr, fontfamilyImpl);
    QualifiedName::createStatic((void*)&fontsizeAttr, fontsizeImpl);
    QualifiedName::createStatic((void*)&fontstyleAttr, fontstyleImpl);
    QualifiedName::createStatic((void*)&fontweightAttr, fontweightImpl);
    QualifiedName::createStatic((void*)&linethicknessAttr, linethicknessImpl);
    QualifiedName::createStatic((void*)&mathbackgroundAttr, mathbackgroundImpl);
    QualifiedName::createStatic((void*)&mathcolorAttr, mathcolorImpl);
    QualifiedName::createStatic((void*)&mathsizeAttr, mathsizeImpl);
    QualifiedName::createStatic((void*)&mathvariantAttr, mathvariantImpl);
    QualifiedName::createStatic((void*)&numalignAttr, numalignImpl);
    QualifiedName::createStatic((void*)&openAttr, openImpl);
    QualifiedName::createStatic((void*)&rowspanAttr, rowspanImpl);
    QualifiedName::createStatic((void*)&separatorsAttr, separatorsImpl);
    QualifiedName::createStatic((void*)&stretchyAttr, stretchyImpl);
}

} // MathML
} // WebCore
