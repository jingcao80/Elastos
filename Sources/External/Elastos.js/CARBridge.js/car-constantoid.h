#ifndef __CAR_BRIDGE_CAR_CONSTANTOID_H
# define __CAR_BRIDGE_CAR_CONSTANTOID_H

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

template<class ConstantoidInfo>
::v8::Local<::v8::NumberObject> CARConstantoid(ConstantoidInfo const *constantoidInfo, char const *what);

CAR_BRIDGE_NAMESPACE_END

# include "car-constantoid-cc.h"

#endif
