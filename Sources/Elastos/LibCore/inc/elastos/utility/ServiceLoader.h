#ifndef __ELASTOS_UTILITY_PROPERTYPERMISSION_H__
#define __ELASTOS_UTILITY_PROPERTYPERMISSION_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Singleton.h"

using Elastos::Core::Singleton;
using Elastos::Core::IClassLoader;

namespace Elastos {
namespace Utility {

class ServiceLoader
    : public Object
    , public IServiceLoader
    , public IIterable
{
private:
    class ServiceIterator
        : public Object
        , public IIterator
    {
    public:
        CAR_INTERFACE_DECL()

        ServiceIterator(
            /* [in] */ ServiceLoader* sl);

        CARAPI HasNext(
            /* [out] */ Boolean* result);

        CARAPI GetNext(
            /* [out] */ IInterface** object);

        CARAPI Remove();

    private:
        CARAPI ReadClass();

        CARAPI CheckValidJavaClassName(
            /* [in] */ const String& classname);

    private:
        const InterfaceID mServiceIt;

        const AutoPtr<IClassLoader> mClassLoaderIt;

        const AutoPtr<ISet> mServicesIt;

        Boolean mIsRead;

        AutoPtr<ILinkedList> mQueue;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI Reload();

    CARAPI GetIterator(
        /* [out] */ IIterator** it);

    static CARAPI_(AutoPtr<IServiceLoader>) Load(
        /* [in] */ InterfaceID service,
        /* [in] */ IClassLoader* cl);

    static CARAPI_(AutoPtr<IServiceLoader>) Load(
        /* [in] */ InterfaceID service);

    static CARAPI_(AutoPtr<IServiceLoader>) LoadInstalled(
        /* [in] */ InterfaceID service);

    static CARAPI_(AutoPtr<IInterface>) LoadFromSystemProperty(
        /* [in] */ const InterfaceID& service);

    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI_(InterfaceID) GetService();

    CARAPI_(AutoPtr<IClassLoader>) GetClassLoader();

    CARAPI_(AutoPtr<ISet>) GetServices();

private:
    ServiceLoader(
        /* [in] */ InterfaceID service,
        /* [in] */ IClassLoader* cl);

    CARAPI InternalLoad();

private:
    const InterfaceID mService;

    const AutoPtr<IClassLoader> mClassLoader;

    const AutoPtr<ISet> mServices;
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_PROPERTYPERMISSION_H__