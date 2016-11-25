#ifndef __ELASTOS_DROID_DATABASE_CSQLITEBULKCURSORDESCRIPTOR_H__
#define __ELASTOS_DROID_DATABASE_CSQLITEBULKCURSORDESCRIPTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Database_CBulkCursorDescriptor.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Database {

CarClass(CBulkCursorDescriptor)
    , public Object
    , public IBulkCursorDescriptor
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBulkCursorDescriptor();

    ~CBulkCursorDescriptor();

    CARAPI constructor();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* writeout);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* readin);

    CARAPI SetCursor(
        /* [in] */ IBulkCursor* cursor);

    CARAPI GetCursor(
        /* [out] */ IBulkCursor** cursor);

    CARAPI SetColumnNames(
        /* [in] */ ArrayOf<String>* columnNames);

    CARAPI GetColumnNames(
        /* [out, callee] */ ArrayOf<String>** columnNames);

    CARAPI SetWantsAllOnMoveCalls(
        /* [in] */ Boolean wantsAllOnMoveCalls);

    CARAPI GetWantsAllOnMoveCalls(
        /* [out] */ Boolean* wantsAllOnMoveCalls);

    CARAPI SetCount(
        /* [in] */ Int32 count);

    CARAPI GetCount(
        /* [out] */ Int32* count);

    CARAPI SetWindow(
        /* [in] */ ICursorWindow* window);

    CARAPI GetWindow(
        /* [out] */ ICursorWindow** window);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SetWriteToParcelFlags(
        /* [in] */ Int32 flags);

private:
    AutoPtr<IBulkCursor> mCursor;
    AutoPtr< ArrayOf<String> > mColumnNames;
    Boolean mWantsAllOnMoveCalls;
    Int32 mCount;
    AutoPtr<ICursorWindow> mWindow;
    Int32 mWriteToParcelFlags;
};

} //Database
} //Droid
} //Elastos
#endif //__ELASTOS_DROID_DATABASE_CSQLITEBULKCURSORDESCRIPTOR_H__
