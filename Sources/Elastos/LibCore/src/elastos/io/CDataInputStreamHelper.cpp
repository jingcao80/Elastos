
#include "CDataInputStreamHelper.h"
#include "DataInputStream.h"

namespace Elastos {
namespace IO {

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
