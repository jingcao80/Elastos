
#include "CDataInputStreamHelper.h"
#include "DataInputStream.h"

namespace Elastos {
namespace IO {

CAR_SINGLETON_IMPL(CDataInputStreamHelper)

CAR_INTERFACE_IMPL(CDataInputStreamHelper, Singleton, IDataInputStreamHelper)

CDataInputStreamHelper::ReadUTF(
    /* [in] */ IDataInput* dataInput,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    Int32 value = 0;
    dataInput->ReadUnsignedInt16(&value);
    *str = DataInputStream::DecodeUTF(value, dataInput);
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
