#ifndef __ELASTOS_DROID_BLUETOOTH_LE_RESULTSTORAGEDESCRIPTOR_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_RESULTSTORAGEDESCRIPTOR_H__

#include "Elastos.Droid.Bluetooth.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

/**
  * Describes the way to store scan result.
  *
  * @hide
  */
// @SystemApi
class ResultStorageDescriptor
    : public Object
    , public IResultStorageDescriptor
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL();

    ResultStorageDescriptor();

    /**
      * Constructor of {@link ResultStorageDescriptor}
      *
      * @param type Type of the data.
      * @param offset Offset from start of the advertise packet payload.
      * @param length Byte length of the data
      */
    ResultStorageDescriptor(
        /* [in] */ Int32 type,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI GetType(
        /* [out] */ Int32* result);

    CARAPI GetOffset(
        /* [out] */ Int32* result);

    CARAPI GetLength(
        /* [out] */ Int32* result);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest,
        /* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

private:
    ResultStorageDescriptor(
        /* [in] */ IParcel* in);

private:
    Int32 mType;
    Int32 mOffset;
    Int32 mLength;
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_RESULTSTORAGEDESCRIPTOR_H__
