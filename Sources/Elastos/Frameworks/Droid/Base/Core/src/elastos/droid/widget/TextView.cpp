#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Provider.h"
#include <Elastos.CoreLibrary.Utility.h>

#include "elastos/droid/widget/TextView.h"
#include "elastos/droid/content/res/CColorStateList.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/content/res/CColorStateListHelper.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/Insets.h"
#include "elastos/droid/graphics/Typeface.h"
//#include "elastos/droid/provider/CSettingsSecure.h"
#include "elastos/droid/text/Layout.h"
#include "elastos/droid/text/CDynamicLayout.h"
#include "elastos/droid/text/CStaticLayout.h"
#include "elastos/droid/text/BoringLayout.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/Selection.h"
#include "elastos/droid/text/CTextPaint.h"
#include "elastos/droid/text/CSpannedString.h"
#include "elastos/droid/text/CSpannableString.h"
#include "elastos/droid/text/TextDirectionHeuristics.h"
#include "elastos/droid/text/method/CMetaKeyKeyListenerHelper.h"
#include "elastos/droid/text/CBoringLayoutMetrics.h"
#include "elastos/droid/text/CEditableFactory.h"
#include "elastos/droid/text/CSpannableFactory.h"
#include "elastos/droid/text/CLengthFilter.h"
#include "elastos/droid/text/utility/Linkify.h"
#include "elastos/droid/text/method/DigitsKeyListener.h"
#include "elastos/droid/text/method/CDialerKeyListener.h"
#include "elastos/droid/text/method/CTextKeyListenerHelper.h"
#include "elastos/droid/text/method/CDateKeyListenerHelper.h"
#include "elastos/droid/text/method/CTimeKeyListenerHelper.h"
#include "elastos/droid/text/method/CDigitsKeyListenerHelper.h"
#include "elastos/droid/text/method/CLinkMovementMethodHelper.h"
#include "elastos/droid/text/method/CArrowKeyMovementMethodHelper.h"
#include "elastos/droid/text/method/CAllCapsTransformationMethod.h"
#include "elastos/droid/text/method/CPasswordTransformationMethodHelper.h"
#include "elastos/droid/text/method/CDateTimeKeyListenerHelper.h"
#include "elastos/droid/text/method/CDialerKeyListenerHelper.h"
#include "elastos/droid/text/method/CSingleLineTransformationMethodHelper.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/utility/CTypedValueHelper.h"
//#include "elastos/droid/view/CViewConfigurationHelper.h"
//#include "elastos/droid/view/CKeyEvent.h"
#include "elastos/droid/view/Gravity.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"
#include "elastos/droid/view/accessibility/CAccessibilityEvent.h"
#include "elastos/droid/view/inputmethod/BaseInputConnection.h"
//#include "elastos/droid/view/inputmethod/CInputMethodManager.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IUndoOwner;
using Elastos::Droid::Content::IClipDataItem;
using Elastos::Droid::Content::IClipDataHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IClipboardManager;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::Res::IColorStateListHelper;
using Elastos::Droid::Content::Res::CColorStateListHelper;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::CColorStateList;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::Insets;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::Typeface;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::InputMethodService::EIID_IExtractEditText;
using Elastos::Droid::Provider::ISettingsSecure;
//using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Layout;
using Elastos::Droid::Text::ILengthFilter;
using Elastos::Droid::Text::IDynamicLayout;
using Elastos::Droid::Text::CDynamicLayout;
using Elastos::Droid::Text::IStaticLayout;
using Elastos::Droid::Text::BoringLayout;
using Elastos::Droid::Text::CStaticLayout;
using Elastos::Droid::Text::Selection;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::CTextPaint;
using Elastos::Droid::Text::EIID_IGetChars;
using Elastos::Droid::Text::EIID_IParcelableSpan;
using Elastos::Droid::Text::EIID_IGraphicsOperations;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::Text::EIID_ISpanWatcher;
using Elastos::Droid::Text::ISpannedString;
using Elastos::Droid::Text::CSpannedString;
using Elastos::Droid::Text::CSpannableFactory;
using Elastos::Droid::Text::CEditableFactory;
using Elastos::Droid::Text::CBoringLayoutMetrics;
using Elastos::Droid::Text::CLengthFilter;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::CSpannableString;
using Elastos::Droid::Text::TextDirectionHeuristics;
using Elastos::Droid::Text::TextUtilsTruncateAt_MARQUEE;
using Elastos::Droid::Text::TextUtilsTruncateAt_NONE;
using Elastos::Droid::Text::TextUtilsTruncateAt_START;
using Elastos::Droid::Text::TextUtilsTruncateAt_MIDDLE;
using Elastos::Droid::Text::TextUtilsTruncateAt_END;
using Elastos::Droid::Text::TextUtilsTruncateAt_END_SMALL;
using Elastos::Droid::Text::Style::ISuggestionSpan;
using Elastos::Droid::Text::Style::EIID_IClickableSpan;
using Elastos::Droid::Text::Style::EIID_ISuggestionSpan;
using Elastos::Droid::Text::Style::EIID_IURLSpan;
using Elastos::Droid::Text::Style::EIID_ISpellCheckSpan;
using Elastos::Droid::Text::Style::IClickableSpan;
using Elastos::Droid::Text::Utility::Linkify;
using Elastos::Droid::Text::Method::ITextKeyListener;
using Elastos::Droid::Text::Method::DigitsKeyListener;
using Elastos::Droid::Text::Method::IDigitsKeyListener;
using Elastos::Droid::Text::Method::CDialerKeyListener;
using Elastos::Droid::Text::Method::IDialerKeyListener;
using Elastos::Droid::Text::Method::IMetaKeyKeyListener;
using Elastos::Droid::Text::Method::IMetaKeyKeyListenerHelper;
using Elastos::Droid::Text::Method::CMetaKeyKeyListenerHelper;
using Elastos::Droid::Text::Method::IDateKeyListener;
using Elastos::Droid::Text::Method::ITextKeyListenerHelper;
using Elastos::Droid::Text::Method::CTextKeyListenerHelper;
using Elastos::Droid::Text::Method::IArrowKeyMovementMethodHelper;
using Elastos::Droid::Text::Method::CArrowKeyMovementMethodHelper;
using Elastos::Droid::Text::Method::CAllCapsTransformationMethod;
using Elastos::Droid::Text::Method::IPasswordTransformationMethodHelper;
using Elastos::Droid::Text::Method::CPasswordTransformationMethodHelper;
using Elastos::Droid::Text::Method::IPasswordTransformationMethod;
using Elastos::Droid::Text::Method::IAllCapsTransformationMethod;
using Elastos::Droid::Text::Method::ITransformationMethod2;
using Elastos::Droid::Text::Method::ILinkMovementMethodHelper;
using Elastos::Droid::Text::Method::CLinkMovementMethodHelper;
using Elastos::Droid::Text::Method::IDigitsKeyListenerHelper;
using Elastos::Droid::Text::Method::CDigitsKeyListenerHelper;
using Elastos::Droid::Text::Method::IDateKeyListenerHelper;
using Elastos::Droid::Text::Method::CDateKeyListenerHelper;
using Elastos::Droid::Text::Method::ITimeKeyListenerHelper;
using Elastos::Droid::Text::Method::CTimeKeyListenerHelper;
using Elastos::Droid::Text::Method::ITimeKeyListener;
using Elastos::Droid::Text::Method::IDateTimeKeyListenerHelper;
using Elastos::Droid::Text::Method::CDateTimeKeyListenerHelper;
using Elastos::Droid::Text::Method::IDateTimeKeyListener;
using Elastos::Droid::Text::Method::IDialerKeyListenerHelper;
using Elastos::Droid::Text::Method::CDialerKeyListenerHelper;
using Elastos::Droid::Text::Method::ISingleLineTransformationMethod;
using Elastos::Droid::Text::Method::ISingleLineTransformationMethodHelper;
using Elastos::Droid::Text::Method::CSingleLineTransformationMethodHelper;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IBaseBundle;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::ITypedValueHelper;
using Elastos::Droid::Utility::CTypedValueHelper;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IDispatcherState;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IFrameCallback;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::IHapticFeedbackConstants;
//using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::Gravity;
//using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::CAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Accessibility::CAccessibilityEvent;
using Elastos::Droid::View::InputMethod::BaseInputConnection;
//using Elastos::Droid::View::InputMethod::CInputMethodManager;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::CString;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

//==============================================================================
//              UpdateTextServicesLocaleRunnable
//==============================================================================

UpdateTextServicesLocaleRunnable::UpdateTextServicesLocaleRunnable(
    /* [in] */ TextView* textView)
{
    AutoPtr<IWeakReferenceSource> wrs = (IWeakReferenceSource*)textView->Probe(EIID_IWeakReferenceSource);
    wrs->GetWeakReference((IWeakReference**)&mTextView);
}

ECode UpdateTextServicesLocaleRunnable::Run()
{
    AutoPtr<IInterface> obj;
    mTextView->Resolve(EIID_ITextView, (IInterface**)&obj);
    if (obj) {
        TextView* tv = (TextView*)ITextView::Probe(obj);
        if (tv) {
            tv->UpdateTextServicesLocaleLocked();
        }
        else {
            Logger::E("TextView", "TextView's subclass does not implement Probe EIID_TextView!");
            assert(0);
        }
    }

    return NOERROR;
}

//==============================================================================
//              CompressTextRunnable
//==============================================================================

CompressTextRunnable::CompressTextRunnable(
    /* [in] */ TextView* textView)
{
    AutoPtr<IWeakReferenceSource> wrs = (IWeakReferenceSource*)textView->Probe(EIID_IWeakReferenceSource);
    wrs->GetWeakReference((IWeakReference**)&mTextView);
}

ECode CompressTextRunnable::Run()
{
    AutoPtr<IInterface> temp;
    mTextView->Resolve(EIID_ITextView, (IInterface**)&temp);
    AutoPtr<ITextView> textView = ITextView::Probe(temp);
    if (textView) {
        IView::Probe(textView)->RequestLayout();
    }
    return NOERROR;
}

//==============================================================================
//          Drawables
//==============================================================================

Drawables::Drawables()
    : mIsRtlCompatibilityMode(FALSE)
    , mOverride(FALSE)
    , mDrawableSizeTop(0)
    , mDrawableSizeBottom(0)
    , mDrawableSizeLeft(0)
    , mDrawableSizeRight(0)
    , mDrawableSizeStart(0)
    , mDrawableSizeEnd(0)
    , mDrawableSizeError(0)
    , mDrawableSizeTemp(0)
    , mDrawableWidthTop(0)
    , mDrawableWidthBottom(0)
    , mDrawableHeightLeft(0)
    , mDrawableHeightRight(0)
    , mDrawableHeightStart(0)
    , mDrawableHeightEnd(0)
    , mDrawableHeightError(0)
    , mDrawableHeightTemp(0)
    , mDrawablePadding(0)
    , mDrawableSaved(DRAWABLE_NONE)
{
    CRect::New((IRect**)&mCompoundRect);
}

Drawables::Drawables(
    /* [in] */ IContext* context)
    : mIsRtlCompatibilityMode(FALSE)
    , mOverride(FALSE)
    , mDrawableSizeTop(0)
    , mDrawableSizeBottom(0)
    , mDrawableSizeLeft(0)
    , mDrawableSizeRight(0)
    , mDrawableSizeStart(0)
    , mDrawableSizeEnd(0)
    , mDrawableSizeError(0)
    , mDrawableSizeTemp(0)
    , mDrawableWidthTop(0)
    , mDrawableWidthBottom(0)
    , mDrawableHeightLeft(0)
    , mDrawableHeightRight(0)
    , mDrawableHeightStart(0)
    , mDrawableHeightEnd(0)
    , mDrawableHeightError(0)
    , mDrawableHeightTemp(0)
    , mDrawablePadding(0)
    , mDrawableSaved(DRAWABLE_NONE)
{
    CRect::New((IRect**)&mCompoundRect);
    AutoPtr<IApplicationInfo> info;
    context->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 targetSdkVersion;
    info->GetTargetSdkVersion(&targetSdkVersion);

    Boolean support;
    info->HasRtlSupport(&support);

    mIsRtlCompatibilityMode = targetSdkVersion < Build::VERSION_CODES::JELLY_BEAN_MR1 || !support;

    mOverride = FALSE;
}

ECode Drawables::ResolveWithLayoutDirection(
        /* [in] */ Int32 layoutDirection)
{
    mDrawableLeft = mDrawableLeftInitial;
    mDrawableRight = mDrawableRightInitial;

    if (mIsRtlCompatibilityMode) {
                // Use "start" drawable as "left" drawable if the "left" drawable was not defined
        if (mDrawableStart && !mDrawableLeft) {
            mDrawableLeft = mDrawableStart;
            mDrawableSizeLeft = mDrawableSizeStart;
            mDrawableHeightLeft = mDrawableHeightStart;
        }
        // Use "end" drawable as "right" drawable if the "right" drawable was not defined
        if (mDrawableEnd && !mDrawableRight) {
            mDrawableRight = mDrawableEnd;
            mDrawableSizeRight = mDrawableSizeEnd;
            mDrawableHeightRight = mDrawableHeightEnd;
        }
    } else {
        // JB-MR1+ normal case: "start" / "end" drawables are overriding "left" / "right"
        // drawable if and only if they have been defined
        switch(layoutDirection) {
            case IView::LAYOUT_DIRECTION_RTL:
                if (mOverride) {
                    mDrawableRight = mDrawableStart;
                    mDrawableSizeRight = mDrawableSizeStart;
                    mDrawableHeightRight = mDrawableHeightStart;

                    mDrawableLeft = mDrawableEnd;
                    mDrawableSizeLeft = mDrawableSizeEnd;
                    mDrawableHeightLeft = mDrawableHeightEnd;
                }
                break;

            case IView::LAYOUT_DIRECTION_LTR:
            default:
                if (mOverride) {
                    mDrawableLeft = mDrawableStart;
                    mDrawableSizeLeft = mDrawableSizeStart;
                    mDrawableHeightLeft = mDrawableHeightStart;

                    mDrawableRight = mDrawableEnd;
                    mDrawableSizeRight = mDrawableSizeEnd;
                    mDrawableHeightRight = mDrawableHeightEnd;
                }
                break;
        }
    }
    ApplyErrorDrawableIfNeeded(layoutDirection);
    UpdateDrawablesLayoutDirection(layoutDirection);

    return NOERROR;
}

void Drawables::UpdateDrawablesLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    if (mDrawableLeft != NULL) {
        mDrawableLeft->SetLayoutDirection(layoutDirection);
    }
    if (mDrawableRight != NULL) {
        mDrawableRight->SetLayoutDirection(layoutDirection);
    }
    if (mDrawableTop != NULL) {
        mDrawableTop->SetLayoutDirection(layoutDirection);
    }
    if (mDrawableBottom != NULL) {
        mDrawableBottom->SetLayoutDirection(layoutDirection);
    }
}

ECode Drawables::SetErrorDrawable(
        /* [in] */ IDrawable* dr,
        /* [in] */ ITextView*tv)
{
    if (dr != mDrawableError && mDrawableError != NULL) {
        mDrawableError->SetCallback(NULL);
    }
    mDrawableError = dr;

    AutoPtr<ArrayOf<Int32> > state;
    IView::Probe(tv)->GetDrawableState((ArrayOf<Int32>**)&state);

    if (mDrawableError != NULL) {
        Boolean rst;
        mDrawableError->SetState(state, &rst);
        mDrawableError->CopyBounds(mCompoundRect);
        mDrawableError->SetCallback(IDrawableCallback::Probe(tv));
        mCompoundRect->GetWidth(&mDrawableSizeError);
        mCompoundRect->GetHeight(&mDrawableHeightError);
    } else {
        mDrawableSizeError = mDrawableHeightError = 0;
    }
    return NOERROR;
}

void Drawables::ApplyErrorDrawableIfNeeded(
        /* [in] */ Int32 layoutDirection)
{
    // first restore the initial state if needed
    switch (mDrawableSaved) {
        case DRAWABLE_LEFT:
            mDrawableLeft = mDrawableTemp;
            mDrawableSizeLeft = mDrawableSizeTemp;
            mDrawableHeightLeft = mDrawableHeightTemp;
            break;
        case DRAWABLE_RIGHT:
            mDrawableRight = mDrawableTemp;
            mDrawableSizeRight = mDrawableSizeTemp;
            mDrawableHeightRight = mDrawableHeightTemp;
            break;
        case DRAWABLE_NONE:
        default:
            break;
    }
    // then, if needed, assign the Error drawable to the correct location
    if (mDrawableError != NULL) {
        switch(layoutDirection) {
            case IView::LAYOUT_DIRECTION_RTL:
                mDrawableSaved = DRAWABLE_LEFT;

                mDrawableTemp = mDrawableLeft;
                mDrawableSizeTemp = mDrawableSizeLeft;
                mDrawableHeightTemp = mDrawableHeightLeft;

                mDrawableLeft = mDrawableError;
                mDrawableSizeLeft = mDrawableSizeError;
                mDrawableHeightLeft = mDrawableHeightError;
                break;
            case IView::LAYOUT_DIRECTION_LTR:
            default:
                mDrawableSaved = DRAWABLE_RIGHT;

                mDrawableTemp = mDrawableRight;
                mDrawableSizeTemp = mDrawableSizeRight;
                mDrawableHeightTemp = mDrawableHeightRight;

                mDrawableRight = mDrawableError;
                mDrawableSizeRight = mDrawableSizeError;
                mDrawableHeightRight = mDrawableHeightError;
                break;
        }
    }
}

//==============================================================================
//          CharWrapper
//==============================================================================

CAR_INTERFACE_IMPL_3(CharWrapper, Object, ICharSequence, IGetChars, IGraphicsOperations)

CharWrapper::CharWrapper(
    /* [in] */ const ArrayOf<Char32>* chars,
    /* [in] */ Int32 start,
    /* [in] */ Int32 len)
    : mStart(start)
    , mLength(len)
{
    if (chars != NULL)
        mChars = chars->Clone();
    else
        mChars = NULL;
}

void CharWrapper::Set(
    /* [in] */ const ArrayOf<Char32>* chars,
    /* [in] */ Int32 start,
    /* [in] */ Int32 len)
{
    if (chars != NULL)
        mChars = chars->Clone();
    else
        mChars = NULL;
    mStart = start;
    mLength = len;
}

ECode CharWrapper::GetLength(
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len);
    *len = mLength;
    return NOERROR;
}

ECode CharWrapper::GetCharAt(
    /* [in] */ Int32 off,
    /* [out] */ Char32* c)
{
    VALIDATE_NOT_NULL(c);
    if (mChars == NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (off < 0 || off >= mChars->GetLength()) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *c = (*mChars)[off];
    return NOERROR;
}

ECode CharWrapper::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    if (mChars != NULL) {
        StringBuilder sb;
        sb.Append(*mChars, mStart, mLength);
        *str = sb.ToString();
    }

    return NOERROR;
}

ECode CharWrapper::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq);
    if (start < 0 || end < 0 || start > mLength || end > mLength) {
        //throw new IndexOutOfBoundsException(start + ", " + end);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (mChars != NULL) {
        StringBuilder sb;
        sb.Append(*mChars, mStart + start, end - start);
        AutoPtr<ICharSequence> seq = sb.ToCharSequence();
        *csq = seq;
        REFCOUNT_ADD(*csq);
        return NOERROR;
    }
    *csq = NULL;
    return NOERROR;
}

ECode CharWrapper::GetChars(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ArrayOf<Char32>* dest,
    /* [in] */ Int32 destoff)
{
    if (dest == NULL || start < 0 || end < 0 || start > mLength || end > mLength) {
        //throw new IndexOutOfBoundsException(start + ", " + end);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    dest->Copy(destoff, mChars, start + mStart, end - start);
    return NOERROR;
}

ECode CharWrapper::DrawText(
    /* [in] */ ICanvas* c,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPaint* p)
{
    VALIDATE_NOT_NULL(c);
    return c->DrawText((ArrayOf<Char32>*)mChars, start + mStart, end - start, x, y, p);
}

ECode CharWrapper::DrawTextRun(
    /* [in] */ ICanvas* c,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Boolean isRtl,
    /* [in] */ IPaint* p)
{
    VALIDATE_NOT_NULL(c);
    Int32 count = end - start;
    Int32 contextCount = contextEnd - contextStart;
    return c->DrawTextRun((ArrayOf<Char32>*)mChars, start + mStart, count, contextStart + mStart,
            contextCount, x, y, isRtl, p);
}

ECode CharWrapper::MeasureText(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ IPaint* p,
    /* [out] */ Float* width)
{
    VALIDATE_NOT_NULL(width);
    VALIDATE_NOT_NULL(p);
    return p->MeasureText((ArrayOf<Char32>*)mChars, start + mStart, end - start, width);
}

ECode CharWrapper::GetTextWidths(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out, callee] */ ArrayOf<Float>** widths,
    /* [in] */ IPaint* p,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(p);
    VALIDATE_NOT_NULL(widths);
    VALIDATE_NOT_NULL(count);
    *count = 0;
    *widths = NULL;

    if (end <= start || start < 0) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    *widths = ArrayOf<Float>::Alloc(end - start);
    REFCOUNT_ADD(*widths);
    return p->GetTextWidths((ArrayOf<Char32>*)mChars, start + mStart, end - start, *widths, count);
}

ECode CharWrapper::GetTextRunAdvances(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Boolean isRtl,
    /* [out] */ ArrayOf<Float>* advances,
    /* [in] */ Int32 advancesIndex,
    /* [in] */ IPaint* p,
    /* [out] */ Float* advance)
{
    VALIDATE_NOT_NULL(p);
    VALIDATE_NOT_NULL(advances);
    VALIDATE_NOT_NULL(advance);

    Int32 count = end - start;
    Int32 contextCount = contextEnd - contextStart;
    return p->GetTextRunAdvances((ArrayOf<Char32>*)mChars, start + mStart, count,
            contextStart + mStart, contextCount, isRtl, advances,
            advancesIndex, advance);
}

ECode CharWrapper::GetTextRunCursor(
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Int32 dir,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 cursorOpt,
    /* [in] */ IPaint* p,
    /* [out] */ Int32* cursor)
{
    VALIDATE_NOT_NULL(cursor);
    Int32 contextCount = contextEnd - contextStart;
    return p->GetTextRunCursor((ArrayOf<Char32>*)mChars, contextStart + mStart,
            contextCount, dir, offset + mStart, cursorOpt, cursor);
}

//==============================================================================
//          Marquee::TickCallback
//==============================================================================

CAR_INTERFACE_IMPL(Marquee::TickCallback, Object, IFrameCallback)

Marquee::TickCallback::TickCallback(
    /* [in] */ Marquee* host)
    : mHost(host)
{}

ECode Marquee::TickCallback::DoFrame(
    /* [in] */ Int64 frameTimeNanos)
{
    mHost->Tick();
    return NOERROR;
}

//==============================================================================
//          Marquee::StartCallback
//==============================================================================

CAR_INTERFACE_IMPL(Marquee::StartCallback, Object, IFrameCallback)

Marquee::StartCallback::StartCallback(
    /* [in] */ Marquee* host)
    : mHost(host)
{}

ECode Marquee::StartCallback::DoFrame(
    /* [in] */ Int64 frameTimeNanos)
{
    mHost->mStatus = mHost->MARQUEE_RUNNING;
    mHost->mChoreographer->GetFrameTime(&(mHost->mLastAnimationMs));
    mHost->Tick();
    return NOERROR;
}

//==============================================================================
//          Marquee::RestartCallback
//==============================================================================

CAR_INTERFACE_IMPL(Marquee::RestartCallback, Object, IFrameCallback)

Marquee::RestartCallback::RestartCallback(
    /* [in] */ Marquee* host)
    : mHost(host)
{}

ECode Marquee::RestartCallback::DoFrame(
    /* [in] */ Int64 frameTimeNanos)
{
    if (mHost->mStatus == mHost->MARQUEE_RUNNING) {
        if (mHost->mRepeatLimit >= 0) {
            mHost->mRepeatLimit--;
        }
        mHost->Start(mHost->mRepeatLimit);
    }

    return NOERROR;
}

//==============================================================================
//          Marquee
//==============================================================================

Marquee::Marquee(
    /* [in] */ ITextView* v)
    : mStatus(0x0)
    , mPixelsPerSecond(0)
    , mMaxScroll(0)
    , mMaxFadeScroll(0)
    , mGhostStart(0)
    , mGhostOffset(0)
    , mFadeStop(0)
    , mRepeatLimit(0)
    , mScroll(0)
    , mLastAnimationMs(0)
{
    AutoPtr<IContext> context;
    IView::Probe(v)->GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> dm;
    resources->GetDisplayMetrics((IDisplayMetrics**)&dm);

    Float density;
    dm->GetDensity(&density);
    mPixelsPerSecond = MARQUEE_DP_PER_SECOND * density;
    assert(0);
    //mChoreographer = Choreographer::GetInstance();

    AutoPtr<IWeakReferenceSource> source = IWeakReferenceSource::Probe(v);
    if (source) {
        source->GetWeakReference((IWeakReference**)&mView);
    }

    mTickCallback = new TickCallback(this);
    mStartCallback = new StartCallback(this);
    mRestartCallback = new RestartCallback(this);
}

void Marquee::Tick()
{
    if (mStatus != MARQUEE_RUNNING) {
        return;
    }

    mChoreographer->RemoveFrameCallback(mTickCallback);

    AutoPtr<IInterface> obj;
    mView->Resolve(EIID_IInterface, (IInterface**)&obj);
    if (obj != NULL) {
        TextView* textView = (TextView*)ITextView::Probe(obj);
        if (textView != NULL) {
            Boolean isFocused, isSelected;
            if ((IView::Probe(textView)->IsFocused(&isFocused), isFocused)
                || (IView::Probe(textView)->IsSelected(&isSelected), isSelected)) {
                Int64 currentMs;
                mChoreographer->GetFrameTime(&currentMs);
                Int64 deltaMs = currentMs - mLastAnimationMs;
                mLastAnimationMs = currentMs;
                Float deltaPx = deltaMs / 1000.0f * mPixelsPerSecond;
                mScroll += deltaPx;
                if (mScroll > mMaxScroll) {
                    mScroll = mMaxScroll;
                    mChoreographer->PostFrameCallbackDelayed(mRestartCallback, MARQUEE_DELAY);
                }
                else {
                    mChoreographer->PostFrameCallback(mTickCallback);
                }
                textView->Invalidate();
            }
        }
    }
}

void Marquee::Stop()
{
    mStatus = MARQUEE_STOPPED;
    mChoreographer->RemoveFrameCallback(mStartCallback);
    mChoreographer->RemoveFrameCallback(mRestartCallback);
    mChoreographer->RemoveFrameCallback(mTickCallback);
    ResetScroll();
}

void Marquee::Start(
    /* [in] */ Int32 repeatLimit)
{
    if (repeatLimit == 0) {
        Stop();
        return;
    }

    mRepeatLimit = repeatLimit;

    AutoPtr<IInterface> obj;
    mView->Resolve(EIID_IInterface, (IInterface**)&obj);
    if (obj == NULL) return;

    TextView* textView = (TextView*)ITextView::Probe(obj);
    if (textView == NULL) return;

    if (textView->mLayout != NULL) {
        mStatus = MARQUEE_STARTING;
        mScroll = 0.0f;
        Int32 w, l, r;
        IView::Probe(textView)->GetWidth(&w);
        textView->GetCompoundPaddingLeft(&l);
        textView->GetCompoundPaddingRight(&r);
        Int32 textWidth = w - l - r;
        Float lineWidth;
        textView->mLayout->GetLineWidth(0, &lineWidth);
        Float gap = textWidth / 3.0f;
        mGhostStart = lineWidth - textWidth + gap;
        mMaxScroll = mGhostStart + textWidth;
        mGhostOffset = lineWidth + gap;
        mFadeStop = lineWidth + textWidth / 6.0f;
        mMaxFadeScroll = mGhostStart + lineWidth + lineWidth;

        textView->Invalidate();
        mChoreographer->PostFrameCallback(mStartCallback);
    }
}

Float Marquee::GetGhostOffset()
{
    return mGhostOffset;
}

Float Marquee::GetScroll()
{
    return mScroll;
}

Float Marquee::GetMaxFadeScroll()
{
    return mMaxFadeScroll;
}

Boolean Marquee::ShouldDrawLeftFade()
{
    return mScroll <= mFadeStop;
}

Boolean Marquee::ShouldDrawGhost()
{
    return mStatus == MARQUEE_RUNNING && mScroll > mGhostStart;
}

Boolean Marquee::IsRunning()
{
    return mStatus == MARQUEE_RUNNING;
}

Boolean Marquee::IsStopped()
{
    return mStatus == MARQUEE_STOPPED;
}

void Marquee::ResetScroll()
{
    mScroll = 0.0f;

    AutoPtr<IInterface> obj;
    mView->Resolve(EIID_IInterface, (IInterface**)&obj);
    if (obj != NULL) {
        TextView* textView = (TextView*)ITextView::Probe(obj);
        if (textView != NULL) {
            textView->Invalidate();
        }
    }
}

//==============================================================================
//          ChangeWatcher
//==============================================================================

CAR_INTERFACE_IMPL_2(ChangeWatcher, Object, ITextWatcher, ISpanWatcher)

ChangeWatcher::ChangeWatcher(
    /* [in] */ TextView* host)
    : mHost(host)
{}

ChangeWatcher::~ChangeWatcher()
{
}

ECode ChangeWatcher::BeforeTextChanged(
    /* [in] */ ICharSequence* buffer,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 after)
{
    if (mHost->DEBUG_EXTRACT) {
        String str;
        buffer->ToString(&str);
        Logger::V("TextView", "beforeTextChanged start=%d before=%d after=%d: %s",
                start, before, after, str.string());
    }

    AutoPtr<IAccessibilityManager> mgr;
    CAccessibilityManager::GetInstance(mHost->mContext, (IAccessibilityManager**)&mgr);
    Boolean isEnabled = TRUE;
    mgr->IsEnabled(&isEnabled);
    Int32 inputType;
    if (isEnabled && ((!mHost->IsPasswordInputType
        ((mHost->GetInputType(&inputType), inputType)) && !mHost->HasPasswordTransformationMethod())
                    || mHost->ShouldSpeakPasswordsForAccessibility())) {
        mBeforeText = buffer;
    }

    mHost->SendBeforeTextChanged(buffer, start, before, after);
    return NOERROR;
}

ECode ChangeWatcher::OnTextChanged(
    /* [in] */ ICharSequence* buffer,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 after)
{
    if (mHost->DEBUG_EXTRACT) {
        String str;
        buffer->ToString(&str);
        Logger::V("TextView", "onTextChanged start=%d before=%d after=%d: %s",
            start, before, after, str.string());
    }

    mHost->HandleTextChanged(buffer, start, before, after);

    AutoPtr<IAccessibilityManager> mgr;
    CAccessibilityManager::GetInstance(mHost->mContext, (IAccessibilityManager**)&mgr);
    Boolean isEnabled = TRUE;
    mgr->IsEnabled(&isEnabled);
    Boolean isFocused, isSelected, isShown;
    if (isEnabled &&
        ((mHost->IsFocused(&isFocused), isFocused)
        || ((mHost->IsSelected(&isSelected), isSelected)
        && (mHost->IsShown(&isShown), isShown)))) {
        mHost->SendAccessibilityEventTypeViewTextChanged(mBeforeText, start, before, after);
        mBeforeText = NULL;
    }

    return NOERROR;
}

ECode ChangeWatcher::AfterTextChanged(
    /* [in] */ IEditable* buffer)
{
    if (mHost->DEBUG_EXTRACT) {
        String str;
        ICharSequence::Probe(buffer)->ToString(&str);
        Logger::V("TextView", "afterTextChanged:%s ", str.string());
    }

    mHost->SendAfterTextChanged(buffer);

    AutoPtr<IMetaKeyKeyListenerHelper> helper;
    CMetaKeyKeyListenerHelper::AcquireSingleton((IMetaKeyKeyListenerHelper**)&helper);
    Int32 state;
    helper->GetMetaState(ICharSequence::Probe(buffer), IMetaKeyKeyListener::META_SELECTING, &state);
    if (state != 0) {
        helper->StopSelecting(mHost, ISpannable::Probe(buffer));
    }

    return NOERROR;
}

ECode ChangeWatcher::OnSpanChanged(
    /* [in] */ ISpannable* buf,
    /* [in] */ IInterface* what,
    /* [in] */ Int32 s,
    /* [in] */ Int32 e,
    /* [in] */ Int32 st,
    /* [in] */ Int32 en)
{
    //if (DEBUG_EXTRACT) Logger::v(TEXT_VIEW_TAG, "onSpanChanged s=" + s + " e=" + e
    //        + " st=" + st + " en=" + en + " what=" + what + ": " + buf);
    return mHost->SpanChange(ISpanned::Probe(buf), what, s, st, e, en);
}

ECode ChangeWatcher::OnSpanAdded(
    /* [in] */ ISpannable* buf,
    /* [in] */ IInterface* what,
    /* [in] */ Int32 s,
    /* [in] */ Int32 e)
{
    //if (DEBUG_EXTRACT) Logger::v(TEXT_VIEW_TAG, "onSpanAdded s=" + s + " e=" + e
    //        + " what=" + what + ": " + buf);
    return mHost->SpanChange(ISpanned::Probe(buf), what, -1, s, -1, e);
}

ECode ChangeWatcher::OnSpanRemoved(
    /* [in] */ ISpannable* buf,
    /* [in] */ IInterface* what,
    /* [in] */ Int32 s,
    /* [in] */ Int32 e)
{
    //if (DEBUG_EXTRACT) Logger::v(TEXT_VIEW_TAG, "onSpanRemoved s=" + s + " e=" + e
    //        + " what=" + what + ": " + buf);
    return  mHost->SpanChange(ISpanned::Probe(buf), what, s, -1, e, -1);
}

//==============================================================================
//          TextView
//==============================================================================

CAR_INTERFACE_IMPL_2(TextView, View, ITextView, IOnPreDrawListener)

static AutoPtr<IRectF> InitCRectF()
{
    AutoPtr<IRectF> r;
    CRectF::New((IRectF**)&r);
    return r;
}

static AutoPtr<ArrayOf<Elastos::Droid::Text::IInputFilter*> > InitNO_FILTERS()
{
    AutoPtr<ArrayOf<Elastos::Droid::Text::IInputFilter*> > filters
        = ArrayOf<Elastos::Droid::Text::IInputFilter*>::Alloc(0);
    return filters;
}

static AutoPtr<ISpanned> InitISpanned()
{
    AutoPtr<ICharSequence> seq;
    CString::New(String(""), (ICharSequence**)&seq);
    static AutoPtr<ISpannedString> str;
    CSpannedString::New(seq, (ISpannedString**)&str);
    static AutoPtr<ISpanned> spanned = ISpanned::Probe(str);

    return spanned;
}

static AutoPtr<ArrayOf<Int32> > InitMULTILINE_STATE_SET()
{
    AutoPtr<ArrayOf<Int32> > sets = ArrayOf<Int32>::Alloc(1);
    (*sets)[0] = R::attr::state_multiline;
    return sets;
}

static AutoPtr<IBoringLayoutMetrics> InitUNKNOWN_BORING()
{
    AutoPtr<CBoringLayoutMetrics> metrics;
    CBoringLayoutMetrics::NewByFriend((CBoringLayoutMetrics**)&metrics);
    return (IBoringLayoutMetrics*)metrics.Get();
}

const char* TextView::TEXT_VIEW_TAG = "TextView";
const Boolean TextView::DEBUG_EXTRACT = FALSE;
const Int32 TextView::SANS;
const Int32 TextView::SERIF;
const Int32 TextView::MONOSPACE;
const Int32 TextView::SIGNED;
const Int32 TextView::DECIMAL;

const Int32 TextView::MARQUEE_FADE_NORMAL;
const Int32 TextView::MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS;
const Int32 TextView::MARQUEE_FADE_SWITCH_SHOW_FADE;

const Int32 TextView::LINES;
const Int32 TextView::EMS;
const Int32 TextView::PIXELS;

const AutoPtr<IRectF> TextView::TEMP_RECTF = InitCRectF();
Object TextView::sTempRectLock;

const Int32 TextView::VERY_WIDE;
const Int32 TextView::ANIMATED_SCROLL_GAP;

AutoPtr<ArrayOf<Elastos::Droid::Text::IInputFilter*> > TextView::NO_FILTERS = InitNO_FILTERS();
AutoPtr<ISpanned> TextView::EMPTY_SPANNED = InitISpanned();

const Int32 TextView::CHANGE_WATCHER_PRIORITY;

AutoPtr<ArrayOf<Int32> > TextView::MULTILINE_STATE_SET = InitMULTILINE_STATE_SET();

Int64 TextView::LAST_CUT_OR_COPY_TIME = 0;

AutoPtr<IBoringLayoutMetrics> TextView::UNKNOWN_BORING = InitUNKNOWN_BORING();

//==============================================================================

/*
 * Kick-start the font cache for the zygote process (to pay the cost of
 * initializing freetype for our default font only once).
 */
void TextView::InitStatic()
{
//static {
//    Paint p = new Paint();
//    p.setAntiAlias(true);
//    // We don't care about the result, just the side-effect of measuring.
//    p.measureText("H");
//}
}

void TextView::InitFields()
{
    mHighlightColor = (0xCC475925);
    mCursorDrawableRes = 0;

    mTextSelectHandleLeftRes = 0;
    mTextSelectHandleRightRes = 0;
    mTextSelectHandleRes = 0;
    mTextEditSuggestionItemLayout = 0;

    mCurTextColor = (0);
    mCurHintTextColor = 0;
    mFreezesText = (FALSE);
    mTemporaryDetach = (FALSE);
    mDispatchTemporaryDetach = (FALSE);

    mShadowRadius = 0;
    mShadowDx = 0;
    mShadowDy = 0;

    mPreDrawRegistered = FALSE;

    mEllipsize = TextUtilsTruncateAt_NONE;

    mRestartMarquee = (FALSE);

    mMarqueeRepeatLimit = (3);

    mLastLayoutDirection = -1;

    mMarqueeFadeMode = MARQUEE_FADE_NORMAL;

    mBufferType = (BufferType_NORMAL);
    mAllowTransformationLengthChange = FALSE;
    mUserSetTextScaleX = FALSE;

    mGravity = (IGravity::TOP | IGravity::START);
    mHorizontallyScrolling = FALSE;

    mAutoLinkMask = 0;
    mLinksClickable = TRUE;

    mSpacingMult = 1.0f;
    mSpacingAdd = 0.0f;

    mMaximum = Elastos::Core::Math::INT32_MAX_VALUE;
    mMaxMode = LINES;
    mMinimum = 0;
    mMinMode = LINES;

    mOldMaximum = mMaximum;
    mOldMaxMode = mMaxMode;

    mMaxWidth = Elastos::Core::Math::INT32_MAX_VALUE;
    mMaxWidthMode = PIXELS;
    mMinWidth = 0;
    mMinWidthMode = PIXELS;

    mSingleLine = FALSE;
    mDesiredHeightAtMeasure = -1;
    mIncludePad = TRUE;
    mDeferScroll = -1;

    mLastScroll = 0;

    mHighlightPathBogus = TRUE;

    mFilters = NO_FILTERS;

    ASSERT_SUCCEEDED(CString::New(String(""), (ICharSequence**)&mText));
    ASSERT_SUCCEEDED(CSpannableFactory::AcquireSingleton(
       (ISpannableFactory**)&mSpannableFactory));
    ASSERT_SUCCEEDED(CEditableFactory::AcquireSingleton(
       (IEditableFactory**)&mEditableFactory));
}

TextView::TextView()
{
    InitFields();
}

ECode TextView::constructor(
    /* [in] */ IContext* context)
{
    constructor(context, NULL);
    return NOERROR;
}

ECode TextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    constructor(context, attrs, R::attr::textViewStyle);
    return NOERROR;
}

ECode TextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    constructor(context, attrs, defStyle, 0);
    return NOERROR;
}

ECode TextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    InitFields();
    ASSERT_SUCCEEDED(View::constructor(context, attrs, defStyleAttr, defStyleRes));
    ASSERT_SUCCEEDED(InitFromAttributes(context, attrs, defStyleAttr, defStyleRes));
    return NOERROR;
}

ECode TextView::InitFromAttributes(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    VALIDATE_NOT_NULL(context);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<ICompatibilityInfo> compat;
    res->GetCompatibilityInfo((ICompatibilityInfo**)&compat);
    Float applicationScale;
    compat->GetApplicationScale(&applicationScale);

    AutoPtr<IDisplayMetrics> dm;
    AutoPtr<ICompatibilityInfo> ci;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    res->GetCompatibilityInfo((ICompatibilityInfo**)&ci);
    Float density;
    dm->GetDensity(&density);

    CTextPaint::New(IPaint::ANTI_ALIAS_FLAG, (ITextPaint**)&mTextPaint);
    mTextPaint->SetDensity(density);
    IPaint::Probe(mTextPaint)->SetCompatibilityScaling(applicationScale);

    ASSERT_SUCCEEDED(CPaint::New(IPaint::ANTI_ALIAS_FLAG, (IPaint**)&mHighlightPaint));
    mHighlightPaint->SetCompatibilityScaling(applicationScale);

    mMovement = GetDefaultMovementMethod();
    mTransformation = NULL;

    Int32 textColorHighlight = 0;
    AutoPtr<IColorStateList> textColor;
    AutoPtr<IColorStateList> textColorHint;
    AutoPtr<IColorStateList> textColorLink;
    Int32 textSize = 15;
    String fontFamily;
    Int32 typefaceIndex = -1;
    Int32 styleIndex = -1;
    Boolean allCaps = FALSE;

    Int32 shadowcolor = 0;
    Float dx = 0, dy = 0, r = 0;
    Boolean elegant = FALSE;
    Float letterSpacing = 0;
    String fontFeatureSettings;

    AutoPtr<IResourcesTheme> theme;
    context->GetTheme((IResourcesTheme**)&theme);

    /*
     * Look the appearance up without checking first if it exists because
     * almost every TextView has one and it greatly simplifies the logic
     * to be able to parse the appearance first and then let specific tags
     * for this View override it.
     */
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::TextViewAppearance),
            ArraySize(R::styleable::TextViewAppearance));
    AutoPtr<ITypedArray> a;
    theme->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    AutoPtr<ITypedArray> appearance;
    Int32 ap;
    a->GetResourceId(R::styleable::TextViewAppearance_textAppearance, -1, &ap);
    a->Recycle();
    if (ap != -1) {
        attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::TextAppearance),
            ArraySize(R::styleable::TextAppearance));
        theme->ObtainStyledAttributes(ap, attrIds, (ITypedArray**)&appearance);
    }

    if (appearance != NULL) {
        Int32 n;
        appearance->GetIndexCount(&n);
        for (Int32 i = 0; i < n; i++) {
            Int32 attr;
            appearance->GetIndex(i, &attr);

            switch (attr) {
            case R::styleable::TextAppearance_textColorHighlight:
                appearance->GetColor(attr, textColorHighlight, &textColorHighlight);
                break;

            case R::styleable::TextAppearance_textColor:
                appearance->GetColorStateList(attr, (IColorStateList**)&textColor);
                break;

            case R::styleable::TextAppearance_textColorHint:
                appearance->GetColorStateList(attr, (IColorStateList**)&textColorHint);
                break;

            case R::styleable::TextAppearance_textColorLink:
                appearance->GetColorStateList(attr, (IColorStateList**)&textColorLink);
                break;

            case R::styleable::TextAppearance_textSize:
                appearance->GetDimensionPixelSize(attr, textSize, &textSize);
                break;

            case R::styleable::TextAppearance_typeface:
                appearance->GetInt32(attr, -1, &typefaceIndex);
                break;

            case R::styleable::TextAppearance_fontFamily:
                appearance->GetString(attr, &fontFamily);
                break;

            case R::styleable::TextAppearance_textStyle:
                appearance->GetInt32(attr, -1, &styleIndex);
                break;

            case R::styleable::TextAppearance_textAllCaps:
                appearance->GetBoolean(attr, FALSE, &allCaps);
                break;

            case R::styleable::TextAppearance_shadowColor:
                appearance->GetInt32(attr, 0, &shadowcolor);
                break;

            case R::styleable::TextAppearance_shadowDx:
                appearance->GetFloat(attr, 0, &dx);
                break;

            case R::styleable::TextAppearance_shadowDy:
                appearance->GetFloat(attr, 0, &dy);
                break;

            case R::styleable::TextAppearance_shadowRadius:
                appearance->GetFloat(attr, 0, &r);
                break;

            case R::styleable::TextAppearance_elegantTextHeight:
                appearance->GetBoolean(attr, FALSE, &elegant);
                break;

            case R::styleable::TextAppearance_letterSpacing:
                appearance->GetFloat(attr, 0, &letterSpacing);
                break;

            case R::styleable::TextAppearance_fontFeatureSettings:
                appearance->GetString(attr, &fontFeatureSettings);
                break;
            }
        }

        appearance->Recycle();
    }

    Boolean editable = GetDefaultEditable();
    AutoPtr<ICharSequence> inputMethod;
    Int32 numeric = 0;
    AutoPtr<ICharSequence> digits;
    Boolean phone = FALSE;
    Boolean autotext = FALSE;
    Int32 autocap = -1;
    Int32 buffertype = 0;
    Boolean selectallonfocus = FALSE;
    AutoPtr<IDrawable> drawableLeft, drawableTop, drawableRight, drawableBottom, drawableStart, drawableEnd;
    Int32 drawablePadding = 0;
    Int32 ellipsize = TextUtilsTruncateAt_NONE;
    Boolean singleLine = FALSE;
    Int32 maxlength = -1;
    AutoPtr<ICharSequence> text;
    CString::New(String(""), (ICharSequence**)&text);
    AutoPtr<ICharSequence> hint;

    Boolean password = FALSE;
    Int32 inputType = IInputType::TYPE_NULL;

    a = NULL;
    attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::TextView),
            ArraySize(R::styleable::TextView));
    theme->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    //TODO  typedarray should has TextView_minHeight value, but now it has not
    SetMinHeight(35);

    Int32 n;
    a->GetIndexCount(&n);
    for (Int32 i = 0; i < n; i++) {
        Int32 attr;
        a->GetIndex(i, &attr);

        Int32 ivalue;
        Boolean bValue;
        Float fValue;
        switch (attr) {
        case R::styleable::TextView_editable:
            a->GetBoolean(attr, editable, &editable);
            break;

        case R::styleable::TextView_inputMethod:
            inputMethod = NULL;
            a->GetText(attr, (ICharSequence**)&inputMethod);
            break;

        case R::styleable::TextView_numeric:
            a->GetInt32(attr, numeric, &numeric);
            break;

        case R::styleable::TextView_digits:
            digits = NULL;
            a->GetText(attr, (ICharSequence**)&digits);
            break;

        case R::styleable::TextView_phoneNumber:
            a->GetBoolean(attr, phone, &phone);
            break;

        case R::styleable::TextView_autoText:
            a->GetBoolean(attr, autotext, &autotext);
            break;

        case R::styleable::TextView_capitalize:
            a->GetInt32(attr, autocap, &autocap);
            break;

        case R::styleable::TextView_bufferType:
            a->GetInt32(attr, buffertype, &buffertype);
            break;

        case R::styleable::TextView_selectAllOnFocus:
            a->GetBoolean(attr, selectallonfocus, &selectallonfocus);
            break;

        case R::styleable::TextView_autoLink:
            a->GetInt32(attr, 0, &mAutoLinkMask);
            break;

        case R::styleable::TextView_linksClickable:
            a->GetBoolean(attr, TRUE, &mLinksClickable);
            break;

        case R::styleable::TextView_drawableLeft:
            drawableLeft = NULL;
            a->GetDrawable(attr, (IDrawable**)&drawableLeft);
            break;

        case R::styleable::TextView_drawableTop:
            drawableTop = drawableTop;
            a->GetDrawable(attr, (IDrawable**)&drawableTop);
            break;

        case R::styleable::TextView_drawableRight:
            drawableRight = NULL;
            a->GetDrawable(attr, (IDrawable**)&drawableRight);
            break;

        case R::styleable::TextView_drawableBottom:
            drawableBottom = NULL;
            a->GetDrawable(attr, (IDrawable**)&drawableBottom);
            break;

        case R::styleable::TextView_drawableStart:
            drawableStart = NULL;
            a->GetDrawable(attr, (IDrawable**)&drawableStart);
            break;

        case R::styleable::TextView_drawableEnd:
            drawableEnd = NULL;
            a->GetDrawable(attr, (IDrawable**)&drawableEnd);
            break;

        case R::styleable::TextView_drawablePadding:
            a->GetDimensionPixelSize(attr, drawablePadding, &drawablePadding);
            break;

        case R::styleable::TextView_maxLines:
            a->GetInt32(attr, -1, &ivalue);
            SetMaxLines(ivalue);
            break;
        case R::styleable::TextView_maxHeight:
            a->GetDimensionPixelSize(attr, -1, &ivalue);
            SetMaxHeight(ivalue);
            break;

        case R::styleable::TextView_lines:
            a->GetInt32(attr, -1, &ivalue);
            SetLines(ivalue);
            break;

        case R::styleable::TextView_height:
            a->GetDimensionPixelSize(attr, -1, &ivalue);
            SetHeight(ivalue);
            break;

        case R::styleable::TextView_minLines:
            a->GetInt32(attr, -1, &ivalue);
            SetMinLines(ivalue);
            break;

        case R::styleable::TextView_minHeight:
            a->GetDimensionPixelSize(attr, -1, &ivalue);
            SetMinHeight(ivalue);
            break;

        case R::styleable::TextView_maxEms:
            a->GetInt32(attr, -1, &ivalue);
            SetMaxEms(ivalue);
            break;

        case R::styleable::TextView_maxWidth:
            a->GetDimensionPixelSize(attr, -1, &ivalue);
            SetMaxWidth(ivalue);
            break;

        case R::styleable::TextView_ems:
            a->GetInt32(attr, -1, &ivalue);
            SetEms(ivalue);
            break;

        case R::styleable::TextView_width:
            a->GetDimensionPixelSize(attr, -1, &ivalue);
            SetWidth(ivalue);
            break;

        case R::styleable::TextView_minEms:
            a->GetInt32(attr, -1, &ivalue);
            SetMinEms(ivalue);
            break;

        case R::styleable::TextView_minWidth:
            a->GetDimensionPixelSize(attr, -1, &ivalue);
            SetMinWidth(ivalue);
            break;

        case R::styleable::TextView_gravity:
            a->GetInt32(attr, -1, &ivalue);
            SetGravity(ivalue);
            break;

        case R::styleable::TextView_hint:
            hint = NULL;
            a->GetText(attr, (ICharSequence**)&hint);
            break;

        case R::styleable::TextView_text:
            text = NULL;
            a->GetText(attr, (ICharSequence**)&text);
            break;

        case R::styleable::TextView_scrollHorizontally:
            a->GetBoolean(attr, FALSE, &bValue);
            if (bValue) {
                SetHorizontallyScrolling(TRUE);
            }
            break;

        case R::styleable::TextView_singleLine:
            a->GetBoolean(attr, singleLine, &singleLine);
            break;

        case R::styleable::TextView_ellipsize:
            a->GetInt32(attr, ellipsize, &ellipsize);
            break;

        case R::styleable::TextView_marqueeRepeatLimit:
            a->GetInt32(attr, mMarqueeRepeatLimit, &ivalue);
            SetMarqueeRepeatLimit(ivalue);
            break;

        case R::styleable::TextView_includeFontPadding:
            a->GetBoolean(attr, TRUE, &bValue);
            if (!bValue) {
                SetIncludeFontPadding(FALSE);
            }
            break;

        case R::styleable::TextView_cursorVisible:
            a->GetBoolean(attr, TRUE, &bValue);
            if (!bValue) {
                SetCursorVisible(FALSE);
            }
            break;

        case R::styleable::TextView_maxLength:
            a->GetInt32(attr, -1, &maxlength);
            break;

        case R::styleable::TextView_textScaleX:
            a->GetFloat(attr, 1.0f, &fValue);
            SetTextScaleX(fValue);
            break;

        case R::styleable::TextView_freezesText:
            a->GetBoolean(attr, FALSE, &mFreezesText);
            break;

        case R::styleable::TextView_shadowColor:
            a->GetInt32(attr, 0, &shadowcolor);
            break;

        case R::styleable::TextView_shadowDx:
            a->GetFloat(attr, 0, &dx);
            break;

        case R::styleable::TextView_shadowDy:
            a->GetFloat(attr, 0, &dy);
            break;

        case R::styleable::TextView_shadowRadius:
            a->GetFloat(attr, 0, &r);
            break;

        case R::styleable::TextView_enabled: {
            Boolean isEnabled;
            IsEnabled(&isEnabled);
            a->GetBoolean(attr, isEnabled, &bValue);
            SetEnabled(bValue);
            break;
        }

        case R::styleable::TextView_textColorHighlight:
            a->GetColor(attr, textColorHighlight, &textColorHighlight);
            break;

        case R::styleable::TextView_textColor:
            a->GetColorStateList(attr, (IColorStateList**)&textColor);
            break;

        case R::styleable::TextView_textColorHint:
            a->GetColorStateList(attr, (IColorStateList**)&textColorHint);
            break;

        case R::styleable::TextView_textColorLink:
            a->GetColorStateList(attr, (IColorStateList**)&textColorLink);
            break;

        case R::styleable::TextView_textSize:
            a->GetDimensionPixelSize(attr, textSize, &textSize);
            break;

        case R::styleable::TextView_typeface:
            a->GetInt32(attr, typefaceIndex, &typefaceIndex);
            break;

        case R::styleable::TextView_textStyle:
            a->GetInt32(attr, styleIndex, &styleIndex);
            break;

        case R::styleable::TextView_fontFamily:
            a->GetString(attr, &fontFamily);
            break;

        case R::styleable::TextView_password:
            a->GetBoolean(attr, password, &password);
            break;

        case R::styleable::TextView_lineSpacingExtra:
            a->GetDimensionPixelSize(attr, (Int32)mSpacingAdd, &ivalue);
            mSpacingAdd = ivalue;
            break;

        case R::styleable::TextView_lineSpacingMultiplier:
            a->GetFloat(attr, mSpacingMult, &mSpacingMult);
            break;

        case R::styleable::TextView_inputType:
            a->GetInt32(attr, inputType, &inputType);

            break;

        case R::styleable::TextView_imeOptions:
            CreateEditorIfNeeded();
            assert(0);
            /*mEditor->CreateInputContentTypeIfNeeded();
            a->GetInt32(attr,
                mEditor->mInputContentType->mImeOptions, &mEditor->mInputContentType->mImeOptions);*/
            break;

        case R::styleable::TextView_imeActionLabel:
            CreateEditorIfNeeded();
            assert(0);
            /*mEditor->CreateInputContentTypeIfNeeded();
            a->GetText(attr, (ICharSequence**)&(mEditor->mInputContentType->mImeActionLabel));*/
            break;

        case R::styleable::TextView_imeActionId:
            CreateEditorIfNeeded();
            assert(0);
            /*mEditor->CreateInputContentTypeIfNeeded();
            a->GetInt32(attr,
                mEditor->mInputContentType->mImeActionId, &mEditor->mInputContentType->mImeActionId);*/
            break;

        case R::styleable::TextView_privateImeOptions:
        {
            String str;
            a->GetString(attr, &str);
            SetPrivateImeOptions(str);
            break;
        }
        case R::styleable::TextView_editorExtras:
        {
            a->GetResourceId(attr, 0, &ivalue);
            ECode ec = SetInputExtras(ivalue);
            if (ec == (Int32)E_XML_PULL_PARSER_EXCEPTION) {
                  Logger::W(TEXT_VIEW_TAG, "Failure reading input extras %08x", ec);
            }
            else if (ec == (Int32)E_IO_EXCEPTION) {
                  Logger::W(TEXT_VIEW_TAG, "Failure reading input extras %08x", ec);
            }
            break;
        }

        case R::styleable::TextView_textCursorDrawable:
            a->GetResourceId(attr, 0, &mCursorDrawableRes);
            break;

        case R::styleable::TextView_textSelectHandleLeft:
            a->GetResourceId(attr, 0, &mTextSelectHandleLeftRes);
            break;

        case R::styleable::TextView_textSelectHandleRight:
            a->GetResourceId(attr, 0, &mTextSelectHandleRightRes);
            break;

        case R::styleable::TextView_textSelectHandle:
            a->GetResourceId(attr, 0, &mTextSelectHandleRes);
            break;

        case R::styleable::TextView_textEditSuggestionItemLayout:
            a->GetResourceId(attr, 0, &mTextEditSuggestionItemLayout);
            break;

        case R::styleable::TextView_textIsSelectable:
            Boolean res;
            a->GetBoolean(attr, FALSE, &res);
            SetTextIsSelectable(res);
            break;

        case R::styleable::TextView_textAllCaps:
            a->GetBoolean(attr, FALSE, &allCaps);
            break;

        case R::styleable::TextView_elegantTextHeight:
            a->GetBoolean(attr, FALSE, &elegant);
            break;

        case R::styleable::TextView_letterSpacing:
            a->GetFloat(attr, 0, &letterSpacing);
            break;

        case R::styleable::TextView_fontFeatureSettings:
            a->GetString(attr, &fontFeatureSettings);
            break;
        }
    }
    a->Recycle();

    BufferType bufferType = BufferType_EDITABLE;

    Int32 variation =
        inputType & (IInputType::TYPE_MASK_CLASS | IInputType::TYPE_MASK_VARIATION);
    Boolean passwordInputType = variation
        == (IInputType::TYPE_CLASS_TEXT | IInputType::TYPE_TEXT_VARIATION_PASSWORD);
    Boolean webPasswordInputType = variation
        == (IInputType::TYPE_CLASS_TEXT | IInputType::TYPE_TEXT_VARIATION_WEB_PASSWORD);
    Boolean numberPasswordInputType = variation
        == (IInputType::TYPE_CLASS_NUMBER | IInputType::TYPE_NUMBER_VARIATION_PASSWORD);
    Boolean isTextSelectable;
    if (inputMethod != NULL) {
        assert(0 && "TODO");
//        Class<?> c;
//
//        try {
//            c = Class.forName(inputMethod.toString());
//        } catch (ClassNotFoundException ex) {
//            throw new RuntimeException(ex);
//        }
//
//        try {
//            mInput = (KeyListener) c.newInstance();
//        } catch (InstantiationException ex) {
//            throw new RuntimeException(ex);
//        } catch (IllegalAccessException ex) {
//            throw new RuntimeException(ex);
//        }
//        try {
//            mInputType = inputType != IInputType::TYPE_NULL
//                    ? inputType
//                    : mInput.GetInputType();
//        } catch (IncompatibleClassChangeError e) {
//            mInputType = IInputType::TYPE_CLASS_TEXT;
//        }
    }
    else if (digits != NULL) {
        CreateEditorIfNeeded();
        String dstring;
        digits->ToString(&dstring);
        AutoPtr<IDigitsKeyListener> listener;
        DigitsKeyListener::GetInstance(dstring, (IDigitsKeyListener**)&listener);
        assert(0);
        /*mEditor->mKeyListener = IKeyListener::Probe(listener);
        mEditor->mInputType = inputType != IInputType::TYPE_NULL ? inputType : IInputType::TYPE_CLASS_TEXT;*/
    }
    else if (inputType != IInputType::TYPE_NULL) {
        SetInputType(inputType, TRUE);
        singleLine = !IsMultilineInputType(inputType);
    }
    else if (phone) {
        CreateEditorIfNeeded();
        AutoPtr<IDialerKeyListener> listener;
        CDialerKeyListener::GetInstance((IDialerKeyListener**)&listener);
        assert(0);
        /*mEditor->mKeyListener = IKeyListener::Probe(listener);
        mEditor->mInputType = inputType = IInputType::TYPE_CLASS_PHONE;*/
    }
    else if (numeric != 0) {
        CreateEditorIfNeeded();
        AutoPtr<IDigitsKeyListener> listener;
        DigitsKeyListener::GetInstance((numeric & SIGNED) != 0, (numeric & DECIMAL) != 0, (IDigitsKeyListener**)&listener);
        assert(0);
        //mEditor->mKeyListener = IKeyListener::Probe(listener);
        inputType = IInputType::TYPE_CLASS_NUMBER;
        if ((numeric & SIGNED) != 0) {
            inputType |= IInputType::TYPE_NUMBER_FLAG_SIGNED;
        }
        if ((numeric & DECIMAL) != 0) {
            inputType |= IInputType::TYPE_NUMBER_FLAG_DECIMAL;
        }
        assert(0);
        //mEditor->mInputType = inputType;

    }
    else if (autotext || autocap != -1) {
        Elastos::Droid::Text::Method::Capitalize cap;

        inputType = IInputType::TYPE_CLASS_TEXT;
        if (!singleLine) {
            inputType |= IInputType::TYPE_TEXT_FLAG_MULTI_LINE;
        }

        switch (autocap) {
        case 1:
            cap = Elastos::Droid::Text::Method::Capitalize_SENTENCES;
            inputType |= IInputType::TYPE_TEXT_FLAG_CAP_SENTENCES;
            break;

        case 2:
            cap = Elastos::Droid::Text::Method::Capitalize_WORDS;
            inputType |= IInputType::TYPE_TEXT_FLAG_CAP_WORDS;
            break;

        case 3:
            cap = Elastos::Droid::Text::Method::Capitalize_CHARACTERS;
            inputType |= IInputType::TYPE_TEXT_FLAG_CAP_CHARACTERS;
            break;

        default:
            cap = Elastos::Droid::Text::Method::Capitalize_NONE;
            break;
        }
        CreateEditorIfNeeded();
        AutoPtr<ITextKeyListenerHelper> helper;
        CTextKeyListenerHelper::AcquireSingleton((ITextKeyListenerHelper**)&helper);
        assert(0);
        //mEditor->mKeyListener = NULL;
        AutoPtr<ITextKeyListener> listener;
        helper->GetInstance(autotext, cap, (ITextKeyListener**)&listener);
        /*mEditor->mKeyListener = IKeyListener::Probe(listener);
        mEditor->mInputType = inputType;*/
    }
    else if (IsTextSelectable(&isTextSelectable), isTextSelectable) {
        // Prevent text changes from keyboard.
        assert(0);
        /*if (mEditor != NULL) {
            mEditor->mKeyListener = NULL;
            mEditor->mInputType = IInputType::TYPE_NULL;
        }*/
        bufferType = Elastos::Droid::Widget::BufferType_SPANNABLE;
        // So that selection can be changed using arrow keys and touch is handled.
        AutoPtr<IArrowKeyMovementMethodHelper> helper;
        CArrowKeyMovementMethodHelper::AcquireSingleton((IArrowKeyMovementMethodHelper**)&helper);
        AutoPtr<IMovementMethod> method;
        helper->GetInstance((IMovementMethod**)&method);
        SetMovementMethod(method);
    }
    else if (editable) {
        CreateEditorIfNeeded();
        AutoPtr<ITextKeyListenerHelper> helper;
        CTextKeyListenerHelper::AcquireSingleton((ITextKeyListenerHelper**)&helper);
        assert(0);
        /*mEditor->mKeyListener = NULL;
        AutoPtr<ITextKeyListener> tkl;
        helper->GetInstance((ITextKeyListener**)&tkl);
        mEditor->mKeyListener = IKeyListener::Probe(tkl);
        mEditor->mInputType = IInputType::TYPE_CLASS_TEXT;*/
    }
    else {
        assert(0);
        //if (mEditor != NULL) mEditor->mKeyListener = NULL;

        switch (buffertype) {
            case 0:
                bufferType = BufferType_NORMAL;
                break;
            case 1:
                bufferType = BufferType_SPANNABLE;
                break;
            case 2:
                bufferType = BufferType_EDITABLE;
                break;
        }
    }

    if (mEditor != NULL) {
        assert(0);
        /*mEditor->AdjustInputType(password, passwordInputType,
                webPasswordInputType, numberPasswordInputType);*/
        }

    if (selectallonfocus) {
        CreateEditorIfNeeded();
        assert(0);
        //mEditor->mSelectAllOnFocus = TRUE;

        if (bufferType == BufferType_NORMAL) {
            bufferType = BufferType_SPANNABLE;
        }
    }

    SetCompoundDrawablesWithIntrinsicBounds(
            drawableLeft, drawableTop, drawableRight, drawableBottom);
    SetRelativeDrawablesIfNeeded(drawableStart, drawableEnd);
    SetCompoundDrawablePadding(drawablePadding);

    // Same as setSingleLine(), but make sure the transformation method and the maximum number
    // of lines of height are unchanged for multi-line TextViews.
    SetInputTypeSingleLine(singleLine);

    ApplySingleLine(singleLine, singleLine, singleLine);

    AutoPtr<IKeyListener> keyListener;
    GetKeyListener((IKeyListener**)&keyListener);
    if (singleLine && keyListener == NULL && ellipsize < 0) {
        ellipsize = TextUtilsTruncateAt_END; // END
    }

    switch (ellipsize) {
        case 1:
            SetEllipsize(TextUtilsTruncateAt_START);
            break;
        case 2:
            SetEllipsize(TextUtilsTruncateAt_MIDDLE);
            break;
        case 3:
            SetEllipsize(TextUtilsTruncateAt_END);
            break;
        case 4:
            {
                AutoPtr<IViewConfigurationHelper> helper;
                assert(0);
                //CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
                AutoPtr<IViewConfiguration> config;
                helper->Get(context, (IViewConfiguration**)&config);
                Boolean isFadingMarqueeEnabled = FALSE;
                config->IsFadingMarqueeEnabled(&isFadingMarqueeEnabled);
                if (isFadingMarqueeEnabled) {
                    SetHorizontalFadingEdgeEnabled(TRUE);
                    mMarqueeFadeMode = MARQUEE_FADE_NORMAL;
                } else {
                    SetHorizontalFadingEdgeEnabled(FALSE);
                    mMarqueeFadeMode = MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS;
                }
                SetEllipsize(TextUtilsTruncateAt_MARQUEE);

            }
            break;
    }

    if (textColor == NULL) {
        CColorStateList::ValueOf(0xFF000000, (IColorStateList**)&textColor);
    }

    SetTextColor(textColor);
    SetHintTextColor(textColorHint);
    SetLinkTextColor(textColorLink);
    if (textColorHighlight != 0) {
        SetHighlightColor(textColorHighlight);
    }

    SetRawTextSize(textSize);
    SetElegantTextHeight(elegant);
    SetLetterSpacing(letterSpacing);
    SetFontFeatureSettings(fontFeatureSettings);

    if (allCaps) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<ITransformationMethod> method;
        CAllCapsTransformationMethod::New(ctx, (IAllCapsTransformationMethod**)&method);
        SetTransformationMethod(method);
    }

    if (password || passwordInputType || webPasswordInputType || numberPasswordInputType) {
        AutoPtr<IPasswordTransformationMethodHelper> helper;
        CPasswordTransformationMethodHelper::AcquireSingleton((IPasswordTransformationMethodHelper**)&helper);
        AutoPtr<ITransformationMethod> method;
        helper->GetInstance((IPasswordTransformationMethod**)&method);
        SetTransformationMethod(method);
        typefaceIndex = MONOSPACE;
    }
    assert(0);
    /*else if (mEditor != NULL &&
            (mEditor->mInputType & (IInputType::TYPE_MASK_CLASS | IInputType::TYPE_MASK_VARIATION))
            == (IInputType::TYPE_CLASS_TEXT | IInputType::TYPE_TEXT_VARIATION_PASSWORD)) {
        typefaceIndex = MONOSPACE;
    }*/

    SetTypefaceFromAttrs(fontFamily, typefaceIndex, styleIndex);

    if (shadowcolor != 0) {
        SetShadowLayer(r, dx, dy, shadowcolor);
    }

    if (maxlength >= 0) {
        AutoPtr<Elastos::Droid::Text::IInputFilter> lenFilter;
        CLengthFilter::New(maxlength, (Elastos::Droid::Text::IInputFilter**)&lenFilter);
        AutoPtr<ArrayOf<Elastos::Droid::Text::IInputFilter*> > lenFilters
            = ArrayOf<Elastos::Droid::Text::IInputFilter*>::Alloc(1);
        lenFilters->Set(0, lenFilter);
        SetFilters(lenFilters);
    }
    else {
        SetFilters(NO_FILTERS);
    }
    SetText(text, bufferType);
    if (hint != NULL) SetHint(hint);
    /*
     * Views are not normally focusable unless specified to be.
     * However, TextViews that have input or movement methods *are*
     * focusable by default.
     */
    a = NULL;
    attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::View),
        ArraySize(R::styleable::View));
    ASSERT_SUCCEEDED(context->ObtainStyledAttributes(
            attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

    keyListener = NULL;
    GetKeyListener((IKeyListener**)&keyListener);
    Boolean focusable = mMovement != NULL || keyListener != NULL;
    Boolean isClickable, isLongClickable;
    Boolean clickable = focusable || (IsClickable(&isClickable), isClickable);
    Boolean longClickable = focusable || (IsLongClickable(&isLongClickable), isLongClickable);

    a->GetIndexCount(&n);
    for (Int32 i = 0; i < n; i++) {
        Int32 attr;
        a->GetIndex(i, &attr);

        switch (attr) {
        case R::styleable::View_focusable:
            a->GetBoolean(attr, focusable, &focusable);
            break;

        case R::styleable::View_clickable:
            a->GetBoolean(attr, clickable, &clickable);
            break;

        case R::styleable::View_longClickable:
            a->GetBoolean(attr, longClickable, &longClickable);
            break;
        }
    }
    a->Recycle();

    SetFocusable(focusable);
    SetClickable(clickable);
    SetLongClickable(longClickable);

    assert(0);
    //if (mEditor != NULL) mEditor->PrepareCursorControllers();

    // If not explicitly specified this view is important for accessibility.
    Int32 importantForAccessibility;
    if ((GetImportantForAccessibility(&importantForAccessibility), importantForAccessibility)
         == IView::IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
        SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
    }
    return NOERROR;
}

void TextView::SetTypefaceFromAttrs(
    /* [in] */ const String& familyName,
    /* [in] */ Int32 typefaceIndex,
    /* [in] */ Int32 styleIndex)
{
    AutoPtr<ITypeface> tf;
    if (!familyName.IsNull()) {
//        AutoPtr<ITypefaceHelper> helper;
//        CTypefaceHelper::AcquireSingleton((ITypefaceHelper**)&helper);
//        helper->Create(familyName, styleIndex, (ITypeface**)&tf);
        Typeface::Create(familyName, styleIndex, (ITypeface**)&tf);
        if (tf != NULL) {
            SetTypeface(tf);
            return;
        }
    }

    switch (typefaceIndex) {
        case SANS:
            tf = Typeface::SANS_SERIF;
            break;

        case SERIF:
            tf = Typeface::SERIF;
            break;

        case MONOSPACE:
            tf = Typeface::MONOSPACE;
            break;
    }

    SetTypeface(tf, styleIndex);
}

void TextView::SetRelativeDrawablesIfNeeded(
    /* [in] */ IDrawable* start,
    /* [in] */ IDrawable* end)
{
    Boolean hasRelativeDrawables = (start != NULL) || (end != NULL);
    if (hasRelativeDrawables) {
        AutoPtr<Drawables> dr = mDrawables;
        if (dr == NULL) {
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            mDrawables = dr = new Drawables(ctx);
        }
        mDrawables->mOverride = TRUE;
        AutoPtr<IRect> compoundRect = dr->mCompoundRect;
        AutoPtr<ArrayOf<Int32> > state;
        GetDrawableState((ArrayOf<Int32>**)&state);
        if (start != NULL) {
            Int32 w, h;
            start->GetIntrinsicWidth(&w);
            start->GetIntrinsicHeight(&h);
            Boolean isStateful;
            start->SetBounds(0, 0, w, h);
            start->SetState(state, &isStateful);
            start->CopyBounds(compoundRect);
            start->SetCallback(this);

            dr->mDrawableStart = start;
            compoundRect->GetWidth(&dr->mDrawableSizeEnd);
            compoundRect->GetHeight(&dr->mDrawableHeightEnd);
        } else {
            dr->mDrawableSizeStart = dr->mDrawableHeightStart = 0;
        }

        if (end != NULL) {
            Int32 w, h;
            end->GetIntrinsicWidth(&w);
            end->GetIntrinsicHeight(&h);
            Boolean isStateful;
            end->SetBounds(0, 0, w, h);
            end->SetState(state, &isStateful);
            end->CopyBounds(compoundRect);
            end->SetCallback(this);

            dr->mDrawableEnd = end;
            compoundRect->GetWidth(&dr->mDrawableSizeEnd);
            compoundRect->GetHeight(&dr->mDrawableHeightEnd);
        } else {
            dr->mDrawableSizeEnd = dr->mDrawableHeightEnd = 0;
        }
        ResetResolvedDrawables();
        ResolveDrawables();
    }
}

ECode TextView::SetEnabled(
    /* [in] */ Boolean enabled)
{
    Boolean isEnabled;
    if (enabled == (IsEnabled(&isEnabled), isEnabled)) {
        return NOERROR;
    }

    if (!enabled) {
        // Hide the soft input if the currently active TextView is disabled
        assert(0);
        AutoPtr<IInputMethodManager> imm;// = CInputMethodManager::PeekInstance();
        if (imm != NULL) {
            Boolean active;
            imm->IsActive(this, &active);
            if (active) {
                Boolean hide;
                AutoPtr<IBinder> temp;
                GetWindowToken((IBinder**)&temp);
                imm->HideSoftInputFromWindow(temp, 0, &hide);
            }
        }
    }

    View::SetEnabled(enabled);

    if (enabled) {
        // Make sure IME is updated with current editor info.
        assert(0);
        AutoPtr<IInputMethodManager> imm;// = CInputMethodManager::PeekInstance();
        if (imm != NULL) imm->RestartInput(this);
    }

    // Will change text color
    if (mEditor != NULL) {
        assert(0);
        /*mEditor->InvalidateTextDisplayList();
        mEditor->PrepareCursorControllers();

        // start or stop the cursor blinking as appropriate
        mEditor->MakeBlink();*/
    }
    return NOERROR;
}

ECode TextView::SetTypeface(
    /* [in] */ ITypeface* tf,
    /* [in] */ Int32 style)
{
    if (style > 0) {
        if (tf == NULL) {
            Typeface::DefaultFromStyle(style, &tf);
        }
        else {
            Typeface::Create(tf, style, &tf);
        }

        SetTypeface(tf);
        // now compute what (if any) algorithmic styling is needed
        Int32 typefaceStyle = 0;
        if (tf != NULL) {
            tf->GetStyle(&typefaceStyle);
        }
        Int32 need = style & ~typefaceStyle;
        IPaint::Probe(mTextPaint)->SetFakeBoldText((need & ITypeface::BOLD) != 0);
        IPaint::Probe(mTextPaint)->SetTextSkewX((need & ITypeface::ITALIC) != 0 ? -0.25f : 0);
    }
    else {
        IPaint::Probe(mTextPaint)->SetFakeBoldText(FALSE);
        IPaint::Probe(mTextPaint)->SetTextSkewX(0);
        SetTypeface(tf);
    }
    return NOERROR;
}

Boolean TextView::GetDefaultEditable()
{
    return FALSE;
}

AutoPtr<IMovementMethod> TextView::GetDefaultMovementMethod()
{
    return NULL;
}

ECode TextView::GetText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text)
    *text = mText;
    REFCOUNT_ADD(*text)
    return NOERROR;
}

ECode TextView::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    mText->GetLength(length);
    return NOERROR;
}

ECode TextView::GetEditableText(
    /* [out] */ IEditable** editable)
{
    VALIDATE_NOT_NULL(editable)
    *editable = IEditable::Probe(mText);
    REFCOUNT_ADD(*editable)
    return NOERROR;
}

ECode TextView::GetLineHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    Int32 spacing;
    IPaint::Probe(mTextPaint)->GetFontMetricsInt(NULL, &spacing);
    *height = Elastos::Core::Math::Round(spacing * mSpacingMult + mSpacingAdd);
    return NOERROR;
}

ECode TextView::GetLayout(
    /* [out] */ ILayout** layout)
{
    VALIDATE_NOT_NULL(layout)
    *layout = mLayout;
    REFCOUNT_ADD(*layout)
    return NOERROR;
}

AutoPtr<ILayout> TextView::GetHintLayout()
{
    return mHintLayout;
}

AutoPtr<IUndoManager> TextView::GetUndoManager()
{
    assert(0);
    //return mEditor == NULL ? NULL : mEditor->mUndoManager;
    return NULL;
}

void TextView::SetUndoManager(
    /* [in] */ IUndoManager* undoManager,
    /* [in] */ const String& tag)
{
    if (undoManager != NULL) {
        CreateEditorIfNeeded();
        assert(0);
        /*mEditor->mUndoManager = undoManager;
        AutoPtr<IUndoOwner> owner;
        undoManager->GetOwner(tag, TO_IINTERFACE(this), (IUndoOwner**)&owner);
        mEditor->mUndoOwner = owner;
        mEditor->mUndoInputFilter = new EditorUndoInputFilter(mEditor);*/
        if (!IEditable::Probe(mText)) {
            SetText(mText, BufferType_EDITABLE);
        }

        SetFilters(IEditable::Probe(mText), mFilters);
    } else if (mEditor != NULL) {
        // XXX need to destroy all associated state.
        assert(0);
        /*mEditor->mUndoManager = NULL;
        mEditor->mUndoOwner = NULL;
        mEditor->mUndoInputFilter = NULL;*/
    }
}

ECode TextView::GetKeyListener(
    /* [out] */ IKeyListener** listener)
{
    VALIDATE_NOT_NULL(listener)
    if (mEditor == NULL) {
        *listener = NULL;
    } else {
        assert(0);
        //*listener = mEditor->mKeyListener;
        REFCOUNT_ADD(*listener)
    }
    return NOERROR;
}

ECode TextView::SetKeyListener(
    /* [in] */ IKeyListener* input)
{
    SetKeyListenerOnly(input);
    FixFocusableAndClickableSettings();

    if (input != NULL) {
        CreateEditorIfNeeded();
        //try {
        assert(0);
            //mEditor->mKeyListener->GetInputType(&mEditor->mInputType);
        //} catch (IncompatibleClassChangeError e) {
        //    mEditor->mInputType = IInputType::TYPE_CLASS_TEXT;
        //}
        // Change inputType, without affecting transformation.
        // No need to applySingleLine since mSingleLine is unchanged.
        SetInputTypeSingleLine(mSingleLine);
    } else {
        assert(0);
        //if (mEditor != NULL) mEditor->mInputType = IInputType::TYPE_NULL;
    }

    assert(0);
    AutoPtr<IInputMethodManager> imm;// = CInputMethodManager::PeekInstance();
    if (imm != NULL) {
        imm->RestartInput(this);
    }

    return NOERROR;
}

void TextView::SetKeyListenerOnly(
    /* [in] */ IKeyListener* input)
{
    if (mEditor == NULL && input == NULL) return; // NULL is the default value

    CreateEditorIfNeeded();
    assert(0);
    /*if (input != mEditor->mKeyListener) {
        mEditor->mKeyListener = input;

        AutoPtr<IEditable> editable = IEditable::Probe(mText);
        if (input != NULL && NULL == editable) {
            SetText(mText);
        }

        editable = IEditable::Probe(mText);
        SetFilters(editable, mFilters);
    }*/
}

ECode TextView::GetMovementMethod(
    /* [out] */ IMovementMethod** movement)
{
    VALIDATE_NOT_NULL(movement)
    *movement = mMovement;
    REFCOUNT_ADD(*movement)
    return NOERROR;
}

ECode TextView::SetMovementMethod(
    /* [in] */ IMovementMethod* movement)
{
    if (movement != mMovement) {
        mMovement = movement;

        AutoPtr<ISpannable> spannable = ISpannable::Probe(mText);
        if (movement != NULL && NULL == spannable) {
            SetText(mText);
        }

        FixFocusableAndClickableSettings();

        // SelectionModifierCursorController depends on textCanBeSelected, which depends on
        // mMovement
        assert(0);
        //if (mEditor != NULL) mEditor->PrepareCursorControllers();
    }

    return NOERROR;
}

void TextView::FixFocusableAndClickableSettings()
{
    assert(0);
    if (mMovement != NULL /*|| (mEditor != NULL && mEditor->mKeyListener != NULL)*/) {
        SetFocusable(TRUE);
        SetClickable(TRUE);
        SetLongClickable(TRUE);
    }
    else {
        SetFocusable(FALSE);
        SetClickable(FALSE);
        SetLongClickable(FALSE);
    }
}

ECode TextView::GetTransformationMethod(
    /* [out] */ ITransformationMethod** method)
{
    VALIDATE_NOT_NULL(method)
    *method = mTransformation;
    REFCOUNT_ADD(*method)
    return NOERROR;
}

ECode TextView::SetTransformationMethod(
    /* [in] */ ITransformationMethod* method)
{
    if (method == mTransformation.Get()) {
        // Avoid the SetText() below if the transformation is
        // the same.
        return NOERROR;
    }

    if (mTransformation != NULL) {
        AutoPtr<ISpannable> spannable = ISpannable::Probe(mText);
        if (spannable) {
            spannable->RemoveSpan(mTransformation.Get());
        }
    }

    mTransformation = method;

    ITransformationMethod2* method2 = ITransformationMethod2::Probe(method);
    if (method2) {
        Boolean isTextSelectable;
        IsTextSelectable(&isTextSelectable);
        mAllowTransformationLengthChange = !isTextSelectable && !(IEditText::Probe(mText.Get()));
        method2->SetLengthChangesAllowed(mAllowTransformationLengthChange);
    } else {
        mAllowTransformationLengthChange = FALSE;
    }
    SetText(mText);
    if (HasPasswordTransformationMethod()) {
        NotifyViewAccessibilityStateChangedIfNeeded(
            IAccessibilityEvent::CONTENT_CHANGE_TYPE_UNDEFINED);
    }

    return NOERROR;
}

ECode TextView::GetCompoundPaddingTop(
    /* [out] */ Int32* top)
{
    VALIDATE_NOT_NULL(top)
    AutoPtr<Drawables> dr = mDrawables;
    if (dr == NULL || dr->mDrawableTop == NULL) {
        *top = mPaddingTop;
        return NOERROR;
    }
    else {
        *top = mPaddingTop + dr->mDrawablePadding + dr->mDrawableSizeTop;
        return NOERROR;
    }
}

ECode TextView::GetCompoundPaddingBottom(
    /* [out] */ Int32* bottom)
{
    VALIDATE_NOT_NULL(bottom)
    AutoPtr<Drawables> dr = mDrawables;
    if (dr == NULL || dr->mDrawableBottom == NULL) {
        *bottom = mPaddingBottom;
        return NOERROR;
    }
    else {
        *bottom = mPaddingBottom + dr->mDrawablePadding + dr->mDrawableSizeBottom;
        return NOERROR;
    }
}

ECode TextView::GetCompoundPaddingLeft(
    /* [out] */ Int32* left)
{
    VALIDATE_NOT_NULL(left)
    AutoPtr<Drawables> dr = mDrawables;
    if (dr == NULL || dr->mDrawableLeft == NULL) {
        *left = mPaddingLeft;
        return NOERROR;
    }
    else {
        *left = mPaddingLeft + dr->mDrawablePadding + dr->mDrawableSizeLeft;
        return NOERROR;
    }
}

ECode TextView::GetCompoundPaddingRight(
    /* [out] */ Int32* right)
{
    VALIDATE_NOT_NULL(right)
    AutoPtr<Drawables> dr = mDrawables;
    if (dr == NULL || dr->mDrawableRight == NULL) {
        *right = mPaddingRight;
        return NOERROR;
    }
    else {
        *right = mPaddingRight + dr->mDrawablePadding + dr->mDrawableSizeRight;
        return NOERROR;
    }
}

ECode TextView::GetCompoundPaddingStart(
    /* [out] */ Int32* start)
{
    VALIDATE_NOT_NULL(start)
    ResolveDrawables();
    Int32 direction;
    switch(GetLayoutDirection(&direction), direction) {
        default:
        case IView::LAYOUT_DIRECTION_LTR:
        {
            return GetCompoundPaddingLeft(start);
        }
        case IView::LAYOUT_DIRECTION_RTL:
        {
            return GetCompoundPaddingRight(start);
        }

    }
    *start = 0;
    return NOERROR;
}

ECode TextView::GetCompoundPaddingEnd(
    /* [out] */ Int32* end)
{
    VALIDATE_NOT_NULL(end)
    ResolveDrawables();
    Int32 direction;
    switch(GetLayoutDirection(&direction), direction) {
        default:
        case IView::LAYOUT_DIRECTION_LTR:
        {
            return GetCompoundPaddingRight(end);
        }
        case IView::LAYOUT_DIRECTION_RTL:
        {
            return GetCompoundPaddingLeft(end);
        }
    }
    *end = 0;
    return NOERROR;
}

ECode TextView::GetExtendedPaddingTop(
    /* [out] */ Int32* top)
{
    VALIDATE_NOT_NULL(top)
    if (mMaxMode != LINES) {
        return GetCompoundPaddingTop(top);
    }

    if (!mLayout) {
        AssumeLayout();
    }

    Int32 number;
    mLayout->GetLineCount(&number);
    if (number <= mMaximum) {
        return GetCompoundPaddingTop(top);
    }

    Int32 compoundPaddingTop, bottom;
    GetCompoundPaddingTop(&compoundPaddingTop);
    GetCompoundPaddingBottom(&bottom);
    Int32 height;
    GetHeight(&height);
    Int32 viewht = height - compoundPaddingTop - bottom;
    Int32 layoutht;
    mLayout->GetLineTop(mMaximum, &layoutht);

    if (layoutht >= viewht) {
        *top = compoundPaddingTop;
        return NOERROR;
    }

    Int32 gravity = mGravity & IGravity::VERTICAL_GRAVITY_MASK;
    if (gravity == IGravity::TOP) {
        *top = compoundPaddingTop;
        return NOERROR;
    }
    else if (gravity == IGravity::BOTTOM) {
        *top = compoundPaddingTop + viewht - layoutht;
        return NOERROR;
    }
    else { // (gravity == IGravity::CENTER_VERTICAL)
        *top = compoundPaddingTop + (viewht - layoutht) / 2;
        return NOERROR;
    }
}

ECode TextView::GetExtendedPaddingBottom(
    /* [out] */ Int32* bottom)
{
    VALIDATE_NOT_NULL(bottom)
    if (mMaxMode != LINES) {
        return GetCompoundPaddingBottom(bottom);
    }

    if (!mLayout) {
        AssumeLayout();
    }

    Int32 number;
    mLayout->GetLineCount(&number);
    if (number <= mMaximum) {
        return GetCompoundPaddingBottom(bottom);
    }

    Int32 top, compoundPaddingBottom;
    GetCompoundPaddingTop(&top);
    GetCompoundPaddingBottom(&compoundPaddingBottom);
    Int32 height;
    GetHeight(&height);
    Int32 viewht = height - top - compoundPaddingBottom;
    Int32 layoutht;
    mLayout->GetLineTop(mMaximum, &layoutht);

    if (layoutht >= viewht) {
        *bottom = compoundPaddingBottom;
        return NOERROR;
    }

    Int32 gravity = mGravity & IGravity::VERTICAL_GRAVITY_MASK;
    if (gravity == IGravity::TOP) {
        *bottom = compoundPaddingBottom + viewht - layoutht;
        return NOERROR;
    }
    else if (gravity == IGravity::BOTTOM) {
        *bottom = compoundPaddingBottom;
        return NOERROR;
    }
    else { // (gravity == IGravity::CENTER_VERTICAL)
        *bottom = compoundPaddingBottom + (viewht - layoutht) / 2;
        return NOERROR;
    }
}

ECode TextView::GetTotalPaddingLeft(
    /* [out] */ Int32* left)
{
    VALIDATE_NOT_NULL(left)
    return GetCompoundPaddingLeft(left);
}

ECode TextView::GetTotalPaddingRight(
    /* [out] */ Int32* right)
{
    VALIDATE_NOT_NULL(right)
    return GetCompoundPaddingRight(right);
}

ECode TextView::GetTotalPaddingStart(
    /* [out] */ Int32* start)
{
    VALIDATE_NOT_NULL(start)
    return GetCompoundPaddingStart(start);
}

ECode TextView::GetTotalPaddingEnd(
    /* [out] */ Int32* end)
{
    VALIDATE_NOT_NULL(end)
    return GetCompoundPaddingEnd(end);
}

ECode TextView::GetTotalPaddingTop(
    /* [out] */ Int32* top)
{
    VALIDATE_NOT_NULL(top)
    GetExtendedPaddingTop(top);
    *top += GetVerticalOffset(TRUE);
    return NOERROR;
}

ECode TextView::GetTotalPaddingBottom(
    /* [out] */ Int32* bottom)
{
    VALIDATE_NOT_NULL(bottom)
    GetExtendedPaddingBottom(bottom);
    *bottom += GetBottomVerticalOffset(TRUE);
    return NOERROR;
}

ECode TextView::SetCompoundDrawables(
    /* [in] */ IDrawable* left,
    /* [in] */ IDrawable* top,
    /* [in] */ IDrawable* right,
    /* [in] */ IDrawable* bottom)
{
    AutoPtr<Drawables> dr = mDrawables;

    if (dr) {
        if (dr->mDrawableStart) dr->mDrawableStart->SetCallback(NULL);
        dr->mDrawableStart = NULL;
        if (dr->mDrawableEnd) dr->mDrawableEnd->SetCallback(NULL);
        dr->mDrawableEnd = NULL;
        dr->mDrawableSizeStart = dr->mDrawableHeightStart = 0;
        dr->mDrawableSizeEnd = dr->mDrawableHeightEnd = 0;
    }

    Boolean drawables = left != NULL || top != NULL
            || right != NULL || bottom != NULL;

    if (!drawables) {
        // Clearing drawables...  can we free the data structure?
        if (dr != NULL) {
            if (dr->mDrawablePadding == 0) {
                mDrawables = NULL;
            }
            else {
                // We need to retain the last set padding, so just clear
                // out all of the fields in the existing structure.
                if (dr->mDrawableLeft != NULL) dr->mDrawableLeft->SetCallback(NULL);
                dr->mDrawableLeft = NULL;
                if (dr->mDrawableTop != NULL) dr->mDrawableTop->SetCallback(NULL);
                dr->mDrawableTop = NULL;
                if (dr->mDrawableRight != NULL) dr->mDrawableRight->SetCallback(NULL);
                dr->mDrawableRight = NULL;
                if (dr->mDrawableBottom != NULL) dr->mDrawableBottom->SetCallback(NULL);
                dr->mDrawableBottom = NULL;
                dr->mDrawableSizeLeft = dr->mDrawableHeightLeft = 0;
                dr->mDrawableSizeRight = dr->mDrawableHeightRight = 0;
                dr->mDrawableSizeTop = dr->mDrawableWidthTop = 0;
                dr->mDrawableSizeBottom = dr->mDrawableWidthBottom = 0;
            }
        }
    }
    else {
        if (dr == NULL) {
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            mDrawables = dr = new Drawables(ctx);
        }

        mDrawables->mOverride = FALSE;

        if (dr->mDrawableLeft.Get() != left && dr->mDrawableLeft != NULL) {
            dr->mDrawableLeft->SetCallback(NULL);
        }
        dr->mDrawableLeft = left;

        if (dr->mDrawableTop.Get() != top && dr->mDrawableTop != NULL) {
            dr->mDrawableTop->SetCallback(NULL);
        }
        dr->mDrawableTop = top;

        if (dr->mDrawableRight.Get() != right && dr->mDrawableRight != NULL) {
            dr->mDrawableRight->SetCallback(NULL);
        }
        dr->mDrawableRight = right;

        if (dr->mDrawableBottom.Get() != bottom && dr->mDrawableBottom != NULL) {
            dr->mDrawableBottom->SetCallback(NULL);
        }
        dr->mDrawableBottom = bottom;

        AutoPtr<IRect> compoundRect = dr->mCompoundRect;

        AutoPtr<ArrayOf<Int32> > state;
        GetDrawableState((ArrayOf<Int32>**)&state);
        Boolean temp;
        Int32 w, h;
        if (left != NULL) {
            left->SetState(state, &temp);
            left->CopyBounds(compoundRect);
            left->SetCallback(this);
            compoundRect->GetWidth(&w);
            compoundRect->GetHeight(&h);
            dr->mDrawableSizeLeft = w;
            dr->mDrawableHeightLeft = h;
        } else {
            dr->mDrawableSizeLeft = dr->mDrawableHeightLeft = 0;
        }

        if (right != NULL) {
            right->SetState(state, &temp);
            right->CopyBounds(compoundRect);
            right->SetCallback(this);
            compoundRect->GetWidth(&w);
            compoundRect->GetHeight(&h);
            dr->mDrawableSizeRight = w;
            dr->mDrawableHeightRight = h;
        } else {
            dr->mDrawableSizeRight = dr->mDrawableHeightRight = 0;
        }

        if (top != NULL) {
            top->SetState(state, &temp);
            top->CopyBounds(compoundRect);
            top->SetCallback(this);
            compoundRect->GetWidth(&w);
            compoundRect->GetHeight(&h);
            dr->mDrawableSizeTop = h;
            dr->mDrawableWidthTop = w;
        } else {
            dr->mDrawableSizeTop = dr->mDrawableWidthTop = 0;
        }

        if (bottom != NULL) {
            bottom->SetState(state, &temp);
            bottom->CopyBounds(compoundRect);
            bottom->SetCallback(this);
            compoundRect->GetWidth(&w);
            compoundRect->GetHeight(&h);
            dr->mDrawableSizeBottom = h;
            dr->mDrawableWidthBottom = w;
        } else {
            dr->mDrawableSizeBottom = dr->mDrawableWidthBottom = 0;
        }
    }

    if (dr) {
        dr->mDrawableLeftInitial = left;
        dr->mDrawableRightInitial = right;
    }

    ResetResolvedDrawables();
    ResolveDrawables();

    Invalidate();
    RequestLayout();
    return NOERROR;
}

ECode TextView::SetCompoundDrawablesWithIntrinsicBounds(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<IContext> c;
    GetContext((IContext**)&c);

    AutoPtr<IDrawable> leftDrawable;
    if (left != 0) {
        c->GetDrawable(left, (IDrawable**)&leftDrawable);
    }

    AutoPtr<IDrawable> topDrawable;
    if (top != 0) {
        c->GetDrawable(top, (IDrawable**)&topDrawable);
    }

    AutoPtr<IDrawable> rightDrawable;
    if (right != 0) {
        c->GetDrawable(right, (IDrawable**)&rightDrawable);
    }

    AutoPtr<IDrawable> bottomDrawable;
    if (bottom != 0) {
        c->GetDrawable(bottom, (IDrawable**)&bottomDrawable);
    }

    return SetCompoundDrawablesWithIntrinsicBounds(leftDrawable,
            topDrawable, rightDrawable, bottomDrawable);
}

ECode TextView::SetCompoundDrawablesWithIntrinsicBounds(
    /* [in] */ IDrawable* left,
    /* [in] */ IDrawable* top,
    /* [in] */ IDrawable* right,
    /* [in] */ IDrawable* bottom)
{
    Int32 width, height;
    if (left != NULL) {
        left->GetIntrinsicWidth(&width);
        left->GetIntrinsicHeight(&height);
        left->SetBounds(0, 0, width, height);
    }
    if (right != NULL) {
        right->GetIntrinsicWidth(&width);
        right->GetIntrinsicHeight(&height);
        right->SetBounds(0, 0, width, height);
    }
    if (top != NULL) {
        top->GetIntrinsicWidth(&width);
        top->GetIntrinsicHeight(&height);
        top->SetBounds(0, 0, width, height);
    }
    if (bottom != NULL) {
        bottom->GetIntrinsicWidth(&width);
        bottom->GetIntrinsicHeight(&height);
        bottom->SetBounds(0, 0, width, height);
    }
    return SetCompoundDrawables(left, top, right, bottom);
}

ECode TextView::SetCompoundDrawablesRelative(
    /* [in] */ IDrawable* start,
    /* [in] */ IDrawable* top,
    /* [in] */ IDrawable* end,
    /* [in] */ IDrawable* bottom)
{
    AutoPtr<Drawables> dr = mDrawables;

    if (dr) {
        if (dr->mDrawableLeft) dr->mDrawableLeft->SetCallback(NULL);
        dr->mDrawableLeft = dr->mDrawableLeftInitial = NULL;
        if (dr->mDrawableRight) dr->mDrawableRight->SetCallback(NULL);
        dr->mDrawableRight = dr->mDrawableRightInitial = NULL;
        dr->mDrawableSizeLeft = dr->mDrawableHeightLeft = 0;
        dr->mDrawableSizeRight = dr->mDrawableHeightRight = 0;
    }

    Boolean drawables = start != NULL || top != NULL
            || end != NULL || bottom != NULL;

    if (!drawables) {
        // Clearing drawables...  can we free the data structure?
        if (dr != NULL) {
            if (dr->mDrawablePadding == 0) {
                mDrawables = NULL;
            } else {
                // We need to retain the last set padding, so just clear
                // out all of the fields in the existing structure.
                if (dr->mDrawableStart != NULL) dr->mDrawableStart->SetCallback(NULL);
                dr->mDrawableStart = NULL;
                if (dr->mDrawableTop != NULL) dr->mDrawableTop->SetCallback(NULL);
                dr->mDrawableTop = NULL;
                if (dr->mDrawableEnd != NULL) dr->mDrawableEnd->SetCallback(NULL);
                dr->mDrawableEnd = NULL;
                if (dr->mDrawableBottom != NULL) dr->mDrawableBottom->SetCallback(NULL);
                dr->mDrawableBottom = NULL;
                dr->mDrawableSizeStart = dr->mDrawableHeightStart = 0;
                dr->mDrawableSizeEnd = dr->mDrawableHeightEnd = 0;
                dr->mDrawableSizeTop = dr->mDrawableWidthTop = 0;
                dr->mDrawableSizeBottom = dr->mDrawableWidthBottom = 0;
            }
        }
    } else {
        if (dr == NULL) {
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            mDrawables = dr = new Drawables(ctx);
        }

         mDrawables->mOverride = TRUE;

        if (dr->mDrawableStart.Get() != start && dr->mDrawableStart != NULL) {
            dr->mDrawableStart->SetCallback(NULL);
        }
        dr->mDrawableStart = start;

        if (dr->mDrawableTop.Get() != top && dr->mDrawableTop != NULL) {
            dr->mDrawableTop->SetCallback(NULL);
        }
        dr->mDrawableTop = top;

        if (dr->mDrawableEnd.Get() != end && dr->mDrawableEnd != NULL) {
            dr->mDrawableEnd->SetCallback(NULL);
        }
        dr->mDrawableEnd = end;

        if (dr->mDrawableBottom.Get() != bottom && dr->mDrawableBottom != NULL) {
            dr->mDrawableBottom->SetCallback(NULL);
        }
        dr->mDrawableBottom = bottom;

        AutoPtr<IRect> compoundRect = dr->mCompoundRect;
        AutoPtr<ArrayOf<Int32> > state;
        GetDrawableState((ArrayOf<Int32>**)&state);
        Int32 cw, ch;
        Boolean result;
        if (start != NULL) {
            start->SetState(state, &result);
            start->CopyBounds(compoundRect);
            start->SetCallback(this);
            compoundRect->GetWidth(&cw);
            compoundRect->GetHeight(&ch);
            dr->mDrawableSizeStart = cw;
            dr->mDrawableHeightStart = ch;
        } else {
            dr->mDrawableSizeStart = dr->mDrawableHeightStart = 0;
        }

        if (end != NULL) {
            end->SetState(state, &result);
            end->CopyBounds(compoundRect);
            end->SetCallback(this);
            compoundRect->GetWidth(&cw);
            compoundRect->GetHeight(&ch);
            dr->mDrawableSizeEnd = cw;
            dr->mDrawableHeightEnd = ch;
        } else {
            dr->mDrawableSizeEnd = dr->mDrawableHeightEnd = 0;
        }

        if (top != NULL) {
            top->SetState(state, &result);
            top->CopyBounds(compoundRect);
            top->SetCallback(this);
            compoundRect->GetWidth(&cw);
            compoundRect->GetHeight(&ch);
            dr->mDrawableSizeTop = ch;
            dr->mDrawableWidthTop = cw;
        } else {
            dr->mDrawableSizeTop = dr->mDrawableWidthTop = 0;
        }

        if (bottom != NULL) {
            bottom->SetState(state, &result);
            bottom->CopyBounds(compoundRect);
            bottom->SetCallback(this);
            compoundRect->GetWidth(&cw);
            compoundRect->GetHeight(&ch);
            dr->mDrawableSizeBottom = ch;
            dr->mDrawableWidthBottom = cw;
        } else {
            dr->mDrawableSizeBottom = dr->mDrawableWidthBottom = 0;
        }
    }

    ResetResolvedDrawables();
    ResolveDrawables();
    Invalidate();
    RequestLayout();

    return NOERROR;
}

ECode TextView::SetCompoundDrawablesRelativeWithIntrinsicBounds(
    /* [in] */ Int32 start,
    /* [in] */ Int32 top,
    /* [in] */ Int32 end,
    /* [in] */ Int32 bottom)
{
    AutoPtr<IContext> c;
    GetContext((IContext**)&c);

    AutoPtr<IDrawable> drStart, drTop, drEnd, drBottom;
    if (start != 0) {
        c->GetDrawable(start, (IDrawable**)&drStart);
    }
    if (top != 0) {
        c->GetDrawable(top, (IDrawable**)&drTop);
    }
    if (end != 0) {
        c->GetDrawable(end, (IDrawable**)&drEnd);
    }
    if (bottom != 0) {
        c->GetDrawable(bottom, (IDrawable**)&drBottom);
    }
    return SetCompoundDrawablesRelativeWithIntrinsicBounds(
            drStart, drTop, drEnd, drBottom);
}

ECode TextView::SetCompoundDrawablesRelativeWithIntrinsicBounds(
    /* [in] */ IDrawable* start,
    /* [in] */ IDrawable* top,
    /* [in] */ IDrawable* end,
    /* [in] */ IDrawable* bottom)
{
    ResetResolvedDrawables();
    Int32 w, h;

    if (start != NULL) {
        start->SetBounds(0, 0, (start->GetIntrinsicWidth(&w), w), (start->GetIntrinsicHeight(&h), h));
    }
    if (end != NULL) {
        end->SetBounds(0, 0, (end->GetIntrinsicWidth(&w), w), (end->GetIntrinsicHeight(&h), h));
    }
    if (top != NULL) {
        top->SetBounds(0, 0, (top->GetIntrinsicWidth(&w), w), (top->GetIntrinsicHeight(&h), h));
    }
    if (bottom != NULL) {
        bottom->SetBounds(0, 0, (bottom->GetIntrinsicWidth(&w), w), (bottom->GetIntrinsicHeight(&h), h));
    }
    return SetCompoundDrawablesRelative(start, top, end, bottom);
}

ECode TextView::GetCompoundDrawables(
    /* [out, callee] */ ArrayOf<IDrawable*>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr< ArrayOf<IDrawable*> > drawables = ArrayOf<IDrawable*>::Alloc(4);
    AutoPtr<Drawables> dr = mDrawables;
    if (dr != NULL) {
        drawables->Set(0, dr->mDrawableLeft);
        drawables->Set(1, dr->mDrawableTop);
        drawables->Set(2, dr->mDrawableRight);
        drawables->Set(3, dr->mDrawableBottom);
    } else {
        drawables->Set(0, NULL);
        drawables->Set(1, NULL);
        drawables->Set(2, NULL);
        drawables->Set(3, NULL);
    }
    *result = drawables;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TextView::GetCompoundDrawablesRelative(
    /* [out, callee] */ ArrayOf<IDrawable*>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<IDrawable*> > drawables = ArrayOf<IDrawable*>::Alloc(4);
    AutoPtr<Drawables> dr = mDrawables;
    if (dr != NULL) {
        drawables->Set(0, dr->mDrawableStart);
        drawables->Set(1, dr->mDrawableTop);
        drawables->Set(2, dr->mDrawableEnd);
        drawables->Set(3, dr->mDrawableBottom);
    } else {
        drawables->Set(0, NULL);
        drawables->Set(1, NULL);
        drawables->Set(2, NULL);
        drawables->Set(3, NULL);
    }
    *result = drawables;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TextView::SetCompoundDrawablePadding(
    /* [in] */ Int32 pad)
{
    AutoPtr<Drawables> dr = mDrawables;
    if (pad == 0) {
        if (dr != NULL) {
            dr->mDrawablePadding = pad;
        }
    } else {
        if (dr == NULL) {
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            mDrawables = dr = new Drawables(ctx);
        }
        dr->mDrawablePadding = pad;
    }

    Invalidate();
    RequestLayout();
    return NOERROR;
}

ECode TextView::GetCompoundDrawablePadding(
    /* [out] */ Int32* pad)
{
    VALIDATE_NOT_NULL(pad)
    AutoPtr<Drawables> dr = mDrawables;
    if (dr != NULL) {
        *pad = dr->mDrawablePadding;
    } else {
        *pad = 0;
    }
    return NOERROR;
}

ECode TextView::SetPadding(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    if (left != mPaddingLeft ||
        right != mPaddingRight ||
        top != mPaddingTop ||
        bottom != mPaddingBottom) {
        NullLayouts();
    }

    // the super call will RequestLayout()
    View::SetPadding(left, top, right, bottom);
    Invalidate();
    return NOERROR;
}

ECode TextView::SetPaddingRelative(
    /* [in] */ Int32 start,
    /* [in] */ Int32 top,
    /* [in] */ Int32 end,
    /* [in] */ Int32 bottom)
{
    Int32 paddingStart, paddingEnd;
    if (start != (GetPaddingStart(&paddingStart), paddingStart)
     || end != (GetPaddingEnd(&paddingEnd), paddingEnd)
     || top != mPaddingTop || bottom != mPaddingBottom) {
            NullLayouts();
    }

    // the super call will RequestLayout()
    View::SetPaddingRelative(start, top, end, bottom);
    Invalidate();
    return NOERROR;
}

Int32 TextView::GetAutoLinkMask(
    /* [out] */ Int32* mask)
{
    VALIDATE_NOT_NULL(mask)
    *mask = mAutoLinkMask;
    return NOERROR;
}

ECode TextView::SetTextAppearance(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resid)
{
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::TextAppearance),
        ArraySize(R::styleable::TextAppearance));
    AutoPtr<ITypedArray> appearance;
    ASSERT_SUCCEEDED(context->ObtainStyledAttributes(resid, attrIds, (ITypedArray**)&appearance));

    Int32 color;
    AutoPtr<IColorStateList> colors;
    Int32 ts;

    appearance->GetColor(R::styleable::TextAppearance_textColorHighlight,
            0, &color);
    if (color != 0) {
        SetHighlightColor(color);
    }

    appearance->GetColorStateList(R::styleable::TextAppearance_textColor,
            (IColorStateList**)&colors);
    if (colors != NULL) {
        SetTextColor(colors);
    }

    appearance->GetDimensionPixelSize(R::styleable::TextAppearance_textSize,
            0, &ts);
    if (ts != 0) {
        SetRawTextSize(ts);
    }

    colors = NULL;
    appearance->GetColorStateList(R::styleable::TextAppearance_textColorHint,
            (IColorStateList**)&colors);
    if (colors != NULL) {
        SetHintTextColor(colors);
    }

    colors = NULL;
    appearance->GetColorStateList(R::styleable::TextAppearance_textColorLink,
            (IColorStateList**)&colors);
    if (colors != NULL) {
        SetLinkTextColor(colors);
    }

    String familyName;

    Int32 typefaceIndex, styleIndex;

    appearance->GetString(R::styleable::TextAppearance_fontFamily, &familyName);

    appearance->GetInt32(R::styleable::TextAppearance_typeface,
            -1, &typefaceIndex);
    appearance->GetInt32(R::styleable::TextAppearance_textStyle,
            -1, &styleIndex);

    SetTypefaceFromAttrs(familyName, typefaceIndex, styleIndex);

    Int32 shadowcolor;
    appearance->GetInt32(R::styleable::TextAppearance_shadowColor, 0, &shadowcolor);
    if (shadowcolor != 0) {
        Float dx, dy, r;
        appearance->GetFloat(R::styleable::TextAppearance_shadowDx, 0, &dx);
        appearance->GetFloat(R::styleable::TextAppearance_shadowDy, 0, &dy);
        appearance->GetFloat(R::styleable::TextAppearance_shadowRadius, 0, &r);

        SetShadowLayer(r, dx, dy, shadowcolor);
    }

    Boolean rValue;
    appearance->GetBoolean(R::styleable::TextAppearance_textAllCaps, FALSE, &rValue);
    if (rValue) {
        AutoPtr<ITransformationMethod> method;
        AutoPtr<IContext> c;
        GetContext((IContext**)&c);
        assert(0 && "TODO");
//TODO        CAllCapsTransformationMethod::New(c, (IAllCapsTransformationMethod**)&method);
        SetTransformationMethod(method);
    }


    if (appearance->HasValue(R::styleable::TextAppearance_elegantTextHeight, &rValue), rValue) {
        SetElegantTextHeight((appearance->GetBoolean(R::styleable::TextAppearance_elegantTextHeight,
            FALSE, &rValue), rValue));
    }

    if (appearance->HasValue(R::styleable::TextAppearance_letterSpacing, &rValue), rValue) {
        Float spacing;
        appearance->GetFloat(R::styleable::TextAppearance_letterSpacing, 0, &spacing);
        SetLetterSpacing(spacing);
    }

    if (appearance->HasValue(R::styleable::TextAppearance_fontFeatureSettings, &rValue), rValue) {
        String spacing;
        appearance->GetString(R::styleable::TextAppearance_fontFeatureSettings, &spacing);
        SetFontFeatureSettings(spacing);
    }

    appearance->Recycle();
    return NOERROR;
}

ECode TextView::GetTextLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    return IPaint::Probe(mTextPaint)->GetTextLocale(locale);
}

ECode TextView::SetTextLocale(
    /* [in] */ ILocale* locale)
{
    return IPaint::Probe(mTextPaint)->SetTextLocale(locale);
}

ECode TextView::GetTextSize(
    /* [out] */ Float* size)
{
    VALIDATE_NOT_NULL(size)
    return IPaint::Probe(mTextPaint)->GetTextSize(size);
}

ECode TextView::GetScaledTextSize(
    /* [out] */ Float* size)
{
    VALIDATE_NOT_NULL(size)
    Float textSize, density;
    IPaint::Probe(mTextPaint)->GetTextSize(&textSize);
    mTextPaint->GetDensity(&density);
    *size = textSize / density;
    return NOERROR;
}

ECode TextView::GetTypefaceStyle(
    /* [out] */ Int32* style)
{
    VALIDATE_NOT_NULL(style)
    AutoPtr<ITypeface> typeface;
    IPaint::Probe(mTextPaint)->GetTypeface((ITypeface**)&typeface);
    typeface->GetStyle(style);
    return NOERROR;
}

ECode TextView::SetTextSize(
    /* [in] */ Float size)
{
    return SetTextSize(ITypedValue::COMPLEX_UNIT_SP, size);
}

ECode TextView::SetTextSize(
    /* [in] */ Int32 unit,
    /* [in] */ Float size)
{
    AutoPtr<IContext> c;
    GetContext((IContext**)&c);
    AutoPtr<IResources> r;

    if (c == NULL) {
        AutoPtr<IResourcesHelper> helper;
        CResourcesHelper::AcquireSingleton((IResourcesHelper**)&helper);
        helper->GetSystem((IResources**)&r);
    }
    else {
        c->GetResources((IResources**)&r);
    }

    AutoPtr<IDisplayMetrics> metrics;
    r->GetDisplayMetrics((IDisplayMetrics**)&metrics);

    AutoPtr<ITypedValueHelper> tvh;
    CTypedValueHelper::AcquireSingleton((ITypedValueHelper**)&tvh);
    assert(tvh != NULL);
    Float result;
    tvh->ApplyDimension(unit, size, metrics, &result);
    SetRawTextSize(result);
    return NOERROR;
}

void TextView::SetRawTextSize(
    /* [in] */ Float size)
{
    assert(mTextPaint != NULL);
    Float textSize;
    IPaint::Probe(mTextPaint)->GetTextSize(&textSize);
    if (size != textSize) {
        IPaint::Probe(mTextPaint)->SetTextSize(size);

        if (mLayout != NULL) {
            NullLayouts();
            RequestLayout();
            Invalidate();
        }
    }
}

ECode TextView::GetTextScaleX(
    /* [out] */ Float* size)
{
    VALIDATE_NOT_NULL(size)
    return IPaint::Probe(mTextPaint)->GetTextScaleX(size);
}

ECode TextView::SetTextScaleX(
    /* [in] */ Float size)
{
    Float scaleX;
    IPaint::Probe(mTextPaint)->GetTextScaleX(&scaleX);
    if (size != scaleX) {
        mUserSetTextScaleX = TRUE;
        IPaint::Probe(mTextPaint)->SetTextScaleX(size);

        if (mLayout != NULL) {
            NullLayouts();
            RequestLayout();
            Invalidate();
        }
    }
    return NOERROR;
}

ECode TextView::SetTypeface(
    /* [in] */ ITypeface* tf)
{
    AutoPtr<ITypeface> face;
    IPaint::Probe(mTextPaint)->GetTypeface((ITypeface**)&face);
    if (face.Get() != tf) {
        IPaint::Probe(mTextPaint)->SetTypeface(tf);

        if (mLayout != NULL) {
            NullLayouts();
            RequestLayout();
            Invalidate();
        }
    }
    return NOERROR;
}

ECode TextView::GetTypeface(
    /* [out] */ ITypeface** face)
{
    VALIDATE_NOT_NULL(face)
    return IPaint::Probe(mTextPaint)->GetTypeface(face);
}

ECode TextView::SetElegantTextHeight(
    /* [in] */ Boolean elegant)
{
    IPaint::Probe(mTextPaint)->SetElegantTextHeight(elegant);
    return NOERROR;
}

ECode TextView::GetLetterSpacing(
    /* [out] */ Float* spacing)
{
    VALIDATE_NOT_NULL(spacing)
    IPaint::Probe(mTextPaint)->GetLetterSpacing(spacing);
    return NOERROR;
}

ECode TextView::SetLetterSpacing(
    /* [in] */ Float letterSpacing)
{
    Float res;
    if (letterSpacing != (IPaint::Probe(mTextPaint)->GetLetterSpacing(&res), res)) {
        IPaint::Probe(mTextPaint)->SetLetterSpacing(letterSpacing);

        if (mLayout) {
            NullLayouts();
            RequestLayout();
            Invalidate();
        }
    }

    return NOERROR;
}

ECode TextView::GetFontFeatureSettings(
    /* [out] */ String* settings)
{
    VALIDATE_NOT_NULL(settings)
    return IPaint::Probe(mTextPaint)->GetFontFeatureSettings(settings);
}

ECode TextView::SetFontFeatureSettings(
    /* [in] */ const String& fontFeatureSettings)
{
    String res;
    IPaint::Probe(mTextPaint)->GetFontFeatureSettings(&res);
    if (fontFeatureSettings.Equals(res)) {
        IPaint::Probe(mTextPaint)->SetFontFeatureSettings(fontFeatureSettings);

        if (mLayout) {
            NullLayouts();
            RequestLayout();
            Invalidate();
        }
    }

    return NOERROR;
}

ECode TextView::SetTextColor(
    /* [in] */ Int32 color)
{
    mTextColor = NULL;
    CColorStateList::ValueOf(color, (IColorStateList**)&mTextColor);
    assert(mTextColor != NULL);
    UpdateTextColors();
    return NOERROR;
}

ECode TextView::SetTextColor(
    /* [in] */ IColorStateList* colors)
{
    if (colors == NULL) {
        //throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    mTextColor = colors;
    UpdateTextColors();
    return NOERROR;
}

ECode TextView::GetTextColors(
    /* [out] */ IColorStateList** colors)
{
    VALIDATE_NOT_NULL(colors)
    *colors = mTextColor;
    REFCOUNT_ADD(*colors)
    return NOERROR;
}

ECode TextView::GetCurrentTextColor(
    /* [out] */ Int32* colors)
{
    VALIDATE_NOT_NULL(colors)
    *colors = mCurTextColor;
    return NOERROR;
}

ECode TextView::SetHighlightColor(
    /* [in] */ Int32 color)
{
    if (mHighlightColor != color) {
        mHighlightColor = color;
        Invalidate();
    }
    return NOERROR;
}

ECode TextView::GetHighlightColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color)
    *color = mHighlightColor;
    return NOERROR;
}

ECode TextView::SetShowSoftInputOnFocus(
    /* [in] */ Boolean show)
{
    CreateEditorIfNeeded();
    assert(0);
    //mEditor->mShowSoftInputOnFocus = show;
    return NOERROR;
}

ECode TextView::GetShowSoftInputOnFocus(
    /* [out] */ Boolean* show)
{
    VALIDATE_NOT_NULL(show)
    assert(0);
    //*show = mEditor == NULL || mEditor->mShowSoftInputOnFocus;
    return NOERROR;
}

ECode TextView::SetShadowLayer(
    /* [in] */ Float radius,
    /* [in] */ Float dx,
    /* [in] */ Float dy,
    /* [in] */ Int32 color)
{
    IPaint::Probe(mTextPaint)->SetShadowLayer(radius, dx, dy, color);

    mShadowRadius = radius;
    mShadowDx = dx;
    mShadowDy = dy;
    mShadowColor = color;

    // Will change text clip region
    assert(0);
    //if (mEditor != NULL) mEditor->InvalidateTextDisplayList();

    Invalidate();
    return NOERROR;
}

ECode TextView::GetShadowRadius(
    /* [out] */ Float* radius)
{
    VALIDATE_NOT_NULL(radius)
    *radius = mShadowRadius;
    return NOERROR;
}

ECode TextView::GetShadowDx(
    /* [out] */ Float* dx)
{
    VALIDATE_NOT_NULL(dx)
    *dx = mShadowDx;
    return NOERROR;
}

ECode TextView::GetShadowDy(
    /* [out] */ Float* dy)
{
    VALIDATE_NOT_NULL(dy)
    *dy = mShadowDy;
    return NOERROR;
}

ECode TextView::GetShadowColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color)
    *color = mShadowColor;
    return NOERROR;
}

ECode TextView::GetPaint(
    /* [out] */ ITextPaint** paint)
{
    VALIDATE_NOT_NULL(paint)
    *paint = mTextPaint;
    REFCOUNT_ADD(*paint)
    return NOERROR;
}

ECode TextView::SetAutoLinkMask(
    /* [in] */ Int32 mask)
{
    mAutoLinkMask = mask;
    return NOERROR;
}

ECode TextView::SetLinksClickable(
    /* [in] */ Boolean whether)
{
    mLinksClickable = whether;
    return NOERROR;
}

ECode TextView::GetLinksClickable(
    /* [out] */ Boolean* whether)
{
    VALIDATE_NOT_NULL(whether)
    *whether = mLinksClickable;
    return NOERROR;
}

ECode TextView::GetUrls(
    /* [out, callee] */ ArrayOf<IURLSpan*>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr< ArrayOf<IURLSpan*> > urls;
    AutoPtr<ISpanned> spanned = ISpanned::Probe(mText);
    if (spanned) {
        Int32 len;
        mText->GetLength(&len);
        AutoPtr< ArrayOf<IInterface*> > temp;
        spanned->GetSpans(0, len, EIID_IURLSpan, (ArrayOf<IInterface*>**)&temp);
        urls = ArrayOf<IURLSpan*>::Alloc(temp->GetLength());
        for (Int32 i = 0; i < temp->GetLength(); i++) {
            urls->Set(i, IURLSpan::Probe((*temp)[i]));
        }
    } else {
        urls = ArrayOf<IURLSpan*>::Alloc(0);
    }

    *result = urls;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TextView::SetHintTextColor(
    /* [in] */ Int32 color)
{
    mHintTextColor = NULL;
    AutoPtr<IColorStateListHelper> helper;
    CColorStateListHelper::AcquireSingleton((IColorStateListHelper**)&helper);
    helper->ValueOf(color, (IColorStateList**)&mHintTextColor);
    UpdateTextColors();
    return NOERROR;
}

ECode TextView::SetHintTextColor(
    /* [in] */ IColorStateList* colors)
{
    mHintTextColor = colors;
    UpdateTextColors();
    return NOERROR;
}

ECode TextView::GetHintTextColors(
    /* [out] */ IColorStateList** colors)
{
    VALIDATE_NOT_NULL(colors)
    *colors = mHintTextColor;
    REFCOUNT_ADD(*colors)
    return NOERROR;
}

ECode TextView::GetCurrentHintTextColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color)
    *color = mHintTextColor != NULL ? mCurHintTextColor : mCurTextColor;
    return NOERROR;
}

ECode TextView::SetLinkTextColor(
    /* [in] */ Int32 color)
{
    mLinkTextColor = NULL;
    AutoPtr<IColorStateListHelper> helper;
    CColorStateListHelper::AcquireSingleton((IColorStateListHelper**)&helper);
    helper->ValueOf(color, (IColorStateList**)&mLinkTextColor);
    UpdateTextColors();
    return NOERROR;
}

ECode TextView::SetLinkTextColor(
    /* [in] */ IColorStateList* colors)
{
    mLinkTextColor = colors;
    UpdateTextColors();
    return NOERROR;
}

ECode TextView::GetLinkTextColors(
    /* [out] */ IColorStateList** colors)
{
    VALIDATE_NOT_NULL(colors)
    *colors = mLinkTextColor;
    REFCOUNT_ADD(*colors)
    return NOERROR;
}

ECode TextView::SetGravity(
    /* [in] */ Int32 gravity)
{
    if ((gravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) == 0) {
        gravity |= IGravity::START;
    }
    if ((gravity & IGravity::VERTICAL_GRAVITY_MASK) == 0) {
        gravity |= IGravity::TOP;
    }

    Boolean newLayout = FALSE;

    if ((gravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) !=
        (mGravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK)) {
        newLayout = TRUE;
    }

    if (gravity != mGravity) {
        Invalidate();
    }

    mGravity = gravity;

    if (mLayout != NULL && newLayout) {
        // XXX this is heavy-handed because no actual content changes.
        Int32 want = 0, hintWant = 0;
        mLayout->GetWidth(&want);
        if (mHintLayout != NULL) mHintLayout->GetWidth(&hintWant);
        Int32 compoundPaddingLeft, compoundPaddingRight;
        GetCompoundPaddingLeft(&compoundPaddingLeft);
        GetCompoundPaddingRight(&compoundPaddingRight);
        MakeNewLayout(want, hintWant, UNKNOWN_BORING, UNKNOWN_BORING,
                mRight - mLeft - compoundPaddingLeft - compoundPaddingRight, TRUE);
    }
    return NOERROR;
}

ECode TextView::GetGravity(
    /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity)
    *gravity = mGravity;
    return NOERROR;
}

ECode TextView::GetPaintFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags)
    return IPaint::Probe(mTextPaint)->GetFlags(flags);
}

ECode TextView::SetPaintFlags(
    /* [in] */ Int32 flags)
{
    Int32 oldFlags;
    IPaint::Probe(mTextPaint)->GetFlags(&oldFlags);
    if (oldFlags != flags) {
        IPaint::Probe(mTextPaint)->SetFlags(flags);

        if (mLayout != NULL) {
            NullLayouts();
            RequestLayout();
            Invalidate();
        }
    }
    return NOERROR;
}

ECode TextView::SetHorizontallyScrolling(
    /* [in] */ Boolean whether)
{
    if (mHorizontallyScrolling != whether) {
        mHorizontallyScrolling = whether;

        if (mLayout != NULL) {
            NullLayouts();
            RequestLayout();
            Invalidate();
        }
    }
    return NOERROR;
}

ECode TextView::GetHorizontallyScrolling(
    /* [out] */ Boolean* whether)
{
    VALIDATE_NOT_NULL(whether)
    *whether = mHorizontallyScrolling;
    return NOERROR;
}

ECode TextView::SetMinLines(
    /* [in] */ Int32 minlines)
{
    mMinimum = minlines;
    mMinMode = LINES;

    RequestLayout();
    Invalidate();
    return NOERROR;
}

ECode TextView::GetMinLines(
    /* [out] */ Int32* minlines)
{
    VALIDATE_NOT_NULL(minlines)
    *minlines = mMinMode == LINES ? mMinimum : -1;
    return NOERROR;
}

ECode TextView::SetMinHeight(
    /* [in] */ Int32 minHeight)
{
    mMinimum = minHeight;
    mMinMode = PIXELS;

    RequestLayout();
    Invalidate();
    return NOERROR;
}

ECode TextView::GetMinHeight(
   /* [out] */ Int32* minHeight)
{
    VALIDATE_NOT_NULL(minHeight)
    *minHeight = mMinMode == PIXELS ? mMinimum : -1;
    return NOERROR;
}

ECode TextView::SetMaxLines(
    /* [in] */ Int32 maxlines)
{
    mMaximum = maxlines;
    mMaxMode = LINES;

    RequestLayout();
    Invalidate();
    return NOERROR;
}

ECode TextView::GetMaxLines(
    /* [out] */ Int32* maxlines)
{
    VALIDATE_NOT_NULL(maxlines)
    *maxlines = mMaxMode == LINES ? mMaximum : -1;
    return NOERROR;
}

ECode TextView::SetMaxHeight(
    /* [in] */ Int32 maxHeight)
{
    mMaximum = maxHeight;
    mMaxMode = PIXELS;

    RequestLayout();
    Invalidate();
    return NOERROR;
}

ECode TextView::GetMaxHeight(
    /* [out] */ Int32* maxHeight)
{
    VALIDATE_NOT_NULL(maxHeight)
    *maxHeight = mMaxMode == PIXELS ? mMaximum : -1;
    return NOERROR;
}

ECode TextView::SetLines(
    /* [in] */ Int32 lines)
{
    mMaximum = mMinimum = lines;
    mMaxMode = mMinMode = LINES;

    RequestLayout();
    Invalidate();
    return NOERROR;
}

ECode TextView::SetHeight(
    /* [in] */ Int32 pixels)
{
    mMaximum = mMinimum = pixels;
    mMaxMode = mMinMode = PIXELS;

    RequestLayout();
    Invalidate();
    return NOERROR;
}

ECode TextView::SetMinEms(
    /* [in] */ Int32 minems)
{
    mMinWidth = minems;
    mMinWidthMode = EMS;

    RequestLayout();
    Invalidate();
    return NOERROR;
}

ECode TextView::GetMinEms(
    /* [out] */ Int32* minems)
{
    VALIDATE_NOT_NULL(minems)
    *minems = mMinWidthMode == EMS ? mMinWidth : -1;
    return NOERROR;
}

ECode TextView::SetMinWidth(
    /* [in] */ Int32 minpixels)
{
    mMinWidth = minpixels;
    mMinWidthMode = PIXELS;

    RequestLayout();
    Invalidate();
    return NOERROR;
}

ECode TextView::GetMinWidth(
    /* [out] */ Int32* minpixels)
{
    VALIDATE_NOT_NULL(minpixels)
    *minpixels = mMinWidthMode == PIXELS ? mMinWidth : -1;
    return NOERROR;
}

ECode TextView::SetMaxEms(
    /* [in] */ Int32 maxems)
{
    mMaxWidth = maxems;
    mMaxWidthMode = EMS;

    RequestLayout();
    Invalidate();
    return NOERROR;
}

ECode TextView::GetMaxEms(
    /* [out] */ Int32* ems)
{
    VALIDATE_NOT_NULL(ems)
    *ems = mMaxWidthMode == EMS ? mMaxWidth : -1;
    return NOERROR;
}

ECode TextView::SetMaxWidth(
    /* [in] */ Int32 maxpixels)
{
    mMaxWidth = maxpixels;
    mMaxWidthMode = PIXELS;

    RequestLayout();
    Invalidate();
    return NOERROR;
}

ECode TextView::GetMaxWidth(
    /* [out] */ Int32* maxpixels)
{
    VALIDATE_NOT_NULL(maxpixels)
    *maxpixels = mMaxWidthMode == PIXELS ? mMaxWidth : -1;
    return NOERROR;
}

ECode TextView::SetEms(
    /* [in] */ Int32 ems)
{
    mMaxWidth = mMinWidth = ems;
    mMaxWidthMode = mMinWidthMode = EMS;

    RequestLayout();
    Invalidate();
    return NOERROR;
}

ECode TextView::SetWidth(
    /* [in] */ Int32 pixels)
{
    mMaxWidth = mMinWidth = pixels;
    mMaxWidthMode = mMinWidthMode = PIXELS;

    RequestLayout();
    Invalidate();
    return NOERROR;
}

ECode TextView::SetLineSpacing(
    /* [in] */ Float add,
    /* [in] */ Float mult)
{
    if (mSpacingAdd != add || mSpacingMult != mult) {
       mSpacingAdd = add;
       mSpacingMult = mult;

        if (mLayout != NULL) {
            NullLayouts();
            RequestLayout();
            Invalidate();
        }

    }
    return NOERROR;
}

ECode TextView::GetLineSpacingMultiplier(
    /* [out] */ Float* multiplier)
{
    VALIDATE_NOT_NULL(multiplier)
    *multiplier = mSpacingMult;
    return NOERROR;
}

ECode TextView::GetLineSpacingExtra(
    /* [out] */ Float* extra)
{
    VALIDATE_NOT_NULL(extra)
    *extra = mSpacingAdd;
    return NOERROR;
}

ECode TextView::Append(
    /* [in] */ ICharSequence* text)
{
    VALIDATE_NOT_NULL(text);
    Int32 len;
    text->GetLength(&len);
    return Append(text, 0, len);
}

ECode TextView::Append(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoPtr<IEditable> editable = IEditable::Probe(mText);
    if (!editable) {
        SetText(mText, BufferType_EDITABLE);
    }
    AutoPtr<IEditable> result = IEditable::Probe(mText);
    return result->Append(text, start, end);
}

void TextView::UpdateTextColors()
{
    Boolean inval = FALSE;
    Int32 color;
    AutoPtr< ArrayOf<Int32> > state;
    GetDrawableState((ArrayOf<Int32>**)&state);
    mTextColor->GetColorForState(state, 0, &color);

    if (color != mCurTextColor) {
        mCurTextColor = color;
        inval = TRUE;
    }

    if (mLinkTextColor != NULL) {
        mLinkTextColor->GetColorForState(state, 0, &color);
        Int32 tpColor;
        mTextPaint->GetLinkColor(&tpColor);
        if (color != tpColor) {
            mTextPaint->SetLinkColor(color);
            inval = TRUE;
        }
    }

    if (mHintTextColor != NULL) {
        mHintTextColor->GetColorForState(state, 0, &color);
        Int32 len;
        mText->GetLength(&len);
        if (color != mCurHintTextColor && len == 0) {
            mCurHintTextColor = color;
            inval = TRUE;
        }
    }

    if (inval) {
        // Text needs to be redrawn with the new color
        assert(0);
        //if (mEditor != NULL) mEditor->InvalidateTextDisplayList();
        Invalidate();
    }
}

ECode TextView::DrawableStateChanged()
{
    View::DrawableStateChanged();

    Boolean textColor = FALSE;
    Boolean hintColor = FALSE;
    Boolean linkColor = FALSE;
    if (mTextColor != NULL) mTextColor->IsStateful(&textColor);
    if (mHintTextColor != NULL) mHintTextColor->IsStateful(&hintColor);
    if (mLinkTextColor != NULL) mLinkTextColor->IsStateful(&linkColor);

    if (textColor || hintColor ||  linkColor) {
        UpdateTextColors();
    }

    AutoPtr<Drawables> dr = mDrawables;
    if (dr != NULL) {
        AutoPtr<ArrayOf<Int32> > state;
        GetDrawableState((ArrayOf<Int32>**)&state);
        Boolean isStateful = FALSE;
        if (dr->mDrawableTop != NULL && (dr->mDrawableTop->IsStateful(&isStateful), isStateful)) {
            dr->mDrawableTop->SetState(state, &isStateful);
        }

        if (dr->mDrawableBottom != NULL && (dr->mDrawableBottom->IsStateful(&isStateful), isStateful)) {
            dr->mDrawableBottom->SetState(state, &isStateful);
        }

        if (dr->mDrawableLeft != NULL && (dr->mDrawableLeft->IsStateful(&isStateful), isStateful)) {
            dr->mDrawableLeft->SetState(state, &isStateful);
        }

        if (dr->mDrawableRight != NULL && (dr->mDrawableRight->IsStateful(&isStateful), isStateful)) {
            dr->mDrawableRight->SetState(state, &isStateful);
        }

        if (dr->mDrawableStart != NULL && (dr->mDrawableStart->IsStateful(&isStateful), isStateful)) {
            dr->mDrawableStart->SetState(state, &isStateful);
        }

        if (dr->mDrawableEnd != NULL && (dr->mDrawableEnd->IsStateful(&isStateful), isStateful)) {
            dr->mDrawableEnd->SetState(state, &isStateful);
        }
    }

    return NOERROR;
}

ECode TextView::DrawableHotspotChanged(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    View::DrawableHotspotChanged(x, y);

    AutoPtr<Drawables> dr = mDrawables;
    if (dr) {
        if (dr->mDrawableTop) {
            dr->mDrawableTop->SetHotspot(x, y);
        }
        if (dr->mDrawableBottom) {
            dr->mDrawableBottom->SetHotspot(x, y);
        }
        if (dr->mDrawableLeft) {
            dr->mDrawableLeft->SetHotspot(x, y);
        }
        if (dr->mDrawableRight) {
            dr->mDrawableRight->SetHotspot(x, y);
        }
        if (dr->mDrawableStart) {
            dr->mDrawableStart->SetHotspot(x, y);
        }
        if (dr->mDrawableEnd) {
            dr->mDrawableEnd->SetHotspot(x, y);
        }
    }

    return NOERROR;
}

AutoPtr<IParcelable> TextView::OnSaveInstanceState()
{
    AutoPtr<IParcelable> superState = View::OnSaveInstanceState();

    // Save state if we are forced to
    Boolean save = mFreezesText;
    Int32 start = 0;
    Int32 end = 0;

    if (mText != NULL) {
        GetSelectionStart(&start);
        GetSelectionEnd(&end);
        if (start >= 0 || end >= 0) {
            // Or save state if there is a selection
            save = TRUE;
        }
    }
    assert(0 && "TODO");
// TODO
//    AutoPtr<CTextViewSavedState> ss;
//   if (save) {
//       //TODO CTextViewSavedState::NewByFriend(superState, (CTextViewSavedState**)&ss);
//       // XXX Should also save the current scroll position!
//       ss->mSelStart = start;
//       ss->mSelEnd = end;
//
//       if (ISpanned::Probe(mText)) {
//
//           AutoPtr<ISpannable> sp;
//           CSpannableStringBuilder::New(mText, (ISpannable**)&sp);
//
//           if (mEditor != NULL) {
//               removeMisspelledSpans(sp);
//               sp.removeSpan(mEditor->mSuggestionRangeSpan);
//           }
//
//           ss->mText = sp;
//       }
//       else {
//           ss->mText = mText;
//       }
//
//       if (IsFocused() && start >= 0 && end >= 0) {
//           ss->mFrozenWithFocus = TRUE;
//       }
//
//       ss->mError = GetError();
//
//       AutoPtr<IParcelable> pss = (IParcelable*)ss->Probe(EIID_IParcelable);
//       return pss;
//   }

    return superState;
}

ECode TextView::RemoveMisspelledSpans(
    /* [in] */ ISpannable* spannable)
{
    assert(spannable != NULL);

    Int32 length;
    ICharSequence::Probe(spannable)->GetLength(&length);

    AutoPtr<ArrayOf<IInterface*> > temp;
    ISpanned::Probe(spannable)->GetSpans(0, length, EIID_ISuggestionSpan, (ArrayOf<IInterface*>**)&temp);
    AutoPtr<ArrayOf<ISuggestionSpan*> > suggestionSpans = ArrayOf<ISuggestionSpan*>::Alloc(temp->GetLength());
    for (Int32 i = 0; i < temp->GetLength(); i++)
    {
        suggestionSpans->Set(i, ISuggestionSpan::Probe((*temp)[i]));
    }

    for (Int32 i = 0; i < suggestionSpans->GetLength(); i++) {
        Int32 flags;
        (*suggestionSpans)[i]->GetFlags(&flags);
        if ((flags & ISuggestionSpan::FLAG_EASY_CORRECT) != 0
            && (flags & ISuggestionSpan::FLAG_MISSPELLED) != 0) {
                spannable->RemoveSpan((*suggestionSpans)[i]);
        }
    }

    return NOERROR;
}

void TextView::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    assert(0 && "TODO");
    if (!ITextViewSavedState::Probe(state)) {
       View::OnRestoreInstanceState(state);
       return;
    }
// TODO
//    CTextViewSavedState* ss = (CTextViewSavedState*)ITextViewSavedState::Probe(state);
//
//    AutoPtr<IParcelable> superState;
//    ss->GetSuperState((IParcelable**)&superState);
//    View::OnRestoreInstanceState(superState);
//
//    // XXX restore buffer type too, as well as lots of other stuff
//    if (ss->mText != NULL) {
//       SetText(ss->mText);
//    }
//
//    if (ss->mSelStart >= 0 && ss->mSelEnd >= 0) {
//       if (ISpannable::Probe(mText)) {
//           Int32 len;
//           mText->GetLength(&len);
//
//           if (ss->mSelStart > len || ss->mSelEnd > len) {
//               String restored("");
//
//               if (ss->mText != NULL) {
//                   restored = String("(restored) ");
//               }
//
//               // Logger::e(TEXT_VIEW_TAG, "Saved cursor position " + ss.selStart +
//               //         "/" + ss.selEnd + " out of range for " + restored +
//               //         "text " + mText);
//           }
//           else {
//                Selection::SetSelection(
//                ISpannable::Probe(mText), ss->mSelStart, ss->mSelEnd);
//
//                if (ss->mFrozenWithFocus) {
//                    CreateEditorIfNeeded();
//                    mEditor->mFrozenWithFocus = TRUE;
//                }
//           }
//       }
//    }
//
//    if (ss->mError != NULL) {
//       AutoPtr<ICharSequence> error = ss->mError;
//       // Display the error later, after the first layout pass
//       // post(new Runnable() {
//       //     public void run() {
//       //         setError(error);
//       //     }
//       // });
//    }
}

ECode TextView::SetFreezesText(
    /* [in] */ Boolean freezesText)
{
    mFreezesText = freezesText;
    return NOERROR;
}

ECode TextView::GetFreezesText(
    /* [out] */ Boolean* text)
{
    VALIDATE_NOT_NULL(text)
    *text = mFreezesText;
    return NOERROR;
}

ECode TextView::SetEditableFactory(
    /* [in] */ IEditableFactory* factory)
{
    mEditableFactory = factory;
    return SetText(mText);
}

ECode TextView::SetSpannableFactory(
    /* [in] */ ISpannableFactory* factory)
{
    mSpannableFactory = factory;
    return SetText(mText);
}

ECode TextView::SetText(
    /* [in] */ ICharSequence* text)
{
    return SetText(text, mBufferType);
}

ECode TextView::SetTextKeepState(
    /* [in] */ ICharSequence* text)
{
    return SetTextKeepState(text, mBufferType);
}

ECode TextView::SetText(
    /* [in] */ ICharSequence* text,
    /* [in] */ BufferType type)
{
    SetText(text, type, TRUE, 0);

    if (mCharWrapper != NULL) {
        mCharWrapper->mChars = NULL;
    }
    return NOERROR;
}

ECode TextView::SetText(
    /* [in] */ ICharSequence* _text,
    /* [in] */ BufferType type,
    /* [in] */ Boolean notifyBefore,
    /* [in] */ Int32 oldlen)
{
    AutoPtr<ICharSequence> text = _text;
    if (text == NULL) {
        CString::New(String(""), (ICharSequence**)&text);
    }

    // If suggestions are not enabled, remove the suggestion spans from the text
    Boolean isSuggestionsEnabled;
    if (IsSuggestionsEnabled(&isSuggestionsEnabled), !isSuggestionsEnabled) {
        text = RemoveSuggestionSpans(text);
        assert(text != NULL);
    }

    if (!mUserSetTextScaleX) IPaint::Probe(mTextPaint)->SetTextScaleX(1.0f);
    AutoPtr<ISpanned> spanned = ISpanned::Probe(text);
    if (spanned) {
        AutoPtr<IInteger32> iobj;
        CInteger32::New(TextUtilsTruncateAt_MARQUEE, (IInteger32**)&iobj);
        Int32 spanStart = 0;
        spanned->GetSpanStart(iobj->Probe(EIID_IInterface), &spanStart);
        if (spanStart >= 0) {
            AutoPtr<IViewConfigurationHelper> helper;
            assert(0);
            //CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
            AutoPtr<IViewConfiguration> vc;
            helper->Get(mContext, (IViewConfiguration**)&vc);
            Boolean enabled = FALSE;
            if (vc->IsFadingMarqueeEnabled(&enabled), enabled) {
                SetHorizontalFadingEdgeEnabled(TRUE);
                mMarqueeFadeMode = MARQUEE_FADE_NORMAL;
            } else {
                SetHorizontalFadingEdgeEnabled(FALSE);
                mMarqueeFadeMode = MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS;
            }
            SetEllipsize(TextUtilsTruncateAt_MARQUEE);
        }
    }

    Int32 len;
    AutoPtr<ICharSequence> out;
    Int32 n = mFilters->GetLength();
    for (Int32 i = 0; i < n; i++) {
        text->GetLength(&len);
        out = NULL;
        (*mFilters)[i]->Filter(text, 0, len, EMPTY_SPANNED, 0, 0, (ICharSequence**)&out);
        if (out != NULL) {
            text = out;
        }
    }

    if (notifyBefore) {
        text->GetLength(&len);
        if (mText != NULL) {
            mText->GetLength(&oldlen);
            SendBeforeTextChanged(mText, 0, oldlen, len);
        }
        else {
            AutoPtr<ICharSequence> cs;
            CString::New(String(""), (ICharSequence**)&cs);
            SendBeforeTextChanged(cs, 0, 0, len);
        }
    }

    Boolean needEditableForNotification = FALSE;

    if (mListeners.IsEmpty() == FALSE) {
        needEditableForNotification = TRUE;
    }

    AutoPtr<IKeyListener> keyListener;
    GetKeyListener((IKeyListener**)&keyListener);
    if (type == BufferType_EDITABLE || keyListener != NULL ||
        needEditableForNotification) {
        CreateEditorIfNeeded();
        assert(mEditableFactory != NULL);
        AutoPtr<IEditable> t;
        mEditableFactory->NewEditable(text, (IEditable**)&t);
        text = ICharSequence::Probe(t);
        SetFilters(t, mFilters);
        assert(0);
        AutoPtr<IInputMethodManager> imm;// = CInputMethodManager::PeekInstance();
        if (imm != NULL) {
            imm->RestartInput(this);
        }
    }
    else if (type == BufferType_SPANNABLE || mMovement != NULL) {
        AutoPtr<ISpannable> t;
        mSpannableFactory->NewSpannable(text, (ISpannable**)&t);
        text = ICharSequence::Probe(t);
    }
    else {
        assert(0);
        /*CharWrapper* cw = reinterpret_cast<CharWrapper*>(text->Probe(EIID_CharWrapper));
        if (NULL == cw) {
            text = TextUtils::StringOrSpannedString(text);
        }*/
    }

    if (mAutoLinkMask != 0) {
        AutoPtr<ISpannable> s2;
        if (type == BufferType_EDITABLE || ISpannable::Probe(text) != NULL) {
            s2 = ISpannable::Probe(text);
        }
        else {
            mSpannableFactory->NewSpannable(text, (ISpannable**)&s2);
        }

        if (Linkify::AddLinks(s2, mAutoLinkMask)) {
            text = ICharSequence::Probe(s2);
            type = (type == BufferType_EDITABLE) ? BufferType_EDITABLE : BufferType_SPANNABLE;

            mText = text;

             if (mLinksClickable && !TextCanBeSelected()) {
                 AutoPtr<ILinkMovementMethodHelper> helper;
                 CLinkMovementMethodHelper::AcquireSingleton((ILinkMovementMethodHelper**)&helper);
                 AutoPtr<IMovementMethod> method;
                 helper->GetInstance((IMovementMethod**)&method);
                 SetMovementMethod(method);
            }
        }
    }
    mBufferType = type;
    mText = text;

    if (mTransformation == NULL) {
        mTransformed = text;
    }
    else {
        mTransformed = NULL;
        mTransformation->GetTransformation(text, this,
                (ICharSequence**)&mTransformed);
    }

    Int32 textLength;
    text->GetLength(&textLength);

    if (ISpannable::Probe(text) && !mAllowTransformationLengthChange) {
        AutoPtr<ISpannable> sp = ISpannable::Probe(text);

        // Remove any ChangeWatchers that might have come
        // from other TextViews.
        AutoPtr<ArrayOf<IInterface*> > watchers;
        assert(0);
        //sp->GetSpans(0, textLength, EIID_ChangeWatcher, (ArrayOf<IInterface*>**)&watchers);
        Int32 count = watchers->GetLength();
        for (Int32 i = 0; i < count; i++) {
            sp->RemoveSpan((*watchers)[i]);
        }
        watchers = NULL;

        if (mChangeWatcher == NULL) {
            mChangeWatcher = new ChangeWatcher(this);
        }

        sp->SetSpan(mChangeWatcher->Probe(EIID_IInterface), 0, textLength,
            ISpanned::SPAN_INCLUSIVE_INCLUSIVE | (CHANGE_WATCHER_PRIORITY << ISpanned::SPAN_PRIORITY_SHIFT));
        if (mEditor != NULL) mEditor->AddSpanWatchers(sp);

        if (mTransformation != NULL) {
            sp->SetSpan(mTransformation, 0, textLength, ISpanned::SPAN_INCLUSIVE_INCLUSIVE);
        }

        if (mMovement != NULL) {
            mMovement->Initialize(this, ISpannable::Probe(text));
            assert(0);
            //if (mEditor != NULL) mEditor->mSelectionMoved = FALSE;
        }
    }

    if (mLayout != NULL) {
        CheckForRelayout();
    }

    SendOnTextChanged(text, 0, oldlen, textLength);
    OnTextChanged(text, 0, oldlen, textLength);

    NotifyViewAccessibilityStateChangedIfNeeded(IAccessibilityEvent::CONTENT_CHANGE_TYPE_TEXT);

    if (needEditableForNotification) {
        SendAfterTextChanged(IEditable::Probe(text));
    }

    // SelectionModifierCursorController depends on textCanBeSelected, which depends on text
    if (mEditor != NULL) {
        assert(0);
        //mEditor->PrepareCursorControllers();
    }

    return NOERROR;
}

ECode TextView::SetText(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 len)
{
    Int32 oldlen = 0;

    if (start < 0 || len < 0 || start + len > text->GetLength()) {
//        throw new IndexOutOfBoundsException(start + ", " + len);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    /*
     * We must do the before-notification here ourselves because if
     * the old text is a CharWrapper we destroy it before calling
     * into the normal path.
     */
    if (mText != NULL) {
        mText->GetLength(&oldlen);
        SendBeforeTextChanged(mText, 0, oldlen, len);
    }
    else {
        AutoPtr<ICharSequence> temp;
        CString::New(String(""), (ICharSequence**)&temp);
        SendBeforeTextChanged(temp, 0, 0, len);
    }

    /*
        Todo:
        Later we'll find a workaround for CharWrapper!
    */
    /*
    if (mCharWrapper == NULL) {
        mCharWrapper = new CharWrapper(text, start, len);
    }
    else {
        mCharWrapper->Set(text, start, len);
    }

    return SetText((ICharSequence*)mCharWrapper->Probe(EIID_ICharSequence),
            mBufferType, FALSE, oldlen);
    */
    StringBuilder sb;
    sb.Append(*text, start, len);
    AutoPtr<ICharSequence> convertedText = sb.ToCharSequence();
    return SetText(convertedText, mBufferType, FALSE, oldlen);
}

ECode TextView::SetTextKeepState(
    /* [in] */ ICharSequence* text,
    /* [in] */ BufferType type)
{
    VALIDATE_NOT_NULL(text);

    Int32 start, end, len;
    GetSelectionStart(&start);
    GetSelectionEnd(&end);
    text->GetLength(&len);

    FAIL_RETURN(SetText(text, type));

    if (start >= 0 || end >= 0) {
        AutoPtr<ISpannable> span = ISpannable::Probe(mText);
        if (span) {
            Selection::SetSelection(
                    span, Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(start, len)),
                    Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(end, len)));
        }
    }
    return NOERROR;
}

ECode TextView::SetText(
    /* [in] */ Int32 resid)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IResources> resources;
    ctx->GetResources((IResources**)&resources);

    AutoPtr<ICharSequence> csq;
    resources->GetText(resid, (ICharSequence**)&csq);
    return SetText(csq);
}

ECode TextView::SetText(
    /* [in] */ Int32 resid,
    /* [in] */ BufferType type)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IResources> resources;
    ctx->GetResources((IResources**)&resources);

    AutoPtr<ICharSequence> csq;
    resources->GetText(resid, (ICharSequence**)&csq);

    return SetText(csq, type);
}

ECode TextView::SetHint(
    /* [in] */ ICharSequence* hint)
{
    mHint = TextUtils::StringOrSpannedString(hint);

    if (mLayout != NULL) {
        CheckForRelayout();
    }

    Int32 len;
    mText->GetLength(&len);
    if (len == 0) {
        Invalidate();
    }

    // Invalidate display list if hint is currently used
    if (mEditor != NULL && len == 0 && mHint != NULL) {
        assert(0);
        //mEditor->InvalidateTextDisplayList();
    }

    return NOERROR;
}

ECode TextView::SetHint(
    /* [in] */ Int32 resid)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IResources> resources;
    ctx->GetResources((IResources**)&resources);

    AutoPtr<ICharSequence> csq;
    resources->GetText(resid, (ICharSequence**)&csq);

    return SetHint(csq);
}

ECode TextView::GetHint(
    /* [out] */ ICharSequence** hint)
{
    VALIDATE_NOT_NULL(hint)
    *hint = mHint;
    REFCOUNT_ADD(*hint)
    return NOERROR;
}

Boolean TextView::IsSingleLine()
{
    return mSingleLine;
}

Boolean TextView::IsMultilineInputType(
    /* [in] */ Int32 type)
{
    return (type & (IInputType::TYPE_MASK_CLASS | IInputType::TYPE_TEXT_FLAG_MULTI_LINE)) ==
        (IInputType::TYPE_CLASS_TEXT | IInputType::TYPE_TEXT_FLAG_MULTI_LINE);
}

AutoPtr<ICharSequence> TextView::RemoveSuggestionSpans(
    /* [in] */ ICharSequence* text)
{
    AutoPtr<ICharSequence> retText;
    if (ISpanned::Probe(text)) {
       AutoPtr<ISpannable> spannable;
       if (ISpannable::Probe(text)) {
           spannable = ISpannable::Probe(text);
           retText = ICharSequence::Probe(spannable);
       } else {
            CSpannableString::New(text, (ISpannableString**)&spannable);
            retText = ICharSequence::Probe(spannable);
       }

       Int32 length(0);
       retText->GetLength(&length);
       AutoPtr<ArrayOf<IInterface*> > spans;
       ISpanned::Probe(spannable)->GetSpans(0, length, EIID_ISuggestionSpan, (ArrayOf<IInterface*>**)&spans);
       for (Int32 i = 0; i < spans->GetLength(); i++) {
           spannable->RemoveSpan((*spans)[i]);
       }
   }
   else {
       retText = text;
   }

   return retText;
}

ECode TextView::SetInputType(
    /* [in] */ Int32 type)
{
    Int32 inputType;
    GetInputType(&inputType);
    const Boolean wasPassword = IsPasswordInputType(inputType);
    const Boolean wasVisiblePassword = IsVisiblePasswordInputType(inputType);
    SetInputType(type, FALSE);
    const Boolean isPassword = IsPasswordInputType(type);
    const Boolean isVisiblePassword = IsVisiblePasswordInputType(type);
    Boolean forceUpdate = FALSE;

    AutoPtr<IPasswordTransformationMethodHelper> helper;
    CPasswordTransformationMethodHelper::AcquireSingleton((IPasswordTransformationMethodHelper**)&helper);
    AutoPtr<IPasswordTransformationMethod> ptm;
    helper->GetInstance((IPasswordTransformationMethod**)&ptm);
    String nullStr;
    if (isPassword) {
         SetTransformationMethod(ITransformationMethod::Probe(ptm));
         SetTypefaceFromAttrs(nullStr /* fontFamily */, MONOSPACE, 0);
    }
    else if (isVisiblePassword) {
        if (mTransformation.Get() == ITransformationMethod::Probe(ptm)) {
            forceUpdate = TRUE;
        }
        SetTypefaceFromAttrs(nullStr /* fontFamily */, MONOSPACE, 0);
    }
    else if (wasPassword || wasVisiblePassword) {
        // not in password mode, clean up typeface and transformation
        SetTypefaceFromAttrs(nullStr /* fontFamily */, -1, -1);
        if (mTransformation.Get() == ITransformationMethod::Probe(ptm)) {
            forceUpdate = TRUE;
        }
    }

    Boolean singleLine = !IsMultilineInputType(type);

    // We need to update the single line mode if it has changed or we
    // were previously in password mode.
    if (mSingleLine != singleLine || forceUpdate) {
       // Change single line mode, but only change the transformation if
       // we are not in password mode.
       ApplySingleLine(singleLine, !isPassword, TRUE);
    }
    Boolean isSuggestionsEnabled;
    if (IsSuggestionsEnabled(&isSuggestionsEnabled), isSuggestionsEnabled) {
        mText = RemoveSuggestionSpans(mText);
    }

    assert(0);
    AutoPtr<IInputMethodManager> imm;// = CInputMethodManager::PeekInstance();
    if (imm != NULL) {
        imm->RestartInput(this);
    }
    return NOERROR;
}

Boolean TextView::HasPasswordTransformationMethod()
{
    if (!mTransformation)
        return FALSE;
    return (IPasswordTransformationMethod::Probe(mTransformation) != NULL);
}

Boolean TextView::IsPasswordInputType(
    /* [in] */ Int32 inputType)
{
    Int32 variation =
            inputType & (IInputType::TYPE_MASK_CLASS | IInputType::TYPE_MASK_VARIATION);
    return variation
            == (IInputType::TYPE_CLASS_TEXT | IInputType::TYPE_TEXT_VARIATION_PASSWORD)
            || variation
            == (IInputType::TYPE_CLASS_TEXT | IInputType::TYPE_TEXT_VARIATION_WEB_PASSWORD)
            || variation
            == (IInputType::TYPE_CLASS_NUMBER | IInputType::TYPE_NUMBER_VARIATION_PASSWORD);
}

Boolean TextView::IsVisiblePasswordInputType(
    /* [in] */ Int32 inputType)
{
    Int32 variation = inputType & (IInputType::TYPE_MASK_CLASS
            | IInputType::TYPE_MASK_VARIATION);
    return variation
            == (IInputType::TYPE_CLASS_TEXT
                    | IInputType::TYPE_TEXT_VARIATION_VISIBLE_PASSWORD);
}

ECode TextView::SetRawInputType(
    /* [in] */ Int32 type)
{
    if (type == IInputType::TYPE_NULL && mEditor == NULL) return NOERROR; //TYPE_NULL is the default value
    CreateEditorIfNeeded();
    assert(0);
    //mEditor->mInputType = type;
    return NOERROR;
}

void TextView::SetInputType(
    /* [in] */ Int32 type,
    /* [in] */ Boolean direct)
{
    Int32 cls = type & IInputType::TYPE_MASK_CLASS;
    AutoPtr<IKeyListener> input;
    if (cls == IInputType::TYPE_CLASS_TEXT) {
        Boolean autotext = (type & IInputType::TYPE_TEXT_FLAG_AUTO_CORRECT) != 0;
        Elastos::Droid::Text::Method::Capitalize cap;
        if ((type & IInputType::TYPE_TEXT_FLAG_CAP_CHARACTERS) != 0) {
            cap = Elastos::Droid::Text::Method::Capitalize_CHARACTERS;
        }
        else if ((type & IInputType::TYPE_TEXT_FLAG_CAP_WORDS) != 0) {
            cap = Elastos::Droid::Text::Method::Capitalize_WORDS;
        }
        else if ((type & IInputType::TYPE_TEXT_FLAG_CAP_SENTENCES) != 0) {
            cap = Elastos::Droid::Text::Method::Capitalize_SENTENCES;
        }
        else {
            cap = Elastos::Droid::Text::Method::Capitalize_NONE;
        }

        AutoPtr<ITextKeyListenerHelper> helper;
        CTextKeyListenerHelper::AcquireSingleton((ITextKeyListenerHelper**)&helper);
        helper->GetInstance(autotext, cap, (ITextKeyListener**)&input);
    }
    else if (cls == IInputType::TYPE_CLASS_NUMBER) {
        AutoPtr<IDigitsKeyListenerHelper> helper;
        CDigitsKeyListenerHelper::AcquireSingleton((IDigitsKeyListenerHelper**)&helper);
        helper->GetInstance(
                (type & IInputType::TYPE_NUMBER_FLAG_SIGNED) != 0,
                (type & IInputType::TYPE_NUMBER_FLAG_DECIMAL) != 0,
                (IDigitsKeyListener**)&input);
    }
    else if (cls == IInputType::TYPE_CLASS_DATETIME) {
        Int32 flag = type & IInputType::TYPE_MASK_VARIATION;
        if (flag == IInputType::TYPE_DATETIME_VARIATION_DATE) {
            AutoPtr<IDateKeyListenerHelper> helper;
            CDateKeyListenerHelper::AcquireSingleton((IDateKeyListenerHelper**)&helper);
            helper->GetInstance((IDateKeyListener**)&input);
        }
        else if (flag == IInputType::TYPE_DATETIME_VARIATION_TIME) {
            AutoPtr<ITimeKeyListenerHelper> helper;
            CTimeKeyListenerHelper::AcquireSingleton((ITimeKeyListenerHelper**)&helper);
            helper->GetInstance((ITimeKeyListener**)&input);
        }
        else {
            AutoPtr<IDateTimeKeyListenerHelper> helper;
            CDateTimeKeyListenerHelper::AcquireSingleton((IDateTimeKeyListenerHelper**)&helper);
            helper->GetInstance((IDateTimeKeyListener**)&input);
        }
    }
    else if (cls == IInputType::TYPE_CLASS_PHONE) {
        AutoPtr<IDialerKeyListenerHelper> helper;
        CDialerKeyListenerHelper::AcquireSingleton((IDialerKeyListenerHelper**)&helper);
        helper->GetInstance((IDialerKeyListener**)&input);
    }
    else {
        AutoPtr<ITextKeyListenerHelper> helper;
        CTextKeyListenerHelper::AcquireSingleton((ITextKeyListenerHelper**)&helper);
        helper->GetInstance((ITextKeyListener**)&input);
    }
    SetRawInputType(type);
    if (direct)
    {
        CreateEditorIfNeeded();
        assert(0);
        //mEditor->mKeyListener = input;
    }
    else {
        SetKeyListenerOnly(input);
    }
}

ECode TextView::GetInputType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    if (mEditor == NULL) {
        *type = IInputType::TYPE_NULL;
    } else {
        assert(0);
        //*type = mEditor->mInputType;
    }
    return NOERROR;
}

ECode TextView::SetImeOptions(
    /* [in] */ Int32 imeOptions)
{
    CreateEditorIfNeeded();
    assert(0);
    /*mEditor->CreateInputContentTypeIfNeeded();
    mEditor->mInputContentType->mImeOptions = imeOptions;*/
    return NOERROR;
}

ECode TextView::GetImeOptions(
    /* [out] */ Int32* options)
{
    VALIDATE_NOT_NULL(options)
    assert(0);
    /*if (mEditor != NULL && mEditor->mInputContentType != NULL) {
        *options = mEditor->mInputContentType->mImeOptions;
    } else {
        *options = IEditorInfo::IME_NULL;
    }*/

    return NOERROR;
}

ECode TextView::SetImeActionLabel(
    /* [in] */ ICharSequence* label,
    /* [in] */ Int32 actionId)
{
    CreateEditorIfNeeded();
    assert(0);
    /*mEditor->CreateInputContentTypeIfNeeded();
    mEditor->mInputContentType->mImeActionLabel = label;
    mEditor->mInputContentType->mImeActionId = actionId;*/
    return NOERROR;
}

ECode TextView::GetImeActionLabel(
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label)
    assert(0);
   /* if (mEditor != NULL && mEditor->mInputContentType != NULL) {
        *label = mEditor->mInputContentType->mImeActionLabel;
        REFCOUNT_ADD(*label)
    } else {
        *label = NULL
    }*/
    return NOERROR;
}

ECode TextView::GetImeActionId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    assert(0);
    /*if (mEditor != NULL && mEditor->mInputContentType != NULL) {
        *id = mEditor->mInputContentType->mImeActionId;
    } else {
        *id = 0;
    }*/
    return NOERROR;
}

ECode TextView::SetOnEditorActionListener(
    /* [in] */ IOnEditorActionListener* l)
{
    assert(0);
    CreateEditorIfNeeded();
    /*mEditor->CreateInputContentTypeIfNeeded();
    mEditor->mInputContentType->mOnEditorActionListener = l;*/
    return NOERROR;
}

ECode TextView::OnEditorAction(
    /* [in] */ Int32 actionCode)
{
    assert(0);
    /*InputContentType* ict = mEditor == NULL ? NULL :mEditor->mInputContentType.Get();
    if (ict != NULL) {
        if (ict->mOnEditorActionListener != NULL) {
            Boolean state = FALSE;
            ict->mOnEditorActionListener->OnEditorAction(
                 this, actionCode, NULL, &state);
            if (state) {
                return NOERROR;
            }
        }

        // This is the handling for some default action.
        // Note that for backwards compatibility we don't do this
        // default handling if explicit ime options have not been given,
        // instead turning this into the normal enter key codes that an
        // app may be expecting.
        if (actionCode == IEditorInfo::IME_ACTION_NEXT) {
            AutoPtr<IView> v = FocusSearch(IView::FOCUS_FORWARD);
            if (v != NULL) {
                Boolean focus = FALSE;
                if (v->RequestFocus(IView::FOCUS_FORWARD, &focus), !focus) {
                    // throw new IllegalStateException("focus search returned a view " +
                    //         "that wasn't able to take focus!");
                    return E_ILLEGAL_STATE_EXCEPTION;
                }
            }
            return NOERROR;
        }
        else if (actionCode == IEditorInfo::IME_ACTION_PREVIOUS) {
            AutoPtr<IView> v = FocusSearch(IView::FOCUS_BACKWARD);
            if (v != NULL) {
                Boolean focus = FALSE;
                if (v->RequestFocus(IView::FOCUS_BACKWARD, &focus), !focus) {
                    throw new IllegalStateException("focus search returned a view " +
                        "that wasn't able to take focus!");
                }
            }
            return NOERROR;
        }
        else if (actionCode == IEditorInfo::IME_ACTION_DONE) {
            assert(0);
            AutoPtr<IInputMethodManager> imm;// = CInputMethodManager::PeekInstance();
            if (imm != NULL) {
                Boolean active;
                imm->IsActive(this, &active);
                if (active) {
                    Boolean ret;
                    imm->HideSoftInputFromWindow(GetWindowToken(), 0, &ret);
                }
            }
            return NOERROR;
        }
    }

    AutoPtr<ViewRootImpl> viewRootImpl = GetViewRootImpl();
    if (viewRootImpl != NULL) {
        Int64 eventTime = SystemClock::GetUptimeMillis();

        AutoPtr<IKeyEvent> downKV, upKV;
        CKeyEvent::New(
                eventTime, eventTime,
                IKeyEvent::ACTION_DOWN, IKeyEvent::KEYCODE_ENTER, 0, 0,
                IKeyCharacterMap::VIRTUAL_KEYBOARD, 0,
                IKeyEvent::FLAG_SOFT_KEYBOARD | IKeyEvent::FLAG_KEEP_TOUCH_MODE
                | IKeyEvent::FLAG_EDITOR_ACTION, (IKeyEvent**)&downKV);
        viewRootImpl->DispatchKeyFromIme(downKV);

        Int64 newEventTime = SystemClock::GetUptimeMillis();
        CKeyEvent::New(
                newEventTime, eventTime,
                IKeyEvent::ACTION_UP, IKeyEvent::KEYCODE_ENTER, 0, 0,
                IKeyCharacterMap::VIRTUAL_KEYBOARD, 0,
                IKeyEvent::FLAG_SOFT_KEYBOARD | IKeyEvent::FLAG_KEEP_TOUCH_MODE
                | IKeyEvent::FLAG_EDITOR_ACTION, (IKeyEvent**)&upKV);
        viewRootImpl->DispatchKeyFromIme(upKV);
    }*/

    return NOERROR;
}

ECode TextView::SetPrivateImeOptions(
    /* [in] */ const String& type)
{
    CreateEditorIfNeeded();
    assert(0);
    /*mEditor->CreateInputContentTypeIfNeeded();
    mEditor->mInputContentType->mPrivateImeOptions = type;*/
    return NOERROR;
}

ECode TextView::GetPrivateImeOptions(
    /* [out] */ String* options)
{
    VALIDATE_NOT_NULL(options)
    assert(0);
    /*if(mEditor != NULL && mEditor->mInputContentType != NULL) {
        *options = mEditor->mInputContentType->mPrivateImeOptions;
    } else {
        *options = String("");
    }*/
    return NOERROR;
}

ECode TextView::SetInputExtras(
    /* [in] */ Int32 xmlResId)
{
    CreateEditorIfNeeded();
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IXmlResourceParser> parser;
    res->GetXml(xmlResId, (IXmlResourceParser**)&parser);
    assert(0);
    /*mEditor->CreateInputContentTypeIfNeeded();
    mEditor->mInputContentType->mExtras = NULL;
    CBundle::New((IBundle**)&mEditor->mInputContentType->mExtras);
    return res->ParseBundleExtras(parser, mEditor->mInputContentType->mExtras);*/
    return NOERROR;
}

ECode TextView::GetInputExtras(
    /* [in] */ Boolean create,
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    assert(0);
    /*if (mEditor == NULL && !create) {
        *bundle = NULL;
        return NOERROR;
    }
    CreateEditorIfNeeded();

    if (mEditor->mInputContentType == NULL) {
        if (!create) {
            *bundle = NULL;
            return NOERROR;
        }
        mEditor->CreateInputContentTypeIfNeeded();
    }
    if (mEditor->mInputContentType->mExtras == NULL) {
        if (!create) {
            *bundle = NULL;
            return NOERROR;
        }
        mEditor->mInputContentType->mExtras = NULL;
        CBundle::New((IBundle**)&mEditor->mInputContentType->mExtras);
    }
    *bundle = mEditor->mInputContentType->mExtras;
    REFCOUNT_ADD(*bundle)*/
    return NOERROR;
}

ECode TextView::GetError(
    /* [out] */ ICharSequence** error)
{
    VALIDATE_NOT_NULL(error)
    if(mEditor == NULL) {
        *error = NULL;
    } else {
        assert(0);
        //*error = mEditor->mError;
        REFCOUNT_ADD(*error)
    }
    return NOERROR;
}

ECode TextView::SetError(
    /* [in] */ ICharSequence* error)
{
    if (error == NULL) {
        return SetError(NULL, NULL);
    }
    else {

        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<IDrawable> dr;
        ctx->GetDrawable(R::drawable::indicator_input_error, (IDrawable**)&dr);

        Int32 width, height;
        dr->GetIntrinsicWidth(&width);
        dr->GetIntrinsicHeight(&height);
        dr->SetBounds(0, 0, width, height);
        return SetError(error, dr);
    }
    return NOERROR;
}

ECode TextView::SetError(
    /* [in] */ ICharSequence* error,
    /* [in] */ IDrawable* icon)
{
    CreateEditorIfNeeded();
    mEditor->SetError(error, icon);
    NotifyViewAccessibilityStateChangedIfNeeded(
            IAccessibilityEvent::CONTENT_CHANGE_TYPE_UNDEFINED);
    return NOERROR;
}

Boolean TextView::SetFrame(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Boolean result = View::SetFrame(l, t, r, b);
    assert(0);
    //if (mEditor != NULL) mEditor->SetFrame();
    RestartMarqueeIfNeeded();
    return result;
}

void TextView::RestartMarqueeIfNeeded()
{
    if (mRestartMarquee && mEllipsize == TextUtilsTruncateAt_MARQUEE) {
        mRestartMarquee = FALSE;
        StartMarquee();
    }
}

ECode TextView::SetFilters(
    /* [in] */ ArrayOf<Elastos::Droid::Text::IInputFilter*>* filters)
{
    if (filters == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mFilters = filters;

    if (mText) {
        AutoPtr<IEditable> editable = IEditable::Probe(mText);
        if (editable != NULL) {
            return SetFilters(editable, filters);
        }
    }
    return NOERROR;
}

ECode TextView::SetFilters(
    /* [in] */ IEditable* e,
    /* [in] */ ArrayOf<Elastos::Droid::Text::IInputFilter*>* filters)
{
    assert(0);
    /*if (mEditor) {
        Boolean undoFilter = mEditor->mUndoInputFilter != NULL;
        Boolean keyFilter = FALSE;
        if (mEditor->mKeyListener->Probe(EIID_IInputFilter)) {
            keyFilter = TRUE;
        }
        Int32 num = 0;
        if (undoFilter) num++;
        if (keyFilter) num++;
        if (num > 0) {
            Int32 length = filters->GetLength() + num;
            AutoPtr<ArrayOf<IInputFilter*> > newFilters = ArrayOf<IInputFilter*>::Alloc(length);
            newFilters->Copy(filters);
            num = 0;
            if (undoFilter) {
                newFilters->Set(filters->GetLength(), mEditor->mUndoInputFilter);
                num++;
            }

            if (keyFilter) {
                newFilters->Set(filters->GetLength() + num, (IInputFilter*)mEditor->mKeyListener->Probe(EIID_IInputFilter));
            }

            e->SetFilters(newFilters);
            return NOERROR;
        }
    }*/
    e->SetFilters(filters);

    return NOERROR;
}

ECode TextView::GetFilters(
    /* [out, callee] */ ArrayOf<Elastos::Droid::Text::IInputFilter*>** filters)
{
    VALIDATE_NOT_NULL(filters)
    *filters = mFilters;
    REFCOUNT_ADD(*filters);
    return NOERROR;
}


Int32 TextView::GetBoxHeight(
    /* [in] */ ILayout* l)
{
    AutoPtr<IInsets> opticalInsets;
    if (IsLayoutModeOptical(mParent)) {
        GetOpticalInsets((IInsets**)&opticalInsets);
    } else {
        opticalInsets = Elastos::Droid::Graphics::Insets::NONE;
    }
    Int32 padding;
    if (l == mHintLayout) {
        GetCompoundPaddingTop(&padding);
        Int32 bottom;
        GetCompoundPaddingBottom(&bottom);
        padding += bottom;
    } else {
        Int32 extendedPaddingTop, extendedPaddingBottom;
        GetExtendedPaddingTop(&extendedPaddingTop);
        GetExtendedPaddingBottom(&extendedPaddingBottom);
        padding = extendedPaddingTop + extendedPaddingBottom;
    }

    Int32 top, bottom;
    opticalInsets->GetTop(&top);
    opticalInsets->GetBottom(&bottom);
    Int32 height;
    GetMeasuredHeight(&height);
    return height - padding + top + bottom;
}

Int32 TextView::GetVerticalOffset(
    /* [in] */ Boolean forceNormal)
{
    Int32 voffset = 0;
    Int32 gravity = mGravity & IGravity::VERTICAL_GRAVITY_MASK;

    Int32 len;
    mText->GetLength(&len);
    AutoPtr<ILayout> l = mLayout;
    if (!forceNormal && len == 0 && mHintLayout != NULL) {
        l = mHintLayout;
    }

    if (gravity != IGravity::TOP) {

        Int32 boxht = GetBoxHeight(l);

        Int32 textht;
        l->GetHeight(&textht);

        if (textht < boxht) {
            if (gravity == IGravity::BOTTOM) {
                voffset = boxht - textht;
            }
            else { // (gravity == IGravity::CENTER_VERTICAL)
                voffset = (boxht - textht) >> 1;
            }
        }
    }
    return voffset;
}

Int32 TextView::GetBottomVerticalOffset(
    /* [in] */ Boolean forceNormal)
{
    Int32 voffset = 0;
    Int32 gravity = mGravity & IGravity::VERTICAL_GRAVITY_MASK;

    Int32 len;
    mText->GetLength(&len);
    AutoPtr<ILayout> l = mLayout;
    if (!forceNormal && len == 0 && mHintLayout != NULL) {
        l = mHintLayout;
    }

    if (gravity != IGravity::BOTTOM) {

        Int32 boxht = GetBoxHeight(l);
        Int32 textht;
        l->GetHeight(&textht);

        if (textht < boxht) {
            if (gravity == IGravity::TOP) {
                voffset = boxht - textht;
            }
            else { // (gravity == IGravity::CENTER_VERTICAL)
                voffset = (boxht - textht) >> 1;
            }
        }
    }
    return voffset;
}

void TextView::InvalidateCursorPath()
{
    if (mHighlightPathBogus) {
        InvalidateCursor();
    }
    else {
        Int32 horizontalPadding, extendedPaddingTop;
        GetCompoundPaddingLeft(&horizontalPadding);
        GetExtendedPaddingTop(&extendedPaddingTop);
        Int32 verticalPadding = extendedPaddingTop + GetVerticalOffset(TRUE);
        assert(0);
        if (0/*mEditor->mCursorCount == 0*/) {
            AutoLock lock(sTempRectLock);

            /*
                * The reason for this concern about the thickness of the
                * cursor and doing the floor/ceil on the coordinates is that
                * some EditTexts (notably textfields in the Browser) have
                * anti-aliased text where not all the characters are
                * necessarily at integer-multiple locations.  This should
                * make sure the entire cursor gets invalidated instead of
                * sometimes missing half a pixel.
                */

            Float thick;
            IPaint::Probe(mTextPaint)->GetStrokeWidth(&thick);
            thick = Elastos::Core::Math::Ceil(thick);
            if (thick < 1.0f) {
                thick = 1.0f;
            }

            thick /= 2;

            mHighlightPath->ComputeBounds(TEMP_RECTF, FALSE);

            Invalidate((Int32)Elastos::Core::Math::Floor(horizontalPadding + ((CRectF*)TEMP_RECTF.Get())->mLeft - thick),
                        (Int32)Elastos::Core::Math::Floor(verticalPadding + ((CRectF*)TEMP_RECTF.Get())->mTop - thick),
                        (Int32)Elastos::Core::Math::Ceil(horizontalPadding + ((CRectF*)TEMP_RECTF.Get())->mRight + thick),
                        (Int32)Elastos::Core::Math::Ceil(verticalPadding + ((CRectF*)TEMP_RECTF.Get())->mBottom + thick));
        }
        else {
            assert(0);
            /*Int32 l, t, r, b;
            AutoPtr<IDrawable> drawable;
            for (Int32 i = 0; i < mEditor->mCursorCount; i++) {
                AutoPtr<IRect> bounds;
                drawable = (*mEditor->mCursorDrawable)[i];
                drawable->GetBounds((IRect**)&bounds);
                bounds->Get(&l, &t, &r, &b);
                Invalidate(l + horizontalPadding, t + verticalPadding,
                        r + horizontalPadding, b + verticalPadding);
            }*/
        }
    }
}

void TextView::InvalidateCursor()
{
    Int32 where;
    GetSelectionEnd(&where);

    InvalidateCursor(where, where, where);
}

void TextView::InvalidateCursor(
    /* [in] */ Int32 a,
    /* [in] */ Int32 b,
    /* [in] */ Int32 c)
{
    if (a >= 0 || b >= 0 || c >= 0) {
        Int32 start = Elastos::Core::Math::Min(Elastos::Core::Math::Min(a, b), c);
        Int32 end = Elastos::Core::Math::Max(Elastos::Core::Math::Max(a, b), c);
        InvalidateRegion(start, end, TRUE /* Also invalidates blinking cursor */);
    }
}

ECode TextView::InvalidateRegion(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Boolean invalidateCursor)
{
    if (mLayout == NULL) {
        Invalidate();
    }
    else {
        Int32 lineStart;
        mLayout->GetLineForOffset(start, &lineStart);
        Int32 top;
        mLayout->GetLineTop(lineStart, &top);

        // This is ridiculous, but the descent from the line above
        // can hang down into the line we really want to redraw,
        // so we have to invalidate part of the line above to make
        // sure everything that needs to be redrawn really is.
        // (But not the whole line above, because that would cause
        // the same problem with the descenders on the line above it!)
        if (lineStart > 0) {
            Int32 descent;
            mLayout->GetLineDescent(lineStart - 1, &descent);
            top -= descent;
        }

        Int32 lineEnd;

        if (start == end)
            lineEnd = lineStart;
        else
            mLayout->GetLineForOffset(end, &lineEnd);

        Int32 bottom;

        mLayout->GetLineBottom(lineEnd, &bottom);

        // mEditor can be NULL in case selection is set programmatically.
        if (invalidateCursor && mEditor != NULL) {
            assert(0);
            /*Int32 t, b;
            for (Int32 i = 0; i < mEditor->mCursorCount; i++) {
                AutoPtr<IRect> bounds;
                (*mEditor->mCursorDrawable)[i]->GetBounds((IRect**)&bounds);
                bounds->GetTop(&t);
                bounds->GetBottom(&b);
                top = Elastos::Core::Math::Min(top, t);
                bottom = Elastos::Core::Math::Max(bottom, b);
            }*/
        }

        Int32 compoundPaddingLeft, extendedPaddingTop;
        GetCompoundPaddingLeft(&compoundPaddingLeft);
        GetExtendedPaddingTop(&extendedPaddingTop);
        Int32 verticalPadding = extendedPaddingTop + GetVerticalOffset(TRUE);

        Float left, right;
        if (lineStart == lineEnd && !invalidateCursor) {
            mLayout->GetPrimaryHorizontal(start, &left);
            mLayout->GetPrimaryHorizontal(1 + end, &right);
            left += compoundPaddingLeft;
            right += compoundPaddingLeft;
        } else {
            // Rectangle bounding box when the region spans several lines
            left = compoundPaddingLeft;
            Int32 width, compoundPaddingRight;
            GetWidth(&width);
            GetCompoundPaddingRight(&compoundPaddingRight);
            right = width - compoundPaddingRight;
        }

        Invalidate(mScrollX + left, verticalPadding + top,
            mScrollX + right, verticalPadding + bottom);
    }
    return NOERROR;
}

void TextView::RegisterForPreDraw()
{
    if (!mPreDrawRegistered) {
        AutoPtr<IViewTreeObserver> observer;
        GetViewTreeObserver((IViewTreeObserver**)&observer);
        IOnPreDrawListener* listener = this;
        assert(listener != NULL);
        observer->AddOnPreDrawListener(listener);
        mPreDrawRegistered = TRUE;
    }
}

void TextView::UnregisterForPreDraw()
{
    AutoPtr<IViewTreeObserver> observer;
    GetViewTreeObserver((IViewTreeObserver**)&observer);
    observer->RemoveOnPreDrawListener(this);
    mPreDrawRegistered = FALSE;
    mPreDrawListenerDetached = FALSE;
}

ECode TextView::OnPreDraw(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (mLayout == NULL) {
        AssumeLayout();
    }

    if (mMovement != NULL) {
        /* This code also provides auto-scrolling when a cursor is moved using a
         * CursorController (insertion point or selection limits).
         * For selection, ensure start or end is visible depending on controller's state.
         */
        Int32 curs;
        GetSelectionEnd(&curs);
        // Do not create the controller if it is not already created.
        assert(0);
        /*if (mEditor != NULL && mEditor->mSelectionModifierCursorController != NULL &&
            mEditor->mSelectionModifierCursorController->IsSelectionStartDragged()) {
                curs = GetSelectionStart();
        }*/

        /*
         * TODO: This should really only keep the end in view if
         * it already was before the text changed.  I'm not sure
         * of a good way to tell from here if it was.
         */
        if (curs < 0 && (mGravity & IGravity::VERTICAL_GRAVITY_MASK) == IGravity::BOTTOM) {
            mText->GetLength(&curs);
        }

        if (curs >= 0) {
            Boolean bringPointIntoView;
            BringPointIntoView(curs, &bringPointIntoView);
        }
    }
    else {
        BringTextIntoView();
    }

    // This has to be checked here since:
    // - onFocusChanged cannot start it when focus is given to a view with selected text (after
    //   a screen rotation) since layout is not yet initialized at that point.
    // - ExtractEditText does not call onFocus when it is displayed. Fixing this issue would
    //   allow to test for hasSelection in onFocusChanged, which would trigger a
    //   startTextSelectionMode here. TODO
    assert(0);
    /*if (mEditor != NULL && mEditor->mCreatedWithASelection) {
        mEditor->StartSelectionActionMode();
        mEditor->mCreatedWithASelection = FALSE;
    }*/

    // Phone specific code (there is no ExtractEditText on tablets).
    // ExtractEditText does not call onFocus when it is displayed, and mHasSelectionOnFocus can
    // not be set. Do the test here instead.
    Boolean hasSelection;
    if (this->Probe(EIID_IExtractEditText) != NULL
        && (HasSelection(&hasSelection), hasSelection) && mEditor != NULL) {
        assert(0);
        //mEditor->StartSelectionActionMode();
    }

    UnregisterForPreDraw();
    *res = TRUE;
    return NOERROR;
}

ECode TextView::OnAttachedToWindow()
{
    View::OnAttachedToWindow();

    mTemporaryDetach = FALSE;

    assert(0);
    //if (mEditor != NULL) mEditor->OnAttachedToWindow();

    if (mPreDrawListenerDetached) {
        AutoPtr<IViewTreeObserver> observer;
        GetViewTreeObserver((IViewTreeObserver**)&observer);
        observer->AddOnPreDrawListener(this);
        mPreDrawListenerDetached = FALSE;
    }
    return NOERROR;
}

ECode TextView::OnDetachedFromWindowInternal()
{

    if (mPreDrawRegistered) {
        AutoPtr<IViewTreeObserver> observer;
        GetViewTreeObserver((IViewTreeObserver**)&observer);
        observer->RemoveOnPreDrawListener(this);
        mPreDrawListenerDetached = TRUE;
    }

    ResetResolvedDrawables();

    assert(0);
    //if (mEditor != NULL) mEditor->OnDetachedFromWindow();

    View::OnDetachedFromWindowInternal();
    return NOERROR;
}

ECode TextView::OnScreenStateChanged(
    /* [in] */ Int32 screenState)
{
    View::OnScreenStateChanged(screenState);
    assert(0);
    //if (mEditor != NULL) mEditor->OnScreenStateChanged(screenState);
    return NOERROR;
}

Boolean TextView::IsPaddingOffsetRequired()
{
    return mShadowRadius != 0 || mDrawables != NULL;
}

Int32 TextView::GetLeftPaddingOffset()
{
    Int32 compoundPaddingLeft;
    GetCompoundPaddingLeft(&compoundPaddingLeft);
    return compoundPaddingLeft - mPaddingLeft +
            (Int32)Elastos::Core::Math::Min(0.f, mShadowDx - mShadowRadius);
}

Int32 TextView::GetTopPaddingOffset()
{
    return (Int32)Elastos::Core::Math::Min(0.f, mShadowDy - mShadowRadius);
}

Int32 TextView::GetBottomPaddingOffset()
{
    return (Int32)Elastos::Core::Math::Max(0.f, mShadowDy + mShadowRadius);
}

Int32 TextView::GetRightPaddingOffset()
{
    Int32 compoundPaddingRight;
    GetCompoundPaddingRight(&compoundPaddingRight);
    return -(compoundPaddingRight - mPaddingRight) +
            (Int32)Elastos::Core::Math::Max(0.f, mShadowDx + mShadowRadius);
}

Boolean TextView::VerifyDrawable(
    /* [in] */ IDrawable* who)
{
    Boolean verified = View::VerifyDrawable(who);
    if (!verified && mDrawables != NULL) {
        return who == mDrawables->mDrawableLeft || who == mDrawables->mDrawableTop ||
                who == mDrawables->mDrawableRight || who == mDrawables->mDrawableBottom ||
                who == mDrawables->mDrawableStart || who == mDrawables->mDrawableEnd;
    }
    return verified;
}

ECode TextView::JumpDrawablesToCurrentState()
{
    View::JumpDrawablesToCurrentState();
    if (mDrawables != NULL) {
        if (mDrawables->mDrawableLeft != NULL) {
            mDrawables->mDrawableLeft->JumpToCurrentState();
        }
        if (mDrawables->mDrawableTop != NULL) {
            mDrawables->mDrawableTop->JumpToCurrentState();
        }
        if (mDrawables->mDrawableRight != NULL) {
            mDrawables->mDrawableRight->JumpToCurrentState();
        }
        if (mDrawables->mDrawableBottom != NULL) {
            mDrawables->mDrawableBottom->JumpToCurrentState();
        }
        if (mDrawables->mDrawableStart != NULL) {
            mDrawables->mDrawableStart->JumpToCurrentState();
        }
        if (mDrawables->mDrawableEnd != NULL) {
            mDrawables->mDrawableEnd->JumpToCurrentState();
        }
    }
    return NOERROR;
}

ECode TextView::InvalidateDrawable(
    /* [in] */ IDrawable* drawable)
{

    Boolean handled = FALSE;
    if (VerifyDrawable(drawable)) {
        AutoPtr<IRect> dirty;
        drawable->GetBounds((IRect**)&dirty);

        Int32 scrollX = mScrollX;
        Int32 scrollY = mScrollY;

        // IMPORTANT: The coordinates below are based on the coordinates computed
        // for each compound drawable in onDraw(). Make sure to update each section
        // accordingly.
        Drawables* drawables = mDrawables;
        if (drawables != NULL) {
            if (drawable == drawables->mDrawableLeft) {
                Int32 compoundPaddingTop, compoundPaddingBottom;
                GetCompoundPaddingTop(&compoundPaddingTop);
                GetCompoundPaddingBottom(&compoundPaddingBottom);
                Int32 vspace = mBottom - mTop - compoundPaddingBottom - compoundPaddingTop;

                scrollX += mPaddingLeft;
                scrollY += compoundPaddingTop + (vspace - drawables->mDrawableHeightLeft) / 2;
                handled = TRUE;
            }
            else if (drawable == drawables->mDrawableRight) {
                Int32 compoundPaddingTop, compoundPaddingBottom;
                GetCompoundPaddingTop(&compoundPaddingTop);
                GetCompoundPaddingBottom(&compoundPaddingBottom);
                Int32 vspace = mBottom - mTop - compoundPaddingBottom - compoundPaddingTop;

                scrollX += (mRight - mLeft - mPaddingRight - drawables->mDrawableSizeRight);
                scrollY += compoundPaddingTop + (vspace - drawables->mDrawableHeightRight) / 2;
                handled = TRUE;
            }
            else if (drawable == drawables->mDrawableTop) {
                Int32 compoundPaddingLeft, compoundPaddingRight;
                GetCompoundPaddingLeft(&compoundPaddingLeft);
                GetCompoundPaddingRight(&compoundPaddingRight);
                Int32 hspace = mRight - mLeft - compoundPaddingRight - compoundPaddingLeft;

                scrollX += compoundPaddingLeft + (hspace - drawables->mDrawableWidthTop) / 2;
                scrollY += mPaddingTop;
                handled = TRUE;
            }
            else if (drawable == drawables->mDrawableBottom) {
                Int32 compoundPaddingLeft, compoundPaddingRight;
                GetCompoundPaddingLeft(&compoundPaddingLeft);
                GetCompoundPaddingRight(&compoundPaddingRight);
                Int32 hspace = mRight - mLeft - compoundPaddingRight - compoundPaddingLeft;

                scrollX += compoundPaddingLeft + (hspace - drawables->mDrawableWidthBottom) / 2;
                scrollY += (mBottom - mTop - mPaddingBottom - drawables->mDrawableSizeBottom);
                handled = TRUE;
            }
        }

        if (handled) {
            CRect* _dirty = (CRect*)dirty.Get();
            Invalidate(_dirty->mLeft + scrollX, _dirty->mTop + scrollY,
                _dirty->mRight + scrollX, _dirty->mBottom + scrollY);
        }

    }

    if (!handled) View::InvalidateDrawable(drawable);
    return NOERROR;
}

ECode TextView::HasOverlappingRendering(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<IDrawable> background;
    GetBackground((IDrawable**)&background);
    AutoPtr<IDrawable> current;
    background->GetCurrent((IDrawable**)&current);
    Boolean isHorizontalFadingEdgeEnabled, hasSelection;
    *res = ((background && current) || ISpannable::Probe(mText)
        || (HasSelection(&hasSelection), hasSelection)
        || (IsHorizontalFadingEdgeEnabled(&isHorizontalFadingEdgeEnabled), isHorizontalFadingEdgeEnabled));
    return NOERROR;
}

ECode TextView::IsTextSelectable(
    /* [out] */ Boolean* selectable)
{
    VALIDATE_NOT_NULL(selectable)
    assert(0);
    //*selectable = mEditor == NULL ? FALSE : mEditor->mTextIsSelectable;
    return NOERROR;
}

ECode TextView::SetTextIsSelectable(
    /* [in] */ Boolean selectable)
{
    if (!selectable && mEditor == NULL) return NOERROR; // FALSE is default value with no edit data

    CreateEditorIfNeeded();
    assert(0);
    //if (mEditor->mTextIsSelectable == selectable) return NOERROR;

    //mEditor->mTextIsSelectable = selectable;
    SetFocusableInTouchMode(selectable);
    SetFocusable(selectable);
    SetClickable(selectable);
    SetLongClickable(selectable);

    // mInputType should already be IInputType::TYPE_NULL and mInput should be NULL

    AutoPtr<IMovementMethod> method;
    if (selectable) {
        AutoPtr<IArrowKeyMovementMethodHelper> helper;
        CArrowKeyMovementMethodHelper::AcquireSingleton((IArrowKeyMovementMethodHelper**)&helper);
        helper->GetInstance((IMovementMethod**)&method);
    }
    SetMovementMethod(method);
    SetText(mText, selectable ? BufferType_SPANNABLE : BufferType_NORMAL);

    // Called by setText above, but safer in case of future code changes
    assert(0);
    //mEditor->PrepareCursorControllers();
    return NOERROR;
}

ECode TextView::OnCreateDrawableState(
    /* [in] */ Int32 extraSpace,
    /* [out] */ ArrayOf<Int32>** drawableState)
{
    VALIDATE_NOT_NULL(drawableState);
    *drawableState = NULL;

    AutoPtr<ArrayOf<Int32> > states;
    if (mSingleLine) {
        View::OnCreateDrawableState(extraSpace, (ArrayOf<Int32>**)&states);
    } else {
        View::OnCreateDrawableState(extraSpace + 1, (ArrayOf<Int32>**)&states);
        MergeDrawableStates(states, MULTILINE_STATE_SET);
    }

    assert(states != NULL);
    *drawableState = states;
    REFCOUNT_ADD(*drawableState);
    Boolean isTextSelectable;
    if (IsTextSelectable(&isTextSelectable), isTextSelectable) {
        // Disable pressed state, which was introduced when TextView was made clickable.
        // Prevents text color change.
        // setClickable(FALSE) would have a similar effect, but it also disables focus changes
        // and long press actions, which are both needed by text selection.
        Int32 length = (*drawableState)->GetLength();
        for (Int32 i = 0; i < length; i++) {
            if ((**drawableState)[i] == R::attr::state_pressed) {
                AutoPtr<ArrayOf<Int32> > nonPressedState = ArrayOf<Int32>::Alloc(length - 1);
                nonPressedState->Copy(*drawableState, 0, i);
                nonPressedState->Copy(i, *drawableState, i + 1, length - i - 1);

                (*drawableState)->Release();
                *drawableState = nonPressedState;
                REFCOUNT_ADD(*drawableState);
                return NOERROR;
            }
        }
    }

    return NOERROR;
}

AutoPtr<IPath> TextView::GetUpdatedHighlightPath()
{
    AutoPtr<IPath> highlight = NULL;
    AutoPtr<IPaint> highlightPaint = mHighlightPaint;

    Int32 selStart, selEnd;
    GetSelectionStart(&selStart);
    GetSelectionEnd(&selEnd);
    Boolean isFocused, isPressed;
    if (mMovement != NULL && ((IsFocused(&isFocused), isFocused)
        || (IsPressed(&isPressed), isPressed)) && selStart >= 0) {
        if (selStart == selEnd) {
            assert(0);
            /*if (mEditor != NULL && mEditor->IsCursorVisible() &&
                (SystemClock::GetUptimeMillis() - mEditor->mShowCursor) %
                    (2 * Editor::BLINK) < Editor::BLINK) {
                if (mHighlightPathBogus) {
                    if (mHighlightPath == NULL) {
                        CPath::New((IPath**)&mHighlightPath);
                    }
                    mHighlightPath->Reset();
                    mLayout->GetCursorPath(selStart, mHighlightPath, mText);
                    mEditor->UpdateCursorsPositions();
                    mHighlightPathBogus = FALSE;
                }

                // XXX should pass to skin instead of drawing directly
                highlightPaint->SetColor(mCurTextColor);
                highlightPaint->SetStyle(Elastos::Droid::Graphics::PaintStyle_STROKE);
                highlight = mHighlightPath;
            }*/
        } else {
            if (mHighlightPathBogus) {
                if (mHighlightPath == NULL) {
                    CPath::New((IPath**)&mHighlightPath);
                }
                mHighlightPath->Reset();
                mLayout->GetSelectionPath(selStart, selEnd, mHighlightPath);
                mHighlightPathBogus = FALSE;
            }

            // XXX should pass to skin instead of drawing directly
            highlightPaint->SetColor(mHighlightColor);
            highlightPaint->SetStyle(Elastos::Droid::Graphics::PaintStyle_FILL);

            highlight = mHighlightPath;
        }
    }
    return highlight;
}

ECode TextView::GetHorizontalOffsetForDrawables(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset)
    *offset = 0;
    return NOERROR;
}

void TextView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    RestartMarqueeIfNeeded();
    // Draw the background for this view
    View::OnDraw(canvas);
    Int32 compoundPaddingTop, compoundPaddingBottom;
    GetCompoundPaddingTop(&compoundPaddingTop);
    GetCompoundPaddingBottom(&compoundPaddingBottom);
    Int32 compoundPaddingLeft, compoundPaddingRight;
    GetCompoundPaddingLeft(&compoundPaddingLeft);
    GetCompoundPaddingRight(&compoundPaddingRight);
    Int32 scrollX = mScrollX;
    Int32 scrollY = mScrollY;
    Int32 right = mRight;
    Int32 left = mLeft;
    Int32 bottom = mBottom;
    Int32 top = mTop;
    Int32 state;

    Boolean isLayoutRtl;
    IsLayoutRtl(&isLayoutRtl);
    Int32 offset;
    GetHorizontalOffsetForDrawables(&offset);
    Int32 leftOffset = isLayoutRtl ? 0 : offset;
    Int32 rightOffset = isLayoutRtl ? offset : 0 ;
    AutoPtr<Drawables> dr = mDrawables;
    if (dr != NULL) {
        /*
         * Compound, not extended, because the icon is not clipped
         * if the text height is smaller.
         */

        Int32 vspace = bottom - top - compoundPaddingBottom - compoundPaddingTop;
        Int32 hspace = right - left - compoundPaddingRight - compoundPaddingLeft;

        // IMPORTANT: The coordinates computed are also used in invalidateDrawable()
        // Make sure to update invalidateDrawable() when changing this code.
        if (dr->mDrawableLeft != NULL) {
            canvas->Save(&state);
            canvas->Translate(scrollX + mPaddingLeft + leftOffset,
                                scrollY + compoundPaddingTop +
                                (vspace - dr->mDrawableHeightLeft) / 2);
            dr->mDrawableLeft->Draw(canvas);
            canvas->Restore();
        }

        // IMPORTANT: The coordinates computed are also used in invalidateDrawable()
        // Make sure to update invalidateDrawable() when changing this code.
        if (dr->mDrawableRight != NULL) {
            canvas->Save(&state);
            canvas->Translate(scrollX + right - left - mPaddingRight - dr->mDrawableSizeRight - rightOffset,
                        scrollY + compoundPaddingTop + (vspace - dr->mDrawableHeightRight) / 2);
            dr->mDrawableRight->Draw(canvas);
            canvas->Restore();
        }

        // IMPORTANT: The coordinates computed are also used in invalidateDrawable()
        // Make sure to update invalidateDrawable() when changing this code.
        if (dr->mDrawableTop != NULL) {
            canvas->Save(&state);
            canvas->Translate(scrollX + compoundPaddingLeft + (hspace - dr->mDrawableWidthTop) / 2,
                    scrollY + mPaddingTop);
            dr->mDrawableTop->Draw(canvas);
            canvas->Restore();
        }

        // IMPORTANT: The coordinates computed are also used in invalidateDrawable()
        // Make sure to update invalidateDrawable() when changing this code.
        if (dr->mDrawableBottom != NULL) {
            canvas->Save(&state);
            canvas->Translate(scrollX + compoundPaddingLeft +
                    (hspace - dr->mDrawableWidthBottom) / 2,
                        scrollY + bottom - top - mPaddingBottom - dr->mDrawableSizeBottom);
            dr->mDrawableBottom->Draw(canvas);
            canvas->Restore();
        }
    }

    Int32 color = mCurTextColor;

    if (mLayout == NULL) {
        AssumeLayout();
    }

    AutoPtr<ILayout> layout = mLayout;

    Int32 len = 0;
    if (mHint != NULL && (mText->GetLength(&len), len) == 0) {
        if (mHintTextColor != NULL) {
            color = mCurHintTextColor;
        }

        layout = mHintLayout;
    }

    IPaint::Probe(mTextPaint)->SetColor(color);

    AutoPtr<ArrayOf<Int32> > drawableState;
    GetDrawableState((ArrayOf<Int32>**)&drawableState);
    mTextPaint->SetDrawableState(drawableState);

    canvas->Save(&state);
    /*  Would be faster if we didn't have to do this. Can we chop the
        (displayable) text so that we don't need to do this ever?
    */
    Int32 extendedPaddingTop, extendedPaddingBottom;
    GetExtendedPaddingTop(&extendedPaddingTop);
    GetExtendedPaddingBottom(&extendedPaddingBottom);

    Int32 vspace = mBottom - mTop - compoundPaddingBottom - compoundPaddingTop;
    Int32 maxScrollY;
    mLayout->GetHeight(&maxScrollY);
    maxScrollY -= vspace;

    Float clipLeft = compoundPaddingLeft + scrollX;
    Float clipTop = (scrollY == 0) ? 0 : extendedPaddingTop + scrollY;
    Float clipRight = right - left - compoundPaddingRight + scrollX;
    Float clipBottom = bottom - top + scrollY -
        ((scrollY == maxScrollY) ? 0 : extendedPaddingBottom);

    if (mShadowRadius != 0) {
        clipLeft += Elastos::Core::Math::Min(0.f, mShadowDx - mShadowRadius);
        clipRight += Elastos::Core::Math::Max(0.f, mShadowDx + mShadowRadius);

        clipTop += Elastos::Core::Math::Min(0.f, mShadowDy - mShadowRadius);
        clipBottom += Elastos::Core::Math::Max(0.f, mShadowDy + mShadowRadius);
    }

    Boolean isNonEmpty;
    canvas->ClipRect(clipLeft, clipTop, clipRight, clipBottom, &isNonEmpty);

    Int32 voffsetText = 0;
    Int32 voffsetCursor = 0;

    // translate in by our padding
    /* shortcircuit calling getVerticaOffset() */
    if ((mGravity & IGravity::VERTICAL_GRAVITY_MASK) != IGravity::TOP) {
        voffsetText = GetVerticalOffset(FALSE);
        voffsetCursor = GetVerticalOffset(TRUE);
    }
    canvas->Translate(compoundPaddingLeft, extendedPaddingTop + voffsetText);

    Int32 layoutDirection;
    GetLayoutDirection(&layoutDirection);
    Int32 absoluteGravity = Gravity::GetAbsoluteGravity(mGravity, layoutDirection);
    if (mEllipsize == TextUtilsTruncateAt_MARQUEE &&
        mMarqueeFadeMode != MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS) {
        Int32 linecount;
        if (!mSingleLine && (GetLineCount(&linecount), linecount) == 1 && CanMarquee() &&
            (absoluteGravity & IGravity::HORIZONTAL_GRAVITY_MASK) != IGravity::LEFT) {
                Int32 width = mRight - mLeft;
                Int32 padding, compoundPaddingRight;
                GetCompoundPaddingLeft(&padding);
                GetCompoundPaddingRight(&compoundPaddingRight);
                padding += compoundPaddingRight;
                Float dx;
                mLayout->GetLineRight(0, &dx);
                dx = dx - (width - padding);
                Int32 direction;
                layout->GetParagraphDirection(0, &direction);
                canvas->Translate(direction * dx, 0.0f);
        }

        if (mMarquee != NULL && mMarquee->IsRunning()) {
            const Float dx = -mMarquee->GetScroll();
            Int32 direction;
            layout->GetParagraphDirection(0, &direction);
            canvas->Translate(direction * dx, 0.0f);
        }
    }

    Int32 cursorOffsetVertical = voffsetCursor - voffsetText;
    AutoPtr<IPath> highlight = GetUpdatedHighlightPath();

    if (mEditor != NULL) {
        assert(0);
        //mEditor->OnDraw(canvas, layout, highlight, mHighlightPaint, cursorOffsetVertical);
    } else {
        layout->Draw(canvas, highlight, mHighlightPaint, cursorOffsetVertical);
    }

    if (mMarquee != NULL && mMarquee->ShouldDrawGhost()) {
        Float dx = (Int32) mMarquee->GetGhostOffset();
        Int32 direction;
        layout->GetParagraphDirection(0, &direction);
        canvas->Translate(direction * dx, 0.0f);
        layout->Draw(canvas, highlight, mHighlightPaint, cursorOffsetVertical);
    }
    canvas->Restore();
}

ECode TextView::GetFocusedRect(
    /* [in] */ IRect* rect)
{
    assert(rect != NULL);

    if (mLayout == NULL) {
        return View::GetFocusedRect(rect);
    }

    Int32 selEnd;
    GetSelectionEnd(&selEnd);
    if (selEnd < 0) {
        return View::GetFocusedRect(rect);
    }

    Int32 selStart;
    GetSelectionStart(&selStart);
    if (selStart < 0 || selStart >= selEnd) {
        Int32 line, l, t, r, b;
        mLayout->GetLineForOffset(selEnd, &line);
        mLayout->GetLineTop(line, &t);
        mLayout->GetLineBottom(line, &b);
        Float pos;
        mLayout->GetPrimaryHorizontal(selEnd, &pos);
        l = pos - 2;
        r = l + 4;
        rect->Set(l, t, r, b);
    } else {
        Int32 lineStart, t, b;
        mLayout->GetLineForOffset(selStart, &lineStart);
        Int32 lineEnd;
        mLayout->GetLineForOffset(selEnd, &lineEnd);

        mLayout->GetLineTop(lineStart, &t);
        mLayout->GetLineBottom(lineEnd, &b);
        rect->SetTop(t);
        rect->SetBottom(b);
        if (lineStart == lineEnd) {
            Float lp, rp;
            mLayout->GetPrimaryHorizontal(selStart, &lp);
            mLayout->GetPrimaryHorizontal(selEnd, &rp);
            rect->SetLeft((Int32)lp);
            rect->SetRight((Int32)rp);
        } else {
            // Selection extends across multiple lines -- make the focused
            // rect cover the entire width.
            if (mHighlightPathBogus) {
                if (mHighlightPath == NULL) {
                    CPath::New((IPath**)&mHighlightPath);
                }
                mHighlightPath->Reset();
                mLayout->GetSelectionPath(selStart, selEnd, mHighlightPath);
                mHighlightPathBogus = FALSE;
            }

            {
                AutoLock lock(TextView::sTempRectLock);
                mHighlightPath->ComputeBounds(TEMP_RECTF, TRUE);
                rect->SetLeft((Int32)((CRectF*)TEMP_RECTF.Get())->mLeft - 1);
                rect->SetRight((Int32)((CRectF*)TEMP_RECTF.Get())->mRight + 1);
            }
        }
    }

    // Adjust for padding and gravity.
    Int32 paddingLeft, paddingTop;
    GetCompoundPaddingLeft(&paddingLeft);
    GetExtendedPaddingTop(&paddingTop);
    if ((mGravity & IGravity::VERTICAL_GRAVITY_MASK) != IGravity::TOP) {
        paddingTop += GetVerticalOffset(FALSE);
    }
    rect->Offset(paddingLeft, paddingTop);

    Int32 paddingBottom, b;
    GetExtendedPaddingBottom(&paddingBottom);
    rect->GetBottom(&b);
    rect->SetBottom(b + paddingBottom);
    return NOERROR;
}

ECode TextView::GetLineCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    if (mLayout != NULL) {
        mLayout->GetLineCount(count);
    } else {
        *count = 0;
    }
    return NOERROR;
}

ECode TextView::GetLineBounds(
    /* [in] */ Int32 line,
    /* [in] */ IRect* bounds,
    /* [out] */ Int32* y)
{
    VALIDATE_NOT_NULL(y)
    if (mLayout == NULL) {
        if (bounds != NULL) {
            bounds->Set(0, 0, 0, 0);
        }
        *y = 0;
        return NOERROR;
    }
    else {
        Int32 baseline, voffset;
        mLayout->GetLineBounds(line, bounds, &baseline);
        GetExtendedPaddingTop(&voffset);
        if ((mGravity & IGravity::VERTICAL_GRAVITY_MASK) != IGravity::TOP) {
            voffset += GetVerticalOffset(TRUE);
        }
        if (bounds != NULL) {
            Int32 compoundPaddingLeft;
            GetCompoundPaddingLeft(&compoundPaddingLeft);
            bounds->Offset(compoundPaddingLeft, voffset);
        }
        *y = baseline + voffset;
        return NOERROR;
    }
}

ECode TextView::GetBaseline(
    /* [out] */ Int32* baseline)
{
    assert(baseline != NULL);

    if (mLayout == NULL) {
        return View::GetBaseline(baseline);
    }

    Int32 voffset = 0;
    if ((mGravity & IGravity::VERTICAL_GRAVITY_MASK) != IGravity::TOP) {
        voffset = GetVerticalOffset(TRUE);
    }

    if (View::IsLayoutModeOptical((IInterface*)mParent)) {
        AutoPtr<IInsets> set;
        GetOpticalInsets((IInsets**)&set);
        Int32 top;
        set->GetTop(&top);
        voffset -= top;
    }

    mLayout->GetLineBaseline(0, baseline);
    Int32 extendedPaddingTop;
    GetExtendedPaddingTop(&extendedPaddingTop);
    *baseline += extendedPaddingTop + voffset;

    return NOERROR;
}

Int32 TextView::GetFadeTop(
    /* [in] */ Boolean offsetRequired)
{
    if (mLayout == NULL) return 0;

    Int32 voffset = 0;
    if ((mGravity & IGravity::VERTICAL_GRAVITY_MASK) != IGravity::TOP) {
        voffset = GetVerticalOffset(TRUE);
    }

    if (offsetRequired) voffset += GetTopPaddingOffset();
    Int32 extendedPaddingTop;
    GetExtendedPaddingTop(&extendedPaddingTop);
    return extendedPaddingTop + voffset;
}

Int32 TextView::GetFadeHeight(
    /* [in] */ Boolean offsetRequired)
{
    if (mLayout) {
        Int32 h;
        mLayout->GetHeight(&h);
        return h;
    }
    return 0;
}

ECode TextView::OnKeyPreIme(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (keyCode == IKeyEvent::KEYCODE_BACK) {
        assert(0);
        Boolean isInSelectionMode;// = mEditor != NULL && mEditor->mSelectionActionMode != NULL;

        if (isInSelectionMode) {
            Int32 action, repeatCount;
            event->GetAction(&action);
            event->GetRepeatCount(&repeatCount);
            if (action == IKeyEvent::ACTION_DOWN && repeatCount == 0) {
                AutoPtr<IDispatcherState> state;
                GetKeyDispatcherState((IDispatcherState**)&state);
                if (state != NULL) {
                    state->StartTracking(event, TO_IINTERFACE(this));
                }
                *result = TRUE;
                return NOERROR;
            } else if (action == IKeyEvent::ACTION_UP) {
                AutoPtr<IDispatcherState> state;
                GetKeyDispatcherState((IDispatcherState**)&state);
                if (state != NULL) {
                    state->HandleUpEvent(event);
                }
                Boolean track, cancel;
                event->IsTracking(&track);
                event->IsCanceled(&cancel);
                if (track && !cancel) {
                    StopSelectionActionMode();
                    *result = TRUE;
                    return NOERROR;
                }
            }
        }
    }

    return View::OnKeyPreIme(keyCode, event, result);
}

ECode TextView::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 which = DoKeyDown(keyCode, event, NULL);
    if (which == 0) {
        // Go through default dispatching.
        return View::OnKeyDown(keyCode, event, result);
    }
    *result = TRUE;
    return NOERROR;
}

ECode TextView::OnKeyMultiple(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 repeatCount,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<IKeyEvent> down;
    assert(0);
    //CKeyEvent::ChangeAction(event, IKeyEvent::ACTION_DOWN, (IKeyEvent**)&down);

    Int32 which = DoKeyDown(keyCode, down, event);
    if (which == 0) {
        // Go through default dispatching.
        return View::OnKeyMultiple(keyCode, repeatCount, event, result);
    }
    if (which == -1) {
        // Consumed the whole thing.
        *result = TRUE;
        return NOERROR;
    }

    repeatCount--;

    // We are going to dispatch the remaining events to either the input
    // or movement method.  To do this, we will just send a repeated stream
    // of down and up events until we have done the complete repeatCount.
    // It would be nice if those interfaces had an onKeyMultiple() method,
    // but adding that is a more complicated change.
    AutoPtr<IKeyEvent> up;
    assert(0);
    //CKeyEvent::ChangeAction(event, IKeyEvent::ACTION_UP, (IKeyEvent**)&up);

    if (which == 1) {
        assert(0);
        /*Boolean res;
        mEditor->mKeyListener->OnKeyUp(
            this, IEditable::Probe(mText),
            keyCode, up, &res);
        while (--repeatCount > 0) {
            mEditor->mKeyListener->OnKeyDown(
                this, IEditable::Probe(mText),
                keyCode, down, &res);
            mEditor->mKeyListener->OnKeyUp(
                this, IEditable::Probe(mText),
                keyCode, up, &res);
        }*/
        HideErrorIfUnchanged();
    }
    else if (which == 2) {
        Boolean res;
        mMovement->OnKeyUp(
            this, ISpannable::Probe(mText),
            keyCode, up, &res);
        while (--repeatCount > 0) {
            mMovement->OnKeyDown(
                this, ISpannable::Probe(mText),
                keyCode, down, &res);
            mMovement->OnKeyUp(
                this, ISpannable::Probe(mText),
                keyCode, up, &res);
        }
    }

    *result = TRUE;
    return NOERROR;
}

Boolean TextView::ShouldAdvanceFocusOnEnter()
{
    AutoPtr<IKeyListener> keyListener;
    GetKeyListener((IKeyListener**)&keyListener);
    if (keyListener == NULL) {
        return FALSE;
    }

    if (mSingleLine) {
        return TRUE;
    }
    assert(0);
   /* if (mEditor != NULL && (mEditor->mInputType & IInputType::TYPE_MASK_CLASS) == IInputType::TYPE_CLASS_TEXT) {
        Int32 variation = mEditor->mInputType & IInputType::TYPE_MASK_VARIATION;
        if (variation == IInputType::TYPE_TEXT_VARIATION_EMAIL_ADDRESS ||
            variation == IInputType::TYPE_TEXT_VARIATION_EMAIL_SUBJECT) {
            return TRUE;
        }
    }*/

    return FALSE;
}

Boolean TextView::ShouldAdvanceFocusOnTab()
{
    AutoPtr<IKeyListener> keyListener;
    GetKeyListener((IKeyListener**)&keyListener);
    assert(0);
    /*if (keyListener != NULL && !mSingleLine && mEditor != NULL &&
            (mEditor->mInputType & IInputType::TYPE_MASK_CLASS) == IInputType::TYPE_CLASS_TEXT) {
        Int32 variation = mEditor->mInputType & IInputType::TYPE_MASK_VARIATION;
        if (variation == IInputType::TYPE_TEXT_FLAG_IME_MULTI_LINE
                || variation == IInputType::TYPE_TEXT_FLAG_MULTI_LINE) {
            return FALSE;
        }
    }*/
    return TRUE;
}

Int32 TextView::DoKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [in] */ IKeyEvent* otherEvent)
{
    Boolean isEnabled;
    if (IsEnabled(&isEnabled), !isEnabled) {
        return 0;
    }

    Int32 repeatCount;
    event->GetRepeatCount(&repeatCount);
    Boolean res;
    assert(0);
    //CKeyEvent::IsModifierKey(keyCode, &res);
    if (repeatCount == 0 && !res) {
        mPreventDefaultMovement = FALSE;
    }

    Int32 flag;
    event->GetFlags(&flag);
    Boolean result;
    Boolean hasNoMod, hasMod;
    event->HasNoModifiers(&hasNoMod);
    switch (keyCode) {
        case IKeyEvent::KEYCODE_ENTER:
            if (hasNoMod) {
                // When mInputContentType is set, we know that we are
                // running in a "modern" cupcake environment, so don't need
                // to worry about the application trying to capture
                // enter key events.
                assert(0);
                /*if (mEditor != NULL && mEditor->mInputContentType != NULL) {
                    // If there is an action listener, given them a
                    // chance to consume the event.
                    if (mEditor->mInputContentType->mOnEditorActionListener != NULL) {
                        mEditor->mInputContentType->mOnEditorActionListener->OnEditorAction(
                                this, IEditorInfo::IME_NULL, event, &result);
                        if (result) {
                            mEditor->mInputContentType->mEnterDown = TRUE;
                            // We are consuming the enter key for them.
                            return -1;
                        }
                    }
                }*/

                // If our editor should move focus when enter is pressed, or
                // this is a generated event from an IME action button, then
                // don't let it be inserted into the text.
                if ((flag & IKeyEvent::FLAG_EDITOR_ACTION) != 0
                    || ShouldAdvanceFocusOnEnter()) {
                        Boolean clickListeners;
                        if (HasOnClickListeners(&clickListeners), clickListeners) {
                            return 0;
                        }
                        return -1;
                }
            }
            break;

        case IKeyEvent::KEYCODE_DPAD_CENTER:
            if (hasNoMod) {
                if (ShouldAdvanceFocusOnEnter()) {
                    return 0;
                }
            }
            break;

        case IKeyEvent::KEYCODE_TAB:
            event->HasModifiers(IKeyEvent::META_SHIFT_ON, &hasMod);
            if (hasNoMod || hasMod) {
                if (ShouldAdvanceFocusOnTab()) {
                    return 0;
                }
            }
            break;

            // Has to be done on key down (and not on key up) to correctly be intercepted.
        case IKeyEvent::KEYCODE_BACK:
            assert(0);
            /*if (mEditor != NULL && mEditor->mSelectionActionMode != NULL) {
                StopSelectionActionMode();
                return -1;
            }*/
            break;
    }

    assert(0);
    /*if (mEditor != NULL && mEditor->mKeyListener != NULL) {

        Boolean doDown = TRUE;
        if (otherEvent != NULL) {
            //try {
            BeginBatchEdit();
            Boolean handled = FALSE;
            mEditor->mKeyListener->OnKeyOther(this, IEditable::Probe(mText), otherEvent, &handled);
            HideErrorIfUnchanged();
            doDown = FALSE;
            if (handled) {
                return -1;
            }
            //} catch (AbstractMethodError e) {
            //    // onKeyOther was added after 1.0, so if it isn't
            //    // implemented we need to try to dispatch as a regular down.
            //} finally {
            EndBatchEdit();
            //}
        }

        if (doDown) {
            BeginBatchEdit();
            Boolean handled;
            mEditor->mKeyListener->OnKeyDown(this, IEditable::Probe(mText), keyCode, event, &handled);
            EndBatchEdit();
            HideErrorIfUnchanged();
            if (handled) return 1;
        }
    }*/

    // bug 650865: sometimes we get a key event before a layout.
    // don't try to move around if we don't know the layout.

    if (mMovement != NULL && mLayout != NULL) {
        Boolean doDown = TRUE;
        if (otherEvent != NULL) {
            Boolean handled;
            mMovement->OnKeyOther(this, ISpannable::Probe(mText), otherEvent, &handled);
            doDown = FALSE;
            if (handled) {
                return -1;
            }
        }

        if (doDown) {
            Boolean res;
            mMovement->OnKeyDown(this, ISpannable::Probe(mText), keyCode, event, &res);
            if (res) {
                Int32 repeatCount;
                event->GetRepeatCount(&repeatCount);
                Boolean res;
                assert(0);
                //CKeyEvent::IsModifierKey(keyCode, &res);
                if (repeatCount == 0 && !res) {
                    mPreventDefaultMovement = TRUE;
                }

                return 2;
            }
        }
    }
    Boolean lastModifierKey;
    assert(0);
    //CKeyEvent::IsModifierKey(keyCode, &lastModifierKey);
    return mPreventDefaultMovement && !lastModifierKey ? -1 : 0;
}

ECode TextView::ResetErrorChangedFlag()
{
    /*
     * Keep track of what the error was before doing the input
     * so that if an input filter changed the error, we leave
     * that error showing.  Otherwise, we take down whatever
     * error was showing when the user types something.
     */
    assert(0);
    //if (mEditor != NULL) mEditor->mErrorWasChanged = FALSE;
    return NOERROR;
}

/**
 * @hide
 */
ECode TextView::HideErrorIfUnchanged()
{
    assert(0);
    //if (mEditor != NULL && mEditor->mError != NULL && !mEditor->mErrorWasChanged) {
        SetError(NULL, NULL);
    //}
    return NOERROR;
}

ECode TextView::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* resValue)
{
    VALIDATE_NOT_NULL(resValue);
    Boolean isEnabled;
    if (IsEnabled(&isEnabled), !isEnabled) {
        return View::OnKeyUp(keyCode, event, resValue);
    }

    Boolean res;
    assert(0);
    //CKeyEvent::IsModifierKey(keyCode, &res);
    if (!res) {
        mPreventDefaultMovement = FALSE;
    }

    Int32 flags;
    event->GetFlags(&flags);
    Boolean result;
    Boolean hasNoMod;
    event->HasNoModifiers(&hasNoMod);
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_CENTER:
        {
            if (hasNoMod) {
                /*
                * If there is a click listener, just call through to
                * super, which will invoke it.
                *
                * If there isn't a click listener, try to show the soft
                * input method.  (It will also
                * call performClick(), but that won't do anything in
                * this case.)
                */
                Boolean clickListeners;
                if (HasOnClickListeners(&clickListeners), !clickListeners) {
                    Boolean onCheckIsTextEditor;
                    if (mMovement != NULL && IEditable::Probe(mText) && mLayout != NULL
                        && (OnCheckIsTextEditor(&onCheckIsTextEditor), onCheckIsTextEditor)) {
                        assert(0);
                        AutoPtr<IInputMethodManager> imm;// = CInputMethodManager::PeekInstance();
                        ViewClicked(imm);
                        Boolean showSoftInputOnFocus;
                        if (imm != NULL &&
                            (GetShowSoftInputOnFocus(&showSoftInputOnFocus), showSoftInputOnFocus)) {
                            imm->ShowSoftInput(this, 0, &result);
                        }
                    }
                }
            }

            return View::OnKeyUp(keyCode, event, resValue);
        }

        case IKeyEvent::KEYCODE_ENTER:
            if (hasNoMod) {
                assert(0);
                /*if (mEditor != NULL  && mEditor->mInputContentType != NULL
                    && mEditor->mInputContentType->mOnEditorActionListener != NULL
                    && mEditor->mInputContentType->mEnterDown) {
                    mEditor->mInputContentType->mEnterDown = FALSE;
                    mEditor->mInputContentType->mOnEditorActionListener->OnEditorAction(
                            this, IEditorInfo::IME_NULL, event, &result);
                    if (result) {
                        *resValue = TRUE;
                        return NOERROR;
                    }
                }*/

                if ((flags & IKeyEvent::FLAG_EDITOR_ACTION) != 0
                    || ShouldAdvanceFocusOnEnter()) {
                        /*
                        * If there is a click listener, just call through to
                        * super, which will invoke it.
                        *
                        * If there isn't a click listener, try to advance focus,
                        * but still call through to super, which will reset the
                        * pressed state and longpress state.  (It will also
                        * call performClick(), but that won't do anything in
                        * this case.)
                        */
                        Boolean clickListeners;
                        if (HasOnClickListeners(&clickListeners), !clickListeners) {
                            AutoPtr<IView> v;
                            FocusSearch(IView::FOCUS_DOWN, (IView**)&v);

                            if (v != NULL) {
                                Boolean focus = FALSE;
                                v->RequestFocus(IView::FOCUS_DOWN, &focus);
                                if (!focus) {
                                    assert(0 && "TODO");
                                    // throw new IllegalStateException("focus search returned a view " +
                                    //         "that wasn't able to take focus!");
                                }

                                /*
                                * Return TRUE because we handled the key; super
                                * will return FALSE because there was no click
                                * listener.
                                */
                                Boolean res;
                                View::OnKeyUp(keyCode, event, &res);
                                return TRUE;
                            }
                            else if ((flags & IKeyEvent::FLAG_EDITOR_ACTION) != 0) {
                                // No target for next focus, but make sure the IME
                                // if this came from it.
                                assert(0);
                                AutoPtr<IInputMethodManager> imm;// = CInputMethodManager::PeekInstance();
                                if (imm != NULL) {
                                    imm->IsActive(this, &result);
                                    if (result) {
                                        AutoPtr<IBinder> token;
                                        GetWindowToken((IBinder**)&token);
                                        imm->HideSoftInputFromWindow(token, 0, &result);
                                    }
                                }
                            }
                        }
                }

                return View::OnKeyUp(keyCode, event, resValue);
            }
            break;
    }
    assert(0);
    /*if (mEditor != NULL && mEditor->mKeyListener != NULL) {
        Boolean res;
        mEditor->mKeyListener->OnKeyUp(this, IEditable::Probe(mText), keyCode, event, &res);
        if (res) {
            *resValue = TRUE;
            return NOERROR;
        }
    }*/

    if (mMovement != NULL && mLayout != NULL) {
        Boolean res;
        mMovement->OnKeyUp(this, ISpannable::Probe(mText), keyCode, event, &res);
        if (res) {
            *resValue = TRUE;
            return NOERROR;
        }
    }
    return View::OnKeyUp(keyCode, event, resValue);
}

ECode TextView::OnCheckIsTextEditor(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    assert(0);
    //*res = mEditor != NULL && mEditor->mInputType != IInputType::TYPE_NULL;
    return NOERROR;
}

ECode TextView::OnCreateInputConnection(
    /* [in] */ IEditorInfo* outAttrs,
    /* [out] */ IInputConnection** res)
{
    VALIDATE_NOT_NULL(res)
    Boolean isEnabled, onCheckIsTextEditor;
    if ((OnCheckIsTextEditor(&onCheckIsTextEditor), onCheckIsTextEditor)
        && (IsEnabled(&isEnabled), isEnabled)) {
        assert(0);
        /*mEditor->CreateInputMethodStateIfNeeded();
        outAttrs->SetInputType(GetInputType());
        if (mEditor != NULL && mEditor->mInputContentType != NULL) {
            outAttrs->SetImeOptions(mEditor->mInputContentType->mImeOptions);
            outAttrs->SetPrivateImeOptions(mEditor->mInputContentType->mPrivateImeOptions);
            outAttrs->SetActionLabel(mEditor->mInputContentType->mImeActionLabel);
            outAttrs->SetActionId(mEditor->mInputContentType->mImeActionId);
            outAttrs->SetExtras(mEditor->mInputContentType->mExtras);
        }
        else {
           outAttrs->SetImeOptions(IEditorInfo::IME_NULL);
        }*/

        Int32 options = 0;
        AutoPtr<IView> searchedView;
        if (NULL != (FocusSearch(IView::FOCUS_DOWN, (IView**)&searchedView), searchedView)) {
            outAttrs->GetImeOptions(&options);
            options |= IEditorInfo::IME_FLAG_NAVIGATE_NEXT;
            outAttrs->SetImeOptions(options);
        }

        if (NULL != (FocusSearch(IView::FOCUS_UP, (IView**)&searchedView), searchedView)) {
            outAttrs->GetImeOptions(&options);
            options |= IEditorInfo::IME_FLAG_NAVIGATE_PREVIOUS;
            outAttrs->SetImeOptions(options);
        }

        outAttrs->GetImeOptions(&options);
        if ((options & IEditorInfo::IME_MASK_ACTION) == IEditorInfo::IME_ACTION_UNSPECIFIED) {
            if ((options & IEditorInfo::IME_FLAG_NAVIGATE_NEXT) != 0) {
                // An action has not been set, but the enter key will move to
                // the next focus, so set the action to that.
                options |= IEditorInfo::IME_ACTION_NEXT;
                outAttrs->SetImeOptions(options);
            }
            else {
                // An action has not been set, and there is no focus to move
                // to, so let's just supply a "done" action.
                options |= IEditorInfo::IME_ACTION_DONE;
                outAttrs->SetImeOptions(options);
            }

            if (!ShouldAdvanceFocusOnEnter()) {
                outAttrs->GetImeOptions(&options);
                options |= IEditorInfo::IME_FLAG_NO_ENTER_ACTION;
                outAttrs->SetImeOptions(options);
            }
        }

        Int32 inputType = 0;
        outAttrs->GetInputType(&inputType);
        if (IsMultilineInputType(inputType)) {
            outAttrs->GetImeOptions(&options);
            options |= IEditorInfo::IME_FLAG_NO_ENTER_ACTION;
            // Multi-line text editors should always show an enter key.
            outAttrs->SetImeOptions(options);
        }

        outAttrs->SetHintText(mHint);
        if (IEditable::Probe(mText) != NULL) {
           AutoPtr<IInputConnection> ic;
           assert(0);
           //CEditableInputConnection::New(this, (IEditableInputConnection**)&ic);
           Int32 start, end;
           GetSelectionStart(&start);
           GetSelectionEnd(&end);
           outAttrs->SetInitialSelStart(start);
           outAttrs->SetInitialSelEnd(end);

           Int32 initialCapsMode = 0, inputType;
           GetInputType(&inputType);
           ic->GetCursorCapsMode(inputType, &initialCapsMode);
           outAttrs->SetInitialCapsMode(initialCapsMode);
           *res = ic;
           REFCOUNT_ADD(*res)
           return NOERROR;
        }
    }

    *res = NULL;
    return NOERROR;
}

ECode TextView::ExtractText(
    /* [in] */ IExtractedTextRequest* request,
    /* [in] */ IExtractedText* outText,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    //*result = mEditor->ExtractText(request, outText);
    return NOERROR;
}

/**
 * This is used to remove all style-impacting spans from text before new
 * extracted text is being replaced into it, so that we don't have any
 * lingering spans applied during the replace.
 */
void TextView::RemoveParcelableSpans(
    /* [in] */ ISpannable* spannable,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoPtr<ArrayOf<IInterface*> > spans;
    ISpanned::Probe(spannable)->GetSpans(start, end, EIID_IParcelableSpan, (ArrayOf<IInterface*>**)&spans);
    for (Int32 i = spans->GetLength() - 1; i > 0; --i) {
        spannable->RemoveSpan((*spans)[i]);
    }
}

ECode TextView::SetExtractedText(
    /* [in] */ IExtractedText* text)
{
    assert(text != NULL);

    AutoPtr<IEditable> content;
    GetEditableText((IEditable**)&content);
    AutoPtr<ICharSequence> tt;
    text->GetText((ICharSequence**)&tt);
    if (tt != NULL) {
        Int32 start;
        text->GetPartialStartOffset(&start);
        if (content == NULL) {
            SetText(tt, BufferType_EDITABLE);
        }
        else if (start < 0) {
            Int32 length;
            ICharSequence::Probe(content)->GetLength(&length);
            RemoveParcelableSpans(ISpannable::Probe(content), 0, length);
            ICharSequence::Probe(content)->GetLength(&length);
            content->Replace(0, length, tt);
        }
        else {
            Int32 N;
            ICharSequence::Probe(content)->GetLength(&N);
            if (start > N) start = N;
            Int32 end;
            text->GetPartialEndOffset(&end);
            if (end > N) end = N;
            RemoveParcelableSpans(ISpannable::Probe(content), start, end);
            content->Replace(start, end, tt);
        }
    }

    // Now set the selection position...  make sure it is in range, to
    // avoid crashes.  If this is a partial update, it is possible that
    // the underlying text may have changed, causing us problems here.
    // Also we just don't want to trust clients to do the right thing.
    AutoPtr<ICharSequence> sp;
    GetText((ICharSequence**)&sp);
    AutoPtr<ISpannable> spannable = ISpannable::Probe(sp);
    Int32 N;
    sp->GetLength(&N);
    Int32 start;
    text->GetSelectionStart(&start);
    if (start < 0) start = 0;
    else if (start > N) start = N;
    Int32 end;
    text->GetSelectionEnd(&end);
    if (end < 0) end = 0;
    else if (end > N) end = N;
    Selection::SetSelection(spannable, start, end);

    // Finally, update the selection mode.
    AutoPtr<IMetaKeyKeyListenerHelper> helper;
    CMetaKeyKeyListenerHelper::AcquireSingleton((IMetaKeyKeyListenerHelper**)&helper);

    Int32 flags;
    text->GetFlags(&flags);
    if ((flags & IExtractedText::FLAG_SELECTING) != 0) {
        helper->StartSelecting(this, spannable);
    } else {
        helper->StopSelecting(this, spannable);
    }
    return NOERROR;
}

ECode TextView::SetExtracting(
    /* [in] */ IExtractedTextRequest* req)
{
    assert(0);
    /*if (mEditor != NULL && mEditor->mInputMethodState != NULL) {
        mEditor->mInputMethodState->mExtractedTextRequest = req;

        // This would stop a possible selection mode, but no such mode is started in case
        // extracted mode will start. Some text is selected though, and will trigger an action mode
        // in the extracted view.
        mEditor->HideControllers();
    }*/
    return NOERROR;
}

ECode TextView::OnCommitCompletion(
    /* [in] */ ICompletionInfo* text)
{
    return NOERROR;
}

ECode TextView::OnCommitCorrection(
    /* [in] */ ICorrectionInfo* info)
{
    if (mEditor != NULL) mEditor->OnCommitCorrection(info);
    return NOERROR;
}

ECode TextView::BeginBatchEdit()
{
    if (mEditor != NULL) mEditor->BeginBatchEdit();
    return NOERROR;
}

ECode TextView::EndBatchEdit()
{
    if (mEditor != NULL) mEditor->EndBatchEdit();
    return NOERROR;
}

ECode TextView::OnBeginBatchEdit()
{
    return NOERROR;
}

ECode TextView::OnEndBatchEdit()
{
    return NOERROR;
}

ECode TextView::OnPrivateIMECommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

void TextView::NullLayouts()
{
    if (mLayout != NULL && IBoringLayout::Probe(mLayout) != NULL
        && mSavedLayout == NULL) {
        mSavedLayout = IBoringLayout::Probe(mLayout);
    }

    if (mHintLayout != NULL && IBoringLayout::Probe(mHintLayout) != NULL
        && mSavedHintLayout == NULL) {
        mSavedHintLayout = IBoringLayout::Probe(mHintLayout);
    }

    mSavedMarqueeModeLayout = mLayout = mHintLayout = NULL;

    mBoring = mHintBoring = NULL;

    // Since it depends on the value of mLayout
    assert(0);
    //if (mEditor != NULL) mEditor->PrepareCursorControllers();
}

void TextView::AssumeLayout()
{
    Int32 compoundPaddingLeft, compoundPaddingRight;
    GetCompoundPaddingLeft(&compoundPaddingLeft);
    GetCompoundPaddingRight(&compoundPaddingRight);
    Int32 width = mRight - mLeft - compoundPaddingLeft - compoundPaddingRight;

    if (width < 1) {
        width = 0;
    }

    Int32 physicalWidth = width;

    if (mHorizontallyScrolling) {
        width = VERY_WIDE;
    }

    MakeNewLayout(width, physicalWidth, UNKNOWN_BORING, UNKNOWN_BORING,
            physicalWidth, FALSE);
}

LayoutAlignment TextView::GetLayoutAlignment()
{
    LayoutAlignment alignment;
    Int32 alignmentNum;
    switch (GetTextAlignment(&alignmentNum), alignmentNum) {
        case IView::TEXT_ALIGNMENT_GRAVITY:
            switch (mGravity & IGravity::RELATIVE_HORIZONTAL_GRAVITY_MASK) {
                case IGravity::START:
                    alignment = Elastos::Droid::Text::ALIGN_NORMAL;
                    break;
                case IGravity::END:
                    alignment = Elastos::Droid::Text::ALIGN_OPPOSITE;
                    break;
                case IGravity::LEFT:
                    alignment = Elastos::Droid::Text::ALIGN_LEFT;
                    break;
                case IGravity::RIGHT:
                    alignment = Elastos::Droid::Text::ALIGN_RIGHT;
                    break;
                case IGravity::CENTER_HORIZONTAL:
                    alignment = Elastos::Droid::Text::ALIGN_CENTER;
                    break;
                default:
                    alignment = Elastos::Droid::Text::ALIGN_NORMAL;
                    break;
            }
            break;
        case IView::TEXT_ALIGNMENT_TEXT_START:
            alignment = Elastos::Droid::Text::ALIGN_NORMAL;
            break;
        case IView::TEXT_ALIGNMENT_TEXT_END:
            alignment = Elastos::Droid::Text::ALIGN_OPPOSITE;
            break;
        case IView::TEXT_ALIGNMENT_CENTER:
            alignment = Elastos::Droid::Text::ALIGN_CENTER;
            break;
        case IView::TEXT_ALIGNMENT_VIEW_START: {
            Int32 direction;
            alignment = ((GetLayoutDirection(&direction), direction) == IView::LAYOUT_DIRECTION_RTL) ?
                Elastos::Droid::Text::ALIGN_RIGHT : Elastos::Droid::Text::ALIGN_LEFT;
            break;
        }
        case IView::TEXT_ALIGNMENT_VIEW_END: {
            Int32 direction;
            alignment = ((GetLayoutDirection(&direction), direction) == IView::LAYOUT_DIRECTION_RTL) ?
                Elastos::Droid::Text::ALIGN_LEFT : Elastos::Droid::Text::ALIGN_RIGHT;
            break;
        }
        case IView::TEXT_ALIGNMENT_INHERIT:
            // This should never happen as we have already resolved the text alignment
            // but better safe than sorry so we just fall through
        default:
            alignment = Elastos::Droid::Text::ALIGN_NORMAL;
            break;
    }
    return alignment;
}

void TextView::MakeNewLayout(
    /* [in] */ Int32 wantWidth,
    /* [in] */ Int32 hintWidth,
    /* [in] */ IBoringLayoutMetrics* _boring,
    /* [in] */ IBoringLayoutMetrics* _hintBoring,
    /* [in] */ Int32 ellipsisWidth,
    /* [in] */ Boolean bringIntoView)
{
    AutoPtr<IBoringLayoutMetrics> boring = _boring;
    AutoPtr<IBoringLayoutMetrics> hintBoring = _hintBoring;
    StopMarquee();

    // Update "old" cached values
    mOldMaximum = mMaximum;
    mOldMaxMode = mMaxMode;

    mHighlightPathBogus = TRUE;

    if (wantWidth < 0) {
        wantWidth = 0;
    }
    if (hintWidth < 0) {
        hintWidth = 0;
    }

    LayoutAlignment alignment = GetLayoutAlignment();

    Boolean testDirChange = mSingleLine && mLayout != NULL &&
        (alignment == Elastos::Droid::Text::ALIGN_NORMAL
        || alignment == Elastos::Droid::Text::ALIGN_OPPOSITE);
    Int32 oldDir = 0;
    if (testDirChange) mLayout->GetParagraphDirection(0, &oldDir);

    AutoPtr<IKeyListener> keyListener;
    GetKeyListener((IKeyListener**)&keyListener);
    Boolean shouldEllipsize = mEllipsize !=
        TextUtilsTruncateAt_NONE && keyListener == NULL;
    Boolean switchEllipsize = mEllipsize == TextUtilsTruncateAt_MARQUEE &&
        mMarqueeFadeMode != MARQUEE_FADE_NORMAL;
    TextUtilsTruncateAt effectiveEllipsize = mEllipsize;
    if (mEllipsize == TextUtilsTruncateAt_MARQUEE &&
        mMarqueeFadeMode == MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS) {
            effectiveEllipsize = TextUtilsTruncateAt_END_SMALL;
    }

    if (mTextDir == NULL) {
        mTextDir = GetTextDirectionHeuristic();
    }

    mLayout = MakeSingleLayout(wantWidth, boring, ellipsisWidth, alignment, shouldEllipsize,
        effectiveEllipsize, effectiveEllipsize == mEllipsize);
    if (switchEllipsize) {
        TextUtilsTruncateAt oppositeEllipsize = effectiveEllipsize ==
            TextUtilsTruncateAt_MARQUEE ?
            TextUtilsTruncateAt_END : TextUtilsTruncateAt_MARQUEE;
        mSavedMarqueeModeLayout = MakeSingleLayout(wantWidth, boring, ellipsisWidth, alignment,
            shouldEllipsize, oppositeEllipsize, effectiveEllipsize != mEllipsize);
    }

    shouldEllipsize = mEllipsize != TextUtilsTruncateAt_NONE;
    mHintLayout = NULL;

    if (mHint != NULL) {
        if (shouldEllipsize) hintWidth = wantWidth;
        if (hintBoring == UNKNOWN_BORING) {
            hintBoring = BoringLayout::IsBoring(mHint, mTextPaint, mTextDir, mHintBoring);

            if (hintBoring != NULL) {
                mHintBoring = hintBoring;
            }
        }

        if (hintBoring != NULL) {
            Int32 hbwidth;
            hintBoring->GetWidth(&hbwidth);
            if (hbwidth <= hintWidth &&
                (!shouldEllipsize || hbwidth <= ellipsisWidth)) {
                if (mSavedHintLayout != NULL) {
                    mHintLayout = NULL;
                    mSavedHintLayout->ReplaceOrMake(mHint, mTextPaint,
                            hintWidth, alignment, mSpacingMult, mSpacingAdd,
                            hintBoring, mIncludePad, (IBoringLayout**)&mHintLayout);
                }
                else {
                    mHintLayout = ILayout::Probe(BoringLayout::Make(mHint, mTextPaint,
                        hintWidth, alignment, mSpacingMult, mSpacingAdd,
                        hintBoring, mIncludePad));
                }

                mSavedHintLayout = (IBoringLayout*)mHintLayout.Get();
            }
            else if (shouldEllipsize && hbwidth <= hintWidth) {
                if (mSavedHintLayout != NULL) {
                    mHintLayout = NULL;
                    mSavedHintLayout->ReplaceOrMake(mHint, mTextPaint,
                            hintWidth, alignment, mSpacingMult, mSpacingAdd,
                            hintBoring, mIncludePad, mEllipsize,
                            ellipsisWidth, (IBoringLayout**)&mHintLayout);
                }
                else {
                    mHintLayout = ILayout::Probe(BoringLayout::Make(mHint, mTextPaint,
                        hintWidth, alignment, mSpacingMult, mSpacingAdd,
                        hintBoring, mIncludePad, mEllipsize,
                        ellipsisWidth));
                }
            }
            else if (shouldEllipsize) {
                Int32 length;
                mHint->GetLength(&length);
                ASSERT_SUCCEEDED(CStaticLayout::New(
                    mHint, 0, length, mTextPaint, hintWidth, alignment, mTextDir,
                    mSpacingMult, mSpacingAdd, mIncludePad, mEllipsize,
                    ellipsisWidth, mMaxMode == LINES ? mMaximum : Elastos::Core::Math::INT32_MAX_VALUE, (IStaticLayout**)&mHintLayout));
            }
            else {
                ASSERT_SUCCEEDED(CStaticLayout::New(
                    mHint, mTextPaint, hintWidth, alignment, mTextDir,mSpacingMult,
                    mSpacingAdd, mIncludePad, (IStaticLayout**)&mHintLayout));
            }
        }
        else if (shouldEllipsize) {
            Int32 length;
            mHint->GetLength(&length);
            ASSERT_SUCCEEDED(CStaticLayout::New(
                mHint, 0, length, mTextPaint, hintWidth, alignment,mTextDir,
                mSpacingMult, mSpacingAdd, mIncludePad, mEllipsize,
                ellipsisWidth, mMaxMode == LINES ? mMaximum : Elastos::Core::Math::INT32_MAX_VALUE, (IStaticLayout**)&mHintLayout));
        }
        else {
            ASSERT_SUCCEEDED(CStaticLayout::New(
                mHint, mTextPaint, hintWidth, alignment, mTextDir,mSpacingMult,
                mSpacingAdd, mIncludePad, (IStaticLayout**)&mHintLayout));
        }
    }

    Int32 paragraphDirection;
    mLayout->GetParagraphDirection(0, &paragraphDirection);
    if (bringIntoView || (testDirChange && oldDir != paragraphDirection)) {
        RegisterForPreDraw();
    }
    if (mEllipsize == TextUtilsTruncateAt_MARQUEE) {
        if (!CompressText(ellipsisWidth)) {
            Int32 height;
            mLayoutParams->GetHeight(&height);
            // If the size of the view does not depend on the size of the text, try to
            // start the marquee immediately
            if (height != IViewGroupLayoutParams::WRAP_CONTENT
                && height != IViewGroupLayoutParams::MATCH_PARENT) {
                StartMarquee();
            }
            else {
                // Defer the start of the marquee until we know our width (see setFrame())
                mRestartMarquee = TRUE;
            }
        }
    }

    // CursorControllers need a non-NULL mLayout
    assert(0);
    //if (mEditor != NULL) mEditor->PrepareCursorControllers();

}

AutoPtr<ILayout> TextView::MakeSingleLayout(
    /* [in] */ Int32 wantWidth,
    /* [in] */ IBoringLayoutMetrics* _boring,
    /* [in] */ Int32 ellipsisWidth,
    /* [in] */ LayoutAlignment alignment,
    /* [in] */ Boolean shouldEllipsize,
    /* [in] */ TextUtilsTruncateAt effectiveEllipsize,
    /* [in] */ Boolean useSaved)
{
    AutoPtr<IBoringLayoutMetrics> boring = _boring;
    AutoPtr<ILayout> result;
    if (ISpannable::Probe(mText)) {
        AutoPtr<IKeyListener> keyListener;
        GetKeyListener((IKeyListener**)&keyListener);
        ASSERT_SUCCEEDED(CDynamicLayout::New(mText, mTransformed, mTextPaint, wantWidth,
            alignment, mTextDir, mSpacingMult, mSpacingAdd, mIncludePad,
            keyListener == NULL ? effectiveEllipsize : TextUtilsTruncateAt_NONE,
            ellipsisWidth, (IDynamicLayout**)&result));
    }
    else {
        if (boring == UNKNOWN_BORING) {
            boring = BoringLayout::IsBoring(mTransformed, mTextPaint, mTextDir, mBoring);
            if (boring != NULL) {
                mBoring = boring;
            }
        }
        Int32 tlen;
        mTransformed->GetLength(&tlen);
        if (boring != NULL) {
            Int32 bwidth;
            boring->GetWidth(&bwidth);
            if (bwidth <= wantWidth &&
                (effectiveEllipsize == TextUtilsTruncateAt_NONE
                || bwidth <= ellipsisWidth)) {
                    if (useSaved && mSavedLayout != NULL) {
                        AutoPtr<IBoringLayout> temp;
                        mSavedLayout->ReplaceOrMake(mTransformed, mTextPaint,
                            wantWidth, alignment, mSpacingMult, mSpacingAdd,
                            boring, mIncludePad, (IBoringLayout**)&temp);
                        result = ILayout::Probe(temp);

                    } else {
                        result = ILayout::Probe(BoringLayout::Make(mTransformed, mTextPaint,
                            wantWidth, alignment, mSpacingMult, mSpacingAdd,
                            boring, mIncludePad));
                    }
                    if (useSaved) {
                        mSavedLayout = (IBoringLayout*) result.Get();
                    }
            } else if (shouldEllipsize && bwidth <= wantWidth) {
                if (useSaved && mSavedLayout != NULL) {
                    mSavedLayout->ReplaceOrMake(mTransformed, mTextPaint,
                        wantWidth, alignment, mSpacingMult, mSpacingAdd,
                        boring, mIncludePad, effectiveEllipsize,
                        ellipsisWidth, (IBoringLayout**)&result);
                } else {
                    result = ILayout::Probe(BoringLayout::Make(mTransformed, mTextPaint,
                        wantWidth, alignment, mSpacingMult, mSpacingAdd,
                        boring, mIncludePad, effectiveEllipsize,
                        ellipsisWidth));
                }
            } else if (shouldEllipsize) {
                ASSERT_SUCCEEDED(CStaticLayout::New(mTransformed,
                    0, tlen,
                    mTextPaint, wantWidth, alignment, mTextDir, mSpacingMult,
                    mSpacingAdd, mIncludePad, effectiveEllipsize,
                    ellipsisWidth, mMaxMode == LINES ? mMaximum : Elastos::Core::Math::INT32_MAX_VALUE,
                    (IStaticLayout**)&result));
            } else {
                ASSERT_SUCCEEDED(CStaticLayout::New(mTransformed, mTextPaint,
                    wantWidth, alignment, mTextDir, mSpacingMult, mSpacingAdd,
                    mIncludePad, (IStaticLayout**)&result));
            }
        } else if (shouldEllipsize) {
            ASSERT_SUCCEEDED(CStaticLayout::New(mTransformed,
                0, tlen,
                mTextPaint, wantWidth, alignment, mTextDir, mSpacingMult,
                mSpacingAdd, mIncludePad, effectiveEllipsize,
                ellipsisWidth, mMaxMode == LINES ? mMaximum : Elastos::Core::Math::INT32_MAX_VALUE,
                (IStaticLayout**)&result));
        } else {
            ASSERT_SUCCEEDED(CStaticLayout::New(mTransformed, mTextPaint,
                wantWidth, alignment, mTextDir, mSpacingMult, mSpacingAdd,
                mIncludePad, (IStaticLayout**)&result));
        }
    }
    return result;
}

Boolean TextView::CompressText(
    /* [in] */ Float width)
{
    Boolean isHardwareAccelerated;
    if (IsHardwareAccelerated(&isHardwareAccelerated), isHardwareAccelerated) return FALSE;

    // Only compress the text if it hasn't been compressed by the previous pass
    Float scaleX;
    IPaint::Probe(mTextPaint)->GetTextScaleX(&scaleX);
    Int32 linecount;
    if (width > 0.0f && mLayout != NULL && (GetLineCount(&linecount), linecount) == 1 && !mUserSetTextScaleX &&
            scaleX == 1.0f) {
        Float textWidth;
        mLayout->GetLineWidth(0, &textWidth);
        Float overflow = (textWidth + 1.0f - width) / width;
        if (overflow > 0.0f && overflow <= Marquee::MARQUEE_DELTA_MAX) {
            IPaint::Probe(mTextPaint)->SetTextScaleX(1.0f - overflow - 0.005f);
            AutoPtr<IRunnable> runnable = new CompressTextRunnable(this);
            Boolean isPost;
            Post(runnable, &isPost);
            return TRUE;
        }
    }

    return FALSE;
}

Int32 TextView::Desired(
    /* [in] */ ILayout* layout)
{
    Int32 N;
    layout->GetLineCount(&N);

    AutoPtr<ICharSequence> text;
    layout->GetText((ICharSequence**)&text);
    Float max = 0;

    // if any line was wrapped, we can't use it.
    // but it's ok for the last line not to have a newline
    for (Int32 i = 0; i < N - 1; i++) {
        Int32 offset;
        Char32 c;
        layout->GetLineEnd(i, &offset);
        text->GetCharAt(offset - 1, &c);
        if (c != '\n')
            return -1;
    }

    for (Int32 i = 0; i < N; i++) {
        Float extent;
        layout->GetLineWidth(i, &extent);
        max = Elastos::Core::Math::Max(max, extent);
    }

    return (Int32)Elastos::Core::Math::Ceil(max);
}

ECode TextView::SetIncludeFontPadding(
    /* [in] */ Boolean includepad)
{
    if (mIncludePad != includepad) {
        mIncludePad = includepad;

        if (mLayout != NULL) {
            NullLayouts();
            RequestLayout();
            Invalidate();
        }
    }
    return NOERROR;
}

ECode TextView::GetIncludeFontPadding(
    /* [out] */ Boolean* padding)
{
    VALIDATE_NOT_NULL(padding)
    *padding = mIncludePad;
    return NOERROR;
}

void TextView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 widthMode = View::MeasureSpec::GetMode(widthMeasureSpec);
    Int32 heightMode = View::MeasureSpec::GetMode(heightMeasureSpec);
    Int32 widthSize = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 heightSize = View::MeasureSpec::GetSize(heightMeasureSpec);
    Int32 width;
    Int32 height;

    AutoPtr<IBoringLayoutMetrics> boring = UNKNOWN_BORING;
    AutoPtr<IBoringLayoutMetrics> hintBoring = UNKNOWN_BORING;

    if (mTextDir == NULL) {
        mTextDir = GetTextDirectionHeuristic();
    }
    Int32 des = -1;
    Boolean fromexisting = FALSE;

    if (widthMode == View::MeasureSpec::EXACTLY) {
        // Parent has told us how big to be. So be it.
        width = widthSize;
    }
    else {
        if (mLayout != NULL && mEllipsize == TextUtilsTruncateAt_NONE) {
            des = Desired(mLayout);
        }

        if (des < 0) {
            boring = BoringLayout::IsBoring(mTransformed, mTextPaint, mTextDir, mBoring);
            if (boring != NULL) {
                mBoring = boring;
            }
        }
        else {
            fromexisting = TRUE;
        }

        if (boring == NULL || boring == UNKNOWN_BORING) {
            if (des < 0) {
                des = (Int32)Elastos::Core::Math::Ceil(Layout::GetDesiredWidth(mTransformed, mTextPaint));
            }

            width = des;
        }
        else {
            boring->GetWidth(&width);
        }

        AutoPtr<Drawables> dr = mDrawables;
        if (dr != NULL) {
            width = Elastos::Core::Math::Max(width, dr->mDrawableWidthTop);
            width = Elastos::Core::Math::Max(width, dr->mDrawableWidthBottom);
        }

        if (mHint != NULL) {
            Int32 hintDes = -1;
            Int32 hintWidth;
            if (mHintLayout != NULL && mEllipsize == TextUtilsTruncateAt_NONE) {
                hintDes = Desired(mHintLayout);
            }

            if (hintDes < 0) {
                hintBoring = BoringLayout::IsBoring(mHint, mTextPaint, mTextDir, mHintBoring);
                if (hintBoring != NULL) {
                    mHintBoring = hintBoring;
                }
            }

            if (hintBoring == NULL || hintBoring == UNKNOWN_BORING) {
                if (hintDes < 0) {
                    hintDes = (Int32)Elastos::Core::Math::Ceil(
                            Layout::GetDesiredWidth(mHint, mTextPaint));
                }

                hintWidth = hintDes;
            }
            else {
                hintBoring->GetWidth(&hintWidth);
            }

            if (hintWidth > width) {
                width = hintWidth;
            }
        }
        Int32 compoundPaddingLeft, compoundPaddingRight;
        GetCompoundPaddingLeft(&compoundPaddingLeft);
        GetCompoundPaddingRight(&compoundPaddingRight);
        width += compoundPaddingLeft + compoundPaddingRight;

        if (mMaxWidthMode == EMS) {
            Int32 lineHeight;
            GetLineHeight(&lineHeight);
            width = Elastos::Core::Math::Min(width, mMaxWidth * lineHeight);
        }
        else {
            width = Elastos::Core::Math::Min(width, mMaxWidth);
        }

        if (mMinWidthMode == EMS) {
            Int32 lineHeight;
            GetLineHeight(&lineHeight);
            width = Elastos::Core::Math::Max(width, mMinWidth * lineHeight);
        }
        else {
            width = Elastos::Core::Math::Max(width, mMinWidth);
        }

        // Check against our minimum width
        width = Elastos::Core::Math::Max(width, GetSuggestedMinimumWidth());

        if (widthMode == View::MeasureSpec::AT_MOST) {
            width = Elastos::Core::Math::Min(widthSize, width);
        }
    }
    Int32 compoundPaddingLeft, compoundPaddingRight;
    GetCompoundPaddingLeft(&compoundPaddingLeft);
    GetCompoundPaddingRight(&compoundPaddingRight);
    Int32 want = width - compoundPaddingLeft - compoundPaddingRight;
    Int32 unpaddedWidth = want;
    if (mHorizontallyScrolling) want = VERY_WIDE;
    Int32 hintWant = want;
    Int32 hintWidth = hintWant;
    if (mHintLayout != NULL) {
        mHintLayout->GetWidth(&hintWidth);
    }
    if (mLayout == NULL) {
        Int32 compoundPaddingLeft, compoundPaddingRight;
        GetCompoundPaddingLeft(&compoundPaddingLeft);
        GetCompoundPaddingRight(&compoundPaddingRight);
        MakeNewLayout(want, hintWant, boring, hintBoring,
            width - compoundPaddingLeft - compoundPaddingRight, FALSE);
    }
    else {
        Int32 layoutWidth, layoutEllipsizedWidth;
        mLayout->GetWidth(&layoutWidth);
        mLayout->GetEllipsizedWidth(&layoutEllipsizedWidth);
        Int32 compoundPaddingLeft, compoundPaddingRight;
        GetCompoundPaddingLeft(&compoundPaddingLeft);
        GetCompoundPaddingRight(&compoundPaddingRight);
        Boolean layoutChanged = (layoutWidth != want) || (hintWidth != hintWant) ||
            (layoutEllipsizedWidth != width - compoundPaddingLeft - compoundPaddingRight);

        Boolean widthChanged = (mHint == NULL) &&
            (mEllipsize == TextUtilsTruncateAt_NONE) &&
            (want > layoutWidth) &&
            (IBoringLayout::Probe(mLayout) || (fromexisting && des >= 0 && des <= want));
        Boolean maximumChanged = (mMaxMode != mOldMaxMode) || (mMaximum != mOldMaximum);
        if (layoutChanged || maximumChanged) {
            if (!maximumChanged && widthChanged) {
                mLayout->IncreaseWidthTo(want);
            } else {
                Int32 compoundPaddingLeft, compoundPaddingRight;
                GetCompoundPaddingLeft(&compoundPaddingLeft);
                GetCompoundPaddingRight(&compoundPaddingRight);
                MakeNewLayout(want, hintWant, boring, hintBoring,
                    width - compoundPaddingLeft - compoundPaddingRight, FALSE);
            }
        } else {
            // Nothing has changed
        }
    }
    if (heightMode == View::MeasureSpec::EXACTLY) {
        // Parent has told us how big to be. So be it.
        height = heightSize;
        mDesiredHeightAtMeasure = -1;
    }
    else {
        Int32 desired = GetDesiredHeight();

        height = desired;
        mDesiredHeightAtMeasure = desired;

        if (heightMode == View::MeasureSpec::AT_MOST) {
            height = Elastos::Core::Math::Min(desired, heightSize);
        }
    }

    if (mAttachInfo != NULL) {
        Int32 count;
        mLayout->GetLineCount(&count);
        Int32 compoundPaddingTop, compoundPaddingBottom;
        GetCompoundPaddingTop(&compoundPaddingTop);
        GetCompoundPaddingBottom(&compoundPaddingBottom);
        Int32 unpaddedHeight = height - compoundPaddingTop - compoundPaddingBottom;
        if (mMaxMode == LINES && count > mMaximum) {
            Int32 pos;
            mLayout->GetLineTop(mMaximum, &pos);
            unpaddedHeight = Elastos::Core::Math::Min(unpaddedHeight, pos);
        }
        /*
         * We didn't let MakeNewLayout() register to bring the cursor into view,
         * so do it here if there is any possibility that it is needed.
         */
        Int32 w, h;
        if (mMovement != NULL ||
            (mLayout->GetWidth(&w), w) > unpaddedWidth ||
            (mLayout->GetHeight(&h), h) > unpaddedHeight) {
            RegisterForPreDraw();
        }
        else {
            ScrollTo(0, 0);
        }
    }
    SetMeasuredDimension(width, height);
}

Int32 TextView::GetDesiredHeight()
{
    return Elastos::Core::Math::Max(
            GetDesiredHeight(mLayout, TRUE),
            GetDesiredHeight(mHintLayout, mEllipsize != TextUtilsTruncateAt_NONE));
}

Int32 TextView::GetDesiredHeight(
    /* [in] */ ILayout* layout,
    /* [in] */ Boolean cap)
{
    if (layout == NULL) {
        return 0;
    }

    Int32 linecount, pad, desired;
    layout->GetLineCount(&linecount);
    GetCompoundPaddingTop(&pad);
    Int32 compoundPaddingBottom;
    GetCompoundPaddingBottom(&compoundPaddingBottom);
    pad += compoundPaddingBottom;
    layout->GetLineTop(linecount, &desired);

    AutoPtr<Drawables> dr = mDrawables;
    if (dr != NULL) {
        desired = Elastos::Core::Math::Max(desired, dr->mDrawableHeightLeft);
        desired = Elastos::Core::Math::Max(desired, dr->mDrawableHeightRight);
    }

    desired += pad;

    if (mMaxMode == LINES) {
        /*
         * Don't cap the hint to a certain number of lines.
         * (Do cap it, though, if we have a maximum pixel height.)
         */
        if (cap) {
            if (linecount > mMaximum) {
                layout->GetLineTop(mMaximum, &desired);

                if (dr != NULL) {
                    desired = Elastos::Core::Math::Max(desired, dr->mDrawableHeightLeft);
                    desired = Elastos::Core::Math::Max(desired, dr->mDrawableHeightRight);
                }

                desired += pad;
                linecount = mMaximum;
            }
        }
    }
    else {
        desired = Elastos::Core::Math::Min(desired, mMaximum);
    }

    if (mMinMode == LINES) {
        if (linecount < mMinimum) {
            Int32 lineHeight;
            GetLineHeight(&lineHeight);
            desired += lineHeight * (mMinimum - linecount);
        }
    }
    else {
        desired = Elastos::Core::Math::Max(desired, mMinimum);
    }

    // Check against our minimum height
    desired = Elastos::Core::Math::Max(desired, GetSuggestedMinimumHeight());

    return desired;
}

void TextView::CheckForResize()
{
    Boolean sizeChanged = FALSE;

    if (mLayout != NULL) {
        Int32 lw, lh;
        mLayoutParams->GetWidth(&lw);
        mLayoutParams->GetHeight(&lh);

        // Check if our width changed
        if (lw == IViewGroupLayoutParams::WRAP_CONTENT) {
            sizeChanged = TRUE;
            Invalidate();
        }

        // Check if our height changed
        if (lh == IViewGroupLayoutParams::WRAP_CONTENT) {
            Int32 desiredHeight = GetDesiredHeight();
            Int32 height;
            if (desiredHeight != (GetHeight(&height), height)) {
                sizeChanged = TRUE;
            }
        }
        else if (lh == IViewGroupLayoutParams::MATCH_PARENT) {
            if (mDesiredHeightAtMeasure >= 0) {
                Int32 desiredHeight = GetDesiredHeight();

                if (desiredHeight != mDesiredHeightAtMeasure) {
                    sizeChanged = TRUE;
                }
            }
        }
    }

    if (sizeChanged) {
        RequestLayout();
        // caller will have already invalidated
    }
}

void TextView::CheckForRelayout()
{
    // If we have a fixed width, we can just swap in a new text layout
    // if the text height stays the same or if the view height is fixed.

    Int32 lw, lh;
    mLayoutParams->GetWidth(&lw);
    mLayoutParams->GetHeight(&lh);
    Int32 compoundPaddingLeft, compoundPaddingRight;
    if ((lw != IViewGroupLayoutParams::WRAP_CONTENT ||
        (mMaxWidthMode == mMinWidthMode && mMaxWidth == mMinWidth)) &&
        (mHint == NULL || mHintLayout != NULL) &&
        (mRight - mLeft - (GetCompoundPaddingLeft(&compoundPaddingLeft), compoundPaddingLeft)
        - (GetCompoundPaddingRight(&compoundPaddingRight), compoundPaddingRight) > 0)) {
        // Static width, so try making a new text layout.

        Int32 oldht;
        mLayout->GetHeight(&oldht);
        Int32 want;
        mLayout->GetWidth(&want);
        Int32 hintWant = 0;
        if (mHintLayout != NULL) {
            mHintLayout->GetWidth(&hintWant);
        }

        /*
         * No need to bring the text into view, since the size is not
         * changing (unless we do the RequestLayout(), in which case it
         * will happen at measure).
         */
        Int32 compoundPaddingLeft, compoundPaddingRight;
        GetCompoundPaddingLeft(&compoundPaddingLeft);
        GetCompoundPaddingRight(&compoundPaddingRight);
        MakeNewLayout(
            want, hintWant, UNKNOWN_BORING, UNKNOWN_BORING,
            mRight - mLeft - compoundPaddingLeft - compoundPaddingRight,
            FALSE);

        if (mEllipsize != TextUtilsTruncateAt_MARQUEE) {
            // In a fixed-height view, so use our new text layout.
            if (lh != IViewGroupLayoutParams::WRAP_CONTENT
                    && lh != IViewGroupLayoutParams::MATCH_PARENT) {
                Invalidate();
                return;
            }

            // Dynamic height, but height has stayed the same,
            // so use our new text layout.
            Int32 ht1, ht2;
            mLayout->GetHeight(&ht1);
            if (mHintLayout != NULL) {
                mHintLayout->GetHeight(&ht2);
            }

            if (ht1 == oldht && (mHintLayout == NULL || ht2 == oldht)) {
                Invalidate();
                return;
            }
        }

        // We lose: the height has changed and we have a dynamic height.
        // Request a new view layout using our new text layout.
        RequestLayout();
        Invalidate();
    }
    else {
        // Dynamic width, so we have no choice but to request a new
        // view layout with a new text layout.

        NullLayouts();
        RequestLayout();
        Invalidate();
    }
}

ECode TextView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    View::OnLayout(changed, left, top, right, bottom);
    if (mDeferScroll >= 0) {
        Int32 curs = mDeferScroll;
        mDeferScroll = -1;
        Int32 length;
        mText->GetLength(&length);
        Boolean bringPointIntoView;
        BringPointIntoView(Elastos::Core::Math::Min(curs, length), &bringPointIntoView);
    }

    return NOERROR;
}

Boolean TextView::IsShowingHint()
{
    return TextUtils::IsEmpty(mText) && !TextUtils::IsEmpty(mHint);
}

Boolean TextView::BringTextIntoView()
{
    AutoPtr<ILayout> layout = IsShowingHint() ? mHintLayout : mLayout;

    Int32 line = 0;
    if ((mGravity & IGravity::VERTICAL_GRAVITY_MASK) == IGravity::BOTTOM) {
        layout->GetLineCount(&line);
        line--;
    }

    LayoutAlignment a;
    layout->GetParagraphAlignment(line, &a);
    Int32 dir, compoundPaddingLeft, compoundPaddingRight, extendedPaddingTop, extendedPaddingBottom;
    layout->GetParagraphDirection(line, &dir);
    GetCompoundPaddingLeft(&compoundPaddingLeft);
    GetCompoundPaddingRight(&compoundPaddingRight);
    GetExtendedPaddingTop(&extendedPaddingTop);
    GetExtendedPaddingBottom(&extendedPaddingBottom);
    Int32 hspace = mRight - mLeft - compoundPaddingLeft - compoundPaddingRight;
    Int32 vspace = mBottom - mTop - extendedPaddingTop - extendedPaddingBottom;
    Int32 ht;
    layout->GetHeight(&ht);

    Int32 scrollx, scrolly;

    // Convert to left, center, or right alignment.
    if (a == Elastos::Droid::Text::ALIGN_NORMAL) {
        a = dir == ILayout::DIR_LEFT_TO_RIGHT ?
            Elastos::Droid::Text::ALIGN_LEFT : Elastos::Droid::Text::ALIGN_RIGHT;
    } else if (a == Elastos::Droid::Text::ALIGN_OPPOSITE){
        a = dir == ILayout::DIR_LEFT_TO_RIGHT ?
            Elastos::Droid::Text::ALIGN_RIGHT : Elastos::Droid::Text::ALIGN_LEFT;
    }

    if (a == Elastos::Droid::Text::ALIGN_CENTER) {
        /*
         * Keep centered if possible, or, if it is too wide to fit,
         * keep leading edge in view.
         */

        Float fLeft, fRight;
        layout->GetLineLeft(line, &fLeft);
        layout->GetLineRight(line, &fRight);
        Int32 left = (Int32)Elastos::Core::Math::Floor(fLeft);
        Int32 right = (Int32)Elastos::Core::Math::Ceil(fRight);

        if (right - left < hspace) {
            scrollx = (right + left) / 2 - hspace / 2;
        }
        else {
            if (dir < 0) {
                scrollx = right - hspace;
            }
            else {
                scrollx = left;
            }
        }
    }
    else if (a == Elastos::Droid::Text::ALIGN_RIGHT) {
        Float lr;
        layout->GetLineRight(line, &lr);
        Int32 right = (Int32) Elastos::Core::Math::Ceil(lr);
        scrollx = right - hspace;
    } else { // a == Layout.Alignment.ALIGN_LEFT (will also be the default)
        Float ll;
        layout->GetLineLeft(line, &ll);
        scrollx = (Int32) Elastos::Core::Math::Floor(ll);
    }

    if (ht < vspace) {
        scrolly = 0;
    }
    else {
        if ((mGravity & IGravity::VERTICAL_GRAVITY_MASK) == IGravity::BOTTOM) {
            scrolly = ht - vspace;
        }
        else {
            scrolly = 0;
        }
    }

    if (scrollx != mScrollX || scrolly != mScrollY) {
        ScrollTo(scrollx, scrolly);
        return TRUE;
    }
    else {
        return FALSE;
    }
    return FALSE;
}

CARAPI TextView::BringPointIntoView(
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean isLayoutRequested;
    if (IsLayoutRequested(&isLayoutRequested), isLayoutRequested) {
        mDeferScroll = offset;
        *result = FALSE;
        return NOERROR;
    }

    Boolean changed = FALSE;

    AutoPtr<ILayout> layout = IsShowingHint() ? mHintLayout: mLayout;

    if (layout == NULL) {
        *result = changed;
        return NOERROR;
    }

    Int32 line = 0;
    layout->GetLineForOffset(offset, &line);

    Int32 grav;

    LayoutAlignment alignment;
    layout->GetParagraphAlignment(line, &alignment);
    switch (alignment) {
        case Elastos::Droid::Text::ALIGN_LEFT:
            grav = 1;
            break;

        case Elastos::Droid::Text::ALIGN_RIGHT:
            grav = -1;
            break;
        case Elastos::Droid::Text::ALIGN_NORMAL:
            layout->GetParagraphDirection(line, &grav);
            break;
        case Elastos::Droid::Text::ALIGN_OPPOSITE:
            layout->GetParagraphDirection(line, &grav);
            grav = -grav;
            break;
        case Elastos::Droid::Text::ALIGN_CENTER:
        default:
            grav = 0;
            break;
    }

    // We only want to clamp the cursor to fit within the layout width
    // in left-to-right modes, because in a right to left alignment,
    // we want to scroll to keep the line-right on the screen, as other
    // lines are likely to have text flush with the right margin, which
    // we want to keep visible.
    // A better long-term solution would probably be to measure both
    // the full line and a blank-trimmed version, and, for example, use
    // the latter measurement for centering and right alignment, but for
    // the time being we only implement the cursor clamping in left to
    // right where it is most likely to be annoying.
    Boolean clamped = grav > 0;
    // FIXME: Is it okay to truncate this, or should we round?
    Float tempX;
    layout->GetPrimaryHorizontal(offset, clamped, &tempX);
    Int32 x = tempX;
    Int32 top, bottom;
    layout->GetLineTop(line, &top);
    layout->GetLineTop(line + 1, &bottom);

    Float lineLeft, lineRight;
    layout->GetLineLeft(line, &lineLeft);
    layout->GetLineRight(line, &lineRight);
    Int32 left = (Int32) Elastos::Core::Math::Floor(lineLeft);
    Int32 right = (Int32) Elastos::Core::Math::Ceil(lineRight);
    Int32 ht, compoundPaddingLeft, compoundPaddingRight, extendedPaddingTop, extendedPaddingBottom;
    layout->GetHeight(&ht);
    GetCompoundPaddingLeft(&compoundPaddingLeft);
    GetCompoundPaddingRight(&compoundPaddingRight);
    GetExtendedPaddingTop(&extendedPaddingTop);
    GetExtendedPaddingBottom(&extendedPaddingBottom);
    Int32 hspace = mRight - mLeft - compoundPaddingLeft - compoundPaddingRight;
    Int32 vspace = mBottom - mTop - extendedPaddingTop - extendedPaddingBottom;

    if (!mHorizontallyScrolling && right - left > hspace && right > x) {
        // If cursor has been clamped, make sure we don't scroll.
        right = Elastos::Core::Math::Max(x, left + hspace);
    }

    Int32 hslack = (bottom - top) / 2;
    Int32 vslack = hslack;

    if (vslack > vspace / 4)
        vslack = vspace / 4;
    if (hslack > hspace / 4)
        hslack = hspace / 4;

    Int32 hs = mScrollX;
    Int32 vs = mScrollY;

    if (top - vs < vslack)
        vs = top - vslack;
    if (bottom - vs > vspace - vslack)
        vs = bottom - (vspace - vslack);
    if (ht - vs < vspace)
        vs = ht - vspace;
    if (0 - vs > 0)
        vs = 0;

    if (grav != 0) {
        if (x - hs < hslack) {
            hs = x - hslack;
        }
        if (x - hs > hspace - hslack) {
            hs = x - (hspace - hslack);
        }
    }

    if (grav < 0) {
        if (left - hs > 0)
            hs = left;
        if (right - hs < hspace)
            hs = right - hspace;
    }
    else if (grav > 0) {
        if (right - hs < hspace)
            hs = right - hspace;
        if (left - hs > 0)
            hs = left;
    }
    else /* grav == 0 */ {
        if (right - left <= hspace) {
            /*
             * If the entire text fits, center it exactly.
             */
            hs = left - (hspace - (right - left)) / 2;
        }
        else if (x > right - hslack) {
            /*
             * If we are near the right edge, keep the right edge
             * at the edge of the view.
             */
            hs = right - hspace;
        }
        else if (x < left + hslack) {
            /*
             * If we are near the left edge, keep the left edge
             * at the edge of the view.
             */
            hs = left;
        }
        else if (left > hs) {
            /*
             * Is there whitespace visible at the left?  Fix it if so.
             */
            hs = left;
        }
        else if (right < hs + hspace) {
            /*
             * Is there whitespace visible at the right?  Fix it if so.
             */
            hs = right - hspace;
        }
        else {
            /*
             * Otherwise, Float as needed.
             */
            if (x - hs < hslack) {
                hs = x - hslack;
            }
            if (x - hs > hspace - hslack) {
                hs = x - (hspace - hslack);
            }
        }
    }

    if (hs != mScrollX || vs != mScrollY) {
        if (mScroller == NULL) {
            ScrollTo(hs, vs);
        }
        else {
            Int64 duration = SystemClock::GetUptimeMillis() - mLastScroll;//TODO AnimationUtils::CurrentAnimationTimeMillis() - mLastScroll;
            Int32 dx = hs - mScrollX;
            Int32 dy = vs - mScrollY;

            if (duration > ANIMATED_SCROLL_GAP) {
                mScroller->StartScroll(mScrollX, mScrollY, dx, dy);
                Int32 du;
                mScroller->GetDuration(&du);
                AwakenScrollBars(du);
                Invalidate();
            }
            else {
                Boolean finished;
                if (mScroller->IsFinished(&finished), !finished) {
                    mScroller->AbortAnimation();
                }

                ScrollBy(dx, dy);
            }

            mLastScroll = SystemClock::GetUptimeMillis();//TODO AnimationUtils::CurrentAnimationTimeMillis();
        }

        changed = TRUE;
    }
    Boolean isFocused;
    if (IsFocused(&isFocused), isFocused) {
        // This offsets because GetInterestingRect() is in terms of viewport coordinates, but
        // requestRectangleOnScreen() is in terms of content coordinates.

        // The offsets here are to ensure the rectangle we are using is
        // within our view bounds, in case the cursor is on the far left
        // or right.  If it isn't withing the bounds, then this request
        // will be ignored.
        if (mTempRect == NULL) {
            CRect::New((IRect**)&mTempRect);
        }
        mTempRect->Set(x - 2, top, x + 2, bottom);
        GetInterestingRect(mTempRect, line);
        mTempRect->Offset(mScrollX, mScrollY);
        Boolean requestRectangleOnScreen;
        if (RequestRectangleOnScreen(mTempRect, &requestRectangleOnScreen)) {
            changed = TRUE;
        }
    }

    *result = changed;
    return NOERROR;
}

ECode TextView::MoveCursorToVisibleOffset(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (!(ISpannable::Probe(mText))) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 start, end;
    GetSelectionStart(&start);
    GetSelectionEnd(&end);
    if (start != end) {
        *result = FALSE;
        return NOERROR;
    }

    //First: make sure the line is visible on screen:

    Int32 line;
    mLayout->GetLineForOffset(start, &line);

    Int32 top, bottom, extendedPaddingTop, extendedPaddingBottom;
    mLayout->GetLineTop(line, &top);
    mLayout->GetLineTop(line + 1, &bottom);
    GetExtendedPaddingTop(&extendedPaddingTop);
    GetExtendedPaddingBottom(&extendedPaddingBottom);
    Int32 vspace = mBottom - mTop - extendedPaddingTop - extendedPaddingBottom;
    Int32 vslack = (bottom - top) / 2;
    if (vslack > vspace / 4)
        vslack = vspace / 4;
    Int32 vs = mScrollY;

    if (top < (vs+vslack)) {
        mLayout->GetLineForVertical(vs+vslack+(bottom-top), &line);
    }
    else if (bottom > (vspace+vs-vslack)) {
        mLayout->GetLineForVertical(vspace+vs-vslack-(bottom-top), &line);
    }

    //Next: make sure the character is visible on screen:
    Int32 compoundPaddingLeft, compoundPaddingRight;
    GetCompoundPaddingLeft(&compoundPaddingLeft);
    GetCompoundPaddingRight(&compoundPaddingRight);
    Int32 hspace = mRight - mLeft - compoundPaddingLeft - compoundPaddingRight;
    Int32 hs = mScrollX;
    Int32 leftChar;
    mLayout->GetOffsetForHorizontal(line, hs, &leftChar);
    Int32 rightChar;
    mLayout->GetOffsetForHorizontal(line, hspace+hs, &rightChar);

    // line might contain bidirectional text
    Int32 lowChar = leftChar < rightChar ? leftChar : rightChar;
    Int32 highChar = leftChar > rightChar ? leftChar : rightChar;

    Int32 newStart = start;
    if (newStart < lowChar) {
        newStart = lowChar;
    } else if (newStart > highChar) {
        newStart = highChar;
    }

    if (newStart != start) {
        Selection::SetSelection(ISpannable::Probe(mText), newStart);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode TextView::ComputeScroll()
{
    if (mScroller != NULL) {
        Boolean result;
        if (mScroller->ComputeScrollOffset(&result), result) {
            mScroller->GetCurrX(&mScrollX);
            mScroller->GetCurrY(&mScrollY);
            InvalidateParentCaches();
            PostInvalidate();  // So we draw again
        }
    }
    return NOERROR;
}

void TextView::GetInterestingRect(
    /* [in] */ IRect* r,
    /* [in] */ Int32 line)
{
    ConvertFromViewportToContentCoordinates(r);

    // Rectangle can can be expanded on first and last line to take
    // padding into account.
    // TODO Take left/right padding into account too?
    Int32 count, extendedPaddingTop, extendedPaddingBottom;
    mLayout->GetLineCount(&count);
    GetExtendedPaddingTop(&extendedPaddingTop);
    GetExtendedPaddingBottom(&extendedPaddingBottom);
    if (line == 0) ((CRect*)r)->mTop -= extendedPaddingTop;
    if (line == count - 1) ((CRect*)r)->mBottom += extendedPaddingBottom;
}

void TextView::ConvertFromViewportToContentCoordinates(
    /* [in] */ IRect* r)
{
    assert(r != NULL);
    Int32 horizontalOffset = ViewportToContentHorizontalOffset();
    ((CRect*)r)->mLeft += horizontalOffset;
    ((CRect*)r)->mRight += horizontalOffset;

    Int32 verticalOffset = ViewportToContentVerticalOffset();
    ((CRect*)r)->mTop += verticalOffset;
    ((CRect*)r)->mBottom += verticalOffset;
}

Int32 TextView::ViewportToContentHorizontalOffset()
{
    Int32 compoundPaddingLeft;
    GetCompoundPaddingLeft(&compoundPaddingLeft);
    return compoundPaddingLeft - mScrollX;
}

Int32 TextView::ViewportToContentVerticalOffset()
{
    Int32 extendedPaddingTop;
    GetExtendedPaddingTop(&extendedPaddingTop);
    Int32 offset = extendedPaddingTop - mScrollY;
    if ((mGravity & IGravity::VERTICAL_GRAVITY_MASK) != IGravity::TOP) {
        offset += GetVerticalOffset(FALSE);
    }
    return offset;
}

ECode TextView::Debug(
    /* [in] */ Int32 depth)
{
    // TODO:
    /*View::debug(depth);

    String output = debugIndent(depth);
    output += "frame={" + mLeft + ", " + mTop + ", " + mRight
            + ", " + mBottom + "} scroll={" + mScrollX + ", " + mScrollY
            + "} ";

    if (mText != NULL) {

        output += "mText=\"" + mText + "\" ";
        if (mLayout != NULL) {
            output += "mLayout width=" + mLayout->GetWidth()
                    + " height=" + mLayout->getHeight();
        }
    } else {
        output += "mText=NULL";
    }
    Log::d(VIEW_LOG_TAG, output);
    return NOERROR;*/
    return NOERROR;
}

ECode TextView::GetSelectionStart(
    /* [out] */ Int32* start)
{
    VALIDATE_NOT_NULL(start)
    AutoPtr<ICharSequence> text;
    GetText((ICharSequence**)&text);
    *start = Selection::GetSelectionStart(ICharSequence::Probe(text));
    return NOERROR;
}

ECode TextView::GetSelectionEnd(
    /* [out] */ Int32* end)
{
    VALIDATE_NOT_NULL(end)
    AutoPtr<ICharSequence> text;
    GetText((ICharSequence**)&text);
    *end = Selection::GetSelectionEnd(ICharSequence::Probe(text));
    return NOERROR;
}

ECode TextView::HasSelection(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 selectionStart, selectionEnd;
    GetSelectionStart(&selectionStart);
    GetSelectionEnd(&selectionEnd);
    *result = selectionStart >= 0 && selectionStart != selectionEnd;
    return NOERROR;
}

ECode TextView::SetSingleLine()
{
    return SetSingleLine(TRUE);
}

ECode TextView::SetAllCaps(
    /* [in] */ Boolean allCaps)
{
    if (allCaps) {
        AutoPtr<IContext> c;
        GetContext((IContext**)&c);
        AutoPtr<IAllCapsTransformationMethod> method;
//TODO        CAllCapsTransformationMethod::New(c, &method);
        assert(0 && "TODO");
        SetTransformationMethod(ITransformationMethod::Probe(method));
    } else {
        SetTransformationMethod(NULL);
    }
    return NOERROR;
}

ECode TextView::SetSingleLine(
    /* [in] */ Boolean singleLine)
{
    // Could be used, but may break backward compatibility.
    // if (mSingleLine == singleLine) return;
    SetInputTypeSingleLine(singleLine);
    return ApplySingleLine(singleLine, TRUE, TRUE);
}

void TextView::SetInputTypeSingleLine(
    /* [in] */ Boolean singleLine)
{
    assert(0);
    /*if (mEditor != NULL &&
            (mEditor->mInputType & IInputType::TYPE_MASK_CLASS) == IInputType::TYPE_CLASS_TEXT) {
        if (singleLine) {
            mEditor->mInputType &= ~IInputType::TYPE_TEXT_FLAG_MULTI_LINE;
        } else {
            mEditor->mInputType |= IInputType::TYPE_TEXT_FLAG_MULTI_LINE;
        }
    }*/
}

ECode TextView::ApplySingleLine(
    /* [in] */ Boolean singleLine,
    /* [in] */ Boolean applyTransformation,
    /* [in] */ Boolean changeMaxLines)
{
    mSingleLine = singleLine;
    if (singleLine) {
        SetLines(1);
        SetHorizontallyScrolling(TRUE);
        if (applyTransformation) {
            AutoPtr<ISingleLineTransformationMethodHelper> helper;
            CSingleLineTransformationMethodHelper::AcquireSingleton((ISingleLineTransformationMethodHelper**)&helper);
            AutoPtr<ISingleLineTransformationMethod> method;
            helper->GetInstance((ISingleLineTransformationMethod**)&method);
            SetTransformationMethod(ITransformationMethod::Probe(method));
        }
    }
    else {
        if (changeMaxLines) {
            SetMaxLines(Elastos::Core::Math::INT32_MAX_VALUE);
        }
        SetHorizontallyScrolling(FALSE);
        if (applyTransformation) {
            SetTransformationMethod(NULL);
        }
    }

    return NOERROR;
}

ECode TextView::SetEllipsize(
    /* [in] */ TextUtilsTruncateAt where)
{
    // TruncateAt is an enum. != comparison is ok between these singleton objects.
    if (mEllipsize != where) {
        mEllipsize = where;

        if (mLayout != NULL) {
            NullLayouts();
            RequestLayout();
            Invalidate();
        }
    }
    return NOERROR;
}

ECode TextView::SetMarqueeRepeatLimit(
    /* [in] */ Int32 marqueeLimit)
{
    mMarqueeRepeatLimit = marqueeLimit;
    return NOERROR;
}

ECode TextView::GetMarqueeRepeatLimit(
    /* [out] */ Int32* marqueeLimit)
{
    VALIDATE_NOT_NULL(marqueeLimit)
    *marqueeLimit = mMarqueeRepeatLimit;
    return NOERROR;
}

ECode TextView::GetEllipsize(
    /* [out] */ TextUtilsTruncateAt* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mEllipsize;
    return NOERROR;
}

ECode TextView::SetSelectAllOnFocus(
    /* [in] */ Boolean selectAllOnFocus)
{
    CreateEditorIfNeeded();
    assert(0);
    //mEditor->mSelectAllOnFocus = selectAllOnFocus;

    if (selectAllOnFocus && !(ISpannable::Probe(mText))) {
        SetText(mText, BufferType_SPANNABLE);
    }
    return NOERROR;
}

ECode TextView::SetCursorVisible(
    /* [in] */ Boolean visible)
{
    assert(0);
    /*if (visible && mEditor == NULL) return NOERROR; // visible is the default value with no edit data
    CreateEditorIfNeeded();
    if (mEditor->mCursorVisible != visible) {
        mEditor->mCursorVisible = visible;
        Invalidate();

        mEditor->MakeBlink();

        // InsertionPointCursorController depends on mCursorVisible
        mEditor->PrepareCursorControllers();
    }*/
    return NOERROR;
}

ECode TextView::IsCursorVisible(
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible)
    assert(0);
    //*visible = mEditor == NULL ? TRUE : mEditor->mCursorVisible;
    return NOERROR;
}

Boolean TextView::CanMarquee()
{
    Int32 compoundPaddingLeft, compoundPaddingRight;
    GetCompoundPaddingLeft(&compoundPaddingLeft);
    GetCompoundPaddingRight(&compoundPaddingRight);
    Int32 width = (mRight - mLeft - compoundPaddingLeft - compoundPaddingRight);
    Float lw, mlw;
    mLayout->GetLineWidth(0, &lw);
    return width > 0 && (lw > width ||
        (mMarqueeFadeMode != MARQUEE_FADE_NORMAL && mSavedMarqueeModeLayout != NULL &&
        (mSavedMarqueeModeLayout->GetLineWidth(0, &mlw), mlw) > width));
}

void TextView::StartMarquee()
{
    // Do not ellipsize EditText
    AutoPtr<IKeyListener> keyListener;
    GetKeyListener((IKeyListener**)&keyListener);
    if (keyListener != NULL) return;

    Int32 width, compoundPaddingLeft, compoundPaddingRight;
    GetWidth(&width);
    GetCompoundPaddingLeft(&compoundPaddingLeft);
    GetCompoundPaddingRight(&compoundPaddingRight);
    if (CompressText(width - compoundPaddingLeft - compoundPaddingRight)) {
        return;
    }
    Boolean isFocused, isSelected;
    Int32 linecount;
    if ((mMarquee == NULL || mMarquee->IsStopped())
        && ((IsFocused(&isFocused), isFocused)
        || (IsSelected(&isSelected), isSelected)) &&
        (GetLineCount(&linecount), linecount) == 1 && CanMarquee()) {

        if (mMarqueeFadeMode == MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS) {
            mMarqueeFadeMode = MARQUEE_FADE_SWITCH_SHOW_FADE;
            AutoPtr<ILayout> tmp = mLayout;
            mLayout = mSavedMarqueeModeLayout;
            mSavedMarqueeModeLayout = tmp;
            SetHorizontalFadingEdgeEnabled(TRUE);
            RequestLayout();
            Invalidate();
        }

        if (mMarquee == NULL) mMarquee = new Marquee(this);
        mMarquee->Start(mMarqueeRepeatLimit);
    }
}

void TextView::StopMarquee()
{
    if (mMarquee != NULL && !mMarquee->IsStopped()) {
        mMarquee->Stop();
    }
    if (mMarqueeFadeMode == MARQUEE_FADE_SWITCH_SHOW_FADE) {
        mMarqueeFadeMode = MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS;
        AutoPtr<ILayout> tmp = mSavedMarqueeModeLayout;
        mSavedMarqueeModeLayout = mLayout;
        mLayout = tmp;
        SetHorizontalFadingEdgeEnabled(FALSE);
        RequestLayout();
        Invalidate();
    }
}

void TextView::StartStopMarquee(
    /* [in] */ Boolean start)
{
    if (mEllipsize == TextUtilsTruncateAt_MARQUEE) {
        if (start) {
            StartMarquee();
        } else {
            StopMarquee();
        }
    }
}

void TextView::OnTextChanged(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 after)
{
    // intentionally empty, template pattern method can be overridden by subclasses
}

void TextView::OnSelectionChanged(
    /* [in] */ Int32 selStart,
    /* [in] */ Int32 selEnd)
{
    SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_TEXT_SELECTION_CHANGED);
}

ECode TextView::AddTextChangedListener(
    /* [in] */ ITextWatcher* watcher)
{
    mListeners.PushBack(watcher);
    return NOERROR;
}

ECode TextView::RemoveTextChangedListener(
    /* [in] */ ITextWatcher* watcher)
{
    AutoPtr<ITextWatcher> temp = watcher;
    List<AutoPtr<ITextWatcher> >::Iterator iter = Find(mListeners.Begin(), mListeners.End(), temp);
    if (iter != mListeners.End()) {
        mListeners.Erase(iter);
    }

    return NOERROR;
}

void TextView::SendBeforeTextChanged(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 after)
{
    List<AutoPtr<ITextWatcher> >::Iterator iter = mListeners.Begin();
    for (; iter != mListeners.End(); ++iter) {
        (*iter)->BeforeTextChanged(text, start, before, after);
    }

    // The spans that are inside or intersect the modified region no longer make sense
    RemoveIntersectingNonAdjacentSpans(start, start + before, EIID_ISpellCheckSpan);
    RemoveIntersectingNonAdjacentSpans(start, start + before, EIID_ISuggestionSpan);
}

// Removes all spans that are inside or actually overlap the start..end range
void TextView::RemoveIntersectingNonAdjacentSpans(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ const InterfaceID& type)
{
    AutoPtr<IEditable> text = IEditable::Probe(mText);
    if (!text) return;

    AutoPtr<ArrayOf<IInterface*> > spans;
    ISpanned::Probe(text)->GetSpans(start, end, type, (ArrayOf<IInterface*>**)&spans);
    Int32 length = spans->GetLength();
    for (Int32 i = length - 1; i > 0; --i) {
        Int32 spanStart, spanEnd;
        ISpanned::Probe(text)->GetSpanStart((*spans)[i], &spanStart);
        ISpanned::Probe(text)->GetSpanEnd((*spans)[i], &spanEnd);
        // Spans that are adjacent to the edited region will be handled in
        // updateSpellCheckSpans. Result depends on what will be added (space or text)
        if (spanEnd == start || spanStart == end) break;
        ISpannable::Probe(text)->RemoveSpan((*spans)[i]);
    }
}

void TextView::RemoveAdjacentSuggestionSpans(
    /* [in] */ Int32 pos)
{
    if (!IEditable::Probe(mText)) return;
    AutoPtr<IEditable> text = IEditable::Probe(mText);

    AutoPtr< ArrayOf<IInterface*> > temp;
    (ISpanned::Probe(text))->GetSpans(pos, pos, EIID_ISuggestionSpan, (ArrayOf<IInterface*>**)&temp);

    AutoPtr< ArrayOf<ISuggestionSpan*> > spans = ArrayOf<ISuggestionSpan*>::Alloc(temp->GetLength());
    for (Int32 i = 0; i < temp->GetLength(); i++) {
        spans->Set(i, ISuggestionSpan::Probe((*temp)[i]));
    }

    for (Int32 i = 0; i < temp->GetLength(); i++) {
        Int32 spanStart, spanEnd;
        (ISpanned::Probe(text))->GetSpanStart((*temp)[i], &spanStart);
        (ISpanned::Probe(text))->GetSpanEnd((*temp)[i], &spanEnd);
        if (spanEnd == pos || spanStart == pos) {
            assert(0);
            /*if (SpellChecker::HaveWordBoundariesChanged(text, pos, pos, spanStart, spanEnd)) {
                (ISpannable::Probe(text))->RemoveSpan((*temp)[i]);
            }*/
        }
    }
}

void TextView::SendOnTextChanged(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 after)
{
    List<AutoPtr<ITextWatcher> >::Iterator iter = mListeners.Begin();
    for (; iter != mListeners.End(); ++iter) {
        (*iter)->OnTextChanged(text, start, before, after);
    }

    if (mEditor != NULL) {
        assert(0);
        //mEditor->SendOnTextChanged(start, after);
    }
}

void TextView::SendAfterTextChanged(
    /* [in] */ IEditable* text)
{
    List<AutoPtr<ITextWatcher> >::Iterator iter = mListeners.Begin();
    for (; iter != mListeners.End(); ++iter) {
        (*iter)->AfterTextChanged(text);
    }
    HideErrorIfUnchanged();
}

void TextView::UpdateAfterEdit()
{
    Invalidate();
    Int32 curs;
    GetSelectionStart(&curs);

    if (curs >= 0 || (mGravity & IGravity::VERTICAL_GRAVITY_MASK) == IGravity::BOTTOM) {
        RegisterForPreDraw();
    }

    CheckForResize();

    if (curs >= 0) {
        mHighlightPathBogus = TRUE;
        assert(0);
        //if (mEditor != NULL) mEditor->MakeBlink();
        Boolean bringPointIntoView;
        BringPointIntoView(curs, &bringPointIntoView);
    }
}

void TextView::HandleTextChanged(
    /* [in] */ ICharSequence* buffer,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 after)
{
    assert(0);
    /*InputMethodState* ims = mEditor == NULL ? NULL : mEditor->mInputMethodState.Get();

    if (ims == NULL || ims->mBatchEditNesting == 0) {
        UpdateAfterEdit();
    }
    if (ims != NULL) {
        ims->mContentChanged = TRUE;
        if (ims->mChangedStart < 0) {
            ims->mChangedStart = start;
            ims->mChangedEnd = start+before;
        }
        else {
            ims->mChangedStart = Elastos::Core::Math::Min(ims->mChangedStart, start);
            ims->mChangedEnd = Elastos::Core::Math::Max(ims->mChangedEnd, start + before - ims->mChangedDelta);
        }
        ims->mChangedDelta += after-before;
    }*/

    ResetErrorChangedFlag();
    SendOnTextChanged(buffer, start, before, after);
    OnTextChanged(buffer, start, before, after);
}

ECode TextView::SpanChange(
    /* [in] */ ISpanned* buf,
    /* [in] */ IInterface* what,
    /* [in] */ Int32 oldStart,
    /* [in] */ Int32 newStart,
    /* [in] */ Int32 oldEnd,
    /* [in] */ Int32 newEnd)
{
    // XXX Make the start and end move together if this ends up
    // spending too much time invalidating.
    assert(0);
    /*Boolean selChanged = FALSE;
    Int32 newSelStart=-1, newSelEnd=-1;

    InputMethodState* ims = mEditor == NULL ? NULL : mEditor->mInputMethodState.Get();

    if (what == Selection::SELECTION_END) {
        selChanged = TRUE;
        newSelEnd = newStart;

        if (oldStart >= 0 || newStart >= 0) {
            InvalidateCursor(Selection::GetSelectionStart(buf), oldStart, newStart);

            CheckForResize();
            RegisterForPreDraw();

            if (mEditor != NULL) mEditor->MakeBlink();
        }
    }

    if (what == Selection::SELECTION_START) {
        selChanged = TRUE;
        newSelStart = newStart;

        if (oldStart >= 0 || newStart >= 0) {
            Int32 end = Selection::GetSelectionEnd(buf);
            InvalidateCursor(end, oldStart, newStart);
        }
    }

    if (selChanged) {
        mHighlightPathBogus = TRUE;
        if (mEditor != NULL && !IsFocused()) mEditor->mSelectionMoved = TRUE;

        Int32 flags;
        buf->GetSpanFlags(what, &flags);
        if ((flags & ISpanned::SPAN_INTERMEDIATE) == 0) {
            if (newSelStart < 0) {
                newSelStart = Selection::GetSelectionStart(buf);
            }
            if (newSelEnd < 0) {
                newSelEnd = Selection::GetSelectionEnd(buf);
            }
            OnSelectionChanged(newSelStart, newSelEnd);
        }
    }

    if (IUpdateAppearance::Probe(what) || IParagraphStyle::Probe(what)) {
        if (ims == NULL || ims->mBatchEditNesting == 0) {
            Invalidate();
            mHighlightPathBogus = TRUE;
            CheckForResize();
        } else {
            ims->mContentChanged = TRUE;
        }
    }

    AutoPtr<IMetaKeyKeyListenerHelper> helper;
    CMetaKeyKeyListenerHelper::AcquireSingleton((IMetaKeyKeyListenerHelper**)&helper);

    Boolean result;
    helper->IsMetaTracker(buf, what, &result);
    if (result) {
        mHighlightPathBogus = TRUE;
        if (ims != NULL && (helper->IsSelectingMetaTracker(buf, what, &result), result)) {
            ims->mSelectionModeChanged = TRUE;
        }

        if (Selection::GetSelectionStart(buf) >= 0) {
            if (ims == NULL || ims->mBatchEditNesting == 0) {
                InvalidateCursor();
            }
            else {
                ims->mCursorChanged = TRUE;
            }
        }
    }

    if (IParcelableSpan::Probe(what)) {
       // If this is a span that can be sent to a remote process,
        // the current extract editor would be interested in it.
        if (ims != NULL && ims->mExtractedTextRequest != NULL) {
            if (ims->mBatchEditNesting != 0) {
                if (oldStart >= 0) {
                    if (ims->mChangedStart > oldStart) {
                        ims->mChangedStart = oldStart;
                    }
                    if (ims->mChangedStart > oldEnd) {
                        ims->mChangedStart = oldEnd;
                    }
                }
                if (newStart >= 0) {
                    if (ims->mChangedStart > newStart) {
                        ims->mChangedStart = newStart;
                    }
                    if (ims->mChangedStart > newEnd) {
                        ims->mChangedStart = newEnd;
                    }
                }
            } else {
                if (DEBUG_EXTRACT)
                    Logger::V(TEXT_VIEW_TAG, "Span change outside of batch: %d-%d, %d-%d",
                        oldStart, oldEnd, newStart, newEnd + what);
                ims->mContentChanged = TRUE;
            }
        }
    }

    if (mEditor != NULL && mEditor->mSpellChecker != NULL && newStart < 0
            && ISpellCheckSpan::Probe(what)) {
            mEditor->mSpellChecker->OnSpellCheckSpanRemoved(
                    (ISpellCheckSpan*)(ISpellCheckSpan::Probe(what)));
    }*/

    return NOERROR;
}

ECode TextView::DispatchFinishTemporaryDetach()
{
    mDispatchTemporaryDetach = TRUE;
    ECode ec = View::DispatchFinishTemporaryDetach();
    mDispatchTemporaryDetach = FALSE;
    return ec;
}

ECode TextView::OnStartTemporaryDetach()
{
    ECode ec = View::OnStartTemporaryDetach();
    // Only track when onStartTemporaryDetach() is called directly,
    // usually because this instance is an editable field in a list
    if (!mDispatchTemporaryDetach) mTemporaryDetach = TRUE;

    // Tell the editor that we are temporarily detached. It can use this to preserve
    // selection state as needed.
    assert(0);
    //if (mEditor != NULL) mEditor->mTemporaryDetach = TRUE;

    return ec;
}

ECode TextView::OnFinishTemporaryDetach()
{
    ECode ec = View::OnFinishTemporaryDetach();
    // Only track when onStartTemporaryDetach() is called directly,
    // usually because this instance is an editable field in a list
    if (!mDispatchTemporaryDetach) mTemporaryDetach = FALSE;
    assert(0);
    //if (mEditor != NULL) mEditor->mTemporaryDetach = FALSE;
    return ec;
}

void TextView::OnFocusChanged(
    /* [in] */ Boolean focused,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    if (mTemporaryDetach) {
        // If we are temporarily in the detach state, then do nothing.
        View::OnFocusChanged(focused, direction, previouslyFocusedRect);
        return;
    }
    assert(0);
    //if (mEditor != NULL) mEditor->OnFocusChanged(focused, direction);

    if (focused) {
        AutoPtr<ISpannable> spannable = ISpannable::Probe(mText);
        if (spannable) {
            AutoPtr<IMetaKeyKeyListenerHelper> helper;
            CMetaKeyKeyListenerHelper::AcquireSingleton((IMetaKeyKeyListenerHelper**)&helper);
            helper->ResetMetaState(spannable);
        }
    }

    StartStopMarquee(focused);

    if (mTransformation != NULL) {
        mTransformation->OnFocusChanged(
            this, mText,
            focused, direction, previouslyFocusedRect);
    }

    View::OnFocusChanged(focused, direction, previouslyFocusedRect);
}

ECode TextView::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    View::OnWindowFocusChanged(hasWindowFocus);
    assert(0);
    //if (mEditor != NULL) mEditor->OnWindowFocusChanged(hasWindowFocus);

    StartStopMarquee(hasWindowFocus);
    return NOERROR;
}

ECode TextView::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    View::OnVisibilityChanged(changedView, visibility);
    if (mEditor != NULL && visibility != IView::VISIBLE) {
        assert(0);
        //mEditor->HideControllers();
    }

    return NOERROR;
}

ECode TextView::ClearComposingText()
{
    AutoPtr<ISpannable> spannable = ISpannable::Probe(mText);
    if (spannable) {
        BaseInputConnection::RemoveComposingSpans(spannable);
    }
    return NOERROR;
}

ECode TextView::SetSelected(
    /* [in] */ Boolean selected)
{
    Boolean wasSelected;
    IsSelected(&wasSelected);

    View::SetSelected(selected);

    if (selected != wasSelected && mEllipsize == TextUtilsTruncateAt_MARQUEE) {
        if (selected) {
            StartMarquee();
        }
        else {
            StopMarquee();
        }
    }
    return NOERROR;
}


ECode TextView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Int32 action;
    event->GetActionMasked(&action);

    assert(0);
    //if (mEditor != NULL) mEditor->OnTouchEvent(event);

    Boolean superResult;
    View::OnTouchEvent(event, &superResult);

    /*
     * Don't handle the release after a long press, because it will
     * move the selection away from whatever the menu action was
     * trying to affect.
     */
    assert(0);
    /*if (mEditor != NULL && mEditor->mDiscardNextActionUp && action == IMotionEvent::ACTION_UP) {
        mEditor->mDiscardNextActionUp = FALSE;
        *res = superResult;
        return NOERROR;
    }*/
    assert(0);
    Boolean isFocused;
    Boolean touchIsFinished = (action == IMotionEvent::ACTION_UP) &&
            /*(mEditor == NULL || !mEditor->mIgnoreActionUpEvent) &&*/
        (IsFocused(&isFocused), isFocused);
    Boolean isEnabled, onCheckIsTextEditor;
     if ((mMovement != NULL || (OnCheckIsTextEditor(&onCheckIsTextEditor), onCheckIsTextEditor))
        && (IsEnabled(&isEnabled), isEnabled)
        && ISpannable::Probe(mText) && mLayout != NULL) {
        AutoPtr<ISpannable> spannable = ISpannable::Probe(mText);
        Boolean handled = FALSE;

        if (mMovement != NULL) {
            Boolean result;
            handled |= mMovement->OnTouchEvent(this, spannable, event, &result);
        }

        Boolean textIsSelectable;
        IsTextSelectable(&textIsSelectable);
        if (touchIsFinished && mLinksClickable && mAutoLinkMask != 0 && textIsSelectable) {
            // The LinkMovementMethod which should handle taps on links has not been installed
            // on non editable text that support text selection.
            // We reproduce its behavior here to open links for these.
            AutoPtr<ArrayOf<IInterface*> > links;
            Int32 selectionStart, selectionEnd;
            GetSelectionStart(&selectionStart);
            GetSelectionEnd(&selectionEnd);
            ISpanned::Probe(spannable)->GetSpans(selectionStart,
                    selectionEnd, EIID_IClickableSpan, (ArrayOf<IInterface*>**)&links);

            if (links->GetLength() > 0) {
                AutoPtr<IClickableSpan> cs = IClickableSpan::Probe((*links)[0]);
                if (cs) {
                    cs->OnClick(this);
                    handled = TRUE;
                }
            }
        }

        if (touchIsFinished && (IsTextEditable() || textIsSelectable)) {
            // Show the IME, except when selecting in read-only text.
            assert(0);
            AutoPtr<IInputMethodManager> imm;// = CInputMethodManager::PeekInstance();
            ViewClicked(imm);
            assert(0);
            /*if (!textIsSelectable && mEditor->mShowSoftInputOnFocus) {
                Boolean result = (imm != NULL);
                if (result) {
                    imm->ShowSoftInput(this, 0, &result);
                }
                handled |= result; ;
            }*/

            // The above condition ensures that the mEditor is not NULL
            assert(0);
            //mEditor->OnTouchUpEvent(event);

            handled = TRUE;
        }

        if (handled) {
            *res = TRUE;
            return NOERROR;
        }
    }

    *res = superResult;
    return NOERROR;
}

ECode TextView::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (mMovement != NULL && ISpannable::Probe(mText) && mLayout != NULL) {
        AutoPtr<ISpannable> spannable = (ISpannable*)ISpannable::Probe(mText);
//        try {
        Boolean result;
        ECode ec = mMovement->OnGenericMotionEvent(this, spannable, event, &result);
        if (!FAILED(ec) && result) {
            *res = TRUE;
            return NOERROR;
        }
//        } catch (AbstractMethodError ex) {
//            // onGenericMotionEvent was added to the MovementMethod interface in API 12.
//            // Ignore its absence in case third party applications implemented the
//            // interface directly.
//        }
    }
    return View::OnGenericMotionEvent(event, res);
}

Boolean TextView::IsTextEditable()
{
    Boolean isEnabled, onCheckIsTextEditor;
    return mText != NULL && IEditable::Probe(mText)
        && (OnCheckIsTextEditor(&onCheckIsTextEditor), onCheckIsTextEditor)
        && (IsEnabled(&isEnabled), isEnabled);
}

ECode TextView::DidTouchFocusSelect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    //*result = mEditor != NULL && mEditor->mTouchFocusSelected;
    return NOERROR;
}

ECode TextView::CancelLongPress()
{
    assert(0);
    ECode ec = View::CancelLongPress();
    //if (mEditor != NULL) mEditor->mIgnoreActionUpEvent = TRUE;
    return ec;
}

ECode TextView::OnTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (mMovement != NULL && ISpannable::Probe(mText) && mLayout != NULL) {
        Boolean result;
        mMovement->OnTrackballEvent(
            this, ISpannable::Probe(mText),
            event, &result);
        if (result) {
            *res = TRUE;
            return NOERROR;
        }
    }
    return View::OnTrackballEvent(event, res);
}

ECode TextView::SetScroller(
    /* [in] */ IScroller* s)
{
    mScroller = s;
    return NOERROR;
}

Float TextView::GetLeftFadingEdgeStrength()
{
    if (mEllipsize == TextUtilsTruncateAt_MARQUEE) {
        Int32 linecount;
        if (mMarquee != NULL && !mMarquee->IsStopped()) {
            AutoPtr<Marquee> marquee = mMarquee;
            if (marquee->ShouldDrawLeftFade()) {
                Int32 length;
                GetHorizontalFadingEdgeLength(&length);
                return marquee->mScroll / length;
            } else {
                return 0.0f;
            }
        } else if ((GetLineCount(&linecount), linecount) == 1) {
            Float lr, ll;
            mLayout->GetLineLeft(0, &ll);
            mLayout->GetLineRight(0, &lr);
            switch (mGravity & IGravity::HORIZONTAL_GRAVITY_MASK) {
                case IGravity::LEFT:
                    return 0.0f;
                case IGravity::RIGHT: {
                    Int32 length, compoundPaddingLeft, compoundPaddingRight;
                    GetHorizontalFadingEdgeLength(&length);
                    GetCompoundPaddingLeft(&compoundPaddingLeft);
                    GetCompoundPaddingRight(&compoundPaddingRight);
                    return (lr - (mRight - mLeft) - compoundPaddingLeft -
                        compoundPaddingRight - ll) / length;
                }
                case IGravity::CENTER_HORIZONTAL:
                case IGravity::FILL_HORIZONTAL:
                {
                    Int32 textDirection;
                    mLayout->GetParagraphDirection(0, &textDirection);
                    if (textDirection == Layout::DIR_LEFT_TO_RIGHT) {
                        return 0.0f;
                    } else {
                        Float lineRight, lineLeft;
                        mLayout->GetLineRight(0, &lineRight);
                        mLayout->GetLineLeft(0, &lineLeft);
                        Int32 length, compoundPaddingLeft, compoundPaddingRight;
                        GetHorizontalFadingEdgeLength(&length);
                        GetCompoundPaddingLeft(&compoundPaddingLeft);
                        GetCompoundPaddingRight(&compoundPaddingRight);
                        return (lineRight - (mRight - mLeft) - compoundPaddingLeft - compoundPaddingRight -
                            lineLeft) / length;
                    }
                }
            }
        }
    }
    return View::GetLeftFadingEdgeStrength();
}

Float TextView::GetRightFadingEdgeStrength()
{
    if (mEllipsize == TextUtilsTruncateAt_MARQUEE &&
            mMarqueeFadeMode != MARQUEE_FADE_SWITCH_SHOW_ELLIPSIS) {
        Int32 linecount;
        if (mMarquee != NULL && !mMarquee->IsStopped()) {
            AutoPtr<Marquee> marquee = mMarquee;
            Int32 length;
            GetHorizontalFadingEdgeLength(&length);
            return (marquee->mMaxFadeScroll - marquee->mScroll) / length;
        } else if ((GetLineCount(&linecount), linecount) == 1) {
            Int32 textWidth = 0;
            Float lineWidth;
            mLayout->GetLineWidth(0, &lineWidth);

            Int32 layoutDirection;
            GetLayoutDirection(&layoutDirection);
            Int32 absoluteGravity = Gravity::GetAbsoluteGravity(mGravity, layoutDirection);
            Int32 compoundPaddingLeft, compoundPaddingRight;
            GetCompoundPaddingLeft(&compoundPaddingLeft);
            GetCompoundPaddingRight(&compoundPaddingRight);
            switch (absoluteGravity & IGravity::HORIZONTAL_GRAVITY_MASK) {
                case IGravity::LEFT: {
                    textWidth = (mRight - mLeft) - compoundPaddingLeft - compoundPaddingRight;
                    Int32 length;
                    GetHorizontalFadingEdgeLength(&length);
                    return (lineWidth - textWidth) / length;
                }
                case IGravity::RIGHT:
                    return 0.0f;
                case IGravity::CENTER_HORIZONTAL:
                case IGravity::FILL_HORIZONTAL:
                {
                    Int32 textDirection;
                    mLayout->GetParagraphDirection(0, &textDirection);
                    if (textDirection == ILayout::DIR_RIGHT_TO_LEFT) {
                        return 0.0f;
                    } else {
                        Int32 length;
                        GetHorizontalFadingEdgeLength(&length);
                        return (lineWidth - ((mRight - mLeft) -
                            compoundPaddingLeft - compoundPaddingRight)) / length;
                    }
                }
            }
        }
    }
    return View::GetRightFadingEdgeStrength();
}

Int32 TextView::ComputeHorizontalScrollRange()
{
    if (mLayout != NULL) {
        Int32 width;
        mLayout->GetWidth(&width);
        Float lineWidth;
        mLayout->GetLineWidth(0, &lineWidth);
        return mSingleLine && (mGravity & IGravity::HORIZONTAL_GRAVITY_MASK) == IGravity::LEFT ?
            (Int32) lineWidth : width;
    }

    return View::ComputeHorizontalScrollRange();
}

Int32 TextView::ComputeVerticalScrollRange()
{
    if (mLayout != NULL) {
        Int32 height;
        mLayout->GetHeight(&height);
        return height;
    }

    return View::ComputeVerticalScrollRange();
}

Int32 TextView::ComputeVerticalScrollExtent()
{
    Int32 height, compoundPaddingTop, compoundPaddingBottom;
    GetHeight(&height);
    GetCompoundPaddingTop(&compoundPaddingTop);
    GetCompoundPaddingBottom(&compoundPaddingBottom);
    return height - compoundPaddingTop - compoundPaddingBottom;
}

ECode TextView::FindViewsWithText(
    /* [in, out] */ IArrayList* outViews,
    /* [in] */ ICharSequence* searched,
    /* [in] */ Int32 flags)
{
    assert(outViews != NULL);
    View::FindViewsWithText(outViews, searched, flags);

    Boolean contains = FALSE;
    outViews->Contains(TO_IINTERFACE(this), &contains);

    if (!contains && (flags & IView::FIND_VIEWS_WITH_TEXT) != 0
        && !TextUtils::IsEmpty(searched) && !TextUtils::IsEmpty(mText)) {
            String searchedLowerCase;
            searched->ToString(&searchedLowerCase);
            searchedLowerCase = searchedLowerCase.ToLowerCase();
            String textLowerCase;
            mText->ToString(&textLowerCase);
            textLowerCase = textLowerCase.ToLowerCase();
            if (textLowerCase.Contains(searchedLowerCase)) {
                outViews->Add(TO_IINTERFACE(this));
            }
    }
    return NOERROR;
}

ECode TextView::GetTextColors(
    /* [in] */ IContext* context,
    /* [in] */ ITypedArray* attrs,
    /* [out] */ IColorStateList** list)
{
    VALIDATE_NOT_NULL(list)
    if (!attrs) {
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::TextView),
        ArraySize(R::styleable::TextView));
    AutoPtr<ITypedArray> a;
    ASSERT_SUCCEEDED(context->ObtainStyledAttributes(attrIds, (ITypedArray**)&a));
    AutoPtr<IColorStateList> colors;
    a->GetColorStateList(R::styleable::TextView_textColor, (IColorStateList**)&colors);

    if (colors == NULL) {
        Int32 ap;
        a->GetResourceId(R::styleable::TextView_textAppearance, 0, &ap);
        if (ap != 0) {
            AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
                const_cast<Int32 *>(R::styleable::TextAppearance),
                ArraySize(R::styleable::TextAppearance));
            AutoPtr<ITypedArray> appearance;
            ASSERT_SUCCEEDED(context->ObtainStyledAttributes(
                    ap, attrIds, (ITypedArray**)&appearance));

            appearance->GetColorStateList(R::styleable::TextAppearance_textColor,
                    (IColorStateList**)&colors);
            appearance->Recycle();
        }
    }
    a->Recycle();

    *list = colors;
    REFCOUNT_ADD(*list);

    return NOERROR;
}

Int32 TextView::GetTextColor(
    /* [in] */ IContext* context,
    /* [in] */ ITypedArray* attrs,
    /* [in] */ Int32 def)
{
    AutoPtr<IColorStateList> colors;
    GetTextColors(context, attrs, (IColorStateList**)&colors);

    if (colors == NULL) {
        return def;
    } else {
        Int32 color;
        colors->GetDefaultColor(&color);
        return color;
    }
}

ECode TextView::OnKeyShortcut(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    switch (keyCode) {
    case IKeyEvent::KEYCODE_A:
        if (CanSelectText()) {
            OnTextContextMenuItem(R::id::selectAll, res);
            return NOERROR;
        }

        break;

    case IKeyEvent::KEYCODE_X:
        if (CanCut()) {
            OnTextContextMenuItem(R::id::cut, res);
            return NOERROR;
        }

        break;

    case IKeyEvent::KEYCODE_C:
        if (CanCopy()) {
            OnTextContextMenuItem(R::id::copy, res);
            return NOERROR;
        }

        break;

    case IKeyEvent::KEYCODE_V:
        if (CanPaste()) {
            OnTextContextMenuItem(R::id::paste, res);
            return NOERROR;
        }

        break;
    }
    return View::OnKeyShortcut(keyCode, event, res);
}

Boolean TextView::CanSelectText()
{
    Int32 len;
    mText->GetLength(&len);
    assert(0);
    //return len != 0 && mEditor != NULL && mEditor->HasSelectionController();
    return FALSE;
}

Boolean TextView::TextCanBeSelected()
{
    // prepareCursorController() relies on this method.
    // If you change this condition, make sure prepareCursorController is called anywhere
    // the value of this condition might be changed.
    Boolean result;
    if (mMovement == NULL || mMovement->CanSelectArbitrarily(&result), !result) {
        return FALSE;
    }
    Boolean isTextSelectable, isEnabled;
    return IsTextEditable() || ((IsTextSelectable(&isTextSelectable), isTextSelectable)
        && ISpannable::Probe(mText)
        && (IsEnabled(&isEnabled), isEnabled));
}

AutoPtr<ILocale> TextView::GetTextServicesLocale(
    /* [in] */ Boolean allowNullLocale)
{
    // Start fetching the text services locale asynchronously.
    UpdateTextServicesLocaleAsync();
    // If !allowNullLocale and there is no cached text services locale, just return the default
    // locale.
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> defaultLocale;
    helper->GetDefault((ILocale**)&defaultLocale);
    return (mCurrentSpellCheckerLocaleCache == NULL && !allowNullLocale) ? defaultLocale
            : mCurrentSpellCheckerLocaleCache;
}

ECode TextView::GetTextServicesLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    AutoPtr<ILocale> temp = GetTextServicesLocale(FALSE);
    *locale = temp;
    REFCOUNT_ADD(*locale)
    return NOERROR;
}

ECode TextView::GetSpellCheckerLocale(
    /* [out] */ ILocale** local)
{
    VALIDATE_NOT_NULL(local)
    AutoPtr<ILocale> temp = GetTextServicesLocale(TRUE /* allowNullLocale */);
    *local = temp;
    REFCOUNT_ADD(*local)
    return NOERROR;
}

void TextView::UpdateTextServicesLocaleAsync()
{
    AutoPtr<UpdateTextServicesLocaleRunnable> runnable = new UpdateTextServicesLocaleRunnable(this);
    AsyncTask::Execute(runnable);
}

void TextView::UpdateTextServicesLocaleLocked()
{
    assert(0);
    // TODO
    // AutoPtr<IInterface> obj;
    // mContext->GetSystemService(IContext::TEXT_SERVICES_MANAGER_SERVICE, (IInterface**)&obj);
    // AutoPtr<ITextServicesManager> textServicesManager = ITextServicesManager::Probe(obj);
    // if (textServicesManager) {
    //    AutoPtr<ISpellCheckerSubtype> subtype;
    //    textServicesManager->GetCurrentSpellCheckerSubtype(TRUE, (ISpellCheckerSubtype**)&subtype);
    //    AutoPtr<ILocale> locale;
    //    if (subtype != NULL) {
    //         AutoPtr<ILocale> scLocale;
    //         subtype->GetLocale((ILocale**)&scLocale);
    //         AutoPtr<ISpellCheckerSubtypeHelper> helper;
    //         CSpellCheckerSubtypeHelper::AcquireSingleton((ISpellCheckerSubtypeHelper**)&helper);

    //         helper->ConstructLocaleFromString(scLocale, (ILocale**)&locale);
    //    } else {
    //         locale = NULL;
    //    }
    // }

    // mCurrentSpellCheckerLocaleCache = locale;
}

ECode TextView::OnLocaleChanged()
{
    // Will be re-created on demand in getWordIterator with the proper new locale
    assert(0);
    //mEditor->mWordIterator = NULL;
    return NOERROR;
}

/**
 * This method is used by the ArrowKeyMovementMethod to jump from one word to the other.
 * Made available to achieve a consistent behavior.
 * @hide
 */
ECode TextView::GetWordIterator(
    /* [out] */ IWordIterator** iterator)
{
    VALIDATE_NOT_NULL(iterator)
    if (mEditor != NULL) {
        assert(0);
        //*iterator = mEditor->GetWordIterator();
        //REFCOUNT_ADD(*iterator)
    } else {
        *iterator = NULL;
    }
    return NOERROR;
}

ECode TextView::OnPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    View::OnPopulateAccessibilityEvent(event);

    Boolean isPassword = HasPasswordTransformationMethod();
    if (!isPassword || ShouldSpeakPasswordsForAccessibility()) {
        AutoPtr<ICharSequence> text;
        GetTextForAccessibility((ICharSequence**)&text);
        if (!TextUtils::IsEmpty(text)) {
            AutoPtr<IList> list;
            IAccessibilityRecord::Probe(event)->GetText((IList**)&list);
            list->Add(text);
        }
    }
    return NOERROR;
}

/**
 * @return TRUE if the user has explicitly allowed accessibility services
 * to speak passwords.
 */
Boolean TextView::ShouldSpeakPasswordsForAccessibility()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    Int32 pwd = 0;
    AutoPtr<ISettingsSecure> ss;
    assert(0);
    //CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    assert(ss != NULL);
    AutoPtr<IContentResolver> res;
    mContext->GetContentResolver((IContentResolver**)&res);
    ss->GetInt32(res, ISettingsSecure::ACCESSIBILITY_SPEAK_PASSWORD, 0, &pwd);
    return pwd == 1;
}

ECode TextView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    View::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> csq;
    CString::New(String("TextView"), (ICharSequence**)&csq);
    IAccessibilityRecord::Probe(event)->SetClassName(csq);
    Boolean isPassword = HasPasswordTransformationMethod();
    IAccessibilityRecord::Probe(event)->SetPassword(isPassword);

    Int32 type;
    event->GetEventType(&type);
    if (type == IAccessibilityEvent::TYPE_VIEW_TEXT_SELECTION_CHANGED) {
        Int32 tl;
        mText->GetLength(&tl);
        IAccessibilityRecord::Probe(event)->SetFromIndex(Selection::GetSelectionStart(mText));
        IAccessibilityRecord::Probe(event)->SetToIndex(Selection::GetSelectionEnd(mText));
        IAccessibilityRecord::Probe(event)->SetItemCount(tl);
    }
    return NOERROR;
}

ECode TextView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    View::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> csq;
    CString::New(String("TextView"), (ICharSequence**)&csq);
    info->SetClassName(csq);
    Boolean isPassword = HasPasswordTransformationMethod();
    info->SetPassword(isPassword);

    if (!isPassword || ShouldSpeakPasswordsForAccessibility()) {
        AutoPtr<ICharSequence> text;
        GetTextForAccessibility((ICharSequence**)&text);
        info->SetText(text);
    }

    if (mBufferType == BufferType_EDITABLE) {
        info->SetEditable(TRUE);
    }

    if (mEditor) {
        assert(0);
        /*info->SetInputType(mEditor->mInputType);

        if (mEditor->mError) {
            info->SetContentInvalid(TRUE);
            info->SetError(mEditor->mError);
        }*/
    }

    if (!TextUtils::IsEmpty(mText)) {
        info->AddAction(IAccessibilityNodeInfo::ACTION_NEXT_AT_MOVEMENT_GRANULARITY);
        info->AddAction(IAccessibilityNodeInfo::ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY);
        info->SetMovementGranularities(IAccessibilityNodeInfo::MOVEMENT_GRANULARITY_CHARACTER
                | IAccessibilityNodeInfo::MOVEMENT_GRANULARITY_WORD
                | IAccessibilityNodeInfo::MOVEMENT_GRANULARITY_LINE
                | IAccessibilityNodeInfo::MOVEMENT_GRANULARITY_PARAGRAPH
                | IAccessibilityNodeInfo::MOVEMENT_GRANULARITY_PAGE);
    }

    Boolean isFocused;
    if (IsFocused(&isFocused), isFocused) {
        if (CanSelectText()) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_SET_SELECTION);
        }
        if (CanCopy()) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_COPY);
        }
        if (CanPaste()) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_PASTE);
        }
        if (CanCut()) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_CUT);
        }
    }

    Int32 numFilters = mFilters->GetLength();
    for (Int32 i = 0; i < numFilters; i++) {
        AutoPtr<Elastos::Droid::Text::IInputFilter> filter = (*mFilters)[i];
        if (ILengthFilter::Probe(filter)) {
            Int32 max;
            (ILengthFilter::Probe(filter))->GetMax(&max);
            info->SetMaxTextLength(max);
        }
    }

    if (!IsSingleLine()) {
        info->SetMultiLine(TRUE);
    }
    return NOERROR;
}

ECode TextView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    switch (action) {
        case IAccessibilityNodeInfo::ACTION_CLICK:
        {
            Boolean handled = FALSE;

            // Simulate View.onTouchEvent for an ACTION_UP event.
            Boolean isClickable, isLongClickable;
            if ((IsClickable(&isClickable), isClickable)
                || (IsLongClickable(&isLongClickable), isLongClickable)) {
                Boolean isFocusable, isFocused;
                if ((IsFocusable(&isFocusable), isFocusable)
                    && (IsFocused(&isFocused), !isFocused)) {
                    RequestFocus(&isFocused);
                }

                PerformClick(&isFocused);
                handled = TRUE;
            }

            // Simulate TextView.onTouchEvent for an ACTION_UP event.
            Boolean isEnabled, isFocused, isTextSelectable, onCheckIsTextEditor;
            if ((mMovement || (OnCheckIsTextEditor(&onCheckIsTextEditor), onCheckIsTextEditor))
                && (IsEnabled(&isEnabled), isEnabled) && ISpannable::Probe(mText)
                && mLayout && (IsTextEditable()
                || (IsTextSelectable(&isTextSelectable), isTextSelectable))
                && (IsFocused(&isFocused), isFocused)) {
                // Show the IME, except when selecting in read-only text.
                assert(0);
                AutoPtr<IInputMethodManager> imm;// = CInputMethodManager::PeekInstance();
                ViewClicked(imm);
                assert(0);
                /*if (!IsTextSelectable() && mEditor->mShowSoftInputOnFocus && imm) {
                    Boolean show;
                    imm->ShowSoftInput(this, 0, &show);
                    handled |= show;
                }*/
            }

            *res = handled;
            return NOERROR;
        }
        case IAccessibilityNodeInfo::ACTION_COPY:
        {
            Boolean isFocused, onTextContextMenuItem;
            if ((IsFocused(&isFocused), isFocused) && CanCopy()) {
                if (OnTextContextMenuItem(R::id::copy, &onTextContextMenuItem), onTextContextMenuItem) {
                    *res = TRUE;
                    return NOERROR;
                }
            }
        }
        *res = FALSE;
        return NOERROR;

        case IAccessibilityNodeInfo::ACTION_PASTE:
        {
            Boolean isFocused, onTextContextMenuItem;
            if ((IsFocused(&isFocused), isFocused) && CanPaste()) {
                if (OnTextContextMenuItem(R::id::paste, &onTextContextMenuItem), onTextContextMenuItem) {
                    *res = TRUE;
                    return NOERROR;
                }
            }
        }
        *res = FALSE;
        return NOERROR;

        case IAccessibilityNodeInfo::ACTION_CUT:
        {
            Boolean isFocused, onTextContextMenuItem;
            if ((IsFocused(&isFocused), isFocused) && CanCut()) {
                if (OnTextContextMenuItem(R::id::cut, &onTextContextMenuItem), onTextContextMenuItem) {
                    *res = TRUE;
                    return NOERROR;
                }
            }
        }
        *res = FALSE;
        return NOERROR;

        case IAccessibilityNodeInfo::ACTION_SET_SELECTION:
        {
            Boolean isFocused;
            if ((IsFocused(&isFocused), isFocused) && CanSelectText()) {
                AutoPtr<ICharSequence> text;
                GetIterableTextForAccessibility((ICharSequence**)&text);
                if (!text) {
                    *res = FALSE;
                    return NOERROR;
                }
                Int32 start = -1, end = -1;
                if (arguments) {
                    (IBaseBundle::Probe(arguments))->GetInt32(
                        IAccessibilityNodeInfo::ACTION_ARGUMENT_SELECTION_START_INT, -1, &start);
                    (IBaseBundle::Probe(arguments))->GetInt32(
                        IAccessibilityNodeInfo::ACTION_ARGUMENT_SELECTION_END_INT, -1, &end);
                }
                Int32 selectionStart, selectionEnd;
                if (((GetSelectionStart(&selectionStart), selectionStart) != start
                    || (GetSelectionEnd(&selectionEnd), selectionEnd) != end)) {
                    // No arguments clears the selection.
                    if (start == end && end == -1) {
                        Selection::RemoveSelection(ISpannable::Probe(text));
                        *res = TRUE;
                        return NOERROR;
                    }
                    Int32 textLen;
                    text->GetLength(&textLen);
                    if (start >= 0 && start <= end && end <= textLen) {
                        Selection::SetSelection(ISpannable::Probe(text), start, end);
                        // Make sure selection mode is engaged.
                        if (mEditor) {
                            assert(0);
                            //mEditor->StartSelectionActionMode();
                        }
                        *res = TRUE;
                        return NOERROR;
                    }
                }
            }
        }
        *res = FALSE;
        return NOERROR;

        default:
            return View::PerformAccessibilityAction(action, arguments, res);
    }
}

ECode TextView::SendAccessibilityEvent(
    /* [in] */ Int32 eventType)
{
    // Do not send scroll events since first they are not interesting for
    // accessibility and second such events a generated too frequently.
    // For details see the implementation of bringTextIntoView().
    if (eventType == IAccessibilityEvent::TYPE_VIEW_SCROLLED) {
        return NOERROR;
    }
    return View::SendAccessibilityEvent(eventType);
}

ECode TextView::GetTextForAccessibility(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> text;
    GetText((ICharSequence**)&text);
    if (TextUtils::IsEmpty(text)) {
        GetHint((ICharSequence**)&text);
    }
    *result = text;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void TextView::SendAccessibilityEventTypeViewTextChanged(
    /* [in] */ ICharSequence* beforeText,
    /* [in] */ Int32 fromIndex,
    /* [in] */ Int32 removedCount,
    /* [in] */ Int32 addedCount)
{
    AutoPtr<IAccessibilityEvent> event;
    CAccessibilityEvent::Obtain(IAccessibilityEvent::TYPE_VIEW_TEXT_CHANGED, (IAccessibilityEvent**)&event);
    assert(event != NULL);
    IAccessibilityRecord::Probe(event)->SetFromIndex(fromIndex);
    IAccessibilityRecord::Probe(event)->SetRemovedCount(removedCount);
    IAccessibilityRecord::Probe(event)->SetAddedCount(addedCount);
    IAccessibilityRecord::Probe(event)->SetBeforeText(beforeText);
    SendAccessibilityEventUnchecked(event);
}

ECode TextView::IsInputMethodTarget(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    AutoPtr<IInputMethodManager> imm;// = CInputMethodManager::PeekInstance();
    Boolean temp = imm != NULL;
    if (temp) {
        imm->IsActive(this, &temp);
    }
    *result = temp;
    return NOERROR;
}

ECode TextView::OnTextContextMenuItem(
    /* [in] */ Int32 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContext> c;
    GetContext((IContext**)&c);
    Int32 min = 0;
    Int32 max = 0;
    mText->GetLength(&max);

    Boolean isFocused;
    if (IsFocused(&isFocused), isFocused) {
        Int32 selStart, selEnd;
        GetSelectionStart(&selStart);
        GetSelectionEnd(&selEnd);

        min = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(selStart, selEnd));
        max = Elastos::Core::Math::Max(0, Elastos::Core::Math::Max(selStart, selEnd));
    }

    switch (id) {
         case R::id::selectAll:
             // This does not enter text selection mode. Text is highlighted, so that it can be
             // bulk edited, like selectAllOnFocus does. Returns true even if text is empty.
             SelectAllText();
             *result = TRUE;
             return NOERROR;

         case R::id::paste:
             Paste(min, max);
             *result = TRUE;
             return NOERROR;

         case R::id::cut: {
             assert(0 && "TODO");
                AutoPtr<IClipDataHelper> helper;
//TODO                 CClipDataHelper::AcquireSingleton((IClipDataHelper**)&helper);
                assert(helper);
                AutoPtr<IClipData> data;
                helper->NewPlainText(NULL, GetTransformedText(min, max), (IClipData**)&data);
                SetPrimaryClip(data);
                DeleteText_internal(min, max);
                StopSelectionActionMode();
                *result = TRUE;
                return NOERROR;
             }

         case R::id::copy: {
             assert(0 && "TODO");
                AutoPtr<IClipDataHelper> helper;
//TODO                 CClipDataHelper::AcquireSingleton((IClipDataHelper**)&helper);
                assert(helper);
                AutoPtr<IClipData> data;
                helper->NewPlainText(NULL, GetTransformedText(min, max), (IClipData**)&data);
                SetPrimaryClip(data);
                StopSelectionActionMode();
                *result = TRUE;
                return NOERROR;
             }
         }

    *result = FALSE;
    return NOERROR;
}

AutoPtr<ICharSequence> TextView::GetTransformedText(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoPtr<ICharSequence> sub;
    mTransformed->SubSequence(start, end, (ICharSequence**)&sub);
    return RemoveSuggestionSpans(sub);
}

ECode TextView::PerformLongClick(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Boolean handled = FALSE;
    View::PerformLongClick(res);
    if (*res) {
        handled = TRUE;
    }

    if (mEditor != NULL) {
        assert(0);
        //handled |= mEditor->PerformLongClick(handled);
    }

    if (handled) {
        Boolean result;
        PerformHapticFeedback(IHapticFeedbackConstants::LONG_PRESS, &result);
        assert(0);
        //if (mEditor != NULL) mEditor->mDiscardNextActionUp = TRUE;
    }

    return handled;
}

void TextView::OnScrollChanged(
    /* [in] */ Int32 horiz,
    /* [in] */ Int32 vert,
    /* [in] */ Int32 oldHoriz,
    /* [in] */ Int32 oldVert)
{
    View::OnScrollChanged(horiz, vert, oldHoriz, oldVert);
    if (mEditor != NULL) {
        assert(0);
        //mEditor->OnScrollChanged();
    }
}

ECode TextView::IsSuggestionsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    assert(0);
    /*if (mEditor == NULL) return FALSE;
    if ((mEditor->mInputType & IInputType::TYPE_MASK_CLASS) != IInputType::TYPE_CLASS_TEXT) {
        return FALSE;
    }
    if ((mEditor->mInputType & IInputType::TYPE_TEXT_FLAG_NO_SUGGESTIONS) > 0) return FALSE;

    Int32 variation = mEditor->mInputType & IInputType::TYPE_MASK_VARIATION;
    *enabled = (variation == IInputType::TYPE_TEXT_VARIATION_NORMAL ||
            variation == IInputType::TYPE_TEXT_VARIATION_EMAIL_SUBJECT ||
            variation == IInputType::TYPE_TEXT_VARIATION_LONG_MESSAGE ||
            variation == IInputType::TYPE_TEXT_VARIATION_SHORT_MESSAGE ||
            variation == IInputType::TYPE_TEXT_VARIATION_WEB_EDIT_TEXT);*/
    return NOERROR;
}

ECode TextView::SetCustomSelectionActionModeCallback(
    /* [in] */ IActionModeCallback* actionModeCallback)
{
    CreateEditorIfNeeded();
    assert(0);
    //mEditor->mCustomSelectionActionModeCallback = actionModeCallback;
    return NOERROR;
}

ECode TextView::GetCustomSelectionActionModeCallback(
    /* [out] */ IActionModeCallback** actionModeCallback)
{
    VALIDATE_NOT_NULL(actionModeCallback)
    if (mEditor == NULL) {
        *actionModeCallback = NULL;
    } else {
        assert(0);
        //*actionModeCallback = mEditor->mCustomSelectionActionModeCallback;
        REFCOUNT_ADD(*actionModeCallback)
    }
    return NOERROR;
}

void TextView::StopSelectionActionMode()
{
    assert(0);
    //mEditor->StopSelectionActionMode();
}

Boolean TextView::CanCut()
{
    if (HasPasswordTransformationMethod()) {
        return FALSE;
    }

    Int32 tl;
    mText->GetLength(&tl);
    assert(0);
    /*if (tl > 0 && HasSelection() && IEditable::Probe(mText)
        && mEditor != NULL && mEditor->mKeyListener != NULL) {
        return TRUE;
    }*/

    return FALSE;
}

Boolean TextView::CanCopy()
{
    if (HasPasswordTransformationMethod()) {
        return FALSE;
    }

    Int32 tl;
    mText->GetLength(&tl);
    Boolean hasSelection;
    if (tl > 0 && (HasSelection(&hasSelection), hasSelection) && mEditor) {
        return TRUE;
    }

    return FALSE;
}

Boolean TextView::CanPaste()
{
    AutoPtr<IContext> c;
    GetContext((IContext**)&c);
    AutoPtr<IInterface> obj;
    c->GetSystemService(IContext::CLIPBOARD_SERVICE, (IInterface**)&obj);
    AutoPtr<IClipboardManager> cm = IClipboardManager::Probe(obj);
    if (!cm) return FALSE;

    Boolean hasClip;
    cm->HasPrimaryClip(&hasClip);
    assert(0);
    /*return (IEditable::Probe(mText)
        && mEditor != NULL && mEditor->mKeyListener != NULL
        && GetSelectionStart() >= 0 && GetSelectionEnd() >= 0
        && hasClip);*/
    return FALSE;
}

Boolean TextView::SelectAllText()
{
    Int32 length;
    mText->GetLength(&length);
    Selection::SetSelection(ISpannable::Probe(mText), 0, length);
    return length > 0;
}

void TextView::Paste(
    /* [in] */ Int32 min,
    /* [in] */ Int32 max)
{
    AutoPtr<IContext> c;
    GetContext((IContext**)&c);
    AutoPtr<IInterface> obj;
    c->GetSystemService(IContext::CLIPBOARD_SERVICE, (IInterface**)&obj);
    AutoPtr<IClipboardManager> clipboard = IClipboardManager::Probe(obj);
    if (!clipboard) return;

    AutoPtr<IClipData> clip;
    clipboard->GetPrimaryClip((IClipData**)&clip);
    if (clip != NULL) {
        Int32 count ;
        clip->GetItemCount(&count);
        Boolean didFirst = FALSE;
        AutoPtr<IClipDataItem> item;
        AutoPtr<ICharSequence> paste;
        AutoPtr<IEditable> editable = IEditable::Probe(mText);
        AutoPtr<ICharSequence> newlineCS;
        CString::New(String("\n"), (ICharSequence**)&newlineCS);

        for (Int32 i = 0; i < count; i++) {
            item = NULL;
            paste = NULL;
            clip->GetItemAt(i, (IClipDataItem**)&item);
            item->CoerceToStyledText(c, (ICharSequence**)&paste);
            if (paste != NULL) {
                if (!didFirst) {
                    Selection::SetSelection(ISpannable::Probe(mText), max);
                    editable->Replace(min, max, paste);
                    didFirst = TRUE;
                } else {
                    Int32 end;
                    GetSelectionEnd(&end);
                    editable->Insert(end, newlineCS);
                    editable->Insert(end, paste);
                }
            }
        }
        StopSelectionActionMode();
        LAST_CUT_OR_COPY_TIME = 0;
    }
}

void TextView::SetPrimaryClip(
    /* [in] */ IClipData* clip)
{
    AutoPtr<IContext> c;
    GetContext((IContext**)&c);
    AutoPtr<IInterface> obj;
    c->GetSystemService(IContext::CLIPBOARD_SERVICE, (IInterface**)&obj);
    AutoPtr<IClipboardManager> clipboard = IClipboardManager::Probe(obj);
    if (!clipboard) return;

    clipboard->SetPrimaryClip(clip);
    LAST_CUT_OR_COPY_TIME = SystemClock::GetUptimeMillis();
}

ECode TextView::GetOffsetForPosition(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset)
    AutoPtr<ILayout> layout;
    GetLayout((ILayout**)&layout);
    if (layout == NULL) return -1;
    Int32 line = GetLineAtCoordinate(y);
    *offset = GetOffsetAtCoordinate(line, x);
    return NOERROR;
}

Float TextView::ConvertToLocalHorizontalCoordinate(
    /* [in] */ Float x)
{
    Int32 totalPaddingLeft;
    GetTotalPaddingLeft(&totalPaddingLeft);
    x -= totalPaddingLeft;
    // Clamp the position to inside of the view.
    x = Elastos::Core::Math::Max(0.0f, x);
    Int32 width, scrollx, paddingRight;
    GetWidth(&width);
    GetTotalPaddingRight(&paddingRight);
    x = Elastos::Core::Math::Min((Float)(width - paddingRight - 1.0), x);
    GetScrollX(&scrollx);
    x += scrollx;
    return x;
}

Int32 TextView::GetLineAtCoordinate(
    /* [in] */ Float y)
{
    Int32 totalPaddingTop;
    GetTotalPaddingTop(&totalPaddingTop);
    y -= totalPaddingTop;
    // Clamp the position to inside of the view.
    y = Elastos::Core::Math::Max(0.0f, y);
    Int32 height, scrolly, totalPaddingBottom;
    GetHeight(&height);
    GetTotalPaddingBottom(&totalPaddingBottom);
    y = Elastos::Core::Math::Min((Float)(height - totalPaddingBottom - 1.0f), y);
    GetScrollY(&scrolly);
    y += scrolly;
    AutoPtr<ILayout> layout;
    GetLayout((ILayout**)&layout);
    Int32 result;
    layout->GetLineForVertical((Int32)y, &result);
    return result;
}

Int32 TextView::GetOffsetAtCoordinate(
    /* [in] */ Int32 line,
    /* [in] */ Float x)
{
    x = ConvertToLocalHorizontalCoordinate(x);
    AutoPtr<ILayout> layout;
    GetLayout((ILayout**)&layout);
    Int32 result;
    layout->GetOffsetForHorizontal(line, x, &result);
    return result;
}

ECode TextView::OnDragEvent(
    /* [in] */ IDragEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Int32 action;
    event->GetAction(&action);
    switch (action) {
        case IDragEvent::ACTION_DRAG_STARTED:{
            assert(0);
            //*res = mEditor != NULL && mEditor->HasInsertionController();
            return NOERROR;
        }

        case IDragEvent::ACTION_DRAG_ENTERED: {
            Boolean focus;
            RequestFocus(&focus);
            *res = TRUE;
            return NOERROR;
        }

        case IDragEvent::ACTION_DRAG_LOCATION: {
            Float x, y;
            event->GetX(&x);
            event->GetY(&y);
            Int32 offset;
            GetOffsetForPosition(x, y, &offset);
            Selection::SetSelection(ISpannable::Probe(mText), offset);
            *res = TRUE;
            return NOERROR;
        }
        case IDragEvent::ACTION_DROP: {
            assert(0);
            //if (mEditor != NULL) mEditor->OnDrop(event);
            *res = TRUE;
            return NOERROR;
        }
        case IDragEvent::ACTION_DRAG_ENDED:
        case IDragEvent::ACTION_DRAG_EXITED:
        default:
            *res = TRUE;
            return NOERROR;
    }
    *res = FALSE;
    return NOERROR;
}

Boolean TextView::IsInBatchEditMode()
{
    assert(0);
    /*if (mEditor == NULL) return FALSE;
    InputMethodState* ims = mEditor->mInputMethodState.Get();
    if (ims != NULL) {
        return ims->mBatchEditNesting > 0;
    }
    return mEditor->mInBatchEditControllers;*/
    return FALSE;
}

ECode TextView::OnRtlPropertiesChanged(
    /* [in] */ Int32 layoutDirection)
{
    View::OnRtlPropertiesChanged(layoutDirection);

    mTextDir = GetTextDirectionHeuristic();

    if (mLayout) CheckForRelayout();

    return NOERROR;
}

AutoPtr<ITextDirectionHeuristic> TextView::GetTextDirectionHeuristic()
{
    AutoPtr<ITextDirectionHeuristic> result;
    if (HasPasswordTransformationMethod()) {
        // TODO: take care of the content direction to show the password text and dots justified
        // to the left or to the right
        result = TextDirectionHeuristics::LTR;
        return result;
    }

    // Always need to resolve layout direction first
    Int32 direction;
    Boolean defaultIsRtl = ((GetLayoutDirection(&direction), direction)
        == IView::LAYOUT_DIRECTION_RTL);

    // Now, we can select the heuristic
    switch (GetTextDirection(&direction), direction) {
        default:
        case IView::TEXT_DIRECTION_FIRST_STRONG:
            result = (defaultIsRtl ? TextDirectionHeuristics::FIRSTSTRONG_RTL :
                    TextDirectionHeuristics::FIRSTSTRONG_LTR);
            return result;
        case IView::TEXT_DIRECTION_ANY_RTL:
            result = TextDirectionHeuristics::ANYRTL_LTR;
            return result;
        case IView::TEXT_DIRECTION_LTR:
            result = TextDirectionHeuristics::LTR;
            return result;
        case IView::TEXT_DIRECTION_RTL:
            result = TextDirectionHeuristics::RTL;
            return result;
        case IView::TEXT_DIRECTION_LOCALE:
            result = TextDirectionHeuristics::LOCALE;
            return result;
    }
    return result;
}

ECode TextView::OnResolveDrawables(
    /* [in] */ Int32 layoutDirection)
{
    // No need to resolve twice
    if (mLastLayoutDirection == layoutDirection) {
        return NOERROR;
    }
    mLastLayoutDirection = layoutDirection;
    // Resolve drawables
    if (mDrawables != NULL) {
        mDrawables->ResolveWithLayoutDirection(layoutDirection);
    }
    return NOERROR;
}

void TextView::ResetResolvedDrawables()
{
    View::ResetResolvedDrawables();
    mLastLayoutDirection = -1;
}

void TextView::ViewClicked(
    /* [in] */ IInputMethodManager* imm)
{
    if (imm != NULL) {
        imm->ViewClicked(this);
    }
}

void TextView::DeleteText_internal(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoPtr<IEditable> editable = IEditable::Probe(mText);
    if (editable) {
        editable->Delete(start, end);
    }
}

void TextView::ReplaceText_internal(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ICharSequence* text)
{
    AutoPtr<IEditable> editable = IEditable::Probe(mText);
    if (editable) {
        editable->Replace(start, end, text);
    }
}

void TextView::SetSpan_internal(
    /* [in] */ IInterface* span,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 flags)
{
    AutoPtr<IEditable> editable = IEditable::Probe(mText);
    if (editable) {
        ISpannable::Probe(editable)->SetSpan(span, start, end, flags);
    }
}

void TextView::SetCursorPosition_internal(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoPtr<IEditable> editable = IEditable::Probe(mText);
    Selection::SetSelection(ISpannable::Probe(editable), start, end);
}

void TextView::CreateEditorIfNeeded()
{
    if (mEditor == NULL) {
        assert(0);
        //mEditor = new Editor(this);
    }
}

ECode TextView::GetIterableTextForAccessibility(
    /* [out] */ ICharSequence** res)
{
    VALIDATE_NOT_NULL(res)
    if (!ISpannable::Probe(mText)) {
        SetText(mText, BufferType_SPANNABLE);
    }
    *res = mText;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

ECode TextView::GetIteratorForGranularity(
    /* [in] */ Int32 granularity,
    /* [out] */ ITextSegmentIterator** res)
{
    VALIDATE_NOT_NULL(res)
    switch (granularity) {
        case IAccessibilityNodeInfo::MOVEMENT_GRANULARITY_LINE: {
            AutoPtr<ICharSequence> text;
            GetIterableTextForAccessibility((ICharSequence**)&text);
            AutoPtr<ILayout> layout;
            if (!TextUtils::IsEmpty(text) && NULL != (GetLayout((ILayout**)&layout), layout)) {
                assert(0 && "TODO");
//TODO
//                ILineTextSegmentIterator iterator =
//                    AccessibilityIterators.LineTextSegmentIterator.getInstance();
//                iterator->Initialize(text, GetLayout());
//                *res = iterator;
//                return NOERROR;
            }
        } break;
        case IAccessibilityNodeInfo::MOVEMENT_GRANULARITY_PAGE: {
            AutoPtr<ICharSequence> text;
            GetIterableTextForAccessibility((ICharSequence**)&text);
            AutoPtr<ILayout> layout;
            if (!TextUtils::IsEmpty(text) && NULL != (GetLayout((ILayout**)&layout), layout)) {
                assert(0 && "TODO");
//TODO
//                IPageTextSegmentIterator iterator =
//                    AccessibilityIterators.PageTextSegmentIterator.getInstance();
//                iterator.Initialize(this);
//                *res = iterator;
//                return NOERROR;
            }
        } break;
    }
    return View::GetIteratorForGranularity(granularity, res);
}

ECode TextView::GetAccessibilitySelectionStart(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    return GetSelectionStart(res);
}

ECode TextView::IsAccessibilitySelectionExtendable(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = TRUE;
    return NOERROR;
}

ECode TextView::GetAccessibilitySelectionEnd(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    return GetSelectionEnd(res);
}

ECode TextView::SetAccessibilitySelection(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    Int32 selectionStart, selectionEnd;
    if ((GetAccessibilitySelectionStart(&selectionStart), selectionStart) == start &&
        (GetAccessibilitySelectionEnd(&selectionEnd), selectionEnd) == end) {
        return NOERROR;
    }
    // Hide all selection controllers used for adjusting selection
    // since we are doing so explicitlty by other means and these
    // controllers interact with how selection behaves.
    if (mEditor) {
        assert(0);
        //mEditor->HideControllers();
    }
    AutoPtr<ICharSequence> text;
    GetIterableTextForAccessibility((ICharSequence**)&text);
    Int32 textLen;
    text->GetLength(&textLen);
    if (Elastos::Core::Math::Min(start, end) >= 0 && Elastos::Core::Math::Max(start, end) <= textLen) {
        Selection::SetSelection(ISpannable::Probe(text), start, end);
    } else {
        Selection::RemoveSelection(ISpannable::Probe(text));
    }
    return NOERROR;
}


} // namespace Widget
} // namespace Droid
} // namespace Elastos
