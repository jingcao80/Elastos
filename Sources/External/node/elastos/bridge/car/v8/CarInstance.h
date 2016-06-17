
#ifndef __CARINSTANCE_H__
#define __CARINSTANCE_H__

#if ENABLE(CAR_BRIDGE)

#include "CarValueV8.h"
#include "CobjectWrapper.h"
#include <wtf/RefCounted.h>

namespace JSC {
namespace Bindings {

class CarKlass;
class CarField;
class CarMethod;

class CarInstance : public RefCounted<CarInstance>
{
public:
    virtual ~CarInstance() {}

    virtual CarKlass* getClass() const = 0;
    // args must be an array of length greater than or equal to the number of
    // arguments expected by the method.
    virtual void invokeMethod(const CarMethod*, CarValue* args, bool* didRaiseUncaughtException) = 0;
    virtual CarValue getField(const CarField*) = 0;

    // These functions are called before and after the main entry points into
    // the native implementations.  They can be used to establish and cleanup
    // any needed state.
    virtual void begin() = 0;
    virtual void end() = 0;

    virtual IInterface* carInstance() const = 0;

    virtual CobjectWrapper* getInstance() const = 0;
};

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(CAR_BRIDGE)

#endif //__CARINSTANCE_H__
