#ifndef __CAR_BRIDGE_CAR_NAMESPACE_H
# define __CAR_BRIDGE_CAR_NAMESPACE_H

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

extern ::v8::Local<::v8::ObjectTemplate> CARNamespaceTemplate(IModuleInfo const *moduleInfo, char const *namespace_);

CAR_BRIDGE_NAMESPACE_END

#endif
