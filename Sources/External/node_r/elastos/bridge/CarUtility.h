
#ifndef __CARUTILITY_H__
#define __CARUTILITY_H__

#include <v8.h>
#include "CarValueV8.h"

namespace Elastos {
namespace Node {

class CarUtility
{
public:
    static CARAPI ConvertV8ObjectToCarValue(
        /* [in] */ v8::Local<v8::Value> object,
        /* [in] */ CarDataType carType,
        /* [in] */ CarValue& result);

    static CARAPI FillingV8ObjectToArgumentList(
        /* [in] */ v8::Local<v8::Value> object,
        /* [in] */ IParamInfo* paramInfo,
        /* [in] */ IArgumentList* argumentList);

private:
    CarUtility();
};

} // namespace Node
} // namespace Elastos

#endif //__CARUTILITY_H__
