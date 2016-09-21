// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "V8HTMLElementWrapperFactory.h"

#include "HTMLNames.h"
#include "bindings/v8/CustomElementWrapper.h"
#include "V8HTMLAnchorElement.h"
#include "V8HTMLAreaElement.h"
#include "V8HTMLAudioElement.h"
#include "V8HTMLBRElement.h"
#include "V8HTMLBaseElement.h"
#include "V8HTMLBodyElement.h"
#include "V8HTMLCanvasElement.h"
#include "V8HTMLContentElement.h"
#include "V8HTMLDListElement.h"
#include "V8HTMLDataListElement.h"
#include "V8HTMLDetailsElement.h"
#include "V8HTMLDirectoryElement.h"
#include "V8HTMLDivElement.h"
#include "V8HTMLFontElement.h"
#include "V8HTMLFormElement.h"
#include "V8HTMLFrameElement.h"
#include "V8HTMLFrameSetElement.h"
#include "V8HTMLHRElement.h"
#include "V8HTMLHeadElement.h"
#include "V8HTMLHeadingElement.h"
#include "V8HTMLHeadingElement.h"
#include "V8HTMLHeadingElement.h"
#include "V8HTMLHeadingElement.h"
#include "V8HTMLHeadingElement.h"
#include "V8HTMLHeadingElement.h"
#include "V8HTMLHtmlElement.h"
#include "V8HTMLIFrameElement.h"
#include "V8HTMLLIElement.h"
#include "V8HTMLLabelElement.h"
#include "V8HTMLLegendElement.h"
#include "V8HTMLMapElement.h"
#include "V8HTMLMarqueeElement.h"
#include "V8HTMLMenuElement.h"
#include "V8HTMLMetaElement.h"
#include "V8HTMLMeterElement.h"
#include "V8HTMLModElement.h"
#include "V8HTMLModElement.h"
#include "V8HTMLOListElement.h"
#include "V8HTMLOptGroupElement.h"
#include "V8HTMLOptionElement.h"
#include "V8HTMLParagraphElement.h"
#include "V8HTMLParamElement.h"
#include "V8HTMLPictureElement.h"
#include "V8HTMLPreElement.h"
#include "V8HTMLPreElement.h"
#include "V8HTMLPreElement.h"
#include "V8HTMLProgressElement.h"
#include "V8HTMLQuoteElement.h"
#include "V8HTMLQuoteElement.h"
#include "V8HTMLShadowElement.h"
#include "V8HTMLSourceElement.h"
#include "V8HTMLSpanElement.h"
#include "V8HTMLTableCaptionElement.h"
#include "V8HTMLTableCellElement.h"
#include "V8HTMLTableCellElement.h"
#include "V8HTMLTableColElement.h"
#include "V8HTMLTableColElement.h"
#include "V8HTMLTableElement.h"
#include "V8HTMLTableRowElement.h"
#include "V8HTMLTableSectionElement.h"
#include "V8HTMLTableSectionElement.h"
#include "V8HTMLTableSectionElement.h"
#include "V8HTMLTemplateElement.h"
#include "V8HTMLTitleElement.h"
#include "V8HTMLTrackElement.h"
#include "V8HTMLUListElement.h"
#include "V8HTMLUnknownElement.h"
#include "V8HTMLVideoElement.h"
#include "V8HTMLDialogElement.h"
#include "V8HTMLButtonElement.h"
#include "V8HTMLFieldSetElement.h"
#include "V8HTMLKeygenElement.h"
#include "V8HTMLOutputElement.h"
#include "V8HTMLSelectElement.h"
#include "V8HTMLTextAreaElement.h"
#include "V8HTMLAppletElement.h"
#include "V8HTMLEmbedElement.h"
#include "V8HTMLLinkElement.h"
#include "V8HTMLScriptElement.h"
#include "V8HTMLStyleElement.h"
#include "V8HTMLImageElement.h"
#include "V8HTMLInputElement.h"
#include "V8HTMLObjectElement.h"
#include "core/html/HTMLAnchorElement.h"
#include "core/html/HTMLAreaElement.h"
#include "core/html/HTMLAudioElement.h"
#include "core/html/HTMLBRElement.h"
#include "core/html/HTMLBaseElement.h"
#include "core/html/HTMLBodyElement.h"
#include "core/html/HTMLCanvasElement.h"
#include "core/html/HTMLContentElement.h"
#include "core/html/HTMLDListElement.h"
#include "core/html/HTMLDataListElement.h"
#include "core/html/HTMLDetailsElement.h"
#include "core/html/HTMLDirectoryElement.h"
#include "core/html/HTMLDivElement.h"
#include "core/html/HTMLFontElement.h"
#include "core/html/HTMLFormElement.h"
#include "core/html/HTMLFrameElement.h"
#include "core/html/HTMLFrameSetElement.h"
#include "core/html/HTMLHRElement.h"
#include "core/html/HTMLHeadElement.h"
#include "core/html/HTMLHeadingElement.h"
#include "core/html/HTMLHeadingElement.h"
#include "core/html/HTMLHeadingElement.h"
#include "core/html/HTMLHeadingElement.h"
#include "core/html/HTMLHeadingElement.h"
#include "core/html/HTMLHeadingElement.h"
#include "core/html/HTMLHtmlElement.h"
#include "core/html/HTMLIFrameElement.h"
#include "core/html/HTMLLIElement.h"
#include "core/html/HTMLLabelElement.h"
#include "core/html/HTMLLegendElement.h"
#include "core/html/HTMLMapElement.h"
#include "core/html/HTMLMarqueeElement.h"
#include "core/html/HTMLMenuElement.h"
#include "core/html/HTMLMetaElement.h"
#include "core/html/HTMLMeterElement.h"
#include "core/html/HTMLModElement.h"
#include "core/html/HTMLModElement.h"
#include "core/html/HTMLOListElement.h"
#include "core/html/HTMLOptGroupElement.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/HTMLParagraphElement.h"
#include "core/html/HTMLParamElement.h"
#include "core/html/HTMLPictureElement.h"
#include "core/html/HTMLPreElement.h"
#include "core/html/HTMLPreElement.h"
#include "core/html/HTMLPreElement.h"
#include "core/html/HTMLProgressElement.h"
#include "core/html/HTMLQuoteElement.h"
#include "core/html/HTMLQuoteElement.h"
#include "core/html/HTMLShadowElement.h"
#include "core/html/HTMLSourceElement.h"
#include "core/html/HTMLSpanElement.h"
#include "core/html/HTMLTableCaptionElement.h"
#include "core/html/HTMLTableCellElement.h"
#include "core/html/HTMLTableCellElement.h"
#include "core/html/HTMLTableColElement.h"
#include "core/html/HTMLTableColElement.h"
#include "core/html/HTMLTableElement.h"
#include "core/html/HTMLTableRowElement.h"
#include "core/html/HTMLTableSectionElement.h"
#include "core/html/HTMLTableSectionElement.h"
#include "core/html/HTMLTableSectionElement.h"
#include "core/html/HTMLTemplateElement.h"
#include "core/html/HTMLTitleElement.h"
#include "core/html/HTMLTrackElement.h"
#include "core/html/HTMLUListElement.h"
#include "core/html/HTMLUnknownElement.h"
#include "core/html/HTMLVideoElement.h"
#include "core/html/HTMLDialogElement.h"
#include "core/html/HTMLButtonElement.h"
#include "core/html/HTMLFieldSetElement.h"
#include "core/html/HTMLKeygenElement.h"
#include "core/html/HTMLOutputElement.h"
#include "core/html/HTMLSelectElement.h"
#include "core/html/HTMLTextAreaElement.h"
#include "core/html/HTMLAppletElement.h"
#include "core/html/HTMLEmbedElement.h"
#include "core/html/HTMLLinkElement.h"
#include "core/html/HTMLScriptElement.h"
#include "core/html/HTMLStyleElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLObjectElement.h"
#include "core/html/HTMLUnknownElement.h"
#include "core/dom/ContextFeatures.h"
#include "core/dom/Document.h"
#include "core/frame/Settings.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "wtf/StdLibExtras.h"

namespace WebCore {

using namespace HTMLNames;

typedef v8::Handle<v8::Object> (*CreateHTMLElementWrapperFunction)(HTMLElement*, v8::Handle<v8::Object> creationContext, v8::Isolate*);

static v8::Handle<v8::Object> createHTMLElementWrapper(HTMLElement*, v8::Handle<v8::Object>, v8::Isolate*)
{
    ASSERT_NOT_REACHED();
    return v8::Handle<v8::Object>();
}
static v8::Handle<v8::Object> createHTMLAnchorElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLAnchorElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLAppletElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLAppletElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLAreaElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLAreaElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLAudioElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    if (!RuntimeEnabledFeatures::mediaEnabled())
        return createV8HTMLFallbackWrapper(toHTMLUnknownElement(element), creationContext, isolate);
    return wrap(static_cast<HTMLAudioElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLBRElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLBRElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLBaseElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLBaseElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLBodyElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLBodyElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLButtonElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLButtonElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLCanvasElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLCanvasElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLContentElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLContentElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLDListElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLDListElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLDataListElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLDataListElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLDetailsElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLDetailsElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLDialogElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    if (!ContextFeatures::dialogElementEnabled(&element->document()))
        return createV8HTMLFallbackWrapper(toHTMLUnknownElement(element), creationContext, isolate);
    return wrap(static_cast<HTMLDialogElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLDirectoryElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLDirectoryElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLDivElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLDivElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLEmbedElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLEmbedElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLFieldSetElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLFieldSetElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLFontElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLFontElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLFormElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLFormElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLFrameElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLFrameElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLFrameSetElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLFrameSetElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLHRElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLHRElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLHeadElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLHeadElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLHeadingElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLHeadingElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLHtmlElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLHtmlElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLIFrameElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLIFrameElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLImageElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLImageElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLInputElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLInputElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLKeygenElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLKeygenElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLLIElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLLIElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLLabelElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLLabelElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLLegendElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLLegendElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLLinkElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLLinkElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLMapElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLMapElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLMarqueeElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLMarqueeElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLMenuElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLMenuElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLMetaElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLMetaElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLMeterElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLMeterElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLModElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLModElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLOListElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLOListElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLObjectElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLObjectElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLOptGroupElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLOptGroupElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLOptionElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLOptionElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLOutputElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLOutputElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLParagraphElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLParagraphElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLParamElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLParamElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLPictureElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    if (!RuntimeEnabledFeatures::pictureEnabled())
        return createV8HTMLFallbackWrapper(toHTMLUnknownElement(element), creationContext, isolate);
    return wrap(static_cast<HTMLPictureElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLPreElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLPreElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLProgressElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLProgressElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLQuoteElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLQuoteElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLScriptElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLScriptElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLSelectElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLSelectElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLShadowElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLShadowElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLSourceElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    if (!RuntimeEnabledFeatures::mediaEnabled())
        return createV8HTMLFallbackWrapper(toHTMLUnknownElement(element), creationContext, isolate);
    return wrap(static_cast<HTMLSourceElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLSpanElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLSpanElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLStyleElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLStyleElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLTableCaptionElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLTableCaptionElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLTableCellElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLTableCellElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLTableColElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLTableColElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLTableElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLTableElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLTableRowElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLTableRowElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLTableSectionElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLTableSectionElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLTemplateElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLTemplateElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLTextAreaElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLTextAreaElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLTitleElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLTitleElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLTrackElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLTrackElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLUListElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLUListElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLUnknownElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<HTMLUnknownElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createHTMLVideoElementWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    if (!RuntimeEnabledFeatures::mediaEnabled())
        return createV8HTMLFallbackWrapper(toHTMLUnknownElement(element), creationContext, isolate);
    return wrap(static_cast<HTMLVideoElement*>(element), creationContext, isolate);
}

v8::Handle<v8::Object> createV8HTMLWrapper(HTMLElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    typedef HashMap<StringImpl*, CreateHTMLElementWrapperFunction> FunctionMap;
    DEFINE_STATIC_LOCAL(FunctionMap, map, ());
    if (map.isEmpty()) {
        map.set(abbrTag.localName().impl(), createHTMLElementWrapper);
        map.set(acronymTag.localName().impl(), createHTMLElementWrapper);
        map.set(addressTag.localName().impl(), createHTMLElementWrapper);
        map.set(articleTag.localName().impl(), createHTMLElementWrapper);
        map.set(asideTag.localName().impl(), createHTMLElementWrapper);
        map.set(bTag.localName().impl(), createHTMLElementWrapper);
        map.set(basefontTag.localName().impl(), createHTMLElementWrapper);
        map.set(bdoTag.localName().impl(), createHTMLElementWrapper);
        map.set(bgsoundTag.localName().impl(), createHTMLElementWrapper);
        map.set(bigTag.localName().impl(), createHTMLElementWrapper);
        map.set(centerTag.localName().impl(), createHTMLElementWrapper);
        map.set(citeTag.localName().impl(), createHTMLElementWrapper);
        map.set(codeTag.localName().impl(), createHTMLElementWrapper);
        map.set(commandTag.localName().impl(), createHTMLElementWrapper);
        map.set(ddTag.localName().impl(), createHTMLElementWrapper);
        map.set(dfnTag.localName().impl(), createHTMLElementWrapper);
        map.set(dtTag.localName().impl(), createHTMLElementWrapper);
        map.set(emTag.localName().impl(), createHTMLElementWrapper);
        map.set(figcaptionTag.localName().impl(), createHTMLElementWrapper);
        map.set(figureTag.localName().impl(), createHTMLElementWrapper);
        map.set(footerTag.localName().impl(), createHTMLElementWrapper);
        map.set(headerTag.localName().impl(), createHTMLElementWrapper);
        map.set(hgroupTag.localName().impl(), createHTMLElementWrapper);
        map.set(iTag.localName().impl(), createHTMLElementWrapper);
        map.set(kbdTag.localName().impl(), createHTMLElementWrapper);
        map.set(layerTag.localName().impl(), createHTMLElementWrapper);
        map.set(mainTag.localName().impl(), createHTMLElementWrapper);
        map.set(markTag.localName().impl(), createHTMLElementWrapper);
        map.set(navTag.localName().impl(), createHTMLElementWrapper);
        map.set(nobrTag.localName().impl(), createHTMLElementWrapper);
        map.set(noframesTag.localName().impl(), createHTMLElementWrapper);
        map.set(nolayerTag.localName().impl(), createHTMLElementWrapper);
        map.set(plaintextTag.localName().impl(), createHTMLElementWrapper);
        map.set(rpTag.localName().impl(), createHTMLElementWrapper);
        map.set(sTag.localName().impl(), createHTMLElementWrapper);
        map.set(sampTag.localName().impl(), createHTMLElementWrapper);
        map.set(sectionTag.localName().impl(), createHTMLElementWrapper);
        map.set(smallTag.localName().impl(), createHTMLElementWrapper);
        map.set(strikeTag.localName().impl(), createHTMLElementWrapper);
        map.set(strongTag.localName().impl(), createHTMLElementWrapper);
        map.set(subTag.localName().impl(), createHTMLElementWrapper);
        map.set(supTag.localName().impl(), createHTMLElementWrapper);
        map.set(ttTag.localName().impl(), createHTMLElementWrapper);
        map.set(uTag.localName().impl(), createHTMLElementWrapper);
        map.set(varTag.localName().impl(), createHTMLElementWrapper);
        map.set(aTag.localName().impl(), createHTMLAnchorElementWrapper);
        map.set(areaTag.localName().impl(), createHTMLAreaElementWrapper);
        map.set(audioTag.localName().impl(), createHTMLAudioElementWrapper);
        map.set(brTag.localName().impl(), createHTMLBRElementWrapper);
        map.set(baseTag.localName().impl(), createHTMLBaseElementWrapper);
        map.set(bodyTag.localName().impl(), createHTMLBodyElementWrapper);
        map.set(canvasTag.localName().impl(), createHTMLCanvasElementWrapper);
        map.set(contentTag.localName().impl(), createHTMLContentElementWrapper);
        map.set(dlTag.localName().impl(), createHTMLDListElementWrapper);
        map.set(datalistTag.localName().impl(), createHTMLDataListElementWrapper);
        map.set(detailsTag.localName().impl(), createHTMLDetailsElementWrapper);
        map.set(dirTag.localName().impl(), createHTMLDirectoryElementWrapper);
        map.set(divTag.localName().impl(), createHTMLDivElementWrapper);
        map.set(fontTag.localName().impl(), createHTMLFontElementWrapper);
        map.set(formTag.localName().impl(), createHTMLFormElementWrapper);
        map.set(frameTag.localName().impl(), createHTMLFrameElementWrapper);
        map.set(framesetTag.localName().impl(), createHTMLFrameSetElementWrapper);
        map.set(hrTag.localName().impl(), createHTMLHRElementWrapper);
        map.set(headTag.localName().impl(), createHTMLHeadElementWrapper);
        map.set(h1Tag.localName().impl(), createHTMLHeadingElementWrapper);
        map.set(h2Tag.localName().impl(), createHTMLHeadingElementWrapper);
        map.set(h3Tag.localName().impl(), createHTMLHeadingElementWrapper);
        map.set(h4Tag.localName().impl(), createHTMLHeadingElementWrapper);
        map.set(h5Tag.localName().impl(), createHTMLHeadingElementWrapper);
        map.set(h6Tag.localName().impl(), createHTMLHeadingElementWrapper);
        map.set(htmlTag.localName().impl(), createHTMLHtmlElementWrapper);
        map.set(iframeTag.localName().impl(), createHTMLIFrameElementWrapper);
        map.set(liTag.localName().impl(), createHTMLLIElementWrapper);
        map.set(labelTag.localName().impl(), createHTMLLabelElementWrapper);
        map.set(legendTag.localName().impl(), createHTMLLegendElementWrapper);
        map.set(mapTag.localName().impl(), createHTMLMapElementWrapper);
        map.set(marqueeTag.localName().impl(), createHTMLMarqueeElementWrapper);
        map.set(menuTag.localName().impl(), createHTMLMenuElementWrapper);
        map.set(metaTag.localName().impl(), createHTMLMetaElementWrapper);
        map.set(meterTag.localName().impl(), createHTMLMeterElementWrapper);
        map.set(delTag.localName().impl(), createHTMLModElementWrapper);
        map.set(insTag.localName().impl(), createHTMLModElementWrapper);
        map.set(olTag.localName().impl(), createHTMLOListElementWrapper);
        map.set(optgroupTag.localName().impl(), createHTMLOptGroupElementWrapper);
        map.set(optionTag.localName().impl(), createHTMLOptionElementWrapper);
        map.set(pTag.localName().impl(), createHTMLParagraphElementWrapper);
        map.set(paramTag.localName().impl(), createHTMLParamElementWrapper);
        map.set(pictureTag.localName().impl(), createHTMLPictureElementWrapper);
        map.set(preTag.localName().impl(), createHTMLPreElementWrapper);
        map.set(listingTag.localName().impl(), createHTMLPreElementWrapper);
        map.set(xmpTag.localName().impl(), createHTMLPreElementWrapper);
        map.set(progressTag.localName().impl(), createHTMLProgressElementWrapper);
        map.set(blockquoteTag.localName().impl(), createHTMLQuoteElementWrapper);
        map.set(qTag.localName().impl(), createHTMLQuoteElementWrapper);
        map.set(shadowTag.localName().impl(), createHTMLShadowElementWrapper);
        map.set(sourceTag.localName().impl(), createHTMLSourceElementWrapper);
        map.set(spanTag.localName().impl(), createHTMLSpanElementWrapper);
        map.set(captionTag.localName().impl(), createHTMLTableCaptionElementWrapper);
        map.set(tdTag.localName().impl(), createHTMLTableCellElementWrapper);
        map.set(thTag.localName().impl(), createHTMLTableCellElementWrapper);
        map.set(colTag.localName().impl(), createHTMLTableColElementWrapper);
        map.set(colgroupTag.localName().impl(), createHTMLTableColElementWrapper);
        map.set(tableTag.localName().impl(), createHTMLTableElementWrapper);
        map.set(trTag.localName().impl(), createHTMLTableRowElementWrapper);
        map.set(tbodyTag.localName().impl(), createHTMLTableSectionElementWrapper);
        map.set(tfootTag.localName().impl(), createHTMLTableSectionElementWrapper);
        map.set(theadTag.localName().impl(), createHTMLTableSectionElementWrapper);
        map.set(templateTag.localName().impl(), createHTMLTemplateElementWrapper);
        map.set(titleTag.localName().impl(), createHTMLTitleElementWrapper);
        map.set(trackTag.localName().impl(), createHTMLTrackElementWrapper);
        map.set(ulTag.localName().impl(), createHTMLUListElementWrapper);
        map.set(imageTag.localName().impl(), createHTMLUnknownElementWrapper);
        map.set(videoTag.localName().impl(), createHTMLVideoElementWrapper);
        map.set(dialogTag.localName().impl(), createHTMLDialogElementWrapper);
        map.set(buttonTag.localName().impl(), createHTMLButtonElementWrapper);
        map.set(fieldsetTag.localName().impl(), createHTMLFieldSetElementWrapper);
        map.set(keygenTag.localName().impl(), createHTMLKeygenElementWrapper);
        map.set(outputTag.localName().impl(), createHTMLOutputElementWrapper);
        map.set(selectTag.localName().impl(), createHTMLSelectElementWrapper);
        map.set(textareaTag.localName().impl(), createHTMLTextAreaElementWrapper);
        map.set(appletTag.localName().impl(), createHTMLAppletElementWrapper);
        map.set(embedTag.localName().impl(), createHTMLEmbedElementWrapper);
        map.set(linkTag.localName().impl(), createHTMLLinkElementWrapper);
        map.set(scriptTag.localName().impl(), createHTMLScriptElementWrapper);
        map.set(styleTag.localName().impl(), createHTMLStyleElementWrapper);
        map.set(imgTag.localName().impl(), createHTMLImageElementWrapper);
        map.set(inputTag.localName().impl(), createHTMLInputElementWrapper);
        map.set(objectTag.localName().impl(), createHTMLObjectElementWrapper);
        map.set(bdiTag.localName().impl(), createHTMLElementWrapper);
        map.set(noembedTag.localName().impl(), createHTMLElementWrapper);
        map.set(noscriptTag.localName().impl(), createHTMLElementWrapper);
        map.set(rtTag.localName().impl(), createHTMLElementWrapper);
        map.set(rubyTag.localName().impl(), createHTMLElementWrapper);
        map.set(summaryTag.localName().impl(), createHTMLElementWrapper);
        map.set(wbrTag.localName().impl(), createHTMLElementWrapper);
    }

    CreateHTMLElementWrapperFunction createWrapperFunction = map.get(element->localName().impl());
    if (createWrapperFunction == createHTMLElementWrapper)
        createWrapperFunction = createV8HTMLDirectWrapper;
    if (element->isCustomElement())
        return CustomElementWrapper<HTMLElement, V8HTMLElement>::wrap(element, creationContext, isolate, createWrapperFunction);

    if (createWrapperFunction)
        return createWrapperFunction(element, creationContext, isolate);
    return wrap(toHTMLUnknownElement(element), creationContext, isolate);
}

const WrapperTypeInfo* findWrapperTypeForHTMLTagName(const AtomicString& name)
{
    typedef HashMap<StringImpl*, const WrapperTypeInfo*> NameTypeMap;
    DEFINE_STATIC_LOCAL(NameTypeMap, map, ());
    if (map.isEmpty()) {
        // FIXME: This seems wrong. We should list every interface here, not
        // just the ones that have specialized JavaScript interfaces.
        map.set(aTag.localName().impl(), &V8HTMLAnchorElement::wrapperTypeInfo);
        map.set(areaTag.localName().impl(), &V8HTMLAreaElement::wrapperTypeInfo);
        map.set(audioTag.localName().impl(), &V8HTMLAudioElement::wrapperTypeInfo);
        map.set(brTag.localName().impl(), &V8HTMLBRElement::wrapperTypeInfo);
        map.set(baseTag.localName().impl(), &V8HTMLBaseElement::wrapperTypeInfo);
        map.set(bodyTag.localName().impl(), &V8HTMLBodyElement::wrapperTypeInfo);
        map.set(canvasTag.localName().impl(), &V8HTMLCanvasElement::wrapperTypeInfo);
        map.set(contentTag.localName().impl(), &V8HTMLContentElement::wrapperTypeInfo);
        map.set(dlTag.localName().impl(), &V8HTMLDListElement::wrapperTypeInfo);
        map.set(datalistTag.localName().impl(), &V8HTMLDataListElement::wrapperTypeInfo);
        map.set(detailsTag.localName().impl(), &V8HTMLDetailsElement::wrapperTypeInfo);
        map.set(dirTag.localName().impl(), &V8HTMLDirectoryElement::wrapperTypeInfo);
        map.set(divTag.localName().impl(), &V8HTMLDivElement::wrapperTypeInfo);
        map.set(fontTag.localName().impl(), &V8HTMLFontElement::wrapperTypeInfo);
        map.set(formTag.localName().impl(), &V8HTMLFormElement::wrapperTypeInfo);
        map.set(frameTag.localName().impl(), &V8HTMLFrameElement::wrapperTypeInfo);
        map.set(framesetTag.localName().impl(), &V8HTMLFrameSetElement::wrapperTypeInfo);
        map.set(hrTag.localName().impl(), &V8HTMLHRElement::wrapperTypeInfo);
        map.set(headTag.localName().impl(), &V8HTMLHeadElement::wrapperTypeInfo);
        map.set(h1Tag.localName().impl(), &V8HTMLHeadingElement::wrapperTypeInfo);
        map.set(h2Tag.localName().impl(), &V8HTMLHeadingElement::wrapperTypeInfo);
        map.set(h3Tag.localName().impl(), &V8HTMLHeadingElement::wrapperTypeInfo);
        map.set(h4Tag.localName().impl(), &V8HTMLHeadingElement::wrapperTypeInfo);
        map.set(h5Tag.localName().impl(), &V8HTMLHeadingElement::wrapperTypeInfo);
        map.set(h6Tag.localName().impl(), &V8HTMLHeadingElement::wrapperTypeInfo);
        map.set(htmlTag.localName().impl(), &V8HTMLHtmlElement::wrapperTypeInfo);
        map.set(iframeTag.localName().impl(), &V8HTMLIFrameElement::wrapperTypeInfo);
        map.set(liTag.localName().impl(), &V8HTMLLIElement::wrapperTypeInfo);
        map.set(labelTag.localName().impl(), &V8HTMLLabelElement::wrapperTypeInfo);
        map.set(legendTag.localName().impl(), &V8HTMLLegendElement::wrapperTypeInfo);
        map.set(mapTag.localName().impl(), &V8HTMLMapElement::wrapperTypeInfo);
        map.set(marqueeTag.localName().impl(), &V8HTMLMarqueeElement::wrapperTypeInfo);
        map.set(menuTag.localName().impl(), &V8HTMLMenuElement::wrapperTypeInfo);
        map.set(metaTag.localName().impl(), &V8HTMLMetaElement::wrapperTypeInfo);
        map.set(meterTag.localName().impl(), &V8HTMLMeterElement::wrapperTypeInfo);
        map.set(delTag.localName().impl(), &V8HTMLModElement::wrapperTypeInfo);
        map.set(insTag.localName().impl(), &V8HTMLModElement::wrapperTypeInfo);
        map.set(olTag.localName().impl(), &V8HTMLOListElement::wrapperTypeInfo);
        map.set(optgroupTag.localName().impl(), &V8HTMLOptGroupElement::wrapperTypeInfo);
        map.set(optionTag.localName().impl(), &V8HTMLOptionElement::wrapperTypeInfo);
        map.set(pTag.localName().impl(), &V8HTMLParagraphElement::wrapperTypeInfo);
        map.set(paramTag.localName().impl(), &V8HTMLParamElement::wrapperTypeInfo);
        map.set(pictureTag.localName().impl(), &V8HTMLPictureElement::wrapperTypeInfo);
        map.set(preTag.localName().impl(), &V8HTMLPreElement::wrapperTypeInfo);
        map.set(listingTag.localName().impl(), &V8HTMLPreElement::wrapperTypeInfo);
        map.set(xmpTag.localName().impl(), &V8HTMLPreElement::wrapperTypeInfo);
        map.set(progressTag.localName().impl(), &V8HTMLProgressElement::wrapperTypeInfo);
        map.set(blockquoteTag.localName().impl(), &V8HTMLQuoteElement::wrapperTypeInfo);
        map.set(qTag.localName().impl(), &V8HTMLQuoteElement::wrapperTypeInfo);
        map.set(shadowTag.localName().impl(), &V8HTMLShadowElement::wrapperTypeInfo);
        map.set(sourceTag.localName().impl(), &V8HTMLSourceElement::wrapperTypeInfo);
        map.set(spanTag.localName().impl(), &V8HTMLSpanElement::wrapperTypeInfo);
        map.set(captionTag.localName().impl(), &V8HTMLTableCaptionElement::wrapperTypeInfo);
        map.set(tdTag.localName().impl(), &V8HTMLTableCellElement::wrapperTypeInfo);
        map.set(thTag.localName().impl(), &V8HTMLTableCellElement::wrapperTypeInfo);
        map.set(colTag.localName().impl(), &V8HTMLTableColElement::wrapperTypeInfo);
        map.set(colgroupTag.localName().impl(), &V8HTMLTableColElement::wrapperTypeInfo);
        map.set(tableTag.localName().impl(), &V8HTMLTableElement::wrapperTypeInfo);
        map.set(trTag.localName().impl(), &V8HTMLTableRowElement::wrapperTypeInfo);
        map.set(tbodyTag.localName().impl(), &V8HTMLTableSectionElement::wrapperTypeInfo);
        map.set(tfootTag.localName().impl(), &V8HTMLTableSectionElement::wrapperTypeInfo);
        map.set(theadTag.localName().impl(), &V8HTMLTableSectionElement::wrapperTypeInfo);
        map.set(templateTag.localName().impl(), &V8HTMLTemplateElement::wrapperTypeInfo);
        map.set(titleTag.localName().impl(), &V8HTMLTitleElement::wrapperTypeInfo);
        map.set(trackTag.localName().impl(), &V8HTMLTrackElement::wrapperTypeInfo);
        map.set(ulTag.localName().impl(), &V8HTMLUListElement::wrapperTypeInfo);
        map.set(imageTag.localName().impl(), &V8HTMLUnknownElement::wrapperTypeInfo);
        map.set(videoTag.localName().impl(), &V8HTMLVideoElement::wrapperTypeInfo);
        map.set(dialogTag.localName().impl(), &V8HTMLDialogElement::wrapperTypeInfo);
        map.set(buttonTag.localName().impl(), &V8HTMLButtonElement::wrapperTypeInfo);
        map.set(fieldsetTag.localName().impl(), &V8HTMLFieldSetElement::wrapperTypeInfo);
        map.set(keygenTag.localName().impl(), &V8HTMLKeygenElement::wrapperTypeInfo);
        map.set(outputTag.localName().impl(), &V8HTMLOutputElement::wrapperTypeInfo);
        map.set(selectTag.localName().impl(), &V8HTMLSelectElement::wrapperTypeInfo);
        map.set(textareaTag.localName().impl(), &V8HTMLTextAreaElement::wrapperTypeInfo);
        map.set(appletTag.localName().impl(), &V8HTMLAppletElement::wrapperTypeInfo);
        map.set(embedTag.localName().impl(), &V8HTMLEmbedElement::wrapperTypeInfo);
        map.set(linkTag.localName().impl(), &V8HTMLLinkElement::wrapperTypeInfo);
        map.set(scriptTag.localName().impl(), &V8HTMLScriptElement::wrapperTypeInfo);
        map.set(styleTag.localName().impl(), &V8HTMLStyleElement::wrapperTypeInfo);
        map.set(imgTag.localName().impl(), &V8HTMLImageElement::wrapperTypeInfo);
        map.set(inputTag.localName().impl(), &V8HTMLInputElement::wrapperTypeInfo);
        map.set(objectTag.localName().impl(), &V8HTMLObjectElement::wrapperTypeInfo);
    }

    if (const WrapperTypeInfo* result = map.get(name.impl()))
        return result;

    return &V8HTMLUnknownElement::wrapperTypeInfo;
}

}
