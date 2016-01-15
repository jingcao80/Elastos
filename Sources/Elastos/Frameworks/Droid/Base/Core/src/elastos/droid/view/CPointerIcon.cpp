
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/CPointerIcon.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Utility::Logging::Logger;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace View {

const char* CPointerIcon::TAG = "PointerIcon";
const Int32 CPointerIcon::STYLE_OEM_FIRST;
const Int32 CPointerIcon::STYLE_DEFAULT;
AutoPtr<IPointerIcon> CPointerIcon::gNullIcon = CPointerIcon::CreateNullIcon();

CAR_INTERFACE_IMPL(CPointerIcon, Object, IPointerIcon)

CAR_OBJECT_IMPL(CPointerIcon)

CPointerIcon::CPointerIcon()
    : mStyle(0)
    , mSystemIconResourceId(0)
    , mHotSpotX(0.0f)
    , mHotSpotY(0.0f)
{
}

ECode CPointerIcon::constructor()
{
    return NOERROR;
}

ECode CPointerIcon::constructor(
    /* [in] */ Int32 style)
{
    mStyle = style;
    return NOERROR;
}

ECode CPointerIcon::GetNullIcon(
    /* [out] */ IPointerIcon** pointerIcon)
{
    VALIDATE_NOT_NULL(pointerIcon);
    *pointerIcon = gNullIcon;
    REFCOUNT_ADD(*pointerIcon);
    return NOERROR;
}

ECode CPointerIcon::GetDefaultIcon(
    /* [in] */ IContext* context,
    /* [out] */ IPointerIcon** pointerIcon)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(pointerIcon);
    return GetSystemIcon(context, STYLE_DEFAULT, pointerIcon);
}

ECode CPointerIcon::GetSystemIcon(
    /* [in] */ IContext* context,
    /* [in] */ Int32 style,
    /* [out] */ IPointerIcon** pointerIcon)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(pointerIcon);

    if (context == NULL) {
        Logger::E(TAG, "context must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (style == STYLE_NULL) {
        *pointerIcon = gNullIcon;
        REFCOUNT_ADD(*pointerIcon)
        return NOERROR;
    }

    Int32 styleIndex = GetSystemIconStyleIndex(style);
    if (styleIndex == 0) {
        styleIndex = GetSystemIconStyleIndex(STYLE_DEFAULT);
    }

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::Pointer),
        ArraySize(R::styleable::Pointer));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(
        NULL, attrIds, R::attr::pointerStyle, 0, (ITypedArray**)&a);
    Int32 resourceId;
    a->GetResourceId(styleIndex, -1, &resourceId);
    a->Recycle();

    if (resourceId == -1) {
        Logger::W(TAG, "Missing theme resources for pointer icon style %d", style);
        if (style == STYLE_DEFAULT) {
            *pointerIcon = gNullIcon;
            REFCOUNT_ADD(*pointerIcon)
            return NOERROR;
        }
        else {
            return GetSystemIcon(context, STYLE_DEFAULT, pointerIcon);
        }
    }

    AutoPtr<CPointerIcon> icon;
    CPointerIcon::NewByFriend(style, (CPointerIcon**)&icon);
    if ((resourceId & 0xff000000) == 0x01000000) {
        icon->mSystemIconResourceId = resourceId;
    }
    else {
        AutoPtr<IResources> r;
        context->GetResources((IResources**)&r);
        FAIL_RETURN(icon->LoadResource(context, r, resourceId));
    }

    *pointerIcon = icon.Get();
    REFCOUNT_ADD(*pointerIcon);
    return NOERROR;
}

ECode CPointerIcon::CreateCustomIcon(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Float hotSpotX,
    /* [in] */ Float hotSpotY,
    /* [out] */ IPointerIcon** pointerIcon)
{
    VALIDATE_NOT_NULL(bitmap);
    VALIDATE_NOT_NULL(pointerIcon);

    if (bitmap == NULL) {
        Logger::E(TAG, "Bitmap must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(ValidateHotSpot(bitmap, hotSpotX, hotSpotY));

    AutoPtr<CPointerIcon> icon;
    CPointerIcon::NewByFriend(STYLE_CUSTOM, (CPointerIcon**)&icon);
    icon->mBitmap = bitmap;
    icon->mHotSpotX = hotSpotX;
    icon->mHotSpotY = hotSpotY;
    *pointerIcon = icon.Get();
    REFCOUNT_ADD(*pointerIcon);
    return NOERROR;
}

ECode CPointerIcon::LoadCustomIcon(
    /* [in] */ IResources* resources,
    /* [in] */ Int32 resourceId,
    /* [out] */ IPointerIcon** pointerIcon)
{
    VALIDATE_NOT_NULL(resources);
    VALIDATE_NOT_NULL(pointerIcon);
    if (resources == NULL) {
        Logger::E(TAG, "resources must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<CPointerIcon> icon;
    CPointerIcon::New(STYLE_CUSTOM, (IPointerIcon**)&icon);
    FAIL_RETURN(icon->LoadResource(NULL, resources, resourceId));
    *pointerIcon = icon.Get();
    REFCOUNT_ADD(*pointerIcon);
    return NOERROR;
}

ECode CPointerIcon::Load(
    /* [in] */ IContext* context,
    /* [out] */ IPointerIcon** result)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (context == NULL) {
        Logger::E(TAG, "context must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mSystemIconResourceId == 0 || mBitmap != NULL) {
        *result = this;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<CPointerIcon> icon;
    CPointerIcon::NewByFriend(mStyle, (CPointerIcon**)&icon);
    icon->mSystemIconResourceId = mSystemIconResourceId;
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    icon->LoadResource(context, r, mSystemIconResourceId);
    *result = icon.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CPointerIcon::IsNullIcon(
    /* [out] */ Boolean* isNullIcon)
{
    VALIDATE_NOT_NULL(isNullIcon)
    *isNullIcon = mStyle == STYLE_NULL;
    return NOERROR;
}

ECode CPointerIcon::IsLoaded(
    /* [out] */ Boolean* isLoaded)
{
    VALIDATE_NOT_NULL(isLoaded)
    *isLoaded = mBitmap != NULL || mStyle == STYLE_NULL;
    return NOERROR;
}

ECode CPointerIcon::GetStyle(
    /* [out] */ Int32* style)
{
    VALIDATE_NOT_NULL(style);
    *style = mStyle;
    return NOERROR;
}

ECode CPointerIcon::GetBitmap(
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    FAIL_RETURN(ThrowIfIconIsNotLoaded());
    *bitmap = mBitmap;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode CPointerIcon::GetHotSpotX(
    /* [out] */ Float* hotSpotX)
{
    VALIDATE_NOT_NULL(hotSpotX);
    FAIL_RETURN(ThrowIfIconIsNotLoaded());
    *hotSpotX = mHotSpotX;
    return NOERROR;
}

ECode CPointerIcon::GetHotSpotY(
    /* [out] */ Float* hotSpotY)
{
    VALIDATE_NOT_NULL(hotSpotY);
    FAIL_RETURN(ThrowIfIconIsNotLoaded());
    *hotSpotY = mHotSpotY;
    return NOERROR;
}

ECode CPointerIcon::DescribeContents(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return 0;
}

ECode CPointerIcon::WriteToParcel(
    /* [in] */ IParcel* parcel,
    /* [in] */ Int32 flags)
{
    VALIDATE_NOT_NULL(parcel);
    parcel->WriteInt32(mStyle);

    if (mStyle != STYLE_NULL) {
        parcel->WriteInt32(mSystemIconResourceId);
        if (mSystemIconResourceId == 0) {
            assert(0);
            //--IParcelable::Probe(mBitmap)->WriteToParcel(parcel, flags);
            parcel->WriteFloat(mHotSpotX);
            parcel->WriteFloat(mHotSpotY);
        }
    }
    return NOERROR;
}

ECode CPointerIcon::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);
    source->ReadInt32(&mStyle);

    if (mStyle != STYLE_NULL) {
        source->ReadInt32(&mSystemIconResourceId);
        if (mSystemIconResourceId == 0) {
            IParcelable::Probe(mBitmap)->ReadFromParcel(source);
            source->ReadFloat(&mHotSpotX);
            source->ReadFloat(&mHotSpotY);
        }
    }
    return NOERROR;
}

ECode CPointerIcon::LoadResource(
    /* [in] */ IContext* context,
    /* [in] */ IResources* resources,
    /* [in] */ Int32 resourceId)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(resources);

    AutoPtr<IXmlResourceParser> parser;
    resources->GetXml(resourceId, (IXmlResourceParser**)&parser);
    Int32 bitmapRes;
    Float hotSpotX;
    Float hotSpotY;
    ECode ec;
    do {
        IXmlPullParser* pullParser = IXmlPullParser::Probe(parser);
        ec = XmlUtils::BeginDocument(pullParser, String("pointer-icon"));
        if (FAILED(ec))
            break;

        Int32 size = ArraySize(R::styleable::PointerIcon);
        AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
        layout->Copy(R::styleable::PointerIcon, size);

        AutoPtr<ITypedArray> a;
        ec = resources->ObtainAttributes(IAttributeSet::Probe(parser), layout, (ITypedArray**)&a);
        if (FAILED(ec))
            break;
        ec = a->GetResourceId(R::styleable::PointerIcon_bitmap, 0, &bitmapRes);
        if (FAILED(ec))
            break;
        ec = a->GetDimension(R::styleable::PointerIcon_hotSpotX, 0, &hotSpotX);
        if (FAILED(ec))
            break;
        ec = a->GetDimension(R::styleable::PointerIcon_hotSpotY, 0, &hotSpotY);
        if (FAILED(ec))
            break;
        ec = a->Recycle();
    } while(0);

    parser->Close();

    if (FAILED(ec)) {
        Logger::E(TAG, "Exception parsing pointer icon resource.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (bitmapRes == 0) {
        Logger::E(TAG, "<pointer-icon> is missing bitmap attribute.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IDrawable> drawable;
    if (NULL == context) {
        resources->GetDrawable(bitmapRes, (IDrawable**)&drawable);
    }
    else {
        context->GetDrawable(bitmapRes, (IDrawable**)&drawable);
    }

    resources->GetDrawable(bitmapRes, (IDrawable**)&drawable);
    if (drawable == NULL || !IBitmapDrawable::Probe(drawable)) {
        Logger::E(TAG, "<pointer-icon> bitmap attribute must "
             "refer to a bitmap drawable.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Set the properties now that we have successfully loaded the icon.
    mBitmap = NULL;
    IBitmapDrawable::Probe(drawable)->GetBitmap((IBitmap**)&mBitmap);
    mHotSpotX = hotSpotX;
    mHotSpotY = hotSpotY;
    return NOERROR;
}

ECode CPointerIcon::ValidateHotSpot(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ Float hotSpotX,
    /* [in] */ Float hotSpotY)
{
    VALIDATE_NOT_NULL(bitmap);
    Int32 w;
    bitmap->GetWidth(&w);
    if (hotSpotX < 0 || hotSpotX >= w) {
        Logger::E(TAG, "x hotspot lies outside of the bitmap area");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 h;
    bitmap->GetHeight(&h);
    if (hotSpotY < 0 || hotSpotY >= h) {
        Logger::E(TAG, "y hotspot lies outside of the bitmap area");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

Int32 CPointerIcon::GetSystemIconStyleIndex(
    /* [in] */ Int32 style)
{
    switch (style) {
        case STYLE_ARROW:
            return R::styleable::Pointer_pointerIconArrow;
        case STYLE_SPOT_HOVER:
            return R::styleable::Pointer_pointerIconSpotHover;
        case STYLE_SPOT_TOUCH:
            return R::styleable::Pointer_pointerIconSpotTouch;
        case STYLE_SPOT_ANCHOR:
            return R::styleable::Pointer_pointerIconSpotAnchor;
        default:
            return 0;
    }
}

AutoPtr<IPointerIcon> CPointerIcon::CreateNullIcon()
{
    AutoPtr<CPointerIcon> icon;
    CPointerIcon::NewByFriend(CPointerIcon::STYLE_NULL, (CPointerIcon**)&icon);
    return icon;
}

ECode CPointerIcon::ThrowIfIconIsNotLoaded()
{
    assert(0);
    Boolean isLoaded = FALSE;
    IsLoaded(&isLoaded);
    if (!isLoaded) {
        //throw new IllegalStateException("The icon is not loaded.");
        Logger::E(TAG, "The icon is not loaded.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
