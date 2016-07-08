#ifndef __RIL_PARCEL_H__
#define __RIL_PARCEL_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include <binder/Parcel.h>
//#include <marshal_ipc.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class RILParcel
    : public Object
{
public:
    RILParcel();
    ~RILParcel();

    CARAPI WriteInt32(
        /* [in] */ Int32 val);

    CARAPI Marshall(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI WriteString(
        /* [in] */ const String& val);

    CARAPI WriteByteArray(
        /* [in] */ ArrayOf<Byte>* array);

    CARAPI WriteByte(
        /* [in] */ Byte val);

    CARAPI WriteStringArray(
        /* [in] */ ArrayOf<String>* array);

    CARAPI WriteInt32Array(
        /* [in] */ ArrayOf<Int32>* array);

    CARAPI Unmarshall(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI SetDataPosition(
        /* [in] */ Int32 pos);

    CARAPI ReadInt32(
        /* [out] */ Int32* result);

    CARAPI ReadInt64(
        /* [out] */ Int64* result);

    CARAPI ReadString(
        /* [out] */ String* result);

    CARAPI ReadStringArray(
        /* [out] */ ArrayOf<String>** result);

    CARAPI CreateByteArray(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI DataAvail(
        /* [out] */ Int32* result);

private:
    android::Parcel* parcel;
};

} // Telephony
} // Internal
} // Droid
} // Elastos


#endif //__RIL_PARCEL_H__
