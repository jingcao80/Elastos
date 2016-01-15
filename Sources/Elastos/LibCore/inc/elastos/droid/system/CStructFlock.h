#ifndef __ELASTOS_DROID_SYSTEM_CSTRUCTFLOCK_H__
#define __ELASTOS_DROID_SYSTEM_CSTRUCTFLOCK_H__

#include "core/Object.h"
#include "_Elastos_Droid_System_CStructFlock.h"


namespace Elastos {
namespace Droid {
namespace System {

CarClass(CStructFlock)
    , public Object
    , public IStructFlock
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /** The operation type, one of F_RDLCK, F_WRLCK, or F_UNLCK. */
    CARAPI GetType(
        /* [out] */ Int16* type);

    CARAPI SetType(
        /* [in] */ Int16 type);

    /** How to interpret l_start, one of SEEK_CUR, SEEK_END, SEEK_SET. */
    CARAPI GetWhence(
        /* [out] */ Int16* whence);

    CARAPI SetWhence(
        /* [in] */ Int16 whence);

    /** Start offset. */
    CARAPI GetStart(
        /* [out] */ Int64* start);

    CARAPI SetStart(
        /* [in] */ Int64 start);

    /** Byte count to operate on. */
    CARAPI GetLen(
        /* [out] */ Int64* len);

    CARAPI SetLen(
        /* [in] */ Int64 len);

    /** Process blocking our lock (filled in by F_GETLK, otherwise unused). */
    CARAPI GetPid(
        /* [out] */ Int32* pid);

    CARAPI SetPid(
        /* [in] */ Int32 pid);

private:
    Int16 mType;
    Int16 mWhence;
    Int64 mStart;
    Int64 mLen;
    Int32 mPid;

};

}// namespace System
}// namespace Droid
}// namespace Elastos


#endif
