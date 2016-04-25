
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/inputmethod/pinyin/CPinyinDecoderService.h"
#include "elastos/droid/inputmethod/pinyin/CIPinyinDecoderService.h"
#include "elastos/droid/inputmethod/pinyin/native/pinyinime.h"
#include "elastos/droid/inputmethod/pinyin/native/sync.h"
#include "elastos/droid/inputmethod/pinyin/native/userdict.h"
#include "R.h"

using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::IO::ICloseable;
using namespace ime_pinyin;

#define RET_BUF_LEN 256
static char16 retbuf[RET_BUF_LEN];
static char16 (*predict_buf)[kMaxPredictSize + 1] = NULL;
static size_t predict_len;
static Sync sync_worker;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

const Int32 CPinyinDecoderService::MAX_PATH_FILE_LENGTH = 100;
Boolean CPinyinDecoderService::sInited = FALSE;

CAR_OBJECT_IMPL(CPinyinDecoderService);

CPinyinDecoderService::CPinyinDecoderService()
{
    CIPinyinDecoderService::New(this, (IPinyinDecoderService**)&mBinder);
}

ECode CPinyinDecoderService::constructor()
{
    return NOERROR;
}

Boolean CPinyinDecoderService::NativeImOpenDecoder(
    /* [in] */ ArrayOf<Byte>* fn_sys_dict,
    /* [in] */ ArrayOf<Byte>* fn_usr_dict)
{
    Boolean ret = FALSE;

    assert(fn_sys_dict != NULL && fn_usr_dict != NULL);
    if (im_open_decoder((const char*)fn_sys_dict->GetPayload(), (const char*)fn_usr_dict->GetPayload())) {
        ret = TRUE;
    }

    return ret;
}

Boolean CPinyinDecoderService::NativeImOpenDecoderFd(
    /* [in] */ IFileDescriptor* fd_sys_dict,
    /* [in] */ Int64 startoffset,
    /* [in] */ Int64 length,
    /* [in, out] */ ArrayOf<Byte>* fn_usr_dict)
{
    assert(fd_sys_dict != NULL && fn_usr_dict != NULL);
    Int32 fd = 0;
    fd_sys_dict->GetDescriptor(&fd);
    Boolean ret = FALSE;

    Int32 newfd = dup(fd);
    if (im_open_decoder_fd(newfd, startoffset, length, (const char*)fn_usr_dict->GetPayload())) {
        ret = TRUE;
    }

    close(newfd);
    return ret;
}

void CPinyinDecoderService::NativeImSetMaxLens(
    /* [in] */ Int32 max_sps_len,
    /* [in] */ Int32 max_hzs_len)
{
    im_set_max_lens(static_cast<size_t>(max_sps_len),
            static_cast<size_t>(max_hzs_len));
}

Boolean CPinyinDecoderService::NativeImCloseDecoder()
{
    im_close_decoder();
    return TRUE;
}

Int32 CPinyinDecoderService::NativeImSearch(
    /* [in] */ ArrayOf<Byte>* pybuf,
    /* [in] */ Int32 pyLen)
{
    Int32 ret = 0;
    if (NULL != pybuf) {
        ret = im_search((const char*)pybuf->GetPayload(), pyLen);
    }

    return ret;
}

Int32 CPinyinDecoderService::NativeImDelSearch(
    /* [in] */ Int32 pos,
    /* [in] */ Boolean is_pos_in_splid,
    /* [in] */ Boolean clear_fixed_this_step)
{
    return im_delsearch(pos, is_pos_in_splid, clear_fixed_this_step);
}

void CPinyinDecoderService::NativeImResetSearch()
{
    im_reset_search();
}

Int32 CPinyinDecoderService::NativeImAddLetter(
    /* [in] */ Byte ch)
{
    return im_add_letter(ch);
}

String CPinyinDecoderService::NativeImGetPyStr(
    /* [in] */ Boolean decoded)
{
    size_t py_len;
    const char *py = im_get_sps_str(&py_len);  // py_len gets decoded length
    assert(NULL != py);
    if (!decoded) {
        py_len = strlen(py);
    }

    const unsigned short *spl_start;
    size_t len;
    len = im_get_spl_start_pos(spl_start);

    size_t i;
    for (i = 0; i < py_len; i++) {
        retbuf[i] = py[i];
    }

    retbuf[i] = (char16)'\0';

    return Char16ArrayToString(retbuf, i);
}

Int32 CPinyinDecoderService::NativeImGetPyStrLen(
    /* [in] */ Boolean decoded)
{
    size_t py_len;
    const char *py = im_get_sps_str(&py_len);  // py_len gets decoded length
    assert(NULL != py);
    if (!decoded) {
        py_len = strlen(py);
    }
    return py_len;
}

AutoPtr<ArrayOf<Int32> > CPinyinDecoderService::NativeImGetSplStart()
{
    const unsigned short *spl_start;
    size_t len;

    // There will be len + 1 elements in the buffer when len > 0.
    len = im_get_spl_start_pos(spl_start);

    AutoPtr<ArrayOf<Int32> > arr = ArrayOf<Int32>::Alloc(len + 2);
    (*arr)[0] = len; // element 0 is used to store the length of buffer.
    for (size_t i = 0; i <= len; i++) {
        (*arr)[i + 1] = spl_start[i];
    }

    return arr;
}

String CPinyinDecoderService::NativeImGetChoice(
    /* [in] */ Int32 candidateId)
{
    if (im_get_candidate(candidateId, retbuf, RET_BUF_LEN)) {
        return Char16ArrayToString(retbuf, utf16_strlen(retbuf));
    }
    else {
        return Char16ArrayToString(retbuf, 0);
    }
}

Int32 CPinyinDecoderService::NativeImChoose(
    /* [in] */ Int32 choice_id)
{
    return im_choose(choice_id);
}

Int32 CPinyinDecoderService::NativeImCancelLastChoice()
{
    return im_cancel_last_choice();
}

Int32 CPinyinDecoderService::NativeImGetFixedLen()
{
    return im_get_fixed_len();
}

Boolean CPinyinDecoderService::NativeImCancelInput()
{
    if (im_cancel_input()) {
        return TRUE;
    }

    return FALSE;
}

Boolean CPinyinDecoderService::NativeImFlushCache()
{
    im_flush_cache();
    return TRUE;
}

Int32 CPinyinDecoderService::NativeImGetPredictsNum(
    /* [in] */ const String& fixed_str)
{
    AutoPtr<ArrayOf<char16> > fixed_ptr = fixed_str.GetChar16s();
    size_t fixed_len = fixed_ptr != NULL ? fixed_ptr->GetLength() : 0;

    char16 fixed_buf[kMaxPredictSize + 1];

    if (fixed_len > kMaxPredictSize) {
        char16* ptr = fixed_ptr->GetPayload();
        ptr += fixed_len - kMaxPredictSize;
        fixed_len = kMaxPredictSize;
    }

    if (fixed_len > 0) {
        utf16_strncpy(fixed_buf, fixed_ptr->GetPayload(), fixed_len);
    }
    fixed_buf[fixed_len] = (char16)'\0';

    predict_len = im_get_predicts(fixed_buf, predict_buf);

    return predict_len;
}

String CPinyinDecoderService::NativeImGetPredictItem(
    /* [in] */ Int32 predict_no)
{
    String retstr;
    if (predict_no < 0 || (size_t)predict_no >= predict_len) {
        retstr = Char16ArrayToString((Char16*)predict_buf[0], 0);
    }
    else {
        retstr = Char16ArrayToString((Char16*)predict_buf[predict_no], utf16_strlen(predict_buf[predict_no]));
    }
    return retstr;
}

// Sync related
String CPinyinDecoderService::NativeSyncUserDict(
    /* [in] */ ArrayOf<Byte>* user_dict,
    /* [in] */ const String& tomerge)
{
    //Not have jni codes....
    assert(0);
    return String(NULL);
}

Boolean CPinyinDecoderService::NativeSyncBegin(
    /* [in] */ ArrayOf<Byte>* dict_file)
{
    assert(dict_file != NULL);
    Boolean ret = FALSE;
    if (true == sync_worker.begin((const char *)dict_file->GetPayload())) {
        ret = TRUE;
    }

    return ret;
}

Boolean CPinyinDecoderService::NativeSyncFinish()
{
    sync_worker.finish();
    return TRUE;
}

String CPinyinDecoderService::NativeSyncGetLemmas()
{
    Int32 len = sync_worker.get_lemmas(retbuf, RET_BUF_LEN);
    if (len == 0) {
        return String(NULL);
    }

    return Char16ArrayToString(retbuf, len);
}

Int32 CPinyinDecoderService::NativeSyncPutLemmas(
    /* [in] */ const String& tomerge)
{
    AutoPtr<ArrayOf<char16> > ptr = tomerge.GetChar16s();
    Int32 len = (size_t)ptr->GetLength();

    return sync_worker.put_lemmas(ptr->GetPayload(), len);
}

Int32 CPinyinDecoderService::NativeSyncGetLastCount()
{
    return sync_worker.get_last_got_count();
}

Int32 CPinyinDecoderService::NativeSyncGetTotalCount()
{
    return sync_worker.get_total_count();
}

Boolean CPinyinDecoderService::NativeSyncClearLastGot()
{
    sync_worker.clear_last_got();
    return TRUE;
}

Int32 CPinyinDecoderService::NativeSyncGetCapacity()
{
    return sync_worker.get_capacity();
}

Boolean CPinyinDecoderService::GetUsrDictFileName(
    /* [in, out] */ ArrayOf<Byte>* usr_dict)
{
    if (usr_dict == NULL || mUsr_dict_file.IsNullOrEmpty()) {
        return FALSE;
    }

    const char* p = mUsr_dict_file.string();
    memcpy(usr_dict->GetPayload(), p, mUsr_dict_file.GetByteLength());
    (*usr_dict)[mUsr_dict_file.GetByteLength()] = 0;

    return TRUE;
}

void CPinyinDecoderService::InitPinyinEngine()
{
    AutoPtr<ArrayOf<Byte> > usr_dict;
    usr_dict = ArrayOf<Byte>::Alloc(MAX_PATH_FILE_LENGTH);

    // Here is how we open a built-in dictionary for access through
    // a file descriptor...
    AutoPtr<IAssetFileDescriptor> afd;
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->OpenRawResourceFd(R::raw::dict_pinyin, (IAssetFileDescriptor**)&afd);
    assert(afd != NULL);
    // if (Environment.getInstance().needDebug()) {
    //     Log
    //             .i("foo", "Dict: start=" + afd.getStartOffset()
    //                     + ", length=" + afd.getLength() + ", fd="
    //                     + afd.getParcelFileDescriptor());
    // }

    if (GetUsrDictFileName(usr_dict)) {
        AutoPtr<IFileDescriptor> fd;
        afd->GetFileDescriptor((IFileDescriptor**)&fd);
        assert(fd != NULL);
        Int64 offset = 0, length = 0;
        afd->GetStartOffset(&offset);
        afd->GetLength(&length);
        sInited = NativeImOpenDecoderFd(fd, offset, length, usr_dict);
    }

    afd->Close();
}

ECode CPinyinDecoderService::OnCreate()
{
    FAIL_RETURN(Service::OnCreate());
    AutoPtr<IFile> file;
    GetFileStreamPath(String("usr_dict.dat"), (IFile**)&file);
    file->GetPath(&mUsr_dict_file);
    // This is a hack to make sure our "files" directory has been
    // created.
    AutoPtr<IFileOutputStream> stream;
    OpenFileOutput(String("dummy"), 0, (IFileOutputStream**)&stream);
    ICloseable::Probe(stream)->Close();

    InitPinyinEngine();
    return NOERROR;
}

ECode CPinyinDecoderService::OnDestroy()
{
    NativeImCloseDecoder();
    sInited = FALSE;
    return Service::OnDestroy();
}

ECode CPinyinDecoderService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    assert(mBinder != NULL);
    VALIDATE_NOT_NULL(binder);
    *binder = IBinder::Probe(mBinder);
    assert(*binder != NULL);
    REFCOUNT_ADD(*binder);
    return NOERROR;
}

String CPinyinDecoderService::Char16ArrayToString(
    /* [in] */ Char16* buf,
    /* [in] */ Int32 size)
{
    assert(buf != NULL && size >= 0);
    if (size == 0) {
        return String(NULL);
    }

    AutoPtr<ArrayOf<Char32> > chars = ArrayOf<Char32>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        (*chars)[i] = buf[i];
    }
    return String(*chars, 0, size);
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
