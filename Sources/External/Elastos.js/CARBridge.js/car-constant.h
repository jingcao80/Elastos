#ifndef __CAR_BRIDGE_CAR_CONSTANT_H
# define __CAR_BRIDGE_CAR_CONSTANT_H

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

extern ::v8::Local<::v8::NumberObject> CARConstant(IConstantInfo const *constantInfo);

CAR_BRIDGE_NAMESPACE_END

#endif
