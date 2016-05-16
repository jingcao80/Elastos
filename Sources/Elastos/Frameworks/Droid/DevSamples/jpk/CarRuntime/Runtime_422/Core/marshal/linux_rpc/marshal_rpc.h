
#ifndef __MARSHAL_RPC_H__
#define __MARSHAL_RPC_H__

#include "clsinfo.h"
#include "elasys_server.h"
#include "marshal_comm.h"

namespace Elastos {
namespace RPC {

typedef struct InterfacePack
{
    EMuid               m_clsid;            // clsid of object
    UInt32              m_uIndex;           // interface index in class
    String              m_sNetAddress;      // net address of stub
}   InterfacePack;

extern ECode StdMarshalInterface(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfacePack *pInterfacePack);

extern ECode StdUnmarshalInterface(
    /* [in] */ UnmarshalFlag flag,
    /* [in] */ InterfacePack *pInterfacePack,
    /* [out] */ IInterface **ppObject);

} // namespace RPC
} // namespace Elastos

#endif //__MARSHAL_RPC_H__
