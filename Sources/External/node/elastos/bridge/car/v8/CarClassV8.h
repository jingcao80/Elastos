
#ifndef __CARCLASSV8_H__
#define __CARCLASSV8_H__

#if ENABLE(CAR_BRIDGE)

#include <wtf/HashMap.h>
#include <wtf/Vector.h>
#include <wtf/text/StringHash.h>
#include <wtf/text/WTFString.h>

namespace JSC {
namespace Bindings {

class CarField;
class CarMethod;

typedef WTF::Vector<CarMethod*> MethodList;
typedef WTF::HashMap<WTF::String, CarField*> FieldMap;

class CarKlass
{
public:
    virtual ~CarKlass() {}

    virtual MethodList methodsNamed(const char* name) const = 0;
    virtual CarField* fieldNamed(const char* name) const = 0;

    virtual bool isView() const = 0;
};

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(CAR_BRIDGE)

#endif //__CARCLASSV8_H__
