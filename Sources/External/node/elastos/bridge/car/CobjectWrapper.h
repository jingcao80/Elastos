#ifndef __COBJECTWRAPPER_H__
#define __COBJECTWRAPPER_H__

#if ENABLE(CAR_BRIDGE)

#include "ext/frameworkdef.h"
#include <elastos.h>

namespace JSC {
namespace Bindings {

class CobjectWrapper : IInterface
{
friend class CarInstance;
friend class CarInstanceV8;

public:
    PInterface Probe(
        /* [in] */ REIID riid)
    {
        if (riid == EIID_IInterface) {
            return (IInterface *)this;
        }
        return (IInterface *)this;
    }
    UInt32 AddRef()
    {
        mRefCount++;
        return mRefCount;
    }

    UInt32 Release()
    {
        if (!--mRefCount)
            delete this;
        return mRefCount;
    }
    ECode GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid)
    {
        if (object == (IInterface*)(IInterface*)this) {
            *iid = EIID_IInterface;
            return NOERROR;
        }
        return NOERROR;
    }

    AutoPtr<IInterface> getInstance() const { return mInstance; }
    void setInstance(IInterface* instance) {
        mInstance = instance;
        REFCOUNT_ADD(instance);
    }

    AutoPtr<IDataTypeInfo> getDataTypeInfo() const { return mDataTypeInfo; }
    void setDataTypeInfo(IDataTypeInfo* dataTypeInfo) {
        mDataTypeInfo = dataTypeInfo;
    }

    void ref() { mRefCount++; }
    void deref()
    {
        if (!--mRefCount)
            delete this;
    }

    CobjectWrapper* get(){return this;}

    CobjectWrapper(IInterface*, IDataTypeInfo*);

protected:
    ~CobjectWrapper();

    AutoPtr<IInterface> mInstance;
    AutoPtr<IDataTypeInfo> mDataTypeInfo;

private:
    unsigned int mRefCount;
};

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(CAR_BRIDGE)

#endif //__COBJECTWRAPPER_H__