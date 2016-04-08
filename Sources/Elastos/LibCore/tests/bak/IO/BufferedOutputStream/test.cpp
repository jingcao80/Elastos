#include "test.h"

CTest::CTest():
    m_pByteAos(NULL),
    m_pByteAis(NULL),
    m_pBufOs(NULL),
    m_pFileString(NULL)
{
    m_pFileString = BufferOf<Byte>::Alloc(3500);
    m_pFileString->SetUsed(3500);
    char pChar[3400] = "Test_All_Tests\nTest_java_io_BufferedInputStream\nTest_BufferedOutputStream\nTest_java_io_ByteArrayInputStream\nTest_java_io_ByteArrayOutputStream\nTest_java_io_DataInputStream\nTest_java_io_File\nTest_java_io_FileDescriptor\nTest_java_io_FileInputStream\nTest_java_io_FileNotFoundException\nTest_java_io_FileOutputStream\nTest_java_io_FilterInputStream\nTest_java_io_FilterOutputStream\nTest_java_io_InputStream\nTest_java_io_IOException\nTest_java_io_OutputStream\nTest_java_io_PrintStream\nTest_java_io_RandomAccessFile\nTest_java_io_SyncFailedException\nTest_java_lang_AbstractMethodError\nTest_java_lang_ArithmeticException\nTest_java_lang_ArrayIndexOutOfBoundsException\nTest_java_lang_ArrayStoreException\nTest_java_lang_Boolean\nTest_java_lang_Byte\nTest_java_lang_Character\nTest_java_lang_Class\nTest_java_lang_ClassCastException\nTest_java_lang_ClassCircularityError\nTest_java_lang_ClassFormatError\nTest_java_lang_ClassLoader\nTest_java_lang_ClassNotFoundException\nTest_java_lang_CloneNotSupportedException\nTest_java_lang_Double\nTest_java_lang_Error\nTest_java_lang_Exception\nTest_java_lang_ExceptionInInitializerError\nTest_java_lang_Float\nTest_java_lang_IllegalAccessError\nTest_java_lang_IllegalAccessException\nTest_java_lang_IllegalArgumentException\nTest_java_lang_IllegalMonitorStateException\nTest_java_lang_IllegalThreadStateException\nTest_java_lang_IncompatibleClassChangeError\nTest_java_lang_IndexOutOfBoundsException\nTest_java_lang_InstantiationError\nTest_java_lang_InstantiationException\nTest_java_lang_Integer\nTest_java_lang_InternalError\nTest_java_lang_InterruptedException\nTest_java_lang_LinkageError\nTest_java_lang_Long\nTest_java_lang_Math\nTest_java_lang_NegativeArraySizeException\nTest_java_lang_NoClassDefFoundError\nTest_java_lang_NoSuchFieldError\nTest_java_lang_NoSuchMethodError\nTest_java_lang_NullPointerException\nTest_java_lang_Number\nTest_java_lang_NumberFormatException\nTest_java_lang_Object\nTest_java_lang_OutOfMemoryError\nTest_java_lang_Runtimtion\nTest_java_lang_SecurityManager\nTest_java_lang_Short\nTest_java_lang_StackOverflowError\nTest_java_lang_String\nTest_java_lang_StringBuffer\nTest_java_lang_StringIndexOutOfBoundsException\nTest_java_lang_System\nTest_java_lang_Thread\nTest_java_lang_ThreadDeath\nTest_java_lang_ThreadGroup\nTest_java_lang_Throwable\nTest_java_lang_UnknownError\nTest_java_lang_UnsatisfiedLinkError\nTest_java_lang_VerifyError\nTest_java_lang_VirtualMachineError\nTest_java_lang_vm_Image\nTest_java_lang_vm_MemorySegment\nTest_java_lang_vm_ROMStoreException\nTest_java_lang_vm_VM\nTest_java_lang_Void\nTest_java_net_BindException\nTest_java_net_ConnectException\nTest_java_net_DatagramPacket\nTest_java_net_DatagramSocket\nTest_java_net_DatagramSocketImpl\nTest_java_net_InetAddress\nTest_java_net_NoRouteToHostException\nTest_java_net_PlainDatagramSocketImpl\nTest_java_net_PlainSocketImpl\nTest_java_net_Socket\nTest_java_net_SocketException\nTest_java_net_SocketImpl\nTest_java_net_SocketInputStream\nTest_java_net_SocketOutputStream\nTest_java_net_UnknownHostException\nTest_java_util_ArrayEnumerator\nTest_java_util_Date\nTest_java_util_EventObject\nTest_java_util_HashEnumerator\nTest_java_util_Hashtable\nTest_java_util_Properties\nTest_java_util_ResourceBundle\nTest_java_util_tm\nTest_java_util_Vector\n";
    m_pFileString->Copy((Byte*) pChar, 3500);

}
CTest::~CTest()
{
}

/*
 * ²âÊÔ¼òÊö: ²âÊÔ¹¹Ôìº¯Êý
 *
 * ²âÊÔÀàÐÍ: ¹¦ÄÜ²âÊÔ
 *
 * ÓÅÏÈ¼¶±ð: P1
 *
 * Ô¤ÆÚ½á¹û: OK
 *
 * ÔËÐÐ·½Ê½: clBufferedOutputStream.ecx 1
 *
 * ÊäÈëÎÄ¼þ:
 *
 * Êä³öÎÄ¼þ: clBufferedOutputStream1.out
 *
 * ½á¹û¼ì²é: Dailybuild
 *
 * ²¹³äËµÃ÷:
 */
 int CTest::TestConstructor(int argc, char *argv[])
{

    ECode rt = CByteArrayOutputStream::New(&m_pByteAos);

    if (0 == rt) rt = CBufferedOutputStream::New((IOutputStream*)m_pByteAos, &m_pBufOs);
//printf("0-------\n");
    if (0 == rt) rt = m_pBufOs->WriteBufferEx(0, 500, *m_pFileString);
//printf("1--------\n");
    if(m_pByteAos) m_pByteAos->Release();
    if(m_pBufOs) m_pBufOs->Release();
//printf("2--------\n");
    if (0 == rt) {
//        TPOK;
        printf("test 1 OK\n");
    }
    else {
//        TPERR("ERR: rt = %d\n", rt);
        printf("ERR: rt = %d\n", rt);
    }
//printf("3--------\n");
    return 0;
}

/*
 * ²âÊÔ¼òÊö: ²âÊÔ¹¹Ôìº¯Êý
 *
 * ²âÊÔÀàÐÍ: ¹¦ÄÜ²âÊÔ
 *
 * ÓÅÏÈ¼¶±ð: P1
 *
 * Ô¤ÆÚ½á¹û: OK
 *
 * ÔËÐÐ·½Ê½: clBufferedOutputStream.ecx 2
 *
 * ÊäÈëÎÄ¼þ:
 *
 * Êä³öÎÄ¼þ: clBufferedOutputStream2.out
 *
 * ½á¹û¼ì²é: Dailybuild
 *
 * ²¹³äËµÃ÷:
 */
 int CTest::TestConstructor2(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pByteAos);

    if (m_pByteAos && (0 == rt)) {
//    printf("==== File: %s, Line: %d ====\n", __FILE__, __LINE__);    
        rt = CBufferedOutputStream::New((IOutputStream*)m_pByteAos, -1, &m_pBufOs);
//    printf("2===%d\n", rt);
        if (rt == 0 || m_pBufOs) {
            printf("Error rt!=0.rt = %d", rt);
            if (m_pBufOs) m_pBufOs->Release();
            if (m_pByteAos) m_pByteAos->Release();
            return -1;          
        }
    }
//    printf("4===%d\n", rt);
    if (m_pByteAos && (0 != rt)) {
        rt = CBufferedOutputStream::New((IOutputStream*)m_pByteAos, 1024, &m_pBufOs);
//    printf("3===%d\n", rt);
        if ((rt != 0) && !m_pBufOs) {
            printf("Error rt==0.rt = %d", rt);
            if (m_pBufOs) m_pBufOs->Release();
            if (m_pByteAos) m_pByteAos->Release();
            return -2;          
        }

        rt = m_pBufOs->WriteBufferEx(0, 500, *m_pFileString);
    }

    if (m_pByteAos) m_pByteAos->Release();
    if (m_pBufOs) m_pBufOs->Release();

    if (0 == rt) {
        printf("test 2:OK\n");
    }
    else {
        printf("test 2 ERR: rt = %d\n", rt);
    }

    return 0;
}
/*
 * ²âÊÔ¼òÊö: Close
 *
 * ²âÊÔÀàÐÍ: ¹¦ÄÜ²âÊÔ
 *
 * ÓÅÏÈ¼¶±ð: P1
 *
 * Ô¤ÆÚ½á¹û: OK
 *
 * ÔËÐÐ·½Ê½: clBufferedOutputStream.ecx 3
 *
 * ÊäÈëÎÄ¼þ:
 *
 * Êä³öÎÄ¼þ: clBufferedOutputStream3.out
 *
 * ½á¹û¼ì²é: Dailybuild
 *
 * ²¹³äËµÃ÷:
 */
 int CTest::TestClose(int argc,char *argv[])
{
//        baos = new ByteArrayOutputStream();
//        os = new java.io.BufferedOutputStream(baos, 600);
//        os.write(fileString.getBytes(), 0, 500);
//        os.flush();
//        assertEquals("Test 1: Bytes not written after flush;",
//                500, ((ByteArrayOutputStream) baos).size());
//        os.close();
//
//        sos = new Support_OutputStream(true);
//        os = new BufferedOutputStream(sos, 10);
//        try {
//            os.flush();
//            fail("Test 2: IOException expected.");
//        } catch (IOException e) {
//            // Expected.
//        }
//        // To avoid exception during tearDown().
//        sos.setThrowsException(false);

    ECode rt = CByteArrayOutputStream::New(&m_pByteAos);

    rt = CBufferedOutputStream::New((IOutputStream*)m_pByteAos, 600, &m_pBufOs);

    if (0 == rt) {
        rt = m_pBufOs->WriteBufferEx(0, 500, *m_pFileString);
//printf("0------\n"); 
    }
printf("1------\n");
    rt = m_pBufOs->Flush();//²»µ÷ÓÃFlushÖ±½Óclose¹ÒÁË
printf("2------\n");  
    rt = m_pBufOs->Close();
printf("3------\n");  

    if (m_pByteAos) m_pByteAos->Release();
    if (m_pBufOs) m_pBufOs->Release();
printf("4ß------\n");  
    if (0 == rt) {
        printf("test 3:OK\n");
    }
    else {
        printf("ERR: rt = %d\n", rt);
    }

    return 0;
}
/*
 * ²âÊÔ¼òÊö: Flush
 *
 * ²âÊÔÀàÐÍ: ¹¦ÄÜ²âÊÔ
 *
 * ÓÅÏÈ¼¶±ð: P1
 *
 * Ô¤ÆÚ½á¹û: OK
 *
 * ÔËÐÐ·½Ê½: clBufferedOutputStream.ecx 4
 *
 * ÊäÈëÎÄ¼þ:
 *
 * Êä³öÎÄ¼þ: clBufferedOutputStream4.out
 *
 * ½á¹û¼ì²é: Dailybuild
 *
 * ²¹³äËµÃ÷:
 */
 int CTest::TestFlush(int argc,char *argv[])
{
//        baos = new ByteArrayOutputStream();
//        os = new java.io.BufferedOutputStream(baos, 600);
//        os.write(fileString.getBytes(), 0, 500);
//        os.flush();
//        assertEquals("Test 1: Bytes not written after flush;",
//                500, ((ByteArrayOutputStream) baos).size());
//        os.close();
//
//        sos = new Support_OutputStream(true);
//        os = new BufferedOutputStream(sos, 10);
//        try {
//            os.flush();
//            fail("Test 2: IOException expected.");
//        } catch (IOException e) {
//            // Expected.
//        }
//        // To avoid exception during tearDown().
//        sos.setThrowsException(false);
    ECode rt = CByteArrayOutputStream::New(&m_pByteAos);

    rt = CBufferedOutputStream::New((IOutputStream*)m_pByteAos, 600, &m_pBufOs);

    if (0 == rt) {
        rt = m_pBufOs->WriteBufferEx(0, 500, *m_pFileString);
    }

    rt = m_pBufOs->Flush();

    rt = m_pBufOs->Close();

    if (m_pByteAos) m_pByteAos->Release();
    if (m_pBufOs) m_pBufOs->Release();

    if (0 == rt) {
        printf("test 4:OK\n");
    }
    else {
        printf("test 4 ERR: rt = %d\n", rt);
    }

    return 0;
}
/*
 * ²âÊÔ¼òÊö: Write
 *
 * ²âÊÔÀàÐÍ: ¹¦ÄÜ²âÊÔ
 *
 * ÓÅÏÈ¼¶±ð: P1
 *
 * Ô¤ÆÚ½á¹û: OK
 *
 * ÔËÐÐ·½Ê½: clBufferedOutputStream.ecx 5
 *
 * ÊäÈëÎÄ¼þ:
 *
 * Êä³öÎÄ¼þ: clBufferedOutputStream5.out
 *
 * ½á¹û¼ì²é: Dailybuild
 *
 * ²¹³äËµÃ÷:
 */
 int CTest::TestWrite(int argc,char *argv[])
{
//        baos = new java.io.ByteArrayOutputStream();
//        os = new java.io.BufferedOutputStream(baos);
//        os.write('t');
//        bais = new java.io.ByteArrayInputStream(baos.toByteArray());
//        assertEquals("Test 1: Byte written, not buffered;",
//                0, bais.available());
//        os.flush();
//        bais = new java.io.ByteArrayInputStream(baos.toByteArray());
//        assertEquals("Test 2: Byte not written after flush;",
//                1, bais.available());
//        byte[] wbytes = new byte[1];
//        bais.read(wbytes, 0, 1);
//        assertEquals("Test 3: Incorrect byte written or read;",
//                't', wbytes[0]);
//        os.close();
//
//        sos = new Support_OutputStream(true);
//        os = new BufferedOutputStream(sos, 1);
//        os.write('t');
//        try {
//            // Exception is only thrown when the buffer is flushed.
//            os.write('e');
//            fail("Test 4: IOException expected.");
//        } catch (IOException e) {
//            // Expected.
//        }
//        // To avoid exception during tearDown().
//        sos.setThrowsException(false);
    ECode rt = CByteArrayOutputStream::New(&m_pByteAos);

    rt = CBufferedOutputStream::New((IOutputStream*)m_pByteAos, &m_pBufOs);
printf("0------\n");
    rt = m_pBufOs->Write('t');
printf("1------\n");


    if (m_pByteAos) m_pByteAos->Release();
    if (m_pBufOs) m_pBufOs->Release();

    if (0 == rt) {
        printf("test 5:OK\n");
    }
    else {
        printf("test 5 ERR: rt = %d\n", rt);
    }

    return 0;
}
/*
 * ²âÊÔ¼òÊö: WriteBuffer
 *
 * ²âÊÔÀàÐÍ: ¹¦ÄÜ²âÊÔ
 *
 * ÓÅÏÈ¼¶±ð: P1
 *
 * Ô¤ÆÚ½á¹û: OK
 *
 * ÔËÐÐ·½Ê½: clBufferedOutputStream.ecx 6
 *
 * ÊäÈëÎÄ¼þ:
 *
 * Êä³öÎÄ¼þ: clBufferedOutputStream6.out
 *
 * ½á¹û¼ì²é: Dailybuild
 *
 * ²¹³äËµÃ÷:
 */
 int CTest::TestWriteBuffer(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pByteAos);

    rt = CBufferedOutputStream::New((IOutputStream*)m_pByteAos, &m_pBufOs);
printf("0------\n");
    rt = m_pBufOs->WriteBuffer(*m_pFileString);
printf("1------\n");


    if (m_pByteAos) m_pByteAos->Release();
    if (m_pBufOs) m_pBufOs->Release();

    if (0 == rt) {
        printf("test 6:OK\n");
    }
    else {
        printf("test 6 ERR: rt = %d\n", rt);
    }

    return 0;
}
/*
 * ²âÊÔ¼òÊö: WriteBufferEx
 *
 * ²âÊÔÀàÐÍ: ¹¦ÄÜ²âÊÔ
 *
 * ÓÅÏÈ¼¶±ð: P1
 *
 * Ô¤ÆÚ½á¹û: OK
 *
 * ÔËÐÐ·½Ê½: clBufferedOutputStream.ecx 7
 *
 * ÊäÈëÎÄ¼þ:
 *
 * Êä³öÎÄ¼þ: clBufferedOutputStream7.out
 *
 * ½á¹û¼ì²é: Dailybuild
 *
 * ²¹³äËµÃ÷:
 */
  int CTest::TestWriteBufferEx(int argc,char *argv[])
{
//        os = new java.io.BufferedOutputStream(
//                baos = new java.io.ByteArrayOutputStream(),512);
//        os.write(fileString.getBytes(), 0, 500);
//        bais = new java.io.ByteArrayInputStream(baos.toByteArray());
//        assertEquals("Test 1: Bytes written, not buffered;",
//                0, bais.available());
//        os.flush();
//        bais = new java.io.ByteArrayInputStream(baos.toByteArray());
//        assertEquals("Test 2: Bytes not written after flush;",
//                500, bais.available());
//        os.write(fileString.getBytes(), 500, 513);
//        bais = new java.io.ByteArrayInputStream(baos.toByteArray());
//        assertTrue("Test 3: Bytes not written when buffer full.",
//                bais.available() >= 1000);
//        byte[] wbytes = new byte[1013];
//        bais.read(wbytes, 0, 1013);
//        assertTrue("Test 4: Incorrect bytes written or read.",
//                fileString.substring(0, 1013).equals(
//                        new String(wbytes, 0, wbytes.length)));
//        os.close();
//
//        sos = new Support_OutputStream(true);
//        os = new BufferedOutputStream(sos, 10);
//        try {
//            os.write(fileString.getBytes(), 0, 500);
//            fail("Test 5: IOException expected.");
//        } catch (IOException e) {
//            // Expected.
//        }
//        // To avoid exception during tearDown().
//        sos.setThrowsException(false);
    ECode rt = CByteArrayOutputStream::New(&m_pByteAos);

    rt = CBufferedOutputStream::New((IOutputStream*)m_pByteAos, 600, &m_pBufOs);

    if (0 == rt) {
        rt = m_pBufOs->WriteBufferEx(0, 500, *m_pFileString);
    }

    if (m_pByteAos) m_pByteAos->Release();
    if (m_pBufOs) m_pBufOs->Release();

    if (0 == rt) {
        printf("test 7:OK\n");
    }
    else {
        printf("ERR 7: rt = %d\n", rt);
    }

    return 0;
}
 /*
 * ²âÊÔ¼òÊö: CheckError
 *
 * ²âÊÔÀàÐÍ: ¹¦ÄÜ²âÊÔ
 *
 * ÓÅÏÈ¼¶±ð: P1
 *
 * Ô¤ÆÚ½á¹û: OK
 *
 * ÔËÐÐ·½Ê½: clBufferedOutputStream.ecx 8
 *
 * ÊäÈëÎÄ¼þ:
 *
 * Êä³öÎÄ¼þ: clBufferedOutputStream8.out
 *
 * ½á¹û¼ì²é: Dailybuild
 *
 * ²¹³äËµÃ÷:
 */
  int CTest::TestCheckError(int argc,char *argv[])
{
    ECode rt = CByteArrayOutputStream::New(&m_pByteAos);

    Boolean hasError = TRUE;

    if (0 == rt) {
        rt = m_pByteAos->CheckError(&hasError);
    }

    rt = m_pByteAos->Close();

    if (m_pByteAos) m_pByteAos->Release();

    if (FALSE == hasError) {
        printf("test 8:OK\n");
    }
    else {
        printf("test 8 ERR: hasError = %d\n", rt);
    }

    return 0;

}