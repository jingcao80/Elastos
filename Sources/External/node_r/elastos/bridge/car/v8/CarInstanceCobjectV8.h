
#ifndef __CARINSTANCECOBJECTV8_H__
#define __CARINSTANCECOBJECTV8_H__

#include <elastos.h>

#if ENABLE(CAR_BRIDGE)

#include "CarInstanceV8.h"

#include "CarClassV8.h"

namespace JSC {
namespace Bindings {

class CarInstanceCobject : public CarInstance
{
public:
    CarInstanceCobject(CobjectWrapper* objectWrapper, bool requireAnnotation);

    virtual CarKlass* getClass() const;
    virtual void invokeMethod(const CarMethod*, CarValue* args, bool& didRaiseUncaughtException);
    virtual CarValue getField(const CarField*);
    virtual void begin();
    virtual void end();

    IInterface* carInstance() const;

    CobjectWrapper* getInstance() const { return mInstance->get(); }

protected:
    RefPtr<CobjectWrapper> mInstance;
    mutable OwnPtr<CarKlass> mClass;
    bool mRequireAnnotation;

    typedef WTF::HashMap<WTF::String, CarValue> DynamicFieldMap;
    DynamicFieldMap mDynamicFields;

    //pthread_mutex_t mMutex;
    //pthread_mutex_t* pMutex;
};

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(CAR_BRIDGE)

#endif //__CARINSTANCECOBJECTV8_H__
