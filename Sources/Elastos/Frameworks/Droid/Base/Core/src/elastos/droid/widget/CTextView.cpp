#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/CTextView.h"

using Elastos::Droid::View::EIID_View;

namespace Elastos {
namespace Droid {
namespace Widget {


IVIEW_METHODS_IMPL(CTextView, TextView)
IDRAWABLECALLBACK_METHODS_IMPL(CTextView, TextView)
IKEYEVENTCALLBACK_METHODS_IMPL(CTextView, TextView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CTextView, TextView)


PInterface CTextView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>(this);
    }
    else if (riid == EIID_TextView) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CTextView::Probe(riid);
}

ECode CTextView::constructor(
    /* [in] */ IContext* context)
{
    return TextView::Init(context);
}

ECode CTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TextView::Init(context, attrs);
}

ECode CTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return TextView::Init(context, attrs, defStyle);
}

ECode CTextView::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TextView::OnPreDraw();
    return NOERROR;
}

ECode CTextView::SetTypeface(
    /* [in] */ ITypeface* tf,
    /* [in] */ Int32 style)
{
    return TextView::SetTypeface(tf, style);
}

ECode CTextView::GetText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    AutoPtr<ICharSequence> c = TextView::GetText();
    *text = c;
    REFCOUNT_ADD(*text);
    return NOERROR;
}

ECode CTextView::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);
    *length = TextView::GetLength();
    return NOERROR;
}

ECode CTextView::GetEditableText(
    /* [out] */ IEditable** editable)
{
    VALIDATE_NOT_NULL(editable);
    AutoPtr<IEditable> obj = TextView::GetEditableText();
    *editable = obj;
    REFCOUNT_ADD(*editable);
    return NOERROR;
}

ECode CTextView::GetLineHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = TextView::GetLineHeight();
    return NOERROR;
}

ECode CTextView::GetLayout(
    /* [out] */ ILayout** layout)
{
    VALIDATE_NOT_NULL(layout);
    AutoPtr<ILayout> obj = TextView::GetLayout();
    *layout = obj;
    REFCOUNT_ADD(*layout);
    return NOERROR;
}

ECode CTextView::GetKeyListener(
    /* [out] */ IKeyListener** listener)
{
    VALIDATE_NOT_NULL(listener);
    AutoPtr<IKeyListener> obj = TextView::GetKeyListener();
    *listener = obj;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

ECode CTextView::SetKeyListener(
    /* [in] */ IKeyListener* input)
{
    return TextView::SetKeyListener(input);
}

ECode CTextView::GetHorizontalOffsetForDrawables(
    /* [out] */ Int32* offset)
{
    *offset = TextView::GetHorizontalOffsetForDrawables();
    return NOERROR;
}

ECode CTextView::GetMovementMethod(
    /* [out] */ IMovementMethod** movement)
{
    VALIDATE_NOT_NULL(movement);
    AutoPtr<IMovementMethod> obj = TextView::GetMovementMethod();
    *movement = obj;
    REFCOUNT_ADD(*movement);
    return NOERROR;
}

ECode CTextView::SetMovementMethod(
    /* [in] */ IMovementMethod* movement)
{
    return TextView::SetMovementMethod(movement);
}

ECode CTextView::GetTransformationMethod(
    /* [out] */ ITransformationMethod** method)
{
    VALIDATE_NOT_NULL(method);
    AutoPtr<ITransformationMethod> obj = TextView::GetTransformationMethod();
    *method = obj;
    REFCOUNT_ADD(*method);
    return NOERROR;
}

ECode CTextView::SetTransformationMethod(
    /* [in] */ ITransformationMethod* method)
{
    return TextView::SetTransformationMethod(method);
}

ECode CTextView::GetCompoundPaddingTop(
    /* [out] */ Int32* top)
{
    VALIDATE_NOT_NULL(top);
    *top = TextView::GetCompoundPaddingTop();
    return NOERROR;
}

ECode CTextView::GetCompoundPaddingBottom(
    /* [out] */ Int32* bottom)
{
    VALIDATE_NOT_NULL(bottom);
    *bottom = TextView::GetCompoundPaddingBottom();
    return NOERROR;
}

ECode CTextView::GetCompoundPaddingLeft(
    /* [out] */ Int32* left)
{
    VALIDATE_NOT_NULL(left);
    *left = TextView::GetCompoundPaddingLeft();
    return NOERROR;
}

ECode CTextView::GetCompoundPaddingRight(
    /* [out] */ Int32* right)
{
    VALIDATE_NOT_NULL(right);
    *right = TextView::GetCompoundPaddingRight();
    return NOERROR;
}

ECode CTextView::GetCompoundPaddingStart(
    /* [out] */ Int32* start)
{
    VALIDATE_NOT_NULL(start);
    *start = TextView::GetCompoundPaddingStart();
    return NOERROR;
}

ECode CTextView::GetCompoundPaddingEnd(
    /* [out] */ Int32* end)
{
    VALIDATE_NOT_NULL(end);
    *end = TextView::GetCompoundPaddingEnd();
    return NOERROR;
}

ECode CTextView::GetExtendedPaddingTop(
    /* [out] */ Int32* top)
{
    VALIDATE_NOT_NULL(top);
    *top = TextView::GetExtendedPaddingTop();
    return NOERROR;
}

ECode CTextView::GetExtendedPaddingBottom(
    /* [out] */ Int32* bottom)
{
    VALIDATE_NOT_NULL(bottom);
    *bottom = TextView::GetExtendedPaddingBottom();
    return NOERROR;
}

ECode CTextView::GetTotalPaddingLeft(
    /* [out] */ Int32* left)
{
    VALIDATE_NOT_NULL(left);
    *left = TextView::GetTotalPaddingLeft();
    return NOERROR;
}

ECode CTextView::GetTotalPaddingRight(
    /* [out] */ Int32* right)
{
    VALIDATE_NOT_NULL(right);
    *right = TextView::GetTotalPaddingRight();
    return NOERROR;
}

ECode CTextView::GetTotalPaddingStart(
    /* [out] */ Int32* start)
{
    VALIDATE_NOT_NULL(start);
    *start = TextView::GetTotalPaddingStart();
    return NOERROR;
}

ECode CTextView::GetTotalPaddingEnd(
    /* [out] */ Int32* end)
{
    VALIDATE_NOT_NULL(end);
    *end = TextView::GetTotalPaddingEnd();
    return NOERROR;
}

ECode CTextView::GetTotalPaddingTop(
    /* [out] */ Int32* top)
{
    VALIDATE_NOT_NULL(top);
    *top = TextView::GetTotalPaddingTop();
    return NOERROR;
}

ECode CTextView::GetTotalPaddingBottom(
    /* [out] */ Int32* bottom)
{
    VALIDATE_NOT_NULL(bottom);
    *bottom = TextView::GetTotalPaddingBottom();
    return NOERROR;
}

ECode CTextView::SetCompoundDrawables(
    /* [in] */ IDrawable* left,
    /* [in] */ IDrawable* top,
    /* [in] */ IDrawable* right,
    /* [in] */ IDrawable* bottom)
{
    return TextView::SetCompoundDrawables(left, top, right, bottom);
}

ECode CTextView::SetCompoundDrawablesWithIntrinsicBounds(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return TextView::SetCompoundDrawablesWithIntrinsicBounds(left, top, right, bottom);
}

ECode CTextView::SetCompoundDrawablesWithIntrinsicBounds(
    /* [in] */ IDrawable* left,
    /* [in] */ IDrawable* top,
    /* [in] */ IDrawable* right,
    /* [in] */ IDrawable* bottom)
{
    return TextView::SetCompoundDrawablesWithIntrinsicBounds(left, top, right, bottom);
}

ECode CTextView::SetCompoundDrawablesRelative(
    /* [in] */ IDrawable* start,
    /* [in] */ IDrawable* top,
    /* [in] */ IDrawable* end,
    /* [in] */ IDrawable* bottom)
{
    return TextView::SetCompoundDrawablesRelative(start, top, end, bottom);
}

ECode CTextView::SetCompoundDrawablesRelativeWithIntrinsicBounds(
    /* [in] */ Int32 start,
    /* [in] */ Int32 top,
    /* [in] */ Int32 end,
    /* [in] */ Int32 bottom)
{
    return TextView::SetCompoundDrawablesRelativeWithIntrinsicBounds(start, top, end, bottom);
}

ECode CTextView::SetCompoundDrawablesRelativeWithIntrinsicBounds(
    /* [in] */ IDrawable* start,
    /* [in] */ IDrawable* top,
    /* [in] */ IDrawable* end,
    /* [in] */ IDrawable* bottom)
{
    return TextView::SetCompoundDrawablesRelativeWithIntrinsicBounds(start, top, end, bottom);
}

ECode CTextView::GetCompoundDrawables(
    /* [out, callee] */ ArrayOf<IDrawable*>** drawables)
{
    VALIDATE_NOT_NULL(drawables);
    AutoPtr<ArrayOf<IDrawable*> > obj = TextView::GetCompoundDrawables();
    *drawables = obj;
    REFCOUNT_ADD(*drawables);
    return NOERROR;
}

ECode CTextView::GetCompoundDrawablesRelative(
    /* [out, callee] */ ArrayOf<IDrawable*>** drawables)
{
    VALIDATE_NOT_NULL(drawables);
    AutoPtr<ArrayOf<IDrawable*> > obj = TextView::GetCompoundDrawablesRelative();
    *drawables = obj;
    REFCOUNT_ADD(*drawables);
    return NOERROR;
}

ECode CTextView::SetCompoundDrawablePadding(
    /* [in] */ Int32 pad)
{
    return TextView::SetCompoundDrawablePadding(pad);
}

ECode CTextView::GetCompoundDrawablePadding(
    /* [out] */ Int32* pad)
{
    VALIDATE_NOT_NULL(pad);
    *pad = TextView::GetCompoundDrawablePadding();
    return NOERROR;
}

ECode CTextView::GetAutoLinkMask(
    /* [out] */ Int32* mask)
{
    VALIDATE_NOT_NULL(mask);
    *mask = TextView::GetAutoLinkMask();
    return NOERROR;
}

ECode CTextView::SetTextAppearance(
    /* [in] */ IContext* contxt,
    /* [in] */ Int32 resid)
{
    return TextView::SetTextAppearance(contxt, resid);
}

ECode CTextView::GetTextLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);
    AutoPtr<ILocale> obj = TextView::GetTextLocale();
    *locale = obj;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}

ECode CTextView::SetTextLocale(
    /* [in] */ ILocale* locale)
{
    return TextView::SetTextLocale(locale);
}

ECode CTextView::GetTextSize(
    /* [out] */ Float* size)
{
    VALIDATE_NOT_NULL(size);
    *size = TextView::GetTextSize();
    return NOERROR;
}

ECode CTextView::SetTextSize(
    /* [in] */ Float size)
{
    return TextView::SetTextSize(size);
}

ECode CTextView::SetTextSize(
    /* [in] */ Int32 unit,
    /* [in] */ Float size)
{
    return TextView::SetTextSize(unit, size);
}

ECode CTextView::GetTextScaleX(
    /* [out] */ Float* size)
{
    VALIDATE_NOT_NULL(size);
    *size = TextView::GetTextScaleX();
    return NOERROR;
}

ECode CTextView::SetTextScaleX(
    /* [in] */ Float size)
{
    return TextView::SetTextScaleX(size);
}

ECode CTextView::SetTypeface(
    /* [in] */ ITypeface* tf)
{
    return TextView::SetTypeface(tf);
}

ECode CTextView::GetTypeface(
    /* [out] */ ITypeface** face)
{
    VALIDATE_NOT_NULL(face);
    AutoPtr<ITypeface> obj = TextView::GetTypeface();
    *face = obj;
    REFCOUNT_ADD(*face);
    return NOERROR;
}

ECode CTextView::SetTextColor(
    /* [in] */ Int32 color)
{
    return TextView::SetTextColor(color);
}

ECode CTextView::SetTextColor(
    /* [in] */ IColorStateList* colors)
{
    return TextView::SetTextColor(colors);
}

ECode CTextView::GetTextColors(
    /* [out] */ IColorStateList** colors)
{
    VALIDATE_NOT_NULL(colors);
    AutoPtr<IColorStateList> obj = TextView::GetTextColors();
    *colors = obj;
    REFCOUNT_ADD(*colors);
    return NOERROR;
}

ECode CTextView::GetCurrentTextColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    *color = TextView::GetCurrentTextColor();
    return NOERROR;
}

ECode CTextView::SetHighlightColor(
    /* [in] */ Int32 color)
{
    return TextView::SetHighlightColor(color);
}

ECode CTextView::GetHighlightColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    *color = TextView::GetHighlightColor();
    return NOERROR;
}

ECode CTextView::SetShowSoftInputOnFocus(
    /* [in] */ Boolean show)
{
    return TextView::SetShowSoftInputOnFocus(show);
}

ECode CTextView::GetShowSoftInputOnFocus(
    /* [out] */ Boolean* show)
{
    VALIDATE_NOT_NULL(show);
    *show = TextView::GetShowSoftInputOnFocus();
    return NOERROR;
}

ECode CTextView::SetShadowLayer(
    /* [in] */ Float radius,
    /* [in] */ Float dx,
    /* [in] */ Float dy,
    /* [in] */ Int32 color)
{
    return TextView::SetShadowLayer(radius, dx, dy, color);
}

ECode CTextView::GetShadowRadius(
    /* [out] */ Float* radius)
{
    VALIDATE_NOT_NULL(radius);
    *radius = TextView::GetShadowRadius();
    return NOERROR;
}

ECode CTextView::GetShadowDx(
    /* [out] */ Float* dx)
{
    VALIDATE_NOT_NULL(dx);
    *dx = TextView::GetShadowDx();
    return NOERROR;
}

ECode CTextView::GetShadowDy(
    /* [out] */ Float* dy)
{
    VALIDATE_NOT_NULL(dy);
    *dy = TextView::GetShadowDy();
    return NOERROR;
}

ECode CTextView::GetShadowColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    *color = TextView::GetShadowColor();
    return NOERROR;
}

ECode CTextView::GetPaint(
    /* [out] */ ITextPaint** paint)
{
    VALIDATE_NOT_NULL(paint);
    AutoPtr<ITextPaint> obj = TextView::GetPaint();
    *paint = obj;
    REFCOUNT_ADD(*paint);
    return NOERROR;
}

ECode CTextView::SetAutoLinkMask(
    /* [in] */ Int32 mask)
{
    return TextView::SetAutoLinkMask(mask);
}

ECode CTextView::SetLinksClickable(
    /* [in] */ Boolean whether)
{
    return TextView::SetLinksClickable(whether);
}

ECode CTextView::GetLinksClickable(
    /* [out] */ Boolean* whether)
{
    VALIDATE_NOT_NULL(whether);
    *whether = TextView::GetLinksClickable();
    return NOERROR;
}

ECode CTextView::GetUrls(
    /* [out, callee] */ ArrayOf<IURLSpan*>** urls)
{
    VALIDATE_NOT_NULL(urls);
    AutoPtr<ArrayOf<IURLSpan*> > obj = TextView::GetUrls();
    *urls = obj;
    REFCOUNT_ADD(*urls);
    return NOERROR;
}

ECode CTextView::SetHintTextColor(
    /* [in] */ Int32 color)
{
    return TextView::SetHintTextColor(color);
}

ECode CTextView::SetHintTextColor(
    /* [in] */ IColorStateList* colors)
{
    return TextView::SetHintTextColor(colors);
}

ECode CTextView::GetHintTextColors(
    /* [out] */ IColorStateList** colors)
{
    VALIDATE_NOT_NULL(colors);
    AutoPtr<IColorStateList> obj = TextView::GetHintTextColors();
    *colors = obj;
    REFCOUNT_ADD(*colors);
    return NOERROR;
}

ECode CTextView::GetCurrentHintTextColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    *color = TextView::GetCurrentHintTextColor();
    return NOERROR;
}

ECode CTextView::SetLinkTextColor(
    /* [in] */ Int32 color)
{
    return TextView::SetLinkTextColor(color);
}

ECode CTextView::SetLinkTextColor(
    /* [in] */ IColorStateList* colors)
{
    return TextView::SetLinkTextColor(colors);
}

ECode CTextView::GetLinkTextColors(
    /* [out] */ IColorStateList** colors)
{
    VALIDATE_NOT_NULL(colors);
    AutoPtr<IColorStateList> obj = TextView::GetLinkTextColors();
    *colors = obj;
    REFCOUNT_ADD(*colors);
    return NOERROR;
}

ECode CTextView::SetGravity(
    /* [in] */ Int32 gravity)
{
    return TextView::SetGravity(gravity);
}

ECode CTextView::GetGravity(
    /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity);
    *gravity = TextView::GetGravity();
    return NOERROR;
}

ECode CTextView::GetPaintFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = TextView::GetPaintFlags();
    return NOERROR;
}

ECode CTextView::SetPaintFlags(
    /* [in] */ Int32 flags)
{
    return TextView::SetPaintFlags(flags);
}

ECode CTextView::SetHorizontallyScrolling(
    /* [in] */ Boolean whether)
{
    return TextView::SetHorizontallyScrolling(whether);
}

ECode CTextView::GetHorizontallyScrolling(
    /* [out] */ Boolean* whether)
{
    VALIDATE_NOT_NULL(whether);
    *whether = TextView::GetHorizontallyScrolling();
    return NOERROR;
}

ECode CTextView::SetMinLines(
    /* [in] */ Int32 minlines)
{
    return TextView::SetMinLines(minlines);
}

ECode CTextView::GetMinLines(
    /* [out] */ Int32* minlines)
{
    VALIDATE_NOT_NULL(minlines);
    *minlines = TextView::GetMinLines();
    return NOERROR;
}

ECode CTextView::SetMinHeight(
    /* [in] */ Int32 minHeight)
{
    return TextView::SetMinHeight(minHeight);
}

ECode CTextView::GetMinHeight(
    /* [out] */ Int32* minHeight)
{
    VALIDATE_NOT_NULL(minHeight);
    *minHeight = TextView::GetMinHeight();
    return NOERROR;
}

ECode CTextView::SetMaxLines(
    /* [in] */ Int32 maxlines)
{
    return TextView::SetMaxLines(maxlines);
}

ECode CTextView::GetMaxLines(
    /* [out] */ Int32* maxlines)
{
    VALIDATE_NOT_NULL(maxlines);
    *maxlines = TextView::GetMaxLines();
    return NOERROR;
}

ECode CTextView::SetMaxHeight(
    /* [in] */ Int32 maxHeight)
{
    return TextView::SetMaxHeight(maxHeight);
}

ECode CTextView::GetMaxHeight(
    /* [out] */ Int32* maxHeight)
{
    VALIDATE_NOT_NULL(maxHeight);
    *maxHeight = TextView::GetMaxHeight();
    return NOERROR;
}

ECode CTextView::SetLines(
    /* [in] */ Int32 lines)
{
    return TextView::SetLines(lines);
}

ECode CTextView::SetHeight(
    /* [in] */ Int32 pixels)
{
    return TextView::SetHeight(pixels);
}

ECode CTextView::SetMinEms(
    /* [in] */ Int32 minems)
{
    return TextView::SetMinEms(minems);
}

ECode CTextView::GetMinEms(
    /* [out] */ Int32* minems)
{
    VALIDATE_NOT_NULL(minems);
    *minems = TextView::GetMinEms();
    return NOERROR;
}

ECode CTextView::SetMinWidth(
    /* [in] */ Int32 minpixels)
{
    return TextView::SetMinWidth(minpixels);
}

ECode CTextView::GetMinWidth(
    /* [out] */ Int32* minpixels)
{
    VALIDATE_NOT_NULL(minpixels);
    *minpixels = TextView::GetMinWidth();
    return NOERROR;
}

ECode CTextView::SetMaxEms(
    /* [in] */ Int32 maxems)
{
    return TextView::SetMaxEms(maxems);
}

ECode CTextView::SetMaxWidth(
    /* [in] */ Int32 maxpixels)
{
    return TextView::SetMaxWidth(maxpixels);
}

ECode CTextView::GetMaxWidth(
    /* [out] */ Int32* maxpixels)
{
    VALIDATE_NOT_NULL(maxpixels);
    *maxpixels = TextView::GetMaxWidth();
    return NOERROR;
}

ECode CTextView::SetEms(
    /* [in] */ Int32 ems)
{
    return TextView::SetEms(ems);
}

ECode CTextView::SetWidth(
    /* [in] */ Int32 pixels)
{
    return TextView::SetWidth(pixels);
}

ECode CTextView::SetLineSpacing(
    /* [in] */ Float add,
    /* [in] */ Float mult)
{
    return TextView::SetLineSpacing(add, mult);
}

ECode CTextView::GetLineSpacingMultiplier(
    /* [out] */ Float* multiplier)
{
    VALIDATE_NOT_NULL(multiplier);
    *multiplier = TextView::GetLineSpacingMultiplier();
    return NOERROR;
}

ECode CTextView::GetLineSpacingExtra(
    /* [out] */ Float* extra)
{
    VALIDATE_NOT_NULL(extra);
    *extra = TextView::GetLineSpacingExtra();
    return NOERROR;
}

ECode CTextView::Append(
    /* [in] */ ICharSequence* text)
{
    return TextView::Append(text);
}

ECode CTextView::Append(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    return TextView::Append(text, start, end);
}

ECode CTextView::SetFreezesText(
    /* [in] */ Boolean freezesText)
{
    return TextView::SetFreezesText(freezesText);
}

ECode CTextView::GetFreezesText(
    /* [out] */ Boolean* text)
{
    VALIDATE_NOT_NULL(text);
    *text = TextView::GetFreezesText();
    return NOERROR;
}

ECode CTextView::SetEditableFactory(
    /* [in] */ IEditableFactory* factory)
{
    return TextView::SetEditableFactory(factory);
}

ECode CTextView::SetSpannableFactory(
    /* [in] */ ISpannableFactory* factory)
{
    return TextView::SetSpannableFactory(factory);
}

ECode CTextView::SetText(
    /* [in] */ ICharSequence* text)
{
    return TextView::SetText(text);
}

ECode CTextView::SetTextKeepState(
    /* [in] */ ICharSequence* text)
{
    return TextView::SetTextKeepState(text);
}

ECode CTextView::SetText(
    /* [in] */ ICharSequence* text,
    /* [in] */ BufferType type)
{
    return TextView::SetText(text, type);
}

ECode CTextView::SetText(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 len)
{
    return TextView::SetText(text, start, len);
}

ECode CTextView::SetTextKeepState(
    /* [in] */ ICharSequence* text,
    /* [in] */ BufferType type)
{
    return TextView::SetTextKeepState(text, type);
}

ECode CTextView::SetText(
    /* [in] */ Int32 resid)
{
    return TextView::SetText(resid);
}

ECode CTextView::SetText(
    /* [in] */ Int32 resid,
    /* [in] */ BufferType type)
{
    return TextView::SetText(resid, type);
}

ECode CTextView::SetHint(
    /* [in] */ ICharSequence* hint)
{
    return TextView::SetHint(hint);
}

ECode CTextView::SetHint(
    /* [in] */ Int32 resid)
{
    return TextView::SetHint(resid);
}

ECode CTextView::GetHint(
    /* [out] */ ICharSequence** hint)
{
    VALIDATE_NOT_NULL(hint);
    AutoPtr<ICharSequence> obj = TextView::GetHint();
    *hint = obj;
    REFCOUNT_ADD(*hint);
    return NOERROR;
}

ECode CTextView::SetInputType(
    /* [in] */ Int32 type)
{
    return TextView::SetInputType(type);
}

ECode CTextView::SetRawInputType(
    /* [in] */ Int32 type)
{
    return TextView::SetRawInputType(type);
}

ECode CTextView::GetInputType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = TextView::GetInputType();
    return NOERROR;
}

ECode CTextView::SetImeOptions(
    /* [in] */ Int32 imeOptions)
{
    return TextView::SetImeOptions(imeOptions);
}

ECode CTextView::GetImeOptions(
    /* [out] */ Int32* options)
{
    VALIDATE_NOT_NULL(options);
    *options = TextView::GetImeOptions();
    return NOERROR;
}

ECode CTextView::SetImeActionLabel(
    /* [in] */ ICharSequence* label,
    /* [in] */ Int32 actionId)
{
    return TextView::SetImeActionLabel(label, actionId);
}

ECode CTextView::GetImeActionLabel(
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);
    AutoPtr<ICharSequence> obj = TextView::GetImeActionLabel();
    *label = obj;
    REFCOUNT_ADD(*label);
    return NOERROR;
}

ECode CTextView::GetImeActionId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = TextView::GetImeActionId();
    return NOERROR;
}

ECode CTextView::SetOnEditorActionListener(
    /* [in] */ IOnEditorActionListener* l)
{
    return TextView::SetOnEditorActionListener(l);
}

ECode CTextView::OnEditorAction(
    /* [in] */ Int32 actionCode)
{
    return TextView::OnEditorAction(actionCode);
}

ECode CTextView::SetPrivateImeOptions(
    /* [in] */ const String& type)
{
    return TextView::SetPrivateImeOptions(type);
}

ECode CTextView::GetPrivateImeOptions(
    /* [out] */ String* options)
{
    VALIDATE_NOT_NULL(options);
    *options = TextView::GetPrivateImeOptions();
    return NOERROR;
}

ECode CTextView::SetInputExtras(
    /* [in] */ Int32 xmlResId)
{
    return TextView::SetInputExtras(xmlResId);
}

ECode CTextView::GetInputExtras(
    /* [in] */ Boolean create,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle);
    AutoPtr<IBundle> obj = TextView::GetInputExtras(create);
    *bundle = obj;
    REFCOUNT_ADD(*bundle);
    return NOERROR;
}

ECode CTextView::GetError(
    /* [out] */ ICharSequence** error)
{
    VALIDATE_NOT_NULL(error);
    AutoPtr<ICharSequence> obj = TextView::GetError();
    *error = obj;
    REFCOUNT_ADD(*error);
    return NOERROR;
}

ECode CTextView::SetError(
    /* [in] */ ICharSequence* error)
{
    return TextView::SetError(error);
}

ECode CTextView::SetError(
    /* [in] */ ICharSequence* error,
    /* [in] */ IDrawable* icon)
{
    return TextView::SetError(error, icon);
}

ECode CTextView::SetFilters(
    /* [in] */ ArrayOf<IInputFilter*>* filters)
{
    return TextView::SetFilters(filters);
}

ECode CTextView::GetFilters(
    /* [out, callee] */ ArrayOf<IInputFilter*>** filters)
{
    VALIDATE_NOT_NULL(filters);
    AutoPtr<ArrayOf<IInputFilter*> > obj = TextView::GetFilters();
    *filters = obj;
    REFCOUNT_ADD(*filters);
    return NOERROR;
}

ECode CTextView::IsTextSelectable(
    /* [out] */ Boolean* selectable)
{
    VALIDATE_NOT_NULL(selectable);
    *selectable = TextView::IsTextSelectable();
    return NOERROR;
}

ECode CTextView::SetTextIsSelectable(
    /* [in] */ Boolean selectable)
{
    return TextView::SetTextIsSelectable(selectable);
}

ECode CTextView::GetLineCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = TextView::GetLineCount();
    return NOERROR;
}

ECode CTextView::GetLineBounds(
    /* [in] */ Int32 line,
    /* [in] */ IRect* bounds,
    /* [out] */ Int32* y)
{
    VALIDATE_NOT_NULL(y);
    *y = TextView::GetLineBounds(line, bounds);
    return NOERROR;
}

ECode CTextView::ResetErrorChangedFlag()
{
    return TextView::ResetErrorChangedFlag();
}

ECode CTextView::HideErrorIfUnchanged()
{
    return TextView::HideErrorIfUnchanged();
}

ECode CTextView::ExtractText(
    /* [in] */ IExtractedTextRequest* request,
    /* [in] */ IExtractedText* outText,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TextView::ExtractText(request, outText);
    return NOERROR;
}

ECode CTextView::SetExtractedText(
    /* [in] */ IExtractedText* text)
{
    return TextView::SetExtractedText(text);
}

ECode CTextView::SetExtracting(
    /* [in] */ IExtractedTextRequest* req)
{
    return TextView::SetExtracting(req);
}

ECode CTextView::OnCommitCompletion(
    /* [in] */ ICompletionInfo* text)
{
    return TextView::OnCommitCompletion(text);
}

ECode CTextView::OnCommitCorrection(
    /* [in] */ ICorrectionInfo* info)
{
    return TextView::OnCommitCorrection(info);
}

ECode CTextView::BeginBatchEdit()
{
    return TextView::BeginBatchEdit();
}

ECode CTextView::EndBatchEdit()
{
    return TextView::EndBatchEdit();
}

ECode CTextView::OnBeginBatchEdit()
{
    return TextView::OnBeginBatchEdit();
}

ECode CTextView::OnEndBatchEdit()
{
    return TextView::OnEndBatchEdit();
}

ECode CTextView::OnPrivateIMECommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TextView::OnPrivateIMECommand(action, data);
    return NOERROR;
}

ECode CTextView::SetIncludeFontPadding(
    /* [in] */ Boolean includepad)
{
    return TextView::SetIncludeFontPadding(includepad);
}

ECode CTextView::GetIncludeFontPadding(
    /* [out] */ Boolean* padding)
{
    VALIDATE_NOT_NULL(padding);
    *padding = TextView::GetIncludeFontPadding();
    return NOERROR;
}

ECode CTextView::BringPointIntoView(
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TextView::BringPointIntoView(offset);
    return NOERROR;
}

ECode CTextView::MoveCursorToVisibleOffset(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TextView::MoveCursorToVisibleOffset();
    return NOERROR;
}

ECode CTextView::GetSelectionStart(
    /* [out] */ Int32* start)
{
    VALIDATE_NOT_NULL(start);
    *start = TextView::GetSelectionStart();
    return NOERROR;
}

ECode CTextView::GetSelectionEnd(
    /* [out] */ Int32* end)
{
    VALIDATE_NOT_NULL(end);
    *end = TextView::GetSelectionEnd();
    return NOERROR;
}

ECode CTextView::HasSelection(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TextView::HasSelection();
    return NOERROR;
}

ECode CTextView::SetSingleLine()
{
    return TextView::SetSingleLine();
}

ECode CTextView::SetAllCaps(
    /* [in] */ Boolean allCaps)
{
    return TextView::SetAllCaps(allCaps);
}

ECode CTextView::SetSingleLine(
    /* [in] */ Boolean singleLine)
{
    return TextView::SetSingleLine(singleLine);
}

ECode CTextView::SetEllipsize(
    /* [in] */ TextUtilsTruncateAt where)
{
    return TextView::SetEllipsize(where);
}

ECode CTextView::SetMarqueeRepeatLimit(
    /* [in] */ Int32 marqueeLimit)
{
    return TextView::SetMarqueeRepeatLimit(marqueeLimit);
}

ECode CTextView::GetMarqueeRepeatLimit(
    /* [out] */ Int32* marqueeLimit)
{
    VALIDATE_NOT_NULL(marqueeLimit);
    *marqueeLimit = TextView::GetMarqueeRepeatLimit();
    return NOERROR;
}

ECode CTextView::GetEllipsize(
    /* [out] */ TextUtilsTruncateAt* size)
{
    VALIDATE_NOT_NULL(size);
    *size = TextView::GetEllipsize();
    return NOERROR;
}

ECode CTextView::SetSelectAllOnFocus(
    /* [in] */ Boolean selectAllOnFocus)
{
    return TextView::SetSelectAllOnFocus(selectAllOnFocus);
}

ECode CTextView::SetCursorVisible(
    /* [in] */ Boolean visible)
{
    return TextView::SetCursorVisible(visible);
}

ECode CTextView::IsCursorVisible(
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible);
    *visible = TextView::IsCursorVisible();
    return NOERROR;
}

ECode CTextView::OnTextContextMenuItem(
    /* [in] */ Int32 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TextView::OnTextContextMenuItem(id);
    return NOERROR;
}

ECode CTextView::AddTextChangedListener(
    /* [in] */ ITextWatcher* watcher)
{
    return TextView::AddTextChangedListener(watcher);
}

ECode CTextView::RemoveTextChangedListener(
    /* [in] */ ITextWatcher* watcher)
{
    return TextView::RemoveTextChangedListener(watcher);
}

ECode CTextView::ClearComposingText()
{
    return TextView::ClearComposingText();
}

ECode CTextView::DidTouchFocusSelect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TextView::DidTouchFocusSelect();
    return NOERROR;
}

ECode CTextView::SetScroller(
    /* [in] */ IScroller* s)
{
    return TextView::SetScroller(s);
}

ECode CTextView::GetTextServicesLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);
    AutoPtr<ILocale> obj = TextView::GetTextServicesLocale();
    *locale = obj;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}

ECode CTextView::GetWordIterator(
    /* [out] */ IWordIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator);
    AutoPtr<IWordIterator> obj = TextView::GetWordIterator();
    *iterator = obj;
    REFCOUNT_ADD(*iterator);
    return NOERROR;
}

ECode CTextView::GetTextForAccessibility(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    AutoPtr<ICharSequence> obj = TextView::GetTextForAccessibility();
    *text = obj;
    REFCOUNT_ADD(*text);
    return NOERROR;
}

ECode CTextView::IsInputMethodTarget(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TextView::IsInputMethodTarget();
    return NOERROR;
}

ECode CTextView::IsSuggestionsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = TextView::IsSuggestionsEnabled();
    return NOERROR;
}

ECode CTextView::SetCustomSelectionActionModeCallback(
    /* [in] */ IActionModeCallback* actionModeCallback)
{
    return TextView::SetCustomSelectionActionModeCallback(actionModeCallback);
}

ECode CTextView::GetCustomSelectionActionModeCallback(
    /* [out] */ IActionModeCallback** actionModeCallback)
{
    VALIDATE_NOT_NULL(actionModeCallback);
    AutoPtr<IActionModeCallback> obj = TextView::GetCustomSelectionActionModeCallback();
    *actionModeCallback = obj;
    REFCOUNT_ADD(*actionModeCallback);
    return NOERROR;
}

ECode CTextView::GetOffsetForPosition(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = TextView::GetOffsetForPosition(x, y);
    return NOERROR;
}

ECode CTextView::RemoveMisspelledSpans(
    /* [in] */ ISpannable* spannable)
{
    return TextView::RemoveMisspelledSpans(spannable);
}

ECode CTextView::OnLocaleChanged()
{
    return TextView::OnLocaleChanged();
}

ECode CTextView::InvalidateRegion(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Boolean invalidateCursor)
{
    return TextView::InvalidateRegion(start, end, invalidateCursor);
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
