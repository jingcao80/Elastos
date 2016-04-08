
#ifndef __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINDECODERSERVICE_H__
#define __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINDECODERSERVICE_H__

#include "_CPinyinDecoderService.h"
#include "elastos/droid/app/Service.h"
#include <sys/types.h>

using Elastos::Droid::Inputmethods::PinyinIME::IPinyinDecoderService;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

/**
 * This class is used to separate the input method kernel in an individual
 * service so that both IME and IME-syncer can use it.
 */
class CPinyinDecoderService: public Elastos::Droid::App::Service
{
public:
    // static {
    //     try {
    //         System.loadLibrary("jni_pinyinime");
    //     } catch (UnsatisfiedLinkError ule) {
    //         Log.e("PinyinDecoderService",
    //                 "WARNING: Could not load jni_pinyinime natives");
    //     }
    // }

    CPinyinDecoderService();

    CARAPI OnCreate();

    CARAPI OnDestroy();

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

private:
    // Get file name of the specified dictionary
    CARAPI_(Boolean) GetUsrDictFileName(
        /* [in, out] */ ArrayOf<Byte>* usr_dict);

    CARAPI InitPinyinEngine();

    static String Char16ArrayToString(
        /* [in] */ Char16* buf,
        /* [in] */ Int32 size);

    static AutoPtr<ArrayOf<Char16> > StringToChar16Array(
        /* [in] */ const String& str);

private:
    //method
    static Boolean nativeImOpenDecoder(
        /* [in] */ ArrayOf<Byte>* fn_sys_dict,
        /* [in] */ ArrayOf<Byte>* fn_usr_dict);

    static Boolean nativeImOpenDecoderFd(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 startOffset,
        /* [in] */ Int64 length,
        /* [in, out] */ ArrayOf<Byte>* fn_usr_dict);

    static void nativeImSetMaxLens(
        /* [in] */ Int32 maxSpsLen,
        /* [in] */ Int32 maxHzsLen);

    static Boolean nativeImCloseDecoder();

    static Int32 nativeImSearch(
        /* [in] */ ArrayOf<Byte>* pyBuf,
        /* [in] */ Int32 pyLen);

    static Int32 nativeImDelSearch(
        /* [in] */ Int32 pos,
        /* [in] */ Boolean is_pos_in_splid,
        /* [in] */ Boolean clear_fixed_this_step);

    static void nativeImResetSearch();

    static Int32 nativeImAddLetter(
        /* [in] */ Byte ch);

    static String nativeImGetPyStr(
        /* [in] */ Boolean decoded);

    static Int32 nativeImGetPyStrLen(
        /* [in] */ Boolean decoded);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) nativeImGetSplStart();

    static String nativeImGetChoice(
        /* [in] */ Int32 choiceId);

    static Int32 nativeImChoose(
        /* [in] */ Int32 choiceId);

    static Int32 nativeImCancelLastChoice();

    static Int32 nativeImGetFixedLen();

    static Boolean nativeImCancelInput();

    static Boolean nativeImFlushCache();

    static Int32 nativeImGetPredictsNum(
        /* [in] */ const String& fixedStr);

    static String nativeImGetPredictItem(
        /* [in] */ Int32 predictNo);

    // Sync related
    static String nativeSyncUserDict(
        /* [in] */ ArrayOf<Byte>* user_dict,
        /* [in] */ const String& tomerge);

    static Boolean nativeSyncBegin(
        /* [in] */ ArrayOf<Byte>* user_dict);

    static Boolean nativeSyncFinish();

    static String nativeSyncGetLemmas();

    static Int32 nativeSyncPutLemmas(
        /* [in] */ const String& tomerge);

    static Int32 nativeSyncGetLastCount();

    static Int32 nativeSyncGetTotalCount();

    static Boolean nativeSyncClearLastGot();

    static Int32 nativeSyncGetCapacity();

private:
    AutoPtr<IPinyinDecoderService> mBinder;
    static const Int32 MAX_PATH_FILE_LENGTH;
    static Boolean mInited;

    String mUsr_dict_file;

private:
    friend class CIPinyinDecoderService;
};

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINDECODERSERVICE_H__
