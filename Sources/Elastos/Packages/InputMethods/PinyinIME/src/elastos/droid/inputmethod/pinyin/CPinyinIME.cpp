
#include <elastos/droid/ext/frameworkdef.h>
#include "Elastos.Droid.Preference.h"
#include "elastos/droid/inputmethod/pinyin/CPinyinIME.h"
#include "elastos/droid/inputmethod/pinyin/CCandidatesContainer.h"
#include "elastos/droid/inputmethod/pinyin/KeyMapDream.h"
#include "elastos/droid/inputmethod/pinyin/Settings.h"
#include "elastos/droid/inputmethod/pinyin/SoundManager.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

// using Elastos::Droid::App::CAlertDialogBuilder;
// using Elastos::Droid::App::IAlertDialogBuilder;
// using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
// using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
// using Elastos::Droid::Content::EIID_IContext;
// using Elastos::Droid::Content::IIntent;
// using Elastos::Droid::Content::CIntent;
// using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::EIID_IServiceConnection;
// using Elastos::Droid::InputMethodService::IInputMethodService;
// using Elastos::Droid::InputMethodService::EIID_IInputMethodService;
// using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Droid::View::CGestureDetector;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IInputEvent;
// using Elastos::Droid::View::CKeyEvent;
// using Elastos::Droid::View::IViewGroupLayoutParams;
// using Elastos::Droid::View::IWindowManagerLayoutParams;
// using Elastos::Droid::Widget::CPopupWindow;
// using Elastos::Core::StringUtils;
// using Elastos::Core::StringBuilder;
// using Elastos::Core::Math;
// using Elastos::Core::EIID_IRunnable;
// using Elastos::Core::CStringWrapper;
// using Elastos::Core::IStringBuffer;
// using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

//==========================================================
// CPinyinIME::PopupTimer
//==========================================================
CPinyinIME::PopupTimer::PopupTimer(
    /* [in] */ CPinyinIME* host)
    : mHost(host)
{
    mParentLocation = ArrayOf<Int32>::Alloc(2);
}

void CPinyinIME::PopupTimer::PostShowFloatingWindow()
{
    IView::Probe(mHost->mFloatingContainer)->Measure(IViewGroupLayoutParams::WRAP_CONTENT,
            IViewGroupLayoutParams::WRAP_CONTENT);

    Int32 mW = 0, mH = 0;
    IView::Probe(mHost->mFloatingContainer)->GetMeasuredWidth(&mW);
    IView::Probe(mHost->mFloatingContainer)->GetMeasuredHeight(&mH);
    mHost->mFloatingWindow->SetWidth(mW);
    mHost->mFloatingWindow->SetHeight(mH);
    Boolean result = FALSE;
    Post(this, &result);
}

void CPinyinIME::PopupTimer::CancelShowing()
{
    Boolean showing = FALSE;
    if (mHost->mFloatingWindow->IsShowing(&showing), showing) {
        mHost->mFloatingWindow->Dismiss();
    }
    RemoveCallbacks(this);
}

ECode CPinyinIME::PopupTimer::Run()
{
    IView::Probe(mHost->mCandidatesContainer)->GetLocationInWindow(mParentLocation);

    Int32 height = 0;
    mHost->mFloatingWindow->GetHeight(&height);
    Boolean showing = FALSE;
    mHost->mFloatingWindow->IsShowing(&showing);
    if (!showing) {
        mHost->mFloatingWindow->ShowAtLocation(IView::Probe(mHost->mCandidatesContainer),
                IGravity::LEFT | IGravity::TOP, (*mParentLocation)[0],
                (*mParentLocation)[1] - height);
    }
    else {
        Int32 width = 0;
        mHost->mFloatingWindow->GetWidth(&width);
        mHost->mFloatingWindow->Update((*mParentLocation)[0],
                (*mParentLocation)[1] - height,
                width, height);
    }
    return NOERROR;
}

// CAR_INTERFACE_IMPL_2(CPinyinIME::BuilderListener, IDialogInterfaceOnClickListener, IDialogInterfaceOnDismissListener);
//
// CPinyinIME::BuilderListener::BuilderListener(
//     /* [in] */ CPinyinIME* ime)
//     : mHost(ime)
// {
// }
//
// ECode CPinyinIME::BuilderListener::OnClick(
//     /* [in] */ IDialogInterface* di,
//     /* [in] */ Int32 position)
// {
//     assert(di != NULL);
//
//     Logger::D("CPinyinIME", "点击了按钮: %d", position);
//     switch (position) {
//         case IDialogInterface::BUTTON_POSITIVE:
//             mHost->LaunchSettings();
//             break;
//
//         case IDialogInterface::BUTTON_NEUTRAL:
//         case IDialogInterface::BUTTON_NEGATIVE:
//             assert(0 && "TODO");
//             // InputMethodManager.getInstance(PinyinIME.this)
//             //         .showInputMethodPicker();
//             break;
//     }
//
//     // di->Dismiss();
//     return NOERROR;
// }
//
// ECode CPinyinIME::BuilderListener::OnDismiss(
//     /* [in] */ IDialogInterface* dialog)
// {
//     mHost->mOptionsDialog = NULL;
//     return NOERROR;
// }


//==========================================================
// CPinyinIME::ChoiceNotifier
//==========================================================
CAR_INTERFACE_IMPL(CPinyinIME::ChoiceNotifier, Handler, ICandidateViewListener);

CPinyinIME::ChoiceNotifier::ChoiceNotifier(
    /* [in] */ CPinyinIME* ime)
    : mIme(ime)
{}

ECode CPinyinIME::ChoiceNotifier::OnClickChoice(
    /* [in] */ Int32 choiceId)
{
    if (choiceId >= 0) {
        mIme->OnChoiceTouched(choiceId);
    }
    return NOERROR;
}

ECode CPinyinIME::ChoiceNotifier::OnToLeftGesture()
{
    if (STATE_COMPOSING == mIme->mImeState) {
        mIme->ChangeToStateInput(TRUE);
    }
    ((CCandidatesContainer*)mIme->mCandidatesContainer.Get())->PageForward(TRUE, FALSE);
    return NOERROR;
}

ECode CPinyinIME::ChoiceNotifier::OnToRightGesture()
{
    if (STATE_COMPOSING == mIme->mImeState) {
        mIme->ChangeToStateInput(TRUE);
    }
    ((CCandidatesContainer*)mIme->mCandidatesContainer.Get())->PageBackward(TRUE, FALSE);
    return NOERROR;
}

ECode CPinyinIME::ChoiceNotifier::OnToTopGesture()
{
    return NOERROR;
}

ECode CPinyinIME::ChoiceNotifier::OnToBottomGesture()
{
    return NOERROR;
}


//==========================================================
// CPinyinIME::OnGestureListener
//==========================================================
const Int32 CPinyinIME::OnGestureListener::MIN_X_FOR_DRAG = 60;
const Int32 CPinyinIME::OnGestureListener::MIN_Y_FOR_DRAG = 40;
const Float CPinyinIME::OnGestureListener::VELOCITY_THRESHOLD_X1 = 0.3f;
const Float CPinyinIME::OnGestureListener::VELOCITY_THRESHOLD_X2 = 0.7f;
const Float CPinyinIME::OnGestureListener::VELOCITY_THRESHOLD_Y1 = 0.2f;
const Float CPinyinIME::OnGestureListener::VELOCITY_THRESHOLD_Y2 = 0.45f;

CPinyinIME::OnGestureListener::OnGestureListener(
    /* [in] */ Boolean reponseGestures,
    /* [in] */ CPinyinIME* ime)
    : mReponseGestures(reponseGestures)
    , mMinVelocityX(Elastos::Core::Math::FLOAT_MAX_VALUE)
    , mMinVelocityY(Elastos::Core::Math::FLOAT_MAX_VALUE)
    , mTimeDown(0)
    , mTimeLastOnScroll(0)
    , mNotGesture(FALSE)
    , mGestureRecognized(FALSE)
    , mIme(ime)
{}

ECode CPinyinIME::OnGestureListener::OnDown(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    mMinVelocityX = Elastos::Core::Math::INT32_MAX_VALUE;
    mMinVelocityY = Elastos::Core::Math::INT32_MAX_VALUE;
    IInputEvent::Probe(e)->GetEventTime(&mTimeDown);
    mTimeLastOnScroll = mTimeDown;
    mNotGesture = FALSE;
    mGestureRecognized = FALSE;
    *res = FALSE;
    return NOERROR;
}

ECode CPinyinIME::OnGestureListener::OnScroll(
    /* [in] */ IMotionEvent* e1,
    /* [in] */ IMotionEvent* e2,
    /* [in] */ Float distanceX,
    /* [in] */ Float distanceY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if (mNotGesture) {
        *res = FALSE;
        return NOERROR;
    }
    if (mGestureRecognized) {
        *res = TRUE;
        return NOERROR;
    }

    Float x1 = 0.f, y1 = 0.f, x2 = 0.f, y2 = 0.f;
    e1->GetX(&x1);
    e1->GetY(&y1);
    e2->GetX(&x2);
    e2->GetY(&y2);
    if (Elastos::Core::Math::Abs(x1 - x2) < MIN_X_FOR_DRAG
            && Elastos::Core::Math::Abs(y1 - y2) < MIN_Y_FOR_DRAG) {
        *res = FALSE;
        return NOERROR;
    }

    Int64 timeNow = 0;
    IInputEvent::Probe(e2)->GetEventTime(&timeNow);
    Int64 spanTotal = timeNow - mTimeDown;
    Int64 spanThis = timeNow - mTimeLastOnScroll;
    if (0 == spanTotal) spanTotal = 1;
    if (0 == spanThis) spanThis = 1;

    Float vXTotal = (x2 - x1) / spanTotal;
    Float vYTotal = (y2 - y1) / spanTotal;

    // The distances are from the current point to the previous one.
    Float vXThis = -distanceX / spanThis;
    Float vYThis = -distanceY / spanThis;

    Float kX = vXTotal * vXThis;
    Float kY = vYTotal * vYThis;
    Float k1 = kX + kY;
    Float k2 = Elastos::Core::Math::Abs(kX) + Elastos::Core::Math::Abs(kY);

    if (k1 / k2 < 0.8) {
        mNotGesture = TRUE;
        *res = FALSE;
        return NOERROR;
    }
    Float absVXTotal = Elastos::Core::Math::Abs(vXTotal);
    Float absVYTotal = Elastos::Core::Math::Abs(vYTotal);
    if (absVXTotal < mMinVelocityX) {
        mMinVelocityX = absVXTotal;
    }
    if (absVYTotal < mMinVelocityY) {
        mMinVelocityY = absVYTotal;
    }

    if (mMinVelocityX < VELOCITY_THRESHOLD_X1
            && mMinVelocityY < VELOCITY_THRESHOLD_Y1) {
        mNotGesture = TRUE;
        *res = FALSE;
        return NOERROR;
    }

    if (vXTotal > VELOCITY_THRESHOLD_X2
            && absVYTotal < VELOCITY_THRESHOLD_Y2) {
        if (mReponseGestures) OnDirectionGesture(IGravity::RIGHT);
        mGestureRecognized = TRUE;
    }
    else if (vXTotal < -VELOCITY_THRESHOLD_X2
            && absVYTotal < VELOCITY_THRESHOLD_Y2) {
        if (mReponseGestures) OnDirectionGesture(IGravity::LEFT);
        mGestureRecognized = TRUE;
    }
    else if (vYTotal > VELOCITY_THRESHOLD_Y2
            && absVXTotal < VELOCITY_THRESHOLD_X2) {
        if (mReponseGestures) OnDirectionGesture(IGravity::BOTTOM);
        mGestureRecognized = TRUE;
    }
    else if (vYTotal < -VELOCITY_THRESHOLD_Y2
            && absVXTotal < VELOCITY_THRESHOLD_X2) {
        if (mReponseGestures) OnDirectionGesture(IGravity::TOP);
        mGestureRecognized = TRUE;
    }

    mTimeLastOnScroll = timeNow;
    *res = mGestureRecognized;
    return NOERROR;
}

ECode CPinyinIME::OnGestureListener::OnFling(
    /* [in] */ IMotionEvent* me1,
    /* [in] */ IMotionEvent* me2,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = mGestureRecognized;
    return NOERROR;
}

void CPinyinIME::OnGestureListener::OnDirectionGesture(
    /* [in] */ Int32 gravity)
{
    if (IGravity::NO_GRAVITY == gravity) {
        return;
    }

    if (IGravity::LEFT == gravity || IGravity::RIGHT == gravity) {
        CCandidatesContainer* container = (CCandidatesContainer*)mIme->mCandidatesContainer.Get();
        Boolean shown = FALSE;
        if (container->IsShown(&shown), shown) {
            if (IGravity::LEFT == gravity) {
                container->PageForward(TRUE, TRUE);
            }
            else {
                container->PageBackward(TRUE, TRUE);
            }
            return;
        }
    }
}


//==========================================================
// CPinyinIME::PinyinDecoderServiceConnection
//==========================================================
CAR_INTERFACE_IMPL(CPinyinIME::PinyinDecoderServiceConnection, Object, IServiceConnection);

CPinyinIME::PinyinDecoderServiceConnection::PinyinDecoderServiceConnection(
    /* [in] */ CPinyinIME* ime)
    : mIme(ime)
{}

ECode CPinyinIME::PinyinDecoderServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    mIme->mDecInfo->mIPinyinDecoderService = IIPinyinDecoderService::Probe(service);
    return NOERROR;
}

ECode CPinyinIME::PinyinDecoderServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return NOERROR;
}


//==========================================================
// CPinyinIME::PinyinDecoderServiceConnection
//==========================================================
const Int32 CPinyinIME::DecodingInfo::PY_STRING_MAX = 28;
const Int32 CPinyinIME::DecodingInfo::MAX_PAGE_SIZE_DISPLAY = 10;

CPinyinIME::DecodingInfo::DecodingInfo(
    /* [in] */ CPinyinIME* ime)
    : mTotalChoicesNum(0)
    , mPosDelSpl(-1)
    , mIsPosInSpl(FALSE)
    , mSurfaceDecodedLen(0)
    , mActiveCmpsLen(0)
    , mActiveCmpsDisplayLen(0)
    , mFixedLen(0)
    , mFinishSelection(FALSE)
    , mCursorPos(0)
    , mHost(ime)
{
    mSurface = new StringBuffer();
}

void CPinyinIME::DecodingInfo::Reset()
{
    Int32 length = 0;
    mSurface->GetLength(&length);
    mSurface->Delete(0, length);
    mSurfaceDecodedLen = 0;
    mCursorPos = 0;
    mFullSent = "";
    mFixedLen = 0;
    mFinishSelection = FALSE;
    mComposingStr = "";
    mComposingStrDisplay = "";
    mActiveCmpsLen = 0;
    mActiveCmpsDisplayLen = 0;

    ResetCandidates();
}

Boolean CPinyinIME::DecodingInfo::IsCandidatesListEmpty()
{
    return mCandidatesList.IsEmpty();
}

Boolean CPinyinIME::DecodingInfo::IsSplStrFull()
{
    Int32 length = 0;
    mSurface->GetLength(&length);
    if (length >= PY_STRING_MAX - 1) return TRUE;
    return FALSE;
}

void CPinyinIME::DecodingInfo::AddSplChar(
    /* [in] */ Char32 ch,
    /* [in] */ Boolean reset)
{
    if (reset) {
        Int32 length = 0;
        mSurface->GetLength(&length);
        mSurface->Delete(0, length);
        mSurfaceDecodedLen = 0;
        mCursorPos = 0;
        // try {
        mIPinyinDecoderService->ImResetSearch();
        // } catch (RemoteException e) {
        // }
    }
    mSurface->InsertChar(mCursorPos, ch);
    mCursorPos++;
}

void CPinyinIME::DecodingInfo::PrepareDeleteBeforeCursor()
{
    if (mCursorPos > 0) {
        Int32 pos;
        for (pos = 0; pos < mFixedLen; pos++) {
            if ((*mSplStart)[pos + 2] >= mCursorPos
                    && (*mSplStart)[pos + 1] < mCursorPos) {
                mPosDelSpl = pos;
                mCursorPos = (*mSplStart)[pos + 1];
                mIsPosInSpl = TRUE;
                break;
            }
        }
        if (mPosDelSpl < 0) {
            mPosDelSpl = mCursorPos - 1;
            mCursorPos--;
            mIsPosInSpl = FALSE;
        }
    }
}

Int32 CPinyinIME::DecodingInfo::GetLength()
{
    Int32 length;
    mSurface->GetLength(&length);
    return length;
}

Char32 CPinyinIME::DecodingInfo::GetCharAt(
    /* [in] */ Int32 index)
{
    Char32 ch;
    mSurface->GetCharAt(index, &ch);
    return ch;
}

AutoPtr<StringBuffer> CPinyinIME::DecodingInfo::GetOrigianlSplStr()
{
    return mSurface;
}

Int32 CPinyinIME::DecodingInfo::GetSplStrDecodedLen()
{
    return mSurfaceDecodedLen;
}

AutoPtr< ArrayOf<Int32> > CPinyinIME::DecodingInfo::GetSplStart()
{
    return mSplStart;
}

String CPinyinIME::DecodingInfo::GetComposingStr()
{
    return mComposingStr;
}

String CPinyinIME::DecodingInfo::GetComposingStrActivePart()
{
    assert (mActiveCmpsLen <= (Int32)mComposingStr.GetLength());
    return mComposingStr.Substring(0, mActiveCmpsLen);
}

Int32 CPinyinIME::DecodingInfo::GetActiveCmpsLen()
{
    return mActiveCmpsLen;
}

String CPinyinIME::DecodingInfo::GetComposingStrForDisplay()
{
    return mComposingStrDisplay;
}

Int32 CPinyinIME::DecodingInfo::GetActiveCmpsDisplayLen()
{
    return mActiveCmpsDisplayLen;
}

String CPinyinIME::DecodingInfo::GetFullSent()
{
    return mFullSent;
}

String CPinyinIME::DecodingInfo::GetCurrentFullSent(
    /* [in] */ Int32 activeCandPos)
{
    // try {
    String retStr = mFullSent.Substring(0, mFixedLen);
    retStr += mCandidatesList[activeCandPos];
    return retStr;
    // } catch (Exception e) {
    // //TODO
    //     return "";
    // }
}

void CPinyinIME::DecodingInfo::ResetCandidates()
{
    mCandidatesList.Clear();
    mTotalChoicesNum = 0;

    mPageStart.Clear();
    mPageStart.PushBack(0);
    mCnToPage.Clear();
    mCnToPage.PushBack(0);
}

Boolean CPinyinIME::DecodingInfo::CandidatesFromApp()
{
    return STATE_APP_COMPLETION == mHost->mImeState;
}

Boolean CPinyinIME::DecodingInfo::CanDoPrediction()
{
    return mComposingStr.GetLength() == mFixedLen;
}

Boolean CPinyinIME::DecodingInfo::SelectionFinished()
{
    return mFinishSelection;
}

void CPinyinIME::DecodingInfo::ChooseDecodingCandidate(
    /* [in] */ Int32 candId)
{
    if (mHost->mImeState != STATE_PREDICT) {
        ResetCandidates();
        Int32 totalChoicesNum = 0;
        // try {
        if (candId < 0) {
            if (GetLength() == 0) {
                totalChoicesNum = 0;
            }
            else {
                if (mPyBuf == NULL) {
                    mPyBuf = ArrayOf<Byte>::Alloc(PY_STRING_MAX);
                }
                for (Int32 i = 0; i < GetLength(); i++) {
                    (*mPyBuf)[i] = (Byte)GetCharAt(i);
                }
                (*mPyBuf)[GetLength()] = 0;

                if (mPosDelSpl < 0) {
                    mIPinyinDecoderService->ImSearch(mPyBuf, GetLength(), &totalChoicesNum);
                }
                else {
                    Boolean clear_fixed_this_step = TRUE;
                    if (STATE_COMPOSING == mHost->mImeState) {
                        clear_fixed_this_step = FALSE;
                    }
                    mIPinyinDecoderService->ImDelSearch(mPosDelSpl, mIsPosInSpl,
                            clear_fixed_this_step, &totalChoicesNum);
                    mPosDelSpl = -1;
                }
            }
        }
        else {
            mIPinyinDecoderService->ImChoose(candId, &totalChoicesNum);
        }
        // } catch (RemoteException e) {
        // }
        UpdateDecInfoForSearch(totalChoicesNum);
    }
}

void CPinyinIME::DecodingInfo::UpdateDecInfoForSearch(
    /* [in] */ Int32 totalChoicesNum)
{
    mTotalChoicesNum = totalChoicesNum;
    if (mTotalChoicesNum < 0) {
        mTotalChoicesNum = 0;
        return;
    }

    // try {
    String pyStr, tmp;
    mSplStart = NULL;
    ECode ec = mIPinyinDecoderService->ImGetSplStart((ArrayOf<Int32>**)&mSplStart);
    FAIL_GOTO(ec, label);
    ec = mIPinyinDecoderService->ImGetPyStr(FALSE, &pyStr);
    FAIL_GOTO(ec, label);
    ec = mIPinyinDecoderService->ImGetPyStrLen(TRUE, &mSurfaceDecodedLen);
    FAIL_GOTO(ec, label);
    assert(mSurfaceDecodedLen <= pyStr.GetLength());

    ec = mIPinyinDecoderService->ImGetChoice(0, &mFullSent);
    FAIL_GOTO(ec, label);
    ec = mIPinyinDecoderService->ImGetFixedLen(&mFixedLen);
    FAIL_GOTO(ec, label);

    // Update the surface string to the one kept by engine.
    mSurface->Replace(0, GetLength(), pyStr);

    if (mCursorPos > GetLength()) {
        mCursorPos = GetLength();
    }
    mSurface->Substring((*mSplStart)[mFixedLen + 1], &tmp);
    mComposingStr = mFullSent.Substring(0, mFixedLen) + tmp;

    mActiveCmpsLen = mComposingStr.GetLength();
    if (mSurfaceDecodedLen > 0) {
        mActiveCmpsLen = mActiveCmpsLen
                - (GetLength() - mSurfaceDecodedLen);
    }

    // Prepare the display string.
    if (0 == mSurfaceDecodedLen) {
        mComposingStrDisplay = mComposingStr;
        mActiveCmpsDisplayLen = mComposingStr.GetLength();
    }
    else {
        mComposingStrDisplay = mFullSent.Substring(0, mFixedLen);
        for (Int32 pos = mFixedLen + 1; pos < mSplStart->GetLength() - 1; pos++) {
            mSurface->Substring((*mSplStart)[pos], (*mSplStart)[pos + 1], &tmp);
            mComposingStrDisplay += tmp;
            if ((*mSplStart)[pos + 1] < mSurfaceDecodedLen) {
                mComposingStrDisplay += String(" ");
            }
        }
        mActiveCmpsDisplayLen = mComposingStrDisplay.GetLength();
        if (mSurfaceDecodedLen < GetLength()) {
            mSurface->Substring(mSurfaceDecodedLen, &tmp);
            mComposingStrDisplay += tmp;
        }
    }

    if (mSplStart->GetLength() == mFixedLen + 2) {
        mFinishSelection = TRUE;
    }
    else {
        mFinishSelection = FALSE;
    }
    // } catch (RemoteException e) {
    //     Log.w(TAG, "PinyinDecoderService died", e);
    // } catch (Exception e) {
        // mTotalChoicesNum = 0;
        // mComposingStr = "";
    // }

label:
    if (FAILED(ec)) {
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            // Log.w(TAG, "PinyinDecoderService died", e);
        }
        else {
            mTotalChoicesNum = 0;
            mComposingStr = "";
        }
    }
    // Prepare page 0.
    if (!mFinishSelection) {
        PreparePage(0);
    }
}

void CPinyinIME::DecodingInfo::ChoosePredictChoice(
    /* [in] */ Int32 choiceId)
{
    if (STATE_PREDICT != mHost->mImeState || choiceId < 0
            || choiceId >= mTotalChoicesNum) {
        return;
    }

    String tmp = mCandidatesList[choiceId];

    ResetCandidates();

    mCandidatesList.PushBack(tmp);
    mTotalChoicesNum = 1;

    mSurface->Replace(0, GetLength(), String(""));
    mCursorPos = 0;
    mFullSent = tmp;
    mFixedLen = tmp.GetLength();
    mComposingStr = mFullSent;
    mActiveCmpsLen = mFixedLen;

    mFinishSelection = TRUE;
}

String CPinyinIME::DecodingInfo::GetCandidate(
    /* [in] */ Int32 candId)
{
    // Only loaded items can be gotten, so we use mCandidatesList.size()
    // instead mTotalChoiceNum.
    if (candId < 0 || candId > mCandidatesList.GetSize()) {
        return String(NULL);
    }
    return mCandidatesList[candId];
}

void CPinyinIME::DecodingInfo::GetCandiagtesForCache()
{
    Int32 fetchStart = mCandidatesList.GetSize();
    Int32 fetchSize = mTotalChoicesNum - fetchStart;
    if (fetchSize > MAX_PAGE_SIZE_DISPLAY) {
        fetchSize = MAX_PAGE_SIZE_DISPLAY;
    }
    // try {
    if (STATE_INPUT == mHost->mImeState ||
            STATE_IDLE == mHost->mImeState ||
            STATE_COMPOSING == mHost->mImeState){
        AutoPtr<ArrayOf<String> > newList;
        mIPinyinDecoderService->ImGetChoiceList(
                fetchStart, fetchSize, mFixedLen, (ArrayOf<String>**)&newList);
        if (newList != NULL && newList->GetLength() > 0) {
            for (Int32 i = 0; i < newList->GetLength(); i++) {
                mCandidatesList.PushBack((*newList)[i]);
            }
        }
    }
    else if (STATE_PREDICT == mHost->mImeState) {
        AutoPtr<ArrayOf<String> > newList;
        mIPinyinDecoderService->ImGetPredictList(
                fetchStart, fetchSize, (ArrayOf<String>**)&newList);
        if (newList != NULL && newList->GetLength() > 0) {
            for (Int32 i = 0; i < newList->GetLength(); i++) {
                mCandidatesList.PushBack((*newList)[i]);
            }
        }
    }
    else if (STATE_APP_COMPLETION == mHost->mImeState) {
        if (NULL != mAppCompletions) {
            for (Int32 pos = fetchStart; pos < fetchSize; pos++) {
                AutoPtr<ICompletionInfo> ci = (*mAppCompletions)[pos];
                if (NULL != ci) {
                    AutoPtr<ICharSequence> s;
                    ci->GetText((ICharSequence**)&s);
                    if (NULL != s) {
                        String str;
                        s->ToString(&str);
                        mCandidatesList.PushBack(str);
                    }
                }
            }
        }
    }
    // } catch (RemoteException e) {
    //     Log.w(TAG, "PinyinDecoderService died", e);
    // }
}

Boolean CPinyinIME::DecodingInfo::PageReady(
    /* [in] */ Int32 pageNo)
{
    // If the page number is less than 0, return FALSE
    if (pageNo < 0) return FALSE;

    // Page pageNo's ending information is not ready.
    if (mPageStart.GetSize() <= pageNo + 1) {
        return FALSE;
    }

    return TRUE;
}

Boolean CPinyinIME::DecodingInfo::PreparePage(
    /* [in] */ Int32 pageNo)
{
    // If the page number is less than 0, return FALSE
    if (pageNo < 0) return FALSE;

    // Make sure the starting information for page pageNo is ready.
    if (mPageStart.GetSize() <= pageNo) {
        return FALSE;
    }

    // Page pageNo's ending information is also ready.
    if (mPageStart.GetSize() > pageNo + 1) {
        return TRUE;
    }

    // If cached items is enough for page pageNo.
    if (mCandidatesList.GetSize() - mPageStart[pageNo] >= MAX_PAGE_SIZE_DISPLAY) {
        return TRUE;
    }

    // Try to get more items from engine
    GetCandiagtesForCache();

    // Try to find if there are available new items to display.
    // If no new item, return FALSE;
    if (mPageStart[pageNo] >= mCandidatesList.GetSize()) {
        return FALSE;
    }

    // If there are new items, return TRUE;
    return TRUE;
}

void CPinyinIME::DecodingInfo::PreparePredicts(
    /* [in] */ ICharSequence* history)
{
    if (NULL == history) return;

    ResetCandidates();

    if (Settings::GetPrediction()) {
        String preEdit;
        history->ToString(&preEdit);
        if (!preEdit.IsNull()) {
            // try {
            ECode ec = mIPinyinDecoderService->ImGetPredictsNum(preEdit, &mTotalChoicesNum);
            if (FAILED(ec)) return;
            // } catch (RemoteException e) {
            //     return;
            // }
        }
    }

    PreparePage(0);
    mFinishSelection = FALSE;
}

void CPinyinIME::DecodingInfo::PrepareAppCompletions(
    /* [in] */ ArrayOf<ICompletionInfo*>* completions)
{
    ResetCandidates();
    mAppCompletions = completions;
    mTotalChoicesNum = completions->GetLength();
    PreparePage(0);
    mFinishSelection = FALSE;
}

Int32 CPinyinIME::DecodingInfo::GetCurrentPageSize(
    /* [in] */ Int32 currentPage)
{
    if (mPageStart.GetSize() <= currentPage + 1) return 0;
    return mPageStart[currentPage + 1]
            - mPageStart[currentPage];
}

Int32 CPinyinIME::DecodingInfo::GetCurrentPageStart(
    /* [in] */ Int32 currentPage)
{
    if (mPageStart.GetSize() < currentPage + 1) return mTotalChoicesNum;
    return mPageStart[currentPage];
}

Boolean CPinyinIME::DecodingInfo::PageForwardable(
    /* [in] */ Int32 currentPage)
{
    if (mPageStart.GetSize() <= currentPage + 1) return FALSE;
    if (mPageStart[currentPage + 1] >= mTotalChoicesNum) {
        return FALSE;
    }
    return TRUE;
}

Boolean CPinyinIME::DecodingInfo::PageBackwardable(
    /* [in] */ Int32 currentPage)
{
    if (currentPage > 0) return TRUE;
    return FALSE;
}

Boolean CPinyinIME::DecodingInfo::CharBeforeCursorIsSeparator()
{
    Int32 len = GetLength();
    if (mCursorPos > len) return FALSE;
    Char32 ch = 0;
    if (mCursorPos > 0 && (mSurface->GetCharAt(mCursorPos - 1, &ch), ch == '\'')) {
        return TRUE;
    }
    return FALSE;
}

Int32 CPinyinIME::DecodingInfo::GetCursorPos()
{
    return mCursorPos;
}

Int32 CPinyinIME::DecodingInfo::GetCursorPosInCmps()
{
    Int32 cursorPos = mCursorPos;

    for (Int32 hzPos = 0; hzPos < mFixedLen; hzPos++) {
        if (mCursorPos >= (*mSplStart)[hzPos + 2]) {
            cursorPos -= (*mSplStart)[hzPos + 2] - (*mSplStart)[hzPos + 1];
            cursorPos += 1;
        }
    }
    return cursorPos;
}

Int32 CPinyinIME::DecodingInfo::GetCursorPosInCmpsDisplay()
{
    Int32 cursorPos = GetCursorPosInCmps();
    // +2 is because: one for mSplStart[0], which is used for other
    // purpose(The length of the segmentation string), and another
    // for the first spelling which does not need a space before it.
    for (Int32 pos = mFixedLen + 2; pos < mSplStart->GetLength() - 1; pos++) {
        if (mCursorPos <= (*mSplStart)[pos]) {
            break;
        }
        else {
            cursorPos++;
        }
    }
    return cursorPos;
}

void CPinyinIME::DecodingInfo::MoveCursorToEdge(
    /* [in] */ Boolean left)
{
    if (left) {
        mCursorPos = 0;
    }
    else {
        mSurface->GetLength(&mCursorPos);
    }
}

void CPinyinIME::DecodingInfo::MoveCursor(
    /* [in] */ Int32 offset)
{
    if (offset > 1 || offset < -1) return;

    if (offset != 0) {
        Int32 hzPos = 0;
        for (hzPos = 0; hzPos <= mFixedLen; hzPos++) {
            if (mCursorPos == (*mSplStart)[hzPos + 1]) {
                if (offset < 0) {
                    if (hzPos > 0) {
                        offset = (*mSplStart)[hzPos] - (*mSplStart)[hzPos + 1];
                    }
                }
                else {
                    if (hzPos < mFixedLen) {
                        offset = (*mSplStart)[hzPos + 2] - (*mSplStart)[hzPos + 1];
                    }
                }
                break;
            }
        }
    }
    mCursorPos += offset;
    if (mCursorPos < 0) {
        mCursorPos = 0;
    }
    else if (mCursorPos > GetLength()) {
        mCursorPos = GetLength();
    }
}

Int32 CPinyinIME::DecodingInfo::GetSplNum()
{
    return (*mSplStart)[0];
}

Int32 CPinyinIME::DecodingInfo::GetFixedLen()
{
    return mFixedLen;
}


//==========================================================
// CPinyinIME::MyReceiver
//==========================================================
ECode CPinyinIME::MyReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    SoundManager::GetInstance(context)->UpdateRingerMode();
    return NOERROR;
}

//==========================================================
// CPinyinIME
//==========================================================
const String CPinyinIME::TAG("PinyinIME");
const Boolean CPinyinIME::SIMULATE_KEY_DELETE = TRUE;

CPinyinIME::CPinyinIME()
    : mImeState(STATE_IDLE)
{
    mFloatingWindowTimer = new PopupTimer(this);
    mDecInfo = new DecodingInfo(this);
    mReceiver = new MyReceiver();
}

CPinyinIME::~CPinyinIME()
{}

ECode CPinyinIME::constructor()
{
    return NOERROR;
}

ECode CPinyinIME::OnCreate()
{
    mEnvironment = Environment::GetInstance();
    // if (mEnvironment.needDebug()) {
    //     Log.d(TAG, "onCreate.");
    // }
    FAIL_RETURN(InputMethodService::OnCreate());

    StartPinyinDecoderService();
    mImEn = new EnglishInputProcessor();

    AutoPtr<IContext> ctx;
    GetApplicationContext((IContext**)&ctx);
    AutoPtr<IPreferenceManagerHelper> pmHelper;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&pmHelper);
    AutoPtr<ISharedPreferences> sp;
    pmHelper->GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&sp);
    Settings::GetInstance(sp);

    mInputModeSwitcher = new InputModeSwitcher(this);
    mChoiceNotifier = new ChoiceNotifier(this);
    mGestureListenerSkb = new OnGestureListener(FALSE, this);
    mGestureListenerCandidates = new OnGestureListener(TRUE, this);
    CGestureDetector::New(this, mGestureListenerSkb, (IGestureDetector**)&mGestureDetectorSkb);
    CGestureDetector::New(this, mGestureListenerCandidates, (IGestureDetector**)&mGestureDetectorCandidates);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    mEnvironment->OnConfigurationChanged(config, this);
    return NOERROR;
}

#if 0
//add by kinier for fix the candidate word can not display bug
ECode CPinyinIME::OnEvaluateFullscreenMode(
    /* [out] */ Boolean* screenMode)
{
    VALIDATE_NOT_NULL(screenMode);
    *screenMode = FALSE;
    return NOERROR;
}

ECode CPinyinIME::OnDestroy()
{
    // if (mEnvironment.needDebug()) {
    //     Log.d(TAG, "onDestroy.");
    // }
    InputMethodService::UnbindService(mPinyinDecoderServiceConnection);
    AutoPtr<ISettings> settings;
    CPinyinSettings::AcquireSingleton((ISettings**)&settings);
    settings->ReleaseInstance();
    return InputMethodService::OnDestroy();
}

ECode CPinyinIME::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    AutoPtr<IPinyinEnvironmentHelper> helper;
    CPinyinEnvironmentHelper::AcquireSingleton((IPinyinEnvironmentHelper**)&helper);
    AutoPtr<IPinyinEnvironment> env;
    helper->GetInstance((IPinyinEnvironment**)&env);
    // if (mEnvironment.needDebug()) {
    //     Log.d(TAG, "onConfigurationChanged");
    //     Log.d(TAG, "--last config: " + env.getConfiguration().toString());
    //     Log.d(TAG, "---new config: " + newConfig.toString());
    // }
    // We need to change the local environment first so that UI components
    // can get the environment instance to handle size issues. When
    // super.onConfigurationChanged() is called, onCreateCandidatesView()
    // and onCreateInputView() will be executed if necessary.
    env->OnConfigurationChanged(newConfig, this);

    // Clear related UI of the previous configuration.
    if (NULL != mSkbContainer) {
        mSkbContainer->DismissPopups();
    }
    if (NULL != mCandidatesBalloon) {
        mCandidatesBalloon->Dismiss();
    }

    InputMethodService::OnConfigurationChanged(newConfig);
    ResetToIdleState(FALSE);
    return NOERROR;
}

ECode CPinyinIME::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 repeat = 0;
    event->GetRepeatCount(&repeat);
    if (ProcessKey(event, 0 != repeat)) {
        *result = TRUE;
        return NOERROR;
    }

    return InputMethodService::OnKeyDown(keyCode, event, result);
}

ECode CPinyinIME::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (ProcessKey(event, TRUE)) {
        *result = TRUE;
        return NOERROR;
    }

    return InputMethodService::OnKeyUp(keyCode, event, result);
}

Boolean CPinyinIME::ProcessKey(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Boolean realAction)
{
    if (Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_BYPASS == mImeState) {
        return FALSE;
    }

    Int32 keyCode = 0;
    event->GetKeyCode(&keyCode);
    // SHIFT-SPACE is used to switch between Chinese and English
    // when HKB is on.
    Boolean pressed = FALSE;
    if (IKeyEvent::KEYCODE_SPACE == keyCode && (event->IsShiftPressed(&pressed), pressed)) {
        if (!realAction) {
            return TRUE;
        }

        Int32 icon = 0;
        mInputModeSwitcher->SwitchLanguageWithHkb(&icon);
        UpdateIcon(icon);
        ResetToIdleState(FALSE);

        Int32 allMetaState = IKeyEvent::META_ALT_ON | IKeyEvent::META_ALT_LEFT_ON
                | IKeyEvent::META_ALT_RIGHT_ON | IKeyEvent::META_SHIFT_ON
                | IKeyEvent::META_SHIFT_LEFT_ON
                | IKeyEvent::META_SHIFT_RIGHT_ON | IKeyEvent::META_SYM_ON;

        AutoPtr<IInputConnection> connection;
        InputMethodService::GetCurrentInputConnection((IInputConnection**)&connection);
        assert(connection != NULL);
        Boolean result = FALSE;
        connection->ClearMetaKeyStates(allMetaState, &result);
        return TRUE;
    }

    // If HKB is on to input English, by-pass the key event so that
    // default key listener will handle it.
    Boolean res = FALSE;
    if (mInputModeSwitcher->IsEnglishWithHkb(&res), res) {
        return FALSE;
    }

    if (ProcessFunctionKeys(keyCode, realAction)) {
        return TRUE;
    }

    Int32 keyChar = 0;
    if (keyCode >= IKeyEvent::KEYCODE_A && keyCode <= IKeyEvent::KEYCODE_Z) {
        keyChar = keyCode - IKeyEvent::KEYCODE_A + 'a';
    } else if (keyCode >= IKeyEvent::KEYCODE_0
            && keyCode <= IKeyEvent::KEYCODE_9) {
        keyChar = keyCode - IKeyEvent::KEYCODE_0 + '0';
    } else if (keyCode == IKeyEvent::KEYCODE_COMMA) {
        keyChar = ',';
    } else if (keyCode == IKeyEvent::KEYCODE_PERIOD) {
        keyChar = '.';
    } else if (keyCode == IKeyEvent::KEYCODE_SPACE) {
        keyChar = ' ';
    } else if (keyCode == IKeyEvent::KEYCODE_APOSTROPHE) {
        keyChar = '\'';
    }

    if (mInputModeSwitcher->IsEnglishWithSkb(&res), res) {
        AutoPtr<IInputConnection> connection;
        InputMethodService::GetCurrentInputConnection((IInputConnection**)&connection);
        mInputModeSwitcher->IsEnglishUpperCaseWithSkb(&res);
        return mImEn->ProcessKey(connection, event, res, realAction);
    } else if (mInputModeSwitcher->IsChineseText(&res), res) {
        if (mImeState == Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_IDLE ||
                mImeState == Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_APP_COMPLETION) {
            mImeState = Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_IDLE;
            return ProcessStateIdle(keyChar, keyCode, event, realAction);
        } else if (mImeState == Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_INPUT) {
            return ProcessStateInput(keyChar, keyCode, event, realAction);
        } else if (mImeState == Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_PREDICT) {
            return ProcessStatePredict(keyChar, keyCode, event, realAction);
        } else if (mImeState == Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_COMPOSING) {
            return ProcessStateEditComposing(keyChar, keyCode, event,
                    realAction);
        }
    } else {
        if (0 != keyChar && realAction) {
            StringBuilder builder;
            builder.AppendChar(keyChar);
            String result = builder.ToString();
            CommitResultText(result);
        }
    }

    return FALSE;
}

Boolean CPinyinIME::ProcessFunctionKeys(
    /* [in] */ Int32 keyCode,
    /* [in] */ Boolean realAction)
{
    // Back key is used to dismiss all popup UI in a soft keyboard.
    if (keyCode == IKeyEvent::KEYCODE_BACK) {
        Boolean result = FALSE;
        if (InputMethodService::IsInputViewShown(&result), result) {
            if (mSkbContainer->HandleBack(realAction, &result), result) return TRUE;
        }
    }

    // Chinese related input is handle separately.
    Boolean res = FALSE;
    if (mInputModeSwitcher->IsChineseText(&res), res) {
        return FALSE;
    }

    if (NULL != mCandidatesContainer && (mCandidatesContainer->IsShown(&res), res)
            && !(mDecInfo->IsCandidatesListEmpty(&res), res)) {
        if (keyCode == IKeyEvent::KEYCODE_DPAD_CENTER) {
            if (!realAction) return TRUE;

            ChooseCandidate(-1);
            return TRUE;
        }

        if (keyCode == IKeyEvent::KEYCODE_DPAD_LEFT) {
            if (!realAction) return TRUE;
            mCandidatesContainer->ActiveCurseBackward(&res);
            return TRUE;
        }

        if (keyCode == IKeyEvent::KEYCODE_DPAD_RIGHT) {
            if (!realAction) return TRUE;
            mCandidatesContainer->ActiveCurseForward(&res);
            return TRUE;
        }

        if (keyCode == IKeyEvent::KEYCODE_DPAD_UP) {
            if (!realAction) return TRUE;
            mCandidatesContainer->PageBackward(FALSE, TRUE, &res);
            return TRUE;
        }

        if (keyCode == IKeyEvent::KEYCODE_DPAD_DOWN) {
            if (!realAction) return TRUE;
            mCandidatesContainer->PageForward(FALSE, TRUE, &res);
            return TRUE;
        }

        if (keyCode == IKeyEvent::KEYCODE_DEL &&
                Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_PREDICT == mImeState) {
            if (!realAction) return TRUE;
            ResetToIdleState(FALSE);
            return TRUE;
        }
    } else {
        if (keyCode == IKeyEvent::KEYCODE_DEL) {
            if (!realAction) return TRUE;
            if (SIMULATE_KEY_DELETE) {
                SimulateKeyEventDownUp(keyCode);
            } else {
                AutoPtr<IInputConnection> connection;
                InputMethodService::GetCurrentInputConnection((IInputConnection**)&connection);
                assert(connection != NULL);
                connection->DeleteSurroundingText(1, 0, &res);
            }
            return TRUE;
        }
        if (keyCode == IKeyEvent::KEYCODE_ENTER) {
            if (!realAction) return TRUE;
            InputMethodService::SendKeyChar('\n');
            return TRUE;
        }
        if (keyCode == IKeyEvent::KEYCODE_SPACE) {
            if (!realAction) return TRUE;
            InputMethodService::SendKeyChar(' ');
            return TRUE;
        }
    }

    return FALSE;
}

Boolean CPinyinIME::ProcessStateIdle(
    /* [in] */ Int32 keyChar,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Boolean realAction)
{
    // In this status, when user presses keys in [a..z], the status will
    // change to input state.
    Boolean pressed = FALSE;
    event->IsAltPressed(&pressed);
    if (keyChar >= 'a' && keyChar <= 'z' && !pressed) {
        if (!realAction) return TRUE;
        mDecInfo->AddSplChar((Char32) keyChar, TRUE);
        ChooseAndUpdate(-1);
        return TRUE;
    } else if (keyCode == IKeyEvent::KEYCODE_DEL) {
        if (!realAction) return TRUE;
        if (SIMULATE_KEY_DELETE) {
            SimulateKeyEventDownUp(keyCode);
        } else {
            AutoPtr<IInputConnection> connection;
            InputMethodService::GetCurrentInputConnection((IInputConnection**)&connection);
            Boolean result = FALSE;
            connection->DeleteSurroundingText(1, 0, &result);
        }
        return TRUE;
    } else if (keyCode == IKeyEvent::KEYCODE_ENTER) {
        if (!realAction) return TRUE;
        InputMethodService::SendKeyChar('\n');
        return TRUE;
    } else if (keyCode == IKeyEvent::KEYCODE_ALT_LEFT
            || keyCode == IKeyEvent::KEYCODE_ALT_RIGHT
            || keyCode == IKeyEvent::KEYCODE_SHIFT_LEFT
            || keyCode == IKeyEvent::KEYCODE_SHIFT_RIGHT) {
        return TRUE;
    } else if (pressed) {
        Char32 fullwidth_char = KeyMapDream::GetChineseLabel(keyCode);
        if (0 != fullwidth_char) {
            if (realAction) {
                String result;
                result += fullwidth_char;
                CommitResultText(result);
            }
            return TRUE;
        } else {
            if (keyCode >= IKeyEvent::KEYCODE_A
                    && keyCode <= IKeyEvent::KEYCODE_Z) {
                return TRUE;
            }
        }
    } else if (keyChar != 0 && keyChar != '\t') {
        if (realAction) {
            if (keyChar == ',' || keyChar == '.') {
                InputCommaPeriod(String(""), keyChar, FALSE, Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_IDLE);
            } else {
                if (0 != keyChar) {
                    String result;
                    result += keyChar;
                    CommitResultText(result);
                }
            }
        }
        return TRUE;
    }
    return FALSE;
}

Boolean CPinyinIME::ProcessStateInput(
    /* [in] */ Int32 keyChar,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Boolean realAction)
{
    // If ALT key is pressed, input alternative key. But if the
    // alternative key is quote key, it will be used for input a splitter
    // in Pinyin string.
    Boolean pressed = FALSE;
    event->IsAltPressed(&pressed);
    if (pressed) {
        Int32 metaState = 0, unicodeChar = 0;
        event->GetMetaState(&metaState);
        if ('\'' != (event->GetUnicodeChar(metaState, &unicodeChar), unicodeChar)) {
            if (realAction) {
                Char32 fullwidth_char = KeyMapDream::GetChineseLabel(keyCode);
                if (0 != fullwidth_char) {
                    Int32 pos = 0;
                    mCandidatesContainer->GetActiveCandiatePos(&pos);
                    String str;
                    mDecInfo->GetCurrentFullSent(pos, &str);
                    CommitResultText(str + fullwidth_char);
                    ResetToIdleState(FALSE);
                }
            }
            return TRUE;
        } else {
            keyChar = '\'';
        }
    }

    Boolean state = FALSE;
    if (keyChar >= 'a' && keyChar <= 'z' || keyChar == '\''
            && !(mDecInfo->CharBeforeCursorIsSeparator(&state), state)
            || keyCode == IKeyEvent::KEYCODE_DEL) {
        if (!realAction) return TRUE;
        return ProcessSurfaceChange(keyChar, keyCode);
    } else if (keyChar == ',' || keyChar == '.') {
        if (!realAction) return TRUE;
        Int32 pos = 0;
        mCandidatesContainer->GetActiveCandiatePos(&pos);
        String str;
        mDecInfo->GetCurrentFullSent(pos, &str);
        InputCommaPeriod(str, keyChar, TRUE, Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_IDLE);
        return TRUE;
    } else if (keyCode == IKeyEvent::KEYCODE_DPAD_UP
            || keyCode == IKeyEvent::KEYCODE_DPAD_DOWN
            || keyCode == IKeyEvent::KEYCODE_DPAD_LEFT
            || keyCode == IKeyEvent::KEYCODE_DPAD_RIGHT) {
        if (!realAction) return TRUE;

        Boolean result = FALSE;
        if (keyCode == IKeyEvent::KEYCODE_DPAD_LEFT) {
            mCandidatesContainer->ActiveCurseBackward(&result);
        } else if (keyCode == IKeyEvent::KEYCODE_DPAD_RIGHT) {
            mCandidatesContainer->ActiveCurseForward(&result);
        } else if (keyCode == IKeyEvent::KEYCODE_DPAD_UP) {
            // If it has been the first page, a up key will shift
            // the state to edit composing string.
            if (!(mCandidatesContainer->PageBackward(FALSE, TRUE, &state), state)) {
                mCandidatesContainer->EnableActiveHighlight(FALSE);
                ChangeToStateComposing(TRUE);
                UpdateComposingText(TRUE);
            }
        } else if (keyCode == IKeyEvent::KEYCODE_DPAD_DOWN) {
            mCandidatesContainer->PageForward(FALSE, TRUE, &result);
        }
        return TRUE;
    } else if (keyCode >= IKeyEvent::KEYCODE_1
            && keyCode <= IKeyEvent::KEYCODE_9) {
        if (!realAction) return TRUE;

        Int32 activePos = keyCode - IKeyEvent::KEYCODE_1;
        Int32 currentPage = 0, value = 0;
        mCandidatesContainer->GetCurrentPage(&currentPage);
        if (activePos < (mDecInfo->GetCurrentPageSize(currentPage, &value), value)) {
            activePos = activePos + (mDecInfo->GetCurrentPageStart(currentPage, &value), value);
            if (activePos >= 0) {
                ChooseAndUpdate(activePos);
            }
        }
        return TRUE;
    } else if (keyCode == IKeyEvent::KEYCODE_ENTER) {
        if (!realAction) return TRUE;

        if (mInputModeSwitcher->IsEnterNoramlState(&state), state) {
            AutoPtr<IStringBuffer> buf;
            mDecInfo->GetOrigianlSplStr((IStringBuffer**)&buf);
            String text;
            buf->ToString(&text);
            CommitResultText(text);
            ResetToIdleState(FALSE);
        } else {
            Int32 pos = 0;
            mCandidatesContainer->GetActiveCandiatePos(&pos);
            String str;
            mDecInfo->GetCurrentFullSent(pos, &str);
            CommitResultText(str);
            InputMethodService::SendKeyChar('\n');
            ResetToIdleState(FALSE);
        }
        return TRUE;
    } else if (keyCode == IKeyEvent::KEYCODE_DPAD_CENTER
            || keyCode == IKeyEvent::KEYCODE_SPACE) {
        if (!realAction) return TRUE;
        ChooseCandidate(-1);
        return TRUE;
    } else if (keyCode == IKeyEvent::KEYCODE_BACK) {
        if (!realAction) return TRUE;
        ResetToIdleState(FALSE);
        RequestHideSelf(0);
        return TRUE;
    }
    return FALSE;
}

Boolean CPinyinIME::ProcessStatePredict(
    /* [in] */ Int32 keyChar,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Boolean realAction)
{
    if (!realAction) return TRUE;

    // If ALT key is pressed, input alternative key.
    Boolean pressed = FALSE;
    event->IsAltPressed(&pressed);
    if (pressed) {
        Char32 fullwidth_char = KeyMapDream::GetChineseLabel(keyCode);
        if (0 != fullwidth_char) {
            Int32 pos = 0;
            mCandidatesContainer->GetActiveCandiatePos(&pos);

            String str;
            mDecInfo->GetCandidate(pos, &str);
            CommitResultText(str + fullwidth_char);
            ResetToIdleState(FALSE);
        }
        return TRUE;
    }

    // In this status, when user presses keys in [a..z], the status will
    // change to input state.
    if (keyChar >= 'a' && keyChar <= 'z') {
        ChangeToStateInput(TRUE);
        mDecInfo->AddSplChar((Char32) keyChar, TRUE);
        ChooseAndUpdate(-1);
    } else if (keyChar == ',' || keyChar == '.') {
        InputCommaPeriod(String(""), keyChar, TRUE, Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_IDLE);
    } else if (keyCode == IKeyEvent::KEYCODE_DPAD_UP
            || keyCode == IKeyEvent::KEYCODE_DPAD_DOWN
            || keyCode == IKeyEvent::KEYCODE_DPAD_LEFT
            || keyCode == IKeyEvent::KEYCODE_DPAD_RIGHT) {
        Boolean result = FALSE;
        if (keyCode == IKeyEvent::KEYCODE_DPAD_LEFT) {
            mCandidatesContainer->ActiveCurseBackward(&result);
        }
        if (keyCode == IKeyEvent::KEYCODE_DPAD_RIGHT) {
            mCandidatesContainer->ActiveCurseForward(&result);
        }
        if (keyCode == IKeyEvent::KEYCODE_DPAD_UP) {
            mCandidatesContainer->PageBackward(FALSE, TRUE, &result);
        }
        if (keyCode == IKeyEvent::KEYCODE_DPAD_DOWN) {
            mCandidatesContainer->PageForward(FALSE, TRUE, &result);
        }
    } else if (keyCode == IKeyEvent::KEYCODE_DEL) {
        ResetToIdleState(FALSE);
    } else if (keyCode == IKeyEvent::KEYCODE_BACK) {
        ResetToIdleState(FALSE);
        RequestHideSelf(0);
    } else if (keyCode >= IKeyEvent::KEYCODE_1
            && keyCode <= IKeyEvent::KEYCODE_9) {
        Int32 activePos = keyCode - IKeyEvent::KEYCODE_1;
        Int32 currentPage = 0, value = 0;
        mCandidatesContainer->GetCurrentPage(&currentPage);
        if (activePos < (mDecInfo->GetCurrentPageSize(currentPage, &value), value)) {
            activePos = activePos + (mDecInfo->GetCurrentPageStart(currentPage, &value), value);
            if (activePos >= 0) {
                ChooseAndUpdate(activePos);
            }
        }
    } else if (keyCode == IKeyEvent::KEYCODE_ENTER) {
        InputMethodService::SendKeyChar('\n');
        ResetToIdleState(FALSE);
    } else if (keyCode == IKeyEvent::KEYCODE_DPAD_CENTER
            || keyCode == IKeyEvent::KEYCODE_SPACE) {
        ChooseCandidate(-1);
    }

    return TRUE;
}

Boolean CPinyinIME::ProcessStateEditComposing(
    /* [in] */ Int32 keyChar,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Boolean realAction)
{
    if (!realAction) return TRUE;

    Elastos::Droid::Inputmethods::PinyinIME::ComposingStatus cmpsvStatus;
    mComposingView->GetComposingStatus(&cmpsvStatus);

    // If ALT key is pressed, input alternative key. But if the
    // alternative key is quote key, it will be used for input a splitter
    // in Pinyin string.
    Boolean pressed = FALSE;
    if (event->IsAltPressed(&pressed), pressed) {
        Int32 metaState = 0, unicodeChar = 0;
        event->GetMetaState(&metaState);
        if ('\'' != (event->GetUnicodeChar(metaState, &unicodeChar), unicodeChar)) {
            Char32 fullwidth_char = KeyMapDream::GetChineseLabel(keyCode);
            if (0 != fullwidth_char) {
                String retStr;
                if (Elastos::Droid::Inputmethods::PinyinIME::ComposingStatus_SHOW_STRING_LOWERCASE == cmpsvStatus) {
                    AutoPtr<IStringBuffer> buf;
                    mDecInfo->GetOrigianlSplStr((IStringBuffer**)&buf);
                    buf->ToString(&retStr);
                } else {
                    mDecInfo->GetComposingStr(&retStr);
                }
                CommitResultText(retStr + fullwidth_char);
                ResetToIdleState(FALSE);
            }
            return TRUE;
        } else {
            keyChar = '\'';
        }
    }

    Boolean state = FALSE;
    if (keyCode == IKeyEvent::KEYCODE_DPAD_DOWN) {
        if (!(mDecInfo->SelectionFinished(&state), state)) {
            ChangeToStateInput(TRUE);
        }
    } else if (keyCode == IKeyEvent::KEYCODE_DPAD_LEFT
            || keyCode == IKeyEvent::KEYCODE_DPAD_RIGHT) {
        mComposingView->MoveCursor(keyCode, &state);
    } else if ((keyCode == IKeyEvent::KEYCODE_ENTER && (mInputModeSwitcher->IsEnterNoramlState(&state), state))
            || keyCode == IKeyEvent::KEYCODE_DPAD_CENTER
            || keyCode == IKeyEvent::KEYCODE_SPACE) {
        if (Elastos::Droid::Inputmethods::PinyinIME::ComposingStatus_SHOW_STRING_LOWERCASE == cmpsvStatus) {
            String str;
            AutoPtr<IStringBuffer> buf;
            mDecInfo->GetOrigianlSplStr((IStringBuffer**)&buf);
            buf->ToString(&str);
            if (!TryInputRawUnicode(str)) {
                CommitResultText(str);
            }
        } else if (Elastos::Droid::Inputmethods::PinyinIME::ComposingStatus_EDIT_PINYIN == cmpsvStatus) {
            String str;
            mDecInfo->GetComposingStr(&str);
            if (!TryInputRawUnicode(str)) {
                CommitResultText(str);
            }
        } else {
            String str;
            mDecInfo->GetComposingStr(&str);
            CommitResultText(str);
        }
        ResetToIdleState(FALSE);
    } else if (keyCode == IKeyEvent::KEYCODE_ENTER
            && !(mInputModeSwitcher->IsEnterNoramlState(&state), state)) {
        String retStr;
        if (!(mDecInfo->IsCandidatesListEmpty(&state), state)) {
            Int32 pos = 0;
            mCandidatesContainer->GetActiveCandiatePos(&pos);
            mDecInfo->GetCurrentFullSent(pos, &retStr);
        } else {
            mDecInfo->GetComposingStr(&retStr);
        }
        CommitResultText(retStr);
        InputMethodService::SendKeyChar('\n');
        ResetToIdleState(FALSE);
    } else if (keyCode == IKeyEvent::KEYCODE_BACK) {
        ResetToIdleState(FALSE);
        RequestHideSelf(0);
        return TRUE;
    } else {
        return ProcessSurfaceChange(keyChar, keyCode);
    }
    return TRUE;
}

Boolean CPinyinIME::TryInputRawUnicode(
    /* [in] */ const String& str)
{
    if (str.GetLength() > 7) {
        if (str.StartWith("unicode")) {
            // try {
            String digitStr = str.Substring(7);
            Int32 startPos = 0;
            Int32 radix = 10;
            if (digitStr.GetLength() > 2 && digitStr.StartWith("0x")) {
                startPos = 2;
                radix = 16;
            }
            digitStr = digitStr.Substring(startPos);
            Int32 unicode = StringUtils::ParseInt32(digitStr, radix);
            if (unicode > 0) {
                Char32 low = (Char32) (unicode & 0x0000ffff);
                Char32 high = (Char32) ((unicode & 0xffff0000) >> 16);
                String lowStr;
                lowStr += low;
                CommitResultText(lowStr);
                if (0 != high) {
                    String highStr;
                    highStr += high;
                    CommitResultText(highStr);
                }
            }
            return TRUE;
            // } catch (NumberFormatException e) {
            //     return FALSE;
            // }
        } else if (str.Substring(str.GetLength() - 7, str.GetLength()).Equals(String("unicode"))) {
            String resultStr = String("");
            AutoPtr<ArrayOf<Char32> > chars = str.GetChars();
            for (Int32 pos = 0; pos < chars->GetLength() - 7; pos++) {
                if (pos > 0) {
                    resultStr += String(" ");
                }

                resultStr += String("0x") + StringUtils::Int32ToHexString((*chars)[pos]);
            }
            CommitResultText(resultStr/*String.valueOf(resultStr)*/);
            return TRUE;
        }
    }
    return FALSE;
}

Boolean CPinyinIME::ProcessSurfaceChange(
    /* [in] */ Int32 keyChar,
    /* [in] */ Int32 keyCode)
{
    Boolean state = FALSE;
    if ((mDecInfo->IsSplStrFull(&state), state) && IKeyEvent::KEYCODE_DEL != keyCode) {
        return TRUE;
    }

    if ((keyChar >= 'a' && keyChar <= 'z')
            || (keyChar == '\'' && !(mDecInfo->CharBeforeCursorIsSeparator(&state), state))
            || (((keyChar >= '0' && keyChar <= '9') || keyChar == ' ') &&
                Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_COMPOSING == mImeState)) {
        mDecInfo->AddSplChar((Char32) keyChar, FALSE);
        ChooseAndUpdate(-1);
    } else if (keyCode == IKeyEvent::KEYCODE_DEL) {
        mDecInfo->PrepareDeleteBeforeCursor();
        ChooseAndUpdate(-1);
    }
    return TRUE;
}

void CPinyinIME::ChangeToStateComposing(
    /* [in] */ Boolean updateUi)
{
    mImeState = Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_COMPOSING;
    if (!updateUi) return;

    Boolean state = FALSE;
    if (NULL != mSkbContainer && (mSkbContainer->IsShown(&state), state)) {
        mSkbContainer->ToggleCandidateMode(TRUE);
    }
}

void CPinyinIME::ChangeToStateInput(
    /* [in] */ Boolean updateUi)
{
    mImeState = Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_INPUT;
    if (!updateUi) return;

    Boolean state = FALSE;
    if (NULL != mSkbContainer && (mSkbContainer->IsShown(&state), state)) {
        mSkbContainer->ToggleCandidateMode(TRUE);
    }
    ShowCandidateWindow(TRUE);
}

void CPinyinIME::SimulateKeyEventDownUp(
    /* [in] */ Int32 keyCode)
{
    AutoPtr<IInputConnection> ic;
    InputMethodService::GetCurrentInputConnection((IInputConnection**)&ic);
    if (NULL == ic) return;

    Boolean result = FALSE;
    AutoPtr<IKeyEvent> event1, event2;
    CKeyEvent::New(IKeyEvent::ACTION_DOWN, keyCode, (IKeyEvent**)&event1);
    ic->SendKeyEvent(event1, &result);

    CKeyEvent::New(IKeyEvent::ACTION_UP, keyCode, (IKeyEvent**)&event2);
    ic->SendKeyEvent(event2, &result);
}

void CPinyinIME::CommitResultText(
    /* [in] */ const String& resultText)
{
    AutoPtr<IInputConnection> ic;
    InputMethodService::GetCurrentInputConnection((IInputConnection**)&ic);
    if (NULL != ic) {
        Boolean result = FALSE;
        AutoPtr<ICharSequence> text;
        CStringWrapper::New(resultText, (ICharSequence**)&text);
        ic->CommitText(text, 1, &result);
    }
    if (NULL != mComposingView) {
        mComposingView->SetVisibility(IView::INVISIBLE);
        mComposingView->Invalidate();
    }
}

void CPinyinIME::UpdateComposingText(
    /* [in] */ Boolean visible)
{
    if (!visible) {
        mComposingView->SetVisibility(IView::INVISIBLE);
    } else {
        mComposingView->SetDecodingInfo(mDecInfo, mImeState);
        mComposingView->SetVisibility(IView::VISIBLE);
    }
    mComposingView->Invalidate();
}

void CPinyinIME::InputCommaPeriod(
    /* [in] */ const String& preEdit,
    /* [in] */ Int32 keyChar,
    /* [in] */ Boolean dismissCandWindow,
    /* [in] */ ImeState nextState)
{
    StringBuilder buffer(preEdit);
    if (keyChar == ',')
        buffer.AppendChar(0xff0c/*'\uff0c'*/);
    else if (keyChar == '.')
        buffer.AppendChar(0x3002/*'\u3002'*/);
    else
        return;
    CommitResultText(buffer.ToString());
    if (dismissCandWindow) ResetCandidateWindow();
    mImeState = nextState;
}

void CPinyinIME::ResetToIdleState(
    /* [in] */ Boolean resetInlineText)
{
    if (Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_IDLE == mImeState) return;

    mImeState = Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_IDLE;
    mDecInfo->Reset();

    if (NULL != mComposingView) mComposingView->Reset();
    if (resetInlineText) CommitResultText(String(""));
    ResetCandidateWindow();
}

void CPinyinIME::ChooseAndUpdate(
    /* [in] */ Int32 candId)
{
    Boolean state = FALSE;
    if (!(mInputModeSwitcher->IsChineseText(&state), state)) {
        String choice;
        mDecInfo->GetCandidate(candId, &choice);
        if (NULL != choice) {
            CommitResultText(choice);
        }
        ResetToIdleState(FALSE);
        return;
    }

    if (Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_PREDICT != mImeState) {
        // Get result candidate list, if choice_id < 0, do a new decoding.
        // If choice_id >=0, select the candidate, and get the new candidate
        // list.
        mDecInfo->ChooseDecodingCandidate(candId);
    } else {
        // Choose a prediction item.
        mDecInfo->ChoosePredictChoice(candId);
    }

    String comStr;
    mDecInfo->GetComposingStr(&comStr);
    if (!comStr.IsNullOrEmpty()) {
        String resultStr;
        mDecInfo->GetComposingStrActivePart(&resultStr);

        // choiceId >= 0 means user finishes a choice selection.
        if (candId >= 0 && (mDecInfo->CanDoPrediction(&state), state)) {
            CommitResultText(resultStr);
            mImeState = Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_PREDICT;
            if (NULL != mSkbContainer && (mSkbContainer->IsShown(&state), state)) {
                mSkbContainer->ToggleCandidateMode(FALSE);
            }
            AutoPtr<ISettings> settings;
            CPinyinSettings::AcquireSingleton((ISettings**)&settings);
            // Try to get the prediction list.
            if (settings->GetPrediction(&state), state) {
                AutoPtr<IInputConnection> ic;
                InputMethodService::GetCurrentInputConnection((IInputConnection**)&ic);
                if (NULL != ic) {
                    AutoPtr<ICharSequence> cs;
                    ic->GetTextBeforeCursor(3, 0, (ICharSequence**)&cs);
                    if (NULL != cs) {
                        mDecInfo->PreparePredicts(cs);
                    }
                }
            } else {
                mDecInfo->ResetCandidates();
            }

            AutoPtr<ArrayOf<String> > candiList;
            mDecInfo->GetCandidatesList((ArrayOf<String>**)&candiList);
            if (candiList != NULL && candiList->GetLength() > 0) {
                ShowCandidateWindow(FALSE);
            } else {
                ResetToIdleState(FALSE);
            }
        } else {
            if (Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_IDLE == mImeState) {
                Int32 value = 0;
                if ((mDecInfo->GetSplStrDecodedLen(&value), value) == 0) {
                    ChangeToStateComposing(TRUE);
                } else {
                    ChangeToStateInput(TRUE);
                }
            } else {
                Boolean state = FALSE;
                if ((mDecInfo->SelectionFinished(&state), state)) {
                    ChangeToStateComposing(TRUE);
                }
            }
            ShowCandidateWindow(TRUE);
        }
    } else {
        ResetToIdleState(FALSE);
    }
}

void CPinyinIME::ChooseCandidate(
    /* [in] */ Int32 activeCandNo)
{
    if (activeCandNo < 0) {
        mCandidatesContainer->GetActiveCandiatePos(&activeCandNo);
    }
    if (activeCandNo >= 0) {
        ChooseAndUpdate(activeCandNo);
    }
}

Boolean CPinyinIME::StartPinyinDecoderService()
{
    AutoPtr<IPinyinDecoderService> service;
    mDecInfo->GetPinyinDecoderService((IPinyinDecoderService**)&service);
    if (NULL == service) {
        AutoPtr<IIntent> serviceIntent;
        CIntent::New((IIntent**)&serviceIntent);
        serviceIntent->SetClassName(this, String("PinyinIME.CPinyinDecoderService"));

        if (NULL == mPinyinDecoderServiceConnection) {
            mPinyinDecoderServiceConnection = new PinyinDecoderServiceConnection(this);
        }

        Boolean result = FALSE;
        // Bind service
        if (InputMethodService::BindService(serviceIntent, mPinyinDecoderServiceConnection,
                    IContext::BIND_AUTO_CREATE, &result), result) {
            return TRUE;
        } else {
            return FALSE;
        }
    }
    return TRUE;
}

ECode CPinyinIME::OnCreateCandidatesView(
    /* [out] */ IView** retView)
{
    // if (mEnvironment.needDebug()) {
    //     Log.d(TAG, "onCreateCandidatesView.");
    // }

    AutoPtr<ILayoutInflater> inflater;
    InputMethodService::GetLayoutInflater((ILayoutInflater**)&inflater);
    // Inflate the floating container view
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::floating_container, NULL, (IView**)&view);
    mFloatingContainer = ILinearLayout::Probe(view);

    // The first child is the composing view.
    view = NULL;
    mFloatingContainer->GetChildAt(0, (IView**)&view);
    mComposingView = IComposingView::Probe(view);

    view = NULL;
    inflater->Inflate(R::layout::candidates_container, NULL, (IView**)&view);
    mCandidatesContainer = ICandidatesContainer::Probe(view);

    // Create balloon hint for candidates view.
    const Int32 MODE_SHIFT = 30;
    const Int32 UNSPECIFIED = 0 << MODE_SHIFT;
    CBalloonHint::New(this, mCandidatesContainer, UNSPECIFIED/*MeasureSpec::UNSPECIFIED*/, (IBalloonHint**)&mCandidatesBalloon);

    AutoPtr<IResources> res;
    InputMethodService::GetResources((IResources**)&res);
    AutoPtr<IDrawable> dr;
    res->GetDrawable(R::drawable::candidate_balloon_bg, (IDrawable**)&dr);
    mCandidatesBalloon->SetBalloonBackground(dr);
    assert(mCandidatesContainer != NULL);
    mCandidatesContainer->Initialize(mChoiceNotifier, mCandidatesBalloon, mGestureDetectorCandidates);

    // The floating window
    Boolean state = FALSE;
    if (NULL != mFloatingWindow && (mFloatingWindow->IsShowing(&state), state)) {
        mFloatingWindowTimer->CancelShowing();
        mFloatingWindow->Dismiss();
    }
    CPopupWindow::New(this, (IPopupWindow**)&mFloatingWindow);
    mFloatingWindow->SetClippingEnabled(FALSE);
    mFloatingWindow->SetBackgroundDrawable(NULL);
    mFloatingWindow->SetInputMethodMode(IPopupWindow::INPUT_METHOD_NOT_NEEDED);
    mFloatingWindow->SetContentView(mFloatingContainer);

    InputMethodService::SetCandidatesViewShown(TRUE);
    *retView = mCandidatesContainer;
    REFCOUNT_ADD(*retView);
    return NOERROR;
}

ECode CPinyinIME::ResponseSoftKeyEvent(
    /* [in] */ ISoftKey* sKey)
{
    if (NULL == sKey) return NOERROR;

    AutoPtr<IInputConnection> ic;
    InputMethodService::GetCurrentInputConnection((IInputConnection**)&ic);
    if (ic == NULL) return NOERROR;

    Int32 keyCode = 0;
    sKey->GetKeyCode(&keyCode);
    Boolean result = FALSE;
    // Process some general keys, including KEYCODE_DEL, KEYCODE_SPACE,
    // KEYCODE_ENTER and KEYCODE_DPAD_CENTER.
    if (sKey->IsKeyCodeKey(&result), result) {
        if (ProcessFunctionKeys(keyCode, TRUE)) return NOERROR;
    }

    if (sKey->IsUserDefKey(&result), result) {
        Int32 icon = 0;
        mInputModeSwitcher->SwitchModeForUserKey(keyCode, &icon);
        UpdateIcon(icon);
        ResetToIdleState(FALSE);
        mSkbContainer->UpdateInputMode();
    } else {
        if (sKey->IsKeyCodeKey(&result), result) {
            AutoPtr<IKeyEvent> eDown;
            AutoPtr<IKeyEvent> eUp;
            CKeyEvent::New(0, 0, IKeyEvent::ACTION_DOWN,
                    keyCode, 0, 0, 0, 0, IKeyEvent::FLAG_SOFT_KEYBOARD, (IKeyEvent**)&eDown);

            CKeyEvent::New(0, 0, IKeyEvent::ACTION_UP, keyCode,
                    0, 0, 0, 0, IKeyEvent::FLAG_SOFT_KEYBOARD, (IKeyEvent**)&eUp);

            OnKeyDown(keyCode, eDown, &result);
            OnKeyUp(keyCode, eUp, &result);
        } else if (sKey->IsUniStrKey(&result), result) {
            Boolean kUsed = FALSE;
            String keyLabel;
            sKey->GetKeyLabel(&keyLabel);
            if ((mInputModeSwitcher->IsChineseTextWithSkb(&result), result)
                    && (Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_INPUT == mImeState
                        || Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_COMPOSING == mImeState)) {
                Int32 len = 0;
                if ((mDecInfo->GetLength(&len), len) > 0 && keyLabel.GetLength() == 1
                        && keyLabel.GetChar(0) == '\'') {
                    ProcessSurfaceChange('\'', 0);
                    kUsed = TRUE;
                }
            }
            if (!kUsed) {
                Int32 pos = 0;
                mCandidatesContainer->GetActiveCandiatePos(&pos);
                String str;
                if (Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_INPUT == mImeState) {
                    CommitResultText((mDecInfo->GetCurrentFullSent(pos, &str), str));
                } else if (Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_COMPOSING == mImeState) {
                    CommitResultText((mDecInfo->GetComposingStr(&str), str));
                }
                CommitResultText(keyLabel);
                ResetToIdleState(FALSE);
            }
        }

        // If the current soft keyboard is not sticky, IME needs to go
        // back to the previous soft keyboard automatically.
        Boolean sticky = FALSE;
        if (!(mSkbContainer->IsCurrentSkbSticky(&sticky), sticky)) {
            Int32 icon = 0;
            mInputModeSwitcher->RequestBackToPreviousSkb(&icon);
            UpdateIcon(icon);
            ResetToIdleState(FALSE);
            mSkbContainer->UpdateInputMode();
        }
    }
    return NOERROR;
}

void CPinyinIME::ShowCandidateWindow(
    /* [in] */ Boolean showComposingView)
{
    // if (mEnvironment.needDebug()) {
    //     Log.d(TAG, "Candidates window is shown. Parent = "
    //             + mCandidatesContainer);
    // }

    InputMethodService::SetCandidatesViewShown(TRUE);

    if (NULL != mSkbContainer) mSkbContainer->RequestLayout();

    if (NULL == mCandidatesContainer) {
        ResetToIdleState(FALSE);
        return;
    }

    UpdateComposingText(showComposingView);
    mCandidatesContainer->ShowCandidates(mDecInfo, Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_COMPOSING != mImeState);
    mFloatingWindowTimer->PostShowFloatingWindow();
}

void CPinyinIME::DismissCandidateWindow()
{
    // if (mEnvironment.needDebug()) {
    //     Log.d(TAG, "Candidates window is to be dismissed");
    // }
    if (NULL == mCandidatesContainer) return;
    // try {
    mFloatingWindowTimer->CancelShowing();
    mFloatingWindow->Dismiss();
    // } catch (Exception e) {
    //     Log.e(TAG, "Fail to show the PopupWindow.");
    // }
    InputMethodService::SetCandidatesViewShown(FALSE);

    Boolean shown = FALSE;
    if (NULL != mSkbContainer && (mSkbContainer->IsShown(&shown), shown)) {
        mSkbContainer->ToggleCandidateMode(FALSE);
    }
}

void CPinyinIME::ResetCandidateWindow()
{
    // if (mEnvironment.needDebug()) {
    //     Log.d(TAG, "Candidates window is to be reset");
    // }
    if (NULL == mCandidatesContainer) return;
    // try {
    mFloatingWindowTimer->CancelShowing();
    mFloatingWindow->Dismiss();
    // } catch (Exception e) {
    //     Log.e(TAG, "Fail to show the PopupWindow.");
    // }

    Boolean shown = FALSE;
    if (NULL != mSkbContainer && (mSkbContainer->IsShown(&shown), shown)) {
        mSkbContainer->ToggleCandidateMode(FALSE);
    }

    mDecInfo->ResetCandidates();

    if (NULL != mCandidatesContainer && (mCandidatesContainer->IsShown(&shown), shown)) {
        ShowCandidateWindow(FALSE);
    }
}

void CPinyinIME::UpdateIcon(
    /* [in] */ Int32 iconId)
{
    if (iconId > 0) {
        InputMethodService::ShowStatusIcon(iconId);
    } else {
        InputMethodService::HideStatusIcon();
    }
}

ECode CPinyinIME::OnCreateInputView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    // if (mEnvironment.needDebug()) {
    //     Log.d(TAG, "onCreateInputView.");
    // }
    AutoPtr<ILayoutInflater> inflater;
    InputMethodService::GetLayoutInflater((ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::skb_container, NULL, (IView**)&view);
    mSkbContainer = ISkbContainer::Probe(view);
    mSkbContainer->SetService(this);
    mSkbContainer->SetInputModeSwitcher(mInputModeSwitcher);
    mSkbContainer->SetGestureDetector(mGestureDetectorSkb);
    *view = mSkbContainer;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CPinyinIME::OnStartInput(
    /* [in] */ IEditorInfo* editorInfo,
    /* [in] */ Boolean restarting)
{
    // if (mEnvironment.needDebug()) {
    //     Log.d(TAG, "onStartInput " + " ccontentType: "
    //             + String.valueOf(editorInfo.inputType) + " Restarting:"
    //             + String.valueOf(restarting));
    // }
    Int32 icon = 0;
    mInputModeSwitcher->RequestInputWithHkb(editorInfo, &icon);
    UpdateIcon(icon);
    ResetToIdleState(FALSE);
    return NOERROR;
}

ECode CPinyinIME::OnStartInputView(
    /* [in] */ IEditorInfo* editorInfo,
    /* [in] */ Boolean restarting)
{
    // if (mEnvironment.needDebug()) {
    //     Log.d(TAG, "onStartInputView " + " contentType: "
    //             + String.valueOf(editorInfo.inputType) + " Restarting:"
    //             + String.valueOf(restarting));
    // }
    Int32 icon = 0;
    mInputModeSwitcher->RequestInputWithSkb(editorInfo, &icon);
    UpdateIcon(icon);
    ResetToIdleState(FALSE);
    mSkbContainer->UpdateInputMode();
    return InputMethodService::SetCandidatesViewShown(FALSE);
}

ECode CPinyinIME::OnFinishInputView(
    /* [in] */ Boolean finishingInput)
{
    // if (mEnvironment.needDebug()) {
    //     Log.d(TAG, "onFinishInputView.");
    // }
    ResetToIdleState(FALSE);
    return InputMethodService::OnFinishInputView(finishingInput);
}

ECode CPinyinIME::OnFinishInput()
{
    // if (mEnvironment.needDebug()) {
    //     Log.d(TAG, "onFinishInput.");
    // }
    ResetToIdleState(FALSE);
    return InputMethodService::OnFinishInput();
}

ECode CPinyinIME::OnFinishCandidatesView(
    /* [in] */ Boolean finishingInput)
{
    // if (mEnvironment.needDebug()) {
    //     Log.d(TAG, "onFinishCandidateView.");
    // }
    ResetToIdleState(FALSE);
    return InputMethodService::OnFinishCandidatesView(finishingInput);
}

ECode CPinyinIME::OnDisplayCompletions(
    /* [in] */ ArrayOf<ICompletionInfo *>* completions)
{
    Boolean state = FALSE;
    if (!(InputMethodService::IsFullscreenMode(&state), state)) return NOERROR;
    if (NULL == completions || completions->GetLength() <= 0) return NOERROR;
    if (NULL == mSkbContainer || !(mSkbContainer->IsShown(&state), state)) return NOERROR;

    if (!(mInputModeSwitcher->IsChineseText(&state), state) ||
            Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_IDLE == mImeState ||
            Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_PREDICT == mImeState) {
        mImeState = Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_APP_COMPLETION;
        mDecInfo->PrepareAppCompletions(completions);
        ShowCandidateWindow(FALSE);
    }
    return NOERROR;
}

void CPinyinIME::OnChoiceTouched(
    /* [in] */ Int32 activeCandNo)
{
    if (mImeState == Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_COMPOSING) {
        ChangeToStateInput(TRUE);
    } else if (mImeState == Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_INPUT
            || mImeState == Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_PREDICT) {
        ChooseCandidate(activeCandNo);
    } else if (mImeState == Elastos::Droid::Inputmethods::PinyinIME::ImeState_STATE_APP_COMPLETION) {
        AutoPtr<ArrayOf<ICompletionInfo*> > completions;
        mDecInfo->GetAppCompletions((ArrayOf<ICompletionInfo*>**)&completions);
        if (NULL != completions && activeCandNo >= 0 &&
                activeCandNo < completions->GetLength()) {
            AutoPtr<ICompletionInfo> ci = (*completions)[activeCandNo];
            if (NULL != ci) {
                AutoPtr<IInputConnection> ic;
                InputMethodService::GetCurrentInputConnection((IInputConnection**)&ic);
                Boolean result = FALSE;
                ic->CommitCompletion(ci, &result);
            }
        }
        ResetToIdleState(FALSE);
    }
}

ECode CPinyinIME::RequestHideSelf(
    /* [in] */ Int32 flags)
{
    // if (mEnvironment.needDebug()) {
    //     Log.d(TAG, "DimissSoftInput.");
    // }
    DismissCandidateWindow();
    Boolean shown = FALSE;
    if (NULL != mSkbContainer && (mSkbContainer->IsShown(&shown), shown)) {
        mSkbContainer->DismissPopups();
    }
    return InputMethodService::RequestHideSelf(flags);
}

ECode CPinyinIME::ShowOptionsMenu()
{
    assert(mOptionsDialog == NULL);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
    builder->SetCancelable(TRUE);
    builder->SetIcon(Elastos::Droid::Inputmethods::PinyinIME::R::drawable::app_icon);
    builder->SetNegativeButton(Elastos::Droid::R::string::cancel, NULL);

    String value;
    InputMethodService::GetString(Elastos::Droid::Inputmethods::PinyinIME::R::string::ime_settings_activity_name, &value);
    AutoPtr<ICharSequence> itemSettings;
    CStringWrapper::New(value, (ICharSequence**)&itemSettings);

    AutoPtr<ICharSequence> itemInputMethod;
    InputMethodService::GetString(Elastos::Droid::R::string::inputMethod, &value);
    CStringWrapper::New(value, (ICharSequence**)&itemInputMethod);

    AutoPtr<ArrayOf<ICharSequence*> > items = ArrayOf<ICharSequence*>::Alloc(2);
    items->Set(0, itemSettings);
    items->Set(1, itemInputMethod);

    AutoPtr<BuilderListener> listener = new BuilderListener(this);
    builder->SetItems(items, listener);

    String name;
    InputMethodService::GetString(R::string::ime_name, &name);
    AutoPtr<ICharSequence> csName;
    CStringWrapper::New(name, (ICharSequence**)&csName);
    builder->SetTitle(csName);
    builder->Create((IAlertDialog**)&mOptionsDialog);
    mOptionsDialog->SetOnDismissListener((IDialogInterfaceOnDismissListener*)listener.Get());

    AutoPtr<IWindow> window;
    mOptionsDialog->GetWindow((IWindow**)&window);
    AutoPtr<IWindowManagerLayoutParams> lp;
    window->GetAttributes((IWindowManagerLayoutParams**)&lp);

    AutoPtr<IBinder> token;
    mSkbContainer->GetWindowToken((IBinder**)&token);
    lp->SetToken(token);

    lp->SetType(IWindowManagerLayoutParams::TYPE_APPLICATION_ATTACHED_DIALOG);
    window->SetAttributes(lp);
    window->AddFlags(IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM);
    return mOptionsDialog->Show();
}

ECode CPinyinIME::GetImeState(
    /* [out] */ ImeState* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mImeState;
    return NOERROR;
}

ECode CPinyinIME::GetCandidatesContainer(
    /* [out] */ ICandidatesContainer** container)
{
    VALIDATE_NOT_NULL(container);
    *container = mCandidatesContainer;
    REFCOUNT_ADD(*container);
    return NOERROR;
}

void CPinyinIME::LaunchSettings()
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    assert(0 && "TODO: CSettingsActivity Not Implement...");
    intent->SetClassName(/*PinyinIME.this*/this, String("PinyinIME.CSettingsActivity"));
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    InputMethodService::StartActivity(intent);
}

#endif

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
