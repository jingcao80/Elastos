#ifndef __CAR_BRIDGE_UTIL_H
# define __CAR_BRIDGE_UTIL_H

# include <node.h>

# include <nan.h>

# include <elastos.h>

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

extern IInterface *Probe(::v8::Local<::v8::Object> object, _ELASTOS EIID const &iid);

extern void AttachAspect(::v8::Local<::v8::Object> object, _ELASTOS ClassID const &aspectId);

extern void DetachAspect(::v8::Local<::v8::Object> object, _ELASTOS ClassID const &aspectId);

CAR_BRIDGE_NAMESPACE_END

#endif
