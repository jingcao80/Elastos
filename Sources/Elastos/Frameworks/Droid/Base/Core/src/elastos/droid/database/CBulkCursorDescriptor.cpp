
#include "elastos/droid/database/CBulkCursorDescriptor.h"
#include "elastos/droid/database/CCursorWindow.h"

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL(CBulkCursorDescriptor, Object, IParcelable);

CAR_OBJECT_IMPL(CBulkCursorDescriptor)

CBulkCursorDescriptor::CBulkCursorDescriptor()
    : mWantsAllOnMoveCalls(FALSE)
    , mCount(0)
{}

ECode CBulkCursorDescriptor::WriteToParcel(
    /* [in] */ IParcel* writeout)
{
    writeout->WriteInterfacePtr(mCursor);
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
    readin->ReadInterfacePtr((Handle32*)&mCursor);
    readin->ReadArrayOfString((ArrayOf<String>**)&mColumnNames);
    readin->ReadBoolean(&mWantsAllOnMoveCalls);
    readin->ReadInt32(&mCount);
    Int32 value;
    if (readin->ReadInt32(&value), value != 0) {
        CCursorWindow::New((ICursorWindow**)&mWindow);
        IParcelable::Probe(mWindow)->ReadFromParcel(readin);
    }
    return NOERROR;
}

ECode CBulkCursorDescriptor::constructor()
{
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
