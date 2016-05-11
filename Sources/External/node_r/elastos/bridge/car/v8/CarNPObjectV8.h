
#ifndef __CARNPOBJECTV8_H__
#define __CARNPOBJECTV8_H__

#if ENABLE(CAR_BRIDGE)

#include "npruntime.h"
#include <wtf/RefPtr.h>

namespace JSC {
namespace Bindings {

class CarInstance;

struct CarNPObject
{
    NPObject mObject;
    RefPtr<CarInstance> mInstance;
};

NPObject* CarInstanceToNPObject(CarInstance*);
CarInstance* ExtractCarInstance(NPObject*);

void CarNPObjectInvalidate(NPObject *npobj);
bool CarNPObjectHasMethod(NPObject *npobj, NPIdentifier name);
bool CarNPObjectInvoke(NPObject *npobj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool CarNPObjectInvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool CarNPObjectHasProperty(NPObject *npobj, NPIdentifier name);
bool CarNPObjectGetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result);
bool CarNPObjectSetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value);
bool CarNPObjectRemoveProperty(NPObject *npobj, NPIdentifier name);
bool CarNPObjectEnumerate(NPObject *npobj, NPIdentifier **value, uint32_t *count);
bool CarNPObjectConstruct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(CAR_BRIDGE)

#endif //__CARNPOBJECTV8_H__
