
#ifndef __ROT_H__
#define __ROT_H__

#include "prxstub.h"
#include "syslinknode.h"

namespace Elastos {
namespace IPC {

typedef struct ExportObject: SLinkNode
{
    android::IBinder    *m_pBinder;
    IInterface          *m_pObject;
    IStub               *m_pIStub;
} ExportObject;

typedef struct ImportObject: SLinkNode
{
    android::IBinder    *m_pBinder;
    IProxy              *m_pIProxy;
} ImportObject;

extern ECode InitROT();

extern void UninitROT();

extern ECode RegisterExportObject(
    /* [in] */ android::IBinder *pBinder,
    /* [in] */ IInterface *pObject,
    /* [in] */ IStub *pIStub);

extern ECode UnregisterExportObject(
    /* [in] */ android::IBinder *pBinder);

extern ECode FindExportObject(
    /* [in] */ IInterface *pObject,
    /* [out] */ ExportObject *pExport);

extern ECode FindExportObject(
    /* [in] */ android::IBinder *pBinder,
    /* [out] */ ExportObject *pExport);

extern ECode RegisterImportObject(
    /* [in] */ android::IBinder *pBinder,
    /* [in] */ IProxy *pIProxy);

extern ECode UnregisterImportObject(
    /* [in] */ android::IBinder *pBinder);

extern ECode FindImportObject(
    /* [in] */ android::IBinder *pBinder,
    /* [out] */ ImportObject *pImport);

} // namespace IPC
} // namespace Elastos

#endif //__ROT_H__
