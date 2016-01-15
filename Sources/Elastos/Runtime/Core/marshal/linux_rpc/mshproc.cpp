
#include <stdio.h>
#include <marshal_rpc.h>

#define ROUND8(n)       (((n)+7)&~7)   // round up to multiple of 8 bytes

namespace Elastos {
namespace RPC {

ECode Proxy_ProcessMsh_BufferSize(
    /* [in] */ const CIMethodInfo* methodInfo,
    /* [in] */ UInt32* args,
    /* [out] */ UInt32* retInSize,
    /* [out] */ UInt32* retOutSize)
{
    UInt32 size = 0;
    UInt32 inSize = 0;
    UInt32 outSize = 0;
    Int32 paramNum = methodInfo->mParamNum;
    const CIBaseType* params = methodInfo->mParams;

    for (Int32 n = 0; n < paramNum; n++) {
        if (BT_IS_OUT(params[n])) {    // [out]
            outSize += sizeof(UInt32); // for pointer

            switch (BT_TYPE(params[n])) {
                case BT_TYPE_PUINT8:
                case BT_TYPE_PUINT16:
                case BT_TYPE_PUINT32:
                    size = sizeof(UInt32);
                    break;

                case BT_TYPE_PUINT64:
                    size = sizeof(UInt64);
                    break;

                case BT_TYPE_PEMUID:
                    size = sizeof(EMuid);
                    break;

                case BT_TYPE_PEGUID:
                    size = sizeof(EMuid) + sizeof(char*) + 80 * sizeof(char);
                    break;

                case BT_TYPE_PSTRUCT :
                    size = sizeof(UInt32) * BT_TYPE_SIZE(params[n]);
                    break;

                case BT_TYPE_PSTRING:
                    size = sizeof(String);
                    break;

                case BT_TYPE_STRINGBUF:
                case BT_TYPE_BUFFEROF:
                case BT_TYPE_ARRAYOF:{
                    size = sizeof(PCARQUINTET);
                    break;
                }

                case BT_TYPE_PINTERFACE:
                    size = sizeof(Elastos::RPC::InterfacePack);
                    break;

                default:
                    MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                            "MshProc: Invalid [in, out] type(%08x), param index: %d.\n",
                            params[n], n));
                    assert(0);
                    return E_INVALID_ARGUMENT;
            }

            if (*args) {
                outSize += size;
            }
            // else: null pointer do not contain data.
        }

        if (BT_IS_IN(params[n])) { // [in]
            switch (BT_TYPE(params[n])) {
                case BT_TYPE_UINT8:
                case BT_TYPE_UINT16:
                case BT_TYPE_UINT32:
                    inSize += sizeof(UInt32);
                    args++;
                    break;

                case BT_TYPE_PUINT8:
                case BT_TYPE_PUINT16:
                case BT_TYPE_PUINT32:
                    if (*args) {
                        inSize += sizeof(UInt32) * 2;
                    }
                    else { // null pointer
                        inSize += sizeof(UInt32);
                    }
                    args++;
                    break;

                case BT_TYPE_UINT64:
#ifdef _mips
                    // Adjust for 64bits align on mips
                    if (!(n % 2)) {
                        inSize += 4;
                        args += 1;
                    }
#endif
#if defined(_arm) && defined(__GNUC__) && (__GNUC__ >= 4)
                    args = (UInt32*)ROUND8((Int32)args);
#endif
                    inSize += sizeof(UInt64) + 1; //+1 for 8-byte alignment
                    args += 2;
                    break;

                case BT_TYPE_PUINT64:
                    if (*args) {
                        inSize += sizeof(UInt32) + sizeof(UInt64);
                    }
                    else { // null pointer
                        inSize += sizeof(UInt32);
                    }
                    args++;
                    break;

                case BT_TYPE_EGUID:
                    inSize += sizeof(EMuid) + sizeof(char*) \
                              + 80 * sizeof(char);
                    args += sizeof(EMuid) / 4 + sizeof(char*) / 4;
                    break;

                case BT_TYPE_EMUID:
                    inSize += sizeof(EMuid);
                    args += sizeof(EMuid) / 4;
                    break;

                case BT_TYPE_PSTRUCT:
                case BT_TYPE_STRUCT:
                    if (*args) {
                        inSize += sizeof(UInt32) + sizeof(UInt32) * BT_TYPE_SIZE(params[n]);
                    }
                    else {  // null pointer
                        inSize += sizeof(UInt32);
                    }
                    args++;
                    break;

                case BT_TYPE_PEGUID:
                    if (*args) {
                        inSize += sizeof(UInt32) + sizeof(EMuid)
                                + 80 * sizeof(char)
                                + sizeof(wchar_t*);
                    }
                    else {  // null pointer
                        inSize += sizeof(UInt32);
                    }
                    args++;
                    break;

                case BT_TYPE_PEMUID:
                    if (*args) {
                        inSize += sizeof(UInt32) + sizeof(EMuid);
                    }
                    else {  // null pointer
                        inSize += sizeof(UInt32);
                    }
                    args++;
                    break;

                case BT_TYPE_STRING:
                    if (*args) {
                        UInt32 nBufLen = (strlen((char *)(*args)) + 1)
                                * sizeof(char);
                        inSize += sizeof(UInt32) + sizeof(UInt32) +
                                MSH_ALIGN_4(nBufLen);
                    }
                    else {
                        inSize += sizeof(UInt32);
                    }
                    args++;
                    break;

                case BT_TYPE_INTERFACE:
                    if (*args) {
                        inSize += sizeof(Elastos::RPC::InterfacePack);
                    }
                    else {  // null pointer
                        inSize += sizeof(UInt32);
                    }
                    args++;
                    break;

                case BT_TYPE_PINTERFACE:
                    if (*args) {
                        if (*(UInt32 *)(*args)) {
                            inSize += sizeof(Elastos::RPC::InterfacePack);
                        }
                        else {
                            inSize += sizeof(UInt32);
                        }
                    }
                    inSize += sizeof(UInt32);  // for pointer
                    args++;
                    break;

                case BT_TYPE_STRINGBUF:
                case BT_TYPE_BUFFEROF:
                case BT_TYPE_ARRAYOF:
                    if (*args) {
                        if (CarQuintetFlag_Type_IObject
                            != (((PCARQUINTET)*args)->mFlags
                                    & CarQuintetFlag_TypeMask)) {
                            inSize += MSH_ALIGN_4(sizeof(UInt32)
                                    + sizeof(CarQuintet)
                                    + ((PCARQUINTET)*args)->mSize);
                        }
                        else {
                            inSize += MSH_ALIGN_4(sizeof(UInt32) + sizeof(CarQuintet));
                            Int32 used = ((PCARQUINTET)*args)->mUsed /
                                    sizeof(IInterface *);
                            Int32* int32Buf = (Int32*)((PCARQUINTET)*args)->mBuf;
                            uint_t usedSize = 0;
                            for (Int32 i = 0; i < used; i++) {
                                if (int32Buf[i]) {
                                    usedSize += sizeof(Elastos::RPC::InterfacePack);
                                }
                                else {  // null pointer
                                    usedSize += sizeof(UInt32);
                                }
                            }
                            inSize += MAX((MemorySize)usedSize, \
                                    MSH_ALIGN_4(((PCARQUINTET)*args)->mSize));
                        }
                    }
                    else {  // null pointer
                        inSize += sizeof(UInt32);
                    }
                    args++;
                    break;

                default:
                    MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                            "MshProc: Invalid [in, out] type(%08x), param index: %d.\n",
                            params[n], n));
                    assert(0);
                    return E_INVALID_ARGUMENT;
            }
        }
        else {  // [out]
            if (((BT_TYPE(params[n]) == BT_TYPE_BUFFEROF) ||
                (BT_TYPE(params[n]) == BT_TYPE_ARRAYOF) ||
                (BT_TYPE(params[n]) == BT_TYPE_STRINGBUF)) && *args) {
                inSize += sizeof(UInt32); // for size only
            }
            args++;
            inSize += sizeof(UInt32);
        }
    }

    *retInSize = inSize;
    *retOutSize = outSize;
    return NOERROR;
}

ECode Proxy_ProcessMsh_In(
    /* [in] */ const CIMethodInfo* methodInfo,
    /* [in] */ UInt32* args,
    /* [in, out] */ IParcel* parcel)
{
    Int32 paramNum = methodInfo->mParamNum;
    const CIBaseType* params = methodInfo->mParams;

    for (Int32 n = 0; n < paramNum; n++) {
        if (BT_IS_IN(params[n])) { // [in] or [in, out]
            switch (BT_TYPE(params[n])) {
                case BT_TYPE_UINT8:
                case BT_TYPE_UINT16:
                case BT_TYPE_UINT32:
                    parcel->WriteInt32((Int32)*args);
                    args++;
                    break;

                case BT_TYPE_UINT64:
#ifdef _mips
                    // Adjust for 64bits align on mips
                    if (!(n % 2)) args += 1;
#endif
#if defined(_arm) && defined(__GNUC__) && (__GNUC__ >= 4)
                    args = (UInt32*)ROUND8((Int32)args);
#endif
                    parcel->WriteInt64((Int64)(*(UInt64 *)args));
                    args += 2;
                    break;

                case BT_TYPE_PUINT8:
                case BT_TYPE_PUINT16:
                case BT_TYPE_PUINT32:
                case BT_TYPE_PUINT64:
                    assert(0);
                    args++;
                    break;

                case BT_TYPE_STRUCT:
                case BT_TYPE_PSTRUCT:
                    parcel->WriteStruct(*args, BT_TYPE_SIZE(params[n]) * 4);
                    args++;
                    break;

                case BT_TYPE_EMUID:
                    parcel->WriteEMuid(*(EMuid *)args);
                    args += sizeof(EMuid) / 4;
                    break;

                case BT_TYPE_EGUID:
                    parcel->WriteEGuid(*(EGuid *)args);
                    args += sizeof(EGuid) / 4;
                    break;

                case BT_TYPE_PEMUID:
                case BT_TYPE_PEGUID:
                case BT_TYPE_STRINGBUF:
                case BT_TYPE_BUFFEROF:
                    assert(0);
                    args++;
                    break;

                case BT_TYPE_ARRAYOF:
                    parcel->WriteArrayOf((Handle32)*args);
                    args++;
                    break;

                case BT_TYPE_STRING:
                    parcel->WriteString(**(String**)args);
                    args++;
                    break;

                case BT_TYPE_INTERFACE: {
                    ECode ec = parcel->WriteInterfacePtr((IInterface *)*args);
                    if (FAILED(ec)) {
                        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                                "MshProc: marshal interface, ec = %x, param index: %d\n", ec, n));
                        return ec;
                    }
                    args++;
                    break;
                }

                case BT_TYPE_PINTERFACE:
                    assert(0);
                    args++;
                    break;

                default:
                    MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                        "MshProc: Invalid [in, out] type(%08x), param index: %d.\n",
                        params[n], n));
                    assert(0);
                    return E_INVALID_ARGUMENT;
            }
        }
        else {  // [out]
            if (*args) parcel->WriteInt32(MSH_NOT_NULL);
            else parcel->WriteInt32(MSH_NULL);

            if (((BT_TYPE(params[n]) == BT_TYPE_BUFFEROF) ||
                (BT_TYPE(params[n]) == BT_TYPE_ARRAYOF) ||
                (BT_TYPE(params[n]) == BT_TYPE_STRINGBUF)) && !BT_IS_CALLEE(params[n]) && *args) {
                    parcel->WriteInt32(((PCARQUINTET)*args)->mSize);
            }
            args++;
        }
    }

    return NOERROR;
}

ECode Proxy_ProcessUnmsh_Out(
    /* [in] */ const CIMethodInfo* methodInfo,
    /* [in] */ IParcel* parcel,
    /* [in] */ UInt32 dataSize,
    /* [in, out] */ UInt32* args)
{
    Int32 paramNum = methodInfo->mParamNum;
    const CIBaseType* params = methodInfo->mParams;

    for (Int32 n = 0; n < paramNum; n++) {
        if (BT_IS_OUT(params[n])) {   // [out] or [in, out]
            if (*args) {
                Int32 pointValue;
                parcel->ReadInt32(&pointValue);
                if (pointValue != (Int32)MSH_NOT_NULL) {
                    MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                            "MshProc: param conflict in proxy's unmarshal, param index: %d.\n", n));
                    return E_INVALID_ARGUMENT;
                }

                switch (BT_TYPE(params[n])) {
                    case BT_TYPE_PUINT8:
                        parcel->ReadByte((Byte*)*args);
                        break;

                    case BT_TYPE_PUINT16:
                        parcel->ReadInt16((Int16*)*args);
                        break;

                    case BT_TYPE_PUINT32:
                        parcel->ReadInt32((Int32*)*args);
                        break;

                    case BT_TYPE_PUINT64:
                        parcel->ReadInt64((Int64*)*args);
                        break;

                    case BT_TYPE_PSTRUCT:
                        parcel->ReadStruct((Handle32*)*args);
                        break;

                    case BT_TYPE_PEMUID:
                        parcel->ReadEMuid((EMuid*)*args);
                        break;

                    case BT_TYPE_PEGUID:
                        parcel->ReadEGuid((EGuid*)*args);
                        break;

                    case BT_TYPE_PSTRING:
                        parcel->ReadString((String*)*args);
                        break;

                    case BT_TYPE_STRINGBUF:
                    case BT_TYPE_BUFFEROF:
                    case BT_TYPE_ARRAYOF:
                        if (!BT_IS_CALLEE(params[n])) {
                            PCARQUINTET p;
                            parcel->ReadArrayOf((Handle32*)&p);
                            PCARQUINTET qArg = (PCARQUINTET)*args;
                            qArg->mUsed = p->mUsed;
                            memcpy(qArg->mBuf, p->mBuf, p->mSize);
                            _CarQuintet_Release(p);
                        }
                        else {
                            parcel->ReadArrayOf((Handle32*)*args);
                        }
                        break;

                    case BT_TYPE_PINTERFACE:
                        parcel->ReadInterfacePtr((Handle32*)*args);
                        break;

                    default:
                        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                                "MshProc: Invalid param type(%08x), param index: %d\n", params[n], n));
                        assert(0);
                        return E_INVALID_ARGUMENT;
                }
            }
            else {
                Int32 pointValue;
                parcel->ReadInt32(&pointValue);
                if (pointValue != MSH_NULL) {
                    MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                            "MshProc: param conflict in proxy's unmarshal, param index: %d.\n", n));
                    return E_INVALID_ARGUMENT;
                }
            }
            args++;
        }
        else {      // [in]
            switch (BT_TYPE(params[n])) {
                case BT_TYPE_UINT64:
#ifdef _mips
                    // Adjust for 64bits align on mips
                    if (!(n % 2)) {
                        args += 1;
                    }
#endif
#if defined(_arm) && defined(__GNUC__) && (__GNUC__ >= 4)
                    args = (UInt32*)ROUND8((Int32)args);
#endif
                    args += 2;
                    break;

                case BT_TYPE_EMUID:
                    args += sizeof(EMuid) / 4;
                    break;

                case BT_TYPE_EGUID:
                    args += sizeof(EMuid) / 4 + sizeof(char*) /4;
                    break;

                default:
                    args++;
                    break;
            }
        }
    }

    return NOERROR;
}

ECode Stub_ProcessUnmsh_In(
    /* [in] */ const CIMethodInfo* methodInfo,
    /* [in] */ IParcel* parcel,
    /* [in, out] */ UInt32* outBuffer,
    /* [in, out] */ UInt32* args)
{
    if (outBuffer) {
        outBuffer = (UInt32 *)((MarshalHeader *)outBuffer + 1);
    }
    Int32 paramNum = methodInfo->mParamNum;
    const CIBaseType* params = methodInfo->mParams;

    for (Int32 n = 0; n < paramNum; n++) {
        if (BT_IS_OUT(params[n])) {    // [out] or [in, out]
            assert(outBuffer);
            Int32 pointValue;
            parcel->ReadInt32(&pointValue);
            if (pointValue == (Int32)MSH_NOT_NULL) {
                *outBuffer = MSH_NOT_NULL;
                outBuffer++;

                switch (BT_TYPE(params[n])) { // [out]
                    case BT_TYPE_PUINT8:
                    case BT_TYPE_PUINT16:
                    case BT_TYPE_PUINT32:
                        *args = (UInt32)outBuffer;
                        outBuffer++;
                        break;

                    case BT_TYPE_PUINT64:
                        *args = (UInt32)outBuffer;
                        outBuffer += 2;
                        break;

                    case BT_TYPE_PEGUID:
                        *args = (UInt32)outBuffer;
                        outBuffer += sizeof(EMuid) / 4;
                        *outBuffer = (UInt32)(outBuffer + 1);
                        outBuffer += sizeof(char*) / 4
                                + 80 * sizeof(char) / 4;
                        break;

                    case BT_TYPE_PEMUID:
                        *args = (UInt32)outBuffer;
                        outBuffer += sizeof(EMuid) / 4;
                        break;

                    case BT_TYPE_PSTRUCT:
                        *args = (UInt32)outBuffer;
                        outBuffer += BT_TYPE_SIZE(params[n]);
                        break;

                    case BT_TYPE_PSTRING:
                        *args = (UInt32)outBuffer;
                        new((void*)outBuffer) String();
                        outBuffer += sizeof(String) / 4;
                        break;

                    case BT_TYPE_STRINGBUF:
                    case BT_TYPE_BUFFEROF:
                    case BT_TYPE_ARRAYOF:
                        if (!BT_IS_CALLEE(params[n])) {
                            Int32 size;
                            parcel->ReadInt32(&size);
                            *outBuffer = (UInt32)malloc(sizeof(CarQuintet));
                            *(PCARQUINTET*)args = (PCARQUINTET)*outBuffer;
                            ((PCARQUINTET)*args)->mSize = size;
                            ((PCARQUINTET)*args)->mBuf = malloc(size);
                        }
                        else {
                            *outBuffer = 0;
                            *args = (UInt32)outBuffer;
                        }
                        outBuffer++;
                        break;

                    case BT_TYPE_PINTERFACE:
                        *outBuffer = 0;
                        *args = (UInt32)outBuffer;
                        outBuffer += sizeof(Elastos::RPC::InterfacePack) / 4;
                        break;

                    default:
                        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                                "MshProc: Invalid param type(%08x) in UnMsh_in, param index: %d.\n",
                                params[n], n));
                        return E_INVALID_ARGUMENT;
                }
            }
            else if (pointValue == MSH_NULL) {
                *outBuffer = MSH_NULL;
                outBuffer++;
                *args = 0;
            }
            else {
                MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                        "MshProc: Invalid pointer value in unmarshal in, param index: %d.\n", n));
                return E_INVALID_ARGUMENT;
            }
            args++;
        }
        else if (BT_IS_IN(params[n])) {    // [in]
            switch (BT_TYPE(params[n])) {
                case BT_TYPE_UINT8:
                case BT_TYPE_UINT16:
                case BT_TYPE_UINT32:
                    parcel->ReadInt32((Int32*)args);
                    args++;
                    break;

                case BT_TYPE_UINT64:
#ifdef _mips
                    // Adjust for 64bits align on mips
                    if (!(n % 2)) {
                        args += 1;
                    }
#endif
#if defined(_arm) && defined(__GNUC__) && (__GNUC__ >= 4)
                    args = (UInt32*)ROUND8((Int32)args);
#endif
                    parcel->ReadInt64((Int64*)args);
                    args += 2;
                    break;

                case BT_TYPE_PUINT8:
                case BT_TYPE_PUINT16:
                case BT_TYPE_PUINT32:
                case BT_TYPE_PUINT64:
                    assert(0);
                    args++;
                    break;

                case BT_TYPE_EMUID:
                    parcel->ReadEMuid((EMuid*)args);
                    args += sizeof(EMuid) / 4;
                    break;

                case BT_TYPE_EGUID:
                    parcel->ReadEGuid((EGuid*)args);
                    args += (sizeof(EGuid) +
                            MSH_ALIGN_4(strlen(((EGuid*)args)->mUunm) + 1)) / 4;
                    break;

                case BT_TYPE_PEMUID:
                case BT_TYPE_PEGUID:
                case BT_TYPE_STRUCT:
                case BT_TYPE_PSTRUCT:
                    assert(0);
                    args++;
                    break;

                case BT_TYPE_STRINGBUF:
                case BT_TYPE_BUFFEROF:
                case BT_TYPE_ARRAYOF:
                    parcel->ReadArrayOf((Handle32*)args);
                    args++;
                    break;

                case BT_TYPE_STRING: {
                    String str;
                    parcel->ReadString(&str);
                    *(String**)args = new String(str);
                    args++;
                    break;
                }

                case BT_TYPE_INTERFACE: {
                    ECode ec = parcel->ReadInterfacePtr((Handle32*)args);
                    if (FAILED(ec)) {
                        MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                                "MshProc: unmsh interface, ec = %x\n", ec));
                        return ec;
                    }
                    args++;
                    break;
                }

                case BT_TYPE_PINTERFACE:
                    assert(0);
                    args++;
                    break;

                default:
                    MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                            "MshProc: Invalid param type(%08x), param index: %d\n", params[n], n));
                    return E_INVALID_ARGUMENT;
            }
        }
        else {
            MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                    "MshProc: Invalid param type(%08x), param index: %d\n", params[n], n));
            return E_INVALID_ARGUMENT;
        }
    }

    return NOERROR;
}

ECode Stub_ProcessMsh_Out(
    /* [in] */ const CIMethodInfo* methodInfo,
    /* [in] */ UInt32* args,
    /* [in] */ UInt32* outBuffer,
    /* [in] */ Boolean onlyReleaseIn,
    /* [in, out] */ IParcel* parcel)
{
    Int32 paramNum = methodInfo->mParamNum;
    const CIBaseType* params = methodInfo->mParams;

    // skip the Out Marshal Header;
    if (outBuffer) {
        outBuffer = (UInt32 *)((MarshalHeader *)outBuffer + 1);
    }

    for (Int32 n = 0; n < paramNum; n++) {
        if (BT_IS_OUT(params[n])) {    // [out] or [in, out]
            args++;
            if (onlyReleaseIn)
                continue;

            if (*outBuffer != MSH_NULL) {
                assert(*outBuffer == MSH_NOT_NULL);
                parcel->WriteInt32(MSH_NOT_NULL);
                outBuffer++;

                switch (BT_TYPE(params[n])) {
                    case BT_TYPE_PUINT64:
                        parcel->WriteInt64(*(Int64*)outBuffer);
                        outBuffer += 2;
                        break;

                    case BT_TYPE_PSTRING:
                        if (*outBuffer) {
                            String* p = (String*)outBuffer;
                            parcel->WriteString(*p);
                            p->~String();
                        }
                        else {
                            parcel->WriteInt32(MSH_NULL);
                        }
                        outBuffer += sizeof(String) / 4;
                        break;

                    case BT_TYPE_STRINGBUF:
                        assert(0);
                        outBuffer++;
                        break;

                    case BT_TYPE_BUFFEROF:
                        assert(0);
                        break;

                    case BT_TYPE_ARRAYOF:
                        parcel->WriteArrayOf((Handle32)*outBuffer);
                        if (!BT_IS_CALLEE(params[n])) {
                            PCARQUINTET p = (PCARQUINTET)*outBuffer;
                            free(p->mBuf);
                            free(p);
                        }
                        outBuffer++;
                        break;

                    case BT_TYPE_PEMUID:
                        parcel->WriteEMuid(*(EMuid*)outBuffer);
                        outBuffer += sizeof(EMuid) / 4;
                        break;

                    case BT_TYPE_PSTRUCT:
                        parcel->WriteStruct((Handle32)outBuffer, BT_TYPE_SIZE(params[n]) * 4);
                        outBuffer += BT_TYPE_SIZE(params[n]);
                        break;

                    case BT_TYPE_PEGUID:
                        parcel->WriteEGuid(*(EGuid*)outBuffer);
                        outBuffer += sizeof(EMuid) / 4 + sizeof(char*) / 4 \
                                       + 80 * sizeof(char) / 4;
                        break;

                    case BT_TYPE_PINTERFACE: {
                        IInterface* object = (IInterface *)*outBuffer;
                        ECode ec = parcel->WriteInterfacePtr(object);
                        if (FAILED(ec)) {
                            MARSHAL_DBGOUT(MSHDBG_ERROR, printf(
                                    "MshProc: marshal interface, ec = %x, param index: %d\n", ec, n));
                            return ec;
                        }

                        if (NULL != object) object->Release();

                        outBuffer += sizeof(Elastos::RPC::InterfacePack) / 4;
                        break;
                    }

                    default:
                        parcel->WriteInt32(*(Int32*)outBuffer);
                        outBuffer++;
                        break;
                }
            }
            else {
                assert(*outBuffer == MSH_NULL);
                parcel->WriteInt32(MSH_NULL);
                outBuffer++;
            }
        }
        else {  // BT_IS_IN
            switch (BT_TYPE(params[n])) {
                case BT_TYPE_UINT64:
#ifdef _mips
                    // Adjust for 64bits align on mips
                    if (!(n % 2)) args += 1;
#endif
#if defined(_arm) && defined(__GNUC__) && (__GNUC__ >= 4)
                    args = (UInt32*)ROUND8((Int32)args);
#endif
                    args += 2;
                    break;

                case BT_TYPE_PUINT8:
                case BT_TYPE_PUINT16:
                case BT_TYPE_PUINT32:
                    free(*(Int32**)args);
                    args++;
                    break;

                case BT_TYPE_PUINT64:
                    free(*(Int64**)args);
                    args++;
                    break;

                case BT_TYPE_STRING: {
                    String* p = *(String**)args;
                    delete p;
                    args++;
                    break;
                }

                case BT_TYPE_EMUID:
                    args += sizeof(EMuid) / 4;
                    break;

                case BT_TYPE_EGUID:
                    args += sizeof(EMuid) / 4 + sizeof(char*) / 4;
                    break;

                case BT_TYPE_PEGUID:
                    free(*(EGuid**)args);
                    args++;
                    break;

                case BT_TYPE_PINTERFACE:
                    if (*args && *(UInt32 *)(*args)) {
                        ((IInterface *)*(UInt32 *)*args)->Release();
                    }
                    args++;
                    break;

                case BT_TYPE_INTERFACE:
                    if (*args) {
                        ((IInterface *)*args)->Release();
                    }
                    args++;
                    break;

                case BT_TYPE_STRINGBUF:
                case BT_TYPE_BUFFEROF:
                case BT_TYPE_ARRAYOF:
                    if (*args) {
                        if (CarQuintetFlag_Type_IObject == (((PCARQUINTET)*args)->mFlags
                                & CarQuintetFlag_TypeMask)) {
                            Int32 used = ((PCARQUINTET)*args)->mUsed /
                                    sizeof(IInterface *);
                            IInterface** itfBuf = (IInterface**)((PCARQUINTET)*args)->mBuf;
                            for (Int32 i = 0; i < used; i++) {
                                if (itfBuf[i]) {
                                    itfBuf[i]->Release();
                                    itfBuf[i] = NULL;
                                }
                            }
                        }
                        else if (CarQuintetFlag_Type_String == (((PCARQUINTET)*args)->mFlags
                                & CarQuintetFlag_TypeMask)) {
                            Int32 size = ((PCARQUINTET)*args)->mSize /
                                    sizeof(String);
                            String* strBuf = (String*)((PCARQUINTET)*args)->mBuf;
                            for (Int32 i = 0; i < size; i++) {
                                if (!strBuf[i].IsNull()) {
                                    strBuf[i] = NULL;
                                }
                            }
                        }
                        _CarQuintet_Release((PCARQUINTET)*args);
                    }
                    args++;
                    break;

                default:
                    args++;
                    break;
            }
        }
    }

    return NOERROR;
}

} // namespace RPC
} // namespace Elastos
