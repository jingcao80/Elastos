
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/BitmapFactory.h"
#include "elastos/droid/graphics/CBitmapFactoryOptions.h"
#include "elastos/droid/graphics/CPorterDuffColorFilter.h"
#include "elastos/droid/graphics/Insets.h"
#include "elastos/droid/graphics/NinePatch.h"
#include "elastos/droid/graphics/drawable/CBitmapDrawable.h"
#include "elastos/droid/graphics/drawable/CColorDrawable.h"
#include "elastos/droid/graphics/drawable/CNinePatchDrawable.h"
#include "elastos/droid/graphics/drawable/CStateListDrawable.h"
#include "elastos/droid/graphics/drawable/CAnimatedRotateDrawable.h"
#include "elastos/droid/graphics/drawable/CLayerDrawable.h"
#include "elastos/droid/graphics/drawable/CGradientDrawable.h"
#include "elastos/droid/graphics/drawable/CClipDrawable.h"
#include "elastos/droid/graphics/drawable/CAnimationDrawable.h"
#include "elastos/droid/graphics/drawable/CLevelListDrawable.h"
#include "elastos/droid/graphics/drawable/CTransitionDrawable.h"
#include "elastos/droid/graphics/drawable/CScaleDrawable.h"
#include "elastos/droid/graphics/drawable/CRotateDrawable.h"
#include "elastos/droid/graphics/drawable/CInsetDrawable.h"
#include "elastos/droid/graphics/drawable/CAnimatedStateListDrawable.h"
#include "elastos/droid/graphics/drawable/CAnimatedVectorDrawable.h"
#include "elastos/droid/graphics/drawable/CRippleDrawable.h"
#include "elastos/droid/graphics/drawable/CVectorDrawable.h"
#include "elastos/droid/content/res/CColorStateList.h"
#include "elastos/droid/utility/StateSet.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/R.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Droid::Graphics::PorterDuffMode_NONE;
using Elastos::Droid::Content::Res::CColorStateList;
using Elastos::Droid::Utility::StateSet;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::R;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CAR_INTERFACE_IMPL(Drawable::ConstantState, Object, IDrawableConstantState);
ECode Drawable::ConstantState::GetWeakReference(
    /* [out] */ IWeakReference** weakReference)
{
    VALIDATE_NOT_NULL(weakReference)
    *weakReference = new WeakReferenceImpl(TO_IINTERFACE(this), CreateWeak(this));
    REFCOUNT_ADD(*weakReference)
    return NOERROR;
}

ECode Drawable::ConstantState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    return NewDrawable(drawable);
}

ECode Drawable::ConstantState::NewDrawable(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** drawable)
{
    return NewDrawable(drawable);
}

ECode Drawable::ConstantState::GetBitmap(
        /* [out] */ IBitmap** bp)
{
    VALIDATE_NOT_NULL(bp);
    *bp = NULL;
    return NOERROR;
}

ECode Drawable::ConstantState::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = FALSE;
    return NOERROR;
}

AutoPtr<IRect> Init_ZERO_BOUNDS_RECT()
{
    AutoPtr<CRect> rect;
    CRect::NewByFriend((CRect**)&rect);
    return (IRect*)rect.Get();
}

CAR_INTERFACE_IMPL(Drawable, Object, IDrawable);
AutoPtr<IRect> Drawable::ZERO_BOUNDS_RECT = Init_ZERO_BOUNDS_RECT();
const PorterDuffMode Drawable::DEFAULT_TINT_MODE = PorterDuffMode_SRC_IN;
Drawable::Drawable()
    : mStateSet(const_cast<ArrayOf<Int32>*>(StateSet::WILD_CARD.Get()))
    , mLevel(0)
    , mChangingConfigurations(0)
    , mBounds(ZERO_BOUNDS_RECT)
    , mVisible(TRUE)
    , mLayoutDirection(0)
{}

Drawable::~Drawable()
{
    mStateSet = NULL;
    mBounds = NULL;
    mCallback = NULL;
}

ECode Drawable::SetBounds(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<IRect> oldBounds = mBounds;

    if (oldBounds == ZERO_BOUNDS_RECT) {
        mBounds = NULL;
        FAIL_RETURN(CRect::New((IRect**)&mBounds));
        oldBounds = mBounds;
    }

    CRect* oldBoundsObj = (CRect*)oldBounds.Get();
    if (oldBoundsObj->mLeft != left || oldBoundsObj->mTop != top ||
            oldBoundsObj->mRight != right || oldBoundsObj->mBottom != bottom) {
        Boolean isEmpty = FALSE;
        if (!(oldBounds->IsEmpty(&isEmpty), isEmpty)) {
            // first invalidate the previous bounds
            InvalidateSelf();
        }
        mBounds->Set(left, top, right, bottom);
        OnBoundsChange(mBounds);
    }
    return NOERROR;
}

ECode Drawable::SetBounds(
    /* [in] */ IRect* bounds)
{
    CRect* boundsObj = (CRect*)bounds;
    return SetBounds(boundsObj->mLeft, boundsObj->mTop, boundsObj->mRight, boundsObj->mBottom);
}

ECode Drawable::CopyBounds(
    /* [in] */ IRect* bounds)
{
    bounds->Set(mBounds);
    return NOERROR;
}

ECode Drawable::CopyBounds(
    /* [out] */ IRect** bounds)
{
    VALIDATE_NOT_NULL(bounds);
    return CRect::New(mBounds.Get(), bounds);
}

ECode Drawable::GetBounds(
    /* [out] */ IRect** bounds)
{
    VALIDATE_NOT_NULL(bounds);
    if (mBounds == ZERO_BOUNDS_RECT) {
        mBounds = NULL;
        ASSERT_SUCCEEDED(CRect::New((IRect**)&mBounds));
    }

    *bounds = mBounds;
    REFCOUNT_ADD(*bounds);
    return NOERROR;
}

ECode Drawable::GetDirtyBounds(
    /* [out] */ IRect** bounds)
{
    return GetBounds(bounds);
}

ECode Drawable::SetChangingConfigurations(
    /* [in] */ Int32 configs)
{
    mChangingConfigurations = configs;
    return NOERROR;
}

ECode Drawable::GetChangingConfigurations(
    /* [out] */ Int32* configs)
{
    VALIDATE_NOT_NULL(configs);
    *configs = mChangingConfigurations;
    return NOERROR;
}

ECode Drawable::SetDither(
    /* [in] */ Boolean dither)
{
    return NOERROR;
}

ECode Drawable::SetFilterBitmap(
    /* [in] */ Boolean filter)
{
    return NOERROR;
}

ECode Drawable::SetCallback(
    /* [in] */ IDrawableCallback* cb)
{
    mCallback = NULL;
    if (cb != NULL) {
        IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(cb);
        assert(wrs != NULL && "IWeakReferenceSource was not implemented by DrawableCallback!");
        wrs->GetWeakReference((IWeakReference**)&mCallback);
    }

    return NOERROR;
}

ECode Drawable::GetCallback(
    /* [out] */ IDrawableCallback** callback)
{
    VALIDATE_NOT_NULL(callback);
    *callback = NULL;
    if (mCallback != NULL) {
        mCallback->Resolve(EIID_IDrawableCallback, (IInterface**)&callback);
    }
    return NOERROR;
}

ECode Drawable::InvalidateSelf()
{
    AutoPtr<IDrawableCallback> callback;
    GetCallback((IDrawableCallback**)&callback);
    if (callback != NULL) {
        callback->InvalidateDrawable(this);
    }

    return NOERROR;
}

ECode Drawable::ScheduleSelf(
    /* [in] */ IRunnable* what,
    /* [in] */ Int64 when)
{
    AutoPtr<IDrawableCallback> callback;
    GetCallback((IDrawableCallback**)&callback);
    if (callback != NULL) {
        callback->ScheduleDrawable(this, what, when);
    }
    return NOERROR;
}

ECode Drawable::UnscheduleSelf(
    /* [in] */ IRunnable* what)
{
    AutoPtr<IDrawableCallback> callback;
    GetCallback((IDrawableCallback**)&callback);
    if (callback != NULL) {
        callback->UnscheduleDrawable(this, what);
    }
    return NOERROR;
}

ECode Drawable::GetLayoutDirection(
    /* [out] */ Int32* dir)
{
    VALIDATE_NOT_NULL(dir);
    *dir = mLayoutDirection;
    return NOERROR;
}

ECode Drawable::SetLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    Int32 dir = 0;
    if ((GetLayoutDirection(&dir), dir) != layoutDirection) {
        mLayoutDirection = layoutDirection;
    }

    return NOERROR;
}

ECode Drawable::GetAlpha(
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = 0xFF;
    return NOERROR;
}

ECode Drawable::SetXfermode(
    /* [in] */ IXfermode* mode)
{
    // Base implementation drops it on the floor for compatibility. Whee!
    // TODO: For this to be included in the API proper, all framework drawables need impls.
    // For right now only BitmapDrawable has it.
    return NOERROR;
}

ECode Drawable::SetTint(
    /* [in] */ Int32 tint)
{
    AutoPtr<IColorStateList> list;
    CColorStateList::ValueOf(tint, (IColorStateList**)&list);
    return SetTintList(list);
}

ECode Drawable::SetTintList(
    /* [in] */ IColorStateList* tint)
{
    return NOERROR;
}

ECode Drawable::SetTintMode(
    /* [in] */ PorterDuffMode tintMode)
{
    return NOERROR;
}

ECode Drawable::GetColorFilter(
    /* [out] */ IColorFilter** filter)
{
    VALIDATE_NOT_NULL(filter);
    *filter = NULL;
    return NOERROR;
}

ECode Drawable::SetColorFilter(
    /* [in] */ Int32 color,
    /* [in] */ PorterDuffMode mode)
{
    AutoPtr<IPorterDuffColorFilter> filter;
    CPorterDuffColorFilter::New(color, mode, (IPorterDuffColorFilter**)&filter);
    return SetColorFilter(IColorFilter::Probe(filter));
}

ECode Drawable::ClearColorFilter()
{
    return SetColorFilter(NULL);
}

ECode Drawable::SetHotspot(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return NOERROR;
}

ECode Drawable::SetHotspotBounds(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return NOERROR;
}

ECode Drawable::GetHotspotBounds(
    /* [in] */ IRect* outRect)
{
    AutoPtr<IRect> bounds;
    GetBounds((IRect**)&bounds);
    return outRect->Set(bounds);
}

ECode Drawable::IsProjected(
    /* [out] */ Boolean* projected)
{
    VALIDATE_NOT_NULL(projected);
    *projected = FALSE;
    return NOERROR;
}

ECode Drawable::IsStateful(
    /* [out] */ Boolean* isStateful)
{
    VALIDATE_NOT_NULL(isStateful);
    *isStateful = FALSE;
    return NOERROR;
}

ECode Drawable::SetState(
    /* [in] */ ArrayOf<Int32>* stateSet,
    /* [out] */ Boolean* isStateful)
{
    VALIDATE_NOT_NULL(isStateful);
    Boolean isEqual = TRUE;

    if (mStateSet.Get() == stateSet) {
        isEqual = TRUE;
    }
    else if (mStateSet == NULL || stateSet == NULL
            || mStateSet->GetLength() != stateSet->GetLength()) {
        isEqual = FALSE;
    }
    else {
        Int32 len = mStateSet->GetLength();
        for (Int32 i = 0; i < len; i++) {
            if ((*mStateSet)[i] != (*stateSet)[i]) {
                isEqual = FALSE;
            }
        }
    }

    if (!isEqual) {
        mStateSet = stateSet;
        *isStateful = OnStateChange(stateSet);
        return NOERROR;
    }
    *isStateful = FALSE;
    return NOERROR;
}

ECode Drawable::GetState(
    /* [out, callee] */ ArrayOf<Int32>** stateSet)
{
    VALIDATE_NOT_NULL(stateSet);
    *stateSet = mStateSet;
    REFCOUNT_ADD(*stateSet);
    return NOERROR;
}

ECode Drawable::JumpToCurrentState()
{
    return NOERROR;
}

ECode Drawable::GetCurrent(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = this;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode Drawable::SetLevel(
    /* [in] */ Int32 level,
    /* [out] */ Boolean* change)
{
    VALIDATE_NOT_NULL(change);
    if (mLevel != level) {
        mLevel = level;
        *change = OnLevelChange(level);
        return NOERROR;
    }

    *change = FALSE;
    return NOERROR;
}

ECode Drawable::GetLevel(
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level);
    *level = mLevel;
    return NOERROR;
}

ECode Drawable::SetVisible(
    /* [in] */ Boolean visible,
    /* [in] */ Boolean restart,
    /* [out] */ Boolean* isDifferent)
{
    VALIDATE_NOT_NULL(isDifferent);
    Boolean changed = mVisible != visible;
    if (changed) {
        mVisible = visible;
        InvalidateSelf();
    }
    *isDifferent = changed;
    return NOERROR;
}

ECode Drawable::IsVisible(
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible);
    *visible = mVisible;
    return NOERROR;
}

ECode Drawable::SetAutoMirrored(
    /* [in] */ Boolean mirrored)
{
    return NOERROR;
}

ECode Drawable::IsAutoMirrored(
    /* [out] */ Boolean* mirrored)
{
    VALIDATE_NOT_NULL(mirrored);
    *mirrored = FALSE;
    return NOERROR;
}

ECode Drawable::ApplyTheme(
    /* [in] */ /*@SuppressWarnings("unused")*/ IResourcesTheme* t)
{
    return NOERROR;
}

ECode Drawable::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    *can = FALSE;
    return NOERROR;
}

ECode Drawable::ResolveOpacity(
    /* [in] */ Int32 op1,
    /* [in] */ Int32 op2,
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    if (op1 == op2) {
        *opacity = op1;
        return NOERROR;
    }
    if (op1 == IPixelFormat::UNKNOWN || op2 == IPixelFormat::UNKNOWN) {
        *opacity = IPixelFormat::UNKNOWN;
        return NOERROR;
    }
    if (op1 == IPixelFormat::TRANSLUCENT || op2 == IPixelFormat::TRANSLUCENT) {
        *opacity = IPixelFormat::TRANSLUCENT;
        return NOERROR;
    }
    if (op1 == IPixelFormat::TRANSPARENT || op2 == IPixelFormat::TRANSPARENT) {
        *opacity = IPixelFormat::TRANSPARENT;
        return NOERROR;
    }
    *opacity = IPixelFormat::OPAQUE;
    return NOERROR;
}

ECode Drawable::GetTransparentRegion(
    /* [out] */ IRegion** bounds)
{
    VALIDATE_NOT_NULL(bounds);
    *bounds = NULL;
    return NOERROR;
}

Boolean Drawable::OnStateChange(
    /* [in] */ const ArrayOf<Int32>* state)
{
    return FALSE;
}

Boolean Drawable::OnLevelChange(
    /* [in] */ Int32 level)
{
    return FALSE;
}

void Drawable::OnBoundsChange(
    /* [in] */ IRect* bounds)
{
}

ECode Drawable::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = -1;
    return NOERROR;
}

ECode Drawable::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = -1;
    return NOERROR;
}

ECode Drawable::GetMinimumWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    Int32 intrinsicWidth = 0;
    GetIntrinsicWidth(&intrinsicWidth);
    *width = intrinsicWidth > 0 ? intrinsicWidth : 0;
    return NOERROR;
}

ECode Drawable::GetMinimumHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    Int32 intrinsicHeight = 0;
    GetIntrinsicHeight(&intrinsicHeight);
    *height = intrinsicHeight > 0 ? intrinsicHeight : 0;
    return NOERROR;
}

ECode Drawable::GetPadding(
    /* [in, out] */ IRect* padding,
    /* [out] */ Boolean* isPadding)
{
    assert(padding != NULL);
    VALIDATE_NOT_NULL(isPadding);
    padding->Set(0, 0, 0, 0);
    *isPadding = FALSE;
    return NOERROR;
}

ECode Drawable::GetOpticalInsets(
    /* [out] */ IInsets** sets)
{
    VALIDATE_NOT_NULL(sets);
    *sets = Insets::NONE;
    REFCOUNT_ADD(*sets);
    return NOERROR;
}

ECode Drawable::GetOutline(
    /* [in] */ /*@NonNull*/ IOutline* outline)
{
    AutoPtr<IRect> bounds;
    GetBounds((IRect**)&bounds);
    outline->SetRect(bounds);
    return outline->SetAlpha(0);
}

ECode Drawable::Mutate()
{
    return NOERROR;
}

ECode Drawable::CreateFromStream(
    /* [in] */ IInputStream* is,
    /* [in] */ const String& srcName,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);

    // Trace.traceBegin(Trace.TRACE_TAG_RESOURCES, srcName != null ? srcName : "Unknown drawable");
    // try {
    return CreateFromResourceStream(NULL, NULL, is, srcName, drawable);
    // } finally {
    //     Trace.traceEnd(Trace.TRACE_TAG_RESOURCES);
    // }
}

ECode Drawable::CreateFromResourceStream(
    /* [in] */ IResources* res,
    /* [in] */ ITypedValue* value,
    /* [in] */ IInputStream* is,
    /* [in] */ const String& srcName,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);

    // Trace.traceBegin(Trace.TRACE_TAG_RESOURCES, srcName != null ? srcName : "Unknown drawable");
    // try {
    return CreateFromResourceStream(res, value, is, srcName, NULL, drawable);
    // } finally {
    //     Trace.traceEnd(Trace.TRACE_TAG_RESOURCES);
    // }
}

ECode Drawable::CreateFromResourceStream(
    /* [in] */ IResources* res,
    /* [in] */ ITypedValue* value,
    /* [in] */ IInputStream* is,
    /* [in] */ const String& srcName,
    /* [in] */ IBitmapFactoryOptions* _opts,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = NULL;

    if (is == NULL) {
        return NOERROR;
    }

    /*  ugh. The decodeStream contract is that we have already allocated
        the pad rect, but if the bitmap does not had a ninepatch chunk,
        then the pad will be ignored. If we could change this to lazily
        alloc/assign the rect, we could avoid the GC churn of making new
        Rects only to drop them on the floor.
    */
    AutoPtr<IRect> pad;
    CRect::New((IRect**)&pad);

    // Special stuff for compatibility mode: if the target density is not
    // the same as the display density, but the resource -is- the same as
    // the display density, then don't scale it down to the target density.
    // This allows us to load the system's density-correct resources into
    // an application in compatibility mode, without scaling those down
    // to the compatibility density only to have them scaled back up when
    // drawn to the screen.
    AutoPtr<IBitmapFactoryOptions> opts = _opts;
    if (opts == NULL) {
        CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&opts);
    }

    if (res != NULL) {
        AutoPtr<IDisplayMetrics> dis;
        res->GetDisplayMetrics((IDisplayMetrics**)&dis);
        assert(dis != NULL);
        Int32 dpi;
        dis->GetNoncompatDensityDpi(&dpi);
        opts->SetInScreenDensity(dpi);
    }
    else {
        opts->SetInScreenDensity(CDisplayMetrics::DENSITY_DEVICE);
    }

    AutoPtr<IBitmap> bm;
    FAIL_RETURN(BitmapFactory::DecodeResourceStream(
            res, value, is, pad, opts, (IBitmap**)&bm));
    if (bm != NULL) {
        AutoPtr< ArrayOf<Byte> > np;
        FAIL_RETURN(bm->GetNinePatchChunk((ArrayOf<Byte>**)&np));
        if (np == NULL || !NinePatch::IsNinePatchChunk(np)) {
            np = NULL;
            pad = NULL;
        }

        AutoPtr<IRect> opticalInsets;
        CRect::New((IRect**)&opticalInsets);
        bm->GetOpticalInsets(opticalInsets);
        return DrawableFromBitmap(res, bm, np, pad, opticalInsets, srcName, drawable);
    }

    return NOERROR;
}

ECode Drawable::CreateFromXml(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    return CreateFromXml(r, parser, NULL, drawable);
}

ECode Drawable::CreateFromXml(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = NULL;

    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(parser);

    Int32 type;
    FAIL_RETURN(parser->Next(&type));
    while (type != IXmlPullParser::START_TAG && type != IXmlPullParser::END_DOCUMENT) {
        // Empty loop
        FAIL_RETURN(parser->Next(&type));
    }

    if (type != IXmlPullParser::START_TAG) {
//        throw new XmlPullParserException("No start tag found");
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    FAIL_RETURN(CreateFromXmlInner(r, parser, attrs, theme, drawable));

    if (*drawable == NULL) {
//        throw new RuntimeException("Unknown initial tag: " + parser.getName());
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

ECode Drawable::CreateFromXmlInner(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    return CreateFromXmlInner(r, parser, attrs, NULL, drawable);
}

ECode Drawable::CreateFromXmlInner(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = NULL;

    String name;
    FAIL_RETURN(parser->GetName((String*)&name));

    if (name.Equals("selector")) {
        FAIL_RETURN(CStateListDrawable::New(drawable));
    }
    else if (name.Equals("level-list")) {
        FAIL_RETURN(CLevelListDrawable::New(drawable));
    }
    else if (name.Equals("animated-selector")) {
        FAIL_RETURN(CAnimatedStateListDrawable::New(drawable));
    }
    else if (name.Equals("layer-list")) {
        FAIL_RETURN(CLayerDrawable::New(drawable));
    }
    else if (name.Equals("transition")) {
        FAIL_RETURN(CTransitionDrawable::New(drawable));
    }
    else if (name.Equals("ripple")) {
        CRippleDrawable::New(drawable);
    }
    else if (name.Equals("color")) {
        FAIL_RETURN(CColorDrawable::New(drawable));
    }
    else if (name.Equals("shape")) {
        FAIL_RETURN(CGradientDrawable::New(drawable));
    }
    else if (name.Equals("vector")) {
        CVectorDrawable::New(drawable);
    }
    else if (name.Equals("animated-vector")) {
        CAnimatedVectorDrawable::New(drawable);
    }
    else if (name.Equals("scale")) {
        FAIL_RETURN(CScaleDrawable::New(drawable));
    }
    else if (name.Equals("clip")) {
        FAIL_RETURN(CClipDrawable::New(drawable));
    }
    else if (name.Equals("rotate")) {
        FAIL_RETURN(CRotateDrawable::New(drawable));;
    }
    else if (name.Equals("animated-rotate")) {
        FAIL_RETURN(CAnimatedRotateDrawable::New(drawable));
    }
    else if (name.Equals("animation-list")) {
        FAIL_RETURN(CAnimationDrawable::New(drawable));
    }
    else if (name.Equals("inset")) {
        FAIL_RETURN(CInsetDrawable::New(drawable));
    }
    else if (name.Equals("bitmap")) {
        //noinspection deprecation
        FAIL_RETURN(CBitmapDrawable::New(r, drawable));
        if (r != NULL) {
            AutoPtr<IDisplayMetrics> metrics;
            r->GetDisplayMetrics((IDisplayMetrics**)&metrics);
            IBitmapDrawable::Probe(*drawable)->SetTargetDensity(metrics);
        }
    }
    else if (name.Equals("nine-patch")) {
        FAIL_RETURN(CNinePatchDrawable::New(drawable));
        if (r != NULL) {
            AutoPtr<IDisplayMetrics> metrics;
            r->GetDisplayMetrics((IDisplayMetrics**)&metrics);
            INinePatchDrawable::Probe(*drawable)->SetTargetDensity(metrics);
        }
    }
     else {
//        throw new XmlPullParserException(parser.getPositionDescription() +
//                ": invalid drawable tag " + name);
        Logger::E("Drawable", "E_XML_PULL_PARSER_EXCEPTION: invalid drawable tag %s", name.string());
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    return (*drawable)->Inflate(r, parser, attrs, theme);
}

ECode Drawable::CreateFromPath(
    /* [in] */ const String& pathName,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = NULL;

    if (pathName.IsNull()) {
        return NOERROR;
    }

    // Trace.traceBegin(Trace.TRACE_TAG_RESOURCES, pathName);
    // try {
    AutoPtr<IBitmap> bm;
    FAIL_RETURN(BitmapFactory::DecodeFile(pathName, (IBitmap**)&bm));
    if (bm != NULL) {
        return DrawableFromBitmap(NULL, bm, NULL, NULL, NULL, pathName, drawable);
    }
    // } finally {
    //     Trace.traceEnd(Trace.TRACE_TAG_RESOURCES);
    // }

    return NOERROR;
}

ECode Drawable::ParseTintMode(
    /* [in] */ Int32 value,
    /* [in] */ PorterDuffMode defaultMode,
    /* [out] */ PorterDuffMode* mode)
{
    VALIDATE_NOT_NULL(mode);
    switch (value) {
        case 3: *mode = PorterDuffMode_SRC_OVER;
        case 5: *mode = PorterDuffMode_SRC_IN;
        case 9: *mode = PorterDuffMode_SRC_ATOP;
        case 14: *mode = PorterDuffMode_MULTIPLY;
        case 15: *mode = PorterDuffMode_SCREEN;
        case 16: *mode = PorterDuffMode_ADD;
        default: *mode = defaultMode;
    }
    return NOERROR;
}

ECode Drawable::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs)
{
    return Inflate(r, parser, attrs, NULL);
}

ECode Drawable::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::Drawable),
            ArraySize(R::styleable::Drawable));

    AutoPtr<ITypedArray> a;
    if (theme != NULL) {
        FAIL_RETURN(theme->ObtainStyledAttributes(
            attrs, attrIds, 0, 0, (ITypedArray**)&a));
    } else {
        FAIL_RETURN(r->ObtainAttributes(attrs, attrIds, (ITypedArray**)&a));
    }

    FAIL_RETURN(InflateWithAttributes(r, parser, a, R::styleable::Drawable_visible));
    return a->Recycle();
}

ECode Drawable::InflateWithAttributes(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ ITypedArray* attrs,
    /* [in] */ Int32 visibleAttr)
{
    return attrs->GetBoolean(visibleAttr, mVisible, &mVisible);
}

ECode Drawable::GetConstantState(
    /* [out] */ IDrawableConstantState** state)
{
    VALIDATE_NOT_NULL(state);
    *state = NULL;
    return NOERROR;
}

ECode Drawable::DrawableFromBitmap(
    /* [in] */ IResources* res,
    /* [in] */ IBitmap* bm,
    /* [in] */ ArrayOf<Byte>* np,
    /* [in] */ IRect* pad,
    /* [in] */ IRect* layoutBounds,
    /* [in] */ const String& srcName,
    /* [out] */ IDrawable** drawable)
{
    assert(drawable != NULL);
    ECode ec = NOERROR;
    AutoPtr<IDrawable> dr;
    if (np != NULL) {
        return CNinePatchDrawable::New(
            res, bm, np, pad, layoutBounds, srcName, drawable);
    }

    return CBitmapDrawable::New(res, bm, drawable);
}

AutoPtr<IPorterDuffColorFilter> Drawable::UpdateTintFilter(
    /* [in] */ IPorterDuffColorFilter* tintFilter,
    /* [in] */ IColorStateList* tint,
    /* [in] */ PorterDuffMode tintMode)
{
    if (tint == NULL || tintMode == PorterDuffMode_NONE) {
        return NULL;
    }

    AutoPtr<ArrayOf<Int32> > state;
    GetState((ArrayOf<Int32>**)&state);
    Int32 color = 0;
    tint->GetColorForState(state, IColor::TRANSPARENT, &color);
    if (tintFilter == NULL) {
        AutoPtr<IPorterDuffColorFilter> filter;
        CPorterDuffColorFilter::New(color, tintMode, (IPorterDuffColorFilter**)&filter);
        return filter;
    }

    ((CPorterDuffColorFilter*)tintFilter)->SetColor(color);
    ((CPorterDuffColorFilter*)tintFilter)->SetMode(tintMode);
    return tintFilter;
}

ECode Drawable::ObtainAttributes(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ IAttributeSet* set,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** a)
{
    if (theme == NULL) {
        return res->ObtainAttributes(set, attrs, a);
    }
    return theme->ObtainStyledAttributes(set, attrs, 0, 0, a);
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
