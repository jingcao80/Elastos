
#ifndef  __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CDECODINGINFO_H__
#define  __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CDECODINGINFO_H__

#include "_Elastos_Droid_Inputmethods_PinyinIME_CDecodingInfo.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/StringBuffer.h>

using Elastos::Core::IInteger32;
using Elastos::Core::IStringBuffer;
using Elastos::Core::StringBuffer;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Etl::List;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Droid::Inputmethods::PinyinIME::IPinyinIME;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

class PinyinCandidateView;

CarClass(CDecodingInfo)
    , public Object
    , public IDecodingInfo
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CDecodingInfo();

    CARAPI constructor(
        /* [in] */ IPinyinIME* ime);

    CARAPI SetTotalChoicesNum(
        /* [in] */ Int32 totalChoicesNum);

    CARAPI GetTotalChoicesNum(
        /* [out] */ Int32* totalChoicesNum);

    CARAPI SetPosDelSpl(
        /* [in] */ Int32 posDelSpl);

    CARAPI GetPosDelSpl(
        /* [out] */ Int32* posDelSpl);

    CARAPI SetPosInSpl(
        /* [in] */ Boolean posDelSpl);

    CARAPI GetPosInSpl(
        /* [out] */ Boolean* posDelSpl);

    CARAPI SetCandidatesList(
        /* [in] */ ArrayOf<String>* candidatesList);

    CARAPI GetCandidatesList(
        /* [out, callee] */ ArrayOf<String>** candidatesList);

    CARAPI SetPageStartList(
        /* [in] */ ArrayOf<Int32>* pageStartList);

    CARAPI GetPageStartList(
        /* [out, callee] */ ArrayOf<Int32>** pageStartList);

    CARAPI SetCnToPageList(
        /* [in] */ ArrayOf<Int32>* cnToPageList);

    CARAPI GetCnToPageList(
        /* [out, callee] */ ArrayOf<Int32>** cnToPageList);

    CARAPI Reset();

    CARAPI IsCandidatesListEmpty(
        /* [out] */ Boolean* empty);

    CARAPI IsSplStrFull(
        /* [out] */ Boolean* full);

    CARAPI AddSplChar(
        /* [in] */ Char32 ch,
        /* [in] */ Boolean reset);

    // Prepare to delete before cursor. We may delete a spelling char if
    // the cursor is in the range of unfixed part, delete a whole spelling
    // if the cursor in inside the range of the fixed part.
    // This function only marks the position used to delete.
    CARAPI PrepareDeleteBeforeCursor();

    CARAPI GetLength(
        /* [out] */ Int32* length);

    CARAPI GetCharAt(
        /* [in] */ Int32 index,
        /* [out] */ Char32* ch);

    CARAPI GetOrigianlSplStr(
        /* [out] */ IStringBuffer** str);

    CARAPI GetSplStrDecodedLen(
        /* [out] */ Int32* len);

    CARAPI GetSplStart(
        /* [out, callee] */ ArrayOf<Int32>** splStart);

    CARAPI GetComposingStr(
        /* [out] */ String* str);

    CARAPI GetComposingStrActivePart(
        /* [out] */ String* part);

    CARAPI GetActiveCmpsLen(
        /* [out] */ Int32* len);

    CARAPI GetComposingStrForDisplay(
        /* [out] */ String* str);

    CARAPI GetActiveCmpsDisplayLen(
        /* [out] */ Int32* len);

    CARAPI GetFullSent(
        /* [out] */ String* value);

    CARAPI GetCurrentFullSent(
        /* [in] */ Int32 activeCandPos,
        /* [out] */ String* value);

    CARAPI ResetCandidates();

    CARAPI CandidatesFromApp(
        /* [out] */ Boolean* state);

    CARAPI CanDoPrediction(
        /* [out] */ Boolean* state);

    CARAPI SelectionFinished(
        /* [out] */ Boolean* state);

    CARAPI GetCandidate(
        /* [in] */ Int32 candId,
        /* [out] */ String* candidate);

    CARAPI PageReady(
        /* [in] */ Int32 pageNo,
        /* [out] */ Boolean* state);

    CARAPI PreparePage(
        /* [in] */ Int32 pageNo,
        /* [out] */ Boolean* state);

    CARAPI PreparePredicts(
        /* [in] */ ICharSequence* history);

    CARAPI GetCurrentPageSize(
        /* [in] */ Int32 currentPage,
        /* [out] */ Int32* value);

    CARAPI GetCurrentPageStart(
        /* [in] */ Int32 currentPage,
        /* [out] */ Int32* value);

    CARAPI PageForwardable(
        /* [in] */ Int32 currentPage,
        /* [out] */ Boolean* state);

    CARAPI PageBackwardable(
        /* [in] */ Int32 currentPage,
        /* [out] */ Boolean* state);

    CARAPI CharBeforeCursorIsSeparator(
        /* [out] */ Boolean* state);

    CARAPI GetCursorPos(
        /* [out] */ Int32* pos);

    CARAPI GetCursorPosInCmps(
        /* [out] */ Int32* pos);

    CARAPI GetCursorPosInCmpsDisplay(
        /* [out] */ Int32* pos);

    CARAPI MoveCursorToEdge(
        /* [in] */ Boolean left);

    CARAPI MoveCursor(
        /* [in] */ Int32 offset);

    CARAPI GetSplNum(
        /* [out] */ Int32* value);

    CARAPI GetFixedLen(
        /* [out] */ Int32* value);

    CARAPI SetPinyinDecoderService(
        /* [in] */ IPinyinDecoderService* pinyinService);

    CARAPI GetPinyinDecoderService(
        /* [out] */ IPinyinDecoderService** pinyinService);

    CARAPI PrepareAppCompletions(
        /* [in] */ ArrayOf<ICompletionInfo*>* completions);

    CARAPI GetAppCompletions(
        /* [out, callee] */ ArrayOf<ICompletionInfo*>** completions);

    // After the user chooses a candidate, input method will do a
    // re-decoding and give the new candidate list.
    // If candidate id is less than 0, means user is inputting Pinyin,
    // not selecting any choice.
    CARAPI ChooseDecodingCandidate(
        /* [in] */ Int32 candId);

    CARAPI ChoosePredictChoice(
        /* [in] */ Int32 choiceId);

private:
    CARAPI_(void) UpdateDecInfoForSearch(
        /* [in] */ Int32 totalChoicesNum);

    CARAPI_(void) GetCandiagtesForCache();


public:
    /**
     * The total number of choices for display. The list may only contains
     * the first part. If user tries to navigate to next page which is not
     * in the result list, we need to get these items.
     **/
    Int32 mTotalChoicesNum;

    /**
     * Candidate list. The first one is the full-sentence candidate.
     */
    List<String> mCandidatesList;// = new Vector<String>();

    /**
     * Element i stores the starting position of page i.
     */
    List<Int32> mPageStart;// = new Vector<Integer>();

    /**
     * Element i stores the number of characters to page i.
     */
    List<Int32> mCnToPage;// = new Vector<Integer>();

    /**
     * The position to delete in Pinyin string. If it is less than 0, IME
     * will do an incremental search, otherwise IME will do a deletion
     * operation. if {@link #mIsPosInSpl} is TRUE, IME will delete the whole
     * string for mPosDelSpl-th spelling, otherwise it will only delete
     * mPosDelSpl-th character in the Pinyin string.
     */
    Int32 mPosDelSpl;

    /**
     * If {@link #mPosDelSpl} is big than or equal to 0, this member is used
     * to indicate that whether the postion is counted in spelling id or
     * character.
     */
    Boolean mIsPosInSpl;

private:
    /**
     * Maximum length of the Pinyin string
     */
    static const Int32 PY_STRING_MAX;

    /**
     * Maximum number of candidates to display in one page.
     */
    static const Int32 MAX_PAGE_SIZE_DISPLAY;

    /**
     * Spelling (Pinyin) string.
     */
    AutoPtr<StringBuffer> mSurface;

    /**
     * Byte buffer used as the Pinyin string parameter for native function
     * call.
     */
    AutoPtr<ArrayOf<Byte> > mPyBuf;

    /**
     * The length of surface string successfully decoded by engine.
     */
    Int32 mSurfaceDecodedLen;

    /**
     * Composing string.
     */
    String mComposingStr;

    /**
     * Length of the active composing string.
     */
    Int32 mActiveCmpsLen;

    /**
     * Composing string for display, it is copied from mComposingStr, and
     * add spaces between spellings.
     **/
    String mComposingStrDisplay;

    /**
     * Length of the active composing string for display.
     */
    Int32 mActiveCmpsDisplayLen;

    /**
     * The first full sentence choice.
     */
    String mFullSent;

    /**
     * Number of characters which have been fixed.
     */
    Int32 mFixedLen;

    /**
     * If this flag is TRUE, selection is finished.
     */
    Boolean mFinishSelection;

    /**
     * The starting position for each spelling. The first one is the number
     * of the real starting position elements.
     */
    AutoPtr<ArrayOf<Int32> > mSplStart;

    /**
     * Editing cursor in mSurface.
     */
    Int32 mCursorPos;

    /**
     * Remote Pinyin-to-Hanzi decoding engine service.
     */
    AutoPtr<IPinyinDecoderService> mIPinyinDecoderService;

    /**
     * The complication information suggested by application.
     */
    AutoPtr<ArrayOf<ICompletionInfo*> > mAppCompletions;

    AutoPtr<IPinyinIME> mPinyinIME;

    friend class PinyinCandidateView;
};

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INPUTMETHODS_PINYINIME_CDECODINGINFO_H__
