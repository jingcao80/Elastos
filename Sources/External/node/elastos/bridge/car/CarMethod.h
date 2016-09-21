#ifndef __CARMETHOD_H__
#define __CARMETHOD_H__

#if ENABLE(CAR_BRIDGE)

#include "Bridge.h"

#include <wtf/text/WTFString.h>

namespace JSC {
namespace Bindings {

typedef const char* RuntimeType;

class CarMethod : public Method {
public:
    virtual ~CarMethod() {}

    virtual WTF::String name() const = 0;
    virtual RuntimeType returnTypeClassName() const = 0;
    virtual WTF::String parameterAt(int) const = 0;
    virtual const char* signature() const = 0;
    virtual CarDataType returnType() const = 0;
    virtual bool isStatic() const = 0;

    virtual bool isRunOnUiThread() const = 0;
    virtual void setRunOnUiThread(bool) = 0;

    virtual AutoPtr<IMethodInfo> methodInfo() const = 0;
};

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(CAR_BRIDGE)

#endif //__CARMETHOD_H__