//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/UiUtils.h"
#include "elastos/core/Math.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::ISurfaceView;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {

//=====================================================================
//                               UiUtils
//=====================================================================
const String UiUtils::TAG("UiUtils");
const Float UiUtils::KEYBOARD_DETECT_BOTTOM_THRESHOLD_DP = 100;
AutoPtr<UiUtils::KeyboardShowingDelegate> UiUtils::sKeyboardShowingDelegate;

ECode UiUtils::SetKeyboardShowingDelegate(
    /* [in] */ KeyboardShowingDelegate* delegate)
{
    VALIDATE_NOT_NULL(delegate);
    // ==================before translated======================
    // sKeyboardShowingDelegate = delegate;

    sKeyboardShowingDelegate = delegate;
    return NOERROR;
}

ECode UiUtils::ShowKeyboard(
    /* [in] */ IView* view)
{
    VALIDATE_NOT_NULL(view);
    // ==================before translated======================
    // InputMethodManager imm =
    //         (InputMethodManager) view.getContext().getSystemService(
    //                 Context.INPUT_METHOD_SERVICE);
    // // Only shows soft keyboard if there isn't an open physical keyboard.
    // imm.showSoftInput(view, InputMethodManager.SHOW_IMPLICIT);

    AutoPtr<IContext> context;
    view->GetContext((IContext**)&context);

    AutoPtr<IInterface> interfaceTmp;
    context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&interfaceTmp);
    IInputMethodManager* imm = IInputMethodManager::Probe(interfaceTmp);

    // Only shows soft keyboard if there isn't an open physical keyboard.
    Boolean funcRet = FALSE;
    imm->ShowSoftInput(view, IInputMethodManager::SHOW_IMPLICIT, &funcRet);
    return NOERROR;
}

Boolean UiUtils::HideKeyboard(
    /* [in] */ IView* view)
{
    // ==================before translated======================
    // InputMethodManager imm =
    //         (InputMethodManager) view.getContext().getSystemService(
    //                 Context.INPUT_METHOD_SERVICE);
    // return imm.hideSoftInputFromWindow(view.getWindowToken(), 0);

    AutoPtr<IContext> context;
    view->GetContext((IContext**)&context);

    AutoPtr<IInterface> interfaceTmp;
    context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&interfaceTmp);
    IInputMethodManager* imm = IInputMethodManager::Probe(interfaceTmp);

    AutoPtr<IBinder> binder;
    view->GetWindowToken((IBinder**)&binder);

    Boolean result = FALSE;
    imm->HideSoftInputFromWindow(binder, 0, &result);
    return result;
}

Boolean UiUtils::IsKeyboardShowing(
    /* [in] */ IContext* context,
    /* [in] */ IView* view)
{
    // ==================before translated======================
    // if (sKeyboardShowingDelegate != null
    //         && sKeyboardShowingDelegate.disableKeyboardCheck(context, view)) {
    //     return false;
    // }
    //
    // View rootView = view.getRootView();
    // if (rootView == null) return false;
    // Rect appRect = new Rect();
    // rootView.getWindowVisibleDisplayFrame(appRect);
    //
    // final float density = context.getResources().getDisplayMetrics().density;
    // final float bottomMarginDp = Math.abs(rootView.getHeight() - appRect.height()) / density;
    // return bottomMarginDp > KEYBOARD_DETECT_BOTTOM_THRESHOLD_DP;

    if (NULL != sKeyboardShowingDelegate && sKeyboardShowingDelegate->DisableKeyboardCheck(context, view)) {
        return FALSE;
    }

    AutoPtr<IView> rootView;
    view->GetRootView((IView**)&rootView);

    if (NULL == rootView)
        return FALSE;

    AutoPtr<IRect> appRect;
    CRect::New((IRect**)&appRect);
    rootView->GetWindowVisibleDisplayFrame(appRect);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);

    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);

    Int32 rootheight = 0;
    rootView->GetHeight(&rootheight);

    Int32 appHeight = 0;
    appRect->GetHeight(&appHeight);

    Float density = 0.0f;
    metrics->GetDensity(&density);
    const Float bottomMarginDp = Elastos::Core::Math::Abs(rootheight - appHeight) / density;
    return (Boolean)(bottomMarginDp > KEYBOARD_DETECT_BOTTOM_THRESHOLD_DP);
}

Int32 UiUtils::InsertBefore(
    /* [in] */ IViewGroup* container,
    /* [in] */ IView* newView,
    /* [in] */ IView* existingView)
{
    // ==================before translated======================
    // return insertView(container, newView, existingView, false);

    return InsertView(container, newView, existingView, FALSE);
}

Int32 UiUtils::InsertAfter(
    /* [in] */ IViewGroup* container,
    /* [in] */ IView* newView,
    /* [in] */ IView* existingView)
{
    // ==================before translated======================
    // return insertView(container, newView, existingView, true);

    return InsertView(container, newView, existingView, TRUE);
}

AutoPtr<IBitmap> UiUtils::GenerateScaledScreenshot(
    /* [in] */ IView* currentView,
    /* [in] */ Int32 maximumDimension,
    /* [in] */ BitmapConfig bitmapConfig)
{
    // ==================before translated======================
    // Bitmap screenshot = null;
    // boolean drawingCacheEnabled = currentView.isDrawingCacheEnabled();
    // try {
    //     prepareViewHierarchyForScreenshot(currentView, true);
    //     if (!drawingCacheEnabled) currentView.setDrawingCacheEnabled(true);
    //     // Android has a maximum drawing cache size and if the drawing cache is bigger
    //     // than that, getDrawingCache() returns null.
    //     Bitmap originalBitmap = currentView.getDrawingCache();
    //     if (originalBitmap != null) {
    //         double originalHeight = originalBitmap.getHeight();
    //         double originalWidth = originalBitmap.getWidth();
    //         int newWidth = (int) originalWidth;
    //         int newHeight = (int) originalHeight;
    //         if (maximumDimension > 0) {
    //             double scale = maximumDimension / Math.max(originalWidth, originalHeight);
    //             newWidth = (int) Math.round(originalWidth * scale);
    //             newHeight = (int) Math.round(originalHeight * scale);
    //         }
    //         Bitmap scaledScreenshot =
    //                 Bitmap.createScaledBitmap(originalBitmap, newWidth, newHeight, true);
    //         if (scaledScreenshot.getConfig() != bitmapConfig) {
    //             screenshot = scaledScreenshot.copy(bitmapConfig, false);
    //             scaledScreenshot.recycle();
    //             scaledScreenshot = null;
    //         } else {
    //             screenshot = scaledScreenshot;
    //         }
    //     } else if (currentView.getMeasuredHeight() > 0 && currentView.getMeasuredWidth() > 0) {
    //         double originalHeight = currentView.getMeasuredHeight();
    //         double originalWidth = currentView.getMeasuredWidth();
    //         int newWidth = (int) originalWidth;
    //         int newHeight = (int) originalHeight;
    //         if (maximumDimension > 0) {
    //             double scale = maximumDimension / Math.max(originalWidth, originalHeight);
    //             newWidth = (int) Math.round(originalWidth * scale);
    //             newHeight = (int) Math.round(originalHeight * scale);
    //         }
    //         Bitmap bitmap = Bitmap.createBitmap(newWidth, newHeight, bitmapConfig);
    //         Canvas canvas = new Canvas(bitmap);
    //         canvas.scale((float) (newWidth / originalWidth),
    //                 (float) (newHeight / originalHeight));
    //         currentView.draw(canvas);
    //         screenshot = bitmap;
    //     }
    // } catch (OutOfMemoryError e) {
    //     Log.d(TAG, "Unable to capture screenshot and scale it down." + e.getMessage());
    // } finally {
    //     if (!drawingCacheEnabled) currentView.setDrawingCacheEnabled(false);
    //     prepareViewHierarchyForScreenshot(currentView, false);
    // }
    // return screenshot;

    AutoPtr<IBitmap> screenshot = NULL;
    Boolean drawingCacheEnabled = FALSE;
    currentView->IsDrawingCacheEnabled(&drawingCacheEnabled);

    //try {
        PrepareViewHierarchyForScreenshot(currentView, TRUE);

        if (!drawingCacheEnabled)
            currentView->SetDrawingCacheEnabled(TRUE);

        // Android has a maximum drawing cache size and if the drawing cache is bigger
        // than that, getDrawingCache() returns null.
        AutoPtr<IBitmap> originalBitmap;
        currentView->GetDrawingCache((IBitmap**)&originalBitmap);

        Int32 currViewMeasureHeight = 0;
        Int32 currViewMeasureWidth = 0;
        currentView->GetMeasuredHeight(&currViewMeasureHeight);
        currentView->GetMeasuredWidth(&currViewMeasureWidth);

        if (originalBitmap != NULL) {
            Int32 originalHeight = 0;
            originalBitmap->GetHeight(&originalHeight);
            Int32 originalWidth = 0;
            originalBitmap->GetWidth(&originalWidth);
            Int32 newWidth = (Int32)originalWidth;
            Int32 newHeight = (Int32)originalHeight;

            if (maximumDimension > 0) {
                Double scale = maximumDimension / Elastos::Core::Math::Max(originalWidth, originalHeight);
                newWidth = (Int32)Elastos::Core::Math::Round(originalWidth * scale);
                newHeight = (Int32)Elastos::Core::Math::Round(originalHeight * scale);
            }

            AutoPtr<IBitmapHelper> helper;
            CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
            AutoPtr<IBitmap> scaledScreenshot;
            helper->CreateScaledBitmap(originalBitmap, newWidth, newHeight, TRUE, (IBitmap**)&scaledScreenshot);

            BitmapConfig scaledConfig;
            scaledScreenshot->GetConfig(&scaledConfig);

            if (scaledConfig != bitmapConfig) {
                scaledScreenshot->Copy(bitmapConfig, FALSE, (IBitmap**)&screenshot);
                scaledScreenshot->Recycle();
                scaledScreenshot = NULL;
            }
            else {
                screenshot = scaledScreenshot;
            }
        }
        else if (currViewMeasureHeight > 0 && currViewMeasureWidth > 0) {
            Int32 originalHeight = 0;
            Int32 originalWidth = 0;
            currentView->GetMeasuredHeight(&originalHeight);
            currentView->GetMeasuredWidth(&originalWidth);
            Int32 newWidth = (Int32) originalWidth;
            Int32 newHeight = (Int32) originalHeight;

            if (maximumDimension > 0) {
                Double scale = maximumDimension / Elastos::Core::Math::Max(originalWidth, originalHeight);
                newWidth = (Int32) Elastos::Core::Math::Round(originalWidth * scale);
                newHeight = (Int32) Elastos::Core::Math::Round(originalHeight * scale);
            }

            AutoPtr<IBitmapHelper> helper;
            CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
            AutoPtr<IBitmap> bitmap;
            helper->CreateBitmap(newWidth, newHeight, bitmapConfig, (IBitmap**)&bitmap);

            AutoPtr<ICanvas> canvas;
            CCanvas::New(bitmap, (ICanvas**)&canvas);

            canvas->Scale((Float) (newWidth / originalWidth),
                    (Float) (newHeight / originalHeight));
            currentView->Draw(canvas);
            screenshot = bitmap;
        }
    //} catch (OutOfMemoryError e) {
    //    Log.d(TAG, "Unable to capture screenshot and scale it down." + e.getMessage());
    //} finally {
        if (!drawingCacheEnabled)
            currentView->SetDrawingCacheEnabled(FALSE);
        PrepareViewHierarchyForScreenshot(currentView, FALSE);
    //}
    return screenshot;
}

UiUtils::UiUtils()
{
}

Int32 UiUtils::InsertView(
    /* [in] */ IViewGroup* container,
    /* [in] */ IView* newView,
    /* [in] */ IView* existingView,
    /* [in] */ Boolean after)
{
    // ==================before translated======================
    // // See if the view has already been added.
    // int index = container.indexOfChild(newView);
    // if (index >= 0) return index;
    //
    // // Find the location of the existing view.
    // index = container.indexOfChild(existingView);
    // if (index < 0) return -1;
    //
    // // Add the view.
    // if (after) index++;
    // container.addView(newView, index);
    // return index;

    // See if the view has already been added.
    Int32 index = 0;
    container->IndexOfChild(newView, &index);
    if (index >= 0)
        return index;

    // Find the location of the existing view.
    container->IndexOfChild(existingView, &index);
    if (index < 0)
        return -1;

    // Add the view.
    if (after)
        ++index;

    container->AddView(newView, index);
    return index;
}

ECode UiUtils::PrepareViewHierarchyForScreenshot(
    /* [in] */ IView* view,
    /* [in] */ Boolean takingScreenshot)
{
    VALIDATE_NOT_NULL(view);
    // ==================before translated======================
    // if (view instanceof ViewGroup) {
    //     ViewGroup viewGroup = (ViewGroup) view;
    //     for (int i = 0; i < viewGroup.getChildCount(); i++) {
    //         prepareViewHierarchyForScreenshot(viewGroup.getChildAt(i), takingScreenshot);
    //     }
    // } else if (view instanceof SurfaceView) {
    //     view.setWillNotDraw(!takingScreenshot);
    // }

    IViewGroup* viewGroup = IViewGroup::Probe(view);
    ISurfaceView* surfaceView = ISurfaceView::Probe(view);
    if (NULL != viewGroup) {
        Int32 childCount = 0;
        viewGroup->GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; ++i) {
            AutoPtr<IView> child;
            viewGroup->GetChildAt(i, (IView**)&child);
            PrepareViewHierarchyForScreenshot(child, takingScreenshot);
        }
    }
    else if (NULL != surfaceView) {
        view->SetWillNotDraw(!takingScreenshot);
    }
    return NOERROR;
}

} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


