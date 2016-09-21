#include "config.h"
#include "StyleBuilderFunctions.h"

#include "CSSValueKeywords.h"
#include "core/css/BasicShapeFunctions.h"
#include "core/css/CSSPrimitiveValueMappings.h"
#include "core/css/Pair.h"
#include "core/css/resolver/StyleResolverState.h"


namespace WebCore {

void StyleBuilderFunctions::applyInitialCSSPropertyVectorEffect(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setVectorEffect(SVGRenderStyle::initialVectorEffect());
}

void StyleBuilderFunctions::applyInheritCSSPropertyVectorEffect(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setVectorEffect(state.parentStyle()->svgStyle()->vectorEffect());
}

void StyleBuilderFunctions::applyValueCSSPropertyVectorEffect(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setVectorEffect(static_cast<EVectorEffect>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFillRule(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setFillRule(SVGRenderStyle::initialFillRule());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFillRule(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setFillRule(state.parentStyle()->svgStyle()->fillRule());
}

void StyleBuilderFunctions::applyValueCSSPropertyFillRule(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setFillRule(static_cast<WindRule>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitLineBoxContain(StyleResolverState& state)
{
    state.style()->setLineBoxContain(RenderStyle::initialLineBoxContain());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitLineBoxContain(StyleResolverState& state)
{
    state.style()->setLineBoxContain(state.parentStyle()->lineBoxContain());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitLineBoxContain(StyleResolverState& state, CSSValue* value)
{
    state.style()->setLineBoxContain(StyleBuilderConverter::convertLineBoxContain(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextDecoration(StyleResolverState& state)
{
    state.style()->setTextDecoration(RenderStyle::initialTextDecoration());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextDecoration(StyleResolverState& state)
{
    state.style()->setTextDecoration(state.parentStyle()->textDecoration());
}

void StyleBuilderFunctions::applyInitialCSSPropertyShapeOutside(StyleResolverState& state)
{
    state.style()->setShapeOutside(RenderStyle::initialShapeOutside());
}

void StyleBuilderFunctions::applyInheritCSSPropertyShapeOutside(StyleResolverState& state)
{
    state.style()->setShapeOutside(state.parentStyle()->shapeOutside());
}

void StyleBuilderFunctions::applyInitialCSSPropertyGlyphOrientationVertical(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setGlyphOrientationVertical(SVGRenderStyle::initialGlyphOrientationVertical());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGlyphOrientationVertical(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setGlyphOrientationVertical(state.parentStyle()->svgStyle()->glyphOrientationVertical());
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitHyphenateCharacter(StyleResolverState& state)
{
    state.style()->setHyphenationString(RenderStyle::initialHyphenationString());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitHyphenateCharacter(StyleResolverState& state)
{
    state.style()->setHyphenationString(state.parentStyle()->hyphenationString());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitHyphenateCharacter(StyleResolverState& state, CSSValue* value)
{
    state.style()->setHyphenationString(StyleBuilderConverter::convertString<CSSValueAuto>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxPack(StyleResolverState& state)
{
    state.style()->setBoxPack(RenderStyle::initialBoxPack());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxPack(StyleResolverState& state)
{
    state.style()->setBoxPack(state.parentStyle()->boxPack());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxPack(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxPack(static_cast<EBoxPack>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMarginBottomCollapse(StyleResolverState& state)
{
    state.style()->setMarginAfterCollapse(RenderStyle::initialMarginAfterCollapse());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMarginBottomCollapse(StyleResolverState& state)
{
    state.style()->setMarginAfterCollapse(state.parentStyle()->marginAfterCollapse());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMarginBottomCollapse(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginAfterCollapse(static_cast<EMarginCollapse>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyOutlineWidth(StyleResolverState& state)
{
    state.style()->setOutlineWidth(RenderStyle::initialOutlineWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyOutlineWidth(StyleResolverState& state)
{
    state.style()->setOutlineWidth(state.parentStyle()->outlineWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyOutlineWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOutlineWidth(StyleBuilderConverter::convertLineWidth<unsigned short>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStrokeOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setStrokeOpacity(SVGRenderStyle::initialStrokeOpacity());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStrokeOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setStrokeOpacity(state.parentStyle()->svgStyle()->strokeOpacity());
}

void StyleBuilderFunctions::applyValueCSSPropertyStrokeOpacity(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setStrokeOpacity(StyleBuilderConverter::convertNumberOrPercentage(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitWrapThrough(StyleResolverState& state)
{
    state.style()->setWrapThrough(RenderStyle::initialWrapThrough());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitWrapThrough(StyleResolverState& state)
{
    state.style()->setWrapThrough(state.parentStyle()->wrapThrough());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitWrapThrough(StyleResolverState& state, CSSValue* value)
{
    state.style()->setWrapThrough(static_cast<WrapThrough>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxFlex(StyleResolverState& state)
{
    state.style()->setBoxFlex(RenderStyle::initialBoxFlex());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxFlex(StyleResolverState& state)
{
    state.style()->setBoxFlex(state.parentStyle()->boxFlex());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxFlex(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxFlex(static_cast<float>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitAppearance(StyleResolverState& state)
{
    state.style()->setAppearance(RenderStyle::initialAppearance());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitAppearance(StyleResolverState& state)
{
    state.style()->setAppearance(state.parentStyle()->appearance());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitAppearance(StyleResolverState& state, CSSValue* value)
{
    state.style()->setAppearance(static_cast<ControlPart>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderBottomStyle(StyleResolverState& state)
{
    state.style()->setBorderBottomStyle(RenderStyle::initialBorderStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderBottomStyle(StyleResolverState& state)
{
    state.style()->setBorderBottomStyle(state.parentStyle()->borderBottomStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderBottomStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderBottomStyle(static_cast<EBorderStyle>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyHeight(StyleResolverState& state)
{
    state.style()->setHeight(RenderStyle::initialSize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyHeight(StyleResolverState& state)
{
    state.style()->setHeight(state.parentStyle()->height());
}

void StyleBuilderFunctions::applyValueCSSPropertyHeight(StyleResolverState& state, CSSValue* value)
{
    state.style()->setHeight(StyleBuilderConverter::convertLengthSizing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPaintOrder(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setPaintOrder(SVGRenderStyle::initialPaintOrder());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPaintOrder(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setPaintOrder(state.parentStyle()->svgStyle()->paintOrder());
}

void StyleBuilderFunctions::applyValueCSSPropertyPaintOrder(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setPaintOrder(StyleBuilderConverter::convertPaintOrder(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextRendering(StyleResolverState& state)
{
    state.fontBuilder().setTextRendering(FontBuilder::initialTextRendering());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextRendering(StyleResolverState& state)
{
    state.fontBuilder().setTextRendering(state.parentFontDescription().textRendering());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextRendering(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setTextRendering(static_cast<TextRenderingMode>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnBreakAfter(StyleResolverState& state)
{
    state.style()->setColumnBreakAfter(RenderStyle::initialPageBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnBreakAfter(StyleResolverState& state)
{
    state.style()->setColumnBreakAfter(state.parentStyle()->columnBreakAfter());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnBreakAfter(StyleResolverState& state, CSSValue* value)
{
    state.style()->setColumnBreakAfter(static_cast<EPageBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderBottomLeftRadius(StyleResolverState& state)
{
    state.style()->setBorderBottomLeftRadius(RenderStyle::initialBorderRadius());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderBottomLeftRadius(StyleResolverState& state)
{
    state.style()->setBorderBottomLeftRadius(state.parentStyle()->borderBottomLeftRadius());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderBottomLeftRadius(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderBottomLeftRadius(StyleBuilderConverter::convertRadius(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyColorInterpolation(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setColorInterpolation(SVGRenderStyle::initialColorInterpolation());
}

void StyleBuilderFunctions::applyInheritCSSPropertyColorInterpolation(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setColorInterpolation(state.parentStyle()->svgStyle()->colorInterpolation());
}

void StyleBuilderFunctions::applyValueCSSPropertyColorInterpolation(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setColorInterpolation(static_cast<EColorInterpolation>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFilter(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setFilterResource(SVGRenderStyle::initialFilterResource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFilter(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setFilterResource(state.parentStyle()->svgStyle()->filterResource());
}

void StyleBuilderFunctions::applyValueCSSPropertyFilter(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setFilterResource(StyleBuilderConverter::convertFragmentIdentifier(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyShapeMargin(StyleResolverState& state)
{
    state.style()->setShapeMargin(RenderStyle::initialShapeMargin());
}

void StyleBuilderFunctions::applyInheritCSSPropertyShapeMargin(StyleResolverState& state)
{
    state.style()->setShapeMargin(state.parentStyle()->shapeMargin());
}

void StyleBuilderFunctions::applyValueCSSPropertyShapeMargin(StyleResolverState& state, CSSValue* value)
{
    state.style()->setShapeMargin(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyInternalMarqueeDirection(StyleResolverState& state)
{
    state.style()->setMarqueeDirection(RenderStyle::initialMarqueeDirection());
}

void StyleBuilderFunctions::applyInheritCSSPropertyInternalMarqueeDirection(StyleResolverState& state)
{
    state.style()->setMarqueeDirection(state.parentStyle()->marqueeDirection());
}

void StyleBuilderFunctions::applyValueCSSPropertyInternalMarqueeDirection(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarqueeDirection(static_cast<EMarqueeDirection>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyImageRendering(StyleResolverState& state)
{
    state.style()->setImageRendering(RenderStyle::initialImageRendering());
}

void StyleBuilderFunctions::applyInheritCSSPropertyImageRendering(StyleResolverState& state)
{
    state.style()->setImageRendering(state.parentStyle()->imageRendering());
}

void StyleBuilderFunctions::applyValueCSSPropertyImageRendering(StyleResolverState& state, CSSValue* value)
{
    state.style()->setImageRendering(static_cast<EImageRendering>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitUserSelect(StyleResolverState& state)
{
    state.style()->setUserSelect(RenderStyle::initialUserSelect());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitUserSelect(StyleResolverState& state)
{
    state.style()->setUserSelect(state.parentStyle()->userSelect());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitUserSelect(StyleResolverState& state, CSSValue* value)
{
    state.style()->setUserSelect(static_cast<EUserSelect>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPaddingBottom(StyleResolverState& state)
{
    state.style()->setPaddingBottom(RenderStyle::initialPadding());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPaddingBottom(StyleResolverState& state)
{
    state.style()->setPaddingBottom(state.parentStyle()->paddingBottom());
}

void StyleBuilderFunctions::applyValueCSSPropertyPaddingBottom(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPaddingBottom(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyQuotes(StyleResolverState& state)
{
    state.style()->setQuotes(RenderStyle::initialQuotes());
}

void StyleBuilderFunctions::applyInheritCSSPropertyQuotes(StyleResolverState& state)
{
    state.style()->setQuotes(state.parentStyle()->quotes());
}

void StyleBuilderFunctions::applyValueCSSPropertyQuotes(StyleResolverState& state, CSSValue* value)
{
    state.style()->setQuotes(StyleBuilderConverter::convertQuotes(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridAutoRows(StyleResolverState& state)
{
    state.style()->setGridAutoRows(RenderStyle::initialGridAutoRows());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridAutoRows(StyleResolverState& state)
{
    state.style()->setGridAutoRows(state.parentStyle()->gridAutoRows());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridAutoRows(StyleResolverState& state, CSSValue* value)
{
    state.style()->setGridAutoRows(StyleBuilderConverter::convertGridTrackSize(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyClipPath(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setClipperResource(SVGRenderStyle::initialClipperResource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyClipPath(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setClipperResource(state.parentStyle()->svgStyle()->clipperResource());
}

void StyleBuilderFunctions::applyValueCSSPropertyClipPath(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setClipperResource(StyleBuilderConverter::convertFragmentIdentifier(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyObjectFit(StyleResolverState& state)
{
    state.style()->setObjectFit(RenderStyle::initialObjectFit());
}

void StyleBuilderFunctions::applyInheritCSSPropertyObjectFit(StyleResolverState& state)
{
    state.style()->setObjectFit(state.parentStyle()->objectFit());
}

void StyleBuilderFunctions::applyValueCSSPropertyObjectFit(StyleResolverState& state, CSSValue* value)
{
    state.style()->setObjectFit(static_cast<ObjectFit>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStopOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setStopOpacity(SVGRenderStyle::initialStopOpacity());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStopOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setStopOpacity(state.parentStyle()->svgStyle()->stopOpacity());
}

void StyleBuilderFunctions::applyValueCSSPropertyStopOpacity(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setStopOpacity(StyleBuilderConverter::convertNumberOrPercentage(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxReflect(StyleResolverState& state)
{
    state.style()->setBoxReflect(RenderStyle::initialBoxReflect());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxReflect(StyleResolverState& state)
{
    state.style()->setBoxReflect(state.parentStyle()->boxReflect());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxReflect(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxReflect(StyleBuilderConverter::convertBoxReflect(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderTopRightRadius(StyleResolverState& state)
{
    state.style()->setBorderTopRightRadius(RenderStyle::initialBorderRadius());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderTopRightRadius(StyleResolverState& state)
{
    state.style()->setBorderTopRightRadius(state.parentStyle()->borderTopRightRadius());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderTopRightRadius(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderTopRightRadius(StyleBuilderConverter::convertRadius(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxDirection(StyleResolverState& state)
{
    state.style()->setBoxDirection(RenderStyle::initialBoxDirection());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxDirection(StyleResolverState& state)
{
    state.style()->setBoxDirection(state.parentStyle()->boxDirection());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxDirection(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxDirection(static_cast<EBoxDirection>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitUserModify(StyleResolverState& state)
{
    state.style()->setUserModify(RenderStyle::initialUserModify());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitUserModify(StyleResolverState& state)
{
    state.style()->setUserModify(state.parentStyle()->userModify());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitUserModify(StyleResolverState& state, CSSValue* value)
{
    state.style()->setUserModify(static_cast<EUserModify>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxDecorationBreak(StyleResolverState& state)
{
    state.style()->setBoxDecorationBreak(RenderStyle::initialBoxDecorationBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxDecorationBreak(StyleResolverState& state)
{
    state.style()->setBoxDecorationBreak(state.parentStyle()->boxDecorationBreak());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxDecorationBreak(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxDecorationBreak(static_cast<EBoxDecorationBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFontKerning(StyleResolverState& state)
{
    state.fontBuilder().setKerning(FontBuilder::initialKerning());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFontKerning(StyleResolverState& state)
{
    state.fontBuilder().setKerning(state.parentFontDescription().kerning());
}

void StyleBuilderFunctions::applyValueCSSPropertyFontKerning(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setKerning(static_cast<FontDescription::Kerning>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyShapeImageThreshold(StyleResolverState& state)
{
    state.style()->setShapeImageThreshold(RenderStyle::initialShapeImageThreshold());
}

void StyleBuilderFunctions::applyInheritCSSPropertyShapeImageThreshold(StyleResolverState& state)
{
    state.style()->setShapeImageThreshold(state.parentStyle()->shapeImageThreshold());
}

void StyleBuilderFunctions::applyValueCSSPropertyShapeImageThreshold(StyleResolverState& state, CSSValue* value)
{
    state.style()->setShapeImageThreshold(static_cast<float>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMarginRight(StyleResolverState& state)
{
    state.style()->setMarginRight(RenderStyle::initialMargin());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMarginRight(StyleResolverState& state)
{
    state.style()->setMarginRight(state.parentStyle()->marginRight());
}

void StyleBuilderFunctions::applyValueCSSPropertyMarginRight(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginRight(StyleBuilderConverter::convertLengthOrAuto(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitFontSmoothing(StyleResolverState& state)
{
    state.fontBuilder().setFontSmoothing(FontBuilder::initialFontSmoothing());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitFontSmoothing(StyleResolverState& state)
{
    state.fontBuilder().setFontSmoothing(state.parentFontDescription().fontSmoothing());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitFontSmoothing(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setFontSmoothing(static_cast<FontSmoothingMode>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPageBreakBefore(StyleResolverState& state)
{
    state.style()->setPageBreakBefore(RenderStyle::initialPageBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPageBreakBefore(StyleResolverState& state)
{
    state.style()->setPageBreakBefore(state.parentStyle()->pageBreakBefore());
}

void StyleBuilderFunctions::applyValueCSSPropertyPageBreakBefore(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPageBreakBefore(static_cast<EPageBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyOpacity(StyleResolverState& state)
{
    state.style()->setOpacity(RenderStyle::initialOpacity());
}

void StyleBuilderFunctions::applyInheritCSSPropertyOpacity(StyleResolverState& state)
{
    state.style()->setOpacity(state.parentStyle()->opacity());
}

void StyleBuilderFunctions::applyValueCSSPropertyOpacity(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOpacity(static_cast<float>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyDominantBaseline(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setDominantBaseline(SVGRenderStyle::initialDominantBaseline());
}

void StyleBuilderFunctions::applyInheritCSSPropertyDominantBaseline(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setDominantBaseline(state.parentStyle()->svgStyle()->dominantBaseline());
}

void StyleBuilderFunctions::applyValueCSSPropertyDominantBaseline(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setDominantBaseline(static_cast<EDominantBaseline>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyAlignSelf(StyleResolverState& state)
{
    state.style()->setAlignSelf(RenderStyle::initialAlignSelf());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAlignSelf(StyleResolverState& state)
{
    state.style()->setAlignSelf(state.parentStyle()->alignSelf());
}

void StyleBuilderFunctions::applyInitialCSSPropertyClear(StyleResolverState& state)
{
    state.style()->setClear(RenderStyle::initialClear());
}

void StyleBuilderFunctions::applyInheritCSSPropertyClear(StyleResolverState& state)
{
    state.style()->setClear(state.parentStyle()->clear());
}

void StyleBuilderFunctions::applyValueCSSPropertyClear(StyleResolverState& state, CSSValue* value)
{
    state.style()->setClear(static_cast<EClear>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnBreakInside(StyleResolverState& state)
{
    state.style()->setColumnBreakInside(RenderStyle::initialPageBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnBreakInside(StyleResolverState& state)
{
    state.style()->setColumnBreakInside(state.parentStyle()->columnBreakInside());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnBreakInside(StyleResolverState& state, CSSValue* value)
{
    state.style()->setColumnBreakInside(static_cast<EPageBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskBoxImageSource(StyleResolverState& state)
{
    state.style()->setMaskBoxImageSource(RenderStyle::initialMaskBoxImageSource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskBoxImageSource(StyleResolverState& state)
{
    state.style()->setMaskBoxImageSource(state.parentStyle()->maskBoxImageSource());
}

void StyleBuilderFunctions::applyInitialCSSPropertyTransformStyle(StyleResolverState& state)
{
    state.style()->setTransformStyle3D(RenderStyle::initialTransformStyle3D());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTransformStyle(StyleResolverState& state)
{
    state.style()->setTransformStyle3D(state.parentStyle()->transformStyle3D());
}

void StyleBuilderFunctions::applyValueCSSPropertyTransformStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTransformStyle3D(static_cast<ETransformStyle3D>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBorderHorizontalSpacing(StyleResolverState& state)
{
    state.style()->setHorizontalBorderSpacing(RenderStyle::initialHorizontalBorderSpacing());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBorderHorizontalSpacing(StyleResolverState& state)
{
    state.style()->setHorizontalBorderSpacing(state.parentStyle()->horizontalBorderSpacing());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBorderHorizontalSpacing(StyleResolverState& state, CSSValue* value)
{
    state.style()->setHorizontalBorderSpacing(StyleBuilderConverter::convertComputedLength<short>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTapHighlightColor(StyleResolverState& state)
{
    state.style()->setTapHighlightColor(RenderStyle::initialTapHighlightColor());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTapHighlightColor(StyleResolverState& state)
{
    state.style()->setTapHighlightColor(state.parentStyle()->tapHighlightColor());
}

void StyleBuilderFunctions::applyInitialCSSPropertyCaptionSide(StyleResolverState& state)
{
    state.style()->setCaptionSide(RenderStyle::initialCaptionSide());
}

void StyleBuilderFunctions::applyInheritCSSPropertyCaptionSide(StyleResolverState& state)
{
    state.style()->setCaptionSide(state.parentStyle()->captionSide());
}

void StyleBuilderFunctions::applyValueCSSPropertyCaptionSide(StyleResolverState& state, CSSValue* value)
{
    state.style()->setCaptionSide(static_cast<ECaptionSide>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitPrintColorAdjust(StyleResolverState& state)
{
    state.style()->setPrintColorAdjust(RenderStyle::initialPrintColorAdjust());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitPrintColorAdjust(StyleResolverState& state)
{
    state.style()->setPrintColorAdjust(state.parentStyle()->printColorAdjust());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitPrintColorAdjust(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPrintColorAdjust(static_cast<PrintColorAdjust>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStrokeDasharray(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setStrokeDashArray(SVGRenderStyle::initialStrokeDashArray());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStrokeDasharray(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setStrokeDashArray(state.parentStyle()->svgStyle()->strokeDashArray());
}

void StyleBuilderFunctions::applyValueCSSPropertyStrokeDasharray(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setStrokeDashArray(StyleBuilderConverter::convertStrokeDasharray(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFlexBasis(StyleResolverState& state)
{
    state.style()->setFlexBasis(RenderStyle::initialFlexBasis());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFlexBasis(StyleResolverState& state)
{
    state.style()->setFlexBasis(state.parentStyle()->flexBasis());
}

void StyleBuilderFunctions::applyValueCSSPropertyFlexBasis(StyleResolverState& state, CSSValue* value)
{
    state.style()->setFlexBasis(StyleBuilderConverter::convertLengthOrAuto(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitRubyPosition(StyleResolverState& state)
{
    state.style()->setRubyPosition(RenderStyle::initialRubyPosition());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitRubyPosition(StyleResolverState& state)
{
    state.style()->setRubyPosition(state.parentStyle()->rubyPosition());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitRubyPosition(StyleResolverState& state, CSSValue* value)
{
    state.style()->setRubyPosition(static_cast<RubyPosition>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFlexShrink(StyleResolverState& state)
{
    state.style()->setFlexShrink(RenderStyle::initialFlexShrink());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFlexShrink(StyleResolverState& state)
{
    state.style()->setFlexShrink(state.parentStyle()->flexShrink());
}

void StyleBuilderFunctions::applyValueCSSPropertyFlexShrink(StyleResolverState& state, CSSValue* value)
{
    state.style()->setFlexShrink(static_cast<float>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTransformOriginY(StyleResolverState& state)
{
    state.style()->setTransformOriginY(RenderStyle::initialTransformOriginY());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTransformOriginY(StyleResolverState& state)
{
    state.style()->setTransformOriginY(state.parentStyle()->transformOriginY());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTransformOriginY(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTransformOriginY(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTransformOriginX(StyleResolverState& state)
{
    state.style()->setTransformOriginX(RenderStyle::initialTransformOriginX());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTransformOriginX(StyleResolverState& state)
{
    state.style()->setTransformOriginX(state.parentStyle()->transformOriginX());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTransformOriginX(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTransformOriginX(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTransform(StyleResolverState& state)
{
    state.style()->setTransform(RenderStyle::initialTransform());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTransform(StyleResolverState& state)
{
    state.style()->setTransform(state.parentStyle()->transform());
}

void StyleBuilderFunctions::applyInitialCSSPropertyScrollBehavior(StyleResolverState& state)
{
    state.style()->setScrollBehavior(RenderStyle::initialScrollBehavior());
}

void StyleBuilderFunctions::applyInheritCSSPropertyScrollBehavior(StyleResolverState& state)
{
    state.style()->setScrollBehavior(state.parentStyle()->scrollBehavior());
}

void StyleBuilderFunctions::applyValueCSSPropertyScrollBehavior(StyleResolverState& state, CSSValue* value)
{
    state.style()->setScrollBehavior(static_cast<ScrollBehavior>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridAutoFlow(StyleResolverState& state)
{
    state.style()->setGridAutoFlow(RenderStyle::initialGridAutoFlow());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridAutoFlow(StyleResolverState& state)
{
    state.style()->setGridAutoFlow(state.parentStyle()->gridAutoFlow());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridAutoFlow(StyleResolverState& state, CSSValue* value)
{
    state.style()->setGridAutoFlow(static_cast<GridAutoFlow>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTouchActionDelay(StyleResolverState& state)
{
    state.style()->setTouchActionDelay(RenderStyle::initialTouchActionDelay());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTouchActionDelay(StyleResolverState& state)
{
    state.style()->setTouchActionDelay(state.parentStyle()->touchActionDelay());
}

void StyleBuilderFunctions::applyValueCSSPropertyTouchActionDelay(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTouchActionDelay(static_cast<TouchActionDelay>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStopColor(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setStopColor(SVGRenderStyle::initialStopColor());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStopColor(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setStopColor(state.parentStyle()->svgStyle()->stopColor());
}

void StyleBuilderFunctions::applyValueCSSPropertyStopColor(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setStopColor(StyleBuilderConverter::convertSVGColor(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitLineClamp(StyleResolverState& state)
{
    state.style()->setLineClamp(RenderStyle::initialLineClamp());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitLineClamp(StyleResolverState& state)
{
    state.style()->setLineClamp(state.parentStyle()->lineClamp());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitLineClamp(StyleResolverState& state, CSSValue* value)
{
    state.style()->setLineClamp(static_cast<LineClampValue>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyJustifySelf(StyleResolverState& state)
{
    state.style()->setJustifySelf(RenderStyle::initialJustifySelf());
}

void StyleBuilderFunctions::applyInheritCSSPropertyJustifySelf(StyleResolverState& state)
{
    state.style()->setJustifySelf(state.parentStyle()->justifySelf());
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTransformStyle(StyleResolverState& state)
{
    state.style()->setTransformStyle3D(RenderStyle::initialTransformStyle3D());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTransformStyle(StyleResolverState& state)
{
    state.style()->setTransformStyle3D(state.parentStyle()->transformStyle3D());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTransformStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTransformStyle3D(static_cast<ETransformStyle3D>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextJustify(StyleResolverState& state)
{
    state.style()->setTextJustify(RenderStyle::initialTextJustify());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextJustify(StyleResolverState& state)
{
    state.style()->setTextJustify(state.parentStyle()->textJustify());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextJustify(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextJustify(static_cast<TextJustify>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextAnchor(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setTextAnchor(SVGRenderStyle::initialTextAnchor());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextAnchor(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setTextAnchor(state.parentStyle()->svgStyle()->textAnchor());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextAnchor(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setTextAnchor(static_cast<ETextAnchor>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFontStyle(StyleResolverState& state)
{
    state.fontBuilder().setStyle(FontBuilder::initialStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFontStyle(StyleResolverState& state)
{
    state.fontBuilder().setStyle(state.parentFontDescription().style());
}

void StyleBuilderFunctions::applyValueCSSPropertyFontStyle(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setStyle(static_cast<FontStyle>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderBottomRightRadius(StyleResolverState& state)
{
    state.style()->setBorderBottomRightRadius(RenderStyle::initialBorderRadius());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderBottomRightRadius(StyleResolverState& state)
{
    state.style()->setBorderBottomRightRadius(state.parentStyle()->borderBottomRightRadius());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderBottomRightRadius(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderBottomRightRadius(StyleBuilderConverter::convertRadius(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderRightWidth(StyleResolverState& state)
{
    state.style()->setBorderRightWidth(RenderStyle::initialBorderWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderRightWidth(StyleResolverState& state)
{
    state.style()->setBorderRightWidth(state.parentStyle()->borderRightWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderRightWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderRightWidth(StyleBuilderConverter::convertLineWidth<unsigned>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderTopLeftRadius(StyleResolverState& state)
{
    state.style()->setBorderTopLeftRadius(RenderStyle::initialBorderRadius());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderTopLeftRadius(StyleResolverState& state)
{
    state.style()->setBorderTopLeftRadius(state.parentStyle()->borderTopLeftRadius());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderTopLeftRadius(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderTopLeftRadius(StyleBuilderConverter::convertRadius(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFontVariant(StyleResolverState& state)
{
    state.fontBuilder().setVariant(FontBuilder::initialVariant());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFontVariant(StyleResolverState& state)
{
    state.fontBuilder().setVariant(state.parentFontDescription().variant());
}

void StyleBuilderFunctions::applyValueCSSPropertyFontVariant(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setVariant(static_cast<FontVariant>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWritingMode(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setWritingMode(SVGRenderStyle::initialWritingMode());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWritingMode(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setWritingMode(state.parentStyle()->svgStyle()->writingMode());
}

void StyleBuilderFunctions::applyValueCSSPropertyWritingMode(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setWritingMode(static_cast<SVGWritingMode>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextSecurity(StyleResolverState& state)
{
    state.style()->setTextSecurity(RenderStyle::initialTextSecurity());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextSecurity(StyleResolverState& state)
{
    state.style()->setTextSecurity(state.parentStyle()->textSecurity());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTextSecurity(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextSecurity(static_cast<ETextSecurity>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderLeftWidth(StyleResolverState& state)
{
    state.style()->setBorderLeftWidth(RenderStyle::initialBorderWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderLeftWidth(StyleResolverState& state)
{
    state.style()->setBorderLeftWidth(state.parentStyle()->borderLeftWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderLeftWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderLeftWidth(StyleBuilderConverter::convertLineWidth<unsigned>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitLineBreak(StyleResolverState& state)
{
    state.style()->setLineBreak(RenderStyle::initialLineBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitLineBreak(StyleResolverState& state)
{
    state.style()->setLineBreak(state.parentStyle()->lineBreak());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitLineBreak(StyleResolverState& state, CSSValue* value)
{
    state.style()->setLineBreak(static_cast<LineBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyIsolation(StyleResolverState& state)
{
    state.style()->setIsolation(RenderStyle::initialIsolation());
}

void StyleBuilderFunctions::applyInheritCSSPropertyIsolation(StyleResolverState& state)
{
    state.style()->setIsolation(state.parentStyle()->isolation());
}

void StyleBuilderFunctions::applyValueCSSPropertyIsolation(StyleResolverState& state, CSSValue* value)
{
    state.style()->setIsolation(static_cast<EIsolation>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGlyphOrientationHorizontal(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setGlyphOrientationHorizontal(SVGRenderStyle::initialGlyphOrientationHorizontal());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGlyphOrientationHorizontal(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setGlyphOrientationHorizontal(state.parentStyle()->svgStyle()->glyphOrientationHorizontal());
}

void StyleBuilderFunctions::applyValueCSSPropertyGlyphOrientationHorizontal(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setGlyphOrientationHorizontal(StyleBuilderConverter::convertGlyphOrientation(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFillOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setFillOpacity(SVGRenderStyle::initialFillOpacity());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFillOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setFillOpacity(state.parentStyle()->svgStyle()->fillOpacity());
}

void StyleBuilderFunctions::applyValueCSSPropertyFillOpacity(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setFillOpacity(StyleBuilderConverter::convertNumberOrPercentage(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderTopWidth(StyleResolverState& state)
{
    state.style()->setBorderTopWidth(RenderStyle::initialBorderWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderTopWidth(StyleResolverState& state)
{
    state.style()->setBorderTopWidth(state.parentStyle()->borderTopWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderTopWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderTopWidth(StyleBuilderConverter::convertLineWidth<unsigned>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBottom(StyleResolverState& state)
{
    state.style()->setBottom(RenderStyle::initialOffset());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBottom(StyleResolverState& state)
{
    state.style()->setBottom(state.parentStyle()->bottom());
}

void StyleBuilderFunctions::applyValueCSSPropertyBottom(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBottom(StyleBuilderConverter::convertLengthOrAuto(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderCollapse(StyleResolverState& state)
{
    state.style()->setBorderCollapse(RenderStyle::initialBorderCollapse());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderCollapse(StyleResolverState& state)
{
    state.style()->setBorderCollapse(state.parentStyle()->borderCollapse());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderCollapse(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderCollapse(static_cast<EBorderCollapse>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTop(StyleResolverState& state)
{
    state.style()->setTop(RenderStyle::initialOffset());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTop(StyleResolverState& state)
{
    state.style()->setTop(state.parentStyle()->top());
}

void StyleBuilderFunctions::applyValueCSSPropertyTop(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTop(StyleBuilderConverter::convertLengthOrAuto(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyUnicodeBidi(StyleResolverState& state)
{
    state.style()->setUnicodeBidi(RenderStyle::initialUnicodeBidi());
}

void StyleBuilderFunctions::applyInheritCSSPropertyUnicodeBidi(StyleResolverState& state)
{
    state.style()->setUnicodeBidi(state.parentStyle()->unicodeBidi());
}

void StyleBuilderFunctions::applyValueCSSPropertyUnicodeBidi(StyleResolverState& state, CSSValue* value)
{
    state.style()->setUnicodeBidi(static_cast<EUnicodeBidi>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFloat(StyleResolverState& state)
{
    state.style()->setFloating(RenderStyle::initialFloating());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFloat(StyleResolverState& state)
{
    state.style()->setFloating(state.parentStyle()->floating());
}

void StyleBuilderFunctions::applyValueCSSPropertyFloat(StyleResolverState& state, CSSValue* value)
{
    state.style()->setFloating(static_cast<EFloat>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitRtlOrdering(StyleResolverState& state)
{
    state.style()->setRTLOrdering(RenderStyle::initialRTLOrdering());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitRtlOrdering(StyleResolverState& state)
{
    state.style()->setRTLOrdering(state.parentStyle()->rtlOrdering());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitRtlOrdering(StyleResolverState& state, CSSValue* value)
{
    state.style()->setRTLOrdering(static_cast<Order>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyInternalMarqueeSpeed(StyleResolverState& state)
{
    state.style()->setMarqueeSpeed(RenderStyle::initialMarqueeSpeed());
}

void StyleBuilderFunctions::applyInheritCSSPropertyInternalMarqueeSpeed(StyleResolverState& state)
{
    state.style()->setMarqueeSpeed(state.parentStyle()->marqueeSpeed());
}

void StyleBuilderFunctions::applyInitialCSSPropertyWordWrap(StyleResolverState& state)
{
    state.style()->setOverflowWrap(RenderStyle::initialOverflowWrap());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWordWrap(StyleResolverState& state)
{
    state.style()->setOverflowWrap(state.parentStyle()->overflowWrap());
}

void StyleBuilderFunctions::applyValueCSSPropertyWordWrap(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOverflowWrap(static_cast<EOverflowWrap>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMarginTop(StyleResolverState& state)
{
    state.style()->setMarginTop(RenderStyle::initialMargin());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMarginTop(StyleResolverState& state)
{
    state.style()->setMarginTop(state.parentStyle()->marginTop());
}

void StyleBuilderFunctions::applyValueCSSPropertyMarginTop(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginTop(StyleBuilderConverter::convertLengthOrAuto(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMaxWidth(StyleResolverState& state)
{
    state.style()->setMaxWidth(RenderStyle::initialMaxSize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMaxWidth(StyleResolverState& state)
{
    state.style()->setMaxWidth(state.parentStyle()->maxWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyMaxWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMaxWidth(StyleBuilderConverter::convertLengthMaxSizing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextShadow(StyleResolverState& state)
{
    state.style()->setTextShadow(RenderStyle::initialTextShadow());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextShadow(StyleResolverState& state)
{
    state.style()->setTextShadow(state.parentStyle()->textShadow());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextShadow(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextShadow(StyleBuilderConverter::convertShadow(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPaddingRight(StyleResolverState& state)
{
    state.style()->setPaddingRight(RenderStyle::initialPadding());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPaddingRight(StyleResolverState& state)
{
    state.style()->setPaddingRight(state.parentStyle()->paddingRight());
}

void StyleBuilderFunctions::applyValueCSSPropertyPaddingRight(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPaddingRight(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyAlignContent(StyleResolverState& state)
{
    state.style()->setAlignContent(RenderStyle::initialAlignContent());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAlignContent(StyleResolverState& state)
{
    state.style()->setAlignContent(state.parentStyle()->alignContent());
}

void StyleBuilderFunctions::applyValueCSSPropertyAlignContent(StyleResolverState& state, CSSValue* value)
{
    state.style()->setAlignContent(static_cast<EAlignContent>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxOrdinalGroup(StyleResolverState& state)
{
    state.style()->setBoxOrdinalGroup(RenderStyle::initialBoxOrdinalGroup());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxOrdinalGroup(StyleResolverState& state)
{
    state.style()->setBoxOrdinalGroup(state.parentStyle()->boxOrdinalGroup());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxOrdinalGroup(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxOrdinalGroup(static_cast<unsigned int>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyColumnFill(StyleResolverState& state)
{
    state.style()->setColumnFill(RenderStyle::initialColumnFill());
}

void StyleBuilderFunctions::applyInheritCSSPropertyColumnFill(StyleResolverState& state)
{
    state.style()->setColumnFill(state.parentStyle()->columnFill());
}

void StyleBuilderFunctions::applyValueCSSPropertyColumnFill(StyleResolverState& state, CSSValue* value)
{
    state.style()->setColumnFill(static_cast<ColumnFill>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyOverflowX(StyleResolverState& state)
{
    state.style()->setOverflowX(RenderStyle::initialOverflowX());
}

void StyleBuilderFunctions::applyInheritCSSPropertyOverflowX(StyleResolverState& state)
{
    state.style()->setOverflowX(state.parentStyle()->overflowX());
}

void StyleBuilderFunctions::applyValueCSSPropertyOverflowX(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOverflowX(static_cast<EOverflow>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyOverflowY(StyleResolverState& state)
{
    state.style()->setOverflowY(RenderStyle::initialOverflowY());
}

void StyleBuilderFunctions::applyInheritCSSPropertyOverflowY(StyleResolverState& state)
{
    state.style()->setOverflowY(state.parentStyle()->overflowY());
}

void StyleBuilderFunctions::applyValueCSSPropertyOverflowY(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOverflowY(static_cast<EOverflow>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPerspective(StyleResolverState& state)
{
    state.style()->setPerspective(RenderStyle::initialPerspective());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPerspective(StyleResolverState& state)
{
    state.style()->setPerspective(state.parentStyle()->perspective());
}

void StyleBuilderFunctions::applyInitialCSSPropertyLineHeight(StyleResolverState& state)
{
    state.style()->setLineHeight(RenderStyle::initialLineHeight());
}

void StyleBuilderFunctions::applyInheritCSSPropertyLineHeight(StyleResolverState& state)
{
    state.style()->setLineHeight(state.parentStyle()->specifiedLineHeight());
}

void StyleBuilderFunctions::applyInitialCSSPropertyOrder(StyleResolverState& state)
{
    state.style()->setOrder(RenderStyle::initialOrder());
}

void StyleBuilderFunctions::applyInheritCSSPropertyOrder(StyleResolverState& state)
{
    state.style()->setOrder(state.parentStyle()->order());
}

void StyleBuilderFunctions::applyValueCSSPropertyOrder(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOrder(static_cast<int>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxAlign(StyleResolverState& state)
{
    state.style()->setBoxAlign(RenderStyle::initialBoxAlign());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxAlign(StyleResolverState& state)
{
    state.style()->setBoxAlign(state.parentStyle()->boxAlign());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxAlign(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxAlign(static_cast<EBoxAlignment>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridAutoColumns(StyleResolverState& state)
{
    state.style()->setGridAutoColumns(RenderStyle::initialGridAutoColumns());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridAutoColumns(StyleResolverState& state)
{
    state.style()->setGridAutoColumns(state.parentStyle()->gridAutoColumns());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridAutoColumns(StyleResolverState& state, CSSValue* value)
{
    state.style()->setGridAutoColumns(StyleBuilderConverter::convertGridTrackSize(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridRowStart(StyleResolverState& state)
{
    state.style()->setGridRowStart(RenderStyle::initialGridRowStart());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridRowStart(StyleResolverState& state)
{
    state.style()->setGridRowStart(state.parentStyle()->gridRowStart());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridRowStart(StyleResolverState& state, CSSValue* value)
{
    state.style()->setGridRowStart(StyleBuilderConverter::convertGridPosition(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextStrokeWidth(StyleResolverState& state)
{
    state.style()->setTextStrokeWidth(RenderStyle::initialTextStrokeWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextStrokeWidth(StyleResolverState& state)
{
    state.style()->setTextStrokeWidth(state.parentStyle()->textStrokeWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTextStrokeWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextStrokeWidth(StyleBuilderConverter::convertTextStrokeWidth(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStrokeDashoffset(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setStrokeDashOffset(SVGRenderStyle::initialStrokeDashOffset());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStrokeDashoffset(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setStrokeDashOffset(state.parentStyle()->svgStyle()->strokeDashOffset());
}

void StyleBuilderFunctions::applyValueCSSPropertyStrokeDashoffset(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setStrokeDashOffset(StyleBuilderConverter::convertSVGLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPosition(StyleResolverState& state)
{
    state.style()->setPosition(RenderStyle::initialPosition());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPosition(StyleResolverState& state)
{
    state.style()->setPosition(state.parentStyle()->position());
}

void StyleBuilderFunctions::applyValueCSSPropertyPosition(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPosition(static_cast<EPosition>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMixBlendMode(StyleResolverState& state)
{
    state.style()->setBlendMode(RenderStyle::initialBlendMode());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMixBlendMode(StyleResolverState& state)
{
    state.style()->setBlendMode(state.parentStyle()->blendMode());
}

void StyleBuilderFunctions::applyValueCSSPropertyMixBlendMode(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBlendMode(static_cast<blink::WebBlendMode>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBaselineShift(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setBaselineShift(SVGRenderStyle::initialBaselineShift());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBaselineShift(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setBaselineShift(state.parentStyle()->svgStyle()->baselineShift());
}

void StyleBuilderFunctions::applyInitialCSSPropertyPaddingLeft(StyleResolverState& state)
{
    state.style()->setPaddingLeft(RenderStyle::initialPadding());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPaddingLeft(StyleResolverState& state)
{
    state.style()->setPaddingLeft(state.parentStyle()->paddingLeft());
}

void StyleBuilderFunctions::applyValueCSSPropertyPaddingLeft(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPaddingLeft(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWhiteSpace(StyleResolverState& state)
{
    state.style()->setWhiteSpace(RenderStyle::initialWhiteSpace());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWhiteSpace(StyleResolverState& state)
{
    state.style()->setWhiteSpace(state.parentStyle()->whiteSpace());
}

void StyleBuilderFunctions::applyValueCSSPropertyWhiteSpace(StyleResolverState& state, CSSValue* value)
{
    state.style()->setWhiteSpace(static_cast<EWhiteSpace>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyOverflowWrap(StyleResolverState& state)
{
    state.style()->setOverflowWrap(RenderStyle::initialOverflowWrap());
}

void StyleBuilderFunctions::applyInheritCSSPropertyOverflowWrap(StyleResolverState& state)
{
    state.style()->setOverflowWrap(state.parentStyle()->overflowWrap());
}

void StyleBuilderFunctions::applyValueCSSPropertyOverflowWrap(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOverflowWrap(static_cast<EOverflowWrap>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyVerticalAlign(StyleResolverState& state)
{
    state.style()->setVerticalAlign(RenderStyle::initialVerticalAlign());
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitLocale(StyleResolverState& state)
{
    state.style()->setLocale(RenderStyle::initialLocale());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitLocale(StyleResolverState& state)
{
    state.style()->setLocale(state.parentStyle()->locale());
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMarginAfterCollapse(StyleResolverState& state)
{
    state.style()->setMarginAfterCollapse(RenderStyle::initialMarginAfterCollapse());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMarginAfterCollapse(StyleResolverState& state)
{
    state.style()->setMarginAfterCollapse(state.parentStyle()->marginAfterCollapse());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMarginAfterCollapse(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginAfterCollapse(static_cast<EMarginCollapse>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextEmphasisPosition(StyleResolverState& state)
{
    state.style()->setTextEmphasisPosition(RenderStyle::initialTextEmphasisPosition());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextEmphasisPosition(StyleResolverState& state)
{
    state.style()->setTextEmphasisPosition(state.parentStyle()->textEmphasisPosition());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTextEmphasisPosition(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextEmphasisPosition(static_cast<TextEmphasisPosition>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWordSpacing(StyleResolverState& state)
{
    state.style()->setWordSpacing(RenderStyle::initialLetterWordSpacing());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWordSpacing(StyleResolverState& state)
{
    state.style()->setWordSpacing(state.parentStyle()->wordSpacing());
}

void StyleBuilderFunctions::applyValueCSSPropertyWordSpacing(StyleResolverState& state, CSSValue* value)
{
    state.style()->setWordSpacing(StyleBuilderConverter::convertSpacing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPageBreakAfter(StyleResolverState& state)
{
    state.style()->setPageBreakAfter(RenderStyle::initialPageBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPageBreakAfter(StyleResolverState& state)
{
    state.style()->setPageBreakAfter(state.parentStyle()->pageBreakAfter());
}

void StyleBuilderFunctions::applyValueCSSPropertyPageBreakAfter(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPageBreakAfter(static_cast<EPageBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMarkerEnd(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setMarkerEndResource(SVGRenderStyle::initialMarkerEndResource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMarkerEnd(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setMarkerEndResource(state.parentStyle()->svgStyle()->markerEndResource());
}

void StyleBuilderFunctions::applyValueCSSPropertyMarkerEnd(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setMarkerEndResource(StyleBuilderConverter::convertFragmentIdentifier(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxLines(StyleResolverState& state)
{
    state.style()->setBoxLines(RenderStyle::initialBoxLines());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxLines(StyleResolverState& state)
{
    state.style()->setBoxLines(state.parentStyle()->boxLines());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxLines(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxLines(static_cast<EBoxLines>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTableLayout(StyleResolverState& state)
{
    state.style()->setTableLayout(RenderStyle::initialTableLayout());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTableLayout(StyleResolverState& state)
{
    state.style()->setTableLayout(state.parentStyle()->tableLayout());
}

void StyleBuilderFunctions::applyValueCSSPropertyTableLayout(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTableLayout(static_cast<ETableLayout>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderRightStyle(StyleResolverState& state)
{
    state.style()->setBorderRightStyle(RenderStyle::initialBorderStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderRightStyle(StyleResolverState& state)
{
    state.style()->setBorderRightStyle(state.parentStyle()->borderRightStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderRightStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderRightStyle(static_cast<EBorderStyle>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridRowEnd(StyleResolverState& state)
{
    state.style()->setGridRowEnd(RenderStyle::initialGridRowEnd());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridRowEnd(StyleResolverState& state)
{
    state.style()->setGridRowEnd(state.parentStyle()->gridRowEnd());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridRowEnd(StyleResolverState& state, CSSValue* value)
{
    state.style()->setGridRowEnd(StyleBuilderConverter::convertGridPosition(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextUnderlinePosition(StyleResolverState& state)
{
    state.style()->setTextUnderlinePosition(RenderStyle::initialTextUnderlinePosition());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextUnderlinePosition(StyleResolverState& state)
{
    state.style()->setTextUnderlinePosition(state.parentStyle()->textUnderlinePosition());
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackfaceVisibility(StyleResolverState& state)
{
    state.style()->setBackfaceVisibility(RenderStyle::initialBackfaceVisibility());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackfaceVisibility(StyleResolverState& state)
{
    state.style()->setBackfaceVisibility(state.parentStyle()->backfaceVisibility());
}

void StyleBuilderFunctions::applyValueCSSPropertyBackfaceVisibility(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBackfaceVisibility(static_cast<EBackfaceVisibility>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyLeft(StyleResolverState& state)
{
    state.style()->setLeft(RenderStyle::initialOffset());
}

void StyleBuilderFunctions::applyInheritCSSPropertyLeft(StyleResolverState& state)
{
    state.style()->setLeft(state.parentStyle()->left());
}

void StyleBuilderFunctions::applyValueCSSPropertyLeft(StyleResolverState& state, CSSValue* value)
{
    state.style()->setLeft(StyleBuilderConverter::convertLengthOrAuto(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWidth(StyleResolverState& state)
{
    state.style()->setWidth(RenderStyle::initialSize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWidth(StyleResolverState& state)
{
    state.style()->setWidth(state.parentStyle()->width());
}

void StyleBuilderFunctions::applyValueCSSPropertyWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setWidth(StyleBuilderConverter::convertLengthSizing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTouchAction(StyleResolverState& state)
{
    state.style()->setTouchAction(RenderStyle::initialTouchAction());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTouchAction(StyleResolverState& state)
{
    state.style()->setTouchAction(state.parentStyle()->touchAction());
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitClipPath(StyleResolverState& state)
{
    state.style()->setClipPath(RenderStyle::initialClipPath());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitClipPath(StyleResolverState& state)
{
    state.style()->setClipPath(state.parentStyle()->clipPath());
}

void StyleBuilderFunctions::applyInitialCSSPropertyStrokeMiterlimit(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setStrokeMiterLimit(SVGRenderStyle::initialStrokeMiterLimit());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStrokeMiterlimit(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setStrokeMiterLimit(state.parentStyle()->svgStyle()->strokeMiterLimit());
}

void StyleBuilderFunctions::applyValueCSSPropertyStrokeMiterlimit(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setStrokeMiterLimit(static_cast<float>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridColumnStart(StyleResolverState& state)
{
    state.style()->setGridColumnStart(RenderStyle::initialGridColumnStart());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridColumnStart(StyleResolverState& state)
{
    state.style()->setGridColumnStart(state.parentStyle()->gridColumnStart());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridColumnStart(StyleResolverState& state, CSSValue* value)
{
    state.style()->setGridColumnStart(StyleBuilderConverter::convertGridPosition(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMarginBottom(StyleResolverState& state)
{
    state.style()->setMarginBottom(RenderStyle::initialMargin());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMarginBottom(StyleResolverState& state)
{
    state.style()->setMarginBottom(state.parentStyle()->marginBottom());
}

void StyleBuilderFunctions::applyValueCSSPropertyMarginBottom(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginBottom(StyleBuilderConverter::convertLengthOrAuto(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyListStylePosition(StyleResolverState& state)
{
    state.style()->setListStylePosition(RenderStyle::initialListStylePosition());
}

void StyleBuilderFunctions::applyInheritCSSPropertyListStylePosition(StyleResolverState& state)
{
    state.style()->setListStylePosition(state.parentStyle()->listStylePosition());
}

void StyleBuilderFunctions::applyValueCSSPropertyListStylePosition(StyleResolverState& state, CSSValue* value)
{
    state.style()->setListStylePosition(static_cast<EListStylePosition>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitFilter(StyleResolverState& state)
{
    state.style()->setFilter(RenderStyle::initialFilter());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitFilter(StyleResolverState& state)
{
    state.style()->setFilter(state.parentStyle()->filter());
}

void StyleBuilderFunctions::applyInitialCSSPropertyOutlineOffset(StyleResolverState& state)
{
    state.style()->setOutlineOffset(RenderStyle::initialOutlineOffset());
}

void StyleBuilderFunctions::applyInheritCSSPropertyOutlineOffset(StyleResolverState& state)
{
    state.style()->setOutlineOffset(state.parentStyle()->outlineOffset());
}

void StyleBuilderFunctions::applyValueCSSPropertyOutlineOffset(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOutlineOffset(StyleBuilderConverter::convertComputedLength<int>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyShapeRendering(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setShapeRendering(SVGRenderStyle::initialShapeRendering());
}

void StyleBuilderFunctions::applyInheritCSSPropertyShapeRendering(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setShapeRendering(state.parentStyle()->svgStyle()->shapeRendering());
}

void StyleBuilderFunctions::applyValueCSSPropertyShapeRendering(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setShapeRendering(static_cast<EShapeRendering>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBorderFit(StyleResolverState& state)
{
    state.style()->setBorderFit(RenderStyle::initialBorderFit());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBorderFit(StyleResolverState& state)
{
    state.style()->setBorderFit(state.parentStyle()->borderFit());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBorderFit(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderFit(static_cast<EBorderFit>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyColorRendering(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setColorRendering(SVGRenderStyle::initialColorRendering());
}

void StyleBuilderFunctions::applyInheritCSSPropertyColorRendering(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setColorRendering(state.parentStyle()->svgStyle()->colorRendering());
}

void StyleBuilderFunctions::applyValueCSSPropertyColorRendering(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setColorRendering(static_cast<EColorRendering>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStrokeLinejoin(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setJoinStyle(SVGRenderStyle::initialJoinStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStrokeLinejoin(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setJoinStyle(state.parentStyle()->svgStyle()->joinStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyStrokeLinejoin(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setJoinStyle(static_cast<LineJoin>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFlexGrow(StyleResolverState& state)
{
    state.style()->setFlexGrow(RenderStyle::initialFlexGrow());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFlexGrow(StyleResolverState& state)
{
    state.style()->setFlexGrow(state.parentStyle()->flexGrow());
}

void StyleBuilderFunctions::applyValueCSSPropertyFlexGrow(StyleResolverState& state, CSSValue* value)
{
    state.style()->setFlexGrow(static_cast<float>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMask(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setMaskerResource(SVGRenderStyle::initialMaskerResource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMask(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setMaskerResource(state.parentStyle()->svgStyle()->maskerResource());
}

void StyleBuilderFunctions::applyValueCSSPropertyMask(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setMaskerResource(StyleBuilderConverter::convertFragmentIdentifier(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMarginBeforeCollapse(StyleResolverState& state)
{
    state.style()->setMarginBeforeCollapse(RenderStyle::initialMarginBeforeCollapse());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMarginBeforeCollapse(StyleResolverState& state)
{
    state.style()->setMarginBeforeCollapse(state.parentStyle()->marginBeforeCollapse());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMarginBeforeCollapse(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginBeforeCollapse(static_cast<EMarginCollapse>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPointerEvents(StyleResolverState& state)
{
    state.style()->setPointerEvents(RenderStyle::initialPointerEvents());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPointerEvents(StyleResolverState& state)
{
    state.style()->setPointerEvents(state.parentStyle()->pointerEvents());
}

void StyleBuilderFunctions::applyValueCSSPropertyPointerEvents(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPointerEvents(static_cast<EPointerEvents>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxOrient(StyleResolverState& state)
{
    state.style()->setBoxOrient(RenderStyle::initialBoxOrient());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxOrient(StyleResolverState& state)
{
    state.style()->setBoxOrient(state.parentStyle()->boxOrient());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxOrient(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxOrient(static_cast<EBoxOrient>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStrokeLinecap(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setCapStyle(SVGRenderStyle::initialCapStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStrokeLinecap(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setCapStyle(state.parentStyle()->svgStyle()->capStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyStrokeLinecap(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setCapStyle(static_cast<LineCap>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderLeftStyle(StyleResolverState& state)
{
    state.style()->setBorderLeftStyle(RenderStyle::initialBorderStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderLeftStyle(StyleResolverState& state)
{
    state.style()->setBorderLeftStyle(state.parentStyle()->borderLeftStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderLeftStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderLeftStyle(static_cast<EBorderStyle>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnRuleWidth(StyleResolverState& state)
{
    state.style()->setColumnRuleWidth(RenderStyle::initialColumnRuleWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnRuleWidth(StyleResolverState& state)
{
    state.style()->setColumnRuleWidth(state.parentStyle()->columnRuleWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnRuleWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setColumnRuleWidth(StyleBuilderConverter::convertLineWidth<unsigned short>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitUserDrag(StyleResolverState& state)
{
    state.style()->setUserDrag(RenderStyle::initialUserDrag());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitUserDrag(StyleResolverState& state)
{
    state.style()->setUserDrag(state.parentStyle()->userDrag());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitUserDrag(StyleResolverState& state, CSSValue* value)
{
    state.style()->setUserDrag(static_cast<EUserDrag>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMarkerMid(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setMarkerMidResource(SVGRenderStyle::initialMarkerMidResource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMarkerMid(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setMarkerMidResource(state.parentStyle()->svgStyle()->markerMidResource());
}

void StyleBuilderFunctions::applyValueCSSPropertyMarkerMid(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setMarkerMidResource(StyleBuilderConverter::convertFragmentIdentifier(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextCombine(StyleResolverState& state)
{
    state.style()->setTextCombine(RenderStyle::initialTextCombine());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextCombine(StyleResolverState& state)
{
    state.style()->setTextCombine(state.parentStyle()->textCombine());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTextCombine(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextCombine(static_cast<TextCombine>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnSpan(StyleResolverState& state)
{
    state.style()->setColumnSpan(RenderStyle::initialColumnSpan());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnSpan(StyleResolverState& state)
{
    state.style()->setColumnSpan(state.parentStyle()->columnSpan());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnSpan(StyleResolverState& state, CSSValue* value)
{
    state.style()->setColumnSpan(static_cast<ColumnSpan>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyResize(StyleResolverState& state)
{
    state.style()->setResize(RenderStyle::initialResize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyResize(StyleResolverState& state)
{
    state.style()->setResize(state.parentStyle()->resize());
}

void StyleBuilderFunctions::applyInitialCSSPropertyLetterSpacing(StyleResolverState& state)
{
    state.style()->setLetterSpacing(RenderStyle::initialLetterWordSpacing());
}

void StyleBuilderFunctions::applyInheritCSSPropertyLetterSpacing(StyleResolverState& state)
{
    state.style()->setLetterSpacing(state.parentStyle()->letterSpacing());
}

void StyleBuilderFunctions::applyValueCSSPropertyLetterSpacing(StyleResolverState& state, CSSValue* value)
{
    state.style()->setLetterSpacing(StyleBuilderConverter::convertSpacing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTransformOriginZ(StyleResolverState& state)
{
    state.style()->setTransformOriginZ(RenderStyle::initialTransformOriginZ());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTransformOriginZ(StyleResolverState& state)
{
    state.style()->setTransformOriginZ(state.parentStyle()->transformOriginZ());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTransformOriginZ(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTransformOriginZ(StyleBuilderConverter::convertComputedLength<float>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextOrientation(StyleResolverState& state)
{
    state.style()->setTextOrientation(RenderStyle::initialTextOrientation());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextOrientation(StyleResolverState& state)
{
    state.style()->setTextOrientation(state.parentStyle()->textOrientation());
}

void StyleBuilderFunctions::applyInitialCSSPropertyColorInterpolationFilters(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setColorInterpolationFilters(SVGRenderStyle::initialColorInterpolationFilters());
}

void StyleBuilderFunctions::applyInheritCSSPropertyColorInterpolationFilters(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setColorInterpolationFilters(state.parentStyle()->svgStyle()->colorInterpolationFilters());
}

void StyleBuilderFunctions::applyValueCSSPropertyColorInterpolationFilters(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setColorInterpolationFilters(static_cast<EColorInterpolation>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnBreakBefore(StyleResolverState& state)
{
    state.style()->setColumnBreakBefore(RenderStyle::initialPageBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnBreakBefore(StyleResolverState& state)
{
    state.style()->setColumnBreakBefore(state.parentStyle()->columnBreakBefore());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnBreakBefore(StyleResolverState& state, CSSValue* value)
{
    state.style()->setColumnBreakBefore(static_cast<EPageBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextTransform(StyleResolverState& state)
{
    state.style()->setTextTransform(RenderStyle::initialTextTransform());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextTransform(StyleResolverState& state)
{
    state.style()->setTextTransform(state.parentStyle()->textTransform());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextTransform(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextTransform(static_cast<ETextTransform>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyRight(StyleResolverState& state)
{
    state.style()->setRight(RenderStyle::initialOffset());
}

void StyleBuilderFunctions::applyInheritCSSPropertyRight(StyleResolverState& state)
{
    state.style()->setRight(state.parentStyle()->right());
}

void StyleBuilderFunctions::applyValueCSSPropertyRight(StyleResolverState& state, CSSValue* value)
{
    state.style()->setRight(StyleBuilderConverter::convertLengthOrAuto(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridColumnEnd(StyleResolverState& state)
{
    state.style()->setGridColumnEnd(RenderStyle::initialGridColumnEnd());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridColumnEnd(StyleResolverState& state)
{
    state.style()->setGridColumnEnd(state.parentStyle()->gridColumnEnd());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridColumnEnd(StyleResolverState& state, CSSValue* value)
{
    state.style()->setGridColumnEnd(StyleBuilderConverter::convertGridPosition(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyDirection(StyleResolverState& state)
{
    state.style()->setDirection(RenderStyle::initialDirection());
}

void StyleBuilderFunctions::applyInheritCSSPropertyDirection(StyleResolverState& state)
{
    state.style()->setDirection(state.parentStyle()->direction());
}

void StyleBuilderFunctions::applyInitialCSSPropertyInternalMarqueeRepetition(StyleResolverState& state)
{
    state.style()->setMarqueeLoopCount(RenderStyle::initialMarqueeLoopCount());
}

void StyleBuilderFunctions::applyInheritCSSPropertyInternalMarqueeRepetition(StyleResolverState& state)
{
    state.style()->setMarqueeLoopCount(state.parentStyle()->marqueeLoopCount());
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBackfaceVisibility(StyleResolverState& state)
{
    state.style()->setBackfaceVisibility(RenderStyle::initialBackfaceVisibility());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBackfaceVisibility(StyleResolverState& state)
{
    state.style()->setBackfaceVisibility(state.parentStyle()->backfaceVisibility());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBackfaceVisibility(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBackfaceVisibility(static_cast<EBackfaceVisibility>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBorderImage(StyleResolverState& state)
{
    state.style()->setBorderImage(RenderStyle::initialNinePieceImage());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBorderImage(StyleResolverState& state)
{
    state.style()->setBorderImage(state.parentStyle()->borderImage());
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBorderVerticalSpacing(StyleResolverState& state)
{
    state.style()->setVerticalBorderSpacing(RenderStyle::initialVerticalBorderSpacing());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBorderVerticalSpacing(StyleResolverState& state)
{
    state.style()->setVerticalBorderSpacing(state.parentStyle()->verticalBorderSpacing());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBorderVerticalSpacing(StyleResolverState& state, CSSValue* value)
{
    state.style()->setVerticalBorderSpacing(StyleBuilderConverter::convertComputedLength<short>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMarginLeft(StyleResolverState& state)
{
    state.style()->setMarginLeft(RenderStyle::initialMargin());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMarginLeft(StyleResolverState& state)
{
    state.style()->setMarginLeft(state.parentStyle()->marginLeft());
}

void StyleBuilderFunctions::applyValueCSSPropertyMarginLeft(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginLeft(StyleBuilderConverter::convertLengthOrAuto(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFlexWrap(StyleResolverState& state)
{
    state.style()->setFlexWrap(RenderStyle::initialFlexWrap());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFlexWrap(StyleResolverState& state)
{
    state.style()->setFlexWrap(state.parentStyle()->flexWrap());
}

void StyleBuilderFunctions::applyValueCSSPropertyFlexWrap(StyleResolverState& state, CSSValue* value)
{
    state.style()->setFlexWrap(static_cast<EFlexWrap>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMinHeight(StyleResolverState& state)
{
    state.style()->setMinHeight(RenderStyle::initialMinSize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMinHeight(StyleResolverState& state)
{
    state.style()->setMinHeight(state.parentStyle()->minHeight());
}

void StyleBuilderFunctions::applyValueCSSPropertyMinHeight(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMinHeight(StyleBuilderConverter::convertLengthSizing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFloodColor(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setFloodColor(SVGRenderStyle::initialFloodColor());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFloodColor(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setFloodColor(state.parentStyle()->svgStyle()->floodColor());
}

void StyleBuilderFunctions::applyValueCSSPropertyFloodColor(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setFloodColor(StyleBuilderConverter::convertSVGColor(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMaxHeight(StyleResolverState& state)
{
    state.style()->setMaxHeight(RenderStyle::initialMaxSize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMaxHeight(StyleResolverState& state)
{
    state.style()->setMaxHeight(state.parentStyle()->maxHeight());
}

void StyleBuilderFunctions::applyValueCSSPropertyMaxHeight(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMaxHeight(StyleBuilderConverter::convertLengthMaxSizing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBufferedRendering(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setBufferedRendering(SVGRenderStyle::initialBufferedRendering());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBufferedRendering(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setBufferedRendering(state.parentStyle()->svgStyle()->bufferedRendering());
}

void StyleBuilderFunctions::applyValueCSSPropertyBufferedRendering(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setBufferedRendering(static_cast<EBufferedRendering>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitWritingMode(StyleResolverState& state)
{
    state.style()->setWritingMode(RenderStyle::initialWritingMode());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitWritingMode(StyleResolverState& state)
{
    state.style()->setWritingMode(state.parentStyle()->writingMode());
}

void StyleBuilderFunctions::applyInitialCSSPropertyAlignmentBaseline(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setAlignmentBaseline(SVGRenderStyle::initialAlignmentBaseline());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAlignmentBaseline(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setAlignmentBaseline(state.parentStyle()->svgStyle()->alignmentBaseline());
}

void StyleBuilderFunctions::applyValueCSSPropertyAlignmentBaseline(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setAlignmentBaseline(static_cast<EAlignmentBaseline>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitWrapFlow(StyleResolverState& state)
{
    state.style()->setWrapFlow(RenderStyle::initialWrapFlow());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitWrapFlow(StyleResolverState& state)
{
    state.style()->setWrapFlow(state.parentStyle()->wrapFlow());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitWrapFlow(StyleResolverState& state, CSSValue* value)
{
    state.style()->setWrapFlow(static_cast<WrapFlow>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMinWidth(StyleResolverState& state)
{
    state.style()->setMinWidth(RenderStyle::initialMinSize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMinWidth(StyleResolverState& state)
{
    state.style()->setMinWidth(state.parentStyle()->minWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyMinWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMinWidth(StyleBuilderConverter::convertLengthSizing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMaskType(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setMaskType(SVGRenderStyle::initialMaskType());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMaskType(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setMaskType(state.parentStyle()->svgStyle()->maskType());
}

void StyleBuilderFunctions::applyValueCSSPropertyMaskType(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setMaskType(static_cast<EMaskType>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnRuleStyle(StyleResolverState& state)
{
    state.style()->setColumnRuleStyle(RenderStyle::initialBorderStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnRuleStyle(StyleResolverState& state)
{
    state.style()->setColumnRuleStyle(state.parentStyle()->columnRuleStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnRuleStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setColumnRuleStyle(static_cast<EBorderStyle>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitPerspectiveOriginY(StyleResolverState& state)
{
    state.style()->setPerspectiveOriginY(RenderStyle::initialPerspectiveOriginY());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitPerspectiveOriginY(StyleResolverState& state)
{
    state.style()->setPerspectiveOriginY(state.parentStyle()->perspectiveOriginY());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitPerspectiveOriginY(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPerspectiveOriginY(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextAlignLast(StyleResolverState& state)
{
    state.style()->setTextAlignLast(RenderStyle::initialTextAlignLast());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextAlignLast(StyleResolverState& state)
{
    state.style()->setTextAlignLast(state.parentStyle()->textAlignLast());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextAlignLast(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextAlignLast(static_cast<TextAlignLast>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMarginTopCollapse(StyleResolverState& state)
{
    state.style()->setMarginBeforeCollapse(RenderStyle::initialMarginBeforeCollapse());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMarginTopCollapse(StyleResolverState& state)
{
    state.style()->setMarginBeforeCollapse(state.parentStyle()->marginBeforeCollapse());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMarginTopCollapse(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginBeforeCollapse(static_cast<EMarginCollapse>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTabSize(StyleResolverState& state)
{
    state.style()->setTabSize(RenderStyle::initialTabSize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTabSize(StyleResolverState& state)
{
    state.style()->setTabSize(state.parentStyle()->tabSize());
}

void StyleBuilderFunctions::applyValueCSSPropertyTabSize(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTabSize(static_cast<unsigned>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyInternalMarqueeIncrement(StyleResolverState& state)
{
    state.style()->setMarqueeIncrement(RenderStyle::initialMarqueeIncrement());
}

void StyleBuilderFunctions::applyInheritCSSPropertyInternalMarqueeIncrement(StyleResolverState& state)
{
    state.style()->setMarqueeIncrement(state.parentStyle()->marqueeIncrement());
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderBottomWidth(StyleResolverState& state)
{
    state.style()->setBorderBottomWidth(RenderStyle::initialBorderWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderBottomWidth(StyleResolverState& state)
{
    state.style()->setBorderBottomWidth(state.parentStyle()->borderBottomWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderBottomWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderBottomWidth(StyleBuilderConverter::convertLineWidth<unsigned>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitHighlight(StyleResolverState& state)
{
    state.style()->setHighlight(RenderStyle::initialHighlight());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitHighlight(StyleResolverState& state)
{
    state.style()->setHighlight(state.parentStyle()->highlight());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitHighlight(StyleResolverState& state, CSSValue* value)
{
    state.style()->setHighlight(StyleBuilderConverter::convertString<CSSValueNone>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyEmptyCells(StyleResolverState& state)
{
    state.style()->setEmptyCells(RenderStyle::initialEmptyCells());
}

void StyleBuilderFunctions::applyInheritCSSPropertyEmptyCells(StyleResolverState& state)
{
    state.style()->setEmptyCells(state.parentStyle()->emptyCells());
}

void StyleBuilderFunctions::applyValueCSSPropertyEmptyCells(StyleResolverState& state, CSSValue* value)
{
    state.style()->setEmptyCells(static_cast<EEmptyCell>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderImageSource(StyleResolverState& state)
{
    state.style()->setBorderImageSource(RenderStyle::initialBorderImageSource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderImageSource(StyleResolverState& state)
{
    state.style()->setBorderImageSource(state.parentStyle()->borderImageSource());
}

void StyleBuilderFunctions::applyInitialCSSPropertyInternalMarqueeStyle(StyleResolverState& state)
{
    state.style()->setMarqueeBehavior(RenderStyle::initialMarqueeBehavior());
}

void StyleBuilderFunctions::applyInheritCSSPropertyInternalMarqueeStyle(StyleResolverState& state)
{
    state.style()->setMarqueeBehavior(state.parentStyle()->marqueeBehavior());
}

void StyleBuilderFunctions::applyValueCSSPropertyInternalMarqueeStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarqueeBehavior(static_cast<EMarqueeBehavior>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextOverflow(StyleResolverState& state)
{
    state.style()->setTextOverflow(RenderStyle::initialTextOverflow());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextOverflow(StyleResolverState& state)
{
    state.style()->setTextOverflow(state.parentStyle()->textOverflow());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextOverflow(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextOverflow(static_cast<TextOverflow>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBoxSizing(StyleResolverState& state)
{
    state.style()->setBoxSizing(RenderStyle::initialBoxSizing());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBoxSizing(StyleResolverState& state)
{
    state.style()->setBoxSizing(state.parentStyle()->boxSizing());
}

void StyleBuilderFunctions::applyValueCSSPropertyBoxSizing(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxSizing(static_cast<EBoxSizing>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStrokeWidth(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setStrokeWidth(SVGRenderStyle::initialStrokeWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStrokeWidth(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setStrokeWidth(state.parentStyle()->svgStyle()->strokeWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyStrokeWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setStrokeWidth(StyleBuilderConverter::convertSVGLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMarkerStart(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setMarkerStartResource(SVGRenderStyle::initialMarkerStartResource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMarkerStart(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setMarkerStartResource(state.parentStyle()->svgStyle()->markerStartResource());
}

void StyleBuilderFunctions::applyValueCSSPropertyMarkerStart(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setMarkerStartResource(StyleBuilderConverter::convertFragmentIdentifier(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextDecorationStyle(StyleResolverState& state)
{
    state.style()->setTextDecorationStyle(RenderStyle::initialTextDecorationStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextDecorationStyle(StyleResolverState& state)
{
    state.style()->setTextDecorationStyle(state.parentStyle()->textDecorationStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextDecorationStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextDecorationStyle(static_cast<TextDecorationStyle>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPaddingTop(StyleResolverState& state)
{
    state.style()->setPaddingTop(RenderStyle::initialPadding());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPaddingTop(StyleResolverState& state)
{
    state.style()->setPaddingTop(state.parentStyle()->paddingTop());
}

void StyleBuilderFunctions::applyValueCSSPropertyPaddingTop(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPaddingTop(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyDisplay(StyleResolverState& state)
{
    state.style()->setDisplay(RenderStyle::initialDisplay());
}

void StyleBuilderFunctions::applyInitialCSSPropertyWordBreak(StyleResolverState& state)
{
    state.style()->setWordBreak(RenderStyle::initialWordBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWordBreak(StyleResolverState& state)
{
    state.style()->setWordBreak(state.parentStyle()->wordBreak());
}

void StyleBuilderFunctions::applyValueCSSPropertyWordBreak(StyleResolverState& state, CSSValue* value)
{
    state.style()->setWordBreak(static_cast<EWordBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderTopStyle(StyleResolverState& state)
{
    state.style()->setBorderTopStyle(RenderStyle::initialBorderStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderTopStyle(StyleResolverState& state)
{
    state.style()->setBorderTopStyle(state.parentStyle()->borderTopStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderTopStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderTopStyle(static_cast<EBorderStyle>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyJustifyContent(StyleResolverState& state)
{
    state.style()->setJustifyContent(RenderStyle::initialJustifyContent());
}

void StyleBuilderFunctions::applyInheritCSSPropertyJustifyContent(StyleResolverState& state)
{
    state.style()->setJustifyContent(state.parentStyle()->justifyContent());
}

void StyleBuilderFunctions::applyValueCSSPropertyJustifyContent(StyleResolverState& state, CSSValue* value)
{
    state.style()->setJustifyContent(static_cast<EJustifyContent>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBoxShadow(StyleResolverState& state)
{
    state.style()->setBoxShadow(RenderStyle::initialBoxShadow());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBoxShadow(StyleResolverState& state)
{
    state.style()->setBoxShadow(state.parentStyle()->boxShadow());
}

void StyleBuilderFunctions::applyValueCSSPropertyBoxShadow(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxShadow(StyleBuilderConverter::convertShadow(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertySpeak(StyleResolverState& state)
{
    state.style()->setSpeak(RenderStyle::initialSpeak());
}

void StyleBuilderFunctions::applyInheritCSSPropertySpeak(StyleResolverState& state)
{
    state.style()->setSpeak(state.parentStyle()->speak());
}

void StyleBuilderFunctions::applyValueCSSPropertySpeak(StyleResolverState& state, CSSValue* value)
{
    state.style()->setSpeak(static_cast<ESpeak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyListStyleImage(StyleResolverState& state)
{
    state.style()->setListStyleImage(RenderStyle::initialListStyleImage());
}

void StyleBuilderFunctions::applyInheritCSSPropertyListStyleImage(StyleResolverState& state)
{
    state.style()->setListStyleImage(state.parentStyle()->listStyleImage());
}

void StyleBuilderFunctions::applyInitialCSSPropertyFloodOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setFloodOpacity(SVGRenderStyle::initialFloodOpacity());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFloodOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setFloodOpacity(state.parentStyle()->svgStyle()->floodOpacity());
}

void StyleBuilderFunctions::applyValueCSSPropertyFloodOpacity(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setFloodOpacity(StyleBuilderConverter::convertNumberOrPercentage(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyClipRule(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setClipRule(SVGRenderStyle::initialClipRule());
}

void StyleBuilderFunctions::applyInheritCSSPropertyClipRule(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setClipRule(state.parentStyle()->svgStyle()->clipRule());
}

void StyleBuilderFunctions::applyValueCSSPropertyClipRule(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setClipRule(static_cast<WindRule>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFlexDirection(StyleResolverState& state)
{
    state.style()->setFlexDirection(RenderStyle::initialFlexDirection());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFlexDirection(StyleResolverState& state)
{
    state.style()->setFlexDirection(state.parentStyle()->flexDirection());
}

void StyleBuilderFunctions::applyValueCSSPropertyFlexDirection(StyleResolverState& state, CSSValue* value)
{
    state.style()->setFlexDirection(static_cast<EFlexDirection>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyLightingColor(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setLightingColor(SVGRenderStyle::initialLightingColor());
}

void StyleBuilderFunctions::applyInheritCSSPropertyLightingColor(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setLightingColor(state.parentStyle()->svgStyle()->lightingColor());
}

void StyleBuilderFunctions::applyValueCSSPropertyLightingColor(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle()->setLightingColor(StyleBuilderConverter::convertSVGColor(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPageBreakInside(StyleResolverState& state)
{
    state.style()->setPageBreakInside(RenderStyle::initialPageBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPageBreakInside(StyleResolverState& state)
{
    state.style()->setPageBreakInside(state.parentStyle()->pageBreakInside());
}

void StyleBuilderFunctions::applyValueCSSPropertyPageBreakInside(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPageBreakInside(static_cast<EPageBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyListStyleType(StyleResolverState& state)
{
    state.style()->setListStyleType(RenderStyle::initialListStyleType());
}

void StyleBuilderFunctions::applyInheritCSSPropertyListStyleType(StyleResolverState& state)
{
    state.style()->setListStyleType(state.parentStyle()->listStyleType());
}

void StyleBuilderFunctions::applyValueCSSPropertyListStyleType(StyleResolverState& state, CSSValue* value)
{
    state.style()->setListStyleType(static_cast<EListStyleType>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextAlign(StyleResolverState& state)
{
    state.style()->setTextAlign(RenderStyle::initialTextAlign());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextAlign(StyleResolverState& state)
{
    state.style()->setTextAlign(state.parentStyle()->textAlign());
}

void StyleBuilderFunctions::applyInitialCSSPropertyAlignItems(StyleResolverState& state)
{
    state.style()->setAlignItems(RenderStyle::initialAlignItems());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAlignItems(StyleResolverState& state)
{
    state.style()->setAlignItems(state.parentStyle()->alignItems());
}

void StyleBuilderFunctions::applyInitialCSSPropertyObjectPosition(StyleResolverState& state)
{
    state.style()->setObjectPosition(RenderStyle::initialObjectPosition());
}

void StyleBuilderFunctions::applyInheritCSSPropertyObjectPosition(StyleResolverState& state)
{
    state.style()->setObjectPosition(state.parentStyle()->objectPosition());
}

void StyleBuilderFunctions::applyValueCSSPropertyObjectPosition(StyleResolverState& state, CSSValue* value)
{
    state.style()->setObjectPosition(StyleBuilderConverter::convertLengthPoint(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxFlexGroup(StyleResolverState& state)
{
    state.style()->setBoxFlexGroup(RenderStyle::initialBoxFlexGroup());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxFlexGroup(StyleResolverState& state)
{
    state.style()->setBoxFlexGroup(state.parentStyle()->boxFlexGroup());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxFlexGroup(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxFlexGroup(static_cast<unsigned int>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitPerspectiveOriginX(StyleResolverState& state)
{
    state.style()->setPerspectiveOriginX(RenderStyle::initialPerspectiveOriginX());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitPerspectiveOriginX(StyleResolverState& state)
{
    state.style()->setPerspectiveOriginX(state.parentStyle()->perspectiveOriginX());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitPerspectiveOriginX(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPerspectiveOriginX(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyVisibility(StyleResolverState& state)
{
    state.style()->setVisibility(RenderStyle::initialVisibility());
}

void StyleBuilderFunctions::applyInheritCSSPropertyVisibility(StyleResolverState& state)
{
    state.style()->setVisibility(state.parentStyle()->visibility());
}

void StyleBuilderFunctions::applyValueCSSPropertyVisibility(StyleResolverState& state, CSSValue* value)
{
    state.style()->setVisibility(static_cast<EVisibility>(*toCSSPrimitiveValue(value)));
}


void StyleBuilderFunctions::applyInitialCSSPropertyWebkitAnimationDelay(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.delayList().clear();
    data.delayList().append(CSSAnimationData::initialDelay());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitAnimationDelay(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyWebkitAnimationDelay(state);
    else
        state.style()->accessAnimations().delayList() = parentData->delayList();
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitAnimationDelay(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.delayList().clear();
    for (CSSValueListIterator i = value; i.hasMore(); i.advance())
        data.delayList().append(state.styleMap().mapAnimationDelay(i.value()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitAnimationDirection(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.directionList().clear();
    data.directionList().append(CSSAnimationData::initialDirection());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitAnimationDirection(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyWebkitAnimationDirection(state);
    else
        state.style()->accessAnimations().directionList() = parentData->directionList();
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitAnimationDirection(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.directionList().clear();
    for (CSSValueListIterator i = value; i.hasMore(); i.advance())
        data.directionList().append(state.styleMap().mapAnimationDirection(i.value()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitAnimationDuration(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.durationList().clear();
    data.durationList().append(CSSAnimationData::initialDuration());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitAnimationDuration(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyWebkitAnimationDuration(state);
    else
        state.style()->accessAnimations().durationList() = parentData->durationList();
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitAnimationDuration(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.durationList().clear();
    for (CSSValueListIterator i = value; i.hasMore(); i.advance())
        data.durationList().append(state.styleMap().mapAnimationDuration(i.value()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitAnimationFillMode(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.fillModeList().clear();
    data.fillModeList().append(CSSAnimationData::initialFillMode());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitAnimationFillMode(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyWebkitAnimationFillMode(state);
    else
        state.style()->accessAnimations().fillModeList() = parentData->fillModeList();
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitAnimationFillMode(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.fillModeList().clear();
    for (CSSValueListIterator i = value; i.hasMore(); i.advance())
        data.fillModeList().append(state.styleMap().mapAnimationFillMode(i.value()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitAnimationIterationCount(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.iterationCountList().clear();
    data.iterationCountList().append(CSSAnimationData::initialIterationCount());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitAnimationIterationCount(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyWebkitAnimationIterationCount(state);
    else
        state.style()->accessAnimations().iterationCountList() = parentData->iterationCountList();
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitAnimationIterationCount(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.iterationCountList().clear();
    for (CSSValueListIterator i = value; i.hasMore(); i.advance())
        data.iterationCountList().append(state.styleMap().mapAnimationIterationCount(i.value()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitAnimationName(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.nameList().clear();
    data.nameList().append(CSSAnimationData::initialName());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitAnimationName(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyWebkitAnimationName(state);
    else
        state.style()->accessAnimations().nameList() = parentData->nameList();
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitAnimationName(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.nameList().clear();
    for (CSSValueListIterator i = value; i.hasMore(); i.advance())
        data.nameList().append(state.styleMap().mapAnimationName(i.value()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitAnimationPlayState(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.playStateList().clear();
    data.playStateList().append(CSSAnimationData::initialPlayState());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitAnimationPlayState(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyWebkitAnimationPlayState(state);
    else
        state.style()->accessAnimations().playStateList() = parentData->playStateList();
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitAnimationPlayState(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.playStateList().clear();
    for (CSSValueListIterator i = value; i.hasMore(); i.advance())
        data.playStateList().append(state.styleMap().mapAnimationPlayState(i.value()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitAnimationTimingFunction(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.timingFunctionList().clear();
    data.timingFunctionList().append(CSSAnimationData::initialTimingFunction());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitAnimationTimingFunction(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyWebkitAnimationTimingFunction(state);
    else
        state.style()->accessAnimations().timingFunctionList() = parentData->timingFunctionList();
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitAnimationTimingFunction(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.timingFunctionList().clear();
    for (CSSValueListIterator i = value; i.hasMore(); i.advance())
        data.timingFunctionList().append(state.styleMap().mapAnimationTimingFunction(i.value()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTransitionDelay(StyleResolverState& state)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.delayList().clear();
    data.delayList().append(CSSTransitionData::initialDelay());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTransitionDelay(StyleResolverState& state)
{
    const CSSTransitionData* parentData = state.parentStyle()->transitions();
    if (!parentData)
        applyInitialCSSPropertyWebkitTransitionDelay(state);
    else
        state.style()->accessTransitions().delayList() = parentData->delayList();
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTransitionDelay(StyleResolverState& state, CSSValue* value)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.delayList().clear();
    for (CSSValueListIterator i = value; i.hasMore(); i.advance())
        data.delayList().append(state.styleMap().mapAnimationDelay(i.value()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTransitionDuration(StyleResolverState& state)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.durationList().clear();
    data.durationList().append(CSSTransitionData::initialDuration());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTransitionDuration(StyleResolverState& state)
{
    const CSSTransitionData* parentData = state.parentStyle()->transitions();
    if (!parentData)
        applyInitialCSSPropertyWebkitTransitionDuration(state);
    else
        state.style()->accessTransitions().durationList() = parentData->durationList();
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTransitionDuration(StyleResolverState& state, CSSValue* value)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.durationList().clear();
    for (CSSValueListIterator i = value; i.hasMore(); i.advance())
        data.durationList().append(state.styleMap().mapAnimationDuration(i.value()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTransitionProperty(StyleResolverState& state)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.propertyList().clear();
    data.propertyList().append(CSSTransitionData::initialProperty());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTransitionProperty(StyleResolverState& state)
{
    const CSSTransitionData* parentData = state.parentStyle()->transitions();
    if (!parentData)
        applyInitialCSSPropertyWebkitTransitionProperty(state);
    else
        state.style()->accessTransitions().propertyList() = parentData->propertyList();
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTransitionProperty(StyleResolverState& state, CSSValue* value)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.propertyList().clear();
    for (CSSValueListIterator i = value; i.hasMore(); i.advance())
        data.propertyList().append(state.styleMap().mapAnimationProperty(i.value()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTransitionTimingFunction(StyleResolverState& state)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.timingFunctionList().clear();
    data.timingFunctionList().append(CSSTransitionData::initialTimingFunction());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTransitionTimingFunction(StyleResolverState& state)
{
    const CSSTransitionData* parentData = state.parentStyle()->transitions();
    if (!parentData)
        applyInitialCSSPropertyWebkitTransitionTimingFunction(state);
    else
        state.style()->accessTransitions().timingFunctionList() = parentData->timingFunctionList();
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTransitionTimingFunction(StyleResolverState& state, CSSValue* value)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.timingFunctionList().clear();
    for (CSSValueListIterator i = value; i.hasMore(); i.advance())
        data.timingFunctionList().append(state.styleMap().mapAnimationTimingFunction(i.value()));
}


void StyleBuilderFunctions::applyInitialCSSPropertyOrphans(StyleResolverState& state)
{
    state.style()->setHasAutoOrphans();
}

void StyleBuilderFunctions::applyInheritCSSPropertyOrphans(StyleResolverState& state)
{
    if (state.parentStyle()->hasAutoOrphans())
        state.style()->setHasAutoOrphans();
    else
        state.style()->setOrphans(state.parentStyle()->orphans());
}

void StyleBuilderFunctions::applyValueCSSPropertyOrphans(StyleResolverState& state, CSSValue* value)
{
    if (!value->isPrimitiveValue())
        return;

    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (primitiveValue->getValueID() == CSSValueAuto)
        state.style()->setHasAutoOrphans();
    else
        state.style()->setOrphans(*primitiveValue);
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnCount(StyleResolverState& state)
{
    state.style()->setHasAutoColumnCount();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnCount(StyleResolverState& state)
{
    if (state.parentStyle()->hasAutoColumnCount())
        state.style()->setHasAutoColumnCount();
    else
        state.style()->setColumnCount(state.parentStyle()->columnCount());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnCount(StyleResolverState& state, CSSValue* value)
{
    if (!value->isPrimitiveValue())
        return;

    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (primitiveValue->getValueID() == CSSValueAuto)
        state.style()->setHasAutoColumnCount();
    else
        state.style()->setColumnCount(*primitiveValue);
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnGap(StyleResolverState& state)
{
    state.style()->setHasNormalColumnGap();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnGap(StyleResolverState& state)
{
    if (state.parentStyle()->hasNormalColumnGap())
        state.style()->setHasNormalColumnGap();
    else
        state.style()->setColumnGap(state.parentStyle()->columnGap());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnGap(StyleResolverState& state, CSSValue* value)
{
    if (!value->isPrimitiveValue())
        return;

    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (primitiveValue->getValueID() == CSSValueNormal)
        state.style()->setHasNormalColumnGap();
    else
        state.style()->setColumnGap(primitiveValue->computeLength<float>(state.cssToLengthConversionData()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnWidth(StyleResolverState& state)
{
    state.style()->setHasAutoColumnWidth();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnWidth(StyleResolverState& state)
{
    if (state.parentStyle()->hasAutoColumnWidth())
        state.style()->setHasAutoColumnWidth();
    else
        state.style()->setColumnWidth(state.parentStyle()->columnWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnWidth(StyleResolverState& state, CSSValue* value)
{
    if (!value->isPrimitiveValue())
        return;

    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (primitiveValue->getValueID() == CSSValueAuto)
        state.style()->setHasAutoColumnWidth();
    else
        state.style()->setColumnWidth(primitiveValue->computeLength<float>(state.cssToLengthConversionData()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWidows(StyleResolverState& state)
{
    state.style()->setHasAutoWidows();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWidows(StyleResolverState& state)
{
    if (state.parentStyle()->hasAutoWidows())
        state.style()->setHasAutoWidows();
    else
        state.style()->setWidows(state.parentStyle()->widows());
}

void StyleBuilderFunctions::applyValueCSSPropertyWidows(StyleResolverState& state, CSSValue* value)
{
    if (!value->isPrimitiveValue())
        return;

    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (primitiveValue->getValueID() == CSSValueAuto)
        state.style()->setHasAutoWidows();
    else
        state.style()->setWidows(*primitiveValue);
}

void StyleBuilderFunctions::applyInitialCSSPropertyZIndex(StyleResolverState& state)
{
    state.style()->setHasAutoZIndex();
}

void StyleBuilderFunctions::applyInheritCSSPropertyZIndex(StyleResolverState& state)
{
    if (state.parentStyle()->hasAutoZIndex())
        state.style()->setHasAutoZIndex();
    else
        state.style()->setZIndex(state.parentStyle()->zIndex());
}

void StyleBuilderFunctions::applyValueCSSPropertyZIndex(StyleResolverState& state, CSSValue* value)
{
    if (!value->isPrimitiveValue())
        return;

    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (primitiveValue->getValueID() == CSSValueAuto)
        state.style()->setHasAutoZIndex();
    else
        state.style()->setZIndex(*primitiveValue);
}


static bool lengthTypeAndValueMatch(const Length& length, LengthType type, float value)
{
    return length.type() == type && length.value() == value;
}

static bool lengthTypeAndValueMatch(const LengthBox& lengthBox, LengthType type, float value)
{
    return (lengthTypeAndValueMatch(lengthBox.left(), type, value)
        && lengthTypeAndValueMatch(lengthBox.right(), type, value)
        && lengthTypeAndValueMatch(lengthBox.top(), type, value)
        && lengthTypeAndValueMatch(lengthBox.bottom(), type, value));
}

static bool lengthTypeAndValueMatch(const BorderImageLength& borderImageLength, LengthType type, float value)
{
    return borderImageLength.isLength() && lengthTypeAndValueMatch(borderImageLength.length(), type, value);
}

static bool lengthTypeAndValueMatch(const BorderImageLengthBox& borderImageLengthBox, LengthType type, float value)
{
    return (lengthTypeAndValueMatch(borderImageLengthBox.left(), type, value)
        && lengthTypeAndValueMatch(borderImageLengthBox.right(), type, value)
        && lengthTypeAndValueMatch(borderImageLengthBox.top(), type, value)
        && lengthTypeAndValueMatch(borderImageLengthBox.bottom(), type, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderImageOutset(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->borderImage();
    if (lengthTypeAndValueMatch(currentImage.outset(), Fixed, 0))
        return;

    NinePieceImage image(currentImage);
    image.setOutset(Length(0, Fixed));
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderImageOutset(StyleResolverState& state)
{
    NinePieceImage image(state.style()->borderImage());
    image.copyOutsetFrom(state.parentStyle()->borderImage());
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderImageOutset(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->borderImage());
    image.setOutset(state.styleMap().mapNinePieceImageQuad(value));
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderImageRepeat(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->borderImage();
    if (currentImage.horizontalRule() == StretchImageRule && currentImage.verticalRule() == StretchImageRule)
        return;

    NinePieceImage image(currentImage);
    image.setHorizontalRule(StretchImageRule);
    image.setVerticalRule(StretchImageRule);
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderImageRepeat(StyleResolverState& state)
{
    NinePieceImage image(state.style()->borderImage());
    image.copyRepeatFrom(state.parentStyle()->borderImage());
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderImageRepeat(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->borderImage());
    state.styleMap().mapNinePieceImageRepeat(value, image);
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderImageSlice(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->borderImage();
    if (currentImage.fill() == false && lengthTypeAndValueMatch(currentImage.imageSlices(), Percent, 100))
        return;

    NinePieceImage image(currentImage);
    image.setImageSlices(LengthBox(Length(100, Percent), Length(100, Percent), Length(100, Percent), Length(100, Percent)));
    image.setFill(false);
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderImageSlice(StyleResolverState& state)
{
    NinePieceImage image(state.style()->borderImage());
    image.copyImageSlicesFrom(state.parentStyle()->borderImage());
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderImageSlice(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->borderImage());
    state.styleMap().mapNinePieceImageSlice(value, image);
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderImageWidth(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->borderImage();
    if (lengthTypeAndValueMatch(currentImage.borderSlices(), Fixed, 1))
        return;

    NinePieceImage image(currentImage);
    image.setBorderSlices(1.0);
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderImageWidth(StyleResolverState& state)
{
    NinePieceImage image(state.style()->borderImage());
    image.copyBorderSlicesFrom(state.parentStyle()->borderImage());
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderImageWidth(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->borderImage());
    image.setBorderSlices(state.styleMap().mapNinePieceImageQuad(value));
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskBoxImageOutset(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->maskBoxImage();
    if (lengthTypeAndValueMatch(currentImage.outset(), Fixed, 0))
        return;

    NinePieceImage image(currentImage);
    image.setOutset(Length(0, Fixed));
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskBoxImageOutset(StyleResolverState& state)
{
    NinePieceImage image(state.style()->maskBoxImage());
    image.copyOutsetFrom(state.parentStyle()->maskBoxImage());
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskBoxImageOutset(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->maskBoxImage());
    image.setOutset(state.styleMap().mapNinePieceImageQuad(value));
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskBoxImageRepeat(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->maskBoxImage();
    if (currentImage.horizontalRule() == StretchImageRule && currentImage.verticalRule() == StretchImageRule)
        return;

    NinePieceImage image(currentImage);
    image.setHorizontalRule(StretchImageRule);
    image.setVerticalRule(StretchImageRule);
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskBoxImageRepeat(StyleResolverState& state)
{
    NinePieceImage image(state.style()->maskBoxImage());
    image.copyRepeatFrom(state.parentStyle()->maskBoxImage());
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskBoxImageRepeat(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->maskBoxImage());
    state.styleMap().mapNinePieceImageRepeat(value, image);
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskBoxImageSlice(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->maskBoxImage();
    // Masks have a different initial value for slices. Preserve the value of 0 for backwards compatibility.
    if (currentImage.fill() == true && lengthTypeAndValueMatch(currentImage.imageSlices(), Fixed, 0))
        return;

    NinePieceImage image(currentImage);
    image.setImageSlices(LengthBox(Length(0, Fixed), Length(0, Fixed), Length(0, Fixed), Length(0, Fixed)));
    image.setFill(true);
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskBoxImageSlice(StyleResolverState& state)
{
    NinePieceImage image(state.style()->maskBoxImage());
    image.copyImageSlicesFrom(state.parentStyle()->maskBoxImage());
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskBoxImageSlice(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->maskBoxImage());
    state.styleMap().mapNinePieceImageSlice(value, image);
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskBoxImageWidth(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->maskBoxImage();
    // Masks have a different initial value for widths. Preserve the value of 'auto' for backwards compatibility.
    if (lengthTypeAndValueMatch(currentImage.borderSlices(), Auto, 0))
        return;

    NinePieceImage image(currentImage);
    image.setBorderSlices(Length(Auto));
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskBoxImageWidth(StyleResolverState& state)
{
    NinePieceImage image(state.style()->maskBoxImage());
    image.copyBorderSlicesFrom(state.parentStyle()->maskBoxImage());
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskBoxImageWidth(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->maskBoxImage());
    image.setBorderSlices(state.styleMap().mapNinePieceImageQuad(value));
    state.style()->setMaskBoxImage(image);
}


void StyleBuilderFunctions::applyValueCSSPropertyBorderImageSource(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderImageSource(state.styleImage(CSSPropertyBorderImageSource, value));
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskBoxImageSource(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMaskBoxImageSource(state.styleImage(CSSPropertyWebkitMaskBoxImageSource, value));
}


void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundColor(StyleResolverState& state)
{
    StyleColor color = RenderStyle::initialBackgroundColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setBackgroundColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBackgroundColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->backgroundColor();
    Color resolvedColor = color.resolve(state.parentStyle()->color());
    if (state.applyPropertyToRegularStyle())
        state.style()->setBackgroundColor(resolvedColor);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBackgroundColor(resolvedColor);
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundColor(StyleResolverState& state, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);

    if (state.applyPropertyToRegularStyle())
        state.style()->setBackgroundColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color()));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBackgroundColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color(), true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderBottomColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderBottomColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderBottomColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderBottomColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->borderBottomColor();
    Color resolvedColor = color.resolve(state.parentStyle()->color());
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderBottomColor(resolvedColor);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderBottomColor(resolvedColor);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderBottomColor(StyleResolverState& state, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);

    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderBottomColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color()));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderBottomColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color(), true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderLeftColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderLeftColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderLeftColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderLeftColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->borderLeftColor();
    Color resolvedColor = color.resolve(state.parentStyle()->color());
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderLeftColor(resolvedColor);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderLeftColor(resolvedColor);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderLeftColor(StyleResolverState& state, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);

    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderLeftColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color()));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderLeftColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color(), true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderRightColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderRightColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderRightColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderRightColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->borderRightColor();
    Color resolvedColor = color.resolve(state.parentStyle()->color());
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderRightColor(resolvedColor);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderRightColor(resolvedColor);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderRightColor(StyleResolverState& state, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);

    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderRightColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color()));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderRightColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color(), true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderTopColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderTopColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderTopColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderTopColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->borderTopColor();
    Color resolvedColor = color.resolve(state.parentStyle()->color());
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderTopColor(resolvedColor);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderTopColor(resolvedColor);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderTopColor(StyleResolverState& state, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);

    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderTopColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color()));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderTopColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color(), true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyOutlineColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setOutlineColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkOutlineColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyOutlineColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->outlineColor();
    Color resolvedColor = color.resolve(state.parentStyle()->color());
    if (state.applyPropertyToRegularStyle())
        state.style()->setOutlineColor(resolvedColor);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkOutlineColor(resolvedColor);
}

void StyleBuilderFunctions::applyValueCSSPropertyOutlineColor(StyleResolverState& state, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);

    if (state.applyPropertyToRegularStyle())
        state.style()->setOutlineColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color()));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkOutlineColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color(), true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextDecorationColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextDecorationColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextDecorationColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextDecorationColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->textDecorationColor();
    Color resolvedColor = color.resolve(state.parentStyle()->color());
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextDecorationColor(resolvedColor);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextDecorationColor(resolvedColor);
}

void StyleBuilderFunctions::applyValueCSSPropertyTextDecorationColor(StyleResolverState& state, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);

    if (state.applyPropertyToRegularStyle())
        state.style()->setTextDecorationColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color()));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextDecorationColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color(), true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnRuleColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setColumnRuleColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkColumnRuleColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnRuleColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->columnRuleColor();
    Color resolvedColor = color.resolve(state.parentStyle()->color());
    if (state.applyPropertyToRegularStyle())
        state.style()->setColumnRuleColor(resolvedColor);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkColumnRuleColor(resolvedColor);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnRuleColor(StyleResolverState& state, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);

    if (state.applyPropertyToRegularStyle())
        state.style()->setColumnRuleColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color()));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkColumnRuleColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color(), true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextEmphasisColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextEmphasisColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextEmphasisColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextEmphasisColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->textEmphasisColor();
    Color resolvedColor = color.resolve(state.parentStyle()->color());
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextEmphasisColor(resolvedColor);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextEmphasisColor(resolvedColor);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTextEmphasisColor(StyleResolverState& state, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);

    if (state.applyPropertyToRegularStyle())
        state.style()->setTextEmphasisColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color()));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextEmphasisColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color(), true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextFillColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextFillColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextFillColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextFillColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->textFillColor();
    Color resolvedColor = color.resolve(state.parentStyle()->color());
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextFillColor(resolvedColor);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextFillColor(resolvedColor);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTextFillColor(StyleResolverState& state, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);

    if (state.applyPropertyToRegularStyle())
        state.style()->setTextFillColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color()));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextFillColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color(), true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextStrokeColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextStrokeColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextStrokeColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextStrokeColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->textStrokeColor();
    Color resolvedColor = color.resolve(state.parentStyle()->color());
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextStrokeColor(resolvedColor);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextStrokeColor(resolvedColor);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTextStrokeColor(StyleResolverState& state, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);

    if (state.applyPropertyToRegularStyle())
        state.style()->setTextStrokeColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color()));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextStrokeColor(state.document().textLinkColors().colorFromPrimitiveValue(primitiveValue, state.style()->color(), true));
}


void StyleBuilderFunctions::applyInitialCSSPropertyCounterIncrement(StyleResolverState& state) { }

void StyleBuilderFunctions::applyInheritCSSPropertyCounterIncrement(StyleResolverState& state)
{
    CounterDirectiveMap& map = state.style()->accessCounterDirectives();
    CounterDirectiveMap& parentMap = state.parentStyle()->accessCounterDirectives();

    typedef CounterDirectiveMap::iterator Iterator;
    Iterator end = parentMap.end();
    for (Iterator it = parentMap.begin(); it != end; ++it) {
        CounterDirectives& directives = map.add(it->key, CounterDirectives()).storedValue->value;
        directives.inheritIncrement(it->value);
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyCounterIncrement(StyleResolverState& state, CSSValue* value)
{
    CounterDirectiveMap& map = state.style()->accessCounterDirectives();
    typedef CounterDirectiveMap::iterator Iterator;

    Iterator end = map.end();
    for (Iterator it = map.begin(); it != end; ++it)
        it->value.clearIncrement();

    if (!value->isValueList()) {
        ASSERT(value->isPrimitiveValue() && toCSSPrimitiveValue(value)->getValueID() == CSSValueNone);
        return;
    }

    CSSValueList* list = toCSSValueList(value);

    int length = list ? list->length() : 0;
    for (int i = 0; i < length; ++i) {
        CSSValue* currValue = list->itemWithoutBoundsCheck(i);
        if (!currValue->isPrimitiveValue())
            continue;

        Pair* pair = toCSSPrimitiveValue(currValue)->getPairValue();
        if (!pair || !pair->first() || !pair->second())
            continue;

        AtomicString identifier(pair->first()->getStringValue());
        int value = pair->second()->getIntValue();
        CounterDirectives& directives = map.add(identifier, CounterDirectives()).storedValue->value;
        directives.addIncrementValue(value);
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyCounterReset(StyleResolverState& state) { }

void StyleBuilderFunctions::applyInheritCSSPropertyCounterReset(StyleResolverState& state)
{
    CounterDirectiveMap& map = state.style()->accessCounterDirectives();
    CounterDirectiveMap& parentMap = state.parentStyle()->accessCounterDirectives();

    typedef CounterDirectiveMap::iterator Iterator;
    Iterator end = parentMap.end();
    for (Iterator it = parentMap.begin(); it != end; ++it) {
        CounterDirectives& directives = map.add(it->key, CounterDirectives()).storedValue->value;
        directives.inheritReset(it->value);
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyCounterReset(StyleResolverState& state, CSSValue* value)
{
    CounterDirectiveMap& map = state.style()->accessCounterDirectives();
    typedef CounterDirectiveMap::iterator Iterator;

    Iterator end = map.end();
    for (Iterator it = map.begin(); it != end; ++it)
        it->value.clearReset();

    if (!value->isValueList()) {
        ASSERT(value->isPrimitiveValue() && toCSSPrimitiveValue(value)->getValueID() == CSSValueNone);
        return;
    }

    CSSValueList* list = toCSSValueList(value);

    int length = list ? list->length() : 0;
    for (int i = 0; i < length; ++i) {
        CSSValue* currValue = list->itemWithoutBoundsCheck(i);
        if (!currValue->isPrimitiveValue())
            continue;

        Pair* pair = toCSSPrimitiveValue(currValue)->getPairValue();
        if (!pair || !pair->first() || !pair->second())
            continue;

        AtomicString identifier(pair->first()->getStringValue());
        int value = pair->second()->getIntValue();
        CounterDirectives& directives = map.add(identifier, CounterDirectives()).storedValue->value;
        directives.setResetValue(value);
    }
}


void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundAttachment(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    currChild->setAttachment(FillLayer::initialFillAttachment(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearAttachment();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundAttachment(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isAttachmentSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(BackgroundFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setAttachment(currParent->attachment());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearAttachment();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundAttachment(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(BackgroundFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillAttachment(CSSPropertyBackgroundAttachment, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillAttachment(CSSPropertyBackgroundAttachment, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearAttachment();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundBlendMode(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    currChild->setBlendMode(FillLayer::initialFillBlendMode(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearBlendMode();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundBlendMode(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isBlendModeSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(BackgroundFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setBlendMode(currParent->blendMode());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearBlendMode();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundBlendMode(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(BackgroundFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillBlendMode(CSSPropertyBackgroundBlendMode, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillBlendMode(CSSPropertyBackgroundBlendMode, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearBlendMode();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundClip(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    currChild->setClip(FillLayer::initialFillClip(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearClip();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundClip(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isClipSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(BackgroundFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setClip(currParent->clip());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearClip();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundClip(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(BackgroundFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillClip(CSSPropertyBackgroundClip, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillClip(CSSPropertyBackgroundClip, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearClip();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundImage(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    currChild->setImage(FillLayer::initialFillImage(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearImage();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundImage(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isImageSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(BackgroundFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setImage(currParent->image());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearImage();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundImage(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(BackgroundFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillImage(CSSPropertyBackgroundImage, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillImage(CSSPropertyBackgroundImage, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearImage();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundOrigin(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    currChild->setOrigin(FillLayer::initialFillOrigin(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearOrigin();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundOrigin(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isOriginSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(BackgroundFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setOrigin(currParent->origin());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearOrigin();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundOrigin(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(BackgroundFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillOrigin(CSSPropertyBackgroundOrigin, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillOrigin(CSSPropertyBackgroundOrigin, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearOrigin();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundPositionX(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    currChild->setXPosition(FillLayer::initialFillXPosition(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearXPosition();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundPositionX(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isXPositionSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(BackgroundFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setXPosition(currParent->xPosition());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearXPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundPositionX(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(BackgroundFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillXPosition(CSSPropertyBackgroundPositionX, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillXPosition(CSSPropertyBackgroundPositionX, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearXPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundPositionY(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    currChild->setYPosition(FillLayer::initialFillYPosition(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearYPosition();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundPositionY(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isYPositionSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(BackgroundFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setYPosition(currParent->yPosition());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearYPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundPositionY(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(BackgroundFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillYPosition(CSSPropertyBackgroundPositionY, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillYPosition(CSSPropertyBackgroundPositionY, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearYPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundRepeatX(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    currChild->setRepeatX(FillLayer::initialFillRepeatX(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearRepeatX();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundRepeatX(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isRepeatXSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(BackgroundFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setRepeatX(currParent->repeatX());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearRepeatX();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundRepeatX(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(BackgroundFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillRepeatX(CSSPropertyBackgroundRepeatX, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillRepeatX(CSSPropertyBackgroundRepeatX, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearRepeatX();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundRepeatY(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    currChild->setRepeatY(FillLayer::initialFillRepeatY(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearRepeatY();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundRepeatY(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isRepeatYSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(BackgroundFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setRepeatY(currParent->repeatY());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearRepeatY();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundRepeatY(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(BackgroundFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillRepeatY(CSSPropertyBackgroundRepeatY, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillRepeatY(CSSPropertyBackgroundRepeatY, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearRepeatY();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundSize(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    currChild->setSize(FillLayer::initialFillSize(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearSize();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundSize(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isSizeSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(BackgroundFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setSize(currParent->size());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearSize();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundSize(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(BackgroundFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillSize(CSSPropertyBackgroundSize, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillSize(CSSPropertyBackgroundSize, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearSize();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyMaskSourceType(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    currChild->setMaskSourceType(FillLayer::initialFillMaskSourceType(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearMaskSourceType();
}

void StyleBuilderFunctions::applyInheritCSSPropertyMaskSourceType(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->maskLayers();
    while (currParent && currParent->isMaskSourceTypeSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(MaskFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setMaskSourceType(currParent->maskSourceType());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearMaskSourceType();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyMaskSourceType(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(MaskFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillMaskSourceType(CSSPropertyMaskSourceType, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillMaskSourceType(CSSPropertyMaskSourceType, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearMaskSourceType();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBackgroundComposite(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    currChild->setComposite(FillLayer::initialFillComposite(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearComposite();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBackgroundComposite(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isCompositeSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(BackgroundFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setComposite(currParent->composite());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearComposite();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBackgroundComposite(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(BackgroundFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillComposite(CSSPropertyWebkitBackgroundComposite, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillComposite(CSSPropertyWebkitBackgroundComposite, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearComposite();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskClip(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    currChild->setClip(FillLayer::initialFillClip(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearClip();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskClip(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->maskLayers();
    while (currParent && currParent->isClipSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(MaskFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setClip(currParent->clip());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearClip();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskClip(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(MaskFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillClip(CSSPropertyWebkitMaskClip, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillClip(CSSPropertyWebkitMaskClip, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearClip();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskComposite(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    currChild->setComposite(FillLayer::initialFillComposite(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearComposite();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskComposite(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->maskLayers();
    while (currParent && currParent->isCompositeSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(MaskFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setComposite(currParent->composite());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearComposite();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskComposite(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(MaskFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillComposite(CSSPropertyWebkitMaskComposite, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillComposite(CSSPropertyWebkitMaskComposite, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearComposite();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskImage(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    currChild->setImage(FillLayer::initialFillImage(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearImage();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskImage(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->maskLayers();
    while (currParent && currParent->isImageSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(MaskFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setImage(currParent->image());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearImage();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskImage(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(MaskFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillImage(CSSPropertyWebkitMaskImage, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillImage(CSSPropertyWebkitMaskImage, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearImage();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskOrigin(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    currChild->setOrigin(FillLayer::initialFillOrigin(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearOrigin();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskOrigin(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->maskLayers();
    while (currParent && currParent->isOriginSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(MaskFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setOrigin(currParent->origin());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearOrigin();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskOrigin(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(MaskFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillOrigin(CSSPropertyWebkitMaskOrigin, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillOrigin(CSSPropertyWebkitMaskOrigin, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearOrigin();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskPositionX(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    currChild->setXPosition(FillLayer::initialFillXPosition(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearXPosition();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskPositionX(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->maskLayers();
    while (currParent && currParent->isXPositionSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(MaskFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setXPosition(currParent->xPosition());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearXPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskPositionX(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(MaskFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillXPosition(CSSPropertyWebkitMaskPositionX, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillXPosition(CSSPropertyWebkitMaskPositionX, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearXPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskPositionY(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    currChild->setYPosition(FillLayer::initialFillYPosition(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearYPosition();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskPositionY(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->maskLayers();
    while (currParent && currParent->isYPositionSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(MaskFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setYPosition(currParent->yPosition());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearYPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskPositionY(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(MaskFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillYPosition(CSSPropertyWebkitMaskPositionY, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillYPosition(CSSPropertyWebkitMaskPositionY, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearYPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskRepeatX(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    currChild->setRepeatX(FillLayer::initialFillRepeatX(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearRepeatX();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskRepeatX(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->maskLayers();
    while (currParent && currParent->isRepeatXSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(MaskFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setRepeatX(currParent->repeatX());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearRepeatX();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskRepeatX(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(MaskFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillRepeatX(CSSPropertyWebkitMaskRepeatX, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillRepeatX(CSSPropertyWebkitMaskRepeatX, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearRepeatX();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskRepeatY(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    currChild->setRepeatY(FillLayer::initialFillRepeatY(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearRepeatY();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskRepeatY(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->maskLayers();
    while (currParent && currParent->isRepeatYSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(MaskFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setRepeatY(currParent->repeatY());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearRepeatY();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskRepeatY(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(MaskFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillRepeatY(CSSPropertyWebkitMaskRepeatY, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillRepeatY(CSSPropertyWebkitMaskRepeatY, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearRepeatY();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskSize(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    currChild->setSize(FillLayer::initialFillSize(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearSize();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskSize(StyleResolverState& state)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = state.parentStyle()->maskLayers();
    while (currParent && currParent->isSizeSet()) {
        if (!currChild) {
            /* Need to make a new layer.*/
            currChild = new FillLayer(MaskFillLayer);
            prevChild->setNext(currChild);
        }
        currChild->setSize(currParent->size());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearSize();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskSize(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild) {
                /* Need to make a new layer to hold this value */
                currChild = new FillLayer(MaskFillLayer);
                prevChild->setNext(currChild);
            }
            state.styleMap().mapFillSize(CSSPropertyWebkitMaskSize, currChild, valueList->itemWithoutBoundsCheck(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        state.styleMap().mapFillSize(CSSPropertyWebkitMaskSize, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearSize();
        currChild = currChild->next();
    }
}


void StyleBuilderFunctions::applyInitialCSSPropertyGridTemplateColumns(StyleResolverState& state)
{
    state.style()->setGridTemplateColumns(RenderStyle::initialGridTemplateColumns());
    state.style()->setNamedGridColumnLines(RenderStyle::initialNamedGridColumnLines());
    state.style()->setOrderedNamedGridColumnLines(RenderStyle::initialOrderedNamedGridColumnLines());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridTemplateColumns(StyleResolverState& state)
{
    state.style()->setGridTemplateColumns(state.parentStyle()->gridTemplateColumns());
    state.style()->setNamedGridColumnLines(state.parentStyle()->namedGridColumnLines());
    state.style()->setOrderedNamedGridColumnLines(state.parentStyle()->orderedNamedGridColumnLines());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridTemplateColumns(StyleResolverState& state, CSSValue* value)
{
    Vector<GridTrackSize> trackSizes;
    NamedGridLinesMap namedGridLines;
    OrderedNamedGridLines orderedNamedGridLines;
    if (!StyleBuilderConverter::convertGridTrackList(value, trackSizes, namedGridLines, orderedNamedGridLines, state))
        return;
    const NamedGridAreaMap& namedGridAreas = state.style()->namedGridArea();
    if (!namedGridAreas.isEmpty())
        StyleBuilderConverter::createImplicitNamedGridLinesFromGridArea(namedGridAreas, namedGridLines, ForColumns);
    state.style()->setGridTemplateColumns(trackSizes);
    state.style()->setNamedGridColumnLines(namedGridLines);
    state.style()->setOrderedNamedGridColumnLines(orderedNamedGridLines);
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridTemplateRows(StyleResolverState& state)
{
    state.style()->setGridTemplateRows(RenderStyle::initialGridTemplateRows());
    state.style()->setNamedGridRowLines(RenderStyle::initialNamedGridRowLines());
    state.style()->setOrderedNamedGridRowLines(RenderStyle::initialOrderedNamedGridRowLines());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridTemplateRows(StyleResolverState& state)
{
    state.style()->setGridTemplateRows(state.parentStyle()->gridTemplateRows());
    state.style()->setNamedGridRowLines(state.parentStyle()->namedGridRowLines());
    state.style()->setOrderedNamedGridRowLines(state.parentStyle()->orderedNamedGridRowLines());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridTemplateRows(StyleResolverState& state, CSSValue* value)
{
    Vector<GridTrackSize> trackSizes;
    NamedGridLinesMap namedGridLines;
    OrderedNamedGridLines orderedNamedGridLines;
    if (!StyleBuilderConverter::convertGridTrackList(value, trackSizes, namedGridLines, orderedNamedGridLines, state))
        return;
    const NamedGridAreaMap& namedGridAreas = state.style()->namedGridArea();
    if (!namedGridAreas.isEmpty())
        StyleBuilderConverter::createImplicitNamedGridLinesFromGridArea(namedGridAreas, namedGridLines, ForRows);
    state.style()->setGridTemplateRows(trackSizes);
    state.style()->setNamedGridRowLines(namedGridLines);
    state.style()->setOrderedNamedGridRowLines(orderedNamedGridLines);
}


void StyleBuilderFunctions::applyValueCSSPropertyInternalMarqueeRepetition(StyleResolverState& state, CSSValue* value)
{
    if (!value->isPrimitiveValue())
        return;

    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (primitiveValue->getValueID() == CSSValueInfinite)
        state.style()->setMarqueeLoopCount(-1);
    else
        state.style()->setMarqueeLoopCount(primitiveValue->getValue<int>(CSSPrimitiveValue::CSS_NUMBER));
}


void StyleBuilderFunctions::applyValueCSSPropertyShapeOutside(StyleResolverState& state, CSSValue* value)
{
    if (value->isPrimitiveValue()) {
        CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
        if (primitiveValue->getValueID() == CSSValueNone)
            state.style()->setShapeOutside(nullptr);
    } else if (value->isImageValue() || value->isImageGeneratorValue() || value->isImageSetValue()) {
        state.style()->setShapeOutside(ShapeValue::createImageValue(state.styleImage(CSSPropertyShapeOutside, value)));
    } else if (value->isValueList()) {
        RefPtr<BasicShape> shape;
        CSSBoxType cssBox = BoxMissing;
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned i = 0; i < valueList->length(); ++i) {
            CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(valueList->itemWithoutBoundsCheck(i));
            if (primitiveValue->isShape())
                shape = basicShapeForValue(state, primitiveValue->getShapeValue());
            else if (primitiveValue->getValueID() == CSSValueContentBox
                || primitiveValue->getValueID() == CSSValueBorderBox
                || primitiveValue->getValueID() == CSSValuePaddingBox
                || primitiveValue->getValueID() == CSSValueMarginBox)
                cssBox = CSSBoxType(*primitiveValue);
            else
                return;
        }

        if (shape)
            state.style()->setShapeOutside(ShapeValue::createShapeValue(shape.release(), cssBox));
        else if (cssBox != BoxMissing)
            state.style()->setShapeOutside(ShapeValue::createBoxShapeValue(cssBox));
    }
}


void StyleBuilderFunctions::applyValueCSSPropertyJustifySelf(StyleResolverState& state, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (Pair* pairValue = primitiveValue->getPairValue()) {
        state.style()->setJustifySelf(*pairValue->first());
        state.style()->setJustifySelfOverflowAlignment(*pairValue->second());
    } else {
        state.style()->setJustifySelf(*primitiveValue);
        // FIXME: We should clear the overflow-alignment mode here and probably
        // also set it in the initial and inherit handlers
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyAlignItems(StyleResolverState& state, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (Pair* pairValue = primitiveValue->getPairValue()) {
        state.style()->setAlignItems(*pairValue->first());
        state.style()->setAlignItemsOverflowAlignment(*pairValue->second());
    } else {
        state.style()->setAlignItems(*primitiveValue);
        // FIXME: We should clear the overflow-alignment mode here and probably
        // also set it in the initial and inherit handlers
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyAlignSelf(StyleResolverState& state, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (Pair* pairValue = primitiveValue->getPairValue()) {
        state.style()->setAlignSelf(*pairValue->first());
        state.style()->setAlignSelfOverflowAlignment(*pairValue->second());
    } else {
        state.style()->setAlignSelf(*primitiveValue);
        // FIXME: We should clear the overflow-alignment mode here and probably
        // also set it in the initial and inherit handlers
    }
}


void StyleBuilderFunctions::applyInitialCSSPropertyFill(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setFillPaint(
        SVGRenderStyle::initialFillPaintType(),
        SVGRenderStyle::initialFillPaintColor(),
        SVGRenderStyle::initialFillPaintUri(),
        state.applyPropertyToRegularStyle(),
        state.applyPropertyToVisitedLinkStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFill(StyleResolverState& state)
{
    const SVGRenderStyle* svgParentStyle = state.parentStyle()->svgStyle();
    state.style()->accessSVGStyle()->setFillPaint(
        svgParentStyle->fillPaintType(),
        svgParentStyle->fillPaintColor(),
        svgParentStyle->fillPaintUri(),
        state.applyPropertyToRegularStyle(),
        state.applyPropertyToVisitedLinkStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyFill(StyleResolverState& state, CSSValue* value)
{
    if (value->isSVGPaint()) {
        SVGPaint* svgPaint = toSVGPaint(value);

        Color color;
        if (svgPaint->paintType() == SVGPaint::SVG_PAINTTYPE_CURRENTCOLOR
                || svgPaint->paintType() == SVGPaint::SVG_PAINTTYPE_URI_CURRENTCOLOR)
            color = state.style()->color();
        else
            color = svgPaint->color();

        state.style()->accessSVGStyle()->setFillPaint(svgPaint->paintType(),
            color,
            svgPaint->uri(),
            state.applyPropertyToRegularStyle(),
            state.applyPropertyToVisitedLinkStyle());
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyStroke(StyleResolverState& state)
{
    state.style()->accessSVGStyle()->setStrokePaint(
        SVGRenderStyle::initialStrokePaintType(),
        SVGRenderStyle::initialStrokePaintColor(),
        SVGRenderStyle::initialStrokePaintUri(),
        state.applyPropertyToRegularStyle(),
        state.applyPropertyToVisitedLinkStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStroke(StyleResolverState& state)
{
    const SVGRenderStyle* svgParentStyle = state.parentStyle()->svgStyle();
    state.style()->accessSVGStyle()->setStrokePaint(
        svgParentStyle->strokePaintType(),
        svgParentStyle->strokePaintColor(),
        svgParentStyle->strokePaintUri(),
        state.applyPropertyToRegularStyle(),
        state.applyPropertyToVisitedLinkStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyStroke(StyleResolverState& state, CSSValue* value)
{
    if (value->isSVGPaint()) {
        SVGPaint* svgPaint = toSVGPaint(value);

        Color color;
        if (svgPaint->paintType() == SVGPaint::SVG_PAINTTYPE_CURRENTCOLOR
                || svgPaint->paintType() == SVGPaint::SVG_PAINTTYPE_URI_CURRENTCOLOR)
            color = state.style()->color();
        else
            color = svgPaint->color();

        state.style()->accessSVGStyle()->setStrokePaint(svgPaint->paintType(),
            color,
            svgPaint->uri(),
            state.applyPropertyToRegularStyle(),
            state.applyPropertyToVisitedLinkStyle());
    }
}

} // namespace WebCore
