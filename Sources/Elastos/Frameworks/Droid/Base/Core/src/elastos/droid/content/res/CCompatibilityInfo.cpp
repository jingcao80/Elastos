
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/content/res/CCompatibilityInfo.h"
#include "elastos/droid/content/pm/CApplicationInfo.h"
#include "elastos/droid/graphics/CPoint.h"
#include "elastos/droid/graphics/CPointF.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRegion.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::CApplicationInfo;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::CPointF;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRegion;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Utility::Logging::Logger;


namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

static const String TAG("CCompatibilityInfo");

//===============================================================
// CCompatibilityInfo::Translator
//===============================================================
CAR_INTERFACE_IMPL(CCompatibilityInfo::Translator, Object, ICompatibilityInfoTranslator)

CCompatibilityInfo::Translator::Translator(
    /* [in] */ Float applicationScale,
    /* [in] */ Float applicationInvertedScale)
    : mApplicationScale(applicationScale)
    , mApplicationInvertedScale(applicationInvertedScale)
{}

CCompatibilityInfo::Translator::Translator()
    : mApplicationScale(1.0)
    , mApplicationInvertedScale(1.0)
{}

ECode CCompatibilityInfo::Translator::TranslateRectInScreenToAppWinFrame(
    /* [in] */ IRect* rect)
{
    return rect->Scale(mApplicationInvertedScale);
}

ECode CCompatibilityInfo::Translator::TranslateRegionInWindowToScreen(
    /* [in] */ IRegion* transparentRegion)
{
    return transparentRegion->Scale(mApplicationScale);
}

/**
* Apply translation to the canvas that is necessary to draw the content.
*/
ECode CCompatibilityInfo::Translator::TranslateCanvas(
    /* [in] */ ICanvas* canvas)
{
    if (mApplicationScale == 1.5f) {
        /*  When we scale for compatibility, we can put our stretched
        bitmaps and ninepatches on exacty 1/2 pixel boundaries,
        which can give us inconsistent drawing due to imperfect
        Float precision in the graphics engine's inverse matrix.

        As a work-around, we translate by a tiny amount to avoid
        landing on exact pixel centers and boundaries, giving us
        the slop we need to draw consistently.

        This constant is meant to resolve to 1/255 after it is
        scaled by 1.5 (mApplicationScale). Note, this is just a guess
        as to what is small enough not to create its own artifacts,
        and big enough to avoid the precision problems. Feel free
        to experiment with smaller values as you choose.
        */
        Float tinyOffset = 2.0f / (3 * 255);
        canvas->Translate(tinyOffset, tinyOffset);
    }

    return canvas->Scale(mApplicationScale, mApplicationScale);
}

ECode CCompatibilityInfo::Translator::TranslateEventInScreenToAppWindow(
    /* [in] */ IMotionEvent* event)
{
    return event->Scale(mApplicationInvertedScale);
}

/**
* Translate the window's layout parameter, from application's view to
* Screen's view.
*/
ECode CCompatibilityInfo::Translator::TranslateWindowLayout(
    /* [in] */ IWindowManagerLayoutParams* params)
{
    return params->Scale(mApplicationScale);
}

/**
* Translate a Rect in application's window to screen.
*/
ECode CCompatibilityInfo::Translator::TranslateRectInAppWindowToScreen(
    /* [in] */ IRect* rect)
{
    return rect->Scale(mApplicationScale);
}

ECode CCompatibilityInfo::Translator::TranslateRectInScreenToAppWindow(
    /* [in] */ IRect* rect)
{
    return rect->Scale(mApplicationInvertedScale);
}

ECode CCompatibilityInfo::Translator::TranslatePointInScreenToAppWindow(
    /* [in] */ IPointF* point)
{
    Float scale = mApplicationInvertedScale;
    if (scale != 1.0f) {
        AutoPtr<CPointF> cls = (CPointF*)point;
        cls->mX = cls->mX * scale;
        cls->mY = cls->mY * scale;
    }

    return NOERROR;
}

ECode CCompatibilityInfo::Translator::TranslateLayoutParamsInAppWindowToScreen(
   /* [in] */ IWindowManagerLayoutParams* params)
{
    return params->Scale(mApplicationScale);
}

ECode CCompatibilityInfo::Translator::GetTranslatedContentInsets(
    /* [in] */ IRect* contentInsets,
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);
    *rect = NULL;

    if (mContentInsetsBuffer == NULL) {
        FAIL_RETURN(CRect::New((IRect**)&mContentInsetsBuffer));
    }

    mContentInsetsBuffer->Set(contentInsets);
    TranslateRectInAppWindowToScreen(mContentInsetsBuffer);
    *rect = mContentInsetsBuffer;
    REFCOUNT_ADD(*rect);

    return NOERROR;
}

ECode CCompatibilityInfo::Translator::GetTranslatedVisibleInsets(
    /* [in] */ IRect* visibleInsets,
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);
    *rect = NULL;

    if (mVisibleInsetsBuffer == NULL) {
        FAIL_RETURN(CRect::New((IRect**)&mVisibleInsetsBuffer));
    }

    mVisibleInsetsBuffer->Set(visibleInsets);
    TranslateRectInAppWindowToScreen(mVisibleInsetsBuffer);
    *rect = mVisibleInsetsBuffer;
    REFCOUNT_ADD(*rect);

    return NOERROR;
}

ECode CCompatibilityInfo::Translator::GetApplicationScale(
    /* [out] */ Float* appScale)
{
    VALIDATE_NOT_NULL(appScale);
    *appScale = mApplicationScale;
    return NOERROR;
}

ECode CCompatibilityInfo::Translator::GetApplicationInvertedScale(
    /* [out] */ Float* appInvertedScale)
{
    VALIDATE_NOT_NULL(appInvertedScale);
    *appInvertedScale = mApplicationInvertedScale;
    return NOERROR;
}

ECode CCompatibilityInfo::Translator::GetTranslatedTouchableArea(
    /* [in] */ IRegion* touchableArea,
    /* [out] */ IRegion** region)
{
    VALIDATE_NOT_NULL(region);

    if (mTouchableAreaBuffer == NULL) {
        FAIL_RETURN(CRegion::New((IRegion**)&mTouchableAreaBuffer));
    }
    Boolean result;
    mTouchableAreaBuffer->Set(touchableArea, &result);
    mTouchableAreaBuffer->Scale(mApplicationScale);
    *region = mTouchableAreaBuffer;
    REFCOUNT_ADD(*region);
    return NOERROR;
}

//===============================================================
// CCompatibilityInfo
//===============================================================
static AutoPtr<ICompatibilityInfo> InitDefaultCompatibilityInfo()
{
    AutoPtr<ICompatibilityInfo> info;
    ASSERT_SUCCEEDED(CCompatibilityInfo::New((ICompatibilityInfo**)&info));
    return info;
}

INIT_PROI_3 const AutoPtr<ICompatibilityInfo> CCompatibilityInfo::DEFAULT_COMPATIBILITY_INFO
        = InitDefaultCompatibilityInfo();

const Int32 CCompatibilityInfo::SCALING_REQUIRED = 1;
const Int32 CCompatibilityInfo::ALWAYS_NEEDS_COMPAT = 2;
const Int32 CCompatibilityInfo::NEVER_NEEDS_COMPAT = 4;
const Int32 CCompatibilityInfo::NEEDS_SCREEN_COMPAT = 8;

CAR_INTERFACE_IMPL_2(CCompatibilityInfo, Object, ICompatibilityInfo, IParcelable)

CAR_OBJECT_IMPL(CCompatibilityInfo)

CCompatibilityInfo::CCompatibilityInfo()
    : mApplicationDensity(0)
    , mApplicationScale(1.0)
    , mApplicationInvertedScale(1.0)
    , mIsThemeable(FALSE)
    , mCompatibilityFlags(0)
{}

ECode CCompatibilityInfo::IsScalingRequired(
    /* [out] */ Boolean* isRequire)
{
    VALIDATE_NOT_NULL(isRequire);
    *isRequire = (mCompatibilityFlags & SCALING_REQUIRED) != 0;
    return NOERROR;
}

ECode CCompatibilityInfo::SupportsScreen(
    /* [out] */ Boolean* isSupport)
{
    VALIDATE_NOT_NULL(isSupport);
    *isSupport = (mCompatibilityFlags & NEEDS_SCREEN_COMPAT) == 0;
    return NOERROR;
}

ECode CCompatibilityInfo::NeverSupportsScreen(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = (mCompatibilityFlags & ALWAYS_NEEDS_COMPAT) != 0;
    return NOERROR;
}

ECode CCompatibilityInfo::AlwaysSupportsScreen(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = (mCompatibilityFlags & NEVER_NEEDS_COMPAT) != 0;
    return NOERROR;
}

ECode CCompatibilityInfo::GetTranslator(
    /* [out] */ ICompatibilityInfoTranslator** translator)
{
    VALIDATE_NOT_NULL(translator);
    *translator = NULL;

    Boolean isRequire;
    IsScalingRequired(&isRequire);
    if (isRequire) {
        *translator = (ICompatibilityInfoTranslator*)new Translator();
        REFCOUNT_ADD(*translator);
    }
    return NOERROR;
}

ECode CCompatibilityInfo::ApplyToDisplayMetrics(
    /* [in] */ IDisplayMetrics* inoutDm)
{
    Boolean isSupport;
    SupportsScreen(&isSupport);
    if (!isSupport) {
        // This is a larger screen device and the app is not
        // compatible with large screens, so diddle it.
        ComputeCompatibleScaling(inoutDm, inoutDm);
    }
    else {
        Int32 noncompatWidthPixels;
        inoutDm->GetNoncompatWidthPixels(&noncompatWidthPixels);
        inoutDm->SetWidthPixels(noncompatWidthPixels);

        Int32 noncompatHeightPixels;
        inoutDm->GetNoncompatHeightPixels(&noncompatHeightPixels);
        inoutDm->SetHeightPixels(noncompatHeightPixels);
    }

    Boolean isScalingReq = FALSE;
    IsScalingRequired(&isScalingReq);
    if (isScalingReq) {
        Float invertedRatio = mApplicationInvertedScale;
        Float noncompatDensity;
        inoutDm->GetNoncompatDensity(&noncompatDensity);
        inoutDm->SetDensity(noncompatDensity * invertedRatio);

        Int32 noncompatDensityDpi = 0;
        inoutDm->GetNoncompatDensityDpi(&noncompatDensityDpi);
        inoutDm->SetDensityDpi((Int32)((noncompatDensityDpi * invertedRatio) + .5f));

        Float noncompatScaledDensity;
        inoutDm->GetNoncompatScaledDensity(&noncompatScaledDensity);
        inoutDm->SetScaledDensity(noncompatScaledDensity * invertedRatio);

        Float noncompatXdpi;
        inoutDm->GetNoncompatXdpi(&noncompatXdpi);
        inoutDm->SetXdpi(noncompatXdpi * invertedRatio);

        Float noncompatYdpi;
        inoutDm->GetNoncompatYdpi(&noncompatYdpi);
        inoutDm->SetYdpi(noncompatYdpi * invertedRatio);

        Int32 widthPixels;
        inoutDm->GetWidthPixels(&widthPixels);
        inoutDm->SetWidthPixels((Int32)(widthPixels * invertedRatio + 0.5f));

        Int32 heightPixels;
        inoutDm->GetHeightPixels(&heightPixels);
        inoutDm->SetHeightPixels((Int32)(heightPixels * invertedRatio + 0.5f));
    }

    return NOERROR;
}

ECode CCompatibilityInfo::ApplyToConfiguration(
    /* [in] */ Int32 displayDensity,
    /* [in] */ IConfiguration* inoutConfig)
{
    Boolean isSupport;
    SupportsScreen(&isSupport);
    if (!isSupport) {
        // This is a larger screen device and the app is not
        // compatible with large screens, so we are forcing it to
        // run as if the screen is normal size.
        Int32 screenLayout;
        inoutConfig->GetScreenLayout(&screenLayout);
        inoutConfig->SetScreenLayout(
            (screenLayout & ~IConfiguration::SCREENLAYOUT_SIZE_MASK)
                | IConfiguration::SCREENLAYOUT_SIZE_NORMAL);

        Int32 compatScreenWidthDp;
        inoutConfig->GetCompatScreenWidthDp(&compatScreenWidthDp);
        inoutConfig->SetScreenWidthDp(compatScreenWidthDp);

        Int32 compatScreenHeightDp;
        inoutConfig->GetCompatScreenHeightDp(&compatScreenHeightDp);
        inoutConfig->SetScreenHeightDp(compatScreenHeightDp);

        Int32 compatSmallestScreenWidthDp;
        inoutConfig->GetCompatSmallestScreenWidthDp(&compatSmallestScreenWidthDp);
        inoutConfig->SetSmallestScreenWidthDp(compatSmallestScreenWidthDp);
    }

    inoutConfig->SetDensityDpi(displayDensity);

    Boolean isScalingReq = FALSE;
    IsScalingRequired(&isScalingReq);
    if (isScalingReq) {
        Float invertedRatio = mApplicationInvertedScale;
        Int32 densityDpi;
        inoutConfig->GetDensityDpi(&densityDpi);
        inoutConfig->SetDensityDpi((Int32)((densityDpi * invertedRatio) + .5f));
    }

    return NOERROR;
}

Float CCompatibilityInfo::ComputeCompatibleScaling(
    /* [in] */ IDisplayMetrics* dm,
    /* [in] */ IDisplayMetrics* outDm)
{
    Int32 width, height;
    dm->GetNoncompatWidthPixels(&width);
    dm->GetNoncompatHeightPixels(&height);
    Int32 shortSize, longSize;
    if (width < height) {
        shortSize = width;
        longSize = height;
    }
    else {
        shortSize = height;
        longSize = width;
    }
    Float density;
    dm->GetDensity(&density);
    Int32 newShortSize = (Int32)(DEFAULT_NORMAL_SHORT_DIMENSION * density + 0.5f);
    Float aspect = ((Float)longSize) / shortSize;
    if (aspect > MAXIMUM_ASPECT_RATIO) {
        aspect = MAXIMUM_ASPECT_RATIO;
    }
    Int32 newLongSize = (Int32)(newShortSize * aspect + 0.5f);
    Int32 newWidth, newHeight;
    if (width < height) {
        newWidth = newShortSize;
        newHeight = newLongSize;
    }
    else {
        newWidth = newLongSize;
        newHeight = newShortSize;
    }

    Float sw = width / (Float)newWidth;
    Float sh = height / (Float)newHeight;
    Float scale = sw < sh ? sw : sh;
    if (scale < 1) {
        scale = 1;
    }

    if (outDm != NULL) {
        outDm->SetWidthPixels(newWidth);
        outDm->SetHeightPixels(newHeight);
    }

    return scale;
}

ECode CCompatibilityInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    ICompatibilityInfo* info = ICompatibilityInfo::Probe(obj);
    if (info == NULL){
        return NOERROR;
    }

    if (info == (ICompatibilityInfo*)this) {
        *result = TRUE;
        return NOERROR;
    }

    using Elastos::Core::Math;
    CCompatibilityInfo* oc = (CCompatibilityInfo*)info;
    if (mCompatibilityFlags != oc->mCompatibilityFlags) {
        return NOERROR;
    }
    if (mApplicationDensity != oc->mApplicationDensity) {
        return NOERROR;
    }
    if (mApplicationScale != oc->mApplicationScale) {
        return NOERROR;
    }
    if (mApplicationInvertedScale != oc->mApplicationInvertedScale) {
        return NOERROR;
    }
    if (mIsThemeable != oc->mIsThemeable) {
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode CCompatibilityInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mCompatibilityFlags);
    source->ReadInt32(&mApplicationDensity);
    source->ReadFloat(&mApplicationScale);
    source->ReadFloat(&mApplicationInvertedScale);
    source->ReadBoolean(&mIsThemeable);
    return NOERROR;
}

ECode CCompatibilityInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mCompatibilityFlags);
    dest->WriteInt32(mApplicationDensity);
    dest->WriteFloat(mApplicationScale);
    dest->WriteFloat(mApplicationInvertedScale);
    dest->WriteBoolean(mIsThemeable);
    return NOERROR;
}

ECode CCompatibilityInfo::GetApplicationDensity(
    /* [out] */ Int32* applicationDensity)
{
    VALIDATE_NOT_NULL(applicationDensity);
    *applicationDensity = mApplicationDensity;
    return NOERROR;
}

ECode CCompatibilityInfo::SetApplicationDensity(
    /* [in] */ Int32 applicationDensity)
{
    mApplicationDensity = applicationDensity;
    return NOERROR;
}

ECode CCompatibilityInfo::GetApplicationScale(
    /* [out] */ Float* applicationScale)
{
    VALIDATE_NOT_NULL(applicationScale);
    *applicationScale = mApplicationScale;
    return NOERROR;
}

ECode CCompatibilityInfo::SetApplicationScale(
    /* [in] */ Float applicationScale)
{
    mApplicationScale = applicationScale;
    return NOERROR;
}

ECode CCompatibilityInfo::GetApplicationInvertedScale(
    /* [out] */ Float* applicationInvertedScale)
{
    VALIDATE_NOT_NULL(applicationInvertedScale);
    *applicationInvertedScale = mApplicationInvertedScale;
    return NOERROR;
}

ECode CCompatibilityInfo::SetIsThemeable(
    /* [in] */ Boolean isThemeable)
{
    mIsThemeable = isThemeable;
    return NOERROR;
}

ECode CCompatibilityInfo::GetIsThemeable(
    /* [out] */ Boolean* isThemeable)
{
    VALIDATE_NOT_NULL(isThemeable)
    *isThemeable = mIsThemeable;
    return NOERROR;
}

ECode CCompatibilityInfo::SetApplicationInvertedScale(
    /* [in] */ Float applicationInvertedScale)
{
    mApplicationInvertedScale = applicationInvertedScale;
    return NOERROR;
}

ECode CCompatibilityInfo::constructor(
    /* [in] */ IApplicationInfo* appInfo,
    /* [in] */ Int32 screenLayout,
    /* [in] */ Int32 screenWidthDp,
    /* [in] */ Boolean forceCompat)
{
    Logger::I(TAG, " >>> CCompatibilityInfo::constructor: %s, screenLayout:%08x, screenWidthDp:%d, forceCompat:%d",
        TO_CSTR(appInfo), screenLayout, screenWidthDp, forceCompat);

    AutoPtr<CApplicationInfo> appInfoCls = (CApplicationInfo*)appInfo;
    Int32 compatFlags = 0;
    mIsThemeable = appInfoCls->mIsThemeable;

    Logger::I(TAG, "    == mRequiresSmallestWidthDp: %d", appInfoCls->mRequiresSmallestWidthDp);
    Logger::I(TAG, "    == mCompatibleWidthLimitDp: %d", appInfoCls->mCompatibleWidthLimitDp);
    Logger::I(TAG, "    == mLargestWidthLimitDp: %d", appInfoCls->mLargestWidthLimitDp);
    Logger::I(TAG, "    == mFlags: %08x", appInfoCls->mFlags);

    if (appInfoCls->mRequiresSmallestWidthDp != 0
        || appInfoCls->mCompatibleWidthLimitDp != 0
        || appInfoCls->mLargestWidthLimitDp != 0) {
        // New style screen requirements spec.
        Int32 required = appInfoCls->mRequiresSmallestWidthDp != 0
                ? appInfoCls->mRequiresSmallestWidthDp
                : appInfoCls->mCompatibleWidthLimitDp;
        if (required == 0) {
            required = appInfoCls->mLargestWidthLimitDp;
        }
        Int32 compat = appInfoCls->mCompatibleWidthLimitDp != 0
                ? appInfoCls->mCompatibleWidthLimitDp : required;
        if (compat < required) {
            compat = required;
        }
        Int32 largest = appInfoCls->mLargestWidthLimitDp;

        if (required > DEFAULT_NORMAL_SHORT_DIMENSION) {
            // For now -- if they require a size larger than the only
            // size we can do in compatibility mode, then don't ever
            // allow the app to go in to compat mode.  Trying to run
            // it at a smaller size it can handle will make it far more
            // broken than running at a larger size than it wants or
            // thinks it can handle.
            compatFlags |= NEVER_NEEDS_COMPAT;
        }
        else if (largest != 0 && screenWidthDp > largest) {
            // If the screen size is larger than the largest size the
            // app thinks it can work with, then always force it in to
            // compatibility mode.
            compatFlags |= NEEDS_SCREEN_COMPAT | ALWAYS_NEEDS_COMPAT;
        }
        else if (compat >= screenWidthDp) {
            // The screen size is something the app says it was designed
            // for, so never do compatibility mode.
            compatFlags |= NEVER_NEEDS_COMPAT;
        }
        else if (forceCompat) {
            // The app may work better with or without compatibility mode.
            // Let the user decide.
            compatFlags |= NEEDS_SCREEN_COMPAT;
        }

        // Modern apps always support densities.
        mApplicationDensity = CDisplayMetrics::DENSITY_DEVICE;
        mApplicationScale = 1.0f;
        mApplicationInvertedScale = 1.0f;
    }
    else {
        /**
         * Has the application said that its UI is expandable?  Based on the
         * <supports-screen> android:expandible in the manifest.
         */
        Int32 EXPANDABLE = 2;

        /**
         * Has the application said that its UI supports large screens?  Based on the
         * <supports-screen> android:largeScreens in the manifest.
         */
        Int32 LARGE_SCREENS = 8;

        /**
         * Has the application said that its UI supports xlarge screens?  Based on the
         * <supports-screen> android:xlargeScreens in the manifest.
         */
        Int32 XLARGE_SCREENS = 32;

        Int32 sizeInfo = 0;

        // We can't rely on the application always setting
        // FLAG_RESIZEABLE_FOR_SCREENS so will compute it based on various input.
        Boolean anyResizeable = FALSE;

        if ((appInfoCls->mFlags & IApplicationInfo::FLAG_SUPPORTS_LARGE_SCREENS) != 0) {
            Logger::I(TAG, "    == LARGE_SCREENS");
            sizeInfo |= LARGE_SCREENS;
            anyResizeable = TRUE;
            if (!forceCompat) {
                // If we aren't forcing the app into compatibility mode, then
                // assume if it supports large screens that we should allow it
                // to use the full space of an xlarge screen as well.
                sizeInfo |= XLARGE_SCREENS | EXPANDABLE;
            }
        }

        if ((appInfoCls->mFlags & IApplicationInfo::FLAG_SUPPORTS_XLARGE_SCREENS) != 0) {
            Logger::I(TAG, "    == XLARGE_SCREENS");
            anyResizeable = TRUE;
            if (!forceCompat) {
                sizeInfo |= XLARGE_SCREENS | EXPANDABLE;
            }
        }

        if ((appInfoCls->mFlags & IApplicationInfo::FLAG_RESIZEABLE_FOR_SCREENS) != 0) {
            Logger::I(TAG, "    == FOR_SCREENS");
            anyResizeable = TRUE;
            sizeInfo |= EXPANDABLE;
        }

        if (forceCompat) {
            // If we are forcing compatibility mode, then ignore an app that
            // just says it is resizable for screens.  We'll only have it fill
            // the screen if it explicitly says it supports the screen size we
            // are running in.
            sizeInfo &= ~EXPANDABLE;
        }

        compatFlags |= NEEDS_SCREEN_COMPAT;
        switch (screenLayout & IConfiguration::SCREENLAYOUT_SIZE_MASK) {
        case IConfiguration::SCREENLAYOUT_SIZE_XLARGE:
            Logger::I(TAG, "    == SCREENLAYOUT_SIZE_XLARGE");
            if ((sizeInfo & XLARGE_SCREENS) != 0) {
                compatFlags &= ~NEEDS_SCREEN_COMPAT;
            }
            if ((appInfoCls->mFlags & IApplicationInfo::FLAG_SUPPORTS_XLARGE_SCREENS) != 0) {
                compatFlags |= NEVER_NEEDS_COMPAT;
            }
            break;
        case IConfiguration::SCREENLAYOUT_SIZE_LARGE:
            Logger::I(TAG, "    == SCREENLAYOUT_SIZE_LARGE");
            if ((sizeInfo & LARGE_SCREENS) != 0) {
                compatFlags &= ~NEEDS_SCREEN_COMPAT;
            }
            if ((appInfoCls->mFlags & IApplicationInfo::FLAG_SUPPORTS_LARGE_SCREENS) != 0) {
                compatFlags |= NEVER_NEEDS_COMPAT;
            }
            break;
        }

        if ((screenLayout & IConfiguration::SCREENLAYOUT_COMPAT_NEEDED) != 0) {
            Logger::I(TAG, "    == SCREENLAYOUT_COMPAT_NEEDED");
            if ((sizeInfo & EXPANDABLE) != 0) {
                compatFlags &= ~NEEDS_SCREEN_COMPAT;
            }
            else if (!anyResizeable) {
                compatFlags |= ALWAYS_NEEDS_COMPAT;
            }
        }
        else {
            compatFlags &= ~NEEDS_SCREEN_COMPAT;
            compatFlags |= NEVER_NEEDS_COMPAT;
        }

        if ((appInfoCls->mFlags & IApplicationInfo::FLAG_SUPPORTS_SCREEN_DENSITIES) != 0) {
            mApplicationDensity = CDisplayMetrics::DENSITY_DEVICE;
            mApplicationScale = 1.0f;
            mApplicationInvertedScale = 1.0f;
        }
        else {
            Logger::I(TAG, " CDisplayMetrics::DENSITY_DEVICE: %d, DENSITY_DEFAULT: %d",
                CDisplayMetrics::DENSITY_DEVICE, IDisplayMetrics::DENSITY_DEFAULT);
            mApplicationDensity = IDisplayMetrics::DENSITY_DEFAULT;
            mApplicationScale = CDisplayMetrics::DENSITY_DEVICE
                    / (Float) IDisplayMetrics::DENSITY_DEFAULT;
            mApplicationInvertedScale = 1.0f / mApplicationScale;
            compatFlags |= SCALING_REQUIRED;

            Logger::I(TAG, " >>>> TODO constructor mApplicationScale: %.2f, mApplicationInvertedScale: %.2f",
                mApplicationScale, mApplicationInvertedScale);

            if (mApplicationScale > 1.0f) {
                Logger::I(TAG, "========================= TODO delete this =========================");

                compatFlags = NEVER_NEEDS_COMPAT;
                mApplicationDensity = CDisplayMetrics::DENSITY_DEVICE;
                mApplicationScale = 1.0f;
                mApplicationInvertedScale = 1.0f;
                mIsThemeable = TRUE;
            }
        }
    }

    mCompatibilityFlags = compatFlags;

    return NOERROR;
}

ECode CCompatibilityInfo::constructor(
    /* [in] */ Int32 compFlags,
    /* [in] */ Int32 dens,
    /* [in] */ Float scale,
    /* [in] */ Float invertedScale,
    /* [in] */ Boolean isThemeable)
{
    mCompatibilityFlags = compFlags;
    mApplicationDensity = dens;
    mApplicationScale = scale;
    mApplicationInvertedScale = invertedScale;
    mIsThemeable = isThemeable;
    return NOERROR;
}

ECode CCompatibilityInfo::constructor()
{
    return constructor(NEVER_NEEDS_COMPAT, CDisplayMetrics::DENSITY_DEVICE, 1.0f, 1.0f, TRUE);
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos
