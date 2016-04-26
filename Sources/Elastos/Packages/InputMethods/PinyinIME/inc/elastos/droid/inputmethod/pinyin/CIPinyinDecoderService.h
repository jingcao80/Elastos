
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_CIPINYINDECODERSERVICE_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_CIPINYINDECODERSERVICE_H__

#include "_Elastos_Droid_InputMethod_Pinyin_CIPinyinDecoderService.h"
#include <elastos/droid/app/Service.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContextWrapper;
using Elastos::Droid::Content::IComponentCallbacks;
using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

class CPinyinDecoderService;

CarClass(CIPinyinDecoderService)
    , public Elastos::Droid::App::Service
    , public IIPinyinDecoderService
    , public IBinder
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IService* host);

    CARAPI GetInt32(
        /* [out] */ Int32* value);

    CARAPI SetMaxLens(
        /* [in] */ Int32 maxSpsLen,
        /* [in] */ Int32 maxHzsLen);

    CARAPI ImSearch(
        /* [in] */ ArrayOf<Byte>* pyBuf,
        /* [in] */ Int32 pyLen,
        /* [out] */ Int32* value);

    CARAPI ImDelSearch(
        /* [in] */ Int32 pos,
        /* [in] */ Boolean is_pos_in_splid,
        /* [in] */ Boolean clear_fixed_this_step,
        /* [out] */ Int32* value);

    CARAPI ImResetSearch();

    CARAPI ImAddLetter(
        /* [in] */ Byte ch,
        /* [out] */ Int32* value);

    CARAPI ImGetPyStr(
        /* [in] */ Boolean decoded,
        /* [out] */ String* pyStr);

    CARAPI ImGetPyStrLen(
        /* [in] */ Boolean decoded,
        /* [out] */ Int32* len);

    CARAPI ImGetSplStart(
        /* [out, callee]*/ ArrayOf<Int32>** values);

    CARAPI ImGetChoice(
        /* [in] */ Int32 choiceId,
        /* [out] */ String* choice);

    CARAPI ImGetChoices(
        /* [in] */ Int32 choicesNum,
        /* [out] */ String* choices);

    CARAPI ImGetChoiceList(
        /* [in] */ Int32 choicesStart,
        /* [in] */ Int32 choicesNum,
        /* [in] */ Int32 sentFixedLen,
        /* [out, callee] */ ArrayOf<String>** strList);

    CARAPI ImChoose(
        /* [in] */ Int32 choiceId,
        /* [out] */ Int32* choose);

    CARAPI ImCancelLastChoice(
        /* [out] */ Int32* value);

    CARAPI ImGetFixedLen(
        /* [out] */ Int32* value);

    CARAPI ImCancelInput(
        /* [out] */ Boolean* status);

    CARAPI ImFlushCache();

    CARAPI ImGetPredictsNum(
        /* [in] */ const String& fixedStr,
        /* [out] */ Int32* value);

    CARAPI ImGetPredictItem(
        /* [in] */ Int32 predictNo,
        /* [out] */ String* item);

    CARAPI ImGetPredictList(
        /* [in] */ Int32 predictsStart,
        /* [in] */ Int32 predictsNum,
        /* [out, callee] */ ArrayOf<String>** strList);

    CARAPI SyncUserDict(
        /* [in] */ const String& tomerge,
        /* [out] */ String* value);

    CARAPI SyncBegin(
        /* [out] */ Boolean* begin);

    CARAPI SyncFinish();

    CARAPI SyncPutLemmas(
        /* [in] */ const String& tomerge,
        /* [out] */ Int32* value);

    CARAPI SyncGetLemmas(
        /* [out] */ String* value);

    CARAPI SyncGetLastCount(
        /* [out] */ Int32* value);

    CARAPI SyncGetTotalCount(
        /* [out] */ Int32* value);

    CARAPI SyncClearLastGot();

    CARAPI ImSyncGetCapacity(
        /* [out] */ Int32* value);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CPinyinDecoderService* mHost;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_CIPINYINDECODERSERVICE_H__
