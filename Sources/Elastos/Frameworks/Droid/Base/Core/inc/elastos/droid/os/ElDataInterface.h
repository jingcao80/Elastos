
#ifndef __ELASTOS_DROID_OS_ELDATAINTERFACE_H__
#define __ELASTOS_DROID_OS_ELDATAINTERFACE_H__


namespace Elastos {
namespace Droid {
namespace Os {


template<typename T>
class ElDataInterface : public ElRefBase, public IInterface
{
public:
    ElDataInterface(
        /* [in] */ const T& data)
        : mData(data)
    {}

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid)
    {
        if (riid == EIID_IInterface) {
            return (PInterface)this;
        }

        return NULL;
    }

    CARAPI_(UInt32) AddRef()
    {
        return ElRefBase::AddRef();
    }

    CARAPI_(UInt32) Release()
    {
        return ElRefBase::Release();
    }

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID)
    {
        VALIDATE_NOT_NULL(pIID);

        if (pObject == (IInterface*)this) {
            *pIID = EIID_IInterface;
        }
        else {
            return E_INVALID_ARGUMENT;
        }

        return NOERROR;
    }

public:
    T mData;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_ELDATAINTERFACE_H__
