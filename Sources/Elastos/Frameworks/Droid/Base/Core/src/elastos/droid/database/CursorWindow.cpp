
#include "Elastos.Droid.Content.h"
#include "elastos/droid/database/CursorWindow.h"
#include "elastos/droid/database/CCursorWindow.h"
#include "elastos/droid/database/NativeCursorWindow.h"
#include "elastos/droid/database/sqlite/SQLiteCommon.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/R.h"
#include <elastos/core/Character.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::R;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::Character;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {

const String CursorWindow::STATS_TAG("CursorWindow");

Int32 CursorWindow::sCursorWindowSize = -1;
HashMap<Int64, Int32> CursorWindow::sWindowToPidMap;
Object CursorWindow::sWindowToPidMapLock;

CAR_INTERFACE_IMPL_2(CursorWindow, SQLiteClosable, ICursorWindow, IParcelable)

Int32 CursorWindow::GetCursorWindowSize()
{
    if (sCursorWindowSize < 0) {
        AutoPtr<IResources> res = CResources::GetSystem();
        Int32 i;
        res->GetInteger(R::integer::config_cursorWindowSize, &i);
        sCursorWindowSize = i * 1024;
    }
    return sCursorWindowSize;
}

CursorWindow::CursorWindow()
    : mWindowPtr(0)
    , mStartPos(0)
{
}

CursorWindow::~CursorWindow()
{
    //try {
    if (mCloseGuard != NULL) {
        mCloseGuard->WarnIfOpen();
    }
    Dispose();
    //} finally {
    // SQLiteClosable::Finalize();
    //}
}

static ECode ThrowExceptionWithRowCol(Int32 row, Int32 column) {
    Slogger::E(String("CursorWindow"), "Couldn't read row %d, col %d from CursorWindow.  Make sure the Cursor is initialized correctly before accessing data from it."
            , row, column);
    return E_ILLEGAL_STATE_EXCEPTION;
}

static ECode ThrowUnknownTypeException(Int32 type) {
    Slogger::E(String("CursorWindow"), "UNKNOWN type %d", type);
    return E_ILLEGAL_STATE_EXCEPTION;
}

Int64 CursorWindow::NativeCreate(
    /* [in] */ const String& name,
    /* [in] */ Int32 cursorWindowSize)
{
    NativeCursorWindow* window;
    android::status_t status = NativeCursorWindow::create(name, cursorWindowSize, &window);
    if (status || !window) {
        ALOGE("Could not allocate CursorWindow '%s' of size %d due to error %d.",
                name.string(), cursorWindowSize, status);
        return 0;
    }

    LOG_WINDOW("nativeInitializeEmpty: window = %p", window);
    return reinterpret_cast<Int64>(window);
}

Int64 CursorWindow::NativeCreateFromParcel(
    /* [in] */ IParcel* parcel)
{
    NativeCursorWindow* window;
    android::status_t status = NativeCursorWindow::createFromParcel(parcel, &window);
    if (status || !window) {
        ALOGE("Could not create CursorWindow from Parcel due to error %d.", status);
        return 0;
    }

    LOG_WINDOW("nativeInitializeFromBinder: numRows = %d, numColumns = %d, window = %p",
           window->getNumRows(), window->getNumColumns(), window);
    return reinterpret_cast<Int64>(window);
}

void CursorWindow::NativeDispose(
    /* [in] */ Int64 windowPtr)
{
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    if (window) {
        LOG_WINDOW("Closing window %p", window);
        delete window;
    }
}

String CursorWindow::NativeGetName(
    /* [in] */ Int64 windowPtr)
{
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    return window->name();
}

ECode CursorWindow::NativeWriteToParcel(
    /* [in] */ Int64 windowPtr,
    /* [in] */ IParcel* parcel)
{
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);

    android::status_t status = window->writeToParcel(parcel);
    if (status) {
        Slogger::E(STATS_TAG, "Could not write CursorWindow to Parcel due to error %d.", status);
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

void CursorWindow::NativeClear(
    /* [in] */ Int64 windowPtr)
{
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    LOG_WINDOW("Clearing window %p", window);
    android::status_t status = window->clear();
    if (status) {
        LOG_WINDOW("Could not clear window. error=%d", status);
    }
}

Int32 CursorWindow::NativeGetNumRows(
    /* [in] */ Int64 windowPtr)
{
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    return window->getNumRows();
}

Boolean CursorWindow::NativeSetNumColumns(
    /* [in] */ Int64 windowPtr,
    /* [in] */ Int32 columnNum)
{
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    android::status_t status = window->setNumColumns(columnNum);
    return status == android::OK;
}

Boolean CursorWindow::NativeAllocRow(
    /* [in] */ Int64 windowPtr)
{
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    android::status_t status = window->allocRow();
    return status == android::OK;
}

void CursorWindow::NativeFreeLastRow(
    /* [in] */ Int64 windowPtr)
{
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    window->freeLastRow();
}

Int32 CursorWindow::NativeGetType(
    /* [in] */ Int64 windowPtr,
    /* [in] */ Int32 row,
    /* [in] */ Int32 column)
{
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    LOG_WINDOW("returning column type affinity for (%d, %d) from %p", row, column, window);

    NativeCursorWindow::FieldSlot* fieldSlot = window->getFieldSlot(row, column);
    if (!fieldSlot) {
        // FIXME: This is really broken but we have CTS tests that depend
        // on this legacy behavior.
        // ThrowExceptionWithRowCol(env, row, column);
        return NativeCursorWindow::FIELD_TYPE_NULL;
    }
    return window->getFieldSlotType(fieldSlot);
}

ECode CursorWindow::NativeGetBlob(
    /* [in] */ Int64 windowPtr,
    /* [in] */ Int32 row,
    /* [in] */ Int32 column,
    /* [out] */ ArrayOf<Byte>** blob)
{
    VALIDATE_NOT_NULL(blob)

    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    LOG_WINDOW("Getting blob for (%d, %d) from %p", row, column, window);

    NativeCursorWindow::FieldSlot* fieldSlot = window->getFieldSlot(row, column);
    if (!fieldSlot) {
        *blob = NULL;
        return ThrowExceptionWithRowCol(row, column);;
    }

    int32_t type = window->getFieldSlotType(fieldSlot);
    if (type == NativeCursorWindow::FIELD_TYPE_BLOB || type == NativeCursorWindow::FIELD_TYPE_STRING) {
        size_t size;
        const void* value = window->getFieldSlotValueBlob(fieldSlot, &size);
        AutoPtr< ArrayOf<Byte> > byteArray = ArrayOf<Byte>::Alloc(size);
        if (!byteArray) {
            *blob = NULL;
            return Sqlite::throw_sqlite3_exception("Native could not create new byte[]");
        }
        //env->SetByteArrayRegion(byteArray, 0, size, static_cast<const jbyte*>(value));
        for(UInt32 i = 0; i < size; i++) {
            (*byteArray)[i] = (static_cast<const byte*>(value))[i];
        }
        *blob = byteArray;
        REFCOUNT_ADD(*blob)
        return NOERROR;
    } else if (type == NativeCursorWindow::FIELD_TYPE_INTEGER) {
        return Sqlite::throw_sqlite3_exception("INTEGER data in nativeGetBlob ");
    } else if (type == NativeCursorWindow::FIELD_TYPE_FLOAT) {
        return Sqlite::throw_sqlite3_exception("FLOAT data in nativeGetBlob ");
    } else if (type == NativeCursorWindow::FIELD_TYPE_NULL) {
        // do nothing
    } else {
        //ThrowUnknownTypeException(env, type);
        return E_UNKNOW_TYPE_EXCEPTION;
    }
    *blob = NULL;
    return NOERROR;
}

ECode CursorWindow::NativeGetString(
    /* [in] */ Int64 windowPtr,
    /* [in] */ Int32 row,
    /* [in] */ Int32 column,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    // LOG_WINDOW("Getting string for (%d, %d) from %p", row, column, window);

    NativeCursorWindow::FieldSlot* fieldSlot = window->getFieldSlot(row, column);
    if (!fieldSlot) {
        *str = String(NULL);
        return ThrowExceptionWithRowCol(row, column);
    }

    int32_t type = window->getFieldSlotType(fieldSlot);
    if (type == NativeCursorWindow::FIELD_TYPE_STRING) {
        size_t sizeIncludingNull;
        const char* value = window->getFieldSlotValueString(fieldSlot, &sizeIncludingNull);
        if (sizeIncludingNull <= 1) {
            *str = String("");
            return NOERROR;
        }
        // Convert to UTF-16 here instead of calling NewStringUTF.  NewStringUTF
        // doesn't like UTF-8 strings with high codepoints.  It actually expects
        // Modified UTF-8 with encoded surrogate pairs.
        *str = String(value, sizeIncludingNull - 1);
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_INTEGER) {
        int64_t value = window->getFieldSlotValueLong(fieldSlot);
        char buf[32];
        snprintf(buf, sizeof(buf), "%lld", value);
        *str = String(buf);
        return NOERROR;
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_FLOAT) {
        double value = window->getFieldSlotValueDouble(fieldSlot);
        char buf[32];
        snprintf(buf, sizeof(buf), "%g", value);
        *str = String(buf);
        return NOERROR;
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_NULL) {
        *str = String(NULL);
        return NOERROR;
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_BLOB) {
        *str = String(NULL);
        Slogger::E(STATS_TAG, "Unable to convert BLOB to string");
        return E_SQLITE3_EXCEPTION;
    }
    else {
        *str = String(NULL);
        return ThrowUnknownTypeException(type);
    }
    return NOERROR;
}

static AutoPtr< ArrayOf<Char32> > AllocCharArrayBuffer(ICharArrayBuffer* bufferObj, Int32 size)
{
    AutoPtr< ArrayOf<Char32> > dataObj;
    bufferObj->GetData((ArrayOf<Char32>**)&dataObj);
    if (dataObj && size) {
        Int32 capacity = dataObj->GetLength();
        if (capacity < size) {
            dataObj = NULL;
        }
    }
    if (!dataObj) {
        Int32 capacity = size;
        if (capacity < 64) {
            capacity = 64;
        }
        dataObj = ArrayOf<Char32>::Alloc(capacity); // might throw OOM
        if (dataObj) {
            bufferObj->SetData(dataObj);
        }
    }
    return dataObj;
}

static void FillCharArrayBufferUTF(ICharArrayBuffer* bufferObj, const char* str, Int32 len)
{
    Int32 size;
    Character::GetCharCount(String(str), 0, strlen(str), &size);
    if (size < 0) {
        size = 0; // invalid UTF8 string
    }
    AutoPtr< ArrayOf<Char32> > dataObj = AllocCharArrayBuffer(bufferObj, size);
    if (dataObj) {
        if (size) {
            Character::ToChar32s(String(str), 0, strlen(str), dataObj, 0);
        }
        bufferObj->SetSizeCopied(size);
    }
}

static void ClearCharArrayBuffer(ICharArrayBuffer* bufferObj)
{
    AutoPtr< ArrayOf<Char32> > dataObj = AllocCharArrayBuffer(bufferObj, 0);
    if (dataObj != NULL) {
        bufferObj->SetSizeCopied(0);
    }
}

ECode CursorWindow::NativeCopyStringToBuffer(
    /* [in] */ Int64 windowPtr,
    /* [in] */ Int32 row,
    /* [in] */ Int32 column,
    /* [in] */ ICharArrayBuffer* buffer)
{
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    LOG_WINDOW("Copying string for (%d, %d) from %p", row, column, window);

    NativeCursorWindow::FieldSlot* fieldSlot = window->getFieldSlot(row, column);
    if (!fieldSlot) {
        return ThrowExceptionWithRowCol(row, column);
    }

    int32_t type = window->getFieldSlotType(fieldSlot);
    if (type == NativeCursorWindow::FIELD_TYPE_STRING) {
        size_t sizeIncludingNull;
        const char* value = window->getFieldSlotValueString(fieldSlot, &sizeIncludingNull);
        if (sizeIncludingNull > 1) {
            FillCharArrayBufferUTF(buffer, value, sizeIncludingNull - 1);
        }
        else {
            ClearCharArrayBuffer(buffer);
        }
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_INTEGER) {
        int64_t value = window->getFieldSlotValueLong(fieldSlot);
        char buf[32];
        snprintf(buf, sizeof(buf), "%lld", value);
        FillCharArrayBufferUTF(buffer, buf, strlen(buf));
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_FLOAT) {
        double value = window->getFieldSlotValueDouble(fieldSlot);
        char buf[32];
        snprintf(buf, sizeof(buf), "%g", value);
        FillCharArrayBufferUTF(buffer, buf, strlen(buf));
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_NULL) {
        ClearCharArrayBuffer(buffer);
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_BLOB) {
        Slogger::E(STATS_TAG, "Unable to convert BLOB to string");
        return E_SQLITE3_EXCEPTION;
    }

    return ThrowUnknownTypeException(type);
}

ECode CursorWindow::NativeGetInt64(
    /* [in] */ Int64 windowPtr,
    /* [in] */ Int32 row,
    /* [in] */ Int32 column,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    LOG_WINDOW("Getting long for (%d, %d) from %p", row, column, window);

    NativeCursorWindow::FieldSlot* fieldSlot = window->getFieldSlot(row, column);
    if (!fieldSlot) {
        *value = 0;
        return ThrowExceptionWithRowCol(row, column);
    }

    int32_t type = window->getFieldSlotType(fieldSlot);
    if (type == NativeCursorWindow::FIELD_TYPE_INTEGER) {
        *value = window->getFieldSlotValueLong(fieldSlot);
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_STRING) {
        size_t sizeIncludingNull;
        const char* c = window->getFieldSlotValueString(fieldSlot, &sizeIncludingNull);
        *value = sizeIncludingNull > 1 ? strtoll(c, NULL, 0) : 0L;
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_FLOAT) {
        *value = Int64(window->getFieldSlotValueDouble(fieldSlot));
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_NULL) {
        *value = 0;
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_BLOB) {
        *value = 0;
        Slogger::E(STATS_TAG, "Unable to convert BLOB to long");
        return E_SQLITE3_EXCEPTION;
    } else {
        *value = 0;
        return ThrowUnknownTypeException(type);
    }
    return NOERROR;
}

ECode CursorWindow::NativeGetDouble(
    /* [in] */ Int64 windowPtr,
    /* [in] */ Int32 row,
    /* [in] */ Int32 column,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value)

    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    LOG_WINDOW("Getting double for (%d, %d) from %p", row, column, window);

    NativeCursorWindow::FieldSlot* fieldSlot = window->getFieldSlot(row, column);
    if (!fieldSlot) {
        *value = 0.0;
        return ThrowExceptionWithRowCol(row, column);
    }

    int32_t type = window->getFieldSlotType(fieldSlot);
    if (type == NativeCursorWindow::FIELD_TYPE_FLOAT) {
        *value = window->getFieldSlotValueDouble(fieldSlot);
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_STRING) {
        size_t sizeIncludingNull;
        const char* c = window->getFieldSlotValueString(fieldSlot, &sizeIncludingNull);
        *value = sizeIncludingNull > 1 ? strtod(c, NULL) : 0.0;
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_INTEGER) {
        *value = double(window->getFieldSlotValueLong(fieldSlot));
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_NULL) {
        *value = 0.0;
    }
    else if (type == NativeCursorWindow::FIELD_TYPE_BLOB) {
        *value = 0.0;
        Slogger::E(STATS_TAG, "Unable to convert BLOB to double");
        return E_SQLITE3_EXCEPTION;
    }
    else {
        *value = 0.0;
        return ThrowUnknownTypeException(type);
    }
    return NOERROR;
}

Boolean CursorWindow::NativePutBlob(
    /* [in] */ Int64 windowPtr,
    /* [in] */ const ArrayOf<Byte>& value,
    /* [in] */ Int32 row,
    /* [in] */ Int32 column)
{
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    Int32 len = value.GetLength();

    void* _value = value.GetPayload();
    android::status_t status = window->putBlob(row, column, _value, len);

    if (status) {
        LOG_WINDOW("Failed to put blob. error=%d", status);
        return FALSE;
    }

    LOG_WINDOW("%d,%d is BLOB with %u bytes", row, column, len);
    return TRUE;
}

Boolean CursorWindow::NativePutString(
    /* [in] */ Int64 windowPtr,
    /* [in] */ const String& valueStr,
    /* [in] */ Int32 row,
    /* [in] */ Int32 column)
{
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    Int32 sizeIncludingNull = valueStr.GetByteLength() + 1;
    if (valueStr.IsNull()) {
        LOG_WINDOW("value can't be transferred to UTFChars");
        return FALSE;
    }
    android::status_t status = window->putString(row, column, valueStr.string(), sizeIncludingNull);

    if (status) {
        LOG_WINDOW("Failed to put string. error=%d", status);
        return FALSE;
    }

    LOG_WINDOW("%d,%d is TEXT with %u bytes", row, column, sizeIncludingNull);
    return TRUE;
}

Boolean CursorWindow::NativePutInt64(
    /* [in] */ Int64 windowPtr,
    /* [in] */ Int64 value,
    /* [in] */ Int32 row,
    /* [in] */ Int32 column)
{
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    android::status_t status = window->putLong(row, column, value);

    if (status) {
        LOG_WINDOW("Failed to put long. error=%d", status);
        return FALSE;
    }

    LOG_WINDOW("%d,%d is INTEGER 0x%016llx", row, column, value);
    return TRUE;
}

Boolean CursorWindow::NativePutDouble(
    /* [in] */ Int64 windowPtr,
    /* [in] */ Double value,
    /* [in] */ Int32 row,
    /* [in] */ Int32 column)
{
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    android::status_t status = window->putDouble(row, column, value);

    if (status) {
        LOG_WINDOW("Failed to put double. error=%d", status);
        return FALSE;
    }

    LOG_WINDOW("%d,%d is FLOAT %lf", row, column, value);
    return TRUE;
}

Boolean CursorWindow::NativePutNull(
    /* [in] */ Int64 windowPtr,
    /* [in] */ Int32 row,
    /* [in] */ Int32 column)
{
    NativeCursorWindow* window = reinterpret_cast<NativeCursorWindow*>(windowPtr);
    android::status_t status = window->putNull(row, column);

    if (status) {
        LOG_WINDOW("Failed to put null. error=%d", status);
        return FALSE;
    }

    LOG_WINDOW("%d,%d is NULL", row, column);
    return TRUE;
}

ECode CursorWindow::constructor()
{
    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mCloseGuard);
    return NOERROR;
}

ECode CursorWindow::constructor(
    /* [in] */ const String& name)
{
    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mCloseGuard);

    mStartPos = 0;
    mName = !name.IsNullOrEmpty() ? name : String("<unnamed>");

    if (sCursorWindowSize < 0) {
        /** The cursor window size. resource xml file specifies the value in kB.
        * convert it to bytes here by multiplying with 1024.
        */
        AutoPtr<IResources> res = CResources::GetSystem();
        Int32 i;
        res->GetInteger(R::integer::config_cursorWindowSize, &i);
        sCursorWindowSize = i * 1024;
    }

    mWindowPtr = NativeCreate(mName, GetCursorWindowSize());
    if (mWindowPtr == 0) {
        //throw new CursorWindowAllocationException("Cursor window allocation of " +
        //        (sCursorWindowSize / 1024) + " kb failed. " + printStats());
        Slogger::E(STATS_TAG, "Cursor window allocation of %d kb failed. %s", (GetCursorWindowSize() / 1024), PrintStats().string());
        return E_CURSOR_WINDOW_ALLOCATION_EXCEPTION;
    }
    mCloseGuard->Open(String("CursorWindow::Close"));
    RecordNewWindow(Binder::GetCallingPid(), mWindowPtr);
    return NOERROR;
}

ECode CursorWindow::constructor(
    /* [in] */ Boolean localWindow)
{
    return constructor(String(NULL));
}

ECode CursorWindow::Dispose()
{
    if (mCloseGuard != NULL) {
        mCloseGuard->Close();
    }
    if (mWindowPtr != 0) {
        RecordClosingOfWindow(mWindowPtr);
        NativeDispose(mWindowPtr);
        mWindowPtr = 0;
    }
    return NOERROR;
}

ECode CursorWindow::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode CursorWindow::Clear()
{
    AcquireReference();
    //try {
    mStartPos = 0;
    NativeClear(mWindowPtr);
    //} finally {
    ReleaseReference();
    //}
    return NOERROR;
}

ECode CursorWindow::GetStartPosition(
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos)
    *pos = mStartPos;
    return mStartPos;
}

ECode CursorWindow::SetStartPosition(
    /* [in] */ Int32 pos)
{
    mStartPos = pos;
    return NOERROR;
}

ECode CursorWindow::GetNumRows(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    AcquireReference();
    //try {
    *num = NativeGetNumRows(mWindowPtr);
    //} finally {
    ReleaseReference();
    //}
    return NOERROR;
}

ECode CursorWindow::SetNumColumns(
    /* [in] */ Int32 columnNum,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    AcquireReference();
    //try {
    *succeeded = NativeSetNumColumns(mWindowPtr, columnNum);
    //} finally {
    ReleaseReference();
    //}
    return NOERROR;
}

ECode CursorWindow::AllocRow(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    AcquireReference();
    //try {
    *succeeded = NativeAllocRow(mWindowPtr);
    //} finally {
    ReleaseReference();
    //}
    return NOERROR;
}

ECode CursorWindow::FreeLastRow()
{
    AcquireReference();
    //try {
    NativeFreeLastRow(mWindowPtr);
    //} finally {
    ReleaseReference();
    //}
    return NOERROR;
}

ECode CursorWindow::IsNull(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Boolean* isNull)
{
    VALIDATE_NOT_NULL(isNull)
    Int32 type;
    GetType(row, col, &type);
    *isNull = (type == ICursor::FIELD_TYPE_NULL);
    return NOERROR;
}

ECode CursorWindow::IsBlob(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Boolean* isBlob)
{
    VALIDATE_NOT_NULL(isBlob)
    Int32 type;
    GetType(row, col, &type);
    *isBlob =  type == (ICursor::FIELD_TYPE_BLOB || type == ICursor::FIELD_TYPE_NULL);
    return NOERROR;
}

ECode CursorWindow::IsInt64(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Boolean* isInt64)
{
    VALIDATE_NOT_NULL(isInt64)
    Int32 type;
    GetType(row, col, &type);
    *isInt64 = type == ICursor::FIELD_TYPE_INTEGER;
    return NOERROR;
}

ECode CursorWindow::IsFloat(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Boolean* isFloat)
{
    VALIDATE_NOT_NULL(isFloat)
    Int32 type;
    GetType(row, col, &type);
    *isFloat = type == ICursor::FIELD_TYPE_FLOAT;
    return NOERROR;
}

ECode CursorWindow::IsString(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Boolean* isString)
{
    VALIDATE_NOT_NULL(isString)
    Int32 type;
    GetType(row, col, &type);
    *isString = type == ICursor::FIELD_TYPE_STRING || type == ICursor::FIELD_TYPE_NULL;
    return NOERROR;
}

ECode CursorWindow::GetType(
    /* [in] */ Int32 row,
    /* [in] */ Int32 column,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    AcquireReference();
    //try {
    *type = NativeGetType(mWindowPtr, row - mStartPos, column);
    //} finally {
    ReleaseReference();
    //}
    return NOERROR;
}

ECode CursorWindow::GetBlob(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ ArrayOf<Byte>** blob)
{
    VALIDATE_NOT_NULL(blob)
    AcquireReference();
    //try {
    ECode ec = NativeGetBlob(mWindowPtr, row - mStartPos, col, blob);
    //} finally {
    ReleaseReference();
    //}
    return ec;
}

ECode CursorWindow::GetString(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    AcquireReference();
    //try {
    ECode ec = NativeGetString(mWindowPtr, row - mStartPos, col, str);
    //} finally {
    ReleaseReference();
    //}
    return ec;
}

ECode CursorWindow::CopyStringToBuffer(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [in] */ ICharArrayBuffer* buffer)
{
    if (buffer == NULL) {
        //throw new IllegalArgumentException("CharArrayBuffer should not be null");
        Slogger::E(STATS_TAG, "CharArrayBuffer should not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AcquireReference();
    //try {
    ECode ec = NativeCopyStringToBuffer(mWindowPtr, row - mStartPos, col, buffer);
    //} finally {
    ReleaseReference();
    //}
    return ec;
}

ECode CursorWindow::GetInt64(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    AcquireReference();
    //try {
    ECode ec = NativeGetInt64(mWindowPtr, row - mStartPos, col, value);
    //} finally {
    ReleaseReference();
    //}
    return ec;
}

ECode CursorWindow::GetDouble(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value)
    AcquireReference();
    //try {
    ECode ec = NativeGetDouble(mWindowPtr, row - mStartPos, col, value);
    //} finally {
    ReleaseReference();
    //}
    return ec;
}

ECode CursorWindow::GetInt16(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Int16* value)
{
    VALIDATE_NOT_NULL(value)
    Int64 v;
    GetInt64(row, col, &v);
    *value = (Int16)v;
    return NOERROR;
}

ECode CursorWindow::GetInt32(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    Int64 v;
    GetInt64(row, col, &v);
    *value = (Int32)v;
    return NOERROR;
}

ECode CursorWindow::GetFloat(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    Double v;
    GetDouble(row, col, &v);
    *value = (Float)v;
    return NOERROR;
}

ECode CursorWindow::PutBlob(
    /* [in] */ const ArrayOf<Byte>& value,
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    AcquireReference();
    //try {
    *succeeded = NativePutBlob(mWindowPtr, value, row - mStartPos, col);
    //} finally {
    ReleaseReference();
    //}
    return NOERROR;
}

ECode CursorWindow::PutString(
    /* [in] */ const String& value,
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    AcquireReference();
    //try {
    *succeeded = NativePutString(mWindowPtr,value, row - mStartPos, col);
    //} finally {
    ReleaseReference();
    //}
    return NOERROR;
}

ECode CursorWindow::PutInt64(
    /* [in] */ Int64 value,
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    AcquireReference();
    //try {
    *succeeded = NativePutInt64(mWindowPtr, value, row - mStartPos, col);
    //} finally {
    ReleaseReference();
    //}
    return NOERROR;
}

ECode CursorWindow::PutDouble(
    /* [in] */ Double value,
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    AcquireReference();
    //try {
    *succeeded = NativePutDouble(mWindowPtr, value, row - mStartPos, col);
    //} finally {
    ReleaseReference();
    //}
    return NOERROR;
}

ECode CursorWindow::PutNull(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded)
    AcquireReference();
    //try {
    *succeeded = NativePutNull(mWindowPtr, row - mStartPos, col);
    //} finally {
    ReleaseReference();
    //}
    return NOERROR;
}

ECode CursorWindow::NewFromParcel(
    /* [in] */ IParcel* parcel,
    /* [out] */ ICursorWindow** cw)
{
    VALIDATE_NOT_NULL(cw)
    AutoPtr<ICursorWindow> cursorWindow;
    CCursorWindow::New((ICursorWindow**)&cursorWindow);
    AutoPtr<IParcelable> p = IParcelable::Probe(cursorWindow);
    p->ReadFromParcel(parcel);
    *cw = cursorWindow;
    REFCOUNT_ADD(*cw)
    return NOERROR;
}

ECode CursorWindow::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mStartPos);
    mWindowPtr = NativeCreateFromParcel(source);
    if (mWindowPtr == 0) {
        Slogger::E("CursorWindow", "Cursor window could not be created from binder.");
        return E_CURSOR_WINDOW_ALLOCATION_EXCEPTION;
    }
    mName = NativeGetName(mWindowPtr);
    return NOERROR;
}

ECode CursorWindow::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    AcquireReference();
    //try {
    dest->WriteInt32(mStartPos);
    ECode ec = NativeWriteToParcel(mWindowPtr, dest);
    //} finally {
    ReleaseReference();
    //}

    Slogger::E("CursorWindow", "WriteToParcel TODO no flags!");
    // if ((flags & PARCELABLE_WRITE_RETURN_VALUE) != 0) {
    //     ReleaseReference();
    // }
    return ec;
}

void CursorWindow::OnAllReferencesReleased()
{
    Dispose();
}

void CursorWindow::RecordNewWindow(
    /* [in] */ Int32 pid,
    /* [in] */ Int64 window)
{
    {    AutoLock syncLock(sWindowToPidMapLock);
        sWindowToPidMap[window] = pid;
        // if (Log.isLoggable(STATS_TAG, Log.VERBOSE)) {
        //     Log.i(STATS_TAG, "Created a new Cursor. " + printStats());
        // }
    }
}

void CursorWindow::RecordClosingOfWindow(
    /* [in] */ Int64 window)
{
    {    AutoLock syncLock(sWindowToPidMapLock);
        if (sWindowToPidMap.IsEmpty()) {
            // this means we are not in the ContentProvider.
            return;
        }
        sWindowToPidMap.Erase(window);
    }
}

String CursorWindow::PrintStats()
{
    StringBuilder buff;
    Int32 myPid = Process::MyPid();
    Int32 total = 0;
    HashMap<Int32, Int32> pidCounts;

    {    AutoLock syncLock(sWindowToPidMapLock);
        if (sWindowToPidMap.IsEmpty()) {
            // this means we are not in the ContentProvider.
            return String("");
        }
        HashMap<Int64, Int32>::Iterator it = sWindowToPidMap.Begin();
        for (; it != sWindowToPidMap.End(); ++it) {
            Int32 pid = it->mSecond;
            Int32 value = pidCounts[pid];
            pidCounts[pid] = ++value;
        }
    }

    HashMap<Int32, Int32>::Iterator pidIt = pidCounts.Begin();
    for (; pidIt != pidCounts.End(); ++pidIt) {
        buff += (" (# cursors opened by ");
        Int32 pid = pidIt->mFirst;
        if (pid == myPid) {
            buff += ("this proc=");
        }
        else {
            buff += "pid ";
            buff += pid;
            buff += "=";
        }
        Int32 num = pidIt->mSecond;
        buff += num;
        buff += (")");
        total += num;
    }
    // limit the returned string size to 1000
    String s;
    if (buff.GetLength() > 980) {
        buff.Substring(0, 980, &s);
    }
    else {
        s = buff.ToString();
    }
    StringBuilder sb("# Open Cursors=");
    sb += total;
    sb += s;
    return sb.ToString();
}

ECode CursorWindow::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    String name;
    GetName(&name);
    StringBuilder sb(name);
    sb += " {";
    sb += mWindowPtr;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
