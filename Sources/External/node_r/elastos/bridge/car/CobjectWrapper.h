
#ifndef __COBJECTWRAPPER_H__
#define __COBJECTWRAPPER_H__

#include "eldef.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Node {

class CobjectWrapper
    : public Object
{
    // friend class CarInstance;
    // friend class CarInstanceCobject;

public:
    CobjectWrapper(
        /* [in] */ IInterface* instance,
        /* [in] */ IDataTypeInfo* typeInfo);

    CARAPI_(AutoPtr<IInterface>) GetInstance();

    CARAPI_(AutoPtr<IDataTypeInfo>) GetDataTypeInfo();

private:
    AutoPtr<IInterface> mInstance;
    AutoPtr<IDataTypeInfo> mDataTypeInfo;
};

} // namespace Node
} // namespace Elastos

#endif //__COBJECTWRAPPER_H__
