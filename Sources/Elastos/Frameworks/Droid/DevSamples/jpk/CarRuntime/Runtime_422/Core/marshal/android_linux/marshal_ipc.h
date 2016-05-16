
#ifndef __MARSHAL_H__
#define __MARSHAL_H__

#include <binder/Binder.h>
#include "clsinfo.h"
#include "elasys_server.h"
#include "marshal_comm.h"

namespace Elastos {
namespace IPC {

typedef struct InterfacePack
{
    EMuid               m_clsid;            // clsid of object
    UInt32              m_uIndex;           // interface index in class
    android::sp<android::IBinder> m_pBinder;// binder object of stub
}   InterfacePack;

extern ECode StdMarshalInterface(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfacePack *pInterfacePack);

extern ECode StdUnmarshalInterface(
    /* [in] */ UnmarshalFlag flag,
    /* [in] */ InterfacePack *pInterfacePack,
    /* [out] */ IInterface **ppObject);

} // namespace IPC
} // namespace Elastos

#endif //__MARSHAL_H__
