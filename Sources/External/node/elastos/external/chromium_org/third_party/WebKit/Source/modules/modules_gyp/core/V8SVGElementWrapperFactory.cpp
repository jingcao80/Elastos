// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "V8SVGElementWrapperFactory.h"

#include "SVGNames.h"
#include "bindings/v8/CustomElementWrapper.h"
#include "V8SVGAElement.h"
#include "V8SVGAnimateElement.h"
#include "V8SVGAnimateMotionElement.h"
#include "V8SVGAnimateTransformElement.h"
#include "V8SVGCircleElement.h"
#include "V8SVGClipPathElement.h"
#include "V8SVGCursorElement.h"
#include "V8SVGDefsElement.h"
#include "V8SVGDescElement.h"
#include "V8SVGDiscardElement.h"
#include "V8SVGEllipseElement.h"
#include "V8SVGFEBlendElement.h"
#include "V8SVGFEColorMatrixElement.h"
#include "V8SVGFEComponentTransferElement.h"
#include "V8SVGFECompositeElement.h"
#include "V8SVGFEConvolveMatrixElement.h"
#include "V8SVGFEDiffuseLightingElement.h"
#include "V8SVGFEDisplacementMapElement.h"
#include "V8SVGFEDistantLightElement.h"
#include "V8SVGFEDropShadowElement.h"
#include "V8SVGFEFloodElement.h"
#include "V8SVGFEFuncAElement.h"
#include "V8SVGFEFuncBElement.h"
#include "V8SVGFEFuncGElement.h"
#include "V8SVGFEFuncRElement.h"
#include "V8SVGFEGaussianBlurElement.h"
#include "V8SVGFEImageElement.h"
#include "V8SVGFEMergeElement.h"
#include "V8SVGFEMergeNodeElement.h"
#include "V8SVGFEMorphologyElement.h"
#include "V8SVGFEOffsetElement.h"
#include "V8SVGFEPointLightElement.h"
#include "V8SVGFESpecularLightingElement.h"
#include "V8SVGFESpotLightElement.h"
#include "V8SVGFETileElement.h"
#include "V8SVGFETurbulenceElement.h"
#include "V8SVGFilterElement.h"
#include "V8SVGForeignObjectElement.h"
#include "V8SVGGElement.h"
#include "V8SVGImageElement.h"
#include "V8SVGLineElement.h"
#include "V8SVGLinearGradientElement.h"
#include "V8SVGMPathElement.h"
#include "V8SVGMarkerElement.h"
#include "V8SVGMaskElement.h"
#include "V8SVGMetadataElement.h"
#include "V8SVGPathElement.h"
#include "V8SVGPatternElement.h"
#include "V8SVGPolygonElement.h"
#include "V8SVGPolylineElement.h"
#include "V8SVGRadialGradientElement.h"
#include "V8SVGRectElement.h"
#include "V8SVGSVGElement.h"
#include "V8SVGSetElement.h"
#include "V8SVGStopElement.h"
#include "V8SVGSwitchElement.h"
#include "V8SVGSymbolElement.h"
#include "V8SVGTSpanElement.h"
#include "V8SVGTextElement.h"
#include "V8SVGTextPathElement.h"
#include "V8SVGTitleElement.h"
#include "V8SVGUseElement.h"
#include "V8SVGViewElement.h"
#include "V8SVGScriptElement.h"
#include "V8SVGStyleElement.h"
#include "V8SVGAltGlyphDefElement.h"
#include "V8SVGAltGlyphElement.h"
#include "V8SVGAltGlyphItemElement.h"
#include "V8SVGFontElement.h"
#include "V8SVGFontFaceElement.h"
#include "V8SVGFontFaceFormatElement.h"
#include "V8SVGFontFaceNameElement.h"
#include "V8SVGFontFaceSrcElement.h"
#include "V8SVGFontFaceUriElement.h"
#include "V8SVGGlyphElement.h"
#include "V8SVGGlyphRefElement.h"
#include "V8SVGHKernElement.h"
#include "V8SVGMissingGlyphElement.h"
#include "V8SVGVKernElement.h"
#include "core/svg/SVGAElement.h"
#include "core/svg/SVGAnimateElement.h"
#include "core/svg/SVGAnimateMotionElement.h"
#include "core/svg/SVGAnimateTransformElement.h"
#include "core/svg/SVGCircleElement.h"
#include "core/svg/SVGClipPathElement.h"
#include "core/svg/SVGCursorElement.h"
#include "core/svg/SVGDefsElement.h"
#include "core/svg/SVGDescElement.h"
#include "core/svg/SVGDiscardElement.h"
#include "core/svg/SVGEllipseElement.h"
#include "core/svg/SVGFEBlendElement.h"
#include "core/svg/SVGFEColorMatrixElement.h"
#include "core/svg/SVGFEComponentTransferElement.h"
#include "core/svg/SVGFECompositeElement.h"
#include "core/svg/SVGFEConvolveMatrixElement.h"
#include "core/svg/SVGFEDiffuseLightingElement.h"
#include "core/svg/SVGFEDisplacementMapElement.h"
#include "core/svg/SVGFEDistantLightElement.h"
#include "core/svg/SVGFEDropShadowElement.h"
#include "core/svg/SVGFEFloodElement.h"
#include "core/svg/SVGFEFuncAElement.h"
#include "core/svg/SVGFEFuncBElement.h"
#include "core/svg/SVGFEFuncGElement.h"
#include "core/svg/SVGFEFuncRElement.h"
#include "core/svg/SVGFEGaussianBlurElement.h"
#include "core/svg/SVGFEImageElement.h"
#include "core/svg/SVGFEMergeElement.h"
#include "core/svg/SVGFEMergeNodeElement.h"
#include "core/svg/SVGFEMorphologyElement.h"
#include "core/svg/SVGFEOffsetElement.h"
#include "core/svg/SVGFEPointLightElement.h"
#include "core/svg/SVGFESpecularLightingElement.h"
#include "core/svg/SVGFESpotLightElement.h"
#include "core/svg/SVGFETileElement.h"
#include "core/svg/SVGFETurbulenceElement.h"
#include "core/svg/SVGFilterElement.h"
#include "core/svg/SVGForeignObjectElement.h"
#include "core/svg/SVGGElement.h"
#include "core/svg/SVGImageElement.h"
#include "core/svg/SVGLineElement.h"
#include "core/svg/SVGLinearGradientElement.h"
#include "core/svg/SVGMPathElement.h"
#include "core/svg/SVGMarkerElement.h"
#include "core/svg/SVGMaskElement.h"
#include "core/svg/SVGMetadataElement.h"
#include "core/svg/SVGPathElement.h"
#include "core/svg/SVGPatternElement.h"
#include "core/svg/SVGPolygonElement.h"
#include "core/svg/SVGPolylineElement.h"
#include "core/svg/SVGRadialGradientElement.h"
#include "core/svg/SVGRectElement.h"
#include "core/svg/SVGSVGElement.h"
#include "core/svg/SVGSetElement.h"
#include "core/svg/SVGStopElement.h"
#include "core/svg/SVGSwitchElement.h"
#include "core/svg/SVGSymbolElement.h"
#include "core/svg/SVGTSpanElement.h"
#include "core/svg/SVGTextElement.h"
#include "core/svg/SVGTextPathElement.h"
#include "core/svg/SVGTitleElement.h"
#include "core/svg/SVGUseElement.h"
#include "core/svg/SVGViewElement.h"
#include "core/svg/SVGScriptElement.h"
#include "core/svg/SVGStyleElement.h"
#include "core/svg/SVGAltGlyphDefElement.h"
#include "core/svg/SVGAltGlyphElement.h"
#include "core/svg/SVGAltGlyphItemElement.h"
#include "core/svg/SVGFontElement.h"
#include "core/svg/SVGFontFaceElement.h"
#include "core/svg/SVGFontFaceFormatElement.h"
#include "core/svg/SVGFontFaceNameElement.h"
#include "core/svg/SVGFontFaceSrcElement.h"
#include "core/svg/SVGFontFaceUriElement.h"
#include "core/svg/SVGGlyphElement.h"
#include "core/svg/SVGGlyphRefElement.h"
#include "core/svg/SVGHKernElement.h"
#include "core/svg/SVGMissingGlyphElement.h"
#include "core/svg/SVGVKernElement.h"
#include "core/svg/SVGElement.h"
#include "core/dom/ContextFeatures.h"
#include "core/dom/Document.h"
#include "core/frame/Settings.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "wtf/StdLibExtras.h"

namespace WebCore {

using namespace SVGNames;

typedef v8::Handle<v8::Object> (*CreateSVGElementWrapperFunction)(SVGElement*, v8::Handle<v8::Object> creationContext, v8::Isolate*);

static v8::Handle<v8::Object> createSVGElementWrapper(SVGElement*, v8::Handle<v8::Object>, v8::Isolate*)
{
    ASSERT_NOT_REACHED();
    return v8::Handle<v8::Object>();
}
static v8::Handle<v8::Object> createSVGAElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGAElement*>(element), creationContext, isolate);
}
#if ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGAltGlyphDefElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGAltGlyphDefElement*>(element), creationContext, isolate);
}
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGAltGlyphElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGAltGlyphElement*>(element), creationContext, isolate);
}
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGAltGlyphItemElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGAltGlyphItemElement*>(element), creationContext, isolate);
}
#endif // ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGAnimateElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGAnimateElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGAnimateMotionElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGAnimateMotionElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGAnimateTransformElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGAnimateTransformElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGCircleElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGCircleElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGClipPathElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGClipPathElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGCursorElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGCursorElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGDefsElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGDefsElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGDescElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGDescElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGDiscardElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGDiscardElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGEllipseElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGEllipseElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEBlendElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEBlendElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEColorMatrixElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEColorMatrixElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEComponentTransferElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEComponentTransferElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFECompositeElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFECompositeElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEConvolveMatrixElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEConvolveMatrixElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEDiffuseLightingElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEDiffuseLightingElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEDisplacementMapElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEDisplacementMapElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEDistantLightElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEDistantLightElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEDropShadowElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEDropShadowElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEFloodElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEFloodElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEFuncAElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEFuncAElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEFuncBElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEFuncBElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEFuncGElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEFuncGElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEFuncRElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEFuncRElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEGaussianBlurElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEGaussianBlurElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEImageElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEImageElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEMergeElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEMergeElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEMergeNodeElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEMergeNodeElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEMorphologyElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEMorphologyElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEOffsetElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEOffsetElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFEPointLightElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFEPointLightElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFESpecularLightingElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFESpecularLightingElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFESpotLightElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFESpotLightElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFETileElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFETileElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFETurbulenceElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFETurbulenceElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGFilterElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFilterElement*>(element), creationContext, isolate);
}
#if ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGFontElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFontElement*>(element), creationContext, isolate);
}
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGFontFaceElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFontFaceElement*>(element), creationContext, isolate);
}
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGFontFaceFormatElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFontFaceFormatElement*>(element), creationContext, isolate);
}
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGFontFaceNameElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFontFaceNameElement*>(element), creationContext, isolate);
}
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGFontFaceSrcElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFontFaceSrcElement*>(element), creationContext, isolate);
}
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGFontFaceUriElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGFontFaceUriElement*>(element), creationContext, isolate);
}
#endif // ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGForeignObjectElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGForeignObjectElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGGElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGGElement*>(element), creationContext, isolate);
}
#if ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGGlyphElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGGlyphElement*>(element), creationContext, isolate);
}
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGGlyphRefElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGGlyphRefElement*>(element), creationContext, isolate);
}
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGHKernElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGHKernElement*>(element), creationContext, isolate);
}
#endif // ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGImageElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGImageElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGLineElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGLineElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGLinearGradientElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGLinearGradientElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGMPathElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGMPathElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGMarkerElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGMarkerElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGMaskElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGMaskElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGMetadataElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGMetadataElement*>(element), creationContext, isolate);
}
#if ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGMissingGlyphElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGMissingGlyphElement*>(element), creationContext, isolate);
}
#endif // ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGPathElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGPathElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGPatternElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGPatternElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGPolygonElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGPolygonElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGPolylineElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGPolylineElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGRadialGradientElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGRadialGradientElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGRectElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGRectElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGSVGElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGSVGElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGScriptElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGScriptElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGSetElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGSetElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGStopElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGStopElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGStyleElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGStyleElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGSwitchElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGSwitchElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGSymbolElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGSymbolElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGTSpanElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGTSpanElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGTextElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGTextElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGTextPathElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGTextPathElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGTitleElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGTitleElement*>(element), creationContext, isolate);
}
static v8::Handle<v8::Object> createSVGUseElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGUseElement*>(element), creationContext, isolate);
}
#if ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGVKernElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGVKernElement*>(element), creationContext, isolate);
}
#endif // ENABLE(SVG_FONTS)
static v8::Handle<v8::Object> createSVGViewElementWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    return wrap(static_cast<SVGViewElement*>(element), creationContext, isolate);
}

v8::Handle<v8::Object> createV8SVGWrapper(SVGElement* element, v8::Handle<v8::Object> creationContext, v8::Isolate* isolate)
{
    typedef HashMap<StringImpl*, CreateSVGElementWrapperFunction> FunctionMap;
    DEFINE_STATIC_LOCAL(FunctionMap, map, ());
    if (map.isEmpty()) {
        map.set(aTag.localName().impl(), createSVGAElementWrapper);
        map.set(animateTag.localName().impl(), createSVGAnimateElementWrapper);
        map.set(animateMotionTag.localName().impl(), createSVGAnimateMotionElementWrapper);
        map.set(animateTransformTag.localName().impl(), createSVGAnimateTransformElementWrapper);
        map.set(circleTag.localName().impl(), createSVGCircleElementWrapper);
        map.set(clipPathTag.localName().impl(), createSVGClipPathElementWrapper);
        map.set(cursorTag.localName().impl(), createSVGCursorElementWrapper);
        map.set(defsTag.localName().impl(), createSVGDefsElementWrapper);
        map.set(descTag.localName().impl(), createSVGDescElementWrapper);
        map.set(discardTag.localName().impl(), createSVGDiscardElementWrapper);
        map.set(ellipseTag.localName().impl(), createSVGEllipseElementWrapper);
        map.set(feBlendTag.localName().impl(), createSVGFEBlendElementWrapper);
        map.set(feColorMatrixTag.localName().impl(), createSVGFEColorMatrixElementWrapper);
        map.set(feComponentTransferTag.localName().impl(), createSVGFEComponentTransferElementWrapper);
        map.set(feCompositeTag.localName().impl(), createSVGFECompositeElementWrapper);
        map.set(feConvolveMatrixTag.localName().impl(), createSVGFEConvolveMatrixElementWrapper);
        map.set(feDiffuseLightingTag.localName().impl(), createSVGFEDiffuseLightingElementWrapper);
        map.set(feDisplacementMapTag.localName().impl(), createSVGFEDisplacementMapElementWrapper);
        map.set(feDistantLightTag.localName().impl(), createSVGFEDistantLightElementWrapper);
        map.set(feDropShadowTag.localName().impl(), createSVGFEDropShadowElementWrapper);
        map.set(feFloodTag.localName().impl(), createSVGFEFloodElementWrapper);
        map.set(feFuncATag.localName().impl(), createSVGFEFuncAElementWrapper);
        map.set(feFuncBTag.localName().impl(), createSVGFEFuncBElementWrapper);
        map.set(feFuncGTag.localName().impl(), createSVGFEFuncGElementWrapper);
        map.set(feFuncRTag.localName().impl(), createSVGFEFuncRElementWrapper);
        map.set(feGaussianBlurTag.localName().impl(), createSVGFEGaussianBlurElementWrapper);
        map.set(feImageTag.localName().impl(), createSVGFEImageElementWrapper);
        map.set(feMergeTag.localName().impl(), createSVGFEMergeElementWrapper);
        map.set(feMergeNodeTag.localName().impl(), createSVGFEMergeNodeElementWrapper);
        map.set(feMorphologyTag.localName().impl(), createSVGFEMorphologyElementWrapper);
        map.set(feOffsetTag.localName().impl(), createSVGFEOffsetElementWrapper);
        map.set(fePointLightTag.localName().impl(), createSVGFEPointLightElementWrapper);
        map.set(feSpecularLightingTag.localName().impl(), createSVGFESpecularLightingElementWrapper);
        map.set(feSpotLightTag.localName().impl(), createSVGFESpotLightElementWrapper);
        map.set(feTileTag.localName().impl(), createSVGFETileElementWrapper);
        map.set(feTurbulenceTag.localName().impl(), createSVGFETurbulenceElementWrapper);
        map.set(filterTag.localName().impl(), createSVGFilterElementWrapper);
        map.set(foreignObjectTag.localName().impl(), createSVGForeignObjectElementWrapper);
        map.set(gTag.localName().impl(), createSVGGElementWrapper);
        map.set(imageTag.localName().impl(), createSVGImageElementWrapper);
        map.set(lineTag.localName().impl(), createSVGLineElementWrapper);
        map.set(linearGradientTag.localName().impl(), createSVGLinearGradientElementWrapper);
        map.set(mpathTag.localName().impl(), createSVGMPathElementWrapper);
        map.set(markerTag.localName().impl(), createSVGMarkerElementWrapper);
        map.set(maskTag.localName().impl(), createSVGMaskElementWrapper);
        map.set(metadataTag.localName().impl(), createSVGMetadataElementWrapper);
        map.set(pathTag.localName().impl(), createSVGPathElementWrapper);
        map.set(patternTag.localName().impl(), createSVGPatternElementWrapper);
        map.set(polygonTag.localName().impl(), createSVGPolygonElementWrapper);
        map.set(polylineTag.localName().impl(), createSVGPolylineElementWrapper);
        map.set(radialGradientTag.localName().impl(), createSVGRadialGradientElementWrapper);
        map.set(rectTag.localName().impl(), createSVGRectElementWrapper);
        map.set(svgTag.localName().impl(), createSVGSVGElementWrapper);
        map.set(setTag.localName().impl(), createSVGSetElementWrapper);
        map.set(stopTag.localName().impl(), createSVGStopElementWrapper);
        map.set(switchTag.localName().impl(), createSVGSwitchElementWrapper);
        map.set(symbolTag.localName().impl(), createSVGSymbolElementWrapper);
        map.set(tspanTag.localName().impl(), createSVGTSpanElementWrapper);
        map.set(textTag.localName().impl(), createSVGTextElementWrapper);
        map.set(textPathTag.localName().impl(), createSVGTextPathElementWrapper);
        map.set(titleTag.localName().impl(), createSVGTitleElementWrapper);
        map.set(useTag.localName().impl(), createSVGUseElementWrapper);
        map.set(viewTag.localName().impl(), createSVGViewElementWrapper);
        map.set(scriptTag.localName().impl(), createSVGScriptElementWrapper);
        map.set(styleTag.localName().impl(), createSVGStyleElementWrapper);
        map.set(animateColorTag.localName().impl(), createSVGElementWrapper);
#if ENABLE(SVG_FONTS)
        map.set(altGlyphDefTag.localName().impl(), createSVGAltGlyphDefElementWrapper);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(altGlyphTag.localName().impl(), createSVGAltGlyphElementWrapper);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(altGlyphItemTag.localName().impl(), createSVGAltGlyphItemElementWrapper);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(fontTag.localName().impl(), createSVGFontElementWrapper);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(font_faceTag.localName().impl(), createSVGFontFaceElementWrapper);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(font_face_formatTag.localName().impl(), createSVGFontFaceFormatElementWrapper);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(font_face_nameTag.localName().impl(), createSVGFontFaceNameElementWrapper);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(font_face_srcTag.localName().impl(), createSVGFontFaceSrcElementWrapper);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(font_face_uriTag.localName().impl(), createSVGFontFaceUriElementWrapper);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(glyphTag.localName().impl(), createSVGGlyphElementWrapper);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(glyphRefTag.localName().impl(), createSVGGlyphRefElementWrapper);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(hkernTag.localName().impl(), createSVGHKernElementWrapper);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(missing_glyphTag.localName().impl(), createSVGMissingGlyphElementWrapper);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(vkernTag.localName().impl(), createSVGVKernElementWrapper);
#endif // ENABLE(SVG_FONTS)
    }

    CreateSVGElementWrapperFunction createWrapperFunction = map.get(element->localName().impl());
    if (createWrapperFunction == createSVGElementWrapper)
        createWrapperFunction = createV8SVGDirectWrapper;
    if (element->isCustomElement())
        return CustomElementWrapper<SVGElement, V8SVGElement>::wrap(element, creationContext, isolate, createWrapperFunction);

    if (createWrapperFunction)
        return createWrapperFunction(element, creationContext, isolate);
    return V8SVGElement::createWrapper(element, creationContext, isolate);
}

const WrapperTypeInfo* findWrapperTypeForSVGTagName(const AtomicString& name)
{
    typedef HashMap<StringImpl*, const WrapperTypeInfo*> NameTypeMap;
    DEFINE_STATIC_LOCAL(NameTypeMap, map, ());
    if (map.isEmpty()) {
        // FIXME: This seems wrong. We should list every interface here, not
        // just the ones that have specialized JavaScript interfaces.
        map.set(aTag.localName().impl(), &V8SVGAElement::wrapperTypeInfo);
        map.set(animateTag.localName().impl(), &V8SVGAnimateElement::wrapperTypeInfo);
        map.set(animateMotionTag.localName().impl(), &V8SVGAnimateMotionElement::wrapperTypeInfo);
        map.set(animateTransformTag.localName().impl(), &V8SVGAnimateTransformElement::wrapperTypeInfo);
        map.set(circleTag.localName().impl(), &V8SVGCircleElement::wrapperTypeInfo);
        map.set(clipPathTag.localName().impl(), &V8SVGClipPathElement::wrapperTypeInfo);
        map.set(cursorTag.localName().impl(), &V8SVGCursorElement::wrapperTypeInfo);
        map.set(defsTag.localName().impl(), &V8SVGDefsElement::wrapperTypeInfo);
        map.set(descTag.localName().impl(), &V8SVGDescElement::wrapperTypeInfo);
        map.set(discardTag.localName().impl(), &V8SVGDiscardElement::wrapperTypeInfo);
        map.set(ellipseTag.localName().impl(), &V8SVGEllipseElement::wrapperTypeInfo);
        map.set(feBlendTag.localName().impl(), &V8SVGFEBlendElement::wrapperTypeInfo);
        map.set(feColorMatrixTag.localName().impl(), &V8SVGFEColorMatrixElement::wrapperTypeInfo);
        map.set(feComponentTransferTag.localName().impl(), &V8SVGFEComponentTransferElement::wrapperTypeInfo);
        map.set(feCompositeTag.localName().impl(), &V8SVGFECompositeElement::wrapperTypeInfo);
        map.set(feConvolveMatrixTag.localName().impl(), &V8SVGFEConvolveMatrixElement::wrapperTypeInfo);
        map.set(feDiffuseLightingTag.localName().impl(), &V8SVGFEDiffuseLightingElement::wrapperTypeInfo);
        map.set(feDisplacementMapTag.localName().impl(), &V8SVGFEDisplacementMapElement::wrapperTypeInfo);
        map.set(feDistantLightTag.localName().impl(), &V8SVGFEDistantLightElement::wrapperTypeInfo);
        map.set(feDropShadowTag.localName().impl(), &V8SVGFEDropShadowElement::wrapperTypeInfo);
        map.set(feFloodTag.localName().impl(), &V8SVGFEFloodElement::wrapperTypeInfo);
        map.set(feFuncATag.localName().impl(), &V8SVGFEFuncAElement::wrapperTypeInfo);
        map.set(feFuncBTag.localName().impl(), &V8SVGFEFuncBElement::wrapperTypeInfo);
        map.set(feFuncGTag.localName().impl(), &V8SVGFEFuncGElement::wrapperTypeInfo);
        map.set(feFuncRTag.localName().impl(), &V8SVGFEFuncRElement::wrapperTypeInfo);
        map.set(feGaussianBlurTag.localName().impl(), &V8SVGFEGaussianBlurElement::wrapperTypeInfo);
        map.set(feImageTag.localName().impl(), &V8SVGFEImageElement::wrapperTypeInfo);
        map.set(feMergeTag.localName().impl(), &V8SVGFEMergeElement::wrapperTypeInfo);
        map.set(feMergeNodeTag.localName().impl(), &V8SVGFEMergeNodeElement::wrapperTypeInfo);
        map.set(feMorphologyTag.localName().impl(), &V8SVGFEMorphologyElement::wrapperTypeInfo);
        map.set(feOffsetTag.localName().impl(), &V8SVGFEOffsetElement::wrapperTypeInfo);
        map.set(fePointLightTag.localName().impl(), &V8SVGFEPointLightElement::wrapperTypeInfo);
        map.set(feSpecularLightingTag.localName().impl(), &V8SVGFESpecularLightingElement::wrapperTypeInfo);
        map.set(feSpotLightTag.localName().impl(), &V8SVGFESpotLightElement::wrapperTypeInfo);
        map.set(feTileTag.localName().impl(), &V8SVGFETileElement::wrapperTypeInfo);
        map.set(feTurbulenceTag.localName().impl(), &V8SVGFETurbulenceElement::wrapperTypeInfo);
        map.set(filterTag.localName().impl(), &V8SVGFilterElement::wrapperTypeInfo);
        map.set(foreignObjectTag.localName().impl(), &V8SVGForeignObjectElement::wrapperTypeInfo);
        map.set(gTag.localName().impl(), &V8SVGGElement::wrapperTypeInfo);
        map.set(imageTag.localName().impl(), &V8SVGImageElement::wrapperTypeInfo);
        map.set(lineTag.localName().impl(), &V8SVGLineElement::wrapperTypeInfo);
        map.set(linearGradientTag.localName().impl(), &V8SVGLinearGradientElement::wrapperTypeInfo);
        map.set(mpathTag.localName().impl(), &V8SVGMPathElement::wrapperTypeInfo);
        map.set(markerTag.localName().impl(), &V8SVGMarkerElement::wrapperTypeInfo);
        map.set(maskTag.localName().impl(), &V8SVGMaskElement::wrapperTypeInfo);
        map.set(metadataTag.localName().impl(), &V8SVGMetadataElement::wrapperTypeInfo);
        map.set(pathTag.localName().impl(), &V8SVGPathElement::wrapperTypeInfo);
        map.set(patternTag.localName().impl(), &V8SVGPatternElement::wrapperTypeInfo);
        map.set(polygonTag.localName().impl(), &V8SVGPolygonElement::wrapperTypeInfo);
        map.set(polylineTag.localName().impl(), &V8SVGPolylineElement::wrapperTypeInfo);
        map.set(radialGradientTag.localName().impl(), &V8SVGRadialGradientElement::wrapperTypeInfo);
        map.set(rectTag.localName().impl(), &V8SVGRectElement::wrapperTypeInfo);
        map.set(svgTag.localName().impl(), &V8SVGSVGElement::wrapperTypeInfo);
        map.set(setTag.localName().impl(), &V8SVGSetElement::wrapperTypeInfo);
        map.set(stopTag.localName().impl(), &V8SVGStopElement::wrapperTypeInfo);
        map.set(switchTag.localName().impl(), &V8SVGSwitchElement::wrapperTypeInfo);
        map.set(symbolTag.localName().impl(), &V8SVGSymbolElement::wrapperTypeInfo);
        map.set(tspanTag.localName().impl(), &V8SVGTSpanElement::wrapperTypeInfo);
        map.set(textTag.localName().impl(), &V8SVGTextElement::wrapperTypeInfo);
        map.set(textPathTag.localName().impl(), &V8SVGTextPathElement::wrapperTypeInfo);
        map.set(titleTag.localName().impl(), &V8SVGTitleElement::wrapperTypeInfo);
        map.set(useTag.localName().impl(), &V8SVGUseElement::wrapperTypeInfo);
        map.set(viewTag.localName().impl(), &V8SVGViewElement::wrapperTypeInfo);
        map.set(scriptTag.localName().impl(), &V8SVGScriptElement::wrapperTypeInfo);
        map.set(styleTag.localName().impl(), &V8SVGStyleElement::wrapperTypeInfo);
#if ENABLE(SVG_FONTS)
        map.set(altGlyphDefTag.localName().impl(), &V8SVGAltGlyphDefElement::wrapperTypeInfo);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(altGlyphTag.localName().impl(), &V8SVGAltGlyphElement::wrapperTypeInfo);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(altGlyphItemTag.localName().impl(), &V8SVGAltGlyphItemElement::wrapperTypeInfo);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(fontTag.localName().impl(), &V8SVGFontElement::wrapperTypeInfo);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(font_faceTag.localName().impl(), &V8SVGFontFaceElement::wrapperTypeInfo);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(font_face_formatTag.localName().impl(), &V8SVGFontFaceFormatElement::wrapperTypeInfo);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(font_face_nameTag.localName().impl(), &V8SVGFontFaceNameElement::wrapperTypeInfo);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(font_face_srcTag.localName().impl(), &V8SVGFontFaceSrcElement::wrapperTypeInfo);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(font_face_uriTag.localName().impl(), &V8SVGFontFaceUriElement::wrapperTypeInfo);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(glyphTag.localName().impl(), &V8SVGGlyphElement::wrapperTypeInfo);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(glyphRefTag.localName().impl(), &V8SVGGlyphRefElement::wrapperTypeInfo);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(hkernTag.localName().impl(), &V8SVGHKernElement::wrapperTypeInfo);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(missing_glyphTag.localName().impl(), &V8SVGMissingGlyphElement::wrapperTypeInfo);
#endif // ENABLE(SVG_FONTS)
#if ENABLE(SVG_FONTS)
        map.set(vkernTag.localName().impl(), &V8SVGVKernElement::wrapperTypeInfo);
#endif // ENABLE(SVG_FONTS)
    }

    if (const WrapperTypeInfo* result = map.get(name.impl()))
        return result;

    return &V8SVGElement::wrapperTypeInfo;
}

}
