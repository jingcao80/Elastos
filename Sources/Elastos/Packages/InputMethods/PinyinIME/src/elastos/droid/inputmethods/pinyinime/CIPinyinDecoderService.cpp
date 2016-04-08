
#include "CIPinyinDecoderService.h"

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

ECode CIPinyinDecoderService::constructor(
    /* [in] */ IService* host)
{
    mHost = (CPinyinDecoderService*)host;
    return NOERROR;
}

ECode CIPinyinDecoderService::GetInt32(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 12345;
    return NOERROR;
}

ECode CIPinyinDecoderService::SetMaxLens(
    /* [in] */ Int32 maxSpsLen,
    /* [in] */ Int32 maxHzsLen)
{
    CPinyinDecoderService::nativeImSetMaxLens(maxSpsLen, maxHzsLen);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImSearch(
    /* [in] */ ArrayOf<Byte>* pyBuf,
    /* [in] */ Int32 pyLen,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::nativeImSearch(pyBuf, pyLen);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImDelSearch(
    /* [in] */ Int32 pos,
    /* [in] */ Boolean is_pos_in_splid,
    /* [in] */ Boolean clear_fixed_this_step,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::nativeImDelSearch(pos, is_pos_in_splid,
            clear_fixed_this_step);

    return NOERROR;
}

ECode CIPinyinDecoderService::ImResetSearch()
{
    CPinyinDecoderService::nativeImResetSearch();
    return NOERROR;
}

ECode CIPinyinDecoderService::ImAddLetter(
    /* [in] */ Byte ch,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::nativeImAddLetter(ch);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetPyStr(
    /* [in] */ Boolean decoded,
    /* [out] */ String* pyStr)
{
    VALIDATE_NOT_NULL(pyStr);
    *pyStr = CPinyinDecoderService::nativeImGetPyStr(decoded);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetPyStrLen(
    /* [in] */ Boolean decoded,
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len);
    *len = CPinyinDecoderService::nativeImGetPyStrLen(decoded);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetSplStart(
    /* [out, callee]*/ ArrayOf<Int32>** values)
{
    VALIDATE_NOT_NULL(values);
    AutoPtr<ArrayOf<Int32> > data = CPinyinDecoderService::nativeImGetSplStart();
    *values = data.Get();
    REFCOUNT_ADD(*values);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetChoice(
    /* [in] */ Int32 choiceId,
    /* [out] */ String* choice)
{
    VALIDATE_NOT_NULL(choice);
    *choice = CPinyinDecoderService::nativeImGetChoice(choiceId);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetChoices(
    /* [in] */ Int32 choicesNum,
    /* [out] */ String* choices)
{
    VALIDATE_NOT_NULL(choices);
    String retStr;
    for (Int32 i = 0; i < choicesNum; i++) {
        if (retStr.IsNull())
            retStr = CPinyinDecoderService::nativeImGetChoice(i);
        else
            retStr += String(" ") + CPinyinDecoderService::nativeImGetChoice(i);
    }
    *choices = retStr;
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetChoiceList(
    /* [in] */ Int32 choicesStart,
    /* [in] */ Int32 choicesNum,
    /* [in] */ Int32 sentFixedLen,
    /* [out, callee] */ ArrayOf<String>** strList)
{
    VALIDATE_NOT_NULL(strList);
    AutoPtr<ArrayOf<String> > choiceList = ArrayOf<String>::Alloc(choicesNum);
    for (Int32 i = choicesStart; i < choicesStart + choicesNum; i++) {
        String retStr = CPinyinDecoderService::nativeImGetChoice(i);
        if (0 == i) retStr = retStr.Substring(sentFixedLen);
        choiceList->Set(i - choicesStart, retStr);
    }
    *strList = choiceList;
    REFCOUNT_ADD(*strList);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImChoose(
    /* [in] */ Int32 choiceId,
    /* [out] */ Int32* choose)
{
    VALIDATE_NOT_NULL(choose);
    *choose = CPinyinDecoderService::nativeImChoose(choiceId);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImCancelLastChoice(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::nativeImCancelLastChoice();
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetFixedLen(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::nativeImGetFixedLen();
    return NOERROR;
}

ECode CIPinyinDecoderService::ImCancelInput(
    /* [out] */ Boolean* status)
{
    VALIDATE_NOT_NULL(status);
    *status = CPinyinDecoderService::nativeImCancelInput();
    return NOERROR;
}

ECode CIPinyinDecoderService::ImFlushCache()
{
    CPinyinDecoderService::nativeImFlushCache();
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetPredictsNum(
    /* [in] */ const String& fixedStr,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::nativeImGetPredictsNum(fixedStr);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetPredictItem(
    /* [in] */ Int32 predictNo,
    /* [out] */ String* item)
{
    VALIDATE_NOT_NULL(item);
    *item = CPinyinDecoderService::nativeImGetPredictItem(predictNo);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetPredictList(
    /* [in] */ Int32 predictsStart,
    /* [in] */ Int32 predictsNum,
    /* [out, callee] */ ArrayOf<String>** strList)
{
    VALIDATE_NOT_NULL(strList);
    AutoPtr<ArrayOf<String> > predictList = ArrayOf<String>::Alloc(predictsNum);
    for (Int32 i = predictsStart; i < predictsStart + predictsNum; i++) {
        predictList->Set(i - predictsStart, CPinyinDecoderService::nativeImGetPredictItem(i));
    }
    *strList = predictList;
    REFCOUNT_ADD(*strList);
    return NOERROR;
}

ECode CIPinyinDecoderService::SyncUserDict(
    /* [in] */ const String& tomerge,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<ArrayOf<Byte> > usr_dict = ArrayOf<Byte>::Alloc(CPinyinDecoderService::MAX_PATH_FILE_LENGTH);

    if (mHost->GetUsrDictFileName(usr_dict)) {
        *value = CPinyinDecoderService::nativeSyncUserDict(usr_dict, tomerge);
        return NOERROR;
    }
    *value = String(NULL);
    return NOERROR;
}

ECode CIPinyinDecoderService::SyncBegin(
    /* [out] */ Boolean* begin)
{
    VALIDATE_NOT_NULL(begin);
    AutoPtr<ArrayOf<Byte> > usr_dict = ArrayOf<Byte>::Alloc(CPinyinDecoderService::MAX_PATH_FILE_LENGTH);

    if (mHost->GetUsrDictFileName(usr_dict)) {
        *begin = CPinyinDecoderService::nativeSyncBegin(usr_dict);
        return NOERROR;
    }

    *begin = FALSE;
    return NOERROR;
}

ECode CIPinyinDecoderService::SyncFinish()
{
    CPinyinDecoderService::nativeSyncFinish();
    return NOERROR;
}

ECode CIPinyinDecoderService::SyncPutLemmas(
    /* [in] */ const String& tomerge,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::nativeSyncPutLemmas(tomerge);
    return NOERROR;
}

ECode CIPinyinDecoderService::SyncGetLemmas(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::nativeSyncGetLemmas();
    return NOERROR;
}

ECode CIPinyinDecoderService::SyncGetLastCount(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::nativeSyncGetLastCount();
    return NOERROR;
}

ECode CIPinyinDecoderService::SyncGetTotalCount(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::nativeSyncGetTotalCount();
    return NOERROR;
}

ECode CIPinyinDecoderService::SyncClearLastGot()
{
    CPinyinDecoderService::nativeSyncClearLastGot();
    return NOERROR;
}

ECode CIPinyinDecoderService::ImSyncGetCapacity(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::nativeSyncGetCapacity();
    return NOERROR;
}

ECode CIPinyinDecoderService::ToString(
    /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
