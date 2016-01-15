#include "test.h"
#include <elautoptr.h>

CTest::CTest():
    m_pBaos(NULL),
    m_pDis(NULL),
    m_pDos(NULL),
    testString("Lorem ipsum dolor sit amet,\nconsectetur adipisicing elit,\nsed do eiusmod tempor incididunt utlabore et dolore magna aliqua.\n")
    ,mUnihw()
//    ,mUnihw("\u0048\u0065\u006C\u006C\u006F\u0020\u0057\u006F\u0072\u006C\u0064")
{

    testLength = testString.GetLength();

}
CTest::~CTest()
{
}

/*
 *
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clDataOutputStream.ecx 1
 */
 int CTest::TestConstructor(int argc, char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);

    if (0 == rt) {
        printf("test 1:OK\n");
    }
    else {
        printf("test 1 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();

    return 0;
}

/*
 *
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clDataOutputStream.ecx 2
 */
 int CTest::TestClose(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);

    rt = m_pDos->Close();

    if (0 == rt) {
        printf("test 2:OK\n");
    }
    else {
        printf("test 2 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();

    return 0;
}
/*
 *
 * ���: OK
 *
 * ���з�ʽ: clDataOutputStream.ecx 3
 */
 int CTest::TestFlush(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);

    IDataOutput* os1;
    os1 = IDataOutput::Probe(m_pDos);

    if ( 0 == rt)
        rt = os1->WriteInt32(9087589);

    rt = m_pDos->Flush();

    if (0 == rt) {
        printf("test 3:OK\n");
    }
    else {
        printf("test 3 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();

    return 0;
}
/*
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clDataOutputStream.ecx 4
 */
 int CTest::TestWrite(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);
//printf("0-------\n");
    m_pDos->Write(42);
    
    m_pDos->Close();

//    rt = CByteArrayInputStream::New(m_pBaos, m_pDis);
//
//    m_pDis->Read();
//    m_pDis->Close();

    if (0 == rt) {
        printf("test 4:OK\n");
    }
    else {
        printf("test 4 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();

    return 0;
}
/*
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clDataOutputStream.ecx 5
 */
 int CTest::TestWriteBuffer(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);

    BufferOf<Byte>* buffer = BufferOf<Byte>::Alloc(140);
    char pChar[140] = "Lorem ipsum dolor sit amet,\nconsectetur adipisicing elit,\nsed do eiusmod tempor incididunt utlabore et dolore magna aliqua.\n";

    buffer->Copy((Byte*) pChar, 140);

    rt =  m_pDos->WriteBuffer(*buffer);
    
    rt = m_pDos->Close();

    if (0 == rt) {
        printf("test 5:OK\n");
    }
    else {
        printf("test 5 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();
    if(buffer) BufferOf<Byte>::Free(buffer);
    return 0;
}
/*
 * ���Լ���: WriteBufferEx
 *
 * ��������: ���ܲ���
 *
 * ���ȼ���: P1
 *
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clDataOutputStream.ecx 6
 */
  int CTest::TestWriteBufferEx(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);
printf("0-------\n");

    BufferOf<Byte>* buffer = BufferOf<Byte>::Alloc(140);
    char pChar[140] = "Lorem ipsum dolor sit amet,\nconsectetur adipisicing elit,\nsed do eiusmod tempor incididunt utlabore et dolore magna aliqua.\n";

    buffer->Copy((Byte*) pChar, 140);


   rt =  m_pDos->WriteBufferEx(5, testLength - 7, *buffer);
    
    rt = m_pDos->Close();

//    rt = CByteArrayInputStream::New(m_pBaos, m_pDis);
//
//    m_pDis->Read();
//    m_pDis->Close();

    if (0 == rt) {
        printf("test 6:OK\n");
    }
    else {
        printf("test 6 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();
    if(buffer) BufferOf<Byte>::Free(buffer);

    return 0;
}
 /*
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clDataOutputStream.ecx 7
 */
  int CTest::TestCheckError(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);

    Boolean hasError = TRUE;

    if (0 == rt) {
        rt = m_pDos->CheckError(&hasError);
    }

    rt = m_pDos->Close();

    if (m_pDos) m_pDos->Release();

    if (FALSE == hasError) {
        printf("test 7:OK\n");
    }
    else {
        printf("test 7 ERR: hasError = %d\n", rt);
    }
    return 0;
}
 /*
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clDataOutputStream.ecx 8
 *
 */
  int CTest::TestWriteBoolean(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);

    IDataOutput* os1 = NULL;
    Boolean value = 0;
    os1 = IDataOutput::Probe(m_pDos);

    if ( 0 == rt)
        rt = os1->WriteBoolean(value);

    if ( 0 == rt)
        rt = m_pDos->Close();

    if (0 == rt) {
        printf("test 8:OK\n");
    }
    else {
        printf("test 8 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();
    return 0;
}
 /*
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clDataOutputStream.ecx 9
 */
  int CTest::TestWriteByte(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);

    IDataOutput* os1 = NULL;
    Boolean value = 0;
    os1 = IDataOutput::Probe(m_pDos);

    if ( 0 == rt)
        rt = os1->WriteByte(value);

    if ( 0 == rt)
        rt = m_pDos->Close();

    if (0 == rt) {
        printf("test 9:OK\n");
    }
    else {
        printf("test 9 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();
    return 0;
}
 /*
 *FAIL_RETURN(mOut->WriteBuffer(buffer));��֪����������
 * ���з�ʽ: clDataOutputStream.ecx 10
 *
 */
  int CTest::TestWriteBytes(int argc,char *argv[])
{
//        os.writeBytes(testString);
//        os.close();
//
//        openDataInputStream();
//        byte[] rbuf = new byte[testLength];
//        dis.read(rbuf, 0, testLength);
//        dis.close();
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);
    if ( 0 == rt) {
        IDataOutput* os1 = NULL;
        os1 = IDataOutput::Probe(m_pDos);
printf("000000000\n");
        BufferOf<Byte>* buffer = BufferOf<Byte>::Alloc(140);
        char pChar[140] = "Lorem ipsum dolor sit amet,\nconsectetur adipisicing elit,\nsed do eiusmod tempor incididunt utlabore et dolore magna aliqua.\n";

        buffer->Copy((Byte*) pChar, 140);
printf("110000000%s, os1:%x\n", buffer->GetPayload(), os1);
        rt = os1->WriteBytes(*buffer);//����
printf("220000000\n");
        if(buffer) BufferOf<Byte>::Free(buffer);
    }
    if ( 0 == rt)
        rt = m_pDos->Close();

//    if (0 == rt)
//        rt = CByteArrayInputStream::New(m_pBaos, m_pDis);
//
//    BufferOf<Byte>* buffer = BufferOf<Byte>::Alloc(140);
//    Int32 number;
//
//    rt = m_pDis->ReadBufferEx(0, testLength, buffer, &number);
//    rt = m_pDis->Close();


    if (0 == rt) {
        printf("test 10:OK\n");
    }
    else {
        printf("test 10 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();
    if(m_pDis) m_pDis->Release();
//    if(buffer) BufferOf<Byte>::Free(buffer);

    return 0;
}
 /*
 *
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clDataOutputStream.ecx 11
 */
  int CTest::TestWriteBytesEx(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);
    if ( 0 == rt) {
        IDataOutput* os1 = NULL;
        os1 = IDataOutput::Probe(m_pDos);
printf("000000000\n");
        BufferOf<Byte>* buffer = BufferOf<Byte>::Alloc(140);
        char pChar[140] = "Lorem ipsum dolor sit amet,\nconsectetur adipisicing elit,\nsed do eiusmod tempor incididunt utlabore et dolore magna aliqua.\n";

        buffer->Copy((Byte*) pChar, 140);
printf("110000000%s, os1:%x\n", buffer->GetPayload(), os1);
        rt = os1->WriteBytesEx(0, 0, *buffer);
printf("220000000\n");
        if(buffer) BufferOf<Byte>::Free(buffer);
    }
    if ( 0 == rt)
        rt = m_pDos->Close();

    if (0 == rt) {
        printf("test 11:OK\n");
    }
    else {
        printf("test 11 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();
    return 0;
}

  int CTest::TestWriteChar(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);

    IDataOutput* os1 = NULL;
    Int32 value = 9;
    os1 = IDataOutput::Probe(m_pDos);

    if ( 0 == rt)
        rt = os1->WriteChar(value);

    rt = m_pDos->Close();

    if (0 == rt) {
        printf("test 12:OK\n");
    }
    else {
        printf("test 12 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();

    return 0;
}

  int CTest::TestWriteDouble(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);

    IDataOutput* os1 = NULL;
    Double value = 16.1;
    os1 = IDataOutput::Probe(m_pDos);

    if ( 0 == rt)
        rt = os1->WriteDouble(value);

    rt = m_pDos->Close();

    if (0 == rt) {
        printf("test 13:OK\n");
    }
    else {
        printf("test 13 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();
    return 0;
}

  int CTest::TestWriteFloat(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);

    IDataOutput* os1 = NULL;
    Float value = 16.1;
    os1 = IDataOutput::Probe(m_pDos);

    if ( 0 == rt)
        rt = os1->WriteFloat(value);

    rt = m_pDos->Close();

    if (0 == rt) {
        printf("test 14:OK\n");
    }
    else {
        printf("test 14 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();
    return 0;
}
  int CTest::TestWriteInt32(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);

    IDataOutput* os1 = NULL;
    Int32 value = 16;
    os1 = IDataOutput::Probe(m_pDos);

    if ( 0 == rt)
        rt = os1->WriteInt32(value);

    rt = m_pDos->Close();

    if (0 == rt) {
        printf("test 15:OK\n");
    }
    else {
        printf("test 15 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();

    return 0;
}

  int CTest::TestWriteInt64(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);

    IDataOutput* os1 = NULL;
    Int64 value = 16;
    os1 = IDataOutput::Probe(m_pDos);

    if ( 0 == rt)
        rt = os1->WriteInt64(value);

    rt = m_pDos->Close();


    if (0 == rt) {
        printf("test 16:OK\n");
    }
    else {
        printf("test 16 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();

    return 0;
}

  int CTest::TestWriteInt16(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pBaos);
    if ( 0 == rt)
        rt = CDataOutputStream::New((IOutputStream*)m_pBaos, &m_pDos);

    IDataOutput* os1 = NULL;
    Int32 value = 16;
    os1 = IDataOutput::Probe(m_pDos);

    if ( 0 == rt)
        rt = os1->WriteInt16(value);

    rt = m_pDos->Close();


    if (0 == rt) {
        printf("test 17:OK\n");
    }
    else {
        printf("test 17 ERR: rt = %d\n", rt);
    }
    if(m_pBaos) m_pBaos->Release();
    if(m_pDos) m_pDos->Release();

    return 0;
}
