
#include "CarValueV8.h"

namespace Elastos {
namespace Node {

CarValue::CarValue()
    : mIOAttribute(ParamIOAttribute_In)
    , mType(0)
    , mByteValue(0)
    , mBooleanValue(FALSE)
    , mInt16Value(0)
    , mInt32Value(0)
    , mInt64Value(0)
    , mCharValue(0)
    , mFloatValue(0.0)
    , mDoubleValue(0.0)
    , mECodeValue(NOERROR)
    , mEnumValue(0)
    , mCarQuintet(NULL)
    , mElementType(0)
{
    mCLSID.mUunm = mUUnm;
}

CarValue::~CarValue()
{
    switch (mElementType) {
        case CarDataType_Int16:
        case CarDataType_Int32:
        case CarDataType_Int64:
        case CarDataType_Byte:
        case CarDataType_Char32:
        case CarDataType_Float:
        case CarDataType_Double:
        case CarDataType_Boolean:
        case CarDataType_ECode:
        case CarDataType_Enum:
        case CarDataType_EMuid:
        case CarDataType_EGuid:
            _CarQuintet_Release(mCarQuintet);
            mCarQuintet = NULL;
            break;
        case CarDataType_String: {
            ArrayOf<String>* strArray = reinterpret_cast< ArrayOf<String>* >(mCarQuintet);
            strArray->Release();
            mCarQuintet = NULL;
            break;
        }
        case CarDataType_Interface: {
            ArrayOf<IInterface*>* itfArray = reinterpret_cast< ArrayOf<IInterface*>* >(mCarQuintet);
            itfArray->Release();
            mCarQuintet = NULL;
            break;
        }
        default:
            break;
    }
}

} // namespace Node
} // namespace Elastos
