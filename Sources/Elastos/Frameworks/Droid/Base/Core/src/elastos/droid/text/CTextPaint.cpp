
#include "elastos/droid/text/CTextPaint.h"

namespace Elastos {
namespace Droid {
namespace Text {

ECode CTextPaint::constructor()
{
    return TextPaint::Init();
}

ECode CTextPaint::constructor(
    /* [in] */ Int32 flags)
{
    return TextPaint::Init(flags);
}

ECode CTextPaint::constructor(
    /* [in] */ IPaint* p)
{
    return TextPaint::Init(p);
}

PInterface CTextPaint::Probe(
    /* [in] */ REIID riid)
{
    if (riid == Elastos::Droid::Graphics::EIID_Paint) {
        return reinterpret_cast<PInterface>((Paint*)this);
    }
    return _CTextPaint::Probe(riid);
}

/**
 * Return the distance above (negative) the baseline (ascent) based on the
 * current typeface and text size.
 *
 * @return the distance above (negative) the baseline (ascent) based on the
 *         current typeface and text size.
 */
ECode CTextPaint::Ascent(
    /* [out] */ Float* distance)
{
    return TextPaint::Ascent(distance);
}

/**
 * Measure the text, stopping early if the measured width exceeds maxWidth.
 * Return the number of chars that were measured, and if measuredWidth is
 * not null, return in it the actual width measured.
 *
 * @param text  The text to measure
 * @param index The offset into text to begin measuring at
 * @param count The number of maximum number of entries to measure. If count
 *              is negative, then the characters before index are measured
 *              in reverse order. This allows for measuring the end of
 *              string.
 * @param maxWidth The maximum width to accumulate.
 * @param measuredWidth Optional. If not null, returns the actual width
 *                     measured.
 * @return The number of chars that were measured. Will always be <=
 *         abs(count).
 */
ECode CTextPaint::BreakText(
    /* [in] */ const ArrayOf<Char32>& text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Float maxWidth,
    /* [in] */ ArrayOf<Float>* measuredWidth,
    /* [out] */ Int32* number)
{
    return TextPaint::BreakText(text, index, count, maxWidth, measuredWidth, number);
}

/**
 * Measure the text, stopping early if the measured width exceeds maxWidth.
 * Return the number of chars that were measured, and if measuredWidth is
 * not null, return in it the actual width measured.
 *
 * @param text  The text to measure
 * @param start The offset into text to begin measuring at
 * @param end   The end of the text slice to measure.
 * @param measureForwards If true, measure forwards, starting at start.
 *                        Otherwise, measure backwards, starting with end.
 * @param maxWidth The maximum width to accumulate.
 * @param measuredWidth Optional. If not null, returns the actual width
 *                     measured.
 * @return The number of chars that were measured. Will always be <=
 *         abs(end - start).
 */
ECode CTextPaint::BreakText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Boolean measureForwards,
    /* [in] */ Float maxWidth,
    /* [in] */ ArrayOf<Float>* measuredWidth,
    /* [out] */ Int32* number)
{
    return TextPaint::BreakText(text, start, end, measureForwards,
        maxWidth, measuredWidth, number);
}

/**
 * Measure the text, stopping early if the measured width exceeds maxWidth.
 * Return the number of chars that were measured, and if measuredWidth is
 * not null, return in it the actual width measured.
 *
 * @param text  The text to measure
 * @param measureForwards If true, measure forwards, starting with the
 *                        first character in the string. Otherwise,
 *                        measure backwards, starting with the
 *                        last character in the string.
 * @param maxWidth The maximum width to accumulate.
 * @param measuredWidth Optional. If not null, returns the actual width
 *                     measured.
 * @return The number of chars that were measured. Will always be <=
 *         abs(count).
 */
ECode CTextPaint::BreakText(
    /* [in] */ const String& text,
    /* [in] */ Boolean measureForwards,
    /* [in] */ Float maxWidth,
    /* [in] */ ArrayOf<Float>* measuredWidth,
    /* [out] */ Int32* number)
{
    return TextPaint::BreakText(text, measureForwards, maxWidth,
        measuredWidth, number);
}

/**
 * Temporary API to clear the shadow layer.
 */
ECode CTextPaint::ClearShadowLayer()
{
    return TextPaint::ClearShadowLayer();
}

/**
 * Return the distance below (positive) the baseline (descent) based on the
 * current typeface and text size.
 *
 * @return the distance below (positive) the baseline (descent) based on
 *         the current typeface and text size.
 */
ECode CTextPaint::Descent(
    /* [out] */ Float* distance)
{
    return TextPaint::Descent(distance);
}

/**
 * Helper to getColor() that just returns the color's alpha value. This is
 * the same as calling getColor() >>> 24. It always returns a value between
 * 0 (completely transparent) and 255 (completely opaque).
 *
 * @return the alpha component of the paint's color.
 */
ECode CTextPaint::GetAlpha(
    /* [out] */ Int32* alpha)
{
    return TextPaint::GetAlpha(alpha);
}

/**
 * Return the paint's color. Note that the color is a 32bit value
 * containing alpha as well as r,g,b. This 32bit value is not premultiplied,
 * meaning that its alpha can be any value, regardless of the values of
 * r,g,b. See the Color class for more details.
 *
 * @return the paint's color (and alpha).
 */
ECode CTextPaint::GetColor(
    /* [out] */ Int32* color)
{
    return TextPaint::GetColor(color);
}

/**
 * Get the paint's colorfilter (maybe be null).
 *
 * @return the paint's colorfilter (maybe be null)
 */
ECode CTextPaint::GetColorFilter(
    /* [out] */ IColorFilter** filter)
{
    return TextPaint::GetColorFilter(filter);
}

/**
 * Applies any/all effects (patheffect, stroking) to src, returning the
 * result in dst. The result is that drawing src with this paint will be
 * the same as drawing dst with a default paint (at least from the
 * geometric perspective).
 *
 * @param src input path
 * @param dst output path (may be the same as src)
 * @return    true if the path should be filled, or false if it should be
 *                 drawn with a hairline (width == 0)
 */
ECode CTextPaint::GetFillPath(
    /* [in] */ IPath* src,
    /* [in] */ IPath* dst,
    /* [out] */ Boolean* isShouldFilled)
{
    return TextPaint::GetFillPath(src, dst, isShouldFilled);
}

/**
 * Return the paint's flags. Use the Flag enum to test flag values.
 *
 * @return the paint's flags (see enums ending in _Flag for bit masks)
 */
ECode CTextPaint::GetFlags(
    /* [out] */ Int32* flags)
{
    return TextPaint::GetFlags(flags);
}

/**
 * Return the font's recommended interline spacing, given the Paint's
 * settings for typeface, textSize, etc. If metrics is not null, return the
 * fontmetric values in it.
 *
 * @param metrics If this object is not null, its fields are filled with
 *                the appropriate values given the paint's text attributes.
 * @return the font's recommended interline spacing.
 */
ECode CTextPaint::GetFontMetrics(
    /* [in] */ IPaintFontMetrics* metrics,
    /* [out] */ Float* spacing)
{/*
    VALIDATE_NOT_NULL(spacing);
    *spacing = Paint::GetFontMetrics(metrics);
    return NOERROR;*/
    return TextPaint::GetFontMetrics(metrics, spacing);
}

/**
 * Allocates a new FontMetrics object, and then calls getFontMetrics(fm)
 * with it, returning the object.
 */
ECode CTextPaint::GetFontMetrics(
    /* [out] */ IPaintFontMetrics** metrics)
{
    return GetFontMetrics(metrics);
}

/**
 * Return the font's interline spacing, given the Paint's settings for
 * typeface, textSize, etc. If metrics is not null, return the fontmetric
 * values in it. Note: all values have been converted to integers from
 * floats, in such a way has to make the answers useful for both spacing
 * and clipping. If you want more control over the rounding, call
 * getFontMetrics().
 *
 * @return the font's interline spacing.
 */
ECode CTextPaint::GetFontMetricsInt(
    /* [in] */ IPaintFontMetricsInt* fmi,
    /* [out] */ Int32* spacing)
{
    return TextPaint::GetFontMetricsInt(fmi, spacing);
}

ECode CTextPaint::GetFontMetricsInt(
    /* [out] */ IPaintFontMetricsInt** fmi)
{
    return TextPaint::GetFontMetricsInt(fmi);
}

/**
 * Return the recommend line spacing based on the current typeface and
 * text size.
 *
 * @return  recommend line spacing based on the current typeface and
 *          text size.
 */
ECode CTextPaint::GetFontSpacing(
    /* [out] */ Float* spacing)
{
    return TextPaint::GetFontSpacing(spacing);
}

/**
 * Return the paint's hinting mode.  Returns either
 * {@link #HINTING_OFF} or {@link #HINTING_ON}.
 */
ECode CTextPaint::GetHinting(
    /* [out] */ Int32* result)
{
    return TextPaint::GetHinting(result);
}

/**
 * Get the paint's maskfilter object.
 *
 * @return the paint's maskfilter (or null)
 */
ECode CTextPaint::GetMaskFilter(
    /* [out] */ IMaskFilter** maskfilter)
{
    return TextPaint::GetMaskFilter(maskfilter);
}

/**
 * Get the paint's patheffect object.
 *
 * @return the paint's patheffect (or null)
 */
ECode CTextPaint::GetPathEffect(
    /* [out] */ IPathEffect** effect)
{
    return TextPaint::GetPathEffect(effect);
}

/**
 * Get the paint's rasterizer (or null).
 * <p />
 * The raster controls/modifies how paths/text are turned into alpha masks.
 *
 * @return         the paint's rasterizer (or null)
 */
ECode CTextPaint::GetRasterizer(
    /* [out] */ IRasterizer** rasterizer)
{
    return TextPaint::GetRasterizer(rasterizer);
}

/**
 * Get the paint's shader object.
 *
 * @return the paint's shader (or null)
 */
ECode CTextPaint::GetShader(
    /* [out] */ IShader** shader)
{
    return TextPaint::GetShader(shader);
}

/**
 * Return the paint's Cap, controlling how the start and end of stroked
 * lines and paths are treated.
 *
 * @return the line cap style for the paint, used whenever the paint's
 *         style is Stroke or StrokeAndFill.
 */
ECode CTextPaint::GetStrokeCap(
    /* [out] */ PaintCap* cap)
{
    return TextPaint::GetStrokeCap(cap);
}

/**
 * Return the paint's stroke join type.
 *
 * @return the paint's Join.
 */
ECode CTextPaint::GetStrokeJoin(
    /* [out] */ PaintJoin* join)
{
    return TextPaint::GetStrokeJoin(join);
}

/**
 * Return the paint's stroke miter value. Used to control the behavior
 * of miter joins when the joins angle is sharp.
 *
 * @return the paint's miter limit, used whenever the paint's style is
 *         Stroke or StrokeAndFill.
 */
ECode CTextPaint::GetStrokeMiter(
    /* [out] */ Float* strokeMiter)
{
    return TextPaint::GetStrokeMiter(strokeMiter);
}

/**
 * Return the width for stroking.
 * <p />
 * A value of 0 strokes in hairline mode.
 * Hairlines always draws a single pixel independent of the canva's matrix.
 *
 * @return the paint's stroke width, used whenever the paint's style is
 *         Stroke or StrokeAndFill.
 */
ECode CTextPaint::GetStrokeWidth(
    /* [out] */ Float* width)
{
    return TextPaint::GetStrokeWidth(width);
}

/**
 * Return the paint's style, used for controlling how primitives'
 * geometries are interpreted (except for drawBitmap, which always assumes
 * FILL_STYLE).
 *
 * @return the paint's style setting (Fill, Stroke, StrokeAndFill)
 */
ECode CTextPaint::GetStyle(
    /* [out] */ PaintStyle* style)
{
    return TextPaint::GetStyle(style);
}

/**
 * Return the paint's Align value for drawing text. This controls how the
 * text is positioned relative to its origin. LEFT align means that all of
 * the text will be drawn to the right of its origin (i.e. the origin
 * specifieds the LEFT edge of the text) and so on.
 *
 * @return the paint's Align value for drawing text.
 */
ECode CTextPaint::GetTextAlign(
    /* [out] */ PaintAlign* align)
{
    return TextPaint::GetTextAlign(align);
}

/**
 * Get the text Locale.
 *
 * @return the paint's Locale used for drawing text, never null.
 */
ECode CTextPaint::GetTextLocale(
    /* [out] */ ILocale** result)
{
    return TextPaint::GetTextLocale(result);
}

/**
 * Return the path (outline) for the specified text.
 * Note: just like Canvas.drawText, this will respect the Align setting
 * in the paint.
 *
 * @param text  The text to retrieve the path from
 * @param start The first character in the text
 * @param end   1 past the last charcter in the text
 * @param x     The x coordinate of the text's origin
 * @param y     The y coordinate of the text's origin
 * @param path  The path to receive the data describing the text. Must
 *              be allocated by the caller.
 */
ECode CTextPaint::GetTextPath(
    /* [in] */ const ArrayOf<Char32>& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPath* path)
{
    return TextPaint::GetTextPath(text, start, end, x, y, path);
}

/**
 * Return the path (outline) for the specified text.
 * Note: just like Canvas.drawText, this will respect the Align setting in
 * the paint.
 *
 * @param text     The text to retrieve the path from
 * @param index    The index of the first character in text
 * @param count    The number of characterss starting with index
 * @param x        The x coordinate of the text's origin
 * @param y        The y coordinate of the text's origin
 * @param path     The path to receive the data describing the text. Must
 *                 be allocated by the caller.
 */
ECode CTextPaint::GetTextPath(
    /* [in] */ const String& text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPath* path)
{
    return TextPaint::GetTextPath(text, index, count, x, y, path);
}

/**
 * Return the paint's horizontal scale factor for text. The default value
 * is 1.0.
 *
 * @return the paint's scale factor in X for drawing/measuring text
 */
ECode CTextPaint::GetTextScaleX(
    /* [out] */ Float* scaleX)
{
    return TextPaint::GetTextScaleX(scaleX);
}

/**
 * Return the paint's text size.
 *
 * @return the paint's text size.
 */
ECode CTextPaint::GetTextSize(
    /* [out] */ Float* size)
{
    return TextPaint::GetTextSize(size);
}

/**
 * Return the paint's horizontal skew factor for text. The default value
 * is 0.
 *
 * @return         the paint's skew factor in X for drawing text.
 */
ECode CTextPaint::GetTextSkewX(
    /* [out] */ Float* skewX)
{
    return TextPaint::GetTextSkewX(skewX);
}

/**
 * Get the paint's typeface object.
 * <p />
 * The typeface object identifies which font to use when drawing or
 * measuring text.
 *
 * @return the paint's typeface (or null)
 */
ECode CTextPaint::GetTypeface(
    /* [out] */ ITypeface** typeface)
{
    return TextPaint::GetTypeface(typeface);
}

/**
 * Get the paint's xfermode object.
 *
 * @return the paint's xfermode (or null)
 */
ECode CTextPaint::GetXfermode(
    /* [out] */ IXfermode** xfermode)
{
    return TextPaint::GetXfermode(xfermode);
}

/**
 * Helper for getFlags(), returning true if ANTI_ALIAS_FLAG bit is set
 * AntiAliasing smooths out the edges of what is being drawn, but is has
 * no impact on the interior of the shape. See setDither() and
 * setFilterBitmap() to affect how colors are treated.
 *
 * @return true if the antialias bit is set in the paint's flags.
 */
ECode CTextPaint::IsAntiAlias(
    /* [out] */ Boolean* isAntiAlias)
{
    return TextPaint::IsAntiAlias(isAntiAlias);
}

/**
 * Helper for getFlags(), returning true if DITHER_FLAG bit is set
 * Dithering affects how colors that are higher precision than the device
 * are down-sampled. No dithering is generally faster, but higher precision
 * colors are just truncated down (e.g. 8888 -> 565). Dithering tries to
 * distribute the error inherent in this process, to reduce the visual
 * artifacts.
 *
 * @return true if the dithering bit is set in the paint's flags.
 */
ECode CTextPaint::IsDither(
    /* [out] */ Boolean* isDither)
{
    return TextPaint::IsDither(isDither);
}

/**
 * Helper for getFlags(), returning true if FAKE_BOLD_TEXT_FLAG bit is set
 *
 * @return true if the fakeBoldText bit is set in the paint's flags.
 */
ECode CTextPaint::IsFakeBoldText(
    /* [out] */ Boolean* isFakeBoldText)
{
    return TextPaint::IsFakeBoldText(isFakeBoldText);
}

/**
 * Whether or not the bitmap filter is activated.
 * Filtering affects the sampling of bitmaps when they are transformed.
 * Filtering does not affect how the colors in the bitmap are converted into
 * device pixels. That is dependent on dithering and xfermodes.
 *
 * @see #setFilterBitmap(boolean) setFilterBitmap()
 */
ECode CTextPaint::IsFilterBitmap(
    /* [out] */ Boolean* isFilterBitmap)
{
    return TextPaint::IsFilterBitmap(isFilterBitmap);
}

/**
 * Helper for getFlags(), returning true if LINEAR_TEXT_FLAG bit is set
 *
 * @return true if the lineartext bit is set in the paint's flags
 */
ECode CTextPaint::IsLinearText(
    /* [out] */ Boolean* isLinearText)
{
    return TextPaint::IsLinearText(isLinearText);
}

/**
 * Helper for getFlags(), returning true if STRIKE_THRU_TEXT_FLAG bit is set
 *
 * @return true if the strikeThruText bit is set in the paint's flags.
 */
ECode CTextPaint::IsStrikeThruText(
    /* [out] */ Boolean* IsStrikeThruText)
{
    return TextPaint::IsStrikeThruText(IsStrikeThruText);
}

/**
 * Helper for getFlags(), returning true if SUBPIXEL_TEXT_FLAG bit is set
 *
 * @return true if the subpixel bit is set in the paint's flags
 */
ECode CTextPaint::IsSubpixelText(
    /* [out] */ Boolean* isSubpixelText)
{
    return TextPaint::IsSubpixelText(isSubpixelText);
}

/**
 * Helper for getFlags(), returning true if UNDERLINE_TEXT_FLAG bit is set
 *
 * @return true if the underlineText bit is set in the paint's flags.
 */
ECode CTextPaint::IsUnderlineText(
    /* [out] */ Boolean* isUnderlineText)
{
    return TextPaint::IsUnderlineText(isUnderlineText);
}

/** Restores the paint to its default settings. */
ECode CTextPaint::Reset()
{
    return TextPaint::Reset();
}

/**
 * Copy the fields from src into this paint. This is equivalent to calling
 * get() on all of the src fields, and calling the corresponding set()
 * methods on this.
 */
ECode CTextPaint::Set(
    /* [in] */ IPaint* src)
{
    VALIDATE_NOT_NULL(src)
    return TextPaint::Set(ITextPaint::Probe(src));
}

/**
 * Helper to setColor(), that takes a,r,g,b and constructs the color int
 *
 * @param a The new alpha component (0..255) of the paint's color.
 * @param r The new red component (0..255) of the paint's color.
 * @param g The new green component (0..255) of the paint's color.
 * @param b The new blue component (0..255) of the paint's color.
 */
ECode CTextPaint::SetARGB(
    /* [in] */ Int32 a,
    /* [in] */ Int32 r,
    /* [in] */ Int32 g,
    /* [in] */ Int32 b)
{
    return TextPaint::SetARGB(a, r, g, b);
}

/**
 * Helper to setColor(), that only assigns the color's alpha value,
 * leaving its r,g,b values unchanged. Results are undefined if the alpha
 * value is outside of the range [0..255]
 *
 * @param a set the alpha component [0..255] of the paint's color.
 */
ECode CTextPaint::SetAlpha(
    /* [in] */ Int32 a)
{
    return TextPaint::SetAlpha(a);
}

/**
 * Helper for setFlags(), setting or clearing the ANTI_ALIAS_FLAG bit
 * AntiAliasing smooths out the edges of what is being drawn, but is has
 * no impact on the interior of the shape. See setDither() and
 * setFilterBitmap() to affect how colors are treated.
 *
 * @param aa true to set the antialias bit in the flags, false to clear it
 */
ECode CTextPaint::SetAntiAlias(
    /* [in] */ Boolean aa)
{
    return TextPaint::SetAntiAlias(aa);
}

/**
 * Set the paint's color. Note that the color is an int containing alpha
 * as well as r,g,b. This 32bit value is not premultiplied, meaning that
 * its alpha can be any value, regardless of the values of r,g,b.
 * See the Color class for more details.
 *
 * @param color The new color (including alpha) to set in the paint.
 */
ECode CTextPaint::SetColor(
    /* [in] */ Int32 color)
{
    return TextPaint::SetColor(color);
}

/**
 * Set or clear the paint's colorfilter, returning the parameter.
 *
 * @param filter May be null. The new filter to be installed in the paint
 * @return       filter
 */
ECode CTextPaint::SetColorFilter(
    /* [in] */ IColorFilter* filter)
{
    return TextPaint::SetColorFilter(filter);
}

/** @hide */
ECode CTextPaint::SetCompatibilityScaling(
    /* [in] */ Float factor)
{
    return TextPaint::SetCompatibilityScaling(factor);
}

/**
 * re for setFlags(), setting or clearing the DITHER_FLAG bit
 * Dithering affects how colors that are higher precision than the device
 * are down-sampled. No dithering is generally faster, but higher precision
 * colors are just truncated down (e.g. 8888 -> 565). Dithering tries to
 * distribute the error inherent in this process, to reduce the visual
 * artifacts.
 *
 * @param dither true to set the dithering bit in flags, false to clear it
 */
ECode CTextPaint::SetDither(
    /* [in] */ Boolean dither)
{
    return TextPaint::SetDither(dither);
}

/**
 * Helper for setFlags(), setting or clearing the STRIKE_THRU_TEXT_FLAG bit
 *
 * @param fakeBoldText true to set the fakeBoldText bit in the paint's
 *                     flags, false to clear it.
 */
ECode CTextPaint::SetFakeBoldText(
    /* [in] */ Boolean fakeBoldText)
{
    return TextPaint::SetFakeBoldText(fakeBoldText);
}

/**
 * Helper for setFlags(), setting or clearing the FILTER_BITMAP_FLAG bit.
 * Filtering affects the sampling of bitmaps when they are transformed.
 * Filtering does not affect how the colors in the bitmap are converted into
 * device pixels. That is dependent on dithering and xfermodes.
 *
 * @param filter true to set the FILTER_BITMAP_FLAG bit in the paint's
 *               flags, false to clear it.
 */
ECode CTextPaint::SetFilterBitmap(
    /* [in] */ Boolean filter)
{
    return TextPaint::SetFilterBitmap(filter);
}

/**
 * Set the paint's flags. Use the Flag enum to specific flag values.
 *
 * @param flags The new flag bits for the paint
 */
ECode CTextPaint::SetFlags(
    /* [in] */ Int32 flags)
{
    return TextPaint::SetFlags(flags);
}

/**
 * Set the paint's hinting mode.  May be either
 * {@link #HINTING_OFF} or {@link #HINTING_ON}.
 */
ECode CTextPaint::SetHinting(
    /* [in] */ Int32 mode)
{
    return TextPaint::SetHinting(mode);
}

/**
 * Helper for setFlags(), setting or clearing the LINEAR_TEXT_FLAG bit
 *
 * @param linearText true to set the linearText bit in the paint's flags,
 *                   false to clear it.
 */
ECode CTextPaint::SetLinearText(
    /* [in] */ Boolean linearText)
{
    return TextPaint::SetLinearText(linearText);
}

/**
 * Set or clear the maskfilter object.
 * <p />
 * Pass null to clear any previous maskfilter.
 * As a convenience, the parameter passed is also returned.
 *
 * @param maskfilter May be null. The maskfilter to be installed in the
 *                   paint
 * @return           maskfilter
 */
ECode CTextPaint::SetMaskFilter(
    /* [in] */ IMaskFilter* maskfilter)
{
    return TextPaint::SetMaskFilter(maskfilter);
}

/**
 * Set or clear the patheffect object.
 * <p />
 * Pass null to clear any previous patheffect.
 * As a convenience, the parameter passed is also returned.
 *
 * @param effect May be null. The patheffect to be installed in the paint
 * @return       effect
 */
ECode CTextPaint::SetPathEffect(
    /* [in] */ IPathEffect* effect)
{
    return TextPaint::SetPathEffect(effect);
}

/**
 * Set or clear the rasterizer object.
 * <p />
 * Pass null to clear any previous rasterizer.
 * As a convenience, the parameter passed is also returned.
 *
 * @param rasterizer May be null. The new rasterizer to be installed in
 *                   the paint.
 * @return           rasterizer
 */
ECode CTextPaint::SetRasterizer(
    /* [in] */ IRasterizer* rasterizer)
{
    return TextPaint::SetRasterizer(rasterizer);
}

/**
 * Set or clear the shader object.
 * <p />
 * Pass null to clear any previous shader.
 * As a convenience, the parameter passed is also returned.
 *
 * @param shader May be null. the new shader to be installed in the paint
 * @return       shader
 */
ECode CTextPaint::SetShader(
    /* [in] */ IShader* shader)
{
    return TextPaint::SetShader(shader);
}

/**
 * Temporary API to expose layer drawing. This draws a shadow layer below
 * the main layer, with the specified offset and color, and blur radius.
 * If radius is 0, then the shadow layer is removed.
 */
ECode CTextPaint::SetShadowLayer(
    /* [in] */ Float radius,
    /* [in] */ Float rdx,
    /* [in] */ Float rdy,
    /* [in] */ Int32 color)
{
    return TextPaint::SetShadowLayer(radius, rdx, rdy, color);
}

/**
 * re for setFlags(), setting or clearing the STRIKE_THRU_TEXT_FLAG bit
 *
 * @param strikeThruText true to set the strikeThruText bit in the paint's
 *                       flags, false to clear it.
 */
ECode CTextPaint::SetStrikeThruText(
    /* [in] */ Boolean strikeThruText)
{
    return TextPaint::SetStrikeThruText(strikeThruText);
}

/**
 * re the paint's Cap.
 *
 * @param cap set the paint's line cap style, used whenever the paint's
 *            style is Stroke or StrokeAndFill.
 */
ECode CTextPaint::SetStrokeCap(
    /* [in] */ PaintCap cap)
{
    return TextPaint::SetStrokeCap(cap);
}

/**
 * Set the paint's Join.
 *
 * @param join set the paint's Join, used whenever the paint's style is
 *             Stroke or StrokeAndFill.
 */
ECode CTextPaint::SetStrokeJoin(
    /* [in] */ PaintJoin join)
{
    return TextPaint::SetStrokeJoin(join);
}

/**
 * Set the paint's stroke miter value. This is used to control the behavior
 * of miter joins when the joins angle is sharp. This value must be >= 0.
 *
 * @param miter set the miter limit on the paint, used whenever the paint's
 *              style is Stroke or StrokeAndFill.
 */
ECode CTextPaint::SetStrokeMiter(
    /* [in] */ Float miter)
{
    return TextPaint::SetStrokeMiter(miter);
}

/**
 * Set the width for stroking.
 * Pass 0 to stroke in hairline mode.
 * Hairlines always draws a single pixel independent of the canva's matrix.
 *
 * @param width set the paint's stroke width, used whenever the paint's
 *              style is Stroke or StrokeAndFill.
 */
ECode CTextPaint::SetStrokeWidth(
    /* [in] */ Float width)
{
    return TextPaint::SetStrokeWidth(width);
}

/**
 * Set the paint's style, used for controlling how primitives'
 * geometries are interpreted (except for drawBitmap, which always assumes
 * Fill).
 *
 * @param style The new style to set in the paint
 */
ECode CTextPaint::SetStyle(
    /* [in] */ PaintStyle style)
{
    return TextPaint::SetStyle(style);
}

/**
 * Helper for setFlags(), setting or clearing the SUBPIXEL_TEXT_FLAG bit
 *
 * @param subpixelText true to set the subpixelText bit in the paint's
 *                     flags, false to clear it.
 */
ECode CTextPaint::SetSubpixelText(
    /* [in] */ Boolean subpixelText)
{
    return TextPaint::SetSubpixelText(subpixelText);
}

/**
 * Set the paint's text alignment. This controls how the
 * text is positioned relative to its origin. LEFT align means that all of
 * the text will be drawn to the right of its origin (i.e. the origin
 * specifieds the LEFT edge of the text) and so on.
 *
 * @param align set the paint's Align value for drawing text.
 */
ECode CTextPaint::SetTextAlign(
    /* [in] */ PaintAlign align)
{
    return TextPaint::SetTextAlign(align);
}

/**
 * Set the text locale.
 *
 * The text locale affects how the text is drawn for some languages.
 *
 * For example, if the locale is {@link Locale#CHINESE} or {@link Locale#CHINA},
 * then the text renderer will prefer to draw text using a Chinese font. Likewise,
 * if the locale is {@link Locale#JAPANESE} or {@link Locale#JAPAN}, then the text
 * renderer will prefer to draw text using a Japanese font.
 *
 * This distinction is important because Chinese and Japanese text both use many
 * of the same Unicode code points but their appearance is subtly different for
 * each language.
 *
 * By default, the text locale is initialized to the system locale (as returned
 * by {@link Locale#getDefault}). This assumes that the text to be rendered will
 * most likely be in the user's preferred language.
 *
 * If the actual language of the text is known, then it can be provided to the
 * text renderer using this method. The text renderer may attempt to guess the
 * language script based on the contents of the text to be drawn independent of
 * the text locale here. Specifying the text locale just helps it do a better
 * job in certain ambiguous cases
 *
 * @param locale the paint's locale value for drawing text, must not be null.
 */
ECode CTextPaint::SetTextLocale(
    /* [in] */ ILocale* locale)
{
    return TextPaint::SetTextLocale(locale);
}

/**
 * Set the paint's horizontal scale factor for text. The default value
 * is 1.0. Values > 1.0 will stretch the text wider. Values < 1.0 will
 * stretch the text narrower.
 *
 * @param scaleX set the paint's scale in X for drawing/measuring text.
 */
ECode CTextPaint::SetTextScaleX(
    /* [in] */ Float scaleX)
{
    return TextPaint::SetTextScaleX(scaleX);
}

/**
 * Set the paint's text size. This value must be > 0
 *
 * @param textSize set the paint's text size.
 */
ECode CTextPaint::SetTextSize(
    /* [in] */ Float textSize)
{
    return TextPaint::SetTextSize(textSize);
}

/**
 * Set the paint's horizontal skew factor for text. The default value
 * is 0. For approximating oblique text, use values around -0.25.
 *
 * @param skewX set the paint's skew factor in X for drawing text.
 */
ECode CTextPaint::SetTextSkewX(
    /* [in] */ Float skewX)
{
    return TextPaint::SetTextSkewX(skewX);
}

/**
 * Set or clear the typeface object.
 * <p />
 * Pass null to clear any previous typeface.
 * As a convenience, the parameter passed is also returned.
 *
 * @param typeface May be null. The typeface to be installed in the paint
 * @return         typeface
 */
ECode CTextPaint::SetTypeface(
    /* [in] */ ITypeface* typeface)
{
    return TextPaint::SetTypeface(typeface);
}

/**
 * Helper for setFlags(), setting or clearing the UNDERLINE_TEXT_FLAG bit
 *
 * @param underlineText true to set the underlineText bit in the paint's
 *                      flags, false to clear it.
 */
ECode CTextPaint::SetUnderlineText(
    /* [in] */ Boolean underlineText)
{
    return TextPaint::SetUnderlineText(underlineText);
}

/**
 * Set or clear the xfermode object.
 * <p />
 * Pass null to clear any previous xfermode.
 * As a convenience, the parameter passed is also returned.
 *
 * @param xfermode May be null. The xfermode to be installed in the paint
 * @return         xfermode
 */
ECode CTextPaint::SetXfermode(
    /* [in] */ IXfermode* xfermode)
{
    return TextPaint::SetXfermode(xfermode);
}

/********************************TextPaint*****************************/

// Special value 0 means no background paint
ECode CTextPaint::SetBgColor(
    /* [in] */ Int32 bgColor)
{
    return TextPaint::SetBgColor(bgColor);
}

ECode CTextPaint::GetBgColor(
    /* [out] */ Int32* bgColor)
{
    return TextPaint::GetBgColor(bgColor);
}

ECode CTextPaint::SetBaselineShift(
    /* [in] */ Int32 baselineShift)
{
    return TextPaint::SetBaselineShift(baselineShift);
}

ECode CTextPaint::GetBaselineShift(
    /* [out] */ Int32* baselineShift)
{
    return TextPaint::GetBaselineShift(baselineShift);
}

ECode CTextPaint::SetLinkColor(
    /* [in] */ Int32 linkColor)
{
    return TextPaint::SetLinkColor(linkColor);
}

ECode CTextPaint::GetLinkColor(
    /* [out] */ Int32* linkColor)
{
    return TextPaint::GetLinkColor(linkColor);
}

ECode CTextPaint::SetDrawableState(
    /* [in] */ ArrayOf<Int32>* drawableState)
{
    return TextPaint::SetDrawableState(drawableState);
}

ECode CTextPaint::GetDrawableState(
    /* [out, callee] */ ArrayOf<Int32>** drawableState)
{
    return TextPaint::GetDrawableState(drawableState);
}

ECode CTextPaint::SetDensity(
    /* [in] */ Float density)
{
    return TextPaint::SetDensity(density);
}

ECode CTextPaint::GetDensity(
    /* [out] */ Float* density)
{
    return TextPaint::GetDensity(density);
}

/**
 * Special value 0 means no custom underline
 * @hide
 */
ECode CTextPaint::SetUnderlineColor(
    /* [in] */ Int32 underlineColor)
{
    return TextPaint::SetUnderlineColor(underlineColor);
}

ECode CTextPaint::GetUnderlineColor(
    /* [out] */ Int32* underlineColor)
{
    return TextPaint::GetUnderlineColor(underlineColor);
}

/**
 * Defined as a multiplier of the default underline thickness. Use 1.0f for default thickness.
 * @hide
 */
ECode CTextPaint::SetUnderlineThickness(
    /* [in] */ Float underlineThickness)
{
    return TextPaint::SetUnderlineThickness(underlineThickness);
}

ECode CTextPaint::GetUnderlineThickness(
    /* [out] */ Float* underlineThickness)
{
    return TextPaint::GetUnderlineThickness(underlineThickness);
}

/**
 * Copy the fields from tp into this TextPaint, including the
 * fields inherited from Paint.
 */
ECode CTextPaint::Set(
    /* [in] */ ITextPaint* tp)
{
    return TextPaint::Set(tp);
}

/**
 * Defines a custom underline for this Paint.
 * @param color underline solid color
 * @param thickness underline thickness
 * @hide
 */
ECode CTextPaint::SetUnderlineText(
    /* [in] */ Int32 color,
    /* [in] */ Float thickness)
{
    return TextPaint::SetUnderlineText(color, thickness);
}

/******************************IPaint extra*********************************/

/**
 * Return the bidi flags on the paint.
 *
 * @return the bidi flags on the paint
 * @hide
 */
ECode CTextPaint::GetBidiFlags(
    /* [out] */ Int32* flags)
{
    return TextPaint::GetBidiFlags(flags);
}

/**
 * Set the bidi flags on the paint.
 * @hide
 */
ECode CTextPaint::SetBidiFlags(
    /* [in] */ Int32 flags)
{
    return TextPaint::SetBidiFlags(flags);
}

/**
 * Return in bounds (allocated by the caller) the smallest rectangle that
 * encloses all of the characters, with an implied origin at (0,0).
 *
 * @param text  String to measure and return its bounds
 * @param start Index of the first char in the string to measure
 * @param end   1 past the last char in the string measure
 * @param bounds Returns the unioned bounds of all the text. Must be
 *               allocated by the caller.
 */
ECode CTextPaint::GetTextBounds(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ IRect* bounds)
{
    return TextPaint::GetTextBounds(text, start, end, bounds);
}

/**
 * Return the advance widths for the characters in the string.
 *
 * @param text     The text to measure. Cannot be null.
 * @param index    The index of the first char to to measure
 * @param count    The number of chars starting with index to measure
 * @param widths   array to receive the advance widths of the characters.
 *                 Must be at least a large as count.
 * @return         the actual number of widths returned.
 */
ECode CTextPaint::GetTextWidths(
    /* [in] */ const ArrayOf<Char32>& text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [out] */ ArrayOf<Float>* widths,
    /* [out] */ Int32* number)
{
    return TextPaint::GetTextWidths(text, index, count, widths, number);
}

/**
 * Return the advance widths for the characters in the string.
 *
 * @param text     The text to measure. Cannot be null.
 * @param start    The index of the first char to to measure
 * @param end      The end of the text slice to measure
 * @param widths   array to receive the advance widths of the characters.
 *                 Must be at least a large as (end - start).
 * @return         the actual number of widths returned.
 */
ECode CTextPaint::GetTextWidths(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ArrayOf<Float>* widths,
    /* [out] */ Int32* number)
{
    return TextPaint::GetTextWidths(text, start, end, widths, number);
}

/**
 * Return the advance widths for the characters in the string.
 *
 * @param text   The text to measure. Cannot be null.
 * @param start  The index of the first char to to measure
 * @param end    The end of the text slice to measure
 * @param widths array to receive the advance widths of the characters.
 *               Must be at least a large as the text.
 * @return       the number of unichars in the specified text.
 */
ECode CTextPaint::GetTextWidths(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ArrayOf<Float>* widths,
    /* [out] */ Int32* number)
{
    return TextPaint::GetTextWidths(text, start, end, widths, number);
}

/**
 * Return the advance widths for the characters in the string.
 *
 * @param text   The text to measure
 * @param widths array to receive the advance widths of the characters.
 *               Must be at least a large as the text.
 * @return       the number of unichars in the specified text.
 */
ECode CTextPaint::GetTextWidths(
    /* [in] */ const String& text,
    /* [in] */ ArrayOf<Float>* widths,
    /* [out] */ Int32* number)
{
    return TextPaint::GetTextWidths(text, widths, number);
}

/**
 * Return in bounds (allocated by the caller) the smallest rectangle that
 * encloses all of the characters, with an implied origin at (0,0).
 *
 * @param text  Array of chars to measure and return their unioned bounds
 * @param index Index of the first char in the array to measure
 * @param count The number of chars, beginning at index, to measure
 * @param bounds Returns the unioned bounds of all the text. Must be
 *               allocated by the caller.
 */
ECode CTextPaint::GetTextBounds(
    /* [in] */ const ArrayOf<Char32>& text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ IRect* bounds)
{
    return TextPaint::GetTextBounds(text, index, count, bounds);
}

/* Return the glyph Ids for the characters in the string.
 *
 * @param text   The text to measure
 * @param start  The index of the first char to to measure
 * @param end    The end of the text slice to measure
 * @param contextStart the index of the first character to use for shaping context,
 * must be <= start
 * @param contextEnd the index past the last character to use for shaping context,
 * must be >= end
 * @param flags the flags to control the advances, either {@link #DIRECTION_LTR}
 * or {@link #DIRECTION_RTL}
 * @param glyphs array to receive the glyph Ids of the characters.
 *               Must be at least a large as the text.
 * @return       the number of glyphs in the returned array
 *
 * @hide
 *
 * Used only for BiDi / RTL Tests
 */
ECode CTextPaint::GetTextGlyphs(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Int32 flags,
    /* [out] */ ArrayOf<Char32>* glyphs,
    /* [out] */ Int32* number)
{
    return TextPaint::GetTextGlyphs(text, start, end, contextStart, contextEnd,
                        flags, glyphs, number);
}

/**
 * Convenience overload that takes a char array instead of a
 * String.
 *
 * @see #getTextRunAdvances(String, int, int, int, int, int, float[], int)
 * @hide
 */
ECode CTextPaint::GetTextRunAdvances(
    /* [in] */ const ArrayOf<Char32>& chars,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Int32 contextIndex,
    /* [in] */ Int32 contextCount,
    /* [in] */ Int32 flags,
    /* [in] */ ArrayOf<Float>* advances,
    /* [in] */ Int32 advancesIndex,
    /* [out] */ Float* advance)
{
    return TextPaint::GetTextRunAdvances(chars, index, count, contextIndex,
        contextCount, flags, advances, advancesIndex, advance);
}

/**
 * Convenience overload that takes a char array instead of a
 * String.
 *
 * @see #getTextRunAdvances(String, int, int, int, int, int, float[], int, int)
 * @hide
 */
ECode CTextPaint::GetTextRunAdvances(
    /* [in] */ const ArrayOf<Char32>& chars,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [in] */ Int32 contextIndex,
    /* [in] */ Int32 contextCount,
    /* [in] */ Int32 flags,
    /* [in] */ ArrayOf<Float>* advances,
    /* [in] */ Int32 advancesIndex,
    /* [in] */ Int32 reserved,
    /* [out] */ Float* advance)
{
    return TextPaint::GetTextRunAdvances(chars, index, count, contextIndex,
        contextCount, flags, advances, advancesIndex, reserved, advance);
}

/**
 * Convenience overload that takes a CharSequence instead of a
 * String.
 *
 * @see #getTextRunAdvances(String, int, int, int, int, int, float[], int)
 * @hide
 */
ECode CTextPaint::GetTextRunAdvances(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Int32 flags,
    /* [in] */ ArrayOf<Float>* advances,
    /* [in] */ Int32 advancesIndex,
    /* [out] */ Float* advance)
{
    return TextPaint::GetTextRunAdvances(text, start, end, contextStart,
        contextEnd, flags, advances, advancesIndex, advance);
}

/**
 * Convenience overload that takes a CharSequence instead of a
 * String.
 *
 * @see #getTextRunAdvances(String, int, int, int, int, int, float[], int)
 * @hide
 */
ECode CTextPaint::GetTextRunAdvances(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Int32 flags,
    /* [in] */ ArrayOf<Float>* advances,
    /* [in] */ Int32 advancesIndex,
    /* [in] */ Int32 reserved,
    /* [out] */ Float* advance)
{
    return TextPaint::GetTextRunAdvances(text, start, end, contextStart, contextEnd,
        flags, advances, advancesIndex, reserved, advance);
}

/**
 * Returns the total advance width for the characters in the run
 * between start and end, and if advances is not null, the advance
 * assigned to each of these characters (java chars).
 *
 * <p>The trailing surrogate in a valid surrogate pair is assigned
 * an advance of 0.  Thus the number of returned advances is
 * always equal to count, not to the number of unicode codepoints
 * represented by the run.
 *
 * <p>In the case of conjuncts or combining marks, the total
 * advance is assigned to the first logical character, and the
 * following characters are assigned an advance of 0.
 *
 * <p>This generates the sum of the advances of glyphs for
 * characters in a reordered cluster as the width of the first
 * logical character in the cluster, and 0 for the widths of all
 * other characters in the cluster.  In effect, such clusters are
 * treated like conjuncts.
 *
 * <p>The shaping bounds limit the amount of context available
 * outside start and end that can be used for shaping analysis.
 * These bounds typically reflect changes in bidi level or font
 * metrics across which shaping does not occur.
 *
 * @param text the text to measure. Cannot be null.
 * @param start the index of the first character to measure
 * @param end the index past the last character to measure
 * @param contextStart the index of the first character to use for shaping context,
 * must be <= start
 * @param contextEnd the index past the last character to use for shaping context,
 * must be >= end
 * @param flags the flags to control the advances, either {@link #DIRECTION_LTR}
 * or {@link #DIRECTION_RTL}
 * @param advances array to receive the advances, must have room for all advances,
 * can be null if only total advance is needed
 * @param advancesIndex the position in advances at which to put the
 * advance corresponding to the character at start
 * @return the total advance
 *
 * @hide
 */
ECode CTextPaint::GetTextRunAdvances(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Int32 flags,
    /* [in] */ ArrayOf<Float>* advances,
    /* [in] */ Int32 advancesIndex,
    /* [out] */ Float* advance)
{
    return TextPaint::GetTextRunAdvances(text, start, end, contextStart,
        contextEnd, flags, advances, advancesIndex, advance);
}

/**
 * Returns the total advance width for the characters in the run
 * between start and end, and if advances is not null, the advance
 * assigned to each of these characters (java chars).
 *
 * <p>The trailing surrogate in a valid surrogate pair is assigned
 * an advance of 0.  Thus the number of returned advances is
 * always equal to count, not to the number of unicode codepoints
 * represented by the run.
 *
 * <p>In the case of conjuncts or combining marks, the total
 * advance is assigned to the first logical character, and the
 * following characters are assigned an advance of 0.
 *
 * <p>This generates the sum of the advances of glyphs for
 * characters in a reordered cluster as the width of the first
 * logical character in the cluster, and 0 for the widths of all
 * other characters in the cluster.  In effect, such clusters are
 * treated like conjuncts.
 *
 * <p>The shaping bounds limit the amount of context available
 * outside start and end that can be used for shaping analysis.
 * These bounds typically reflect changes in bidi level or font
 * metrics across which shaping does not occur.
 *
 * @param text the text to measure. Cannot be null.
 * @param start the index of the first character to measure
 * @param end the index past the last character to measure
 * @param contextStart the index of the first character to use for shaping context,
 * must be <= start
 * @param contextEnd the index past the last character to use for shaping context,
 * must be >= end
 * @param flags the flags to control the advances, either {@link #DIRECTION_LTR}
 * or {@link #DIRECTION_RTL}
 * @param advances array to receive the advances, must have room for all advances,
 * can be null if only total advance is needed
 * @param advancesIndex the position in advances at which to put the
 * advance corresponding to the character at start
 * @param reserved int reserved value
 * @return the total advance
 *
 * @hide
 */
ECode CTextPaint::GetTextRunAdvances(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Int32 flags,
    /* [in] */ ArrayOf<Float>* advances,
    /* [in] */ Int32 advancesIndex,
    /* [in] */ Int32 reserved,
    /* [out] */ Float* advance)
{
    return TextPaint::GetTextRunAdvances(text, start, end, contextStart,
        contextEnd, flags, advances, advancesIndex, reserved, advance);
}

/**
 * Returns the next cursor position in the run.  This avoids placing the
 * cursor between surrogates, between characters that form conjuncts,
 * between base characters and combining marks, or within a reordering
 * cluster.
 *
 * <p>ContextStart and offset are relative to the start of text.
 * The context is the shaping context for cursor movement, generally
 * the bounds of the metric span enclosing the cursor in the direction of
 * movement.
 *
 * <p>If cursorOpt is {@link #CURSOR_AT} and the offset is not a valid
 * cursor position, this returns -1.  Otherwise this will never return a
 * value before contextStart or after contextStart + contextLength.
 *
 * @param text the text
 * @param contextStart the start of the context
 * @param contextLength the length of the context
 * @param flags either {@link #DIRECTION_RTL} or {@link #DIRECTION_LTR}
 * @param offset the cursor position to move from
 * @param cursorOpt how to move the cursor, one of {@link #CURSOR_AFTER},
 * {@link #CURSOR_AT_OR_AFTER}, {@link #CURSOR_BEFORE},
 * {@link #CURSOR_AT_OR_BEFORE}, or {@link #CURSOR_AT}
 * @return the offset of the next position, or -1
 * @hide
 */
ECode CTextPaint::GetTextRunCursor(
    /* [in] */ const ArrayOf<Char32>& text,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextLength,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 cursorOpt,
    /* [out] */ Int32* position)
{
    return TextPaint::GetTextRunCursor(text, contextStart, contextLength,
        flags, offset, cursorOpt, position);
}

/**
 * Returns the next cursor position in the run.  This avoids placing the
 * cursor between surrogates, between characters that form conjuncts,
 * between base characters and combining marks, or within a reordering
 * cluster.
 *
 * <p>ContextStart, contextEnd, and offset are relative to the start of
 * text.  The context is the shaping context for cursor movement, generally
 * the bounds of the metric span enclosing the cursor in the direction of
 * movement.
 *
 * <p>If cursorOpt is {@link #CURSOR_AT} and the offset is not a valid
 * cursor position, this returns -1.  Otherwise this will never return a
 * value before contextStart or after contextEnd.
 *
 * @param text the text
 * @param contextStart the start of the context
 * @param contextEnd the end of the context
 * @param flags either {@link #DIRECTION_RTL} or {@link #DIRECTION_LTR}
 * @param offset the cursor position to move from
 * @param cursorOpt how to move the cursor, one of {@link #CURSOR_AFTER},
 * {@link #CURSOR_AT_OR_AFTER}, {@link #CURSOR_BEFORE},
 * {@link #CURSOR_AT_OR_BEFORE}, or {@link #CURSOR_AT}
 * @return the offset of the next position, or -1
 * @hide
 */
ECode CTextPaint::GetTextRunCursor(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 cursorOpt,
    /* [out] */ Int32* position)
{
    return TextPaint::GetTextRunCursor(text, contextStart, contextEnd,
        flags, offset, cursorOpt, position);
}

/**
 * Returns the next cursor position in the run.  This avoids placing the
 * cursor between surrogates, between characters that form conjuncts,
 * between base characters and combining marks, or within a reordering
 * cluster.
 *
 * <p>ContextStart, contextEnd, and offset are relative to the start of
 * text.  The context is the shaping context for cursor movement, generally
 * the bounds of the metric span enclosing the cursor in the direction of
 * movement.
 *
 * <p>If cursorOpt is {@link #CURSOR_AT} and the offset is not a valid
 * cursor position, this returns -1.  Otherwise this will never return a
 * value before contextStart or after contextEnd.
 *
 * @param text the text
 * @param contextStart the start of the context
 * @param contextEnd the end of the context
 * @param flags either {@link #DIRECTION_RTL} or {@link #DIRECTION_LTR}
 * @param offset the cursor position to move from
 * @param cursorOpt how to move the cursor, one of {@link #CURSOR_AFTER},
 * {@link #CURSOR_AT_OR_AFTER}, {@link #CURSOR_BEFORE},
 * {@link #CURSOR_AT_OR_BEFORE}, or {@link #CURSOR_AT}
 * @return the offset of the next position, or -1
 * @hide
 */
ECode CTextPaint::GetTextRunCursor(
    /* [in] */ const String& text,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 cursorOpt,
    /* [out] */ Int32* position)
{
    return TextPaint::GetTextRunCursor(text, contextStart, contextEnd,
        flags, offset, cursorOpt, position);
}

/**
 * Return the width of the text.
 *
 * @param text  The text to measure. Cannot be null.
 * @param index The index of the first character to start measuring
 * @param count THe number of characters to measure, beginning with start
 * @return      The width of the text
 */
ECode CTextPaint::MeasureText(
    /* [in] */ const ArrayOf<Char32>& text,
    /* [in] */ Int32 index,
    /* [in] */ Int32 count,
    /* [out] */ Float* width)
{
    return TextPaint::MeasureText(text, index, count, width);
}

 /**
 * Return the width of the text.
 *
 * @param text  The text to measure. Cannot be null.
 * @param start The index of the first character to start measuring
 * @param end   1 beyond the index of the last character to measure
 * @return      The width of the text
 */
ECode CTextPaint::MeasureText(
    /* [in] */ const String& text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Float* width)
{
    return TextPaint::MeasureText(text, start, end, width);
}

/**
 * Return the width of the text.
 *
 * @param text  The text to measure. Cannot be null.
 * @return      The width of the text
 */
ECode CTextPaint::MeasureText(
    /* [in] */ const String& text,
    /* [out] */ Float* width)
{
    return TextPaint::MeasureText(text, width);
}

/**
 * Return the width of the text.
 *
 * @param text  The text to measure
 * @param start The index of the first character to start measuring
 * @param end   1 beyond the index of the last character to measure
 * @return      The width of the text
 */
ECode CTextPaint::MeasureText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Float* width)
{
    return TextPaint::MeasureText(text, start, end, width);
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
