
#ifndef __CARCLASSCOBJECTV8_H__
#define __CARCLASSCOBJECTV8_H__

#if ENABLE(CAR_BRIDGE)

#include <elastos.h>
#include "CobjectWrapper.h"
#include "CarClassV8.h"
#include "CarFieldV8.h"

namespace JSC {
namespace Bindings {

class CarClassCobject : public CarKlass
{
public:
    CarClassCobject(CobjectWrapper*, bool);

    virtual ~CarClassCobject();

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

#endif //__CARCLASSCOBJECTV8_H__
