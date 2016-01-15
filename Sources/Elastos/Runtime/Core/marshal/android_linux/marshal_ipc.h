
#ifndef __MARSHAL_H__
#define __MARSHAL_H__

#include <binder/Binder.h>
#include <elastos.h>
#include <marshal_comm.h>

namespace Elastos {
namespace IPC {

typedef struct InterfacePack
{
    EMuid               mClsid;            // clsid of object
    UInt32              mIndex;           // interface index in class
    android::sp<android::IBinder> mBinder;// binder object of stub
} InterfacePack;

extern ECode StdMarshalInterface(
    /* [in] */ IInterface* object,
    /* [out] */ InterfacePack* interfacePack);

extern ECode StdUnmarshalInterface(
    /* [in] */ UnmarshalFlag flag,
    /* [in] */ InterfacePack* interfacePack,
    /* [out] */ IInterface** object);

} // namespace IPC
} // namespace Elastos

#endif //__MARSHAL_H__
