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

#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageCache.h"
#include <Elastos.Droid.Utility.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Utility::CFastImmutableArraySet;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Utility::ISet;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

HdmiCecMessageCache::HdmiCecMessageCache()
{
    CSparseArray::New((ISparseArray**)&mCache);
}

ECode HdmiCecMessageCache::constructor()
{
    return NOERROR;
}

ECode HdmiCecMessageCache::GetMessage(
    /* [in] */ Int32 address,
    /* [in] */ Int32 opcode,
    /* [out] */ IHdmiCecMessage** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mCache->Get(address, (IInterface**)&obj);
    AutoPtr<ISparseArray> messages = ISparseArray::Probe(obj);
    if (messages == NULL) {
        *result = NULL;
        return NOERROR;
    }
    obj = NULL;
    messages->Get(opcode, (IInterface**)&obj);
    *result = IHdmiCecMessage::Probe(obj);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiCecMessageCache::FlushMessagesFrom(
    /* [in] */ Int32 address)
{
    return mCache->Remove(address);
}

ECode HdmiCecMessageCache::FlushAll()
{
    return mCache->Clear();
}

ECode HdmiCecMessageCache::CacheMessage(
    /* [in] */ IHdmiCecMessage* message)
{
    Int32 opcode;
    message->GetOpcode(&opcode);
    if (!IsCacheable(opcode)) {
        return NOERROR;
    }
    Int32 source;
    message->GetSource(&source);
    AutoPtr<IInterface> obj;
    mCache->Get(source, (IInterface**)&obj);
    AutoPtr<ISparseArray> messages = ISparseArray::Probe(obj);
    if (messages == NULL) {
        CSparseArray::New((ISparseArray**)&messages);
        mCache->Put(source, messages);
    }
    messages->Put(opcode, message);
    return NOERROR;
}

Boolean HdmiCecMessageCache::IsCacheable(
    /* [in] */ Int32 opcode)
{
    AutoPtr<IInteger32> iOpcode;
    CInteger32::New(opcode, (IInteger32**)&iOpcode);
    Boolean rev;
    ISet::Probe(CACHEABLE_OPCODES)->Contains(iOpcode, &rev);
    return rev;
}

AutoPtr<IFastImmutableArraySet> HdmiCecMessageCache::InitCACHEABLE_OPCODES()
{
    AutoPtr<IFastImmutableArraySet> rev;
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(4);
    AutoPtr<IInteger32> i32;
    CInteger32::New(Constants::MESSAGE_SET_OSD_NAME, (IInteger32**)&i32);
    array->Set(0, i32);
    i32 = NULL;
    CInteger32::New(Constants::MESSAGE_REPORT_PHYSICAL_ADDRESS, (IInteger32**)&i32);
    array->Set(1, i32);
    i32 = NULL;
    CInteger32::New(Constants::MESSAGE_DEVICE_VENDOR_ID, (IInteger32**)&i32);
    array->Set(2, i32);
    i32 = NULL;
    CInteger32::New(Constants::MESSAGE_CEC_VERSION, (IInteger32**)&i32);
    array->Set(3, i32);
    CFastImmutableArraySet::New(array, (IFastImmutableArraySet**)&rev);
    return rev;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
