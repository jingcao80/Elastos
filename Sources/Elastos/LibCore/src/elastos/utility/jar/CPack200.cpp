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

#include "CPack200.h"
#include "CSystem.h"

using Elastos::Core::ISystem;
using Elastos::Core::IClassLoader;

namespace Elastos {
namespace Utility {
namespace Jar {

static const String TAG("CPack200");

static const String SYSTEM_PROPERTY_PACKER("java.util.jar.Pack200.Packer");

static const String SYSTEM_PROPERTY_UNPACKER("java.util.jar.Pack200.Unpacker");

#define LOAD_CLASS(var, var_type) \
do {\
    AutoPtr<IClassLoader> classLoader; \
    AutoPtr<IClassInfo> classInfo; \
    AutoPtr<IInterface> object; \
    ECode ec; \
    /*FAIL_GOTO(ec = CActivityThread::sSystemContext->GetClassLoader((IClassLoader**)&classLoader), label)*/ \
    FAIL_GOTO(ec = classLoader->LoadClass(className, (IClassInfo**)&classInfo), label) \
    FAIL_GOTO(ec = classInfo->CreateObject((PInterface*)&object), label) \
label: \
    if (FAILED(ec)) { \
        /*Slogger::E(TAG, "Can't load class %s", className.string()); */\
        *var = NULL; \
        return ec; \
    } \
    *var = (var_type*)var_type::Probe(object.Get()); \
    REFCOUNT_ADD(*var) \
} while(0);

CAR_INTERFACE_IMPL(CPack200, Singleton, IPack200)

CAR_SINGLETON_IMPL(CPack200)

ECode CPack200::NewPacker(
    /* [out] */ IPacker** packer)
{
    VALIDATE_NOT_NULL(packer)
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    String className;
    system->GetProperty(SYSTEM_PROPERTY_PACKER,
        String("org.apache.harmony.pack200.Pack200PackerAdapter"), &className);
    LOAD_CLASS(packer, IPacker)
    return NOERROR;
}

ECode CPack200::NewUnpacker(
    /* [out] */ IUnpacker** unpacker)
{
    VALIDATE_NOT_NULL(unpacker)
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    String className;
    system->GetProperty(SYSTEM_PROPERTY_UNPACKER,
        String("org.apache.harmony.unpack200.Pack200UnpackerAdapter"), &className);
    LOAD_CLASS(unpacker, IUnpacker)
    return NOERROR;
}

} // namespace Jar
} // namespace Utility
} // namespace Elastos

