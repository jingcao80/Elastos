
#ifndef __CPACKAGE_H__
#define __CPACKAGE_H__

#include <elasys_server.h>

class CCallbackParcel : public IParcel
{
public:
    CCallbackParcel();

    virtual ~CCallbackParcel();

    CARAPI_(UInt32)AddRef();

    CARAPI_(UInt32)Release();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI Marshall(
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    CARAPI Unmarshall(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI AppendFrom(
        /* [in] */ IParcel* parcel,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI HasFileDescriptors(
        /* [out] */ Boolean* result);

    CARAPI ReadByte(
        /* [out] */ Byte *pValue);

    CARAPI WriteByte(
        /* [in] */ Byte value);

    CARAPI ReadBoolean(
        /* [out] */ Boolean *pValue);

    CARAPI WriteBoolean(
        /* [in] */ Boolean value);

    CARAPI ReadChar8(
        /* [out] */ Char8 *pValue);

    CARAPI WriteChar8(
        /* [in] */ Char8 value);

    CARAPI ReadChar16(
        /* [out] */ Char16 *pValue);

    CARAPI WriteChar16(
        /* [in] */ Char16 value);

    CARAPI ReadChar32(
        /* [out] */ Char32 *pValue);

    CARAPI WriteChar32(
        /* [in] */ Char32 value);

    CARAPI ReadInt16(
        /* [out] */ Int16 *pValue);

    CARAPI WriteInt16(
        /* [in] */ Int16 value);

    CARAPI ReadInt32(
        /* [out] */ Int32 *pValue);

    CARAPI WriteInt32(
        /* [in] */ Int32 value);

    CARAPI ReadInt64(
        /* [out] */ Int64 *pValue);

    CARAPI WriteInt64(
        /* [in] */ Int64 value);

    CARAPI ReadFloat(
        /* [out] */ Float *value);

    CARAPI WriteFloat(
        /* [in] */ Float value);

    CARAPI ReadDouble(
        /* [out] */ Double *value);

    CARAPI WriteDouble(
        /* [in] */ Double value);

    CARAPI ReadCString(
        /* [out] */ CString* pStr);

    CARAPI WriteCString(
        /* [in] */ CString str);

    CARAPI ReadString(
        /* [out] */ String *pStr);

    CARAPI WriteString(
        /* [in] */ const String& str);

    CARAPI ReadStruct(
        /* [ou] */ Handle32 *pAddress);

    CARAPI WriteStruct(
        /* [in] */ Handle32 pValue,
        /* [in] */ Int32 size);

    CARAPI ReadEMuid(
        /* [out] */ EMuid *pId);

    CARAPI WriteEMuid(
        /* [in] */ const EMuid & id);

    CARAPI ReadEGuid(
        /* [out] */ EGuid *pId);

    CARAPI WriteEGuid(
        /* [in] */ const EGuid & id);

    CARAPI ReadArrayOf(
        /* [out] */ Handle32 *ppArray);

    CARAPI WriteArrayOf(
        /* [in] */ Handle32 pArray);

    CARAPI ReadArrayOfCString(
        /* [out, callee] */ ArrayOf<CString>** ppArray);

    CARAPI WriteArrayOfCString(
        /* [in] */ ArrayOf<CString>* array);

    CARAPI ReadArrayOfString(
        /* [out, callee] */ ArrayOf<String>** ppArray);

    CARAPI WriteArrayOfString(
        /* [in] */ ArrayOf<String>* array);

    CARAPI ReadBufferOf(
        /* [out] */ Handle32 *ppBuffer);

    CARAPI WriteBufferOf(
        /* [in] */ Handle32 pBuffer);

    CARAPI ReadBufferOfCString(
        /* [out] */ Handle32 *ppBuffer);

    CARAPI WriteBufferOfCString(
        /* [in] */ const BufferOf<CString> & buffer);

    CARAPI ReadBufferOfString(
        /* [out] */ Handle32 *ppBuffer);

    CARAPI WriteBufferOfString(
        /* [in] */ const BufferOf<String> & buffer);

    CARAPI ReadStringBuf(
        /* [out] */ Handle32 *ppBuffer);

    CARAPI WriteStringBuf(
        /* [in] */ Handle32 buffer);

    CARAPI ReadMemoryBuf(
        /* [out] */ Handle32 *ppBuffer);

    CARAPI WriteMemoryBuf(
        /* [in] */ Handle32 buffer);

    CARAPI ReadInterfacePtr(
        /* [out] */ Handle32 *pItfPtr);

    CARAPI WriteInterfacePtr(
        /* [in] */ IInterface* pValue);

    CARAPI ReadBytePtr(
        /* [out] */ Handle32 *bypp);

    CARAPI WriteBytePtr(
        /* [in] */ Handle32 address);

    CARAPI ReadBooleanPtr(
        /* [out] */ Handle32 *bopp);

    CARAPI WriteBooleanPtr(
        /* [in] */ Handle32 address);

    CARAPI ReadChar8Ptr(
        /* [out] */ Handle32 *c8pp);

    CARAPI WriteChar8Ptr(
        /* [in] */ Handle32 address);

    CARAPI ReadChar16Ptr(
        /* [out] */ Handle32 *c16pp);

    CARAPI WriteChar16Ptr(
        /* [in] */ Handle32 address);

    CARAPI ReadInt16Ptr(
        /* [out] */ Handle32 *i16pp);

    CARAPI WriteInt16Ptr(
        /* [in] */ Handle32 address);

    CARAPI ReadInt32Ptr(
        /* [out] */ Handle32 *i32ptr);

    CARAPI WriteInt32Ptr(
        /* [in] */ Handle32 address);

    CARAPI ReadInt64Ptr(
        /* [out] */ Handle32 *i64ptr);

    CARAPI WriteInt64Ptr(
        /* [in] */ Handle32 address);

    CARAPI ReadFloatPtr(
        /* [out] */ Handle32 *fpp);

    CARAPI WriteFloatPtr(
        /* [in] */ Handle32 address);

    CARAPI ReadDoublePtr(
        /* [out] */ Handle32 *dpp);

    CARAPI WriteDoublePtr(
        /* [in] */ Handle32 address);

    CARAPI ReadStringPtr(
        /* [out] */ Handle32 *spp);

    CARAPI WriteStringPtr(
        /* [in] */ Handle32 address);

    CARAPI ReadStructPtr(
        /* [ou] */ Handle32 *pAddress);

    CARAPI ReadEMuidPtr(
        /* [out] */ Handle32 *idpp);

    CARAPI WriteEMuidPtr(
        /* [in] */ Handle32 idPtr);

    CARAPI ReadEGuidPtr(
        /* [out] */ Handle32 *idpp);

    CARAPI WriteEGuidPtr(
        /* [in] */ Handle32 idPtr);

    CARAPI ReadInterfacePtrPtr(
        /* [out] */ Handle32 *itfpp);

    CARAPI WriteInterfacePtrPtr(
        /* [in] */ Handle32 address);

    CARAPI ReadBufferOfPtr(
        /* [out] */ Handle32 *ppBuffer);

    CARAPI WriteFileDescriptor(
        /* [in] */ Int32 fd);

    CARAPI WriteDupFileDescriptor(
        /* [in] */ Int32 fd);

    CARAPI ReadFileDescriptor(
        /* [out] */ Int32* fd);

    CARAPI Clone(
        /* [in] */ IParcel* pSrcParcel);

    CARAPI GetDataPosition(
        /* [out] */ Int32* position);

    CARAPI SetDataPosition(
        /* [in] */ Int32 position);

    CARAPI GetElementSize(
        /* [out] */ Int32* pSize);

    CARAPI GetElementPayload(
        /* [ouy] */ Handle32* pBuffer);

private:
    ECode GrowTypeBuffer();

    ECode GrowElemBuffer();

    ECode GrowDataBuffer(int minSize);

    ECode WriteValue(PVoid pValue, Int32 type, Int32 size);

private:
    Int32 m_nRefs;

    Int32 m_elemCount;

    Byte* m_elemTypes;
    Int32 m_typeBufCapacity;

    Byte* m_elemBuf;
    Byte* m_elemPtr;
    Int32 m_elemBufCapacity;

    Byte* m_dataBuf;
    Byte* m_dataPtr;
    Int32 m_dataBufCapacity;
};

#endif
