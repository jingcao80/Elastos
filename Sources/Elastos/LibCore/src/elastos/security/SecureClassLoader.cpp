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

#include "SecureClassLoader.h"
#include "CPermissions.h"
#include "AutoLock.h"
#include "CHashMap.h"

using Elastos::Core::AutoLock;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Security {

SecureClassLoader::SecureClassLoader()
{
    CHashMap::New((IHashMap**)&mPds);
}

ECode SecureClassLoader::constructor(
    /* [in] */ IClassLoader* parent)
{
    assert(0 && "TODO");
    // return ClassLoader::constructor(parent);
    return NOERROR;
}

AutoPtr<IPermissionCollection> SecureClassLoader::GetPermissions(
    /* [in] */ ICodeSource* codesource)
{
    // Do nothing by default, ProtectionDomain will take care about
    // permissions in dynamic
    AutoPtr<IPermissionCollection> pc;
    CPermissions::New((IPermissionCollection**)&pc);
    return pc;
}

ECode SecureClassLoader::DefineClass(
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len,
    /* [in] */ ICodeSource* cs,
    /* [out] */ IClassInfo** result)
{
    VALIDATE_NOT_NULL(result);
    // return cs == NULL ? DefineClass(name, b, off, len) : DefineClass(name,
    //         b, off, len, GetPD(cs));
    assert(0 && "TODO");
    return NOERROR;
}

ECode SecureClassLoader::DefineClass(
    /* [in] */ const String& name,
    /* [in] */ IByteBuffer* b,
    /* [in] */ ICodeSource* cs,
    /* [out] */ IClassInfo** result)
{
    VALIDATE_NOT_NULL(result);
    //FIXME 1.5 - remove b.array(), call super.defineClass(,ByteBuffer,)
    // directly

    // AutoPtr<ArrayOf<Byte> > data = b->Array();
    // return cs == NULL ? DefineClass(name, data, 0, data.length)
    //         : DefineClass(name, data, 0, data.length, GetPD(cs));
    assert(0 && "TODO");
    return NOERROR;
}

AutoPtr<ProtectionDomain> SecureClassLoader::GetPD(
    /* [in] */ ICodeSource* cs)
{
    if (cs == NULL) {
        return NULL;
    }
    // need to cache PDs, otherwise every class from a given CodeSource
    // will have it's own ProtectionDomain, which does not look right.
    AutoPtr<ProtectionDomain> pd;
    {
        AutoLock lock(PDS);
        AutoPtr<IInterface> obj;
        mPds->Get(cs, (IInterface**)&obj);
        if ((pd = (ProtectionDomain*)IProtectionDomain::Probe(obj)) != NULL) {
            return pd;
        }
        AutoPtr<IPermissionCollection> perms = GetPermissions(cs);
        pd = new ProtectionDomain(cs, perms, this, NULL);
        mPds->Put(cs, pd->Probe(EIID_IInterface));
    }
    return pd;
}

} // namespace Security
} // namespace Elastos
