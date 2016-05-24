
#ifndef __CARCLASSV8_H__
#define __CARCLASSV8_H__

#if ENABLE(CAR_BRIDGE)

#include <elastos.h>
#include "CobjectWrapper.h"
#include "CarKlass.h"
#include "CarField.h"

namespace JSC {
namespace Bindings {

class CarClassV8 : public CarKlass
{
public:
    CarClassV8(CobjectWrapper*, bool);

    virtual ~CarClassV8();

    // CarClass implementation
    virtual MethodList methodsNamed(const char* name) const;
    virtual CarField* fieldNamed(const char* name) const;

    virtual bool isView() const;
private:
    typedef WTF::HashMap<WTF::String, MethodList*> MethodListMap;
    MethodListMap mMethods;
    FieldMap mFields;
    bool mRequireAnnotation;

    bool mIsView;
};

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(CAR_BRIDGE)

#endif //__CARCLASSV8_H__
