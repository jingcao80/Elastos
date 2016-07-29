#ifndef __CAR_BRIDGE_CAR_MODULE_H
# define __CAR_BRIDGE_CAR_MODULE_H

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

extern ::v8::Local<::v8::ObjectTemplate> CARModuleTemplate(IModuleInfo const *moduleInfo);

CAR_BRIDGE_NAMESPACE_END

#endif
