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

#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Widget.h>
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ApiCompatibilityUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/components/ValidationMessageBubble.h"
#include "elastos/droid/webkit/webview/chromium/native/components/api/ValidationMessageBubble_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/RenderCoordinates.h"
#include "elastos/droid/webkit/webview/chromium/native/content/R_Content.h"
//#include "elastos/droid/widget/CRelativeLayoutLayoutParams.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::EIID_IViewManager;
using Elastos::Droid::View::EIID_IViewParent;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::View;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ApiCompatibilityUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::RenderCoordinates;
using Elastos::Droid::Webkit::Webview::Chromium::Content::R;
using Elastos::Droid::Widget::CPopupWindow;
//using Elastos::Droid::Widget::CRelativeLayoutLayoutParams;
using Elastos::Droid::Widget::IRelativeLayoutLayoutParams;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Components {

//=====================================================================
//                       ValidationMessageBubble
//=====================================================================
ValidationMessageBubble::ValidationMessageBubble(
    /* [in] */ ContentViewCore* contentViewCore,
    /* [in] */ IRectF* anchor,
    /* [in] */ const String& mainText,
    /* [in] */ const String& subText)
{
    // ==================before translated======================
    // final ViewGroup root = (ViewGroup) View.inflate(contentViewCore.getContext(),
    //         R.layout.validation_message_bubble, null);
    // mPopup = new PopupWindow(root);
    // updateTextViews(root, mainText, subText);
    // measure(contentViewCore.getRenderCoordinates());
    // Point origin = adjustWindowPosition(
    //         contentViewCore, (int) (anchor.centerX() - getAnchorOffset()), (int) anchor.bottom);
    // mPopup.showAtLocation(
    //         contentViewCore.getContainerView(), Gravity.NO_GRAVITY, origin.x, origin.y);

    AutoPtr<IView> view;
    Elastos::Droid::View::View::Inflate(contentViewCore->GetContext(), R::layout::validation_message_bubble, NULL, (IView**)&view);
    IViewGroup* root = IViewGroup::Probe(view);

    CPopupWindow::New(IView::Probe(root), (IPopupWindow**)&mPopup);
    UpdateTextViews(root, mainText, subText);
    Measure(contentViewCore->GetRenderCoordinates());

    Float centerX = 0.0f;
    Float bottom = 0.0f;
    anchor->GetCenterX(&centerX);
    anchor->GetBottom(&bottom);
    AutoPtr<IPoint> origin = AdjustWindowPosition(contentViewCore, (Int32)(centerX - GetAnchorOffset()), (Int32)bottom);

    AutoPtr<IViewGroup> viewGroup = contentViewCore->GetContainerView();
    IView* viewTmp = IView::Probe(viewGroup);
    Int32 originX = 0;
    Int32 originY = 0;
    origin->GetX(&originX);
    origin->GetY(&originY);
    mPopup->ShowAtLocation(viewTmp, IGravity::NO_GRAVITY, originX, originY);
}

AutoPtr<ValidationMessageBubble> ValidationMessageBubble::CreateAndShow(
    /* [in] */ ContentViewCore* contentViewCore,
    /* [in] */ Int32 anchorX,
    /* [in] */ Int32 anchorY,
    /* [in] */ Int32 anchorWidth,
    /* [in] */ Int32 anchorHeight,
    /* [in] */ const String& mainText,
    /* [in] */ const String& subText)
{
    // ==================before translated======================
    // final RectF anchorPixInScreen = makePixRectInScreen(
    //         contentViewCore, anchorX, anchorY, anchorWidth, anchorHeight);
    // return new ValidationMessageBubble(contentViewCore, anchorPixInScreen, mainText, subText);

    AutoPtr<IRectF> anchorPixInScreen = MakePixRectInScreen(contentViewCore, anchorX, anchorY, anchorWidth, anchorHeight);
    AutoPtr<ValidationMessageBubble> result = new ValidationMessageBubble(contentViewCore, anchorPixInScreen, mainText, subText);
    return result;
}

ECode ValidationMessageBubble::Close()
{
    // ==================before translated======================
    // if (mPopup == null) return;
    // mPopup.dismiss();
    // mPopup = null;

    if (NULL == mPopup) assert(0);
    return NOERROR;
}

ECode ValidationMessageBubble::SetPositionRelativeToAnchor(
    /* [in] */ ContentViewCore* contentViewCore,
    /* [in] */ Int32 anchorX,
    /* [in] */ Int32 anchorY,
    /* [in] */ Int32 anchorWidth,
    /* [in] */ Int32 anchorHeight)
{
    VALIDATE_NOT_NULL(contentViewCore);
    // ==================before translated======================
    // RectF anchor = makePixRectInScreen(
    //         contentViewCore, anchorX, anchorY, anchorWidth, anchorHeight);
    // Point origin = adjustWindowPosition(
    //         contentViewCore, (int) (anchor.centerX() - getAnchorOffset()), (int) anchor.bottom);
    // mPopup.update(origin.x, origin.y, mPopup.getWidth(), mPopup.getHeight());

    AutoPtr<IRectF> anchor = MakePixRectInScreen(contentViewCore, anchorX, anchorY, anchorWidth, anchorHeight);
    Float centerX = 0.0f;
    Float bottom = 0.0f;
    anchor->GetCenterX(&centerX);
    anchor->GetBottom(&bottom);
    AutoPtr<IPoint> origin = AdjustWindowPosition(contentViewCore, (Int32)(centerX - GetAnchorOffset()), (Int32)bottom);
    Int32 originX = 0;
    Int32 originY = 0;
    origin->GetX(&originX);
    origin->GetY(&originY);

    Int32 width = 0;
    Int32 height = 0;
    mPopup->GetWidth(&width);
    mPopup->GetHeight(&height);
    mPopup->Update(originX, originY, width, height);
    return NOERROR;
}

AutoPtr<IRectF> ValidationMessageBubble::MakePixRectInScreen(
    /* [in] */ ContentViewCore* contentViewCore,
    /* [in] */ Int32 anchorX,
    /* [in] */ Int32 anchorY,
    /* [in] */ Int32 anchorWidth,
    /* [in] */ Int32 anchorHeight)
{
    // ==================before translated======================
    // final RenderCoordinates coordinates = contentViewCore.getRenderCoordinates();
    // final float yOffset = getWebViewOffsetYPixInScreen(contentViewCore);
    // return new RectF(
    //         coordinates.fromLocalCssToPix(anchorX),
    //         coordinates.fromLocalCssToPix(anchorY) + yOffset,
    //         coordinates.fromLocalCssToPix(anchorX + anchorWidth),
    //         coordinates.fromLocalCssToPix(anchorY + anchorHeight) + yOffset);

    AutoPtr<RenderCoordinates> coordinates = contentViewCore->GetRenderCoordinates();
    Float yOffset = GetWebViewOffsetYPixInScreen(contentViewCore);

    AutoPtr<IRectF> result;
    CRectF::New((IRectF**)&result);
    result->Set(
        coordinates->FromLocalCssToPix(anchorX),
        coordinates->FromLocalCssToPix(anchorY) + yOffset,
        coordinates->FromLocalCssToPix(anchorX + anchorWidth),
        coordinates->FromLocalCssToPix(anchorY + anchorHeight) + yOffset
    );

    return result;
}

Float ValidationMessageBubble::GetWebViewOffsetYPixInScreen(
    /* [in] */ ContentViewCore* contentViewCore)
{
    // ==================before translated======================
    // int[] location = new int[2];
    // contentViewCore.getContainerView().getLocationOnScreen(location);
    // return location[1] + contentViewCore.getRenderCoordinates().getContentOffsetYPix();

    AutoPtr<IViewGroup> viewGroup = contentViewCore->GetContainerView();
    AutoPtr<IView> view = IView::Probe(viewGroup);
    AutoPtr< ArrayOf<Int32> > temp = ArrayOf<Int32>::Alloc(2);
    view->GetLocationOnScreen((ArrayOf<Int32>*)temp);

    AutoPtr<RenderCoordinates> coordinates = contentViewCore->GetRenderCoordinates();
    Float offsetYPix = coordinates->GetContentOffsetYPix();
    Float result = (*temp)[1] + offsetYPix;
    return result;
}

ECode ValidationMessageBubble::UpdateTextViews(
    /* [in] */ IViewGroup* root,
    /* [in] */ const String& mainText,
    /* [in] */ const String& subText)
{
    VALIDATE_NOT_NULL(root);
    // ==================before translated======================
    // ((TextView) root.findViewById(R.id.main_text)).setText(mainText);
    // final TextView subTextView = (TextView) root.findViewById(R.id.sub_text);
    // if (!TextUtils.isEmpty(subText)) {
    //     subTextView.setText(subText);
    // } else {
    //     ((ViewGroup) subTextView.getParent()).removeView(subTextView);
    // }

    IView* rootView = IView::Probe(root);
    AutoPtr<IView> viewTmp;
    FAIL_RETURN(rootView->FindViewById(R::id::main_text, (IView**)&viewTmp));
    ITextView* textView = ITextView::Probe(viewTmp);

    AutoPtr<ICharSequence> charSequence;
    CString::New(mainText, (ICharSequence**)&charSequence);
    textView->SetText(charSequence);

    AutoPtr<IView> viewTmp1;
    FAIL_RETURN(rootView->FindViewById(R::id::sub_text, (IView**)&viewTmp1));
    ITextView* subTextView = ITextView::Probe(viewTmp1);

    AutoPtr<ICharSequence> charSequence1;
    CString::New(subText, (ICharSequence**)&charSequence1);
    if (TextUtils::IsEmpty(charSequence1)) {
        subTextView->SetText(charSequence1);
    }
    else {
        IView* viewTmp2 = IView::Probe(subTextView);
        AutoPtr<IViewParent> parent;
        viewTmp2->GetParent((IViewParent**)&parent);
        IViewManager* viewManager = IViewManager::Probe(parent);
        viewManager->RemoveView(viewTmp2);
    }

    return NOERROR;
}

ECode ValidationMessageBubble::Measure(
    /* [in] */ RenderCoordinates* coordinates)
{
    VALIDATE_NOT_NULL(coordinates);
    // ==================before translated======================
    // mPopup.setWindowLayoutMode(
    //         ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
    // mPopup.getContentView().setLayoutParams(
    //         new RelativeLayout.LayoutParams(
    //                 RelativeLayout.LayoutParams.WRAP_CONTENT,
    //                 RelativeLayout.LayoutParams.WRAP_CONTENT));
    // mPopup.getContentView().measure(
    //         View.MeasureSpec.makeMeasureSpec(coordinates.getLastFrameViewportWidthPixInt(),
    //                 View.MeasureSpec.AT_MOST),
    //         View.MeasureSpec.makeMeasureSpec(coordinates.getLastFrameViewportHeightPixInt(),
    //                 View.MeasureSpec.AT_MOST));

    mPopup->SetWindowLayoutMode(IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT);
    AutoPtr<IView> contentView;
    mPopup->GetContentView((IView**)&contentView);

    AutoPtr<IRelativeLayoutLayoutParams> layoutParams;
    //-- has no cpp: CRelativeLayoutLayoutParams::New(
    //  IRelativeLayoutLayoutParams::WRAP_CONTENT,
    //    IRelativeLayoutLayoutParams::WRAP_CONTENT,
    //    (IRelativeLayoutLayoutParams**)&layoutParams
    //);
    IViewGroupLayoutParams* layoutParamsTmp = IViewGroupLayoutParams::Probe(layoutParams);
    contentView->SetLayoutParams(layoutParamsTmp);

    Int32 measureWidth = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(coordinates->GetLastFrameViewportWidthPixInt(),
        Elastos::Droid::View::View::MeasureSpec::AT_MOST);
    Int32 measureHeight = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(coordinates->GetLastFrameViewportHeightPixInt(),
        Elastos::Droid::View::View::MeasureSpec::AT_MOST);
    contentView->Measure(measureWidth, measureHeight);
    return NOERROR;
}

Float ValidationMessageBubble::GetAnchorOffset()
{
    // ==================before translated======================
    // final View root = mPopup.getContentView();
    // final int width = root.getMeasuredWidth();
    // final int arrowWidth = root.findViewById(R.id.arrow_image).getMeasuredWidth();
    // return ApiCompatibilityUtils.isLayoutRtl(root) ?
    //         (width * 3 / 4 - arrowWidth / 2) : (width / 4 + arrowWidth / 2);

    AutoPtr<IView> root;
    mPopup->GetContentView((IView**)&root);
    Int32 width = 0;
    root->GetMeasuredWidth(&width);

    AutoPtr<IView> viewTmp;
    FAIL_RETURN(root->FindViewById(R::id::arrow_image, (IView**)&viewTmp));

    Int32 arrowWidth = 0;
    viewTmp->GetMeasuredWidth(&arrowWidth);

    Float result = ApiCompatibilityUtils::IsLayoutRtl(root) ? (width * 3 / 4 - arrowWidth / 2) : (width / 4 + arrowWidth / 2);
    return result;
}

AutoPtr<IPoint> ValidationMessageBubble::AdjustWindowPosition(
    /* [in] */ ContentViewCore* contentViewCore,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    // ==================before translated======================
    // final RenderCoordinates coordinates = contentViewCore.getRenderCoordinates();
    // final int viewWidth = coordinates.getLastFrameViewportWidthPixInt();
    // final int viewBottom = (int) getWebViewOffsetYPixInScreen(contentViewCore) +
    //         coordinates.getLastFrameViewportHeightPixInt();
    // final int width = mPopup.getContentView().getMeasuredWidth();
    // final int height = mPopup.getContentView().getMeasuredHeight();
    // if (x < 0) {
    //     x = 0;
    // } else if (x + width > viewWidth) {
    //     x = viewWidth - width;
    // }
    // if (y + height > viewBottom) {
    //     y = viewBottom - height;
    // }
    // return new Point(x, y);

    AutoPtr<RenderCoordinates> coordinates = contentViewCore->GetRenderCoordinates();
    Int32 viewWidth = coordinates->GetLastFrameViewportWidthPixInt();
    Int32 viewBottom = (Int32)GetWebViewOffsetYPixInScreen(contentViewCore) +
        coordinates->GetLastFrameViewportHeightPixInt();

    AutoPtr<IView> contentView;
    mPopup->GetContentView((IView**)&contentView);
    Int32 width = 0;
    Int32 height = 0;
    contentView->GetMeasuredWidth(&width);
    contentView->GetMeasuredHeight(&height);

    if (x < 0) {
        x = 0;
    }
    else if (x + width > viewWidth) {
        x = viewWidth - width;
    }
    if (y + height > viewBottom) {
        y = viewBottom - height;
    }

    AutoPtr<IPoint> result;
    CPoint::New(x, y, (IPoint**)&result);
    return result;
}

AutoPtr<IInterface> ValidationMessageBubble::CreateAndShow(
    /* [in] */ IInterface* contentViewCore,
    /* [in] */ Int32 anchorX,
    /* [in] */ Int32 anchorY,
    /* [in] */ Int32 anchorWidth,
    /* [in] */ Int32 anchorHeight,
    /* [in] */ const String& mainText,
    /* [in] */ const String& subText)
{
    ContentViewCore* cvc = (ContentViewCore*)(IObject::Probe(contentViewCore));
    return TO_IINTERFACE(CreateAndShow(cvc, anchorX, anchorY, anchorWidth, anchorHeight, mainText, subText));
}

void ValidationMessageBubble::Close(
    /* [in] */ IInterface* obj)
{
    ValidationMessageBubble* mObj = (ValidationMessageBubble*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("ValidationMessageBubble", "ValidationMessageBubble::Close, mObj is NULL");
        return;
    }
    mObj->Close();
}

void ValidationMessageBubble::SetPositionRelativeToAnchor(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* contentViewCore,
    /* [in] */ Int32 anchorX,
    /* [in] */ Int32 anchorY,
    /* [in] */ Int32 anchorWidth,
    /* [in] */ Int32 anchorHeight)
{
    ValidationMessageBubble* mObj = (ValidationMessageBubble*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("ValidationMessageBubble", "ValidationMessageBubble::SetPositionRelativeToAnchor, mObj is NULL");
        return;
    }
    ContentViewCore* cvc = (ContentViewCore*)(IObject::Probe(contentViewCore));
    mObj->SetPositionRelativeToAnchor(cvc, anchorX, anchorY, anchorWidth, anchorHeight);
}

} // namespace Components
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

