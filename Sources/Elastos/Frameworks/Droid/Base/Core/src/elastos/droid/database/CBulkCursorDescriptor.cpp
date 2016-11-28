
#include "elastos/droid/database/CBulkCursorDescriptor.h"
#include "elastos/droid/database/CCursorWindow.h"
#include "elastos/droid/database/BulkCursorNative.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Database::Sqlite::ISQLiteClosable;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL_2(CBulkCursorDescriptor, Object, IBulkCursorDescriptor, IParcelable)

CAR_OBJECT_IMPL(CBulkCursorDescriptor)

CBulkCursorDescriptor::CBulkCursorDescriptor()
    : mWantsAllOnMoveCalls(FALSE)
    , mCount(0)
    , mWriteToParcelFlags(0)
{}

CBulkCursorDescriptor::~CBulkCursorDescriptor()
{
    // disabled by luo.zhaohui, do not AcquireReference and there is no need to ReleaseReference.
    // see CursorWindow::WriteToParcel(): PARCELABLE_WRITE_RETURN_VALUE
    // if (mWriteToParcelFlags & IParcelable::PARCELABLE_WRITE_RETURN_VALUE) {
    //     if (mWindow != NULL) {
    //         // Logger::I("CBulkCursorDescriptor",
    //         //     " >> ~CBulkCursorDescriptor: mWindow->ReleaseReference() for PARCELABLE_WRITE_RETURN_VALUE");
    //         ISQLiteClosable::Probe(mWindow)->ReleaseReference();
    //     }
    // }
}

ECode CBulkCursorDescriptor::SetWriteToParcelFlags(
    /* [in] */ Int32 flags)
{
    mWriteToParcelFlags = flags;
    return NOERROR;
}

ECode CBulkCursorDescriptor::constructor()
{
    return NOERROR;
}

ECode CBulkCursorDescriptor::WriteToParcel(
    /* [in] */ IParcel* writeout)
{
    AutoPtr<IBinder> binder;
    IBulkCursorProxy* proxy = IBulkCursorProxy::Probe(mCursor);
    if (proxy != NULL) {
        proxy->AsBinder((IBinder**)&binder);
    }
    else {
        binder = IBinder::Probe(mCursor);
    }

    writeout->WriteInterfacePtr(binder);
    writeout->WriteArrayOfString(mColumnNames);
    writeout->WriteBoolean(mWantsAllOnMoveCalls);
    writeout->WriteInt32(mCount);
    if (mWindow != NULL) {
        writeout->WriteInt32(1);
        IParcelable::Probe(mWindow)->WriteToParcel(writeout);
    }
    else {
        writeout->WriteInt32(0);
    }

    return NOERROR;
}

ECode CBulkCursorDescriptor::ReadFromParcel(
    /* [in] */ IParcel* readin)
{
    AutoPtr<IBinder> binder;
    readin->ReadInterfacePtr((Handle32*)&binder);
    readin->ReadArrayOfString((ArrayOf<String>**)&mColumnNames);
    readin->ReadBoolean(&mWantsAllOnMoveCalls);
    readin->ReadInt32(&mCount);
    Int32 value;
    readin->ReadInt32(&value);
    if (value != 0) {
        CCursorWindow::New((ICursorWindow**)&mWindow);
        IParcelable::Probe(mWindow)->ReadFromParcel(readin);
    }

    mCursor = BulkCursorNative::AsInterface(binder);
    return NOERROR;
}

ECode CBulkCursorDescriptor::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CBulkCursorDescriptor{0x");
    sb += StringUtils::ToHexString((Int32)this);
    sb += ", cursor:";
    sb += TO_CSTR(mCursor);
    sb += ", columns:{";
    if (mColumnNames != NULL) {
        for (Int32 i = 0; i < mColumnNames->GetLength(); ++i) {
            if (i != 0) sb += ", ";
            if ((*mColumnNames)[i] != NULL) {
                sb += StringUtils::ToHexString((Int32)(*mColumnNames)[i].string());
                sb += ":";
                sb += (*mColumnNames)[i].string();
            }
            else {
                sb += "null";
            }
        }
    }
    sb += "}, count:";
    sb += mCount;
    sb += ", cursorWindow: ";
    sb += TO_CSTR(mWindow);
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

ECode CBulkCursorDescriptor::SetCursor(
    /* [in] */ IBulkCursor* cursor)
{
    mCursor = cursor;
    return NOERROR;
}

ECode CBulkCursorDescriptor::GetCursor(
    /* [out] */ IBulkCursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    *cursor = mCursor;
    REFCOUNT_ADD(*cursor)
    return NOERROR;
}

ECode CBulkCursorDescriptor::SetColumnNames(
    /* [in] */ ArrayOf<String>* columnNames)
{
    mColumnNames = columnNames;
    return NOERROR;
}

ECode CBulkCursorDescriptor::GetColumnNames(
    /* [out, callee] */ ArrayOf<String>** columnNames)
{
    VALIDATE_NOT_NULL(columnNames)
    *columnNames = mColumnNames;
    REFCOUNT_ADD(*columnNames)
    return NOERROR;
}

ECode CBulkCursorDescriptor::SetWantsAllOnMoveCalls(
    /* [in] */ Boolean wantsAllOnMoveCalls)
{
    mWantsAllOnMoveCalls = wantsAllOnMoveCalls;
    return NOERROR;
}

ECode CBulkCursorDescriptor::GetWantsAllOnMoveCalls(
    /* [out] */ Boolean* wantsAllOnMoveCalls)
{
    VALIDATE_NOT_NULL(wantsAllOnMoveCalls)
    *wantsAllOnMoveCalls = mWantsAllOnMoveCalls;
    return NOERROR;
}

ECode CBulkCursorDescriptor::SetCount(
    /* [in] */ Int32 count)
{
    mCount = count;
    return NOERROR;
}

ECode CBulkCursorDescriptor::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = mCount;
    return NOERROR;
}

ECode CBulkCursorDescriptor::SetWindow(
    /* [in] */ ICursorWindow* window)
{
    mWindow = window;
    return NOERROR;
}

ECode CBulkCursorDescriptor::GetWindow(
    /* [out] */ ICursorWindow** window)
{
    VALIDATE_NOT_NULL(window)
    *window = mWindow;
    REFCOUNT_ADD(*window)
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
