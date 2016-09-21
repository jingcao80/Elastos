// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HTMLElementTypeHelpers_h
#define HTMLElementTypeHelpers_h

#include "core/dom/ContextFeatures.h"
#include "core/dom/Element.h"
#include "HTMLNames.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace WebCore {
// Type checking.
class HTMLAnchorElement;
void isHTMLAnchorElement(const HTMLAnchorElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLAnchorElement(const HTMLAnchorElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLAnchorElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::aTag);
}
inline bool isHTMLAnchorElement(const Element& element) {
    return element.isHTMLElement() && isHTMLAnchorElement(toHTMLElement(element));
}
inline bool isHTMLAnchorElement(const HTMLElement* element) { return element && isHTMLAnchorElement(*element); }
inline bool isHTMLAnchorElement(const Element* element) { return element && isHTMLAnchorElement(*element); }
template<typename T> inline bool isHTMLAnchorElement(const PassRefPtr<T>& node) { return isHTMLAnchorElement(node.get()); }
template<typename T> inline bool isHTMLAnchorElement(const RefPtr<T>& node) { return isHTMLAnchorElement(node.get()); }
inline bool isHTMLAnchorElement(const Node& node) { return node.isElementNode() ? isHTMLAnchorElement(toElement(node)) : false; }
inline bool isHTMLAnchorElement(const Node* node) { return node && node->isElementNode() ? isHTMLAnchorElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLAnchorElement>(const Element& element) { return isHTMLAnchorElement(element); }
template <> inline bool isElementOfType<const HTMLAnchorElement>(const HTMLElement& element) { return isHTMLAnchorElement(element); }
class HTMLAreaElement;
void isHTMLAreaElement(const HTMLAreaElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLAreaElement(const HTMLAreaElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLAreaElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::areaTag);
}
inline bool isHTMLAreaElement(const Element& element) {
    return element.isHTMLElement() && isHTMLAreaElement(toHTMLElement(element));
}
inline bool isHTMLAreaElement(const HTMLElement* element) { return element && isHTMLAreaElement(*element); }
inline bool isHTMLAreaElement(const Element* element) { return element && isHTMLAreaElement(*element); }
template<typename T> inline bool isHTMLAreaElement(const PassRefPtr<T>& node) { return isHTMLAreaElement(node.get()); }
template<typename T> inline bool isHTMLAreaElement(const RefPtr<T>& node) { return isHTMLAreaElement(node.get()); }
inline bool isHTMLAreaElement(const Node& node) { return node.isElementNode() ? isHTMLAreaElement(toElement(node)) : false; }
inline bool isHTMLAreaElement(const Node* node) { return node && node->isElementNode() ? isHTMLAreaElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLAreaElement>(const Element& element) { return isHTMLAreaElement(element); }
template <> inline bool isElementOfType<const HTMLAreaElement>(const HTMLElement& element) { return isHTMLAreaElement(element); }
class HTMLAudioElement;
void isHTMLAudioElement(const HTMLAudioElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLAudioElement(const HTMLAudioElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLAudioElement(const HTMLElement& element) {
    if (element.isHTMLUnknownElement())
        return false;
    return element.hasLocalName(HTMLNames::audioTag);
}
inline bool isHTMLAudioElement(const Element& element) {
    return element.isHTMLElement() && isHTMLAudioElement(toHTMLElement(element));
}
inline bool isHTMLAudioElement(const HTMLElement* element) { return element && isHTMLAudioElement(*element); }
inline bool isHTMLAudioElement(const Element* element) { return element && isHTMLAudioElement(*element); }
template<typename T> inline bool isHTMLAudioElement(const PassRefPtr<T>& node) { return isHTMLAudioElement(node.get()); }
template<typename T> inline bool isHTMLAudioElement(const RefPtr<T>& node) { return isHTMLAudioElement(node.get()); }
inline bool isHTMLAudioElement(const Node& node) { return node.isElementNode() ? isHTMLAudioElement(toElement(node)) : false; }
inline bool isHTMLAudioElement(const Node* node) { return node && node->isElementNode() ? isHTMLAudioElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLAudioElement>(const Element& element) { return isHTMLAudioElement(element); }
template <> inline bool isElementOfType<const HTMLAudioElement>(const HTMLElement& element) { return isHTMLAudioElement(element); }
class HTMLBRElement;
void isHTMLBRElement(const HTMLBRElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLBRElement(const HTMLBRElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLBRElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::brTag);
}
inline bool isHTMLBRElement(const Element& element) {
    return element.isHTMLElement() && isHTMLBRElement(toHTMLElement(element));
}
inline bool isHTMLBRElement(const HTMLElement* element) { return element && isHTMLBRElement(*element); }
inline bool isHTMLBRElement(const Element* element) { return element && isHTMLBRElement(*element); }
template<typename T> inline bool isHTMLBRElement(const PassRefPtr<T>& node) { return isHTMLBRElement(node.get()); }
template<typename T> inline bool isHTMLBRElement(const RefPtr<T>& node) { return isHTMLBRElement(node.get()); }
inline bool isHTMLBRElement(const Node& node) { return node.isElementNode() ? isHTMLBRElement(toElement(node)) : false; }
inline bool isHTMLBRElement(const Node* node) { return node && node->isElementNode() ? isHTMLBRElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLBRElement>(const Element& element) { return isHTMLBRElement(element); }
template <> inline bool isElementOfType<const HTMLBRElement>(const HTMLElement& element) { return isHTMLBRElement(element); }
class HTMLBaseElement;
void isHTMLBaseElement(const HTMLBaseElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLBaseElement(const HTMLBaseElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLBaseElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::baseTag);
}
inline bool isHTMLBaseElement(const Element& element) {
    return element.isHTMLElement() && isHTMLBaseElement(toHTMLElement(element));
}
inline bool isHTMLBaseElement(const HTMLElement* element) { return element && isHTMLBaseElement(*element); }
inline bool isHTMLBaseElement(const Element* element) { return element && isHTMLBaseElement(*element); }
template<typename T> inline bool isHTMLBaseElement(const PassRefPtr<T>& node) { return isHTMLBaseElement(node.get()); }
template<typename T> inline bool isHTMLBaseElement(const RefPtr<T>& node) { return isHTMLBaseElement(node.get()); }
inline bool isHTMLBaseElement(const Node& node) { return node.isElementNode() ? isHTMLBaseElement(toElement(node)) : false; }
inline bool isHTMLBaseElement(const Node* node) { return node && node->isElementNode() ? isHTMLBaseElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLBaseElement>(const Element& element) { return isHTMLBaseElement(element); }
template <> inline bool isElementOfType<const HTMLBaseElement>(const HTMLElement& element) { return isHTMLBaseElement(element); }
class HTMLBodyElement;
void isHTMLBodyElement(const HTMLBodyElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLBodyElement(const HTMLBodyElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLBodyElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::bodyTag);
}
inline bool isHTMLBodyElement(const Element& element) {
    return element.isHTMLElement() && isHTMLBodyElement(toHTMLElement(element));
}
inline bool isHTMLBodyElement(const HTMLElement* element) { return element && isHTMLBodyElement(*element); }
inline bool isHTMLBodyElement(const Element* element) { return element && isHTMLBodyElement(*element); }
template<typename T> inline bool isHTMLBodyElement(const PassRefPtr<T>& node) { return isHTMLBodyElement(node.get()); }
template<typename T> inline bool isHTMLBodyElement(const RefPtr<T>& node) { return isHTMLBodyElement(node.get()); }
inline bool isHTMLBodyElement(const Node& node) { return node.isElementNode() ? isHTMLBodyElement(toElement(node)) : false; }
inline bool isHTMLBodyElement(const Node* node) { return node && node->isElementNode() ? isHTMLBodyElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLBodyElement>(const Element& element) { return isHTMLBodyElement(element); }
template <> inline bool isElementOfType<const HTMLBodyElement>(const HTMLElement& element) { return isHTMLBodyElement(element); }
class HTMLCanvasElement;
void isHTMLCanvasElement(const HTMLCanvasElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLCanvasElement(const HTMLCanvasElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLCanvasElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::canvasTag);
}
inline bool isHTMLCanvasElement(const Element& element) {
    return element.isHTMLElement() && isHTMLCanvasElement(toHTMLElement(element));
}
inline bool isHTMLCanvasElement(const HTMLElement* element) { return element && isHTMLCanvasElement(*element); }
inline bool isHTMLCanvasElement(const Element* element) { return element && isHTMLCanvasElement(*element); }
template<typename T> inline bool isHTMLCanvasElement(const PassRefPtr<T>& node) { return isHTMLCanvasElement(node.get()); }
template<typename T> inline bool isHTMLCanvasElement(const RefPtr<T>& node) { return isHTMLCanvasElement(node.get()); }
inline bool isHTMLCanvasElement(const Node& node) { return node.isElementNode() ? isHTMLCanvasElement(toElement(node)) : false; }
inline bool isHTMLCanvasElement(const Node* node) { return node && node->isElementNode() ? isHTMLCanvasElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLCanvasElement>(const Element& element) { return isHTMLCanvasElement(element); }
template <> inline bool isElementOfType<const HTMLCanvasElement>(const HTMLElement& element) { return isHTMLCanvasElement(element); }
class HTMLContentElement;
void isHTMLContentElement(const HTMLContentElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLContentElement(const HTMLContentElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLContentElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::contentTag);
}
inline bool isHTMLContentElement(const Element& element) {
    return element.isHTMLElement() && isHTMLContentElement(toHTMLElement(element));
}
inline bool isHTMLContentElement(const HTMLElement* element) { return element && isHTMLContentElement(*element); }
inline bool isHTMLContentElement(const Element* element) { return element && isHTMLContentElement(*element); }
template<typename T> inline bool isHTMLContentElement(const PassRefPtr<T>& node) { return isHTMLContentElement(node.get()); }
template<typename T> inline bool isHTMLContentElement(const RefPtr<T>& node) { return isHTMLContentElement(node.get()); }
inline bool isHTMLContentElement(const Node& node) { return node.isElementNode() ? isHTMLContentElement(toElement(node)) : false; }
inline bool isHTMLContentElement(const Node* node) { return node && node->isElementNode() ? isHTMLContentElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLContentElement>(const Element& element) { return isHTMLContentElement(element); }
template <> inline bool isElementOfType<const HTMLContentElement>(const HTMLElement& element) { return isHTMLContentElement(element); }
class HTMLDListElement;
void isHTMLDListElement(const HTMLDListElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLDListElement(const HTMLDListElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLDListElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::dlTag);
}
inline bool isHTMLDListElement(const Element& element) {
    return element.isHTMLElement() && isHTMLDListElement(toHTMLElement(element));
}
inline bool isHTMLDListElement(const HTMLElement* element) { return element && isHTMLDListElement(*element); }
inline bool isHTMLDListElement(const Element* element) { return element && isHTMLDListElement(*element); }
template<typename T> inline bool isHTMLDListElement(const PassRefPtr<T>& node) { return isHTMLDListElement(node.get()); }
template<typename T> inline bool isHTMLDListElement(const RefPtr<T>& node) { return isHTMLDListElement(node.get()); }
inline bool isHTMLDListElement(const Node& node) { return node.isElementNode() ? isHTMLDListElement(toElement(node)) : false; }
inline bool isHTMLDListElement(const Node* node) { return node && node->isElementNode() ? isHTMLDListElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLDListElement>(const Element& element) { return isHTMLDListElement(element); }
template <> inline bool isElementOfType<const HTMLDListElement>(const HTMLElement& element) { return isHTMLDListElement(element); }
class HTMLDataListElement;
void isHTMLDataListElement(const HTMLDataListElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLDataListElement(const HTMLDataListElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLDataListElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::datalistTag);
}
inline bool isHTMLDataListElement(const Element& element) {
    return element.isHTMLElement() && isHTMLDataListElement(toHTMLElement(element));
}
inline bool isHTMLDataListElement(const HTMLElement* element) { return element && isHTMLDataListElement(*element); }
inline bool isHTMLDataListElement(const Element* element) { return element && isHTMLDataListElement(*element); }
template<typename T> inline bool isHTMLDataListElement(const PassRefPtr<T>& node) { return isHTMLDataListElement(node.get()); }
template<typename T> inline bool isHTMLDataListElement(const RefPtr<T>& node) { return isHTMLDataListElement(node.get()); }
inline bool isHTMLDataListElement(const Node& node) { return node.isElementNode() ? isHTMLDataListElement(toElement(node)) : false; }
inline bool isHTMLDataListElement(const Node* node) { return node && node->isElementNode() ? isHTMLDataListElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLDataListElement>(const Element& element) { return isHTMLDataListElement(element); }
template <> inline bool isElementOfType<const HTMLDataListElement>(const HTMLElement& element) { return isHTMLDataListElement(element); }
class HTMLDetailsElement;
void isHTMLDetailsElement(const HTMLDetailsElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLDetailsElement(const HTMLDetailsElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLDetailsElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::detailsTag);
}
inline bool isHTMLDetailsElement(const Element& element) {
    return element.isHTMLElement() && isHTMLDetailsElement(toHTMLElement(element));
}
inline bool isHTMLDetailsElement(const HTMLElement* element) { return element && isHTMLDetailsElement(*element); }
inline bool isHTMLDetailsElement(const Element* element) { return element && isHTMLDetailsElement(*element); }
template<typename T> inline bool isHTMLDetailsElement(const PassRefPtr<T>& node) { return isHTMLDetailsElement(node.get()); }
template<typename T> inline bool isHTMLDetailsElement(const RefPtr<T>& node) { return isHTMLDetailsElement(node.get()); }
inline bool isHTMLDetailsElement(const Node& node) { return node.isElementNode() ? isHTMLDetailsElement(toElement(node)) : false; }
inline bool isHTMLDetailsElement(const Node* node) { return node && node->isElementNode() ? isHTMLDetailsElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLDetailsElement>(const Element& element) { return isHTMLDetailsElement(element); }
template <> inline bool isElementOfType<const HTMLDetailsElement>(const HTMLElement& element) { return isHTMLDetailsElement(element); }
class HTMLDirectoryElement;
void isHTMLDirectoryElement(const HTMLDirectoryElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLDirectoryElement(const HTMLDirectoryElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLDirectoryElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::dirTag);
}
inline bool isHTMLDirectoryElement(const Element& element) {
    return element.isHTMLElement() && isHTMLDirectoryElement(toHTMLElement(element));
}
inline bool isHTMLDirectoryElement(const HTMLElement* element) { return element && isHTMLDirectoryElement(*element); }
inline bool isHTMLDirectoryElement(const Element* element) { return element && isHTMLDirectoryElement(*element); }
template<typename T> inline bool isHTMLDirectoryElement(const PassRefPtr<T>& node) { return isHTMLDirectoryElement(node.get()); }
template<typename T> inline bool isHTMLDirectoryElement(const RefPtr<T>& node) { return isHTMLDirectoryElement(node.get()); }
inline bool isHTMLDirectoryElement(const Node& node) { return node.isElementNode() ? isHTMLDirectoryElement(toElement(node)) : false; }
inline bool isHTMLDirectoryElement(const Node* node) { return node && node->isElementNode() ? isHTMLDirectoryElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLDirectoryElement>(const Element& element) { return isHTMLDirectoryElement(element); }
template <> inline bool isElementOfType<const HTMLDirectoryElement>(const HTMLElement& element) { return isHTMLDirectoryElement(element); }
class HTMLDivElement;
void isHTMLDivElement(const HTMLDivElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLDivElement(const HTMLDivElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLDivElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::divTag);
}
inline bool isHTMLDivElement(const Element& element) {
    return element.isHTMLElement() && isHTMLDivElement(toHTMLElement(element));
}
inline bool isHTMLDivElement(const HTMLElement* element) { return element && isHTMLDivElement(*element); }
inline bool isHTMLDivElement(const Element* element) { return element && isHTMLDivElement(*element); }
template<typename T> inline bool isHTMLDivElement(const PassRefPtr<T>& node) { return isHTMLDivElement(node.get()); }
template<typename T> inline bool isHTMLDivElement(const RefPtr<T>& node) { return isHTMLDivElement(node.get()); }
inline bool isHTMLDivElement(const Node& node) { return node.isElementNode() ? isHTMLDivElement(toElement(node)) : false; }
inline bool isHTMLDivElement(const Node* node) { return node && node->isElementNode() ? isHTMLDivElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLDivElement>(const Element& element) { return isHTMLDivElement(element); }
template <> inline bool isElementOfType<const HTMLDivElement>(const HTMLElement& element) { return isHTMLDivElement(element); }
class HTMLFontElement;
void isHTMLFontElement(const HTMLFontElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLFontElement(const HTMLFontElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLFontElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::fontTag);
}
inline bool isHTMLFontElement(const Element& element) {
    return element.isHTMLElement() && isHTMLFontElement(toHTMLElement(element));
}
inline bool isHTMLFontElement(const HTMLElement* element) { return element && isHTMLFontElement(*element); }
inline bool isHTMLFontElement(const Element* element) { return element && isHTMLFontElement(*element); }
template<typename T> inline bool isHTMLFontElement(const PassRefPtr<T>& node) { return isHTMLFontElement(node.get()); }
template<typename T> inline bool isHTMLFontElement(const RefPtr<T>& node) { return isHTMLFontElement(node.get()); }
inline bool isHTMLFontElement(const Node& node) { return node.isElementNode() ? isHTMLFontElement(toElement(node)) : false; }
inline bool isHTMLFontElement(const Node* node) { return node && node->isElementNode() ? isHTMLFontElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLFontElement>(const Element& element) { return isHTMLFontElement(element); }
template <> inline bool isElementOfType<const HTMLFontElement>(const HTMLElement& element) { return isHTMLFontElement(element); }
class HTMLFormElement;
void isHTMLFormElement(const HTMLFormElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLFormElement(const HTMLFormElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLFormElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::formTag);
}
inline bool isHTMLFormElement(const Element& element) {
    return element.isHTMLElement() && isHTMLFormElement(toHTMLElement(element));
}
inline bool isHTMLFormElement(const HTMLElement* element) { return element && isHTMLFormElement(*element); }
inline bool isHTMLFormElement(const Element* element) { return element && isHTMLFormElement(*element); }
template<typename T> inline bool isHTMLFormElement(const PassRefPtr<T>& node) { return isHTMLFormElement(node.get()); }
template<typename T> inline bool isHTMLFormElement(const RefPtr<T>& node) { return isHTMLFormElement(node.get()); }
inline bool isHTMLFormElement(const Node& node) { return node.isElementNode() ? isHTMLFormElement(toElement(node)) : false; }
inline bool isHTMLFormElement(const Node* node) { return node && node->isElementNode() ? isHTMLFormElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLFormElement>(const Element& element) { return isHTMLFormElement(element); }
template <> inline bool isElementOfType<const HTMLFormElement>(const HTMLElement& element) { return isHTMLFormElement(element); }
class HTMLFrameElement;
void isHTMLFrameElement(const HTMLFrameElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLFrameElement(const HTMLFrameElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLFrameElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::frameTag);
}
inline bool isHTMLFrameElement(const Element& element) {
    return element.isHTMLElement() && isHTMLFrameElement(toHTMLElement(element));
}
inline bool isHTMLFrameElement(const HTMLElement* element) { return element && isHTMLFrameElement(*element); }
inline bool isHTMLFrameElement(const Element* element) { return element && isHTMLFrameElement(*element); }
template<typename T> inline bool isHTMLFrameElement(const PassRefPtr<T>& node) { return isHTMLFrameElement(node.get()); }
template<typename T> inline bool isHTMLFrameElement(const RefPtr<T>& node) { return isHTMLFrameElement(node.get()); }
inline bool isHTMLFrameElement(const Node& node) { return node.isElementNode() ? isHTMLFrameElement(toElement(node)) : false; }
inline bool isHTMLFrameElement(const Node* node) { return node && node->isElementNode() ? isHTMLFrameElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLFrameElement>(const Element& element) { return isHTMLFrameElement(element); }
template <> inline bool isElementOfType<const HTMLFrameElement>(const HTMLElement& element) { return isHTMLFrameElement(element); }
class HTMLFrameSetElement;
void isHTMLFrameSetElement(const HTMLFrameSetElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLFrameSetElement(const HTMLFrameSetElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLFrameSetElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::framesetTag);
}
inline bool isHTMLFrameSetElement(const Element& element) {
    return element.isHTMLElement() && isHTMLFrameSetElement(toHTMLElement(element));
}
inline bool isHTMLFrameSetElement(const HTMLElement* element) { return element && isHTMLFrameSetElement(*element); }
inline bool isHTMLFrameSetElement(const Element* element) { return element && isHTMLFrameSetElement(*element); }
template<typename T> inline bool isHTMLFrameSetElement(const PassRefPtr<T>& node) { return isHTMLFrameSetElement(node.get()); }
template<typename T> inline bool isHTMLFrameSetElement(const RefPtr<T>& node) { return isHTMLFrameSetElement(node.get()); }
inline bool isHTMLFrameSetElement(const Node& node) { return node.isElementNode() ? isHTMLFrameSetElement(toElement(node)) : false; }
inline bool isHTMLFrameSetElement(const Node* node) { return node && node->isElementNode() ? isHTMLFrameSetElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLFrameSetElement>(const Element& element) { return isHTMLFrameSetElement(element); }
template <> inline bool isElementOfType<const HTMLFrameSetElement>(const HTMLElement& element) { return isHTMLFrameSetElement(element); }
class HTMLHRElement;
void isHTMLHRElement(const HTMLHRElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLHRElement(const HTMLHRElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLHRElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::hrTag);
}
inline bool isHTMLHRElement(const Element& element) {
    return element.isHTMLElement() && isHTMLHRElement(toHTMLElement(element));
}
inline bool isHTMLHRElement(const HTMLElement* element) { return element && isHTMLHRElement(*element); }
inline bool isHTMLHRElement(const Element* element) { return element && isHTMLHRElement(*element); }
template<typename T> inline bool isHTMLHRElement(const PassRefPtr<T>& node) { return isHTMLHRElement(node.get()); }
template<typename T> inline bool isHTMLHRElement(const RefPtr<T>& node) { return isHTMLHRElement(node.get()); }
inline bool isHTMLHRElement(const Node& node) { return node.isElementNode() ? isHTMLHRElement(toElement(node)) : false; }
inline bool isHTMLHRElement(const Node* node) { return node && node->isElementNode() ? isHTMLHRElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLHRElement>(const Element& element) { return isHTMLHRElement(element); }
template <> inline bool isElementOfType<const HTMLHRElement>(const HTMLElement& element) { return isHTMLHRElement(element); }
class HTMLHeadElement;
void isHTMLHeadElement(const HTMLHeadElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLHeadElement(const HTMLHeadElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLHeadElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::headTag);
}
inline bool isHTMLHeadElement(const Element& element) {
    return element.isHTMLElement() && isHTMLHeadElement(toHTMLElement(element));
}
inline bool isHTMLHeadElement(const HTMLElement* element) { return element && isHTMLHeadElement(*element); }
inline bool isHTMLHeadElement(const Element* element) { return element && isHTMLHeadElement(*element); }
template<typename T> inline bool isHTMLHeadElement(const PassRefPtr<T>& node) { return isHTMLHeadElement(node.get()); }
template<typename T> inline bool isHTMLHeadElement(const RefPtr<T>& node) { return isHTMLHeadElement(node.get()); }
inline bool isHTMLHeadElement(const Node& node) { return node.isElementNode() ? isHTMLHeadElement(toElement(node)) : false; }
inline bool isHTMLHeadElement(const Node* node) { return node && node->isElementNode() ? isHTMLHeadElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLHeadElement>(const Element& element) { return isHTMLHeadElement(element); }
template <> inline bool isElementOfType<const HTMLHeadElement>(const HTMLElement& element) { return isHTMLHeadElement(element); }
class HTMLHtmlElement;
void isHTMLHtmlElement(const HTMLHtmlElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLHtmlElement(const HTMLHtmlElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLHtmlElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::htmlTag);
}
inline bool isHTMLHtmlElement(const Element& element) {
    return element.isHTMLElement() && isHTMLHtmlElement(toHTMLElement(element));
}
inline bool isHTMLHtmlElement(const HTMLElement* element) { return element && isHTMLHtmlElement(*element); }
inline bool isHTMLHtmlElement(const Element* element) { return element && isHTMLHtmlElement(*element); }
template<typename T> inline bool isHTMLHtmlElement(const PassRefPtr<T>& node) { return isHTMLHtmlElement(node.get()); }
template<typename T> inline bool isHTMLHtmlElement(const RefPtr<T>& node) { return isHTMLHtmlElement(node.get()); }
inline bool isHTMLHtmlElement(const Node& node) { return node.isElementNode() ? isHTMLHtmlElement(toElement(node)) : false; }
inline bool isHTMLHtmlElement(const Node* node) { return node && node->isElementNode() ? isHTMLHtmlElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLHtmlElement>(const Element& element) { return isHTMLHtmlElement(element); }
template <> inline bool isElementOfType<const HTMLHtmlElement>(const HTMLElement& element) { return isHTMLHtmlElement(element); }
class HTMLIFrameElement;
void isHTMLIFrameElement(const HTMLIFrameElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLIFrameElement(const HTMLIFrameElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLIFrameElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::iframeTag);
}
inline bool isHTMLIFrameElement(const Element& element) {
    return element.isHTMLElement() && isHTMLIFrameElement(toHTMLElement(element));
}
inline bool isHTMLIFrameElement(const HTMLElement* element) { return element && isHTMLIFrameElement(*element); }
inline bool isHTMLIFrameElement(const Element* element) { return element && isHTMLIFrameElement(*element); }
template<typename T> inline bool isHTMLIFrameElement(const PassRefPtr<T>& node) { return isHTMLIFrameElement(node.get()); }
template<typename T> inline bool isHTMLIFrameElement(const RefPtr<T>& node) { return isHTMLIFrameElement(node.get()); }
inline bool isHTMLIFrameElement(const Node& node) { return node.isElementNode() ? isHTMLIFrameElement(toElement(node)) : false; }
inline bool isHTMLIFrameElement(const Node* node) { return node && node->isElementNode() ? isHTMLIFrameElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLIFrameElement>(const Element& element) { return isHTMLIFrameElement(element); }
template <> inline bool isElementOfType<const HTMLIFrameElement>(const HTMLElement& element) { return isHTMLIFrameElement(element); }
class HTMLLIElement;
void isHTMLLIElement(const HTMLLIElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLLIElement(const HTMLLIElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLLIElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::liTag);
}
inline bool isHTMLLIElement(const Element& element) {
    return element.isHTMLElement() && isHTMLLIElement(toHTMLElement(element));
}
inline bool isHTMLLIElement(const HTMLElement* element) { return element && isHTMLLIElement(*element); }
inline bool isHTMLLIElement(const Element* element) { return element && isHTMLLIElement(*element); }
template<typename T> inline bool isHTMLLIElement(const PassRefPtr<T>& node) { return isHTMLLIElement(node.get()); }
template<typename T> inline bool isHTMLLIElement(const RefPtr<T>& node) { return isHTMLLIElement(node.get()); }
inline bool isHTMLLIElement(const Node& node) { return node.isElementNode() ? isHTMLLIElement(toElement(node)) : false; }
inline bool isHTMLLIElement(const Node* node) { return node && node->isElementNode() ? isHTMLLIElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLLIElement>(const Element& element) { return isHTMLLIElement(element); }
template <> inline bool isElementOfType<const HTMLLIElement>(const HTMLElement& element) { return isHTMLLIElement(element); }
class HTMLLabelElement;
void isHTMLLabelElement(const HTMLLabelElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLLabelElement(const HTMLLabelElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLLabelElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::labelTag);
}
inline bool isHTMLLabelElement(const Element& element) {
    return element.isHTMLElement() && isHTMLLabelElement(toHTMLElement(element));
}
inline bool isHTMLLabelElement(const HTMLElement* element) { return element && isHTMLLabelElement(*element); }
inline bool isHTMLLabelElement(const Element* element) { return element && isHTMLLabelElement(*element); }
template<typename T> inline bool isHTMLLabelElement(const PassRefPtr<T>& node) { return isHTMLLabelElement(node.get()); }
template<typename T> inline bool isHTMLLabelElement(const RefPtr<T>& node) { return isHTMLLabelElement(node.get()); }
inline bool isHTMLLabelElement(const Node& node) { return node.isElementNode() ? isHTMLLabelElement(toElement(node)) : false; }
inline bool isHTMLLabelElement(const Node* node) { return node && node->isElementNode() ? isHTMLLabelElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLLabelElement>(const Element& element) { return isHTMLLabelElement(element); }
template <> inline bool isElementOfType<const HTMLLabelElement>(const HTMLElement& element) { return isHTMLLabelElement(element); }
class HTMLLegendElement;
void isHTMLLegendElement(const HTMLLegendElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLLegendElement(const HTMLLegendElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLLegendElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::legendTag);
}
inline bool isHTMLLegendElement(const Element& element) {
    return element.isHTMLElement() && isHTMLLegendElement(toHTMLElement(element));
}
inline bool isHTMLLegendElement(const HTMLElement* element) { return element && isHTMLLegendElement(*element); }
inline bool isHTMLLegendElement(const Element* element) { return element && isHTMLLegendElement(*element); }
template<typename T> inline bool isHTMLLegendElement(const PassRefPtr<T>& node) { return isHTMLLegendElement(node.get()); }
template<typename T> inline bool isHTMLLegendElement(const RefPtr<T>& node) { return isHTMLLegendElement(node.get()); }
inline bool isHTMLLegendElement(const Node& node) { return node.isElementNode() ? isHTMLLegendElement(toElement(node)) : false; }
inline bool isHTMLLegendElement(const Node* node) { return node && node->isElementNode() ? isHTMLLegendElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLLegendElement>(const Element& element) { return isHTMLLegendElement(element); }
template <> inline bool isElementOfType<const HTMLLegendElement>(const HTMLElement& element) { return isHTMLLegendElement(element); }
class HTMLMapElement;
void isHTMLMapElement(const HTMLMapElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLMapElement(const HTMLMapElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLMapElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::mapTag);
}
inline bool isHTMLMapElement(const Element& element) {
    return element.isHTMLElement() && isHTMLMapElement(toHTMLElement(element));
}
inline bool isHTMLMapElement(const HTMLElement* element) { return element && isHTMLMapElement(*element); }
inline bool isHTMLMapElement(const Element* element) { return element && isHTMLMapElement(*element); }
template<typename T> inline bool isHTMLMapElement(const PassRefPtr<T>& node) { return isHTMLMapElement(node.get()); }
template<typename T> inline bool isHTMLMapElement(const RefPtr<T>& node) { return isHTMLMapElement(node.get()); }
inline bool isHTMLMapElement(const Node& node) { return node.isElementNode() ? isHTMLMapElement(toElement(node)) : false; }
inline bool isHTMLMapElement(const Node* node) { return node && node->isElementNode() ? isHTMLMapElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLMapElement>(const Element& element) { return isHTMLMapElement(element); }
template <> inline bool isElementOfType<const HTMLMapElement>(const HTMLElement& element) { return isHTMLMapElement(element); }
class HTMLMarqueeElement;
void isHTMLMarqueeElement(const HTMLMarqueeElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLMarqueeElement(const HTMLMarqueeElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLMarqueeElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::marqueeTag);
}
inline bool isHTMLMarqueeElement(const Element& element) {
    return element.isHTMLElement() && isHTMLMarqueeElement(toHTMLElement(element));
}
inline bool isHTMLMarqueeElement(const HTMLElement* element) { return element && isHTMLMarqueeElement(*element); }
inline bool isHTMLMarqueeElement(const Element* element) { return element && isHTMLMarqueeElement(*element); }
template<typename T> inline bool isHTMLMarqueeElement(const PassRefPtr<T>& node) { return isHTMLMarqueeElement(node.get()); }
template<typename T> inline bool isHTMLMarqueeElement(const RefPtr<T>& node) { return isHTMLMarqueeElement(node.get()); }
inline bool isHTMLMarqueeElement(const Node& node) { return node.isElementNode() ? isHTMLMarqueeElement(toElement(node)) : false; }
inline bool isHTMLMarqueeElement(const Node* node) { return node && node->isElementNode() ? isHTMLMarqueeElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLMarqueeElement>(const Element& element) { return isHTMLMarqueeElement(element); }
template <> inline bool isElementOfType<const HTMLMarqueeElement>(const HTMLElement& element) { return isHTMLMarqueeElement(element); }
class HTMLMenuElement;
void isHTMLMenuElement(const HTMLMenuElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLMenuElement(const HTMLMenuElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLMenuElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::menuTag);
}
inline bool isHTMLMenuElement(const Element& element) {
    return element.isHTMLElement() && isHTMLMenuElement(toHTMLElement(element));
}
inline bool isHTMLMenuElement(const HTMLElement* element) { return element && isHTMLMenuElement(*element); }
inline bool isHTMLMenuElement(const Element* element) { return element && isHTMLMenuElement(*element); }
template<typename T> inline bool isHTMLMenuElement(const PassRefPtr<T>& node) { return isHTMLMenuElement(node.get()); }
template<typename T> inline bool isHTMLMenuElement(const RefPtr<T>& node) { return isHTMLMenuElement(node.get()); }
inline bool isHTMLMenuElement(const Node& node) { return node.isElementNode() ? isHTMLMenuElement(toElement(node)) : false; }
inline bool isHTMLMenuElement(const Node* node) { return node && node->isElementNode() ? isHTMLMenuElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLMenuElement>(const Element& element) { return isHTMLMenuElement(element); }
template <> inline bool isElementOfType<const HTMLMenuElement>(const HTMLElement& element) { return isHTMLMenuElement(element); }
class HTMLMetaElement;
void isHTMLMetaElement(const HTMLMetaElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLMetaElement(const HTMLMetaElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLMetaElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::metaTag);
}
inline bool isHTMLMetaElement(const Element& element) {
    return element.isHTMLElement() && isHTMLMetaElement(toHTMLElement(element));
}
inline bool isHTMLMetaElement(const HTMLElement* element) { return element && isHTMLMetaElement(*element); }
inline bool isHTMLMetaElement(const Element* element) { return element && isHTMLMetaElement(*element); }
template<typename T> inline bool isHTMLMetaElement(const PassRefPtr<T>& node) { return isHTMLMetaElement(node.get()); }
template<typename T> inline bool isHTMLMetaElement(const RefPtr<T>& node) { return isHTMLMetaElement(node.get()); }
inline bool isHTMLMetaElement(const Node& node) { return node.isElementNode() ? isHTMLMetaElement(toElement(node)) : false; }
inline bool isHTMLMetaElement(const Node* node) { return node && node->isElementNode() ? isHTMLMetaElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLMetaElement>(const Element& element) { return isHTMLMetaElement(element); }
template <> inline bool isElementOfType<const HTMLMetaElement>(const HTMLElement& element) { return isHTMLMetaElement(element); }
class HTMLMeterElement;
void isHTMLMeterElement(const HTMLMeterElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLMeterElement(const HTMLMeterElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLMeterElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::meterTag);
}
inline bool isHTMLMeterElement(const Element& element) {
    return element.isHTMLElement() && isHTMLMeterElement(toHTMLElement(element));
}
inline bool isHTMLMeterElement(const HTMLElement* element) { return element && isHTMLMeterElement(*element); }
inline bool isHTMLMeterElement(const Element* element) { return element && isHTMLMeterElement(*element); }
template<typename T> inline bool isHTMLMeterElement(const PassRefPtr<T>& node) { return isHTMLMeterElement(node.get()); }
template<typename T> inline bool isHTMLMeterElement(const RefPtr<T>& node) { return isHTMLMeterElement(node.get()); }
inline bool isHTMLMeterElement(const Node& node) { return node.isElementNode() ? isHTMLMeterElement(toElement(node)) : false; }
inline bool isHTMLMeterElement(const Node* node) { return node && node->isElementNode() ? isHTMLMeterElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLMeterElement>(const Element& element) { return isHTMLMeterElement(element); }
template <> inline bool isElementOfType<const HTMLMeterElement>(const HTMLElement& element) { return isHTMLMeterElement(element); }
class HTMLOListElement;
void isHTMLOListElement(const HTMLOListElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLOListElement(const HTMLOListElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLOListElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::olTag);
}
inline bool isHTMLOListElement(const Element& element) {
    return element.isHTMLElement() && isHTMLOListElement(toHTMLElement(element));
}
inline bool isHTMLOListElement(const HTMLElement* element) { return element && isHTMLOListElement(*element); }
inline bool isHTMLOListElement(const Element* element) { return element && isHTMLOListElement(*element); }
template<typename T> inline bool isHTMLOListElement(const PassRefPtr<T>& node) { return isHTMLOListElement(node.get()); }
template<typename T> inline bool isHTMLOListElement(const RefPtr<T>& node) { return isHTMLOListElement(node.get()); }
inline bool isHTMLOListElement(const Node& node) { return node.isElementNode() ? isHTMLOListElement(toElement(node)) : false; }
inline bool isHTMLOListElement(const Node* node) { return node && node->isElementNode() ? isHTMLOListElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLOListElement>(const Element& element) { return isHTMLOListElement(element); }
template <> inline bool isElementOfType<const HTMLOListElement>(const HTMLElement& element) { return isHTMLOListElement(element); }
class HTMLOptGroupElement;
void isHTMLOptGroupElement(const HTMLOptGroupElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLOptGroupElement(const HTMLOptGroupElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLOptGroupElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::optgroupTag);
}
inline bool isHTMLOptGroupElement(const Element& element) {
    return element.isHTMLElement() && isHTMLOptGroupElement(toHTMLElement(element));
}
inline bool isHTMLOptGroupElement(const HTMLElement* element) { return element && isHTMLOptGroupElement(*element); }
inline bool isHTMLOptGroupElement(const Element* element) { return element && isHTMLOptGroupElement(*element); }
template<typename T> inline bool isHTMLOptGroupElement(const PassRefPtr<T>& node) { return isHTMLOptGroupElement(node.get()); }
template<typename T> inline bool isHTMLOptGroupElement(const RefPtr<T>& node) { return isHTMLOptGroupElement(node.get()); }
inline bool isHTMLOptGroupElement(const Node& node) { return node.isElementNode() ? isHTMLOptGroupElement(toElement(node)) : false; }
inline bool isHTMLOptGroupElement(const Node* node) { return node && node->isElementNode() ? isHTMLOptGroupElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLOptGroupElement>(const Element& element) { return isHTMLOptGroupElement(element); }
template <> inline bool isElementOfType<const HTMLOptGroupElement>(const HTMLElement& element) { return isHTMLOptGroupElement(element); }
class HTMLOptionElement;
void isHTMLOptionElement(const HTMLOptionElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLOptionElement(const HTMLOptionElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLOptionElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::optionTag);
}
inline bool isHTMLOptionElement(const Element& element) {
    return element.isHTMLElement() && isHTMLOptionElement(toHTMLElement(element));
}
inline bool isHTMLOptionElement(const HTMLElement* element) { return element && isHTMLOptionElement(*element); }
inline bool isHTMLOptionElement(const Element* element) { return element && isHTMLOptionElement(*element); }
template<typename T> inline bool isHTMLOptionElement(const PassRefPtr<T>& node) { return isHTMLOptionElement(node.get()); }
template<typename T> inline bool isHTMLOptionElement(const RefPtr<T>& node) { return isHTMLOptionElement(node.get()); }
inline bool isHTMLOptionElement(const Node& node) { return node.isElementNode() ? isHTMLOptionElement(toElement(node)) : false; }
inline bool isHTMLOptionElement(const Node* node) { return node && node->isElementNode() ? isHTMLOptionElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLOptionElement>(const Element& element) { return isHTMLOptionElement(element); }
template <> inline bool isElementOfType<const HTMLOptionElement>(const HTMLElement& element) { return isHTMLOptionElement(element); }
class HTMLParagraphElement;
void isHTMLParagraphElement(const HTMLParagraphElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLParagraphElement(const HTMLParagraphElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLParagraphElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::pTag);
}
inline bool isHTMLParagraphElement(const Element& element) {
    return element.isHTMLElement() && isHTMLParagraphElement(toHTMLElement(element));
}
inline bool isHTMLParagraphElement(const HTMLElement* element) { return element && isHTMLParagraphElement(*element); }
inline bool isHTMLParagraphElement(const Element* element) { return element && isHTMLParagraphElement(*element); }
template<typename T> inline bool isHTMLParagraphElement(const PassRefPtr<T>& node) { return isHTMLParagraphElement(node.get()); }
template<typename T> inline bool isHTMLParagraphElement(const RefPtr<T>& node) { return isHTMLParagraphElement(node.get()); }
inline bool isHTMLParagraphElement(const Node& node) { return node.isElementNode() ? isHTMLParagraphElement(toElement(node)) : false; }
inline bool isHTMLParagraphElement(const Node* node) { return node && node->isElementNode() ? isHTMLParagraphElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLParagraphElement>(const Element& element) { return isHTMLParagraphElement(element); }
template <> inline bool isElementOfType<const HTMLParagraphElement>(const HTMLElement& element) { return isHTMLParagraphElement(element); }
class HTMLParamElement;
void isHTMLParamElement(const HTMLParamElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLParamElement(const HTMLParamElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLParamElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::paramTag);
}
inline bool isHTMLParamElement(const Element& element) {
    return element.isHTMLElement() && isHTMLParamElement(toHTMLElement(element));
}
inline bool isHTMLParamElement(const HTMLElement* element) { return element && isHTMLParamElement(*element); }
inline bool isHTMLParamElement(const Element* element) { return element && isHTMLParamElement(*element); }
template<typename T> inline bool isHTMLParamElement(const PassRefPtr<T>& node) { return isHTMLParamElement(node.get()); }
template<typename T> inline bool isHTMLParamElement(const RefPtr<T>& node) { return isHTMLParamElement(node.get()); }
inline bool isHTMLParamElement(const Node& node) { return node.isElementNode() ? isHTMLParamElement(toElement(node)) : false; }
inline bool isHTMLParamElement(const Node* node) { return node && node->isElementNode() ? isHTMLParamElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLParamElement>(const Element& element) { return isHTMLParamElement(element); }
template <> inline bool isElementOfType<const HTMLParamElement>(const HTMLElement& element) { return isHTMLParamElement(element); }
class HTMLPictureElement;
void isHTMLPictureElement(const HTMLPictureElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLPictureElement(const HTMLPictureElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLPictureElement(const HTMLElement& element) {
    if (element.isHTMLUnknownElement())
        return false;
    return element.hasLocalName(HTMLNames::pictureTag);
}
inline bool isHTMLPictureElement(const Element& element) {
    return element.isHTMLElement() && isHTMLPictureElement(toHTMLElement(element));
}
inline bool isHTMLPictureElement(const HTMLElement* element) { return element && isHTMLPictureElement(*element); }
inline bool isHTMLPictureElement(const Element* element) { return element && isHTMLPictureElement(*element); }
template<typename T> inline bool isHTMLPictureElement(const PassRefPtr<T>& node) { return isHTMLPictureElement(node.get()); }
template<typename T> inline bool isHTMLPictureElement(const RefPtr<T>& node) { return isHTMLPictureElement(node.get()); }
inline bool isHTMLPictureElement(const Node& node) { return node.isElementNode() ? isHTMLPictureElement(toElement(node)) : false; }
inline bool isHTMLPictureElement(const Node* node) { return node && node->isElementNode() ? isHTMLPictureElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLPictureElement>(const Element& element) { return isHTMLPictureElement(element); }
template <> inline bool isElementOfType<const HTMLPictureElement>(const HTMLElement& element) { return isHTMLPictureElement(element); }
class HTMLProgressElement;
void isHTMLProgressElement(const HTMLProgressElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLProgressElement(const HTMLProgressElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLProgressElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::progressTag);
}
inline bool isHTMLProgressElement(const Element& element) {
    return element.isHTMLElement() && isHTMLProgressElement(toHTMLElement(element));
}
inline bool isHTMLProgressElement(const HTMLElement* element) { return element && isHTMLProgressElement(*element); }
inline bool isHTMLProgressElement(const Element* element) { return element && isHTMLProgressElement(*element); }
template<typename T> inline bool isHTMLProgressElement(const PassRefPtr<T>& node) { return isHTMLProgressElement(node.get()); }
template<typename T> inline bool isHTMLProgressElement(const RefPtr<T>& node) { return isHTMLProgressElement(node.get()); }
inline bool isHTMLProgressElement(const Node& node) { return node.isElementNode() ? isHTMLProgressElement(toElement(node)) : false; }
inline bool isHTMLProgressElement(const Node* node) { return node && node->isElementNode() ? isHTMLProgressElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLProgressElement>(const Element& element) { return isHTMLProgressElement(element); }
template <> inline bool isElementOfType<const HTMLProgressElement>(const HTMLElement& element) { return isHTMLProgressElement(element); }
class HTMLShadowElement;
void isHTMLShadowElement(const HTMLShadowElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLShadowElement(const HTMLShadowElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLShadowElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::shadowTag);
}
inline bool isHTMLShadowElement(const Element& element) {
    return element.isHTMLElement() && isHTMLShadowElement(toHTMLElement(element));
}
inline bool isHTMLShadowElement(const HTMLElement* element) { return element && isHTMLShadowElement(*element); }
inline bool isHTMLShadowElement(const Element* element) { return element && isHTMLShadowElement(*element); }
template<typename T> inline bool isHTMLShadowElement(const PassRefPtr<T>& node) { return isHTMLShadowElement(node.get()); }
template<typename T> inline bool isHTMLShadowElement(const RefPtr<T>& node) { return isHTMLShadowElement(node.get()); }
inline bool isHTMLShadowElement(const Node& node) { return node.isElementNode() ? isHTMLShadowElement(toElement(node)) : false; }
inline bool isHTMLShadowElement(const Node* node) { return node && node->isElementNode() ? isHTMLShadowElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLShadowElement>(const Element& element) { return isHTMLShadowElement(element); }
template <> inline bool isElementOfType<const HTMLShadowElement>(const HTMLElement& element) { return isHTMLShadowElement(element); }
class HTMLSourceElement;
void isHTMLSourceElement(const HTMLSourceElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLSourceElement(const HTMLSourceElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLSourceElement(const HTMLElement& element) {
    if (element.isHTMLUnknownElement())
        return false;
    return element.hasLocalName(HTMLNames::sourceTag);
}
inline bool isHTMLSourceElement(const Element& element) {
    return element.isHTMLElement() && isHTMLSourceElement(toHTMLElement(element));
}
inline bool isHTMLSourceElement(const HTMLElement* element) { return element && isHTMLSourceElement(*element); }
inline bool isHTMLSourceElement(const Element* element) { return element && isHTMLSourceElement(*element); }
template<typename T> inline bool isHTMLSourceElement(const PassRefPtr<T>& node) { return isHTMLSourceElement(node.get()); }
template<typename T> inline bool isHTMLSourceElement(const RefPtr<T>& node) { return isHTMLSourceElement(node.get()); }
inline bool isHTMLSourceElement(const Node& node) { return node.isElementNode() ? isHTMLSourceElement(toElement(node)) : false; }
inline bool isHTMLSourceElement(const Node* node) { return node && node->isElementNode() ? isHTMLSourceElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLSourceElement>(const Element& element) { return isHTMLSourceElement(element); }
template <> inline bool isElementOfType<const HTMLSourceElement>(const HTMLElement& element) { return isHTMLSourceElement(element); }
class HTMLSpanElement;
void isHTMLSpanElement(const HTMLSpanElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLSpanElement(const HTMLSpanElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLSpanElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::spanTag);
}
inline bool isHTMLSpanElement(const Element& element) {
    return element.isHTMLElement() && isHTMLSpanElement(toHTMLElement(element));
}
inline bool isHTMLSpanElement(const HTMLElement* element) { return element && isHTMLSpanElement(*element); }
inline bool isHTMLSpanElement(const Element* element) { return element && isHTMLSpanElement(*element); }
template<typename T> inline bool isHTMLSpanElement(const PassRefPtr<T>& node) { return isHTMLSpanElement(node.get()); }
template<typename T> inline bool isHTMLSpanElement(const RefPtr<T>& node) { return isHTMLSpanElement(node.get()); }
inline bool isHTMLSpanElement(const Node& node) { return node.isElementNode() ? isHTMLSpanElement(toElement(node)) : false; }
inline bool isHTMLSpanElement(const Node* node) { return node && node->isElementNode() ? isHTMLSpanElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLSpanElement>(const Element& element) { return isHTMLSpanElement(element); }
template <> inline bool isElementOfType<const HTMLSpanElement>(const HTMLElement& element) { return isHTMLSpanElement(element); }
class HTMLTableCaptionElement;
void isHTMLTableCaptionElement(const HTMLTableCaptionElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLTableCaptionElement(const HTMLTableCaptionElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLTableCaptionElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::captionTag);
}
inline bool isHTMLTableCaptionElement(const Element& element) {
    return element.isHTMLElement() && isHTMLTableCaptionElement(toHTMLElement(element));
}
inline bool isHTMLTableCaptionElement(const HTMLElement* element) { return element && isHTMLTableCaptionElement(*element); }
inline bool isHTMLTableCaptionElement(const Element* element) { return element && isHTMLTableCaptionElement(*element); }
template<typename T> inline bool isHTMLTableCaptionElement(const PassRefPtr<T>& node) { return isHTMLTableCaptionElement(node.get()); }
template<typename T> inline bool isHTMLTableCaptionElement(const RefPtr<T>& node) { return isHTMLTableCaptionElement(node.get()); }
inline bool isHTMLTableCaptionElement(const Node& node) { return node.isElementNode() ? isHTMLTableCaptionElement(toElement(node)) : false; }
inline bool isHTMLTableCaptionElement(const Node* node) { return node && node->isElementNode() ? isHTMLTableCaptionElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLTableCaptionElement>(const Element& element) { return isHTMLTableCaptionElement(element); }
template <> inline bool isElementOfType<const HTMLTableCaptionElement>(const HTMLElement& element) { return isHTMLTableCaptionElement(element); }
class HTMLTableElement;
void isHTMLTableElement(const HTMLTableElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLTableElement(const HTMLTableElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLTableElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::tableTag);
}
inline bool isHTMLTableElement(const Element& element) {
    return element.isHTMLElement() && isHTMLTableElement(toHTMLElement(element));
}
inline bool isHTMLTableElement(const HTMLElement* element) { return element && isHTMLTableElement(*element); }
inline bool isHTMLTableElement(const Element* element) { return element && isHTMLTableElement(*element); }
template<typename T> inline bool isHTMLTableElement(const PassRefPtr<T>& node) { return isHTMLTableElement(node.get()); }
template<typename T> inline bool isHTMLTableElement(const RefPtr<T>& node) { return isHTMLTableElement(node.get()); }
inline bool isHTMLTableElement(const Node& node) { return node.isElementNode() ? isHTMLTableElement(toElement(node)) : false; }
inline bool isHTMLTableElement(const Node* node) { return node && node->isElementNode() ? isHTMLTableElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLTableElement>(const Element& element) { return isHTMLTableElement(element); }
template <> inline bool isElementOfType<const HTMLTableElement>(const HTMLElement& element) { return isHTMLTableElement(element); }
class HTMLTableRowElement;
void isHTMLTableRowElement(const HTMLTableRowElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLTableRowElement(const HTMLTableRowElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLTableRowElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::trTag);
}
inline bool isHTMLTableRowElement(const Element& element) {
    return element.isHTMLElement() && isHTMLTableRowElement(toHTMLElement(element));
}
inline bool isHTMLTableRowElement(const HTMLElement* element) { return element && isHTMLTableRowElement(*element); }
inline bool isHTMLTableRowElement(const Element* element) { return element && isHTMLTableRowElement(*element); }
template<typename T> inline bool isHTMLTableRowElement(const PassRefPtr<T>& node) { return isHTMLTableRowElement(node.get()); }
template<typename T> inline bool isHTMLTableRowElement(const RefPtr<T>& node) { return isHTMLTableRowElement(node.get()); }
inline bool isHTMLTableRowElement(const Node& node) { return node.isElementNode() ? isHTMLTableRowElement(toElement(node)) : false; }
inline bool isHTMLTableRowElement(const Node* node) { return node && node->isElementNode() ? isHTMLTableRowElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLTableRowElement>(const Element& element) { return isHTMLTableRowElement(element); }
template <> inline bool isElementOfType<const HTMLTableRowElement>(const HTMLElement& element) { return isHTMLTableRowElement(element); }
class HTMLTemplateElement;
void isHTMLTemplateElement(const HTMLTemplateElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLTemplateElement(const HTMLTemplateElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLTemplateElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::templateTag);
}
inline bool isHTMLTemplateElement(const Element& element) {
    return element.isHTMLElement() && isHTMLTemplateElement(toHTMLElement(element));
}
inline bool isHTMLTemplateElement(const HTMLElement* element) { return element && isHTMLTemplateElement(*element); }
inline bool isHTMLTemplateElement(const Element* element) { return element && isHTMLTemplateElement(*element); }
template<typename T> inline bool isHTMLTemplateElement(const PassRefPtr<T>& node) { return isHTMLTemplateElement(node.get()); }
template<typename T> inline bool isHTMLTemplateElement(const RefPtr<T>& node) { return isHTMLTemplateElement(node.get()); }
inline bool isHTMLTemplateElement(const Node& node) { return node.isElementNode() ? isHTMLTemplateElement(toElement(node)) : false; }
inline bool isHTMLTemplateElement(const Node* node) { return node && node->isElementNode() ? isHTMLTemplateElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLTemplateElement>(const Element& element) { return isHTMLTemplateElement(element); }
template <> inline bool isElementOfType<const HTMLTemplateElement>(const HTMLElement& element) { return isHTMLTemplateElement(element); }
class HTMLTitleElement;
void isHTMLTitleElement(const HTMLTitleElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLTitleElement(const HTMLTitleElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLTitleElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::titleTag);
}
inline bool isHTMLTitleElement(const Element& element) {
    return element.isHTMLElement() && isHTMLTitleElement(toHTMLElement(element));
}
inline bool isHTMLTitleElement(const HTMLElement* element) { return element && isHTMLTitleElement(*element); }
inline bool isHTMLTitleElement(const Element* element) { return element && isHTMLTitleElement(*element); }
template<typename T> inline bool isHTMLTitleElement(const PassRefPtr<T>& node) { return isHTMLTitleElement(node.get()); }
template<typename T> inline bool isHTMLTitleElement(const RefPtr<T>& node) { return isHTMLTitleElement(node.get()); }
inline bool isHTMLTitleElement(const Node& node) { return node.isElementNode() ? isHTMLTitleElement(toElement(node)) : false; }
inline bool isHTMLTitleElement(const Node* node) { return node && node->isElementNode() ? isHTMLTitleElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLTitleElement>(const Element& element) { return isHTMLTitleElement(element); }
template <> inline bool isElementOfType<const HTMLTitleElement>(const HTMLElement& element) { return isHTMLTitleElement(element); }
class HTMLTrackElement;
void isHTMLTrackElement(const HTMLTrackElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLTrackElement(const HTMLTrackElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLTrackElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::trackTag);
}
inline bool isHTMLTrackElement(const Element& element) {
    return element.isHTMLElement() && isHTMLTrackElement(toHTMLElement(element));
}
inline bool isHTMLTrackElement(const HTMLElement* element) { return element && isHTMLTrackElement(*element); }
inline bool isHTMLTrackElement(const Element* element) { return element && isHTMLTrackElement(*element); }
template<typename T> inline bool isHTMLTrackElement(const PassRefPtr<T>& node) { return isHTMLTrackElement(node.get()); }
template<typename T> inline bool isHTMLTrackElement(const RefPtr<T>& node) { return isHTMLTrackElement(node.get()); }
inline bool isHTMLTrackElement(const Node& node) { return node.isElementNode() ? isHTMLTrackElement(toElement(node)) : false; }
inline bool isHTMLTrackElement(const Node* node) { return node && node->isElementNode() ? isHTMLTrackElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLTrackElement>(const Element& element) { return isHTMLTrackElement(element); }
template <> inline bool isElementOfType<const HTMLTrackElement>(const HTMLElement& element) { return isHTMLTrackElement(element); }
class HTMLUListElement;
void isHTMLUListElement(const HTMLUListElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLUListElement(const HTMLUListElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLUListElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::ulTag);
}
inline bool isHTMLUListElement(const Element& element) {
    return element.isHTMLElement() && isHTMLUListElement(toHTMLElement(element));
}
inline bool isHTMLUListElement(const HTMLElement* element) { return element && isHTMLUListElement(*element); }
inline bool isHTMLUListElement(const Element* element) { return element && isHTMLUListElement(*element); }
template<typename T> inline bool isHTMLUListElement(const PassRefPtr<T>& node) { return isHTMLUListElement(node.get()); }
template<typename T> inline bool isHTMLUListElement(const RefPtr<T>& node) { return isHTMLUListElement(node.get()); }
inline bool isHTMLUListElement(const Node& node) { return node.isElementNode() ? isHTMLUListElement(toElement(node)) : false; }
inline bool isHTMLUListElement(const Node* node) { return node && node->isElementNode() ? isHTMLUListElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLUListElement>(const Element& element) { return isHTMLUListElement(element); }
template <> inline bool isElementOfType<const HTMLUListElement>(const HTMLElement& element) { return isHTMLUListElement(element); }
class HTMLVideoElement;
void isHTMLVideoElement(const HTMLVideoElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLVideoElement(const HTMLVideoElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLVideoElement(const HTMLElement& element) {
    if (element.isHTMLUnknownElement())
        return false;
    return element.hasLocalName(HTMLNames::videoTag);
}
inline bool isHTMLVideoElement(const Element& element) {
    return element.isHTMLElement() && isHTMLVideoElement(toHTMLElement(element));
}
inline bool isHTMLVideoElement(const HTMLElement* element) { return element && isHTMLVideoElement(*element); }
inline bool isHTMLVideoElement(const Element* element) { return element && isHTMLVideoElement(*element); }
template<typename T> inline bool isHTMLVideoElement(const PassRefPtr<T>& node) { return isHTMLVideoElement(node.get()); }
template<typename T> inline bool isHTMLVideoElement(const RefPtr<T>& node) { return isHTMLVideoElement(node.get()); }
inline bool isHTMLVideoElement(const Node& node) { return node.isElementNode() ? isHTMLVideoElement(toElement(node)) : false; }
inline bool isHTMLVideoElement(const Node* node) { return node && node->isElementNode() ? isHTMLVideoElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLVideoElement>(const Element& element) { return isHTMLVideoElement(element); }
template <> inline bool isElementOfType<const HTMLVideoElement>(const HTMLElement& element) { return isHTMLVideoElement(element); }
class HTMLDialogElement;
void isHTMLDialogElement(const HTMLDialogElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLDialogElement(const HTMLDialogElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLDialogElement(const HTMLElement& element) {
    if (element.isHTMLUnknownElement())
        return false;
    return element.hasLocalName(HTMLNames::dialogTag);
}
inline bool isHTMLDialogElement(const Element& element) {
    return element.isHTMLElement() && isHTMLDialogElement(toHTMLElement(element));
}
inline bool isHTMLDialogElement(const HTMLElement* element) { return element && isHTMLDialogElement(*element); }
inline bool isHTMLDialogElement(const Element* element) { return element && isHTMLDialogElement(*element); }
template<typename T> inline bool isHTMLDialogElement(const PassRefPtr<T>& node) { return isHTMLDialogElement(node.get()); }
template<typename T> inline bool isHTMLDialogElement(const RefPtr<T>& node) { return isHTMLDialogElement(node.get()); }
inline bool isHTMLDialogElement(const Node& node) { return node.isElementNode() ? isHTMLDialogElement(toElement(node)) : false; }
inline bool isHTMLDialogElement(const Node* node) { return node && node->isElementNode() ? isHTMLDialogElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLDialogElement>(const Element& element) { return isHTMLDialogElement(element); }
template <> inline bool isElementOfType<const HTMLDialogElement>(const HTMLElement& element) { return isHTMLDialogElement(element); }
class HTMLButtonElement;
void isHTMLButtonElement(const HTMLButtonElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLButtonElement(const HTMLButtonElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLButtonElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::buttonTag);
}
inline bool isHTMLButtonElement(const Element& element) {
    return element.isHTMLElement() && isHTMLButtonElement(toHTMLElement(element));
}
inline bool isHTMLButtonElement(const HTMLElement* element) { return element && isHTMLButtonElement(*element); }
inline bool isHTMLButtonElement(const Element* element) { return element && isHTMLButtonElement(*element); }
template<typename T> inline bool isHTMLButtonElement(const PassRefPtr<T>& node) { return isHTMLButtonElement(node.get()); }
template<typename T> inline bool isHTMLButtonElement(const RefPtr<T>& node) { return isHTMLButtonElement(node.get()); }
inline bool isHTMLButtonElement(const Node& node) { return node.isElementNode() ? isHTMLButtonElement(toElement(node)) : false; }
inline bool isHTMLButtonElement(const Node* node) { return node && node->isElementNode() ? isHTMLButtonElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLButtonElement>(const Element& element) { return isHTMLButtonElement(element); }
template <> inline bool isElementOfType<const HTMLButtonElement>(const HTMLElement& element) { return isHTMLButtonElement(element); }
class HTMLFieldSetElement;
void isHTMLFieldSetElement(const HTMLFieldSetElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLFieldSetElement(const HTMLFieldSetElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLFieldSetElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::fieldsetTag);
}
inline bool isHTMLFieldSetElement(const Element& element) {
    return element.isHTMLElement() && isHTMLFieldSetElement(toHTMLElement(element));
}
inline bool isHTMLFieldSetElement(const HTMLElement* element) { return element && isHTMLFieldSetElement(*element); }
inline bool isHTMLFieldSetElement(const Element* element) { return element && isHTMLFieldSetElement(*element); }
template<typename T> inline bool isHTMLFieldSetElement(const PassRefPtr<T>& node) { return isHTMLFieldSetElement(node.get()); }
template<typename T> inline bool isHTMLFieldSetElement(const RefPtr<T>& node) { return isHTMLFieldSetElement(node.get()); }
inline bool isHTMLFieldSetElement(const Node& node) { return node.isElementNode() ? isHTMLFieldSetElement(toElement(node)) : false; }
inline bool isHTMLFieldSetElement(const Node* node) { return node && node->isElementNode() ? isHTMLFieldSetElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLFieldSetElement>(const Element& element) { return isHTMLFieldSetElement(element); }
template <> inline bool isElementOfType<const HTMLFieldSetElement>(const HTMLElement& element) { return isHTMLFieldSetElement(element); }
class HTMLKeygenElement;
void isHTMLKeygenElement(const HTMLKeygenElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLKeygenElement(const HTMLKeygenElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLKeygenElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::keygenTag);
}
inline bool isHTMLKeygenElement(const Element& element) {
    return element.isHTMLElement() && isHTMLKeygenElement(toHTMLElement(element));
}
inline bool isHTMLKeygenElement(const HTMLElement* element) { return element && isHTMLKeygenElement(*element); }
inline bool isHTMLKeygenElement(const Element* element) { return element && isHTMLKeygenElement(*element); }
template<typename T> inline bool isHTMLKeygenElement(const PassRefPtr<T>& node) { return isHTMLKeygenElement(node.get()); }
template<typename T> inline bool isHTMLKeygenElement(const RefPtr<T>& node) { return isHTMLKeygenElement(node.get()); }
inline bool isHTMLKeygenElement(const Node& node) { return node.isElementNode() ? isHTMLKeygenElement(toElement(node)) : false; }
inline bool isHTMLKeygenElement(const Node* node) { return node && node->isElementNode() ? isHTMLKeygenElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLKeygenElement>(const Element& element) { return isHTMLKeygenElement(element); }
template <> inline bool isElementOfType<const HTMLKeygenElement>(const HTMLElement& element) { return isHTMLKeygenElement(element); }
class HTMLOutputElement;
void isHTMLOutputElement(const HTMLOutputElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLOutputElement(const HTMLOutputElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLOutputElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::outputTag);
}
inline bool isHTMLOutputElement(const Element& element) {
    return element.isHTMLElement() && isHTMLOutputElement(toHTMLElement(element));
}
inline bool isHTMLOutputElement(const HTMLElement* element) { return element && isHTMLOutputElement(*element); }
inline bool isHTMLOutputElement(const Element* element) { return element && isHTMLOutputElement(*element); }
template<typename T> inline bool isHTMLOutputElement(const PassRefPtr<T>& node) { return isHTMLOutputElement(node.get()); }
template<typename T> inline bool isHTMLOutputElement(const RefPtr<T>& node) { return isHTMLOutputElement(node.get()); }
inline bool isHTMLOutputElement(const Node& node) { return node.isElementNode() ? isHTMLOutputElement(toElement(node)) : false; }
inline bool isHTMLOutputElement(const Node* node) { return node && node->isElementNode() ? isHTMLOutputElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLOutputElement>(const Element& element) { return isHTMLOutputElement(element); }
template <> inline bool isElementOfType<const HTMLOutputElement>(const HTMLElement& element) { return isHTMLOutputElement(element); }
class HTMLSelectElement;
void isHTMLSelectElement(const HTMLSelectElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLSelectElement(const HTMLSelectElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLSelectElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::selectTag);
}
inline bool isHTMLSelectElement(const Element& element) {
    return element.isHTMLElement() && isHTMLSelectElement(toHTMLElement(element));
}
inline bool isHTMLSelectElement(const HTMLElement* element) { return element && isHTMLSelectElement(*element); }
inline bool isHTMLSelectElement(const Element* element) { return element && isHTMLSelectElement(*element); }
template<typename T> inline bool isHTMLSelectElement(const PassRefPtr<T>& node) { return isHTMLSelectElement(node.get()); }
template<typename T> inline bool isHTMLSelectElement(const RefPtr<T>& node) { return isHTMLSelectElement(node.get()); }
inline bool isHTMLSelectElement(const Node& node) { return node.isElementNode() ? isHTMLSelectElement(toElement(node)) : false; }
inline bool isHTMLSelectElement(const Node* node) { return node && node->isElementNode() ? isHTMLSelectElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLSelectElement>(const Element& element) { return isHTMLSelectElement(element); }
template <> inline bool isElementOfType<const HTMLSelectElement>(const HTMLElement& element) { return isHTMLSelectElement(element); }
class HTMLTextAreaElement;
void isHTMLTextAreaElement(const HTMLTextAreaElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLTextAreaElement(const HTMLTextAreaElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLTextAreaElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::textareaTag);
}
inline bool isHTMLTextAreaElement(const Element& element) {
    return element.isHTMLElement() && isHTMLTextAreaElement(toHTMLElement(element));
}
inline bool isHTMLTextAreaElement(const HTMLElement* element) { return element && isHTMLTextAreaElement(*element); }
inline bool isHTMLTextAreaElement(const Element* element) { return element && isHTMLTextAreaElement(*element); }
template<typename T> inline bool isHTMLTextAreaElement(const PassRefPtr<T>& node) { return isHTMLTextAreaElement(node.get()); }
template<typename T> inline bool isHTMLTextAreaElement(const RefPtr<T>& node) { return isHTMLTextAreaElement(node.get()); }
inline bool isHTMLTextAreaElement(const Node& node) { return node.isElementNode() ? isHTMLTextAreaElement(toElement(node)) : false; }
inline bool isHTMLTextAreaElement(const Node* node) { return node && node->isElementNode() ? isHTMLTextAreaElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLTextAreaElement>(const Element& element) { return isHTMLTextAreaElement(element); }
template <> inline bool isElementOfType<const HTMLTextAreaElement>(const HTMLElement& element) { return isHTMLTextAreaElement(element); }
class HTMLAppletElement;
void isHTMLAppletElement(const HTMLAppletElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLAppletElement(const HTMLAppletElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLAppletElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::appletTag);
}
inline bool isHTMLAppletElement(const Element& element) {
    return element.isHTMLElement() && isHTMLAppletElement(toHTMLElement(element));
}
inline bool isHTMLAppletElement(const HTMLElement* element) { return element && isHTMLAppletElement(*element); }
inline bool isHTMLAppletElement(const Element* element) { return element && isHTMLAppletElement(*element); }
template<typename T> inline bool isHTMLAppletElement(const PassRefPtr<T>& node) { return isHTMLAppletElement(node.get()); }
template<typename T> inline bool isHTMLAppletElement(const RefPtr<T>& node) { return isHTMLAppletElement(node.get()); }
inline bool isHTMLAppletElement(const Node& node) { return node.isElementNode() ? isHTMLAppletElement(toElement(node)) : false; }
inline bool isHTMLAppletElement(const Node* node) { return node && node->isElementNode() ? isHTMLAppletElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLAppletElement>(const Element& element) { return isHTMLAppletElement(element); }
template <> inline bool isElementOfType<const HTMLAppletElement>(const HTMLElement& element) { return isHTMLAppletElement(element); }
class HTMLEmbedElement;
void isHTMLEmbedElement(const HTMLEmbedElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLEmbedElement(const HTMLEmbedElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLEmbedElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::embedTag);
}
inline bool isHTMLEmbedElement(const Element& element) {
    return element.isHTMLElement() && isHTMLEmbedElement(toHTMLElement(element));
}
inline bool isHTMLEmbedElement(const HTMLElement* element) { return element && isHTMLEmbedElement(*element); }
inline bool isHTMLEmbedElement(const Element* element) { return element && isHTMLEmbedElement(*element); }
template<typename T> inline bool isHTMLEmbedElement(const PassRefPtr<T>& node) { return isHTMLEmbedElement(node.get()); }
template<typename T> inline bool isHTMLEmbedElement(const RefPtr<T>& node) { return isHTMLEmbedElement(node.get()); }
inline bool isHTMLEmbedElement(const Node& node) { return node.isElementNode() ? isHTMLEmbedElement(toElement(node)) : false; }
inline bool isHTMLEmbedElement(const Node* node) { return node && node->isElementNode() ? isHTMLEmbedElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLEmbedElement>(const Element& element) { return isHTMLEmbedElement(element); }
template <> inline bool isElementOfType<const HTMLEmbedElement>(const HTMLElement& element) { return isHTMLEmbedElement(element); }
class HTMLLinkElement;
void isHTMLLinkElement(const HTMLLinkElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLLinkElement(const HTMLLinkElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLLinkElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::linkTag);
}
inline bool isHTMLLinkElement(const Element& element) {
    return element.isHTMLElement() && isHTMLLinkElement(toHTMLElement(element));
}
inline bool isHTMLLinkElement(const HTMLElement* element) { return element && isHTMLLinkElement(*element); }
inline bool isHTMLLinkElement(const Element* element) { return element && isHTMLLinkElement(*element); }
template<typename T> inline bool isHTMLLinkElement(const PassRefPtr<T>& node) { return isHTMLLinkElement(node.get()); }
template<typename T> inline bool isHTMLLinkElement(const RefPtr<T>& node) { return isHTMLLinkElement(node.get()); }
inline bool isHTMLLinkElement(const Node& node) { return node.isElementNode() ? isHTMLLinkElement(toElement(node)) : false; }
inline bool isHTMLLinkElement(const Node* node) { return node && node->isElementNode() ? isHTMLLinkElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLLinkElement>(const Element& element) { return isHTMLLinkElement(element); }
template <> inline bool isElementOfType<const HTMLLinkElement>(const HTMLElement& element) { return isHTMLLinkElement(element); }
class HTMLScriptElement;
void isHTMLScriptElement(const HTMLScriptElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLScriptElement(const HTMLScriptElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLScriptElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::scriptTag);
}
inline bool isHTMLScriptElement(const Element& element) {
    return element.isHTMLElement() && isHTMLScriptElement(toHTMLElement(element));
}
inline bool isHTMLScriptElement(const HTMLElement* element) { return element && isHTMLScriptElement(*element); }
inline bool isHTMLScriptElement(const Element* element) { return element && isHTMLScriptElement(*element); }
template<typename T> inline bool isHTMLScriptElement(const PassRefPtr<T>& node) { return isHTMLScriptElement(node.get()); }
template<typename T> inline bool isHTMLScriptElement(const RefPtr<T>& node) { return isHTMLScriptElement(node.get()); }
inline bool isHTMLScriptElement(const Node& node) { return node.isElementNode() ? isHTMLScriptElement(toElement(node)) : false; }
inline bool isHTMLScriptElement(const Node* node) { return node && node->isElementNode() ? isHTMLScriptElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLScriptElement>(const Element& element) { return isHTMLScriptElement(element); }
template <> inline bool isElementOfType<const HTMLScriptElement>(const HTMLElement& element) { return isHTMLScriptElement(element); }
class HTMLStyleElement;
void isHTMLStyleElement(const HTMLStyleElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLStyleElement(const HTMLStyleElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLStyleElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::styleTag);
}
inline bool isHTMLStyleElement(const Element& element) {
    return element.isHTMLElement() && isHTMLStyleElement(toHTMLElement(element));
}
inline bool isHTMLStyleElement(const HTMLElement* element) { return element && isHTMLStyleElement(*element); }
inline bool isHTMLStyleElement(const Element* element) { return element && isHTMLStyleElement(*element); }
template<typename T> inline bool isHTMLStyleElement(const PassRefPtr<T>& node) { return isHTMLStyleElement(node.get()); }
template<typename T> inline bool isHTMLStyleElement(const RefPtr<T>& node) { return isHTMLStyleElement(node.get()); }
inline bool isHTMLStyleElement(const Node& node) { return node.isElementNode() ? isHTMLStyleElement(toElement(node)) : false; }
inline bool isHTMLStyleElement(const Node* node) { return node && node->isElementNode() ? isHTMLStyleElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLStyleElement>(const Element& element) { return isHTMLStyleElement(element); }
template <> inline bool isElementOfType<const HTMLStyleElement>(const HTMLElement& element) { return isHTMLStyleElement(element); }
class HTMLImageElement;
void isHTMLImageElement(const HTMLImageElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLImageElement(const HTMLImageElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLImageElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::imgTag);
}
inline bool isHTMLImageElement(const Element& element) {
    return element.isHTMLElement() && isHTMLImageElement(toHTMLElement(element));
}
inline bool isHTMLImageElement(const HTMLElement* element) { return element && isHTMLImageElement(*element); }
inline bool isHTMLImageElement(const Element* element) { return element && isHTMLImageElement(*element); }
template<typename T> inline bool isHTMLImageElement(const PassRefPtr<T>& node) { return isHTMLImageElement(node.get()); }
template<typename T> inline bool isHTMLImageElement(const RefPtr<T>& node) { return isHTMLImageElement(node.get()); }
inline bool isHTMLImageElement(const Node& node) { return node.isElementNode() ? isHTMLImageElement(toElement(node)) : false; }
inline bool isHTMLImageElement(const Node* node) { return node && node->isElementNode() ? isHTMLImageElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLImageElement>(const Element& element) { return isHTMLImageElement(element); }
template <> inline bool isElementOfType<const HTMLImageElement>(const HTMLElement& element) { return isHTMLImageElement(element); }
class HTMLInputElement;
void isHTMLInputElement(const HTMLInputElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLInputElement(const HTMLInputElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLInputElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::inputTag);
}
inline bool isHTMLInputElement(const Element& element) {
    return element.isHTMLElement() && isHTMLInputElement(toHTMLElement(element));
}
inline bool isHTMLInputElement(const HTMLElement* element) { return element && isHTMLInputElement(*element); }
inline bool isHTMLInputElement(const Element* element) { return element && isHTMLInputElement(*element); }
template<typename T> inline bool isHTMLInputElement(const PassRefPtr<T>& node) { return isHTMLInputElement(node.get()); }
template<typename T> inline bool isHTMLInputElement(const RefPtr<T>& node) { return isHTMLInputElement(node.get()); }
inline bool isHTMLInputElement(const Node& node) { return node.isElementNode() ? isHTMLInputElement(toElement(node)) : false; }
inline bool isHTMLInputElement(const Node* node) { return node && node->isElementNode() ? isHTMLInputElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLInputElement>(const Element& element) { return isHTMLInputElement(element); }
template <> inline bool isElementOfType<const HTMLInputElement>(const HTMLElement& element) { return isHTMLInputElement(element); }
class HTMLObjectElement;
void isHTMLObjectElement(const HTMLObjectElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLObjectElement(const HTMLObjectElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLObjectElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::objectTag);
}
inline bool isHTMLObjectElement(const Element& element) {
    return element.isHTMLElement() && isHTMLObjectElement(toHTMLElement(element));
}
inline bool isHTMLObjectElement(const HTMLElement* element) { return element && isHTMLObjectElement(*element); }
inline bool isHTMLObjectElement(const Element* element) { return element && isHTMLObjectElement(*element); }
template<typename T> inline bool isHTMLObjectElement(const PassRefPtr<T>& node) { return isHTMLObjectElement(node.get()); }
template<typename T> inline bool isHTMLObjectElement(const RefPtr<T>& node) { return isHTMLObjectElement(node.get()); }
inline bool isHTMLObjectElement(const Node& node) { return node.isElementNode() ? isHTMLObjectElement(toElement(node)) : false; }
inline bool isHTMLObjectElement(const Node* node) { return node && node->isElementNode() ? isHTMLObjectElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLObjectElement>(const Element& element) { return isHTMLObjectElement(element); }
template <> inline bool isElementOfType<const HTMLObjectElement>(const HTMLElement& element) { return isHTMLObjectElement(element); }
class HTMLBDIElement;
void isHTMLBDIElement(const HTMLBDIElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLBDIElement(const HTMLBDIElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLBDIElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::bdiTag);
}
inline bool isHTMLBDIElement(const Element& element) {
    return element.isHTMLElement() && isHTMLBDIElement(toHTMLElement(element));
}
inline bool isHTMLBDIElement(const HTMLElement* element) { return element && isHTMLBDIElement(*element); }
inline bool isHTMLBDIElement(const Element* element) { return element && isHTMLBDIElement(*element); }
template<typename T> inline bool isHTMLBDIElement(const PassRefPtr<T>& node) { return isHTMLBDIElement(node.get()); }
template<typename T> inline bool isHTMLBDIElement(const RefPtr<T>& node) { return isHTMLBDIElement(node.get()); }
inline bool isHTMLBDIElement(const Node& node) { return node.isElementNode() ? isHTMLBDIElement(toElement(node)) : false; }
inline bool isHTMLBDIElement(const Node* node) { return node && node->isElementNode() ? isHTMLBDIElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLBDIElement>(const Element& element) { return isHTMLBDIElement(element); }
template <> inline bool isElementOfType<const HTMLBDIElement>(const HTMLElement& element) { return isHTMLBDIElement(element); }
class HTMLNoEmbedElement;
void isHTMLNoEmbedElement(const HTMLNoEmbedElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLNoEmbedElement(const HTMLNoEmbedElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLNoEmbedElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::noembedTag);
}
inline bool isHTMLNoEmbedElement(const Element& element) {
    return element.isHTMLElement() && isHTMLNoEmbedElement(toHTMLElement(element));
}
inline bool isHTMLNoEmbedElement(const HTMLElement* element) { return element && isHTMLNoEmbedElement(*element); }
inline bool isHTMLNoEmbedElement(const Element* element) { return element && isHTMLNoEmbedElement(*element); }
template<typename T> inline bool isHTMLNoEmbedElement(const PassRefPtr<T>& node) { return isHTMLNoEmbedElement(node.get()); }
template<typename T> inline bool isHTMLNoEmbedElement(const RefPtr<T>& node) { return isHTMLNoEmbedElement(node.get()); }
inline bool isHTMLNoEmbedElement(const Node& node) { return node.isElementNode() ? isHTMLNoEmbedElement(toElement(node)) : false; }
inline bool isHTMLNoEmbedElement(const Node* node) { return node && node->isElementNode() ? isHTMLNoEmbedElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLNoEmbedElement>(const Element& element) { return isHTMLNoEmbedElement(element); }
template <> inline bool isElementOfType<const HTMLNoEmbedElement>(const HTMLElement& element) { return isHTMLNoEmbedElement(element); }
class HTMLNoScriptElement;
void isHTMLNoScriptElement(const HTMLNoScriptElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLNoScriptElement(const HTMLNoScriptElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLNoScriptElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::noscriptTag);
}
inline bool isHTMLNoScriptElement(const Element& element) {
    return element.isHTMLElement() && isHTMLNoScriptElement(toHTMLElement(element));
}
inline bool isHTMLNoScriptElement(const HTMLElement* element) { return element && isHTMLNoScriptElement(*element); }
inline bool isHTMLNoScriptElement(const Element* element) { return element && isHTMLNoScriptElement(*element); }
template<typename T> inline bool isHTMLNoScriptElement(const PassRefPtr<T>& node) { return isHTMLNoScriptElement(node.get()); }
template<typename T> inline bool isHTMLNoScriptElement(const RefPtr<T>& node) { return isHTMLNoScriptElement(node.get()); }
inline bool isHTMLNoScriptElement(const Node& node) { return node.isElementNode() ? isHTMLNoScriptElement(toElement(node)) : false; }
inline bool isHTMLNoScriptElement(const Node* node) { return node && node->isElementNode() ? isHTMLNoScriptElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLNoScriptElement>(const Element& element) { return isHTMLNoScriptElement(element); }
template <> inline bool isElementOfType<const HTMLNoScriptElement>(const HTMLElement& element) { return isHTMLNoScriptElement(element); }
class HTMLRTElement;
void isHTMLRTElement(const HTMLRTElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLRTElement(const HTMLRTElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLRTElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::rtTag);
}
inline bool isHTMLRTElement(const Element& element) {
    return element.isHTMLElement() && isHTMLRTElement(toHTMLElement(element));
}
inline bool isHTMLRTElement(const HTMLElement* element) { return element && isHTMLRTElement(*element); }
inline bool isHTMLRTElement(const Element* element) { return element && isHTMLRTElement(*element); }
template<typename T> inline bool isHTMLRTElement(const PassRefPtr<T>& node) { return isHTMLRTElement(node.get()); }
template<typename T> inline bool isHTMLRTElement(const RefPtr<T>& node) { return isHTMLRTElement(node.get()); }
inline bool isHTMLRTElement(const Node& node) { return node.isElementNode() ? isHTMLRTElement(toElement(node)) : false; }
inline bool isHTMLRTElement(const Node* node) { return node && node->isElementNode() ? isHTMLRTElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLRTElement>(const Element& element) { return isHTMLRTElement(element); }
template <> inline bool isElementOfType<const HTMLRTElement>(const HTMLElement& element) { return isHTMLRTElement(element); }
class HTMLRubyElement;
void isHTMLRubyElement(const HTMLRubyElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLRubyElement(const HTMLRubyElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLRubyElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::rubyTag);
}
inline bool isHTMLRubyElement(const Element& element) {
    return element.isHTMLElement() && isHTMLRubyElement(toHTMLElement(element));
}
inline bool isHTMLRubyElement(const HTMLElement* element) { return element && isHTMLRubyElement(*element); }
inline bool isHTMLRubyElement(const Element* element) { return element && isHTMLRubyElement(*element); }
template<typename T> inline bool isHTMLRubyElement(const PassRefPtr<T>& node) { return isHTMLRubyElement(node.get()); }
template<typename T> inline bool isHTMLRubyElement(const RefPtr<T>& node) { return isHTMLRubyElement(node.get()); }
inline bool isHTMLRubyElement(const Node& node) { return node.isElementNode() ? isHTMLRubyElement(toElement(node)) : false; }
inline bool isHTMLRubyElement(const Node* node) { return node && node->isElementNode() ? isHTMLRubyElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLRubyElement>(const Element& element) { return isHTMLRubyElement(element); }
template <> inline bool isElementOfType<const HTMLRubyElement>(const HTMLElement& element) { return isHTMLRubyElement(element); }
class HTMLSummaryElement;
void isHTMLSummaryElement(const HTMLSummaryElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLSummaryElement(const HTMLSummaryElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLSummaryElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::summaryTag);
}
inline bool isHTMLSummaryElement(const Element& element) {
    return element.isHTMLElement() && isHTMLSummaryElement(toHTMLElement(element));
}
inline bool isHTMLSummaryElement(const HTMLElement* element) { return element && isHTMLSummaryElement(*element); }
inline bool isHTMLSummaryElement(const Element* element) { return element && isHTMLSummaryElement(*element); }
template<typename T> inline bool isHTMLSummaryElement(const PassRefPtr<T>& node) { return isHTMLSummaryElement(node.get()); }
template<typename T> inline bool isHTMLSummaryElement(const RefPtr<T>& node) { return isHTMLSummaryElement(node.get()); }
inline bool isHTMLSummaryElement(const Node& node) { return node.isElementNode() ? isHTMLSummaryElement(toElement(node)) : false; }
inline bool isHTMLSummaryElement(const Node* node) { return node && node->isElementNode() ? isHTMLSummaryElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLSummaryElement>(const Element& element) { return isHTMLSummaryElement(element); }
template <> inline bool isElementOfType<const HTMLSummaryElement>(const HTMLElement& element) { return isHTMLSummaryElement(element); }
class HTMLWBRElement;
void isHTMLWBRElement(const HTMLWBRElement&); // Catch unnecessary runtime check of type known at compile time.
void isHTMLWBRElement(const HTMLWBRElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isHTMLWBRElement(const HTMLElement& element) {
    return element.hasLocalName(HTMLNames::wbrTag);
}
inline bool isHTMLWBRElement(const Element& element) {
    return element.isHTMLElement() && isHTMLWBRElement(toHTMLElement(element));
}
inline bool isHTMLWBRElement(const HTMLElement* element) { return element && isHTMLWBRElement(*element); }
inline bool isHTMLWBRElement(const Element* element) { return element && isHTMLWBRElement(*element); }
template<typename T> inline bool isHTMLWBRElement(const PassRefPtr<T>& node) { return isHTMLWBRElement(node.get()); }
template<typename T> inline bool isHTMLWBRElement(const RefPtr<T>& node) { return isHTMLWBRElement(node.get()); }
inline bool isHTMLWBRElement(const Node& node) { return node.isElementNode() ? isHTMLWBRElement(toElement(node)) : false; }
inline bool isHTMLWBRElement(const Node* node) { return node && node->isElementNode() ? isHTMLWBRElement(*toElement(node)) : false; }
template <> inline bool isElementOfType<const HTMLWBRElement>(const Element& element) { return isHTMLWBRElement(element); }
template <> inline bool isElementOfType<const HTMLWBRElement>(const HTMLElement& element) { return isHTMLWBRElement(element); }
// Using macros because the types are forward-declared and we don't want to use reinterpret_cast in the
// casting functions above. reinterpret_cast would be unsafe due to multiple inheritence.

#define toHTMLAnchorElement(x) WebCore::toElement<WebCore::HTMLAnchorElement>(x)
#define toHTMLAreaElement(x) WebCore::toElement<WebCore::HTMLAreaElement>(x)
#define toHTMLAudioElement(x) WebCore::toElement<WebCore::HTMLAudioElement>(x)
#define toHTMLBRElement(x) WebCore::toElement<WebCore::HTMLBRElement>(x)
#define toHTMLBaseElement(x) WebCore::toElement<WebCore::HTMLBaseElement>(x)
#define toHTMLBodyElement(x) WebCore::toElement<WebCore::HTMLBodyElement>(x)
#define toHTMLCanvasElement(x) WebCore::toElement<WebCore::HTMLCanvasElement>(x)
#define toHTMLContentElement(x) WebCore::toElement<WebCore::HTMLContentElement>(x)
#define toHTMLDListElement(x) WebCore::toElement<WebCore::HTMLDListElement>(x)
#define toHTMLDataListElement(x) WebCore::toElement<WebCore::HTMLDataListElement>(x)
#define toHTMLDetailsElement(x) WebCore::toElement<WebCore::HTMLDetailsElement>(x)
#define toHTMLDirectoryElement(x) WebCore::toElement<WebCore::HTMLDirectoryElement>(x)
#define toHTMLDivElement(x) WebCore::toElement<WebCore::HTMLDivElement>(x)
#define toHTMLFontElement(x) WebCore::toElement<WebCore::HTMLFontElement>(x)
#define toHTMLFormElement(x) WebCore::toElement<WebCore::HTMLFormElement>(x)
#define toHTMLFrameElement(x) WebCore::toElement<WebCore::HTMLFrameElement>(x)
#define toHTMLFrameSetElement(x) WebCore::toElement<WebCore::HTMLFrameSetElement>(x)
#define toHTMLHRElement(x) WebCore::toElement<WebCore::HTMLHRElement>(x)
#define toHTMLHeadElement(x) WebCore::toElement<WebCore::HTMLHeadElement>(x)
#define toHTMLHtmlElement(x) WebCore::toElement<WebCore::HTMLHtmlElement>(x)
#define toHTMLIFrameElement(x) WebCore::toElement<WebCore::HTMLIFrameElement>(x)
#define toHTMLLIElement(x) WebCore::toElement<WebCore::HTMLLIElement>(x)
#define toHTMLLabelElement(x) WebCore::toElement<WebCore::HTMLLabelElement>(x)
#define toHTMLLegendElement(x) WebCore::toElement<WebCore::HTMLLegendElement>(x)
#define toHTMLMapElement(x) WebCore::toElement<WebCore::HTMLMapElement>(x)
#define toHTMLMarqueeElement(x) WebCore::toElement<WebCore::HTMLMarqueeElement>(x)
#define toHTMLMenuElement(x) WebCore::toElement<WebCore::HTMLMenuElement>(x)
#define toHTMLMetaElement(x) WebCore::toElement<WebCore::HTMLMetaElement>(x)
#define toHTMLMeterElement(x) WebCore::toElement<WebCore::HTMLMeterElement>(x)
#define toHTMLOListElement(x) WebCore::toElement<WebCore::HTMLOListElement>(x)
#define toHTMLOptGroupElement(x) WebCore::toElement<WebCore::HTMLOptGroupElement>(x)
#define toHTMLOptionElement(x) WebCore::toElement<WebCore::HTMLOptionElement>(x)
#define toHTMLParagraphElement(x) WebCore::toElement<WebCore::HTMLParagraphElement>(x)
#define toHTMLParamElement(x) WebCore::toElement<WebCore::HTMLParamElement>(x)
#define toHTMLPictureElement(x) WebCore::toElement<WebCore::HTMLPictureElement>(x)
#define toHTMLProgressElement(x) WebCore::toElement<WebCore::HTMLProgressElement>(x)
#define toHTMLShadowElement(x) WebCore::toElement<WebCore::HTMLShadowElement>(x)
#define toHTMLSourceElement(x) WebCore::toElement<WebCore::HTMLSourceElement>(x)
#define toHTMLSpanElement(x) WebCore::toElement<WebCore::HTMLSpanElement>(x)
#define toHTMLTableCaptionElement(x) WebCore::toElement<WebCore::HTMLTableCaptionElement>(x)
#define toHTMLTableElement(x) WebCore::toElement<WebCore::HTMLTableElement>(x)
#define toHTMLTableRowElement(x) WebCore::toElement<WebCore::HTMLTableRowElement>(x)
#define toHTMLTemplateElement(x) WebCore::toElement<WebCore::HTMLTemplateElement>(x)
#define toHTMLTitleElement(x) WebCore::toElement<WebCore::HTMLTitleElement>(x)
#define toHTMLTrackElement(x) WebCore::toElement<WebCore::HTMLTrackElement>(x)
#define toHTMLUListElement(x) WebCore::toElement<WebCore::HTMLUListElement>(x)
#define toHTMLVideoElement(x) WebCore::toElement<WebCore::HTMLVideoElement>(x)
#define toHTMLDialogElement(x) WebCore::toElement<WebCore::HTMLDialogElement>(x)
#define toHTMLButtonElement(x) WebCore::toElement<WebCore::HTMLButtonElement>(x)
#define toHTMLFieldSetElement(x) WebCore::toElement<WebCore::HTMLFieldSetElement>(x)
#define toHTMLKeygenElement(x) WebCore::toElement<WebCore::HTMLKeygenElement>(x)
#define toHTMLOutputElement(x) WebCore::toElement<WebCore::HTMLOutputElement>(x)
#define toHTMLSelectElement(x) WebCore::toElement<WebCore::HTMLSelectElement>(x)
#define toHTMLTextAreaElement(x) WebCore::toElement<WebCore::HTMLTextAreaElement>(x)
#define toHTMLAppletElement(x) WebCore::toElement<WebCore::HTMLAppletElement>(x)
#define toHTMLEmbedElement(x) WebCore::toElement<WebCore::HTMLEmbedElement>(x)
#define toHTMLLinkElement(x) WebCore::toElement<WebCore::HTMLLinkElement>(x)
#define toHTMLScriptElement(x) WebCore::toElement<WebCore::HTMLScriptElement>(x)
#define toHTMLStyleElement(x) WebCore::toElement<WebCore::HTMLStyleElement>(x)
#define toHTMLImageElement(x) WebCore::toElement<WebCore::HTMLImageElement>(x)
#define toHTMLInputElement(x) WebCore::toElement<WebCore::HTMLInputElement>(x)
#define toHTMLObjectElement(x) WebCore::toElement<WebCore::HTMLObjectElement>(x)
#define toHTMLBDIElement(x) WebCore::toElement<WebCore::HTMLBDIElement>(x)
#define toHTMLNoEmbedElement(x) WebCore::toElement<WebCore::HTMLNoEmbedElement>(x)
#define toHTMLNoScriptElement(x) WebCore::toElement<WebCore::HTMLNoScriptElement>(x)
#define toHTMLRTElement(x) WebCore::toElement<WebCore::HTMLRTElement>(x)
#define toHTMLRubyElement(x) WebCore::toElement<WebCore::HTMLRubyElement>(x)
#define toHTMLSummaryElement(x) WebCore::toElement<WebCore::HTMLSummaryElement>(x)
#define toHTMLWBRElement(x) WebCore::toElement<WebCore::HTMLWBRElement>(x)
} // WebCore

#endif
