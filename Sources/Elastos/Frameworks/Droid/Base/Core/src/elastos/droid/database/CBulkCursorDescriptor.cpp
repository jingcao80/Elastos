
#include "elastos/droid/database/CBulkCursorDescriptor.h"
#include "elastos/droid/database/CCursorWindow.h"
#include "elastos/droid/database/BulkCursorNative.h"
#include "Elastos.Droid.Os.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL_2(CBulkCursorDescriptor, Object, IBulkCursorDescriptor, IParcelable)

CAR_OBJECT_IMPL(CBulkCursorDescriptor)

CBulkCursorDescriptor::CBulkCursorDescriptor()
    : mWantsAllOnMoveCalls(FALSE)
    , mCount(0)
{}

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

    Logger::D("CBulkCursorDescriptor", " >> WriteToParcel: binder: %s, cursor: %s", TO_CSTR(binder), TO_CSTR(mCursor));
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

    Logger::D("CBulkCursorDescriptor", " >> ReadFromParcel: binder: %s, cursor: %s", TO_CSTR(binder), TO_CSTR(mCursor));
    return NOERROR;
}

ECode CBulkCursorDescriptor::constructor()
{
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
