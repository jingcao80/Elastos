
#include "elastos/droid/inputmethod/pinyin/CIPinyinDecoderService.h"
#include "elastos/droid/inputmethod/pinyin/CPinyinDecoderService.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

CAR_OBJECT_IMPL(CIPinyinDecoderService);

CAR_INTERFACE_IMPL_2(CIPinyinDecoderService, Service, IIPinyinDecoderService, IBinder);

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
    CPinyinDecoderService::NativeImSetMaxLens(maxSpsLen, maxHzsLen);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImSearch(
    /* [in] */ ArrayOf<Byte>* pyBuf,
    /* [in] */ Int32 pyLen,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::NativeImSearch(pyBuf, pyLen);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImDelSearch(
    /* [in] */ Int32 pos,
    /* [in] */ Boolean is_pos_in_splid,
    /* [in] */ Boolean clear_fixed_this_step,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::NativeImDelSearch(pos, is_pos_in_splid,
            clear_fixed_this_step);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImResetSearch()
{
    CPinyinDecoderService::NativeImResetSearch();
    return NOERROR;
}

ECode CIPinyinDecoderService::ImAddLetter(
    /* [in] */ Byte ch,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::NativeImAddLetter(ch);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetPyStr(
    /* [in] */ Boolean decoded,
    /* [out] */ String* pyStr)
{
    VALIDATE_NOT_NULL(pyStr);
    *pyStr = CPinyinDecoderService::NativeImGetPyStr(decoded);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetPyStrLen(
    /* [in] */ Boolean decoded,
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len);
    *len = CPinyinDecoderService::NativeImGetPyStrLen(decoded);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetSplStart(
    /* [out, callee]*/ ArrayOf<Int32>** values)
{
    VALIDATE_NOT_NULL(values);
    AutoPtr<ArrayOf<Int32> > data = CPinyinDecoderService::NativeImGetSplStart();
    *values = data.Get();
    REFCOUNT_ADD(*values);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetChoice(
    /* [in] */ Int32 choiceId,
    /* [out] */ String* choice)
{
    VALIDATE_NOT_NULL(choice);
    *choice = CPinyinDecoderService::NativeImGetChoice(choiceId);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetChoices(
    /* [in] */ Int32 choicesNum,
    /* [out] */ String* choices)
{
    VALIDATE_NOT_NULL(choices);
    String retStr;
    for (Int32 i = 0; i < choicesNum; i++) {
        if (retStr.IsNull()) {
            retStr = CPinyinDecoderService::NativeImGetChoice(i);
        }
        else {
            retStr += String(" ") + CPinyinDecoderService::NativeImGetChoice(i);
        }
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
        String retStr = CPinyinDecoderService::NativeImGetChoice(i);
        if (0 == i) retStr = retStr.Substring(sentFixedLen);
        (*choiceList)[i - choicesStart] = retStr;
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
    *choose = CPinyinDecoderService::NativeImChoose(choiceId);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImCancelLastChoice(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::NativeImCancelLastChoice();
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetFixedLen(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::NativeImGetFixedLen();
    return NOERROR;
}

ECode CIPinyinDecoderService::ImCancelInput(
    /* [out] */ Boolean* status)
{
    VALIDATE_NOT_NULL(status);
    *status = CPinyinDecoderService::NativeImCancelInput();
    return NOERROR;
}

ECode CIPinyinDecoderService::ImFlushCache()
{
    CPinyinDecoderService::NativeImFlushCache();
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetPredictsNum(
    /* [in] */ const String& fixedStr,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::NativeImGetPredictsNum(fixedStr);
    return NOERROR;
}

ECode CIPinyinDecoderService::ImGetPredictItem(
    /* [in] */ Int32 predictNo,
    /* [out] */ String* item)
{
    VALIDATE_NOT_NULL(item);
    *item = CPinyinDecoderService::NativeImGetPredictItem(predictNo);
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
        (*predictList)[i - predictsStart] = CPinyinDecoderService::NativeImGetPredictItem(i);
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
        *value = CPinyinDecoderService::NativeSyncUserDict(usr_dict, tomerge);
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
        *begin = CPinyinDecoderService::NativeSyncBegin(usr_dict);
        return NOERROR;
    }
    *begin = FALSE;
    return NOERROR;
}

ECode CIPinyinDecoderService::SyncFinish()
{
    CPinyinDecoderService::NativeSyncFinish();
    return NOERROR;
}

ECode CIPinyinDecoderService::SyncPutLemmas(
    /* [in] */ const String& tomerge,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::NativeSyncPutLemmas(tomerge);
    return NOERROR;
}

ECode CIPinyinDecoderService::SyncGetLemmas(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::NativeSyncGetLemmas();
    return NOERROR;
}

ECode CIPinyinDecoderService::SyncGetLastCount(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::NativeSyncGetLastCount();
    return NOERROR;
}

ECode CIPinyinDecoderService::SyncGetTotalCount(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::NativeSyncGetTotalCount();
    return NOERROR;
}

ECode CIPinyinDecoderService::SyncClearLastGot()
{
    CPinyinDecoderService::NativeSyncClearLastGot();
    return NOERROR;
}

ECode CIPinyinDecoderService::ImSyncGetCapacity(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CPinyinDecoderService::NativeSyncGetCapacity();
    return NOERROR;
}

ECode CIPinyinDecoderService::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
