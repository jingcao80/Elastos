
#ifndef __ROT_H__
#define __ROT_H__

#include <prxstub.h>
#include <elcontainer.h>

namespace Elastos {
namespace IPC {

typedef struct ExportObject: SLinkNode
{
    android::IBinder*   mBinder;
    IInterface*         mObject;
    IStub*              mIStub;
} ExportObject;

typedef struct ImportObject: SLinkNode
{
    android::IBinder*   mBinder;
    IProxy*             mIProxy;
} ImportObject;

extern ECode InitROT();

extern void UninitROT();

extern ECode RegisterExportObject(
    /* [in] */ android::IBinder* binder,
    /* [in] */ IInterface* object,
    /* [in] */ IStub* stub);

extern ECode UnregisterExportObject(
    /* [in] */ android::IBinder* binder);

extern ECode FindExportObject(
    /* [in] */ IInterface* object,
    /* [out] */ ExportObject* expObj);

extern ECode FindExportObject(
    /* [in] */ android::IBinder* binder,
    /* [out] */ ExportObject* expObj);

extern ECode RegisterImportObject(
    /* [in] */ android::IBinder* binder,
    /* [in] */ IProxy* proxy);

extern ECode UnregisterImportObject(
    /* [in] */ android::IBinder* binder);

extern ECode FindImportObject(
    /* [in] */ android::IBinder* binder,
    /* [out] */ ImportObject* impObj);

} // namespace IPC
} // namespace Elastos

#endif //__ROT_H__
