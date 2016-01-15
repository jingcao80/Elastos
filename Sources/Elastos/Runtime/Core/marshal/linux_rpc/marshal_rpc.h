
#ifndef __MARSHAL_RPC_H__
#define __MARSHAL_RPC_H__

#include <elastos.h>
#include <marshal_comm.h>

namespace Elastos {
namespace RPC {

typedef struct InterfacePack
{
    EMuid               mClsid;           // clsid of object
    UInt32              mIndex;           // interface index in class
    String              mNetAddress;      // net address of stub
}   InterfacePack;

extern ECode StdMarshalInterface(
    /* [in] */ IInterface* object,
    /* [out] */ InterfacePack* interfacePack);

extern ECode StdUnmarshalInterface(
    /* [in] */ UnmarshalFlag flag,
    /* [in] */ InterfacePack* interfacePack,
    /* [out] */ IInterface** object);

} // namespace RPC
} // namespace Elastos

#endif //__MARSHAL_RPC_H__
