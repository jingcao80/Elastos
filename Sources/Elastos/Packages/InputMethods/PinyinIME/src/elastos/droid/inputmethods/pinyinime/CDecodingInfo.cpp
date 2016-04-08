
#include "elastos/droid/inputmethods/pinyinime/CDecodingInfo.h"
#include "elastos/droid/inputmethods/pinyinime/CPinyinSettings.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

const Int32 CDecodingInfo::PY_STRING_MAX = 28;
const Int32 CDecodingInfo::MAX_PAGE_SIZE_DISPLAY = 10;
static String TAG("CDecodingInfo");

CAR_OBJECT_IMPL(CDecodingInfo);
CAR_INTERFACE_IMPL(CDecodingInfo, Object, IDecodingInfo);

CDecodingInfo::CDecodingInfo()
    : mTotalChoicesNum(0)
    , mPosDelSpl(-1)
    , mIsPosInSpl(FALSE)
    , mSurfaceDecodedLen(0)
    , mActiveCmpsLen(0)
    , mActiveCmpsDisplayLen(0)
    , mFixedLen(0)
    , mFinishSelection(FALSE)
    , mCursorPos(0)
{
    mSurface = new StringBuffer();
    mSurfaceDecodedLen = 0;
}

ECode CDecodingInfo::constructor(
    /* [in] */ IPinyinIME* ime)
{
    mPinyinIME = ime;
    return NOERROR;
}

ECode CDecodingInfo::Reset()
{
    Int32 length = 0;
    mSurface->GetLength(&length);
    mSurface->Delete(0, length);
    mSurfaceDecodedLen = 0;
    mCursorPos = 0;
    mFullSent = String("");
    mFixedLen = 0;
    mFinishSelection = FALSE;
    mComposingStr = String("");
    mComposingStrDisplay = String("");
    mActiveCmpsLen = 0;
    mActiveCmpsDisplayLen = 0;

    return ResetCandidates();
}

ECode CDecodingInfo::IsCandidatesListEmpty(
    /* [out] */ Boolean* empty)
{
    VALIDATE_NOT_NULL(empty);
    *empty = mCandidatesList.IsEmpty();
    return NOERROR;
}

ECode CDecodingInfo::IsSplStrFull(
    /* [out] */ Boolean* full)
{
    VALIDATE_NOT_NULL(full);
    Int32 length = 0;
    mSurface->GetLength(&length);

    if (length >= PY_STRING_MAX - 1) {
        *full = TRUE;
        return NOERROR;
    }

    *full = FALSE;
    return NOERROR;
}

ECode CDecodingInfo::AddSplChar(
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
    return NOERROR;
}

ECode CDecodingInfo::PrepareDeleteBeforeCursor()
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

    return NOERROR;
}

ECode CDecodingInfo::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);
    return mSurface->GetLength(length);
}

ECode CDecodingInfo::GetCharAt(
    /* [in] */ Int32 index,
    /* [out] */ Char32* ch)
{
    VALIDATE_NOT_NULL(ch);
    return mSurface->GetCharAt(index, ch);
}

ECode CDecodingInfo::GetOrigianlSplStr(
    /* [out] */ IStringBuffer** str)
{
    VALIDATE_NOT_NULL(str);
    *str = mSurface;
    REFCOUNT_ADD(*str);
    return NOERROR;
}

ECode CDecodingInfo::GetSplStrDecodedLen(
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len);
    *len = mSurfaceDecodedLen;
    return NOERROR;
}

ECode CDecodingInfo::GetSplStart(
    /* [out, callee] */ ArrayOf<Int32>** splStart)
{
    VALIDATE_NOT_NULL(splStart);
    *splStart = mSplStart;
    REFCOUNT_ADD(*splStart);
    return NOERROR;
}

ECode CDecodingInfo::GetComposingStr(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = mComposingStr;
    return NOERROR;
}

ECode CDecodingInfo::GetComposingStrActivePart(
    /* [out] */ String* part)
{
    VALIDATE_NOT_NULL(part);
    assert (mActiveCmpsLen <= (Int32)mComposingStr.GetLength());
    *part = mComposingStr.Substring(0, mActiveCmpsLen);
    return NOERROR;
}

ECode CDecodingInfo::GetActiveCmpsLen(
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len);
    *len = mActiveCmpsLen;
    return NOERROR;
}

ECode CDecodingInfo::GetComposingStrForDisplay(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = mComposingStrDisplay;
    return NOERROR;
}

ECode CDecodingInfo::GetActiveCmpsDisplayLen(
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len);
    *len = mActiveCmpsDisplayLen;
    return NOERROR;
}

ECode CDecodingInfo::GetFullSent(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mFullSent;
    return NOERROR;
}

ECode CDecodingInfo::GetCurrentFullSent(
    /* [in] */ Int32 activeCandPos,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    // try {
    String retStr = mFullSent.Substring(0, mFixedLen);
    retStr += mCandidatesList[activeCandPos];
    *value = retStr;
    // } catch (Exception e) {
    // //TODO
    //     return "";
    // }
    return NOERROR;
}

ECode CDecodingInfo::ResetCandidates()
{
    mCandidatesList.Clear();
    mTotalChoicesNum = 0;

    mPageStart.Clear();
    mPageStart.PushBack(0);
    mCnToPage.Clear();
    mCnToPage.PushBack(0);
    return NOERROR;
}

ECode CDecodingInfo::CandidatesFromApp(
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    ImeState imestate;
    mPinyinIME->GetImeState(&imestate);
    *state = ImeState_STATE_APP_COMPLETION == imestate;
    return NOERROR;
}

ECode CDecodingInfo::CanDoPrediction(
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    //TODO
    // *state = (Int32)mComposingStr.GetLength() == mFixedLen;
    *state = mComposingStr.GetLength() == mFixedLen;
    return NOERROR;
}

ECode CDecodingInfo::SelectionFinished(
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mFinishSelection;
    return NOERROR;
}

ECode CDecodingInfo::ChooseDecodingCandidate(
    /* [in] */ Int32 candId)
{
    ImeState imestate;
    mPinyinIME->GetImeState(&imestate);

    if (imestate != ImeState_STATE_PREDICT) {
        ResetCandidates();
        Int32 totalChoicesNum = 0;
        // try {
        if (candId < 0) {
            String str = mSurface->ToString();
            Int32 len = str.GetByteLength();
            if (len == 0) {
                totalChoicesNum = 0;
            }
            else {
                if (mPyBuf == NULL)
                    mPyBuf = ArrayOf<Byte>::Alloc(PY_STRING_MAX);

                Char32 ch;
                for (Int32 i = 0; i < len; i++) {
                    GetCharAt(i, &ch);
                    (*mPyBuf)[i] = ch;
                }

                (*mPyBuf)[len] = 0;
                if (mPosDelSpl < 0) {
                    mIPinyinDecoderService->ImSearch(mPyBuf, len, &totalChoicesNum);
                } else {
                    Boolean clear_fixed_this_step = TRUE;
                    if (ImeState_STATE_COMPOSING == imestate) {
                        clear_fixed_this_step = FALSE;
                    }
                    mIPinyinDecoderService->ImDelSearch(mPosDelSpl, mIsPosInSpl,
                                    clear_fixed_this_step, &totalChoicesNum);
                    mPosDelSpl = -1;
                }
            }
        } else {
            mIPinyinDecoderService->ImChoose(candId, &totalChoicesNum);
        }
        // } catch (RemoteException e) {
        // }
        UpdateDecInfoForSearch(totalChoicesNum);
    }
    return NOERROR;
}

void CDecodingInfo::UpdateDecInfoForSearch(
    /* [in] */ Int32 totalChoicesNum)
{
    mTotalChoicesNum = totalChoicesNum;
    if (mTotalChoicesNum < 0) {
        mTotalChoicesNum = 0;
        return;
    }

    // try {
    ECode ec = NOERROR;
    String pyStr, tmp;
    Int32 len = 0;

    mSplStart = NULL;
    ec = mIPinyinDecoderService->ImGetSplStart((ArrayOf<Int32>**)&mSplStart);
    FAIL_GOTO(ec, label);
    ec = mIPinyinDecoderService->ImGetPyStr(FALSE, &pyStr);
    FAIL_GOTO(ec, label);
    ec = mIPinyinDecoderService->ImGetPyStrLen(TRUE, &mSurfaceDecodedLen);
    FAIL_GOTO(ec, label);
    //TODO
    // assert (mSurfaceDecodedLen <= (Int32)pyStr.GetLength());
    if (!pyStr.IsNull()) {
        assert(mSurfaceDecodedLen <= pyStr.GetLength());
    }

    ec = mIPinyinDecoderService->ImGetChoice(0, &mFullSent);
    FAIL_GOTO(ec, label);
    ec = mIPinyinDecoderService->ImGetFixedLen(&mFixedLen);
    FAIL_GOTO(ec, label);

    // Update the surface string to the one kept by engine.
    if (pyStr.IsNull()) {
        ec = mSurface->Replace(0, (mSurface->GetLength(&len), len), String(""));
    }
    else {
        ec = mSurface->Replace(0, (mSurface->GetLength(&len), len), pyStr);
    }

    FAIL_GOTO(ec, label);

    mSurface->GetLength(&len);
    if (mCursorPos > len) {
        mCursorPos = len;
    }

    ec = mSurface->Substring((*mSplStart)[mFixedLen + 1], &tmp);
    FAIL_GOTO(ec, label);
    mComposingStr = mFullSent.Substring(0, mFixedLen);
    if (mComposingStr.IsNull()) {
        mComposingStr = tmp;
    }
    else {
        mComposingStr.Append(tmp);
    }

    mActiveCmpsLen = mComposingStr.GetLength();
    if (mSurfaceDecodedLen > 0) {
        mActiveCmpsLen = mActiveCmpsLen - (len - mSurfaceDecodedLen);
    }

    // Prepare the display string.
    if (0 == mSurfaceDecodedLen) {
        mComposingStrDisplay = mComposingStr;
        mActiveCmpsDisplayLen = mComposingStr.GetLength();
    }
    else {
        mComposingStrDisplay = mFullSent.Substring(0, mFixedLen);
        for (Int32 pos = mFixedLen + 1; pos < mSplStart->GetLength() - 1; pos++) {
            ec = mSurface->Substring((*mSplStart)[pos], (*mSplStart)[pos + 1], &tmp);
            FAIL_GOTO(ec, label);
            mComposingStrDisplay += tmp;
            if ((*mSplStart)[pos + 1] < mSurfaceDecodedLen) {
                mComposingStrDisplay += String(" ");
            }
        }

        mActiveCmpsDisplayLen = mComposingStrDisplay.GetLength();
        mSurface->GetLength(&len);
        if (mSurfaceDecodedLen < len) {
            ec = mSurface->Substring(mSurfaceDecodedLen, &tmp);
            FAIL_GOTO(ec, label);
            mComposingStrDisplay += tmp;
        }
    }

    if (mSplStart->GetLength() == mFixedLen + 2) {
        mFinishSelection = TRUE;
    } else {
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
        if (ec == /*RemoteException*/(Int32)E_REMOTE_SERVICE_EXCEPTION) {
            // Log.w(TAG, "PinyinDecoderService died", e);
        }
        else {
            mTotalChoicesNum = 0;
            mComposingStr = String("");
        }
    }

    // Prepare page 0.
    if (!mFinishSelection) {
        Boolean result = FALSE;
        PreparePage(0, &result);
    }
}

ECode CDecodingInfo::ChoosePredictChoice(
    /* [in] */ Int32 choiceId)
{
    ImeState imestate;
    mPinyinIME->GetImeState(&imestate);
    if (ImeState_STATE_PREDICT != imestate || choiceId < 0
            || choiceId >= mTotalChoicesNum) {
        return NOERROR;
    }

    String tmp = mCandidatesList[choiceId];

    ResetCandidates();

    mCandidatesList.PushBack(tmp);
    mTotalChoicesNum = 1;

    Int32 len = 0;
    mSurface->GetLength(&len);
    mSurface->Replace(0, len, String(""));
    mCursorPos = 0;
    mFullSent = tmp;
    mFixedLen = tmp.GetLength();
    mComposingStr = mFullSent;
    mActiveCmpsLen = mFixedLen;

    mFinishSelection = TRUE;
    return NOERROR;
}

ECode CDecodingInfo::GetCandidate(
    /* [in] */ Int32 candId,
    /* [out] */ String* candidate)
{
    VALIDATE_NOT_NULL(candidate);
    // Only loaded items can be gotten, so we use mCandidatesList.size()
    // instead mTotalChoiceNum.
    if (candId < 0 || candId > (Int32)mCandidatesList.GetSize()) {
        *candidate = String(NULL);
        return NOERROR;
    }

    *candidate = mCandidatesList[candId];
    return NOERROR;
}

void CDecodingInfo::GetCandiagtesForCache()
{
    Int32 fetchStart = mCandidatesList.GetSize();
    Int32 fetchSize = mTotalChoicesNum - fetchStart;
    if (fetchSize > MAX_PAGE_SIZE_DISPLAY) {
        fetchSize = MAX_PAGE_SIZE_DISPLAY;
    }
    // try {
    ImeState imestate;
    mPinyinIME->GetImeState(&imestate);
    AutoPtr<ArrayOf<String> > newList;
    if (ImeState_STATE_INPUT == imestate ||
            ImeState_STATE_IDLE == imestate ||
            ImeState_STATE_COMPOSING == imestate){
        mIPinyinDecoderService->ImGetChoiceList(fetchStart, fetchSize, mFixedLen, (ArrayOf<String>**)&newList);
    } else if (ImeState_STATE_PREDICT == imestate) {
        mIPinyinDecoderService->ImGetPredictList(fetchStart, fetchSize, (ArrayOf<String>**)&newList);
    } else if (ImeState_STATE_APP_COMPLETION == imestate) {
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
        return;
    }

    if (newList != NULL && newList->GetLength() > 0) {
        for (Int32 i = 0; i < newList->GetLength(); i++) {
            mCandidatesList.PushBack((*newList)[i]);
        }
    }
    // } catch (RemoteException e) {
    //     Log.w(TAG, "PinyinDecoderService died", e);
    // }
}

ECode CDecodingInfo::PageReady(
    /* [in] */ Int32 pageNo,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    // If the page number is less than 0, return FALSE
    if (pageNo < 0) {
        *state = FALSE;
        return NOERROR;
    }

    // Page pageNo's ending information is not ready.
    if ((Int32)mPageStart.GetSize() <= pageNo + 1) {
        *state = FALSE;
        return NOERROR;
    }

    *state = TRUE;
    return NOERROR;
}

ECode CDecodingInfo::PreparePage(
    /* [in] */ Int32 pageNo,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    // If the page number is less than 0, return FALSE
    if (pageNo < 0) {
        *state = FALSE;
        return NOERROR;
    }

    // Make sure the starting information for page pageNo is ready.
    if ((Int32)mPageStart.GetSize() <= pageNo) {
        *state = FALSE;
        return NOERROR;
    }

    // Page pageNo's ending information is also ready.
    if ((Int32)mPageStart.GetSize() > pageNo + 1) {
        *state = TRUE;
        return NOERROR;
    }

    // If cached items is enough for page pageNo.
    if ((Int32)mCandidatesList.GetSize() - mPageStart[pageNo] >= MAX_PAGE_SIZE_DISPLAY) {
        *state = TRUE;
        return NOERROR;
    }

    // Try to get more items from engine
    GetCandiagtesForCache();

    // Try to find if there are available new items to display.
    // If no new item, return FALSE;
    if (mPageStart[pageNo] >= (Int32)mCandidatesList.GetSize()) {
        *state = FALSE;
        return NOERROR;
    }

    // If there are new items, return TRUE;
    *state = TRUE;
    return NOERROR;
}

ECode CDecodingInfo::PreparePredicts(
    /* [in] */ ICharSequence* history)
{
    if (NULL == history) return NOERROR;

    ResetCandidates();

    AutoPtr<ISettings> settings;
    CPinyinSettings::AcquireSingleton((ISettings**)&settings);
    Boolean result = FALSE;
    if (settings->GetPrediction(&result), result) {
        String preEdit;
        history->ToString(&preEdit);
        // Int32 predictNum = 0;
        if (!preEdit.IsNull()) {
            // try {
            FAIL_RETURN(mIPinyinDecoderService->ImGetPredictsNum(preEdit, &mTotalChoicesNum));
            // } catch (RemoteException e) {
            //     return;
            // }
        }
    }

    Boolean state = FALSE;
    PreparePage(0, &state);
    mFinishSelection = FALSE;
    return NOERROR;
}

ECode CDecodingInfo::PrepareAppCompletions(
    /* [in] */ ArrayOf<ICompletionInfo*>* completions)
{
    assert(completions != NULL);
    ResetCandidates();
    mAppCompletions = completions;
    mTotalChoicesNum = completions->GetLength();

    Boolean state = FALSE;
    PreparePage(0, &state);
    mFinishSelection = FALSE;
    return NOERROR;
}

ECode CDecodingInfo::GetCurrentPageSize(
    /* [in] */ Int32 currentPage,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    if ((Int32)mPageStart.GetSize() <= currentPage + 1) {
        *value = 0;
        return NOERROR;
    }

    *value = mPageStart[currentPage + 1] - mPageStart[currentPage];
    return NOERROR;
}

ECode CDecodingInfo::GetCurrentPageStart(
    /* [in] */ Int32 currentPage,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    if ((Int32)mPageStart.GetSize() < currentPage + 1) {
        *value = mTotalChoicesNum;
        return NOERROR;
    }
    *value = mPageStart[currentPage];
    return NOERROR;
}

ECode CDecodingInfo::PageForwardable(
    /* [in] */ Int32 currentPage,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    if ((Int32)mPageStart.GetSize() <= currentPage + 1) {
        *state = FALSE;
        return NOERROR;
    }
    if (mPageStart[currentPage + 1] >= mTotalChoicesNum) {
        *state = FALSE;
        return NOERROR;
    }

    *state = TRUE;
    return NOERROR;
}

ECode CDecodingInfo::PageBackwardable(
    /* [in] */ Int32 currentPage,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    if (currentPage > 0) {
        *state = TRUE;
        return NOERROR;
    }
    *state = FALSE;
    return NOERROR;
}

ECode CDecodingInfo::CharBeforeCursorIsSeparator(
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    Int32 len = 0;
    mSurface->GetLength(&len);
    if (mCursorPos > len) {
        *state = FALSE;
        return NOERROR;
    }

    Char32 ch = 0;
    if (mCursorPos > 0 && (mSurface->GetCharAt(mCursorPos - 1, &ch), ch) == '\'') {
        *state = TRUE;
        return NOERROR;
    }
    *state = FALSE;
    return NOERROR;
}

ECode CDecodingInfo::GetCursorPos(
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos);
    *pos = mCursorPos;
    return NOERROR;
}

ECode CDecodingInfo::GetCursorPosInCmps(
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos);
    Int32 cursorPos = mCursorPos;
    // Int32 fixedLen = 0;

    for (Int32 hzPos = 0; hzPos < mFixedLen; hzPos++) {
        if (mCursorPos >= (*mSplStart)[hzPos + 2]) {
            cursorPos -= (*mSplStart)[hzPos + 2] - (*mSplStart)[hzPos + 1];
            cursorPos += 1;
        }
    }
    *pos = cursorPos;
    return NOERROR;
}

ECode CDecodingInfo::GetCursorPosInCmpsDisplay(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    Int32 cursorPos = 0;
    GetCursorPosInCmps(&cursorPos);
    // +2 is because: one for mSplStart[0], which is used for other
    // purpose(The length of the segmentation string), and another
    // for the first spelling which does not need a space before it.
    for (Int32 pos = mFixedLen + 2; pos < mSplStart->GetLength() - 1; pos++) {
        if (mCursorPos <= (*mSplStart)[pos]) {
            break;
        } else {
            cursorPos++;
        }
    }
    *value = cursorPos;
    return NOERROR;
}

ECode CDecodingInfo::MoveCursorToEdge(
    /* [in] */ Boolean left)
{
    if (left)
        mCursorPos = 0;
    else
        mSurface->GetLength(&mCursorPos);

    return NOERROR;
}

ECode CDecodingInfo::MoveCursor(
    /* [in] */ Int32 offset)
{
    if (offset > 1 || offset < -1) return NOERROR;

    if (offset != 0) {
        Int32 hzPos = 0;
        for (hzPos = 0; hzPos <= mFixedLen; hzPos++) {
            if (mCursorPos == (*mSplStart)[hzPos + 1]) {
                if (offset < 0) {
                    if (hzPos > 0) {
                        offset = (*mSplStart)[hzPos] - (*mSplStart)[hzPos + 1];
                    }
                } else {
                    if (hzPos < mFixedLen) {
                        offset = (*mSplStart)[hzPos + 2] - (*mSplStart)[hzPos + 1];
                    }
                }
                break;
            }
        }
    }

    Int32 len = 0;
    mCursorPos += offset;
    if (mCursorPos < 0) {
        mCursorPos = 0;
    } else if (mCursorPos > (mSurface->GetLength(&len), len)) {
        mCursorPos = len;
    }

    return NOERROR;
}

ECode CDecodingInfo::GetSplNum(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = (*mSplStart)[0];
    return NOERROR;
}

ECode CDecodingInfo::GetFixedLen(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mFixedLen;
    return NOERROR;
}

ECode CDecodingInfo::SetTotalChoicesNum(
    /* [in] */ Int32 totalChoicesNum)
{
    mTotalChoicesNum = totalChoicesNum;
    return NOERROR;
}

ECode CDecodingInfo::GetTotalChoicesNum(
    /* [out] */ Int32* totalChoicesNum)
{
    VALIDATE_NOT_NULL(totalChoicesNum);
    *totalChoicesNum = mTotalChoicesNum;
    return NOERROR;
}

ECode CDecodingInfo::SetPosDelSpl(
    /* [in] */ Int32 posDelSpl)
{
    mPosDelSpl = posDelSpl;
    return NOERROR;
}

ECode CDecodingInfo::GetPosDelSpl(
    /* [out] */ Int32* posDelSpl)
{
    VALIDATE_NOT_NULL(posDelSpl);
    *posDelSpl = mPosDelSpl;
    return NOERROR;
}

ECode CDecodingInfo::SetPosInSpl(
    /* [in] */ Boolean posInSpl)
{
    mIsPosInSpl = posInSpl;
    return NOERROR;
}

ECode CDecodingInfo::GetPosInSpl(
    /* [out] */ Boolean* posInSpl)
{
    VALIDATE_NOT_NULL(posInSpl);
    *posInSpl = mIsPosInSpl;
    return NOERROR;
}

ECode CDecodingInfo::SetCandidatesList(
    /* [in] */ ArrayOf<String>* candidatesList)
{
    mCandidatesList.Clear();
    if (candidatesList != NULL && candidatesList->GetLength() > 0) {
        for (Int32 i = 0; i < candidatesList->GetLength(); i++) {
            mCandidatesList.PushBack((*candidatesList)[i]);
        }
    }

    return NOERROR;
}

ECode CDecodingInfo::GetCandidatesList(
    /* [out, callee] */ ArrayOf<String>** candidatesList)
{
    VALIDATE_NOT_NULL(candidatesList);
    if (mCandidatesList.IsEmpty()) {
        *candidatesList = NULL;
    }

    AutoPtr<ArrayOf<String> > results = ArrayOf<String>::Alloc(mCandidatesList.GetSize());
    List<String>::Iterator ator = mCandidatesList.Begin();
    for (Int32 i = 0; ator != mCandidatesList.End(); ++ator, i++) {
        results->Set(i, *ator);
    }
    *candidatesList = results;
    REFCOUNT_ADD(*candidatesList);
    return NOERROR;
}

ECode CDecodingInfo::SetPageStartList(
    /* [in] */ ArrayOf<Int32>* pageStartList)
{
    mPageStart.Clear();
    if (pageStartList != NULL && pageStartList->GetLength() > 0) {
        for (Int32 i = 0; i < pageStartList->GetLength(); i++) {
            mPageStart.PushBack((*pageStartList)[i]);
        }
    }
    return NOERROR;
}

ECode CDecodingInfo::GetPageStartList(
    /* [out, callee] */ ArrayOf<Int32>** pageStartList)
{
    VALIDATE_NOT_NULL(pageStartList);
    if (mPageStart.IsEmpty()) {
        *pageStartList = NULL;
    }

    AutoPtr<ArrayOf<Int32> > results = ArrayOf<Int32>::Alloc(mPageStart.GetSize());
    List<Int32>::Iterator ator = mPageStart.Begin();
    for (Int32 i = 0; ator != mPageStart.End(); ++ator, i++) {
        results->Set(i, *ator);
    }
    *pageStartList = results;
    REFCOUNT_ADD(*pageStartList);
    return NOERROR;
}

ECode CDecodingInfo::SetCnToPageList(
    /* [in] */ ArrayOf<Int32>* cnToPageList)
{
    mCnToPage.Clear();
    if (cnToPageList != NULL && cnToPageList->GetLength() > 0) {
        for (Int32 i = 0; i < cnToPageList->GetLength(); i++) {
            mCnToPage.PushBack((*cnToPageList)[i]);
        }
    }
    return NOERROR;
}

ECode CDecodingInfo::GetCnToPageList(
    /* [out, callee] */ ArrayOf<Int32>** cnToPageList)
{
    VALIDATE_NOT_NULL(cnToPageList);
    if (mCnToPage.IsEmpty()) {
        *cnToPageList = NULL;
    }

    AutoPtr<ArrayOf<Int32> > results = ArrayOf<Int32>::Alloc(mCnToPage.GetSize());
    List<Int32>::Iterator ator = mCnToPage.Begin();
    for (Int32 i = 0; ator != mCnToPage.End(); ++ator, i++) {
        results->Set(i, *ator);
    }
    *cnToPageList = results;
    REFCOUNT_ADD(*cnToPageList);
    return NOERROR;
}

ECode CDecodingInfo::SetPinyinDecoderService(
    /* [in] */ IPinyinDecoderService* pinyinService)
{
    mIPinyinDecoderService = pinyinService;
    return NOERROR;
}

ECode CDecodingInfo::GetPinyinDecoderService(
    /* [out] */ IPinyinDecoderService** pinyinService)
{
    VALIDATE_NOT_NULL(pinyinService);
    *pinyinService = mIPinyinDecoderService;
    REFCOUNT_ADD(*pinyinService);
    return NOERROR;
}

ECode CDecodingInfo::GetAppCompletions(
    /* [out, callee] */ ArrayOf<ICompletionInfo*>** completions)
{
    VALIDATE_NOT_NULL(completions);
    *completions = mAppCompletions;
    REFCOUNT_ADD(*completions);
    return NOERROR;
}

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
