#include "elastos/droid/database/AbstractWindowedCursor.h"
#include "elastos/droid/database/CCursorWindow.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL(AbstractWindowedCursor, AbstractCursor, IAbstractWindowedCursor);

ECode AbstractWindowedCursor::GetBlob(
    /* [in] */ Int32 columnIndex,
    /* [out] */ ArrayOf<Byte>** blob)
{
    VALIDATE_NOT_NULL(blob)
    FAIL_RETURN(CheckPosition())
    return mWindow->GetBlob(mPos, columnIndex, blob);
}

ECode AbstractWindowedCursor::GetString(
    /* [in] */ Int32 columnIndex,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    FAIL_RETURN(CheckPosition())
    return mWindow->GetString(mPos, columnIndex, str);
}

ECode AbstractWindowedCursor::CopyStringToBuffer(
    /* [in] */ Int32 columnIndex,
    /* [in] */ ICharArrayBuffer* buffer)
{
    FAIL_RETURN(CheckPosition())

    return mWindow->CopyStringToBuffer(mPos, columnIndex, buffer);
}

ECode AbstractWindowedCursor::GetInt16(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int16* value)
{
    VALIDATE_NOT_NULL(value)
    FAIL_RETURN(CheckPosition())
    return mWindow->GetInt16(mPos, columnIndex, value);
}

ECode AbstractWindowedCursor::GetInt32(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    FAIL_RETURN(CheckPosition())
    return mWindow->GetInt32(mPos, columnIndex, value);
}

ECode AbstractWindowedCursor::GetInt64(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    FAIL_RETURN(CheckPosition())
    return mWindow->GetInt64(mPos, columnIndex, value);
}

ECode AbstractWindowedCursor::GetFloat(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    FAIL_RETURN(CheckPosition())
    return mWindow->GetFloat(mPos, columnIndex, value);
}

ECode AbstractWindowedCursor::GetDouble(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value)
    FAIL_RETURN(CheckPosition())
    return mWindow->GetDouble(mPos, columnIndex, value);
}

ECode AbstractWindowedCursor::IsNull(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* isNull)
{
    VALIDATE_NOT_NULL(isNull)
    FAIL_RETURN(CheckPosition())
    Int32 type;
    mWindow->GetType(mPos, columnIndex, &type);
    *isNull = type== ICursor::FIELD_TYPE_NULL;
    return NOERROR;
}

ECode AbstractWindowedCursor::IsBlob(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* isBlob)
{
    VALIDATE_NOT_NULL(isBlob)
    FAIL_RETURN(CheckPosition())
    Int32 type;
    *isBlob = (GetType(columnIndex, &type), type == ICursor::FIELD_TYPE_BLOB);
    return NOERROR;
}

ECode AbstractWindowedCursor::IsString(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* isString)
{
    VALIDATE_NOT_NULL(isString)
    FAIL_RETURN(CheckPosition())
    Int32 type;
    *isString = (GetType(columnIndex, &type), type == ICursor::FIELD_TYPE_STRING);
    return NOERROR;
}

ECode AbstractWindowedCursor::IsInt64(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* isInt64)
{
    VALIDATE_NOT_NULL(isInt64)
    FAIL_RETURN(CheckPosition())
    Int32 type;
    *isInt64 = (GetType(columnIndex, &type), type == ICursor::FIELD_TYPE_INTEGER);
    return NOERROR;
}

ECode AbstractWindowedCursor::IsFloat(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* isFloat)
{
    VALIDATE_NOT_NULL(isFloat)
    FAIL_RETURN(CheckPosition())
    Int32 type;
    *isFloat = (GetType(columnIndex, &type), type == ICursor::FIELD_TYPE_FLOAT);
    return NOERROR;
}

ECode AbstractWindowedCursor::GetType(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    FAIL_RETURN(CheckPosition())
    return mWindow->GetType(mPos, columnIndex, type);
}

ECode AbstractWindowedCursor::CheckPosition()
{
    FAIL_RETURN(AbstractCursor::CheckPosition());

    if (mWindow == NULL) {
//        throw new StaleDataException("Access closed cursor");
        Slogger::E(String("AbstractWindowedCursor"), "Access closed cursor");
        return E_STALE_DATA_EXCEPTION;
    }
    return NOERROR;
}

ECode AbstractWindowedCursor::GetWindow(
    /* [out] */ ICursorWindow** window)
{
    VALIDATE_NOT_NULL(window)
    *window = mWindow;
    REFCOUNT_ADD(*window)
    return NOERROR;
}

ECode AbstractWindowedCursor::SetWindow(
    /* [in] */ ICursorWindow* window)
{
    if (window != mWindow) {
        CloseWindow();
        mWindow = window;
    }
    return NOERROR;
}

ECode AbstractWindowedCursor::HasWindow(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWindow != NULL;
    return NOERROR;
}

ECode AbstractWindowedCursor::CloseWindow()
{
    if (mWindow != NULL) {
        ICloseable::Probe(mWindow)->Close();
        mWindow = NULL;
    }
    return NOERROR;
}

ECode AbstractWindowedCursor::ClearOrCreateWindow(
    /* [in] */ const String& name)
{
    if (mWindow == NULL) {
        CCursorWindow::New(name, (ICursorWindow**)&mWindow);
    }
    else {
        mWindow->Clear();
    }
    return NOERROR;
}

ECode AbstractWindowedCursor::OnDeactivateOrClose()
{
    AbstractCursor::OnDeactivateOrClose();
    CloseWindow();
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
