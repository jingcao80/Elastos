//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_UTILITY_PROPERTYPERMISSION_H__
#define __ELASTOS_UTILITY_PROPERTYPERMISSION_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"

using Elastos::Core::Object;
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