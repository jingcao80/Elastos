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

#include <Elastos.CoreLibrary.Net.h>
#include "_Elastos.Droid.Core.h"
#include "elastos/droid/net/CInterfaceConfiguration.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_OBJECT_IMPL(CInterfaceConfiguration)

CAR_INTERFACE_IMPL_2(CInterfaceConfiguration, Object, IParcelable, IInterfaceConfiguration)

const String CInterfaceConfiguration::FLAG_UP("up");
const String CInterfaceConfiguration::FLAG_DOWN("down");

CInterfaceConfiguration::CInterfaceConfiguration()
{
    //AutoPtr<ISets> tmp;
    // TODO: Waiting for Sets
    //assert(0);
    // CSets::AcquireSingleton((ISets**)&tmp);
    // tmp->NewHashSet((IHashSet**)&mFlags);
    CHashSet::New((IHashSet**)&mFlags);
}

ECode CInterfaceConfiguration::constructor()
{
    return NOERROR;
}

ECode CInterfaceConfiguration::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder builder;
    builder.Append("mHwAddr=");
    builder.Append(mHwAddr);
    builder.Append(" mAddr=");
    String mAddrS;
    IObject::Probe(mAddr)->ToString(&mAddrS);
    builder.Append(mAddrS);
    builder.Append(" mFlags=");
    AutoPtr<IIterable> flags;
    GetFlags((IIterable**)&flags);
    builder.Append(IInterface::Probe(flags));
    *result = builder.ToString();
    return NOERROR;
}

ECode CInterfaceConfiguration::GetFlags(
    /* [out] */ IIterable** result)
{
    VALIDATE_NOT_NULL(result);

    *result = IIterable::Probe(mFlags);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CInterfaceConfiguration::HasFlag(
    /* [in] */ const String& flag,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(ValidateFlag(flag));
    AutoPtr<ICharSequence> csq;
    CString::New(flag, (ICharSequence**)&csq);
    return mFlags->Contains(csq, result);
}

ECode CInterfaceConfiguration::ClearFlag(
    /* [in] */ const String& flag)
{
    FAIL_RETURN(ValidateFlag(flag));
    AutoPtr<ICharSequence> csq;
    CString::New(flag, (ICharSequence**)&csq);
    mFlags->Remove(csq);
    return NOERROR;
}

ECode CInterfaceConfiguration::SetFlag(
    /* [in] */ const String& flag)
{
    FAIL_RETURN(ValidateFlag(flag));
    AutoPtr<ICharSequence> csq;
    CString::New(flag, (ICharSequence**)&csq);
    mFlags->Add(csq);
    return NOERROR;
}

ECode CInterfaceConfiguration::SetInterfaceUp()
{
    AutoPtr<ICharSequence> csq;
    CString::New(FLAG_DOWN, (ICharSequence**)&csq);
    mFlags->Remove(csq);
    csq = NULL;
    CString::New(FLAG_UP, (ICharSequence**)&csq);
    mFlags->Add(csq);
    return NOERROR;
}

ECode CInterfaceConfiguration::SetInterfaceDown()
{
    AutoPtr<ICharSequence> csq;
    CString::New(FLAG_UP, (ICharSequence**)&csq);
    mFlags->Remove(csq);
    csq = NULL;
    CString::New(FLAG_DOWN, (ICharSequence**)&csq);
    mFlags->Add(csq);
    return NOERROR;
}

ECode CInterfaceConfiguration::GetLinkAddress(
    /* [out] */ ILinkAddress** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAddr;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CInterfaceConfiguration::SetLinkAddress(
    /* [in] */ ILinkAddress* addr)
{
    mAddr = addr;
    return NOERROR;
}

ECode CInterfaceConfiguration::GetHardwareAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mHwAddr;
    return NOERROR;
}

ECode CInterfaceConfiguration::SetHardwareAddress(
    /* [in] */ const String& hwAddr)
{
    mHwAddr = hwAddr;
    return NOERROR;
}

ECode CInterfaceConfiguration::IsActive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
    Boolean hasFlag;
    if (HasFlag(FLAG_UP, &hasFlag), hasFlag) {
        AutoPtr<IInetAddress> addr;
        mAddr->GetAddress((IInetAddress**)&addr);
        AutoPtr< ArrayOf<Byte> > byteArray;
        addr->GetAddress((ArrayOf<Byte>**)&byteArray);
        for (Int32 i = 0; i < byteArray->GetLength(); i++) {
            Byte b = (*byteArray)[i];
            if (b != 0) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
    // } catch (NullPointerException e) {
    //     return false;
    // }
}

ECode CInterfaceConfiguration::ReadFromParcel(
    /* [in] */ IParcel* src)
{
    src->ReadString(&mHwAddr);
    Byte bv;
    src->ReadByte(&bv);
    if (bv == 1) {
        AutoPtr<IInterface> obj;
        src->ReadInterfacePtr((Handle32*)&obj);
        mAddr = ILinkAddress::Probe(obj);
    }
    Int32 size;
    src->ReadInt32(&size);
    for (Int32 i = 0; i < size; i++) {
        String flag;
        src->ReadString(&flag);
        AutoPtr<ICharSequence> csq;
        CString::New(flag, (ICharSequence**)&csq);
        mFlags->Add(csq);
    }
    return NOERROR;
}

ECode CInterfaceConfiguration::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mHwAddr);
    if (mAddr != NULL) {
        dest->WriteByte(1);
        dest->WriteInterfacePtr(mAddr.Get());
    }
    else {
        dest->WriteByte(0);
    }
    Int32 size;
    mFlags->GetSize(&size);
    dest->WriteInt32(size);
    AutoPtr<IIterator> iter;
    mFlags->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> iCur;
        iter->GetNext((IInterface**)&iCur);
        String s;
        ICharSequence::Probe(iCur)->ToString(&s);
        dest->WriteString(s);
    }
    return NOERROR;
}

ECode CInterfaceConfiguration::ValidateFlag(
        /* [in] */ const String& flag)
{
    if (flag.IndexOf(' ') >= 0) {
        Logger::E("InterfaceConfiguration", "flag contains space: %s", flag.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
