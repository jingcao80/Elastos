
#ifndef __MARSHAL_H__
#define __MARSHAL_H__

#include "clsinfo.h"
#include "elasys_server.h"

_ELASTOS_NAMESPACE_USING

#ifndef INT_MAX
#define INT_MAX 0x7fffffff
#endif

#define STUB_OBJECT_DBUS_OBJECT_PATH    "/Elastos/IPC/CObjectStub"
#define STUB_OBJECT_DBUS_INTERFACE      "Elatos.IPC.IStub"

#define MSHDBG_NORMAL           1
#define MSHDBG_CREF             2
#define MSHDBG_WARNING          3
#define MSHDBG_ERROR            4
#define MSHDBG_NONE             100

#define MSHDBG_LEVEL            MSHDBG_ERROR

#if defined(_DEBUG) || defined(_MARSHAL_DEBUG)
#define MARSHAL_DBGOUT(level, exp) \
    ((level) >= MSHDBG_LEVEL ? (void)(exp) : (void)0)
extern int _DumpGUID(REIID riid);
extern int _DumpCLSID(RClassID rclsid);
extern int _DumpBin(const void *pv, int size);

#define DUMP_GUID(riid)         _DumpGUID(riid)
#define DUMP_CLSID(rclsid)      _DumpCLSID(rclsid)
#else
#define MARSHAL_DBGOUT(level, exp)
#define DUMP_GUID(riid)
#define DUMP_CLSID(rclsid)
#endif // _MARSHAL_DEBUG

#define MARSHAL_MAGIC           0x7a79636f

#define MSH_MAX_METHODS         (192 + 4)

#define MSH_ALIGN_4(n)          RoundUp2((n), 4)

#define MSH_NULL                ((UInt32)0)
#define MSH_NOT_NULL            ((UInt32)-2)

typedef struct MarshalHeader
{
    UInt32              m_uMagic;           // magic number
    Int16               m_hInterfaceIndex;  // interface index in object
    Int16               m_hMethodIndex;     // method index in interface
    UInt32              m_uInSize;          // size of in data
    UInt32              m_uOutSize;         // size of out data
}   MarshalHeader;

typedef struct InterfacePack
{
    EMuid               m_clsid;            // clsid of object
    UInt32              m_uIndex;           // interface index in class
    Char8               m_stubConnName[256];// dbus unique connection name of stub
}   InterfacePack;

extern ECode StdMarshalInterface(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfacePack *pInterfacePack);

extern ECode StdUnmarshalInterface(
        /* [in] */ UnmarshalFlag flag,
        /* [in] */ InterfacePack *pInterfacePack,
        /* [out] */ IInterface **ppObject);

extern ECode Proxy_ProcessMsh_BufferSize(
        /* [in] */ const CIMethodInfo *pMethodInfo,
        /* [in] */ UInt32 *puArgs,
        /* [out] */ UInt32 *puInSize,
        /* [out] */ UInt32 *puOutSize);

extern ECode Proxy_ProcessMsh_In(
        /* [in] */ const CIMethodInfo *pMethodInfo,
        /* [in] */ UInt32 *puArgs,
        /* [in, out] */ IParcel *pParcel);

extern ECode Proxy_ProcessUnmsh_Out(
        /* [in] */ const CIMethodInfo *pMethodInfo,
        /* [in] */ IParcel *pParcel,
        /* [in] */ UInt32 uDataSize,
        /* [in, out] */ UInt32 *puArgs);

extern ECode Stub_ProcessUnmsh_In(
        /* [in] */ const CIMethodInfo *pMethodInfo,
        /* [in] */ IParcel *pParcel,
        /* [in, out] */ UInt32 *puOutBuffer,
        /* [in, out] */ UInt32 *puArgs);

extern ECode Stub_ProcessMsh_Out(
        /* [in] */ const CIMethodInfo *pMethodInfo,
        /* [in] */ UInt32 *puArgs,
        /* [in] */ UInt32 *puOutBuffer,
        /* [in] */ Boolean bOnlyReleaseIn,
        /* [in, out] */ IParcel* pParcel);

#endif //__MARSHAL_H__
