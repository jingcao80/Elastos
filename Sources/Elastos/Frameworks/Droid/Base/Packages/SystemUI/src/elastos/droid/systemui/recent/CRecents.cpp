#include "elastos/droid/systemui/recent/CRecents.h"
#include "elastos/droid/systemui/recent/RecentsActivity.h"
#include "elastos/droid/systemui/recents/AlternateRecentsComponent.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/systemui/recent/RecentTasksLoader.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::EIID_IActivityOptionsOnAnimationStartedListener;
using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::App::IActivityOptionsHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawableConstantState;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::SystemUI::EIID_IRecentsComponent;
using Elastos::Droid::SystemUI::Recent::RecentsActivity;
using Elastos::Droid::SystemUI::Recent::RecentTasksLoader;
using Elastos::Droid::SystemUI::Recents::AlternateRecentsComponent;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

//---------------------------------------------------------------------------------------
// CRecents::MyOnAnimationStartedListener
//---------------------------------------------------------------------------------------
CAR_INTERFACE_IMPL(CRecents::MyOnAnimationStartedListener, Object, IActivityOptionsOnAnimationStartedListener)
CRecents::MyOnAnimationStartedListener::MyOnAnimationStartedListener(
    /* [in] */ CRecents* host)
    : mHost(host)
{}

ECode CRecents::MyOnAnimationStartedListener::OnAnimationStarted()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IRecentsActivity::WINDOW_ANIMATION_START_INTENT, (IIntent**)&intent);
    intent->SetPackage(String("Elastos.Droid.SystemUI"));
    mHost->SendBroadcastSafely(intent);
    return NOERROR;
}

//---------------------------------------------------------------------------------------
// CRecents
//---------------------------------------------------------------------------------------
CAR_OBJECT_IMPL(CRecents)

CAR_INTERFACE_IMPL(CRecents, SystemUI, IRecentsComponent)

const String CRecents::TAG("CRecents");
const Boolean CRecents::DEBUG = FALSE;

CRecents::CRecents()
    : mUseAlternateRecents(TRUE)
    , mBootCompleted(FALSE)
{}

ECode CRecents::Start()
{
    if (mUseAlternateRecents) {
        if (mAlternateRecents == NULL) {
            mAlternateRecents = new AlternateRecentsComponent(mContext);
        }
        mAlternateRecents->OnStart();
    }

    PutComponent(String("EIID_IRecentsComponent"), (IRecentsComponent*)this);
    return NOERROR;
}

ECode CRecents::OnBootCompleted()
{
    if (mUseAlternateRecents) {
        if (mAlternateRecents != NULL) {
            mAlternateRecents->OnBootCompleted();
        }
    }
    mBootCompleted = TRUE;
    return NOERROR;
}

ECode CRecents::ShowRecents(
    /* [in] */ Boolean triggeredFromAltTab,
    /* [in] */ IView* statusBarView)
{
    if (mUseAlternateRecents) {
        if (mAlternateRecents != NULL) {
            mAlternateRecents->OnShowRecents(triggeredFromAltTab, statusBarView);
        }
    }
    return NOERROR;
}

ECode CRecents::HideRecents(
    /* [in] */ Boolean triggeredFromAltTab,
    /* [in] */ Boolean triggeredFromHomeKey)
{
    if (mUseAlternateRecents) {
        if (mAlternateRecents != NULL) {
            mAlternateRecents->OnHideRecents(triggeredFromAltTab, triggeredFromHomeKey);
        }
    }
    else {
        AutoPtr<IIntent> intent;
        CIntent::New(IRecentsActivity::CLOSE_RECENTS_INTENT, (IIntent**)&intent);

        intent->SetPackage(String("Elastos.Droid.SystemUI"));
        SendBroadcastSafely(intent);

        AutoPtr<RecentTasksLoader> rtl = RecentTasksLoader::GetInstance(mContext);
        rtl->CancelPreloadingFirstTask();
    }
    return NOERROR;
}

ECode CRecents::ToggleRecents(
    /* [in] */ IDisplay* display,
    /* [in] */ Int32 layoutDirection,
    /* [in] */ IView* statusBarView)
{
    if (mUseAlternateRecents) {
        // Launch the alternate recents if required
        if (mAlternateRecents != NULL) {
            mAlternateRecents->OnToggleRecents(statusBarView);
        }
        return NOERROR;
    }

    // try {
    AutoPtr<RecentTasksLoader> rtl = RecentTasksLoader::GetInstance(mContext);
    AutoPtr<ITaskDescription> firstTask;
    rtl->GetFirstTask((ITaskDescription**)&firstTask);

    if (DEBUG) Logger::D(TAG, "toggle recents panel, firstTask:%s", TO_CSTR(firstTask));

    AutoPtr<IIntent> intent;
    CIntent::New(IRecentsActivity::TOGGLE_RECENTS_INTENT, (IIntent**)&intent);
    intent->SetClassName(
        String("Elastos.Droid.SystemUI"),
        String("Elastos.Droid.SystemUI.Recent.CRecentsActivity"));
    intent->SetFlags(
        IIntent::FLAG_ACTIVITY_NEW_TASK |
        IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);

    AutoPtr<IUserHandle> uh;
    CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&uh);
    if (firstTask == NULL) {
        Boolean b;
        RecentsActivity::ForceOpaqueBackground(mContext, &b);
        if (b) {
            AutoPtr<IActivityOptionsHelper> aoh;
            CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoh);
            AutoPtr<IActivityOptions> opts;
            aoh->MakeCustomAnimation(mContext,
                R::anim::recents_launch_from_launcher_enter,
                R::anim::recents_launch_from_launcher_exit,
                (IActivityOptions**)&opts);
            AutoPtr<IBundle> bundle;
            opts->ToBundle((IBundle**)&bundle);
            mContext->StartActivityAsUser(intent, bundle, uh);
        }
        else {
            // The correct window animation will be applied via the activity's style
            mContext->StartActivityAsUser(intent, uh);
        }
    }
    else {
        AutoPtr<IBitmap> first;
        AutoPtr<IDrawable> drawable;
        firstTask->GetThumbnail((IDrawable**)&drawable);
        if (IBitmapDrawable::Probe(drawable) != NULL) {
            IDrawableConstantState::Probe(drawable)->GetBitmap((IBitmap**)&first);
        }
        else {
            AutoPtr<IBitmapHelper> bh;
            CBitmapHelper::AcquireSingleton((IBitmapHelper**)&bh);
            bh->CreateBitmap(1, 1, BitmapConfig_ARGB_8888, (IBitmap**)&first);
            AutoPtr<IDrawable> d;
            rtl->GetDefaultThumbnail((IDrawable**)&d);
            AutoPtr<ICanvas> c;
            CCanvas::New(first, (ICanvas**)&c);
            d->Draw(c);
        }
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);

        Int32 tw, th;
        res->GetDimensionPixelSize(R::dimen::status_bar_recents_thumbnail_width, &tw);
        res->GetDimensionPixelSize(R::dimen::status_bar_recents_thumbnail_height, &th);
        Float thumbWidth = (Float)tw;
        Float thumbHeight = (Float)th;
        if (first == NULL) {
            Logger::E(TAG, "CRecents thumbnail is null");
            return E_RUNTIME_EXCEPTION;
        }
        Int32 width, height;
        first->GetWidth(&width);
        first->GetHeight(&height);
        if (width != thumbWidth || height != thumbHeight) {
            AutoPtr<IBitmapHelper> bh;
            CBitmapHelper::AcquireSingleton((IBitmapHelper**)&bh);
            bh->CreateScaledBitmap(first, (Int32) thumbWidth, (Int32) thumbHeight,
                TRUE, (IBitmap**)&first);
            if (first == NULL) {
                Logger::E(TAG, "CRecents thumbnail is null");
                return E_RUNTIME_EXCEPTION;
            }
        }

        AutoPtr<IDisplayMetrics> dm;
        CDisplayMetrics::New((IDisplayMetrics**)&dm);
        display->GetMetrics(dm);
        // calculate it here, but consider moving it elsewhere
        // first, determine which orientation you're in.
        AutoPtr<IConfiguration> config;
        res->GetConfiguration((IConfiguration**)&config);
        Int32 x, y, orientation;

        config->GetOrientation(&orientation);
        if (orientation == IConfiguration::ORIENTATION_PORTRAIT) {
            Int32 ii;
            res->GetDimensionPixelSize(
                R::dimen::status_bar_recents_app_label_left_margin, &ii);
            Float appLabelLeftMargin = (Float)ii;
            res->GetDimensionPixelSize(
                R::dimen::status_bar_recents_app_label_width, &ii);
            Float appLabelWidth = ii;
            res->GetDimensionPixelSize(
                R::dimen::status_bar_recents_thumbnail_left_margin, &ii);
            Float thumbLeftMargin = ii;
            res->GetDimensionPixelSize(
                R::dimen::status_bar_recents_thumbnail_bg_padding, &ii);
            Float thumbBgPadding = ii;
            Float width = appLabelLeftMargin + appLabelWidth + thumbLeftMargin
                    + thumbWidth + 2 * thumbBgPadding;

            Int32 widthPixels, heightPixels;
            dm->GetWidthPixels(&widthPixels);
            dm->GetHeightPixels(&heightPixels);
            x = (Int32) ((widthPixels - width) / 2.0f + appLabelLeftMargin + appLabelWidth
                + thumbBgPadding + thumbLeftMargin);
            Int32 dim;
            res->GetDimensionPixelSize(R::dimen::status_bar_recents_thumbnail_height, &dim);
            y = (Int32) (heightPixels - dim - thumbBgPadding);
            if (layoutDirection == IView::LAYOUT_DIRECTION_RTL) {
                Int32 dps;
                res->GetDimensionPixelSize(R::dimen::status_bar_recents_thumbnail_width, &dps);
                x = widthPixels - x - dps;
            }

        }
        else { // if (config.orientation == Configuration.ORIENTATION_LANDSCAPE) {
            Int32 ii2;
            res->GetDimensionPixelSize(
                R::dimen::status_bar_recents_thumbnail_top_margin, &ii2);
            Float thumbTopMargin = (Float)ii2;

            res->GetDimensionPixelSize(
                R::dimen::status_bar_recents_thumbnail_bg_padding, &ii2);
            Float thumbBgPadding = (Float) ii2;

            res->GetDimensionPixelSize(
                R::dimen::status_bar_recents_text_description_padding, &ii2);
            Float textPadding = (Float)ii2;

            res->GetDimensionPixelSize(
                R::dimen::status_bar_recents_app_label_text_size, &ii2);
            Float labelTextSize = (Float)ii2;

            AutoPtr<IPaint> p;
            CPaint::New((IPaint**)&p);
            p->SetTextSize(labelTextSize);
            AutoPtr<IPaintFontMetricsInt> pfm;
            p->GetFontMetricsInt((IPaintFontMetricsInt**)&pfm);
            Int32 bottom, top;
            pfm->GetBottom(&bottom);
            pfm->GetTop(&top);
            Float labelTextHeight = (Float)(bottom - top);

            res->GetDimensionPixelSize(
                R::dimen::status_bar_recents_app_description_text_size, &ii2);
            Float descriptionTextSize = (Float)ii2;
            p->SetTextSize(descriptionTextSize);
            Float descriptionTextHeight = (Float)(bottom - top);

            res->GetDimensionPixelSize(
                R::dimen::status_bar_height, &ii2);
            Float statusBarHeight = (Float)ii2;
            Float recentsItemTopPadding = statusBarHeight;

            Float height = thumbTopMargin + thumbHeight
                    + 2 * thumbBgPadding + textPadding + labelTextHeight
                    + recentsItemTopPadding + textPadding + descriptionTextHeight;
            res->GetDimensionPixelSize(R::dimen::status_bar_recents_item_padding, &ii2);
            Float recentsItemRightPadding = (Float)ii2;

            res->GetDimensionPixelSize(R::dimen::status_bar_recents_right_glow_margin, &ii2);
            Float recentsScrollViewRightPadding = (Float)ii2;

            Int32 widthPixels, heightPixels;
            dm->GetWidthPixels(&widthPixels);
            dm->GetHeightPixels(&heightPixels);
            Int32 dim;
            res->GetDimensionPixelSize(R::dimen::status_bar_recents_thumbnail_width, &dim);
            x = (Int32) (widthPixels - dim
                - thumbBgPadding - recentsItemRightPadding
                - recentsScrollViewRightPadding);
            y = (Int32) ((heightPixels - statusBarHeight - height) / 2.0f + thumbTopMargin
                + recentsItemTopPadding + thumbBgPadding + statusBarHeight);
        }

        AutoPtr<IActivityOptionsHelper> aoh;
        CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoh);

        AutoPtr<MyOnAnimationStartedListener> osl = new MyOnAnimationStartedListener(this);
        AutoPtr<IActivityOptions> opts;
        aoh->MakeThumbnailScaleDownAnimation(statusBarView, first, x, y, osl,
            (IActivityOptions**)&opts);

        intent->PutBooleanExtra(IRecentsActivity::WAITING_FOR_WINDOW_ANIMATION_PARAM, TRUE);
        AutoPtr<IBundle> bundle;
        opts->ToBundle((IBundle**)&bundle);
        StartActivitySafely(intent, bundle);
    }
    // } catch (ActivityNotFoundException e) {
        // Log.e(TAG, "Failed to launch RecentAppsIntent", e);
    // }
    return NOERROR;
}

ECode CRecents::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    if (mUseAlternateRecents) {
        if (mAlternateRecents != NULL) {
            mAlternateRecents->OnConfigurationChanged(newConfig);
        }
    }
    return NOERROR;
}

ECode CRecents::PreloadRecents()
{
    if (mUseAlternateRecents) {
        if (mAlternateRecents != NULL) {
            mAlternateRecents->OnPreloadRecents();
        }
    }
    else {
        AutoPtr<IIntent> intent;
        CIntent::New(IRecentsActivity::PRELOAD_INTENT, (IIntent**)&intent);
        intent->SetClassName(String("Elastos.Droid.SystemUI"),
            String("Elastos.Droid.SystemUI.Recent.CRecentsPreloadReceiver"));
        SendBroadcastSafely(intent);

        AutoPtr<RecentTasksLoader> rtl = RecentTasksLoader::GetInstance(mContext);
        rtl->PreloadFirstTask();
    }
    return NOERROR;
}

ECode CRecents::CancelPreloadingRecents()
{
    if (mUseAlternateRecents) {
        if (mAlternateRecents != NULL) {
            mAlternateRecents->OnCancelPreloadingRecents();
        }
    }
    else {
        AutoPtr<IIntent> intent;
        CIntent::New(IRecentsActivity::CANCEL_PRELOAD_INTENT, (IIntent**)&intent);
        intent->SetClassName(String("Elastos.Droid.SystemUI"),
            String("Elastos.Droid.SystemUI.Recent.CRecentsPreloadReceiver"));
        SendBroadcastSafely(intent);

        AutoPtr<RecentTasksLoader> rtl = RecentTasksLoader::GetInstance(mContext);
        rtl->CancelPreloadingFirstTask();
    }
    return NOERROR;
}

ECode CRecents::ShowNextAffiliatedTask()
{
    if (mUseAlternateRecents) {
        if (mAlternateRecents != NULL) {
            mAlternateRecents->OnShowNextAffiliatedTask();
        }
    }
    return NOERROR;
}

ECode CRecents::ShowPrevAffiliatedTask()
{
    if (mUseAlternateRecents) {
        if (mAlternateRecents != NULL) {
            mAlternateRecents->OnShowPrevAffiliatedTask();
        }
    }
    return NOERROR;
}

ECode CRecents::SetCallback(
    /* [in] */ IRecentsComponentCallbacks* cb)
{
    if (mUseAlternateRecents) {
        if (mAlternateRecents != NULL) {
            mAlternateRecents->SetRecentsComponentCallback(cb);
        }
    }
    return NOERROR;
}

void CRecents::SendBroadcastSafely(
    /* [in] */ IIntent* intent)
{
    if (!mBootCompleted) return;
    AutoPtr<IUserHandle> uh;
    CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&uh);
    mContext->SendBroadcastAsUser(intent, uh);
}

void CRecents::StartActivitySafely(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* opts)
{
    if (!mBootCompleted) return;
    AutoPtr<IUserHandle> uh;
    CUserHandle::New(IUserHandle::USER_CURRENT, (IUserHandle**)&uh);
    mContext->StartActivityAsUser(intent, opts, uh);
}

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos