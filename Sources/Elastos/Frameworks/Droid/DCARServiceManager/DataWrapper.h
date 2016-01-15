
#ifndef __DATAWRAPPER_H__
#define __DATAWRAPPER_H__

#include <elastos.h>

using namespace Elastos;

class DataWrapper : public ElLightRefBase
{
public:
    DataWrapper(
        /* [in] */ Byte* data,
        /* [in] */ Int32 size);

    CARAPI ReadString(
        /* [out] */ String* value);

    CARAPI WriteString(
        /* [in] */ const String& value);

    CARAPI ReadInt32(
        /* [out] */ Int32* value);

    CARAPI WriteInt32(
        /* [in] */ Int32 value);

    CARAPI ReadEMuid(
        /* [out] */ EMuid* value);

    CARAPI WriteEMuid(
        /* [in] */ const EMuid& value);

    CARAPI_(Byte*) Data();

    CARAPI_(UInt32) DataSize();

private:
    Byte* mData;
    UInt32 mDataCapacity;
    UInt32 mDataPos;
};

#endif //__DATAWRAPPER_H__
