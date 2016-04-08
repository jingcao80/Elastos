#include "test.h"
#include <elautoptr.h>

const String CTest::m_text("0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59\n60\n61\n62\n63\n64\n65\n66\n67\n68\n69\n70\n71\n72\n73\n74\n75\n76\n77\n78\n79\n80\n81\n82\n83\n84\n85\n86\n87\n88\n89\n90\n91\n92\n93\n94\n95\n96\n97\n98\n99\n100\n101\n102\n103\n104\n105\n106\n107\n108\n109\n110\n111\n112\n113\n114\n115\n116\n117\n118\n119\n120\n121\n122\n123\n124\n125\n126\n127\n128\n129\n130\n131\n132\n133\n134\n135\n136\n137\n138\n139\n140\n141\n142\n143\n144\n145\n146\n147\n148\n149\n150\n151\n152\n153\n154\n155\n156\n157\n158\n159\n160\n161\n162\n163\n164\n165\n166\n167\n168\n169\n170\n171\n172\n173\n174\n175\n176\n177\n178\n179\n180\n181\n182\n183\n184\n185\n186\n187\n188\n189\n190\n191\n192\n193\n194\n195\n196\n197\n198\n199\n200\n201\n202\n203\n204\n205\n206\n207\n208\n209\n210\n211\n212\n213\n214\n215\n216\n217\n218\n219\n220\n221\n222\n223\n224\n225\n226\n227\n228\n229\n230\n231\n232\n233\n234\n235\n236\n237\n238\n239\n240\n241\n242\n243\n244\n245\n246\n247\n248\n249\n250\n251\n252\n253\n254\n255\n256\n257\n258\n259\n260\n261\n262\n263\n264\n265\n266\n267\n268\n269\n270\n271\n272\n273\n274\n275\n276\n277\n278\n279\n280\n281\n282\n283\n284\n285\n286\n287\n288\n289\n290\n291\n292\n293\n294\n295\n296\n297\n298\n299\n300\n301\n302\n303\n304\n305\n306\n307\n308\n309\n310\n311\n312\n313\n314\n315\n316\n317\n318\n319\n320\n321\n322\n323\n324\n325\n326\n327\n328\n329\n330\n331\n332\n333\n334\n335\n336\n337\n338\n339\n340\n341\n342\n343\n344\n345\n346\n347\n348\n349\n350\n351\n352\n353\n354\n355\n356\n357\n358\n359\n360\n361\n362\n363\n364\n365\n366\n367\n368\n369\n370\n371\n372\n373\n374\n375\n376\n377\n378\n379\n380\n381\n382\n383\n384\n385\n386\n387\n388\n389\n390\n391\n392\n393\n394\n395\n396\n397\n398\n399\n400\n401\n402\n403\n404\n405\n406\n407\n408\n409\n410\n411\n412\n413\n414\n415\n416\n417\n418\n419\n420\n421\n422\n423\n424\n425\n426\n427\n428\n429\n430\n431\n432\n433\n434\n435\n436\n437\n438\n439\n440\n441\n442\n443\n444\n445\n446\n447\n448\n449\n450\n451\n452\n453\n454\n455\n456\n457\n458\n459\n460\n461\n462\n463\n464\n465\n466\n467\n468\n469\n470\n471\n472\n473\n474\n475\n476\n477\n478\n479\n480\n481\n482\n483\n484\n485\n486\n487\n488\n489\n490\n491\n492\n493\n494\n495\n496\n497\n498\n499\n500\n501");

CTest::CTest():
    m_pr(NULL),

    m_dosText("0\r\n1\r\n2")
{
}
CTest::~CTest()
{
}

/*
 *
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clPushbackReader.ecx 1
 */
 int CTest::TestConstructor(int argc, char *argv[])
{
    ICharArrayReader * ir;

    ArrayOf<Char8>* buffer = ArrayOf<Char8>::Alloc(1024);
    if (buffer == NULL) return E_OUT_OF_MEMORY_ERROR;
    const char * str = (const char*) m_text;
    buffer->Copy((Char8*) str, 1024);

    ECode rt = CCharArrayReader::New(buffer, &ir);

    if (rt == 0) {
        rt = CPushbackReader::New((IReader*)ir, &m_pr);
    }

    if (0 == rt) {
        printf("test 1:OK\n");
    }
    else {
        printf("test 1 ERR: rt = %d\n", rt);
    }
    if(ir) ir->Release();
    if(m_pr) m_pr->Release();
    ArrayOf<Char8>::Free(buffer);
    return 0;
}

/*
 *
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clPushbackReader.ecx 2
 */
 int CTest::TestConstructor2(int argc,char *argv[])
{
    ICharArrayReader * ir;

    ArrayOf<Char8>* buffer = ArrayOf<Char8>::Alloc(1024);
    if (buffer == NULL) return E_OUT_OF_MEMORY_ERROR;
    const char * str = (const char*) m_text;
    buffer->Copy((Char8*) str, 1024);

    ECode rt = CCharArrayReader::New(buffer, &ir);

    if (rt == 0) {
        rt = CPushbackReader::New((IReader*)ir, 3, &m_pr);
    }

    if (0 == rt) {
        printf("test 2:OK\n");
    }
    else {
        printf("test 2 ERR: rt = %d\n", rt);
    }
    if(ir) ir->Release();
    if(m_pr) m_pr->Release();
    ArrayOf<Char8>::Free(buffer);
    return 0;
}
/*
 *
 * ���: OK
 *
 * ���з�ʽ: clPushbackReader.ecx 3
 */
 int CTest::TestClose(int argc,char *argv[])
{
    ICharArrayReader * ir;

    ArrayOf<Char8>* buffer = ArrayOf<Char8>::Alloc(1024);
    if (buffer == NULL) return E_OUT_OF_MEMORY_ERROR;
    const char * str = (const char*) m_text;
    buffer->Copy((Char8*) str, 1024);

    ECode rt = CCharArrayReader::New(buffer, &ir);

    if (rt == 0) {
        rt = CPushbackReader::New((IReader*)ir, &m_pr);
    }

    if (0 == rt) {
        rt = m_pr->Close();
    }
    if (0 == rt) {
        printf("test 3:OK\n");
    }
    else {
        printf("test 3:ERR: rt = %d\n", rt);
    }
    if(ir) ir->Release();
    if(m_pr) m_pr->Release();
    ArrayOf<Char8>::Free(buffer);
    return 0;
}
/*
 * Ԥ�ڽ��: OK
 * Markδʵ��
 * ���з�ʽ: clPushbackReader.ecx 4
 */
 int CTest::TestMark(int argc,char *argv[])
{
    ICharArrayReader * ir;

    ArrayOf<Char8>* buffer = ArrayOf<Char8>::Alloc(1024);
    if (buffer == NULL) return E_OUT_OF_MEMORY_ERROR;
    const char * str = (const char*) m_text;
    buffer->Copy((Char8*) str, 1024);

    ECode rt = CCharArrayReader::New(buffer, &ir);

    if (rt == 0) {
        rt = CPushbackReader::New((IReader*)ir, &m_pr);
    }
    if (0 == rt) {
        rt = m_pr->Mark(44);
    }
    if (0 == rt) {
        printf("test 4:OK\n");
    }
    else {
        printf("test 4 ERR: rt = %d\n", rt);
    }
    if(ir) ir->Release();
    if(m_pr) m_pr->Release();
    ArrayOf<Char8>::Free(buffer);
    return 0;
}
/*
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clPushbackReader.ecx 5
 */
 int CTest::TestIsMarkSupported(int argc,char *argv[])
{
    ICharArrayReader * ir;

    ArrayOf<Char8>* buffer = ArrayOf<Char8>::Alloc(1024);
    if (buffer == NULL) return E_OUT_OF_MEMORY_ERROR;
    const char * str = (const char*) m_text;
    buffer->Copy((Char8*) str, 1024);

    ECode rt = CCharArrayReader::New(buffer, &ir);

    if (rt == 0) {
        rt = CPushbackReader::New((IReader*)ir, &m_pr);
    }
    Boolean supported;
    if (0 == rt) {
        rt = m_pr->IsMarkSupported(&supported);
    }
    if (0 == rt) {
        printf("test 5:OK\n");
    }
    else {
        printf("test 5 ERR: rt = %d\n", rt);
    }
    if(ir) ir->Release();
    if(m_pr) m_pr->Release();
    ArrayOf<Char8>::Free(buffer);
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
 * ���з�ʽ: clPushbackReader.ecx 6
 */
  int CTest::TestRead(int argc,char *argv[])
{
    ICharArrayReader * ir;

    ArrayOf<Char8>* buffer = ArrayOf<Char8>::Alloc(1024);
    if (buffer == NULL) return E_OUT_OF_MEMORY_ERROR;
    const char * str = (const char*) m_text;
    buffer->Copy((Char8*) str, 1024);

    ECode rt = CCharArrayReader::New(buffer, &ir);

    if (rt == 0) {
        rt = CPushbackReader::New((IReader*)ir, &m_pr);
    }
    Int32 value;
    if (0 == rt) {
        rt = m_pr->Read(&value);
    }
    if (0 == rt) {
        printf("test 6:OK\n");
    }
    else {
        printf("test 6 ERR: rt = %d\n", rt);
    }
    if(ir) ir->Release();
    if(m_pr) m_pr->Release();
    ArrayOf<Char8>::Free(buffer);

    return 0;
}
 /*
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clPushbackReader.ecx 7
 */
  int CTest::TestReadBuffer(int argc,char *argv[])
{
    ICharArrayReader * ir;

    ArrayOf<Char8>* buffer = ArrayOf<Char8>::Alloc(1024);
    if (buffer == NULL) return E_OUT_OF_MEMORY_ERROR;
    const char * str = (const char*) m_text;
    buffer->Copy((Char8*) str, 1024);

    ECode rt = CCharArrayReader::New(buffer, &ir);

    if (rt == 0) {
        rt = CPushbackReader::New((IReader*)ir, &m_pr);
    }
    Int32 number;
    ArrayOf<Char8> *buf = ArrayOf<Char8>::Alloc(1024);
    if (0 == rt) {
        rt = m_pr->ReadBuffer(buf, &number);
    }
    if (0 == rt) {
        printf("test 7:OK\n");
    }
    else {
        printf("test 7 ERR: rt = %d\n", rt);
    }
    if(ir) ir->Release();
    if(m_pr) m_pr->Release();
    ArrayOf<Char8>::Free(buffer);
    ArrayOf<Char8>::Free(buf);
    return 0;
}
 /*
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clPushbackReader.ecx 8
 *
 */
  int CTest::TestReadBufferEx(int argc,char *argv[])
{
    ICharArrayReader * ir;

    ArrayOf<Char8>* buffer = ArrayOf<Char8>::Alloc(1024);
    if (buffer == NULL) return E_OUT_OF_MEMORY_ERROR;
    const char * str = (const char*) m_text;
    buffer->Copy((Char8*) str, 1024);

    ECode rt = CCharArrayReader::New(buffer, &ir);

    if (rt == 0) {
        rt = CPushbackReader::New((IReader*)ir, &m_pr);
    }
    Int32 number;
    ArrayOf<Char8> *buf = ArrayOf<Char8>::Alloc(1024);
    if (0 == rt) {
        rt = m_pr->ReadBufferEx(0, 1, buf, &number);
    }
    if (0 == rt) {
        printf("test 8:OK\n");
    }
    else {
        printf("test 8 ERR: rt = %d\n", rt);
    }
    if(ir) ir->Release();
    if(m_pr) m_pr->Release();
    ArrayOf<Char8>::Free(buffer);
    ArrayOf<Char8>::Free(buf);
    return 0;
}
 /*
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clPushbackReader.ecx 9
 */
  int CTest::TestIsReady(int argc,char *argv[])
{
    ICharArrayReader * ir;

    ArrayOf<Char8>* buffer = ArrayOf<Char8>::Alloc(1024);
    if (buffer == NULL) return E_OUT_OF_MEMORY_ERROR;
    const char * str = (const char*) m_text;
    buffer->Copy((Char8*) str, 1024);

    ECode rt = CCharArrayReader::New(buffer, &ir);

    if (rt == 0) {
        rt = CPushbackReader::New((IReader*)ir, &m_pr);
    }
    Boolean ready;

    if (0 == rt) {
        rt = m_pr->IsReady(&ready);
    }
    if (0 == rt) {
        printf("test 9:OK\n");
    }
    else {
        printf("test 9 ERR: rt = %d\n", rt);
    }
    if(ir) ir->Release();
    if(m_pr) m_pr->Release();
    ArrayOf<Char8>::Free(buffer);
    return 0;
}
 /*
 * Ԥ�ڽ��: OK///Resetûʵ��
 *
 * ���з�ʽ: clPushbackReader.ecx 10
 */
  int CTest::TestReset(int argc,char *argv[])
{
    ICharArrayReader * ir;

    ArrayOf<Char8>* buffer = ArrayOf<Char8>::Alloc(1024);
    if (buffer == NULL) return E_OUT_OF_MEMORY_ERROR;
    const char * str = (const char*) m_text;
    buffer->Copy((Char8*) str, 1024);

    ECode rt = CCharArrayReader::New(buffer, &ir);

    if (rt == 0) {
        rt = CPushbackReader::New((IReader*)ir, &m_pr);
    }

    if (0 == rt) {
        rt = m_pr->Reset();
    }
    if (0 == rt) {
        printf("test 10:OK\n");
    }
    else {
        printf("test 10 ERR: rt = %d\n", rt);
    }
    if(ir) ir->Release();
    if(m_pr) m_pr->Release();
    ArrayOf<Char8>::Free(buffer);
    return 0;
}
 /*
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clPushbackReader.ecx 11
 */
  int CTest::TestSkip(int argc,char *argv[])
{
    ICharArrayReader * ir;

    ArrayOf<Char8>* buffer = ArrayOf<Char8>::Alloc(1024);
    if (buffer == NULL) return E_OUT_OF_MEMORY_ERROR;
    const char * str = (const char*) m_text;
    buffer->Copy((Char8*) str, 1024);

    ECode rt = CCharArrayReader::New(buffer, &ir);

    if (rt == 0) {
        rt = CPushbackReader::New((IReader*)ir, &m_pr);
    }
    Int64 number;
    if (0 == rt) {
        rt = m_pr->Skip(1, &number);
    }
    if (0 == rt) {
        printf("test 11:OK\n");
    }
    else {
        printf("test 11 ERR: rt = %d\n", rt);
    }
    if(ir) ir->Release();
    if(m_pr) m_pr->Release();
    ArrayOf<Char8>::Free(buffer);
    return 0;
}

 /*
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clPushbackReader.ecx 12
 */
  int CTest::TestUnRead(int argc,char *argv[])
{
    ICharArrayReader * ir;

    ArrayOf<Char8>* buffer = ArrayOf<Char8>::Alloc(1024);
    if (buffer == NULL) return E_OUT_OF_MEMORY_ERROR;
    const char * str = (const char*) m_text;
    buffer->Copy((Char8*) str, 1024);

    ECode rt = CCharArrayReader::New(buffer, &ir);

    if (rt == 0) {
        rt = CPushbackReader::New((IReader*)ir, &m_pr);
    }

    if (0 == rt) {
        rt = m_pr->UnRead(1);
    }
    if (0 == rt) {
        printf("test 12:OK\n");
    }
    else {
        printf("test 12 ERR: rt = %d\n", rt);
    }
    if(ir) ir->Release();
    if(m_pr) m_pr->Release();
    ArrayOf<Char8>::Free(buffer);
    return 0;
}
 /*
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clPushbackReader.ecx 13
 */
  int CTest::TestUnReadBuffer(int argc,char *argv[])
{
    ICharArrayReader * ir;

    ArrayOf<Char8>* buffer = ArrayOf<Char8>::Alloc(1024);
    if (buffer == NULL) return E_OUT_OF_MEMORY_ERROR;
    const char * str = (const char*) m_text;
    buffer->Copy((Char8*) str, 1024);

    ECode rt = CCharArrayReader::New(buffer, &ir);

    if (rt == 0) {
        rt = CPushbackReader::New((IReader*)ir, 1025, &m_pr);
    }

    if (0 == rt) {
        rt = m_pr->UnReadBuffer(*buffer);
    }
    if (0 == rt) {
        printf("test 13:OK\n");
    }
    else {
        printf("test 13 ERR: rt = %d\n", rt);
    }
    if(ir) ir->Release();
    if(m_pr) m_pr->Release();
    ArrayOf<Char8>::Free(buffer);
    return 0;
}
 /*
 * Ԥ�ڽ��: OK
 *
 * ���з�ʽ: clPushbackReader.ecx 14
 */
  int CTest::TestUnReadBufferEx(int argc,char *argv[])
{
    ICharArrayReader * ir;

    ArrayOf<Char8>* buffer = ArrayOf<Char8>::Alloc(1024);
    if (buffer == NULL) return E_OUT_OF_MEMORY_ERROR;
    const char * str = (const char*) m_text;
    buffer->Copy((Char8*) str, 1024);

    ECode rt = CCharArrayReader::New(buffer, &ir);

    if (rt == 0) {
        rt = CPushbackReader::New((IReader*)ir, &m_pr);
    }
    if (0 == rt) {
        rt = m_pr->UnReadBufferEx(0, 0, *buffer);
    }
    if (0 == rt) {
        printf("test 14:OK\n");
    }
    else {
        printf("test 14 ERR: rt = %d\n", rt);
    }
    if(ir) ir->Release();
    if(m_pr) m_pr->Release();
    ArrayOf<Char8>::Free(buffer);
    return 0;
}
