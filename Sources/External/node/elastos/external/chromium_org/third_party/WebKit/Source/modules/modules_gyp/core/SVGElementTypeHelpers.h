// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SVGElementTypeHelpers_h
#define SVGElementTypeHelpers_h

#include "core/dom/ContextFeatures.h"
#include "core/dom/Element.h"
#include "SVGNames.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace WebCore {
// Type checking.
class SVGAElement;
void isSVGAElement(const SVGAElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGAElement(const SVGAElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGAElement(const Element& element) {
    return element.hasTagName(SVGNames::aTag);
}
inline bool isSVGAElement(const Element* element) { return element && isSVGAElement(*element); }
template<typename T> inline bool isSVGAElement(const PassRefPtr<T>& node) { return isSVGAElement(node.get()); }
template<typename T> inline bool isSVGAElement(const RefPtr<T>& node) { return isSVGAElement(node.get()); }
inline bool isSVGAElement(const Node& node) { return node.isElementNode() ? isSVGAElement(toElement(node)) : false; }
inline bool isSVGAElement(const Node* node) { return node && node->isElementNode() ? isSVGAElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGAElement>(const Element& element) { return isSVGAElement(element); }
class SVGAnimateMotionElement;
void isSVGAnimateMotionElement(const SVGAnimateMotionElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGAnimateMotionElement(const SVGAnimateMotionElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGAnimateMotionElement(const Element& element) {
    return element.hasTagName(SVGNames::animateMotionTag);
}
inline bool isSVGAnimateMotionElement(const Element* element) { return element && isSVGAnimateMotionElement(*element); }
template<typename T> inline bool isSVGAnimateMotionElement(const PassRefPtr<T>& node) { return isSVGAnimateMotionElement(node.get()); }
template<typename T> inline bool isSVGAnimateMotionElement(const RefPtr<T>& node) { return isSVGAnimateMotionElement(node.get()); }
inline bool isSVGAnimateMotionElement(const Node& node) { return node.isElementNode() ? isSVGAnimateMotionElement(toElement(node)) : false; }
inline bool isSVGAnimateMotionElement(const Node* node) { return node && node->isElementNode() ? isSVGAnimateMotionElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGAnimateMotionElement>(const Element& element) { return isSVGAnimateMotionElement(element); }
class SVGAnimateTransformElement;
void isSVGAnimateTransformElement(const SVGAnimateTransformElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGAnimateTransformElement(const SVGAnimateTransformElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGAnimateTransformElement(const Element& element) {
    return element.hasTagName(SVGNames::animateTransformTag);
}
inline bool isSVGAnimateTransformElement(const Element* element) { return element && isSVGAnimateTransformElement(*element); }
template<typename T> inline bool isSVGAnimateTransformElement(const PassRefPtr<T>& node) { return isSVGAnimateTransformElement(node.get()); }
template<typename T> inline bool isSVGAnimateTransformElement(const RefPtr<T>& node) { return isSVGAnimateTransformElement(node.get()); }
inline bool isSVGAnimateTransformElement(const Node& node) { return node.isElementNode() ? isSVGAnimateTransformElement(toElement(node)) : false; }
inline bool isSVGAnimateTransformElement(const Node* node) { return node && node->isElementNode() ? isSVGAnimateTransformElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGAnimateTransformElement>(const Element& element) { return isSVGAnimateTransformElement(element); }
class SVGCircleElement;
void isSVGCircleElement(const SVGCircleElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGCircleElement(const SVGCircleElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGCircleElement(const Element& element) {
    return element.hasTagName(SVGNames::circleTag);
}
inline bool isSVGCircleElement(const Element* element) { return element && isSVGCircleElement(*element); }
template<typename T> inline bool isSVGCircleElement(const PassRefPtr<T>& node) { return isSVGCircleElement(node.get()); }
template<typename T> inline bool isSVGCircleElement(const RefPtr<T>& node) { return isSVGCircleElement(node.get()); }
inline bool isSVGCircleElement(const Node& node) { return node.isElementNode() ? isSVGCircleElement(toElement(node)) : false; }
inline bool isSVGCircleElement(const Node* node) { return node && node->isElementNode() ? isSVGCircleElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGCircleElement>(const Element& element) { return isSVGCircleElement(element); }
class SVGClipPathElement;
void isSVGClipPathElement(const SVGClipPathElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGClipPathElement(const SVGClipPathElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGClipPathElement(const Element& element) {
    return element.hasTagName(SVGNames::clipPathTag);
}
inline bool isSVGClipPathElement(const Element* element) { return element && isSVGClipPathElement(*element); }
template<typename T> inline bool isSVGClipPathElement(const PassRefPtr<T>& node) { return isSVGClipPathElement(node.get()); }
template<typename T> inline bool isSVGClipPathElement(const RefPtr<T>& node) { return isSVGClipPathElement(node.get()); }
inline bool isSVGClipPathElement(const Node& node) { return node.isElementNode() ? isSVGClipPathElement(toElement(node)) : false; }
inline bool isSVGClipPathElement(const Node* node) { return node && node->isElementNode() ? isSVGClipPathElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGClipPathElement>(const Element& element) { return isSVGClipPathElement(element); }
class SVGCursorElement;
void isSVGCursorElement(const SVGCursorElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGCursorElement(const SVGCursorElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGCursorElement(const Element& element) {
    return element.hasTagName(SVGNames::cursorTag);
}
inline bool isSVGCursorElement(const Element* element) { return element && isSVGCursorElement(*element); }
template<typename T> inline bool isSVGCursorElement(const PassRefPtr<T>& node) { return isSVGCursorElement(node.get()); }
template<typename T> inline bool isSVGCursorElement(const RefPtr<T>& node) { return isSVGCursorElement(node.get()); }
inline bool isSVGCursorElement(const Node& node) { return node.isElementNode() ? isSVGCursorElement(toElement(node)) : false; }
inline bool isSVGCursorElement(const Node* node) { return node && node->isElementNode() ? isSVGCursorElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGCursorElement>(const Element& element) { return isSVGCursorElement(element); }
class SVGDefsElement;
void isSVGDefsElement(const SVGDefsElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGDefsElement(const SVGDefsElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGDefsElement(const Element& element) {
    return element.hasTagName(SVGNames::defsTag);
}
inline bool isSVGDefsElement(const Element* element) { return element && isSVGDefsElement(*element); }
template<typename T> inline bool isSVGDefsElement(const PassRefPtr<T>& node) { return isSVGDefsElement(node.get()); }
template<typename T> inline bool isSVGDefsElement(const RefPtr<T>& node) { return isSVGDefsElement(node.get()); }
inline bool isSVGDefsElement(const Node& node) { return node.isElementNode() ? isSVGDefsElement(toElement(node)) : false; }
inline bool isSVGDefsElement(const Node* node) { return node && node->isElementNode() ? isSVGDefsElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGDefsElement>(const Element& element) { return isSVGDefsElement(element); }
class SVGDescElement;
void isSVGDescElement(const SVGDescElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGDescElement(const SVGDescElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGDescElement(const Element& element) {
    return element.hasTagName(SVGNames::descTag);
}
inline bool isSVGDescElement(const Element* element) { return element && isSVGDescElement(*element); }
template<typename T> inline bool isSVGDescElement(const PassRefPtr<T>& node) { return isSVGDescElement(node.get()); }
template<typename T> inline bool isSVGDescElement(const RefPtr<T>& node) { return isSVGDescElement(node.get()); }
inline bool isSVGDescElement(const Node& node) { return node.isElementNode() ? isSVGDescElement(toElement(node)) : false; }
inline bool isSVGDescElement(const Node* node) { return node && node->isElementNode() ? isSVGDescElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGDescElement>(const Element& element) { return isSVGDescElement(element); }
class SVGDiscardElement;
void isSVGDiscardElement(const SVGDiscardElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGDiscardElement(const SVGDiscardElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGDiscardElement(const Element& element) {
    return element.hasTagName(SVGNames::discardTag);
}
inline bool isSVGDiscardElement(const Element* element) { return element && isSVGDiscardElement(*element); }
template<typename T> inline bool isSVGDiscardElement(const PassRefPtr<T>& node) { return isSVGDiscardElement(node.get()); }
template<typename T> inline bool isSVGDiscardElement(const RefPtr<T>& node) { return isSVGDiscardElement(node.get()); }
inline bool isSVGDiscardElement(const Node& node) { return node.isElementNode() ? isSVGDiscardElement(toElement(node)) : false; }
inline bool isSVGDiscardElement(const Node* node) { return node && node->isElementNode() ? isSVGDiscardElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGDiscardElement>(const Element& element) { return isSVGDiscardElement(element); }
class SVGEllipseElement;
void isSVGEllipseElement(const SVGEllipseElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGEllipseElement(const SVGEllipseElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGEllipseElement(const Element& element) {
    return element.hasTagName(SVGNames::ellipseTag);
}
inline bool isSVGEllipseElement(const Element* element) { return element && isSVGEllipseElement(*element); }
template<typename T> inline bool isSVGEllipseElement(const PassRefPtr<T>& node) { return isSVGEllipseElement(node.get()); }
template<typename T> inline bool isSVGEllipseElement(const RefPtr<T>& node) { return isSVGEllipseElement(node.get()); }
inline bool isSVGEllipseElement(const Node& node) { return node.isElementNode() ? isSVGEllipseElement(toElement(node)) : false; }
inline bool isSVGEllipseElement(const Node* node) { return node && node->isElementNode() ? isSVGEllipseElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGEllipseElement>(const Element& element) { return isSVGEllipseElement(element); }
class SVGFEBlendElement;
void isSVGFEBlendElement(const SVGFEBlendElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEBlendElement(const SVGFEBlendElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEBlendElement(const Element& element) {
    return element.hasTagName(SVGNames::feBlendTag);
}
inline bool isSVGFEBlendElement(const Element* element) { return element && isSVGFEBlendElement(*element); }
template<typename T> inline bool isSVGFEBlendElement(const PassRefPtr<T>& node) { return isSVGFEBlendElement(node.get()); }
template<typename T> inline bool isSVGFEBlendElement(const RefPtr<T>& node) { return isSVGFEBlendElement(node.get()); }
inline bool isSVGFEBlendElement(const Node& node) { return node.isElementNode() ? isSVGFEBlendElement(toElement(node)) : false; }
inline bool isSVGFEBlendElement(const Node* node) { return node && node->isElementNode() ? isSVGFEBlendElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEBlendElement>(const Element& element) { return isSVGFEBlendElement(element); }
class SVGFEColorMatrixElement;
void isSVGFEColorMatrixElement(const SVGFEColorMatrixElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEColorMatrixElement(const SVGFEColorMatrixElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEColorMatrixElement(const Element& element) {
    return element.hasTagName(SVGNames::feColorMatrixTag);
}
inline bool isSVGFEColorMatrixElement(const Element* element) { return element && isSVGFEColorMatrixElement(*element); }
template<typename T> inline bool isSVGFEColorMatrixElement(const PassRefPtr<T>& node) { return isSVGFEColorMatrixElement(node.get()); }
template<typename T> inline bool isSVGFEColorMatrixElement(const RefPtr<T>& node) { return isSVGFEColorMatrixElement(node.get()); }
inline bool isSVGFEColorMatrixElement(const Node& node) { return node.isElementNode() ? isSVGFEColorMatrixElement(toElement(node)) : false; }
inline bool isSVGFEColorMatrixElement(const Node* node) { return node && node->isElementNode() ? isSVGFEColorMatrixElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEColorMatrixElement>(const Element& element) { return isSVGFEColorMatrixElement(element); }
class SVGFEComponentTransferElement;
void isSVGFEComponentTransferElement(const SVGFEComponentTransferElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEComponentTransferElement(const SVGFEComponentTransferElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEComponentTransferElement(const Element& element) {
    return element.hasTagName(SVGNames::feComponentTransferTag);
}
inline bool isSVGFEComponentTransferElement(const Element* element) { return element && isSVGFEComponentTransferElement(*element); }
template<typename T> inline bool isSVGFEComponentTransferElement(const PassRefPtr<T>& node) { return isSVGFEComponentTransferElement(node.get()); }
template<typename T> inline bool isSVGFEComponentTransferElement(const RefPtr<T>& node) { return isSVGFEComponentTransferElement(node.get()); }
inline bool isSVGFEComponentTransferElement(const Node& node) { return node.isElementNode() ? isSVGFEComponentTransferElement(toElement(node)) : false; }
inline bool isSVGFEComponentTransferElement(const Node* node) { return node && node->isElementNode() ? isSVGFEComponentTransferElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEComponentTransferElement>(const Element& element) { return isSVGFEComponentTransferElement(element); }
class SVGFECompositeElement;
void isSVGFECompositeElement(const SVGFECompositeElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFECompositeElement(const SVGFECompositeElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFECompositeElement(const Element& element) {
    return element.hasTagName(SVGNames::feCompositeTag);
}
inline bool isSVGFECompositeElement(const Element* element) { return element && isSVGFECompositeElement(*element); }
template<typename T> inline bool isSVGFECompositeElement(const PassRefPtr<T>& node) { return isSVGFECompositeElement(node.get()); }
template<typename T> inline bool isSVGFECompositeElement(const RefPtr<T>& node) { return isSVGFECompositeElement(node.get()); }
inline bool isSVGFECompositeElement(const Node& node) { return node.isElementNode() ? isSVGFECompositeElement(toElement(node)) : false; }
inline bool isSVGFECompositeElement(const Node* node) { return node && node->isElementNode() ? isSVGFECompositeElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFECompositeElement>(const Element& element) { return isSVGFECompositeElement(element); }
class SVGFEConvolveMatrixElement;
void isSVGFEConvolveMatrixElement(const SVGFEConvolveMatrixElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEConvolveMatrixElement(const SVGFEConvolveMatrixElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEConvolveMatrixElement(const Element& element) {
    return element.hasTagName(SVGNames::feConvolveMatrixTag);
}
inline bool isSVGFEConvolveMatrixElement(const Element* element) { return element && isSVGFEConvolveMatrixElement(*element); }
template<typename T> inline bool isSVGFEConvolveMatrixElement(const PassRefPtr<T>& node) { return isSVGFEConvolveMatrixElement(node.get()); }
template<typename T> inline bool isSVGFEConvolveMatrixElement(const RefPtr<T>& node) { return isSVGFEConvolveMatrixElement(node.get()); }
inline bool isSVGFEConvolveMatrixElement(const Node& node) { return node.isElementNode() ? isSVGFEConvolveMatrixElement(toElement(node)) : false; }
inline bool isSVGFEConvolveMatrixElement(const Node* node) { return node && node->isElementNode() ? isSVGFEConvolveMatrixElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEConvolveMatrixElement>(const Element& element) { return isSVGFEConvolveMatrixElement(element); }
class SVGFEDiffuseLightingElement;
void isSVGFEDiffuseLightingElement(const SVGFEDiffuseLightingElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEDiffuseLightingElement(const SVGFEDiffuseLightingElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEDiffuseLightingElement(const Element& element) {
    return element.hasTagName(SVGNames::feDiffuseLightingTag);
}
inline bool isSVGFEDiffuseLightingElement(const Element* element) { return element && isSVGFEDiffuseLightingElement(*element); }
template<typename T> inline bool isSVGFEDiffuseLightingElement(const PassRefPtr<T>& node) { return isSVGFEDiffuseLightingElement(node.get()); }
template<typename T> inline bool isSVGFEDiffuseLightingElement(const RefPtr<T>& node) { return isSVGFEDiffuseLightingElement(node.get()); }
inline bool isSVGFEDiffuseLightingElement(const Node& node) { return node.isElementNode() ? isSVGFEDiffuseLightingElement(toElement(node)) : false; }
inline bool isSVGFEDiffuseLightingElement(const Node* node) { return node && node->isElementNode() ? isSVGFEDiffuseLightingElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEDiffuseLightingElement>(const Element& element) { return isSVGFEDiffuseLightingElement(element); }
class SVGFEDisplacementMapElement;
void isSVGFEDisplacementMapElement(const SVGFEDisplacementMapElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEDisplacementMapElement(const SVGFEDisplacementMapElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEDisplacementMapElement(const Element& element) {
    return element.hasTagName(SVGNames::feDisplacementMapTag);
}
inline bool isSVGFEDisplacementMapElement(const Element* element) { return element && isSVGFEDisplacementMapElement(*element); }
template<typename T> inline bool isSVGFEDisplacementMapElement(const PassRefPtr<T>& node) { return isSVGFEDisplacementMapElement(node.get()); }
template<typename T> inline bool isSVGFEDisplacementMapElement(const RefPtr<T>& node) { return isSVGFEDisplacementMapElement(node.get()); }
inline bool isSVGFEDisplacementMapElement(const Node& node) { return node.isElementNode() ? isSVGFEDisplacementMapElement(toElement(node)) : false; }
inline bool isSVGFEDisplacementMapElement(const Node* node) { return node && node->isElementNode() ? isSVGFEDisplacementMapElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEDisplacementMapElement>(const Element& element) { return isSVGFEDisplacementMapElement(element); }
class SVGFEDistantLightElement;
void isSVGFEDistantLightElement(const SVGFEDistantLightElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEDistantLightElement(const SVGFEDistantLightElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEDistantLightElement(const Element& element) {
    return element.hasTagName(SVGNames::feDistantLightTag);
}
inline bool isSVGFEDistantLightElement(const Element* element) { return element && isSVGFEDistantLightElement(*element); }
template<typename T> inline bool isSVGFEDistantLightElement(const PassRefPtr<T>& node) { return isSVGFEDistantLightElement(node.get()); }
template<typename T> inline bool isSVGFEDistantLightElement(const RefPtr<T>& node) { return isSVGFEDistantLightElement(node.get()); }
inline bool isSVGFEDistantLightElement(const Node& node) { return node.isElementNode() ? isSVGFEDistantLightElement(toElement(node)) : false; }
inline bool isSVGFEDistantLightElement(const Node* node) { return node && node->isElementNode() ? isSVGFEDistantLightElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEDistantLightElement>(const Element& element) { return isSVGFEDistantLightElement(element); }
class SVGFEDropShadowElement;
void isSVGFEDropShadowElement(const SVGFEDropShadowElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEDropShadowElement(const SVGFEDropShadowElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEDropShadowElement(const Element& element) {
    return element.hasTagName(SVGNames::feDropShadowTag);
}
inline bool isSVGFEDropShadowElement(const Element* element) { return element && isSVGFEDropShadowElement(*element); }
template<typename T> inline bool isSVGFEDropShadowElement(const PassRefPtr<T>& node) { return isSVGFEDropShadowElement(node.get()); }
template<typename T> inline bool isSVGFEDropShadowElement(const RefPtr<T>& node) { return isSVGFEDropShadowElement(node.get()); }
inline bool isSVGFEDropShadowElement(const Node& node) { return node.isElementNode() ? isSVGFEDropShadowElement(toElement(node)) : false; }
inline bool isSVGFEDropShadowElement(const Node* node) { return node && node->isElementNode() ? isSVGFEDropShadowElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEDropShadowElement>(const Element& element) { return isSVGFEDropShadowElement(element); }
class SVGFEFloodElement;
void isSVGFEFloodElement(const SVGFEFloodElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEFloodElement(const SVGFEFloodElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEFloodElement(const Element& element) {
    return element.hasTagName(SVGNames::feFloodTag);
}
inline bool isSVGFEFloodElement(const Element* element) { return element && isSVGFEFloodElement(*element); }
template<typename T> inline bool isSVGFEFloodElement(const PassRefPtr<T>& node) { return isSVGFEFloodElement(node.get()); }
template<typename T> inline bool isSVGFEFloodElement(const RefPtr<T>& node) { return isSVGFEFloodElement(node.get()); }
inline bool isSVGFEFloodElement(const Node& node) { return node.isElementNode() ? isSVGFEFloodElement(toElement(node)) : false; }
inline bool isSVGFEFloodElement(const Node* node) { return node && node->isElementNode() ? isSVGFEFloodElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEFloodElement>(const Element& element) { return isSVGFEFloodElement(element); }
class SVGFEFuncAElement;
void isSVGFEFuncAElement(const SVGFEFuncAElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEFuncAElement(const SVGFEFuncAElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEFuncAElement(const Element& element) {
    return element.hasTagName(SVGNames::feFuncATag);
}
inline bool isSVGFEFuncAElement(const Element* element) { return element && isSVGFEFuncAElement(*element); }
template<typename T> inline bool isSVGFEFuncAElement(const PassRefPtr<T>& node) { return isSVGFEFuncAElement(node.get()); }
template<typename T> inline bool isSVGFEFuncAElement(const RefPtr<T>& node) { return isSVGFEFuncAElement(node.get()); }
inline bool isSVGFEFuncAElement(const Node& node) { return node.isElementNode() ? isSVGFEFuncAElement(toElement(node)) : false; }
inline bool isSVGFEFuncAElement(const Node* node) { return node && node->isElementNode() ? isSVGFEFuncAElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEFuncAElement>(const Element& element) { return isSVGFEFuncAElement(element); }
class SVGFEFuncBElement;
void isSVGFEFuncBElement(const SVGFEFuncBElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEFuncBElement(const SVGFEFuncBElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEFuncBElement(const Element& element) {
    return element.hasTagName(SVGNames::feFuncBTag);
}
inline bool isSVGFEFuncBElement(const Element* element) { return element && isSVGFEFuncBElement(*element); }
template<typename T> inline bool isSVGFEFuncBElement(const PassRefPtr<T>& node) { return isSVGFEFuncBElement(node.get()); }
template<typename T> inline bool isSVGFEFuncBElement(const RefPtr<T>& node) { return isSVGFEFuncBElement(node.get()); }
inline bool isSVGFEFuncBElement(const Node& node) { return node.isElementNode() ? isSVGFEFuncBElement(toElement(node)) : false; }
inline bool isSVGFEFuncBElement(const Node* node) { return node && node->isElementNode() ? isSVGFEFuncBElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEFuncBElement>(const Element& element) { return isSVGFEFuncBElement(element); }
class SVGFEFuncGElement;
void isSVGFEFuncGElement(const SVGFEFuncGElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEFuncGElement(const SVGFEFuncGElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEFuncGElement(const Element& element) {
    return element.hasTagName(SVGNames::feFuncGTag);
}
inline bool isSVGFEFuncGElement(const Element* element) { return element && isSVGFEFuncGElement(*element); }
template<typename T> inline bool isSVGFEFuncGElement(const PassRefPtr<T>& node) { return isSVGFEFuncGElement(node.get()); }
template<typename T> inline bool isSVGFEFuncGElement(const RefPtr<T>& node) { return isSVGFEFuncGElement(node.get()); }
inline bool isSVGFEFuncGElement(const Node& node) { return node.isElementNode() ? isSVGFEFuncGElement(toElement(node)) : false; }
inline bool isSVGFEFuncGElement(const Node* node) { return node && node->isElementNode() ? isSVGFEFuncGElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEFuncGElement>(const Element& element) { return isSVGFEFuncGElement(element); }
class SVGFEFuncRElement;
void isSVGFEFuncRElement(const SVGFEFuncRElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEFuncRElement(const SVGFEFuncRElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEFuncRElement(const Element& element) {
    return element.hasTagName(SVGNames::feFuncRTag);
}
inline bool isSVGFEFuncRElement(const Element* element) { return element && isSVGFEFuncRElement(*element); }
template<typename T> inline bool isSVGFEFuncRElement(const PassRefPtr<T>& node) { return isSVGFEFuncRElement(node.get()); }
template<typename T> inline bool isSVGFEFuncRElement(const RefPtr<T>& node) { return isSVGFEFuncRElement(node.get()); }
inline bool isSVGFEFuncRElement(const Node& node) { return node.isElementNode() ? isSVGFEFuncRElement(toElement(node)) : false; }
inline bool isSVGFEFuncRElement(const Node* node) { return node && node->isElementNode() ? isSVGFEFuncRElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEFuncRElement>(const Element& element) { return isSVGFEFuncRElement(element); }
class SVGFEGaussianBlurElement;
void isSVGFEGaussianBlurElement(const SVGFEGaussianBlurElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEGaussianBlurElement(const SVGFEGaussianBlurElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEGaussianBlurElement(const Element& element) {
    return element.hasTagName(SVGNames::feGaussianBlurTag);
}
inline bool isSVGFEGaussianBlurElement(const Element* element) { return element && isSVGFEGaussianBlurElement(*element); }
template<typename T> inline bool isSVGFEGaussianBlurElement(const PassRefPtr<T>& node) { return isSVGFEGaussianBlurElement(node.get()); }
template<typename T> inline bool isSVGFEGaussianBlurElement(const RefPtr<T>& node) { return isSVGFEGaussianBlurElement(node.get()); }
inline bool isSVGFEGaussianBlurElement(const Node& node) { return node.isElementNode() ? isSVGFEGaussianBlurElement(toElement(node)) : false; }
inline bool isSVGFEGaussianBlurElement(const Node* node) { return node && node->isElementNode() ? isSVGFEGaussianBlurElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEGaussianBlurElement>(const Element& element) { return isSVGFEGaussianBlurElement(element); }
class SVGFEImageElement;
void isSVGFEImageElement(const SVGFEImageElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEImageElement(const SVGFEImageElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEImageElement(const Element& element) {
    return element.hasTagName(SVGNames::feImageTag);
}
inline bool isSVGFEImageElement(const Element* element) { return element && isSVGFEImageElement(*element); }
template<typename T> inline bool isSVGFEImageElement(const PassRefPtr<T>& node) { return isSVGFEImageElement(node.get()); }
template<typename T> inline bool isSVGFEImageElement(const RefPtr<T>& node) { return isSVGFEImageElement(node.get()); }
inline bool isSVGFEImageElement(const Node& node) { return node.isElementNode() ? isSVGFEImageElement(toElement(node)) : false; }
inline bool isSVGFEImageElement(const Node* node) { return node && node->isElementNode() ? isSVGFEImageElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEImageElement>(const Element& element) { return isSVGFEImageElement(element); }
class SVGFEMergeElement;
void isSVGFEMergeElement(const SVGFEMergeElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEMergeElement(const SVGFEMergeElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEMergeElement(const Element& element) {
    return element.hasTagName(SVGNames::feMergeTag);
}
inline bool isSVGFEMergeElement(const Element* element) { return element && isSVGFEMergeElement(*element); }
template<typename T> inline bool isSVGFEMergeElement(const PassRefPtr<T>& node) { return isSVGFEMergeElement(node.get()); }
template<typename T> inline bool isSVGFEMergeElement(const RefPtr<T>& node) { return isSVGFEMergeElement(node.get()); }
inline bool isSVGFEMergeElement(const Node& node) { return node.isElementNode() ? isSVGFEMergeElement(toElement(node)) : false; }
inline bool isSVGFEMergeElement(const Node* node) { return node && node->isElementNode() ? isSVGFEMergeElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEMergeElement>(const Element& element) { return isSVGFEMergeElement(element); }
class SVGFEMergeNodeElement;
void isSVGFEMergeNodeElement(const SVGFEMergeNodeElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEMergeNodeElement(const SVGFEMergeNodeElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEMergeNodeElement(const Element& element) {
    return element.hasTagName(SVGNames::feMergeNodeTag);
}
inline bool isSVGFEMergeNodeElement(const Element* element) { return element && isSVGFEMergeNodeElement(*element); }
template<typename T> inline bool isSVGFEMergeNodeElement(const PassRefPtr<T>& node) { return isSVGFEMergeNodeElement(node.get()); }
template<typename T> inline bool isSVGFEMergeNodeElement(const RefPtr<T>& node) { return isSVGFEMergeNodeElement(node.get()); }
inline bool isSVGFEMergeNodeElement(const Node& node) { return node.isElementNode() ? isSVGFEMergeNodeElement(toElement(node)) : false; }
inline bool isSVGFEMergeNodeElement(const Node* node) { return node && node->isElementNode() ? isSVGFEMergeNodeElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEMergeNodeElement>(const Element& element) { return isSVGFEMergeNodeElement(element); }
class SVGFEMorphologyElement;
void isSVGFEMorphologyElement(const SVGFEMorphologyElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEMorphologyElement(const SVGFEMorphologyElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEMorphologyElement(const Element& element) {
    return element.hasTagName(SVGNames::feMorphologyTag);
}
inline bool isSVGFEMorphologyElement(const Element* element) { return element && isSVGFEMorphologyElement(*element); }
template<typename T> inline bool isSVGFEMorphologyElement(const PassRefPtr<T>& node) { return isSVGFEMorphologyElement(node.get()); }
template<typename T> inline bool isSVGFEMorphologyElement(const RefPtr<T>& node) { return isSVGFEMorphologyElement(node.get()); }
inline bool isSVGFEMorphologyElement(const Node& node) { return node.isElementNode() ? isSVGFEMorphologyElement(toElement(node)) : false; }
inline bool isSVGFEMorphologyElement(const Node* node) { return node && node->isElementNode() ? isSVGFEMorphologyElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEMorphologyElement>(const Element& element) { return isSVGFEMorphologyElement(element); }
class SVGFEOffsetElement;
void isSVGFEOffsetElement(const SVGFEOffsetElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEOffsetElement(const SVGFEOffsetElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEOffsetElement(const Element& element) {
    return element.hasTagName(SVGNames::feOffsetTag);
}
inline bool isSVGFEOffsetElement(const Element* element) { return element && isSVGFEOffsetElement(*element); }
template<typename T> inline bool isSVGFEOffsetElement(const PassRefPtr<T>& node) { return isSVGFEOffsetElement(node.get()); }
template<typename T> inline bool isSVGFEOffsetElement(const RefPtr<T>& node) { return isSVGFEOffsetElement(node.get()); }
inline bool isSVGFEOffsetElement(const Node& node) { return node.isElementNode() ? isSVGFEOffsetElement(toElement(node)) : false; }
inline bool isSVGFEOffsetElement(const Node* node) { return node && node->isElementNode() ? isSVGFEOffsetElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEOffsetElement>(const Element& element) { return isSVGFEOffsetElement(element); }
class SVGFEPointLightElement;
void isSVGFEPointLightElement(const SVGFEPointLightElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEPointLightElement(const SVGFEPointLightElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEPointLightElement(const Element& element) {
    return element.hasTagName(SVGNames::fePointLightTag);
}
inline bool isSVGFEPointLightElement(const Element* element) { return element && isSVGFEPointLightElement(*element); }
template<typename T> inline bool isSVGFEPointLightElement(const PassRefPtr<T>& node) { return isSVGFEPointLightElement(node.get()); }
template<typename T> inline bool isSVGFEPointLightElement(const RefPtr<T>& node) { return isSVGFEPointLightElement(node.get()); }
inline bool isSVGFEPointLightElement(const Node& node) { return node.isElementNode() ? isSVGFEPointLightElement(toElement(node)) : false; }
inline bool isSVGFEPointLightElement(const Node* node) { return node && node->isElementNode() ? isSVGFEPointLightElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFEPointLightElement>(const Element& element) { return isSVGFEPointLightElement(element); }
class SVGFESpecularLightingElement;
void isSVGFESpecularLightingElement(const SVGFESpecularLightingElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFESpecularLightingElement(const SVGFESpecularLightingElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFESpecularLightingElement(const Element& element) {
    return element.hasTagName(SVGNames::feSpecularLightingTag);
}
inline bool isSVGFESpecularLightingElement(const Element* element) { return element && isSVGFESpecularLightingElement(*element); }
template<typename T> inline bool isSVGFESpecularLightingElement(const PassRefPtr<T>& node) { return isSVGFESpecularLightingElement(node.get()); }
template<typename T> inline bool isSVGFESpecularLightingElement(const RefPtr<T>& node) { return isSVGFESpecularLightingElement(node.get()); }
inline bool isSVGFESpecularLightingElement(const Node& node) { return node.isElementNode() ? isSVGFESpecularLightingElement(toElement(node)) : false; }
inline bool isSVGFESpecularLightingElement(const Node* node) { return node && node->isElementNode() ? isSVGFESpecularLightingElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFESpecularLightingElement>(const Element& element) { return isSVGFESpecularLightingElement(element); }
class SVGFESpotLightElement;
void isSVGFESpotLightElement(const SVGFESpotLightElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFESpotLightElement(const SVGFESpotLightElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFESpotLightElement(const Element& element) {
    return element.hasTagName(SVGNames::feSpotLightTag);
}
inline bool isSVGFESpotLightElement(const Element* element) { return element && isSVGFESpotLightElement(*element); }
template<typename T> inline bool isSVGFESpotLightElement(const PassRefPtr<T>& node) { return isSVGFESpotLightElement(node.get()); }
template<typename T> inline bool isSVGFESpotLightElement(const RefPtr<T>& node) { return isSVGFESpotLightElement(node.get()); }
inline bool isSVGFESpotLightElement(const Node& node) { return node.isElementNode() ? isSVGFESpotLightElement(toElement(node)) : false; }
inline bool isSVGFESpotLightElement(const Node* node) { return node && node->isElementNode() ? isSVGFESpotLightElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFESpotLightElement>(const Element& element) { return isSVGFESpotLightElement(element); }
class SVGFETileElement;
void isSVGFETileElement(const SVGFETileElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFETileElement(const SVGFETileElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFETileElement(const Element& element) {
    return element.hasTagName(SVGNames::feTileTag);
}
inline bool isSVGFETileElement(const Element* element) { return element && isSVGFETileElement(*element); }
template<typename T> inline bool isSVGFETileElement(const PassRefPtr<T>& node) { return isSVGFETileElement(node.get()); }
template<typename T> inline bool isSVGFETileElement(const RefPtr<T>& node) { return isSVGFETileElement(node.get()); }
inline bool isSVGFETileElement(const Node& node) { return node.isElementNode() ? isSVGFETileElement(toElement(node)) : false; }
inline bool isSVGFETileElement(const Node* node) { return node && node->isElementNode() ? isSVGFETileElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFETileElement>(const Element& element) { return isSVGFETileElement(element); }
class SVGFETurbulenceElement;
void isSVGFETurbulenceElement(const SVGFETurbulenceElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFETurbulenceElement(const SVGFETurbulenceElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFETurbulenceElement(const Element& element) {
    return element.hasTagName(SVGNames::feTurbulenceTag);
}
inline bool isSVGFETurbulenceElement(const Element* element) { return element && isSVGFETurbulenceElement(*element); }
template<typename T> inline bool isSVGFETurbulenceElement(const PassRefPtr<T>& node) { return isSVGFETurbulenceElement(node.get()); }
template<typename T> inline bool isSVGFETurbulenceElement(const RefPtr<T>& node) { return isSVGFETurbulenceElement(node.get()); }
inline bool isSVGFETurbulenceElement(const Node& node) { return node.isElementNode() ? isSVGFETurbulenceElement(toElement(node)) : false; }
inline bool isSVGFETurbulenceElement(const Node* node) { return node && node->isElementNode() ? isSVGFETurbulenceElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFETurbulenceElement>(const Element& element) { return isSVGFETurbulenceElement(element); }
class SVGFilterElement;
void isSVGFilterElement(const SVGFilterElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFilterElement(const SVGFilterElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFilterElement(const Element& element) {
    return element.hasTagName(SVGNames::filterTag);
}
inline bool isSVGFilterElement(const Element* element) { return element && isSVGFilterElement(*element); }
template<typename T> inline bool isSVGFilterElement(const PassRefPtr<T>& node) { return isSVGFilterElement(node.get()); }
template<typename T> inline bool isSVGFilterElement(const RefPtr<T>& node) { return isSVGFilterElement(node.get()); }
inline bool isSVGFilterElement(const Node& node) { return node.isElementNode() ? isSVGFilterElement(toElement(node)) : false; }
inline bool isSVGFilterElement(const Node* node) { return node && node->isElementNode() ? isSVGFilterElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFilterElement>(const Element& element) { return isSVGFilterElement(element); }
class SVGForeignObjectElement;
void isSVGForeignObjectElement(const SVGForeignObjectElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGForeignObjectElement(const SVGForeignObjectElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGForeignObjectElement(const Element& element) {
    return element.hasTagName(SVGNames::foreignObjectTag);
}
inline bool isSVGForeignObjectElement(const Element* element) { return element && isSVGForeignObjectElement(*element); }
template<typename T> inline bool isSVGForeignObjectElement(const PassRefPtr<T>& node) { return isSVGForeignObjectElement(node.get()); }
template<typename T> inline bool isSVGForeignObjectElement(const RefPtr<T>& node) { return isSVGForeignObjectElement(node.get()); }
inline bool isSVGForeignObjectElement(const Node& node) { return node.isElementNode() ? isSVGForeignObjectElement(toElement(node)) : false; }
inline bool isSVGForeignObjectElement(const Node* node) { return node && node->isElementNode() ? isSVGForeignObjectElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGForeignObjectElement>(const Element& element) { return isSVGForeignObjectElement(element); }
class SVGGElement;
void isSVGGElement(const SVGGElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGGElement(const SVGGElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGGElement(const Element& element) {
    return element.hasTagName(SVGNames::gTag);
}
inline bool isSVGGElement(const Element* element) { return element && isSVGGElement(*element); }
template<typename T> inline bool isSVGGElement(const PassRefPtr<T>& node) { return isSVGGElement(node.get()); }
template<typename T> inline bool isSVGGElement(const RefPtr<T>& node) { return isSVGGElement(node.get()); }
inline bool isSVGGElement(const Node& node) { return node.isElementNode() ? isSVGGElement(toElement(node)) : false; }
inline bool isSVGGElement(const Node* node) { return node && node->isElementNode() ? isSVGGElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGGElement>(const Element& element) { return isSVGGElement(element); }
class SVGImageElement;
void isSVGImageElement(const SVGImageElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGImageElement(const SVGImageElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGImageElement(const Element& element) {
    return element.hasTagName(SVGNames::imageTag);
}
inline bool isSVGImageElement(const Element* element) { return element && isSVGImageElement(*element); }
template<typename T> inline bool isSVGImageElement(const PassRefPtr<T>& node) { return isSVGImageElement(node.get()); }
template<typename T> inline bool isSVGImageElement(const RefPtr<T>& node) { return isSVGImageElement(node.get()); }
inline bool isSVGImageElement(const Node& node) { return node.isElementNode() ? isSVGImageElement(toElement(node)) : false; }
inline bool isSVGImageElement(const Node* node) { return node && node->isElementNode() ? isSVGImageElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGImageElement>(const Element& element) { return isSVGImageElement(element); }
class SVGLineElement;
void isSVGLineElement(const SVGLineElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGLineElement(const SVGLineElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGLineElement(const Element& element) {
    return element.hasTagName(SVGNames::lineTag);
}
inline bool isSVGLineElement(const Element* element) { return element && isSVGLineElement(*element); }
template<typename T> inline bool isSVGLineElement(const PassRefPtr<T>& node) { return isSVGLineElement(node.get()); }
template<typename T> inline bool isSVGLineElement(const RefPtr<T>& node) { return isSVGLineElement(node.get()); }
inline bool isSVGLineElement(const Node& node) { return node.isElementNode() ? isSVGLineElement(toElement(node)) : false; }
inline bool isSVGLineElement(const Node* node) { return node && node->isElementNode() ? isSVGLineElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGLineElement>(const Element& element) { return isSVGLineElement(element); }
class SVGLinearGradientElement;
void isSVGLinearGradientElement(const SVGLinearGradientElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGLinearGradientElement(const SVGLinearGradientElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGLinearGradientElement(const Element& element) {
    return element.hasTagName(SVGNames::linearGradientTag);
}
inline bool isSVGLinearGradientElement(const Element* element) { return element && isSVGLinearGradientElement(*element); }
template<typename T> inline bool isSVGLinearGradientElement(const PassRefPtr<T>& node) { return isSVGLinearGradientElement(node.get()); }
template<typename T> inline bool isSVGLinearGradientElement(const RefPtr<T>& node) { return isSVGLinearGradientElement(node.get()); }
inline bool isSVGLinearGradientElement(const Node& node) { return node.isElementNode() ? isSVGLinearGradientElement(toElement(node)) : false; }
inline bool isSVGLinearGradientElement(const Node* node) { return node && node->isElementNode() ? isSVGLinearGradientElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGLinearGradientElement>(const Element& element) { return isSVGLinearGradientElement(element); }
class SVGMPathElement;
void isSVGMPathElement(const SVGMPathElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGMPathElement(const SVGMPathElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGMPathElement(const Element& element) {
    return element.hasTagName(SVGNames::mpathTag);
}
inline bool isSVGMPathElement(const Element* element) { return element && isSVGMPathElement(*element); }
template<typename T> inline bool isSVGMPathElement(const PassRefPtr<T>& node) { return isSVGMPathElement(node.get()); }
template<typename T> inline bool isSVGMPathElement(const RefPtr<T>& node) { return isSVGMPathElement(node.get()); }
inline bool isSVGMPathElement(const Node& node) { return node.isElementNode() ? isSVGMPathElement(toElement(node)) : false; }
inline bool isSVGMPathElement(const Node* node) { return node && node->isElementNode() ? isSVGMPathElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGMPathElement>(const Element& element) { return isSVGMPathElement(element); }
class SVGMarkerElement;
void isSVGMarkerElement(const SVGMarkerElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGMarkerElement(const SVGMarkerElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGMarkerElement(const Element& element) {
    return element.hasTagName(SVGNames::markerTag);
}
inline bool isSVGMarkerElement(const Element* element) { return element && isSVGMarkerElement(*element); }
template<typename T> inline bool isSVGMarkerElement(const PassRefPtr<T>& node) { return isSVGMarkerElement(node.get()); }
template<typename T> inline bool isSVGMarkerElement(const RefPtr<T>& node) { return isSVGMarkerElement(node.get()); }
inline bool isSVGMarkerElement(const Node& node) { return node.isElementNode() ? isSVGMarkerElement(toElement(node)) : false; }
inline bool isSVGMarkerElement(const Node* node) { return node && node->isElementNode() ? isSVGMarkerElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGMarkerElement>(const Element& element) { return isSVGMarkerElement(element); }
class SVGMaskElement;
void isSVGMaskElement(const SVGMaskElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGMaskElement(const SVGMaskElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGMaskElement(const Element& element) {
    return element.hasTagName(SVGNames::maskTag);
}
inline bool isSVGMaskElement(const Element* element) { return element && isSVGMaskElement(*element); }
template<typename T> inline bool isSVGMaskElement(const PassRefPtr<T>& node) { return isSVGMaskElement(node.get()); }
template<typename T> inline bool isSVGMaskElement(const RefPtr<T>& node) { return isSVGMaskElement(node.get()); }
inline bool isSVGMaskElement(const Node& node) { return node.isElementNode() ? isSVGMaskElement(toElement(node)) : false; }
inline bool isSVGMaskElement(const Node* node) { return node && node->isElementNode() ? isSVGMaskElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGMaskElement>(const Element& element) { return isSVGMaskElement(element); }
class SVGMetadataElement;
void isSVGMetadataElement(const SVGMetadataElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGMetadataElement(const SVGMetadataElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGMetadataElement(const Element& element) {
    return element.hasTagName(SVGNames::metadataTag);
}
inline bool isSVGMetadataElement(const Element* element) { return element && isSVGMetadataElement(*element); }
template<typename T> inline bool isSVGMetadataElement(const PassRefPtr<T>& node) { return isSVGMetadataElement(node.get()); }
template<typename T> inline bool isSVGMetadataElement(const RefPtr<T>& node) { return isSVGMetadataElement(node.get()); }
inline bool isSVGMetadataElement(const Node& node) { return node.isElementNode() ? isSVGMetadataElement(toElement(node)) : false; }
inline bool isSVGMetadataElement(const Node* node) { return node && node->isElementNode() ? isSVGMetadataElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGMetadataElement>(const Element& element) { return isSVGMetadataElement(element); }
class SVGPathElement;
void isSVGPathElement(const SVGPathElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGPathElement(const SVGPathElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGPathElement(const Element& element) {
    return element.hasTagName(SVGNames::pathTag);
}
inline bool isSVGPathElement(const Element* element) { return element && isSVGPathElement(*element); }
template<typename T> inline bool isSVGPathElement(const PassRefPtr<T>& node) { return isSVGPathElement(node.get()); }
template<typename T> inline bool isSVGPathElement(const RefPtr<T>& node) { return isSVGPathElement(node.get()); }
inline bool isSVGPathElement(const Node& node) { return node.isElementNode() ? isSVGPathElement(toElement(node)) : false; }
inline bool isSVGPathElement(const Node* node) { return node && node->isElementNode() ? isSVGPathElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGPathElement>(const Element& element) { return isSVGPathElement(element); }
class SVGPatternElement;
void isSVGPatternElement(const SVGPatternElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGPatternElement(const SVGPatternElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGPatternElement(const Element& element) {
    return element.hasTagName(SVGNames::patternTag);
}
inline bool isSVGPatternElement(const Element* element) { return element && isSVGPatternElement(*element); }
template<typename T> inline bool isSVGPatternElement(const PassRefPtr<T>& node) { return isSVGPatternElement(node.get()); }
template<typename T> inline bool isSVGPatternElement(const RefPtr<T>& node) { return isSVGPatternElement(node.get()); }
inline bool isSVGPatternElement(const Node& node) { return node.isElementNode() ? isSVGPatternElement(toElement(node)) : false; }
inline bool isSVGPatternElement(const Node* node) { return node && node->isElementNode() ? isSVGPatternElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGPatternElement>(const Element& element) { return isSVGPatternElement(element); }
class SVGPolygonElement;
void isSVGPolygonElement(const SVGPolygonElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGPolygonElement(const SVGPolygonElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGPolygonElement(const Element& element) {
    return element.hasTagName(SVGNames::polygonTag);
}
inline bool isSVGPolygonElement(const Element* element) { return element && isSVGPolygonElement(*element); }
template<typename T> inline bool isSVGPolygonElement(const PassRefPtr<T>& node) { return isSVGPolygonElement(node.get()); }
template<typename T> inline bool isSVGPolygonElement(const RefPtr<T>& node) { return isSVGPolygonElement(node.get()); }
inline bool isSVGPolygonElement(const Node& node) { return node.isElementNode() ? isSVGPolygonElement(toElement(node)) : false; }
inline bool isSVGPolygonElement(const Node* node) { return node && node->isElementNode() ? isSVGPolygonElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGPolygonElement>(const Element& element) { return isSVGPolygonElement(element); }
class SVGPolylineElement;
void isSVGPolylineElement(const SVGPolylineElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGPolylineElement(const SVGPolylineElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGPolylineElement(const Element& element) {
    return element.hasTagName(SVGNames::polylineTag);
}
inline bool isSVGPolylineElement(const Element* element) { return element && isSVGPolylineElement(*element); }
template<typename T> inline bool isSVGPolylineElement(const PassRefPtr<T>& node) { return isSVGPolylineElement(node.get()); }
template<typename T> inline bool isSVGPolylineElement(const RefPtr<T>& node) { return isSVGPolylineElement(node.get()); }
inline bool isSVGPolylineElement(const Node& node) { return node.isElementNode() ? isSVGPolylineElement(toElement(node)) : false; }
inline bool isSVGPolylineElement(const Node* node) { return node && node->isElementNode() ? isSVGPolylineElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGPolylineElement>(const Element& element) { return isSVGPolylineElement(element); }
class SVGRadialGradientElement;
void isSVGRadialGradientElement(const SVGRadialGradientElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGRadialGradientElement(const SVGRadialGradientElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGRadialGradientElement(const Element& element) {
    return element.hasTagName(SVGNames::radialGradientTag);
}
inline bool isSVGRadialGradientElement(const Element* element) { return element && isSVGRadialGradientElement(*element); }
template<typename T> inline bool isSVGRadialGradientElement(const PassRefPtr<T>& node) { return isSVGRadialGradientElement(node.get()); }
template<typename T> inline bool isSVGRadialGradientElement(const RefPtr<T>& node) { return isSVGRadialGradientElement(node.get()); }
inline bool isSVGRadialGradientElement(const Node& node) { return node.isElementNode() ? isSVGRadialGradientElement(toElement(node)) : false; }
inline bool isSVGRadialGradientElement(const Node* node) { return node && node->isElementNode() ? isSVGRadialGradientElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGRadialGradientElement>(const Element& element) { return isSVGRadialGradientElement(element); }
class SVGRectElement;
void isSVGRectElement(const SVGRectElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGRectElement(const SVGRectElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGRectElement(const Element& element) {
    return element.hasTagName(SVGNames::rectTag);
}
inline bool isSVGRectElement(const Element* element) { return element && isSVGRectElement(*element); }
template<typename T> inline bool isSVGRectElement(const PassRefPtr<T>& node) { return isSVGRectElement(node.get()); }
template<typename T> inline bool isSVGRectElement(const RefPtr<T>& node) { return isSVGRectElement(node.get()); }
inline bool isSVGRectElement(const Node& node) { return node.isElementNode() ? isSVGRectElement(toElement(node)) : false; }
inline bool isSVGRectElement(const Node* node) { return node && node->isElementNode() ? isSVGRectElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGRectElement>(const Element& element) { return isSVGRectElement(element); }
class SVGSVGElement;
void isSVGSVGElement(const SVGSVGElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGSVGElement(const SVGSVGElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGSVGElement(const Element& element) {
    return element.hasTagName(SVGNames::svgTag);
}
inline bool isSVGSVGElement(const Element* element) { return element && isSVGSVGElement(*element); }
template<typename T> inline bool isSVGSVGElement(const PassRefPtr<T>& node) { return isSVGSVGElement(node.get()); }
template<typename T> inline bool isSVGSVGElement(const RefPtr<T>& node) { return isSVGSVGElement(node.get()); }
inline bool isSVGSVGElement(const Node& node) { return node.isElementNode() ? isSVGSVGElement(toElement(node)) : false; }
inline bool isSVGSVGElement(const Node* node) { return node && node->isElementNode() ? isSVGSVGElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGSVGElement>(const Element& element) { return isSVGSVGElement(element); }
class SVGSetElement;
void isSVGSetElement(const SVGSetElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGSetElement(const SVGSetElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGSetElement(const Element& element) {
    return element.hasTagName(SVGNames::setTag);
}
inline bool isSVGSetElement(const Element* element) { return element && isSVGSetElement(*element); }
template<typename T> inline bool isSVGSetElement(const PassRefPtr<T>& node) { return isSVGSetElement(node.get()); }
template<typename T> inline bool isSVGSetElement(const RefPtr<T>& node) { return isSVGSetElement(node.get()); }
inline bool isSVGSetElement(const Node& node) { return node.isElementNode() ? isSVGSetElement(toElement(node)) : false; }
inline bool isSVGSetElement(const Node* node) { return node && node->isElementNode() ? isSVGSetElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGSetElement>(const Element& element) { return isSVGSetElement(element); }
class SVGStopElement;
void isSVGStopElement(const SVGStopElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGStopElement(const SVGStopElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGStopElement(const Element& element) {
    return element.hasTagName(SVGNames::stopTag);
}
inline bool isSVGStopElement(const Element* element) { return element && isSVGStopElement(*element); }
template<typename T> inline bool isSVGStopElement(const PassRefPtr<T>& node) { return isSVGStopElement(node.get()); }
template<typename T> inline bool isSVGStopElement(const RefPtr<T>& node) { return isSVGStopElement(node.get()); }
inline bool isSVGStopElement(const Node& node) { return node.isElementNode() ? isSVGStopElement(toElement(node)) : false; }
inline bool isSVGStopElement(const Node* node) { return node && node->isElementNode() ? isSVGStopElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGStopElement>(const Element& element) { return isSVGStopElement(element); }
class SVGSwitchElement;
void isSVGSwitchElement(const SVGSwitchElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGSwitchElement(const SVGSwitchElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGSwitchElement(const Element& element) {
    return element.hasTagName(SVGNames::switchTag);
}
inline bool isSVGSwitchElement(const Element* element) { return element && isSVGSwitchElement(*element); }
template<typename T> inline bool isSVGSwitchElement(const PassRefPtr<T>& node) { return isSVGSwitchElement(node.get()); }
template<typename T> inline bool isSVGSwitchElement(const RefPtr<T>& node) { return isSVGSwitchElement(node.get()); }
inline bool isSVGSwitchElement(const Node& node) { return node.isElementNode() ? isSVGSwitchElement(toElement(node)) : false; }
inline bool isSVGSwitchElement(const Node* node) { return node && node->isElementNode() ? isSVGSwitchElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGSwitchElement>(const Element& element) { return isSVGSwitchElement(element); }
class SVGSymbolElement;
void isSVGSymbolElement(const SVGSymbolElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGSymbolElement(const SVGSymbolElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGSymbolElement(const Element& element) {
    return element.hasTagName(SVGNames::symbolTag);
}
inline bool isSVGSymbolElement(const Element* element) { return element && isSVGSymbolElement(*element); }
template<typename T> inline bool isSVGSymbolElement(const PassRefPtr<T>& node) { return isSVGSymbolElement(node.get()); }
template<typename T> inline bool isSVGSymbolElement(const RefPtr<T>& node) { return isSVGSymbolElement(node.get()); }
inline bool isSVGSymbolElement(const Node& node) { return node.isElementNode() ? isSVGSymbolElement(toElement(node)) : false; }
inline bool isSVGSymbolElement(const Node* node) { return node && node->isElementNode() ? isSVGSymbolElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGSymbolElement>(const Element& element) { return isSVGSymbolElement(element); }
class SVGTSpanElement;
void isSVGTSpanElement(const SVGTSpanElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGTSpanElement(const SVGTSpanElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGTSpanElement(const Element& element) {
    return element.hasTagName(SVGNames::tspanTag);
}
inline bool isSVGTSpanElement(const Element* element) { return element && isSVGTSpanElement(*element); }
template<typename T> inline bool isSVGTSpanElement(const PassRefPtr<T>& node) { return isSVGTSpanElement(node.get()); }
template<typename T> inline bool isSVGTSpanElement(const RefPtr<T>& node) { return isSVGTSpanElement(node.get()); }
inline bool isSVGTSpanElement(const Node& node) { return node.isElementNode() ? isSVGTSpanElement(toElement(node)) : false; }
inline bool isSVGTSpanElement(const Node* node) { return node && node->isElementNode() ? isSVGTSpanElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGTSpanElement>(const Element& element) { return isSVGTSpanElement(element); }
class SVGTextElement;
void isSVGTextElement(const SVGTextElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGTextElement(const SVGTextElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGTextElement(const Element& element) {
    return element.hasTagName(SVGNames::textTag);
}
inline bool isSVGTextElement(const Element* element) { return element && isSVGTextElement(*element); }
template<typename T> inline bool isSVGTextElement(const PassRefPtr<T>& node) { return isSVGTextElement(node.get()); }
template<typename T> inline bool isSVGTextElement(const RefPtr<T>& node) { return isSVGTextElement(node.get()); }
inline bool isSVGTextElement(const Node& node) { return node.isElementNode() ? isSVGTextElement(toElement(node)) : false; }
inline bool isSVGTextElement(const Node* node) { return node && node->isElementNode() ? isSVGTextElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGTextElement>(const Element& element) { return isSVGTextElement(element); }
class SVGTextPathElement;
void isSVGTextPathElement(const SVGTextPathElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGTextPathElement(const SVGTextPathElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGTextPathElement(const Element& element) {
    return element.hasTagName(SVGNames::textPathTag);
}
inline bool isSVGTextPathElement(const Element* element) { return element && isSVGTextPathElement(*element); }
template<typename T> inline bool isSVGTextPathElement(const PassRefPtr<T>& node) { return isSVGTextPathElement(node.get()); }
template<typename T> inline bool isSVGTextPathElement(const RefPtr<T>& node) { return isSVGTextPathElement(node.get()); }
inline bool isSVGTextPathElement(const Node& node) { return node.isElementNode() ? isSVGTextPathElement(toElement(node)) : false; }
inline bool isSVGTextPathElement(const Node* node) { return node && node->isElementNode() ? isSVGTextPathElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGTextPathElement>(const Element& element) { return isSVGTextPathElement(element); }
class SVGTitleElement;
void isSVGTitleElement(const SVGTitleElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGTitleElement(const SVGTitleElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGTitleElement(const Element& element) {
    return element.hasTagName(SVGNames::titleTag);
}
inline bool isSVGTitleElement(const Element* element) { return element && isSVGTitleElement(*element); }
template<typename T> inline bool isSVGTitleElement(const PassRefPtr<T>& node) { return isSVGTitleElement(node.get()); }
template<typename T> inline bool isSVGTitleElement(const RefPtr<T>& node) { return isSVGTitleElement(node.get()); }
inline bool isSVGTitleElement(const Node& node) { return node.isElementNode() ? isSVGTitleElement(toElement(node)) : false; }
inline bool isSVGTitleElement(const Node* node) { return node && node->isElementNode() ? isSVGTitleElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGTitleElement>(const Element& element) { return isSVGTitleElement(element); }
class SVGUseElement;
void isSVGUseElement(const SVGUseElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGUseElement(const SVGUseElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGUseElement(const Element& element) {
    return element.hasTagName(SVGNames::useTag);
}
inline bool isSVGUseElement(const Element* element) { return element && isSVGUseElement(*element); }
template<typename T> inline bool isSVGUseElement(const PassRefPtr<T>& node) { return isSVGUseElement(node.get()); }
template<typename T> inline bool isSVGUseElement(const RefPtr<T>& node) { return isSVGUseElement(node.get()); }
inline bool isSVGUseElement(const Node& node) { return node.isElementNode() ? isSVGUseElement(toElement(node)) : false; }
inline bool isSVGUseElement(const Node* node) { return node && node->isElementNode() ? isSVGUseElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGUseElement>(const Element& element) { return isSVGUseElement(element); }
class SVGViewElement;
void isSVGViewElement(const SVGViewElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGViewElement(const SVGViewElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGViewElement(const Element& element) {
    return element.hasTagName(SVGNames::viewTag);
}
inline bool isSVGViewElement(const Element* element) { return element && isSVGViewElement(*element); }
template<typename T> inline bool isSVGViewElement(const PassRefPtr<T>& node) { return isSVGViewElement(node.get()); }
template<typename T> inline bool isSVGViewElement(const RefPtr<T>& node) { return isSVGViewElement(node.get()); }
inline bool isSVGViewElement(const Node& node) { return node.isElementNode() ? isSVGViewElement(toElement(node)) : false; }
inline bool isSVGViewElement(const Node* node) { return node && node->isElementNode() ? isSVGViewElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGViewElement>(const Element& element) { return isSVGViewElement(element); }
class SVGScriptElement;
void isSVGScriptElement(const SVGScriptElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGScriptElement(const SVGScriptElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGScriptElement(const Element& element) {
    return element.hasTagName(SVGNames::scriptTag);
}
inline bool isSVGScriptElement(const Element* element) { return element && isSVGScriptElement(*element); }
template<typename T> inline bool isSVGScriptElement(const PassRefPtr<T>& node) { return isSVGScriptElement(node.get()); }
template<typename T> inline bool isSVGScriptElement(const RefPtr<T>& node) { return isSVGScriptElement(node.get()); }
inline bool isSVGScriptElement(const Node& node) { return node.isElementNode() ? isSVGScriptElement(toElement(node)) : false; }
inline bool isSVGScriptElement(const Node* node) { return node && node->isElementNode() ? isSVGScriptElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGScriptElement>(const Element& element) { return isSVGScriptElement(element); }
class SVGStyleElement;
void isSVGStyleElement(const SVGStyleElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGStyleElement(const SVGStyleElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGStyleElement(const Element& element) {
    return element.hasTagName(SVGNames::styleTag);
}
inline bool isSVGStyleElement(const Element* element) { return element && isSVGStyleElement(*element); }
template<typename T> inline bool isSVGStyleElement(const PassRefPtr<T>& node) { return isSVGStyleElement(node.get()); }
template<typename T> inline bool isSVGStyleElement(const RefPtr<T>& node) { return isSVGStyleElement(node.get()); }
inline bool isSVGStyleElement(const Node& node) { return node.isElementNode() ? isSVGStyleElement(toElement(node)) : false; }
inline bool isSVGStyleElement(const Node* node) { return node && node->isElementNode() ? isSVGStyleElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGStyleElement>(const Element& element) { return isSVGStyleElement(element); }
#if ENABLE(SVG_FONTS)
class SVGAltGlyphDefElement;
void isSVGAltGlyphDefElement(const SVGAltGlyphDefElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGAltGlyphDefElement(const SVGAltGlyphDefElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGAltGlyphDefElement(const Element& element) {
    return element.hasTagName(SVGNames::altGlyphDefTag);
}
inline bool isSVGAltGlyphDefElement(const Element* element) { return element && isSVGAltGlyphDefElement(*element); }
template<typename T> inline bool isSVGAltGlyphDefElement(const PassRefPtr<T>& node) { return isSVGAltGlyphDefElement(node.get()); }
template<typename T> inline bool isSVGAltGlyphDefElement(const RefPtr<T>& node) { return isSVGAltGlyphDefElement(node.get()); }
inline bool isSVGAltGlyphDefElement(const Node& node) { return node.isElementNode() ? isSVGAltGlyphDefElement(toElement(node)) : false; }
inline bool isSVGAltGlyphDefElement(const Node* node) { return node && node->isElementNode() ? isSVGAltGlyphDefElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGAltGlyphDefElement>(const Element& element) { return isSVGAltGlyphDefElement(element); }
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
class SVGAltGlyphElement;
void isSVGAltGlyphElement(const SVGAltGlyphElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGAltGlyphElement(const SVGAltGlyphElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGAltGlyphElement(const Element& element) {
    return element.hasTagName(SVGNames::altGlyphTag);
}
inline bool isSVGAltGlyphElement(const Element* element) { return element && isSVGAltGlyphElement(*element); }
template<typename T> inline bool isSVGAltGlyphElement(const PassRefPtr<T>& node) { return isSVGAltGlyphElement(node.get()); }
template<typename T> inline bool isSVGAltGlyphElement(const RefPtr<T>& node) { return isSVGAltGlyphElement(node.get()); }
inline bool isSVGAltGlyphElement(const Node& node) { return node.isElementNode() ? isSVGAltGlyphElement(toElement(node)) : false; }
inline bool isSVGAltGlyphElement(const Node* node) { return node && node->isElementNode() ? isSVGAltGlyphElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGAltGlyphElement>(const Element& element) { return isSVGAltGlyphElement(element); }
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
class SVGAltGlyphItemElement;
void isSVGAltGlyphItemElement(const SVGAltGlyphItemElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGAltGlyphItemElement(const SVGAltGlyphItemElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGAltGlyphItemElement(const Element& element) {
    return element.hasTagName(SVGNames::altGlyphItemTag);
}
inline bool isSVGAltGlyphItemElement(const Element* element) { return element && isSVGAltGlyphItemElement(*element); }
template<typename T> inline bool isSVGAltGlyphItemElement(const PassRefPtr<T>& node) { return isSVGAltGlyphItemElement(node.get()); }
template<typename T> inline bool isSVGAltGlyphItemElement(const RefPtr<T>& node) { return isSVGAltGlyphItemElement(node.get()); }
inline bool isSVGAltGlyphItemElement(const Node& node) { return node.isElementNode() ? isSVGAltGlyphItemElement(toElement(node)) : false; }
inline bool isSVGAltGlyphItemElement(const Node* node) { return node && node->isElementNode() ? isSVGAltGlyphItemElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGAltGlyphItemElement>(const Element& element) { return isSVGAltGlyphItemElement(element); }
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
class SVGFontElement;
void isSVGFontElement(const SVGFontElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFontElement(const SVGFontElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFontElement(const Element& element) {
    return element.hasTagName(SVGNames::fontTag);
}
inline bool isSVGFontElement(const Element* element) { return element && isSVGFontElement(*element); }
template<typename T> inline bool isSVGFontElement(const PassRefPtr<T>& node) { return isSVGFontElement(node.get()); }
template<typename T> inline bool isSVGFontElement(const RefPtr<T>& node) { return isSVGFontElement(node.get()); }
inline bool isSVGFontElement(const Node& node) { return node.isElementNode() ? isSVGFontElement(toElement(node)) : false; }
inline bool isSVGFontElement(const Node* node) { return node && node->isElementNode() ? isSVGFontElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFontElement>(const Element& element) { return isSVGFontElement(element); }
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
class SVGFontFaceElement;
void isSVGFontFaceElement(const SVGFontFaceElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFontFaceElement(const SVGFontFaceElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFontFaceElement(const Element& element) {
    return element.hasTagName(SVGNames::font_faceTag);
}
inline bool isSVGFontFaceElement(const Element* element) { return element && isSVGFontFaceElement(*element); }
template<typename T> inline bool isSVGFontFaceElement(const PassRefPtr<T>& node) { return isSVGFontFaceElement(node.get()); }
template<typename T> inline bool isSVGFontFaceElement(const RefPtr<T>& node) { return isSVGFontFaceElement(node.get()); }
inline bool isSVGFontFaceElement(const Node& node) { return node.isElementNode() ? isSVGFontFaceElement(toElement(node)) : false; }
inline bool isSVGFontFaceElement(const Node* node) { return node && node->isElementNode() ? isSVGFontFaceElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFontFaceElement>(const Element& element) { return isSVGFontFaceElement(element); }
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
class SVGFontFaceFormatElement;
void isSVGFontFaceFormatElement(const SVGFontFaceFormatElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFontFaceFormatElement(const SVGFontFaceFormatElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFontFaceFormatElement(const Element& element) {
    return element.hasTagName(SVGNames::font_face_formatTag);
}
inline bool isSVGFontFaceFormatElement(const Element* element) { return element && isSVGFontFaceFormatElement(*element); }
template<typename T> inline bool isSVGFontFaceFormatElement(const PassRefPtr<T>& node) { return isSVGFontFaceFormatElement(node.get()); }
template<typename T> inline bool isSVGFontFaceFormatElement(const RefPtr<T>& node) { return isSVGFontFaceFormatElement(node.get()); }
inline bool isSVGFontFaceFormatElement(const Node& node) { return node.isElementNode() ? isSVGFontFaceFormatElement(toElement(node)) : false; }
inline bool isSVGFontFaceFormatElement(const Node* node) { return node && node->isElementNode() ? isSVGFontFaceFormatElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFontFaceFormatElement>(const Element& element) { return isSVGFontFaceFormatElement(element); }
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
class SVGFontFaceNameElement;
void isSVGFontFaceNameElement(const SVGFontFaceNameElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFontFaceNameElement(const SVGFontFaceNameElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFontFaceNameElement(const Element& element) {
    return element.hasTagName(SVGNames::font_face_nameTag);
}
inline bool isSVGFontFaceNameElement(const Element* element) { return element && isSVGFontFaceNameElement(*element); }
template<typename T> inline bool isSVGFontFaceNameElement(const PassRefPtr<T>& node) { return isSVGFontFaceNameElement(node.get()); }
template<typename T> inline bool isSVGFontFaceNameElement(const RefPtr<T>& node) { return isSVGFontFaceNameElement(node.get()); }
inline bool isSVGFontFaceNameElement(const Node& node) { return node.isElementNode() ? isSVGFontFaceNameElement(toElement(node)) : false; }
inline bool isSVGFontFaceNameElement(const Node* node) { return node && node->isElementNode() ? isSVGFontFaceNameElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFontFaceNameElement>(const Element& element) { return isSVGFontFaceNameElement(element); }
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
class SVGFontFaceSrcElement;
void isSVGFontFaceSrcElement(const SVGFontFaceSrcElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFontFaceSrcElement(const SVGFontFaceSrcElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFontFaceSrcElement(const Element& element) {
    return element.hasTagName(SVGNames::font_face_srcTag);
}
inline bool isSVGFontFaceSrcElement(const Element* element) { return element && isSVGFontFaceSrcElement(*element); }
template<typename T> inline bool isSVGFontFaceSrcElement(const PassRefPtr<T>& node) { return isSVGFontFaceSrcElement(node.get()); }
template<typename T> inline bool isSVGFontFaceSrcElement(const RefPtr<T>& node) { return isSVGFontFaceSrcElement(node.get()); }
inline bool isSVGFontFaceSrcElement(const Node& node) { return node.isElementNode() ? isSVGFontFaceSrcElement(toElement(node)) : false; }
inline bool isSVGFontFaceSrcElement(const Node* node) { return node && node->isElementNode() ? isSVGFontFaceSrcElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFontFaceSrcElement>(const Element& element) { return isSVGFontFaceSrcElement(element); }
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
class SVGFontFaceUriElement;
void isSVGFontFaceUriElement(const SVGFontFaceUriElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFontFaceUriElement(const SVGFontFaceUriElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFontFaceUriElement(const Element& element) {
    return element.hasTagName(SVGNames::font_face_uriTag);
}
inline bool isSVGFontFaceUriElement(const Element* element) { return element && isSVGFontFaceUriElement(*element); }
template<typename T> inline bool isSVGFontFaceUriElement(const PassRefPtr<T>& node) { return isSVGFontFaceUriElement(node.get()); }
template<typename T> inline bool isSVGFontFaceUriElement(const RefPtr<T>& node) { return isSVGFontFaceUriElement(node.get()); }
inline bool isSVGFontFaceUriElement(const Node& node) { return node.isElementNode() ? isSVGFontFaceUriElement(toElement(node)) : false; }
inline bool isSVGFontFaceUriElement(const Node* node) { return node && node->isElementNode() ? isSVGFontFaceUriElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGFontFaceUriElement>(const Element& element) { return isSVGFontFaceUriElement(element); }
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
class SVGGlyphElement;
void isSVGGlyphElement(const SVGGlyphElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGGlyphElement(const SVGGlyphElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGGlyphElement(const Element& element) {
    return element.hasTagName(SVGNames::glyphTag);
}
inline bool isSVGGlyphElement(const Element* element) { return element && isSVGGlyphElement(*element); }
template<typename T> inline bool isSVGGlyphElement(const PassRefPtr<T>& node) { return isSVGGlyphElement(node.get()); }
template<typename T> inline bool isSVGGlyphElement(const RefPtr<T>& node) { return isSVGGlyphElement(node.get()); }
inline bool isSVGGlyphElement(const Node& node) { return node.isElementNode() ? isSVGGlyphElement(toElement(node)) : false; }
inline bool isSVGGlyphElement(const Node* node) { return node && node->isElementNode() ? isSVGGlyphElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGGlyphElement>(const Element& element) { return isSVGGlyphElement(element); }
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
class SVGGlyphRefElement;
void isSVGGlyphRefElement(const SVGGlyphRefElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGGlyphRefElement(const SVGGlyphRefElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGGlyphRefElement(const Element& element) {
    return element.hasTagName(SVGNames::glyphRefTag);
}
inline bool isSVGGlyphRefElement(const Element* element) { return element && isSVGGlyphRefElement(*element); }
template<typename T> inline bool isSVGGlyphRefElement(const PassRefPtr<T>& node) { return isSVGGlyphRefElement(node.get()); }
template<typename T> inline bool isSVGGlyphRefElement(const RefPtr<T>& node) { return isSVGGlyphRefElement(node.get()); }
inline bool isSVGGlyphRefElement(const Node& node) { return node.isElementNode() ? isSVGGlyphRefElement(toElement(node)) : false; }
inline bool isSVGGlyphRefElement(const Node* node) { return node && node->isElementNode() ? isSVGGlyphRefElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGGlyphRefElement>(const Element& element) { return isSVGGlyphRefElement(element); }
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
class SVGHKernElement;
void isSVGHKernElement(const SVGHKernElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGHKernElement(const SVGHKernElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGHKernElement(const Element& element) {
    return element.hasTagName(SVGNames::hkernTag);
}
inline bool isSVGHKernElement(const Element* element) { return element && isSVGHKernElement(*element); }
template<typename T> inline bool isSVGHKernElement(const PassRefPtr<T>& node) { return isSVGHKernElement(node.get()); }
template<typename T> inline bool isSVGHKernElement(const RefPtr<T>& node) { return isSVGHKernElement(node.get()); }
inline bool isSVGHKernElement(const Node& node) { return node.isElementNode() ? isSVGHKernElement(toElement(node)) : false; }
inline bool isSVGHKernElement(const Node* node) { return node && node->isElementNode() ? isSVGHKernElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGHKernElement>(const Element& element) { return isSVGHKernElement(element); }
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
class SVGMissingGlyphElement;
void isSVGMissingGlyphElement(const SVGMissingGlyphElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGMissingGlyphElement(const SVGMissingGlyphElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGMissingGlyphElement(const Element& element) {
    return element.hasTagName(SVGNames::missing_glyphTag);
}
inline bool isSVGMissingGlyphElement(const Element* element) { return element && isSVGMissingGlyphElement(*element); }
template<typename T> inline bool isSVGMissingGlyphElement(const PassRefPtr<T>& node) { return isSVGMissingGlyphElement(node.get()); }
template<typename T> inline bool isSVGMissingGlyphElement(const RefPtr<T>& node) { return isSVGMissingGlyphElement(node.get()); }
inline bool isSVGMissingGlyphElement(const Node& node) { return node.isElementNode() ? isSVGMissingGlyphElement(toElement(node)) : false; }
inline bool isSVGMissingGlyphElement(const Node* node) { return node && node->isElementNode() ? isSVGMissingGlyphElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGMissingGlyphElement>(const Element& element) { return isSVGMissingGlyphElement(element); }
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
class SVGVKernElement;
void isSVGVKernElement(const SVGVKernElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGVKernElement(const SVGVKernElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGVKernElement(const Element& element) {
    return element.hasTagName(SVGNames::vkernTag);
}
inline bool isSVGVKernElement(const Element* element) { return element && isSVGVKernElement(*element); }
template<typename T> inline bool isSVGVKernElement(const PassRefPtr<T>& node) { return isSVGVKernElement(node.get()); }
template<typename T> inline bool isSVGVKernElement(const RefPtr<T>& node) { return isSVGVKernElement(node.get()); }
inline bool isSVGVKernElement(const Node& node) { return node.isElementNode() ? isSVGVKernElement(toElement(node)) : false; }
inline bool isSVGVKernElement(const Node* node) { return node && node->isElementNode() ? isSVGVKernElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const SVGVKernElement>(const Element& element) { return isSVGVKernElement(element); }
#endif // ENABLE(SVG_FONTS)
// Using macros because the types are forward-declared and we don't want to use reinterpret_cast in the
// casting functions above. reinterpret_cast would be unsafe due to multiple inheritence.

#define toSVGAElement(x) WebCore::toElement<WebCore::SVGAElement>(x)
#define toSVGAnimateMotionElement(x) WebCore::toElement<WebCore::SVGAnimateMotionElement>(x)
#define toSVGAnimateTransformElement(x) WebCore::toElement<WebCore::SVGAnimateTransformElement>(x)
#define toSVGCircleElement(x) WebCore::toElement<WebCore::SVGCircleElement>(x)
#define toSVGClipPathElement(x) WebCore::toElement<WebCore::SVGClipPathElement>(x)
#define toSVGCursorElement(x) WebCore::toElement<WebCore::SVGCursorElement>(x)
#define toSVGDefsElement(x) WebCore::toElement<WebCore::SVGDefsElement>(x)
#define toSVGDescElement(x) WebCore::toElement<WebCore::SVGDescElement>(x)
#define toSVGDiscardElement(x) WebCore::toElement<WebCore::SVGDiscardElement>(x)
#define toSVGEllipseElement(x) WebCore::toElement<WebCore::SVGEllipseElement>(x)
#define toSVGFEBlendElement(x) WebCore::toElement<WebCore::SVGFEBlendElement>(x)
#define toSVGFEColorMatrixElement(x) WebCore::toElement<WebCore::SVGFEColorMatrixElement>(x)
#define toSVGFEComponentTransferElement(x) WebCore::toElement<WebCore::SVGFEComponentTransferElement>(x)
#define toSVGFECompositeElement(x) WebCore::toElement<WebCore::SVGFECompositeElement>(x)
#define toSVGFEConvolveMatrixElement(x) WebCore::toElement<WebCore::SVGFEConvolveMatrixElement>(x)
#define toSVGFEDiffuseLightingElement(x) WebCore::toElement<WebCore::SVGFEDiffuseLightingElement>(x)
#define toSVGFEDisplacementMapElement(x) WebCore::toElement<WebCore::SVGFEDisplacementMapElement>(x)
#define toSVGFEDistantLightElement(x) WebCore::toElement<WebCore::SVGFEDistantLightElement>(x)
#define toSVGFEDropShadowElement(x) WebCore::toElement<WebCore::SVGFEDropShadowElement>(x)
#define toSVGFEFloodElement(x) WebCore::toElement<WebCore::SVGFEFloodElement>(x)
#define toSVGFEFuncAElement(x) WebCore::toElement<WebCore::SVGFEFuncAElement>(x)
#define toSVGFEFuncBElement(x) WebCore::toElement<WebCore::SVGFEFuncBElement>(x)
#define toSVGFEFuncGElement(x) WebCore::toElement<WebCore::SVGFEFuncGElement>(x)
#define toSVGFEFuncRElement(x) WebCore::toElement<WebCore::SVGFEFuncRElement>(x)
#define toSVGFEGaussianBlurElement(x) WebCore::toElement<WebCore::SVGFEGaussianBlurElement>(x)
#define toSVGFEImageElement(x) WebCore::toElement<WebCore::SVGFEImageElement>(x)
#define toSVGFEMergeElement(x) WebCore::toElement<WebCore::SVGFEMergeElement>(x)
#define toSVGFEMergeNodeElement(x) WebCore::toElement<WebCore::SVGFEMergeNodeElement>(x)
#define toSVGFEMorphologyElement(x) WebCore::toElement<WebCore::SVGFEMorphologyElement>(x)
#define toSVGFEOffsetElement(x) WebCore::toElement<WebCore::SVGFEOffsetElement>(x)
#define toSVGFEPointLightElement(x) WebCore::toElement<WebCore::SVGFEPointLightElement>(x)
#define toSVGFESpecularLightingElement(x) WebCore::toElement<WebCore::SVGFESpecularLightingElement>(x)
#define toSVGFESpotLightElement(x) WebCore::toElement<WebCore::SVGFESpotLightElement>(x)
#define toSVGFETileElement(x) WebCore::toElement<WebCore::SVGFETileElement>(x)
#define toSVGFETurbulenceElement(x) WebCore::toElement<WebCore::SVGFETurbulenceElement>(x)
#define toSVGFilterElement(x) WebCore::toElement<WebCore::SVGFilterElement>(x)
#define toSVGForeignObjectElement(x) WebCore::toElement<WebCore::SVGForeignObjectElement>(x)
#define toSVGGElement(x) WebCore::toElement<WebCore::SVGGElement>(x)
#define toSVGImageElement(x) WebCore::toElement<WebCore::SVGImageElement>(x)
#define toSVGLineElement(x) WebCore::toElement<WebCore::SVGLineElement>(x)
#define toSVGLinearGradientElement(x) WebCore::toElement<WebCore::SVGLinearGradientElement>(x)
#define toSVGMPathElement(x) WebCore::toElement<WebCore::SVGMPathElement>(x)
#define toSVGMarkerElement(x) WebCore::toElement<WebCore::SVGMarkerElement>(x)
#define toSVGMaskElement(x) WebCore::toElement<WebCore::SVGMaskElement>(x)
#define toSVGMetadataElement(x) WebCore::toElement<WebCore::SVGMetadataElement>(x)
#define toSVGPathElement(x) WebCore::toElement<WebCore::SVGPathElement>(x)
#define toSVGPatternElement(x) WebCore::toElement<WebCore::SVGPatternElement>(x)
#define toSVGPolygonElement(x) WebCore::toElement<WebCore::SVGPolygonElement>(x)
#define toSVGPolylineElement(x) WebCore::toElement<WebCore::SVGPolylineElement>(x)
#define toSVGRadialGradientElement(x) WebCore::toElement<WebCore::SVGRadialGradientElement>(x)
#define toSVGRectElement(x) WebCore::toElement<WebCore::SVGRectElement>(x)
#define toSVGSVGElement(x) WebCore::toElement<WebCore::SVGSVGElement>(x)
#define toSVGSetElement(x) WebCore::toElement<WebCore::SVGSetElement>(x)
#define toSVGStopElement(x) WebCore::toElement<WebCore::SVGStopElement>(x)
#define toSVGSwitchElement(x) WebCore::toElement<WebCore::SVGSwitchElement>(x)
#define toSVGSymbolElement(x) WebCore::toElement<WebCore::SVGSymbolElement>(x)
#define toSVGTSpanElement(x) WebCore::toElement<WebCore::SVGTSpanElement>(x)
#define toSVGTextElement(x) WebCore::toElement<WebCore::SVGTextElement>(x)
#define toSVGTextPathElement(x) WebCore::toElement<WebCore::SVGTextPathElement>(x)
#define toSVGTitleElement(x) WebCore::toElement<WebCore::SVGTitleElement>(x)
#define toSVGUseElement(x) WebCore::toElement<WebCore::SVGUseElement>(x)
#define toSVGViewElement(x) WebCore::toElement<WebCore::SVGViewElement>(x)
#define toSVGScriptElement(x) WebCore::toElement<WebCore::SVGScriptElement>(x)
#define toSVGStyleElement(x) WebCore::toElement<WebCore::SVGStyleElement>(x)
#if ENABLE(SVG_FONTS)
#define toSVGAltGlyphDefElement(x) WebCore::toElement<WebCore::SVGAltGlyphDefElement>(x)
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
#define toSVGAltGlyphElement(x) WebCore::toElement<WebCore::SVGAltGlyphElement>(x)
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
#define toSVGAltGlyphItemElement(x) WebCore::toElement<WebCore::SVGAltGlyphItemElement>(x)
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
#define toSVGFontElement(x) WebCore::toElement<WebCore::SVGFontElement>(x)
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
#define toSVGFontFaceElement(x) WebCore::toElement<WebCore::SVGFontFaceElement>(x)
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
#define toSVGFontFaceFormatElement(x) WebCore::toElement<WebCore::SVGFontFaceFormatElement>(x)
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
#define toSVGFontFaceNameElement(x) WebCore::toElement<WebCore::SVGFontFaceNameElement>(x)
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
#define toSVGFontFaceSrcElement(x) WebCore::toElement<WebCore::SVGFontFaceSrcElement>(x)
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
#define toSVGFontFaceUriElement(x) WebCore::toElement<WebCore::SVGFontFaceUriElement>(x)
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
#define toSVGGlyphElement(x) WebCore::toElement<WebCore::SVGGlyphElement>(x)
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
#define toSVGGlyphRefElement(x) WebCore::toElement<WebCore::SVGGlyphRefElement>(x)
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
#define toSVGHKernElement(x) WebCore::toElement<WebCore::SVGHKernElement>(x)
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
#define toSVGMissingGlyphElement(x) WebCore::toElement<WebCore::SVGMissingGlyphElement>(x)
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
#define toSVGVKernElement(x) WebCore::toElement<WebCore::SVGVKernElement>(x)
#endif // ENABLE(SVG_FONTS)
} // WebCore

#endif
