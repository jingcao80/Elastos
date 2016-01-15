
#include "elastos/droid/server/net/NetworkIdentitySet.h"
#include <elastos/droid/net/ReturnOutValue.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Net::CNetworkIdentity;

using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

const Int32 NetworkIdentitySet::VERSION_INIT = 1;
const Int32 NetworkIdentitySet::VERSION_ADD_ROAMING = 2;
const Int32 NetworkIdentitySet::VERSION_ADD_NETWORK_ID = 3;

ECode NetworkIdentitySet::constructor()
{
    return NOERROR;
}

ECode NetworkIdentitySet::constructor(
    /* [in] */ IDataInput* in)
{
    Int32 version;
    in->ReadInt32(&version);
    Int32 size;
    in->ReadInt32(&size);
    for (Int32 i = 0; i < size; i++) {
        if (version <= VERSION_INIT) {
            Int32 ignored;
            in->ReadInt32(&ignored);
        }
        Int32 type;
        in->ReadInt32(&type);
        Int32 subType;
        in->ReadInt32(&subType);
        String subscriberId = ReadOptionalString(in);
        String networkId;
        if (version >= VERSION_ADD_NETWORK_ID) {
            networkId = ReadOptionalString(in);
        }
        else {
            networkId = NULL;
        }
        Boolean roaming;
        if (version >= VERSION_ADD_ROAMING) {
            in->ReadBoolean(&roaming);
        }
        else {
            roaming = FALSE;
        }

        AutoPtr<INetworkIdentity> identity;
        CNetworkIdentity::New(type, subType, subscriberId, networkId, FALSE,
                                (INetworkIdentity**)&identity);
        Add(identity);
    }
    return NOERROR;
}

ECode NetworkIdentitySet::WriteToStream(
    /* [in] */ IDataOutput* out)
{
    VALIDATE_NOT_NULL(out);
    out->WriteInt32(VERSION_ADD_NETWORK_ID);
    Int32 size;
    GetSize(&size);
    out->WriteInt32(size);
    AutoPtr<IIterator> iter;
    GetIterator((IIterator**)&iter);
    while (Ptr(iter)->Func(iter->HasNext)) {
        AutoPtr<INetworkIdentity> ident = INetworkIdentity::Probe(Ptr(iter)->Func(iter->GetNext));
        Int32 type;
        ident->GetType(&type);
        out->WriteInt32(type);
        Int32 subtype;
        ident->GetSubType(&subtype);
        out->WriteInt32(subtype);
        String subscriberId;
        ident->GetSubscriberId(&subscriberId);
        WriteOptionalString(out, subscriberId);
        String networkId;
        ident->GetNetworkId(&networkId);
        WriteOptionalString(out, networkId);
        Boolean roaming;
        ident->GetRoaming(&roaming);
        out->WriteBoolean(roaming);
    }
    return NOERROR;
}

void NetworkIdentitySet::WriteOptionalString(
    /* [in] */ IDataOutput* out,
    /* [in] */ const String& value)
{
    if (!value.IsNull()) {
        out->WriteByte(1);
        out->WriteUTF(value);
    }
    else {
        out->WriteByte(0);
    }
}

String NetworkIdentitySet::ReadOptionalString(
    /* [in] */ IDataInput* in)
{
    Byte val;
    in->ReadByte(&val);
    if (val != 0) {
        String str;
        in->ReadUTF(&str);
        return str;
    }
    else {
        return String(NULL);
    }
}

} // namespace Net
} // namespace Server
} // namepsace Droid
} // namespace Elastos
