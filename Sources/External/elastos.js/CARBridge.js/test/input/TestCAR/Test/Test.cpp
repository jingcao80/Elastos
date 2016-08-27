
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <elastos.h>

#include <CARBridge.js.test.input.Test.h>
#include "../CTest.h"


#define _BUF_SIZE 4096



char const *_GetString(Boolean b)
{
    return b != FALSE ? "true" : "false";
}

char const *_GetString(ETest etest)
{
    switch (etest) {
    case ETest_0:
        return "ETest_0";

    case ETest_1:
        return "ETest_1";

    default:
        abort();
    }
}

int main(int argc, char const *argv[])
{
    AutoPtr<ITest> itest;
    ITest *_itest;

    FILE *output;

    int n;

    char ebuf[_BUF_SIZE];
    char obuf[_BUF_SIZE];

    Int16 i16;

    Int32 i32;

    Int64 i64;

    Byte byte;

    Float f;

    Double d;

    Char32 c32;

    String s("7jfw3r9gnefiogj0rejgnbalsjvefLJlgkjfelgjijL");

    Boolean b;

    EMuid emuid;

    EGuid eguid;
    char _eguidUunm[] = "fqufrv3q0h038qtugN20Vg85HQ3b0Gj0Q2gJ3Q0J503GrJfErKgJRvh";
    eguid.mUunm = _eguidUunm;

    ECode ecode;

    int __;
    PVoid ptr = &__;

    ETest etest;

    Int16 _i16s[2];
    ArrayOf<Int16> i16s(_i16s, 2);

    Int32 _i32s[2];
    ArrayOf<Int32> i32s(_i32s, 2);

    Int64 _i64s[2];
    ArrayOf<Int64> i64s(_i64s, 2);

    Byte _bytes[2];
    ArrayOf<Byte> bytes(_bytes, 2);

    Float _fs[2];
    ArrayOf<Float> fs(_fs, 2);

    Double _ds[2];
    ArrayOf<Double> ds(_ds, 2);

    ETest _etests[2];
    ArrayOf<ETest> etests(_etests, 2);

    Char32 _c32s[2];
    ArrayOf<Char32> c32s(_c32s, 2);

    String _ss[2];
    ArrayOf<String> ss(_ss, 2);

    Boolean _bs[2];
    ArrayOf<Boolean> bs(_bs, 2);

    EMuid _emuids[2];
    ArrayOf<EMuid> emuids(_emuids, 2);

    EGuid _eguids[2];
    char _eguids0Uunm[] = "jArwwgj08vcj09arhgV0aiernVofjvofjosAijoaijvjRvIh0VwrivjsFhjFlKgflvfoeiIJdjvkfnlkdf";
    _eguids[0].mUunm = _eguids0Uunm;
    char _eguids1Uunm[] = "DsjkEe98Ug38rqhgb3ibaerijvoJv9Q3yghvAr9hAihv9hgawirneihb398Bhr89ygQ983y57GhBj";
    _eguids[1].mUunm = _eguids1Uunm;
    ArrayOf<EGuid> eguids(_eguids, 2);

    ECode _ecodes[2];
    ArrayOf<ECode> ecodes(_ecodes, 2);

    ITest *_itests[2];
    ArrayOf<ITest *> itests(_itests, 2);

    STest _stests[2];
    char _stests0EGuidUunm[] = "oY608uht4W98yhq3rijNb3750ohjbnr3q98yh250F49hjn9ih";
    _stests[0].eguid.mUunm = _stests0EGuidUunm;
    char _stests0VEGuid0Uunm[] = "Q8nYbhy0n4th409jnm3y0on4tw9eujBn03yo5nm3509b3rq9jbF53jh548yfF237r2F058Uy653";
    _stests[0].veguid[0].mUunm = _stests0VEGuid0Uunm;
    char _stests0VEGuid1Uunm[] = "omnsgplm9etw8hbjw0ribn40ofkfj9uqh0y5mb49uh";
    _stests[0].veguid[1].mUunm = _stests0VEGuid1Uunm;
    char _stests0VVEGuid00Uunm[] = "09qijv02ijqh0wi9uyjnaoJ0N5NM4QKBJ0Fwhg89248gHvw";
    _stests[0].vveguid[0][0].mUunm = _stests0VVEGuid00Uunm;
    char _stests0VVEGuid01Uunm[] = "Uhjm4w09hjbmf0hj309hjt0jbthj03hj40hj509h";
    _stests[0].vveguid[0][1].mUunm = _stests0VVEGuid01Uunm;
    char _stests0VVEGuid10Uunm[] = "q83HgN35Qh249iNqT2u2059uH359mcF3v0Nu3nyUb3hjVTmU930bn3v4uG49Wu";
    _stests[0].vveguid[1][0].mUunm = _stests0VVEGuid10Uunm;
    char _stests0VVEGuid11Uunm[] = "I307i8o4Uwty31bytv2uny0bn6uihm460hu65j439ub05vunHb56yiv24yuvtCy25";
    _stests[0].vveguid[1][1].mUunm = _stests0VVEGuid11Uunm;
    char _stests1EGuidUunm[] = "f487Ytvu0F569ui360yiTq9483yh9gfyuG58omi7ubyvT9crTtyUb540";
    _stests[1].eguid.mUunm = _stests1EGuidUunm;
    char _stests1VEGuid0Uunm[] = "uiN758vtun3HB8u6yJrMv4uyv0mYbu4vttybuyjv5ub9y654imVIomtu09u9036bIyM2509";
    _stests[1].veguid[0].mUunm = _stests1VEGuid0Uunm;
    char _stests1VEGuid1Uunm[] = "uByF420nvy375F725t659yubb6ojHmeroIghtigjsg7w68rtg83qynB35jh0tce5Sc5etgHj7o";
    _stests[1].veguid[1].mUunm = _stests1VEGuid1Uunm;
    char _stests1VVEGuid00Uunm[] = "69U88rv7typ8Pk5sx54wwereecry0uYYmi59vtu398yv54uv3qUtyv3vt536";
    _stests[1].vveguid[0][0].mUunm = _stests1VVEGuid00Uunm;
    char _stests1VVEGuid01Uunm[] = "u4Mior3qu98Trw98ybuM4bpoityv3teNgetiOphn7509jvny4w9ejGvu4w5yboIm450B";
    _stests[1].vveguid[0][1].mUunm = _stests1VVEGuid01Uunm;
    char _stests1VVEGuid10Uunm[] = "yVuJn498vUyb5e6Byu49rv67435yBF609u4012089t9854uhJwo";
    _stests[1].vveguid[1][0].mUunm = _stests1VVEGuid10Uunm;
    char _stests1VVEGuid11Uunm[] = "PiiubE509mgUn4w7yv359Mubo5eiUn9tr8jYbu509myuVn54rcUN48tyuB56";
    _stests[1].vveguid[1][1].mUunm = _stests1VVEGuid11Uunm;
    ArrayOf<STest> stests(_stests, 2);

    STest stest;
    char _stestEGuidUunm[] = "jFwajGj249G3jFvoAJf0943qUg93gVAhrHgQgGhrq9hVr9aH";
    stest.eguid.mUunm = _stestEGuidUunm;
    char _stestVEGuid0Uunm[] = "83hQgOR3g9vh3rfIrJ9pghwrVNq3uGj3qirng5Uhjw4tOjwGj39iVh9Gh";
    stest.veguid[0].mUunm = _stestVEGuid0Uunm;
    char _stestVEGuid1Uunm[] = "Q9uGHq3r9UhWrUhG3ruVhqr39hGruvQ309hGr3iWbh4wthRhbuHbRhR";
    stest.veguid[1].mUunm = _stestVEGuid1Uunm;
    char _stestVVEGuid00Uunm[] = "alkGjq398Ygh93q8yg9w48hb9Q428hy98eahbwoijBerjbw4p9uhBjwF29hGj";
    stest.vveguid[0][0].mUunm = _stestVVEGuid00Uunm;
    char _stestVVEGuid01Uunm[] = "209qHb4w90uJhb50ejN590hj458hj4wt09hm4tijb40jh4w0IJn4thj40";
    stest.vveguid[0][1].mUunm = _stestVVEGuid01Uunm;
    char _stestVVEGuid10Uunm[] = "94jB498Whj05924hj5109hj4209ug49w7vwaFfyu10F3";
    stest.vveguid[1][0].mUunm = _stestVVEGuid10Uunm;
    char _stestVVEGuid11Uunm[] = "Jhwt4OibjnWibnrwoIjnwt4hBF4598j4098hNt4oihj049Jb4tijbn9Yt4jb6";
    stest.vveguid[1][1].mUunm = _stestVVEGuid11Uunm;

    ITest *itest_;

    assert(!FAILED(CTest::New(&_itest)));
    itest = _itest, _itest->Release();

    assert(!FAILED(itest->TestSelf()));

    assert((n = snprintf(ebuf, _BUF_SIZE, "%p (ITest)\n", itest.Get())) >= 0);

    assert((output = fopen(OUTPUT_FILE, "r")) != NULL);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestVoid()));

    assert((n = snprintf(ebuf, _BUF_SIZE, "void (void)\n")) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestInt16(i16)));

    assert((n = snprintf(ebuf, _BUF_SIZE, "%d (Int16)\n", i16)) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestInt32(i32)));

    assert((n = snprintf(ebuf, _BUF_SIZE, "%d (Int32)\n", i32)) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestInt64(i64)));

    assert((n = snprintf(ebuf, _BUF_SIZE, "%lld (Int64)\n", i64)) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestByte(byte)));

    assert((n = snprintf(ebuf, _BUF_SIZE, "\'%c\' (Byte)\n", byte)) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestFloat(f)));

    assert((n = snprintf(ebuf, _BUF_SIZE, "%f (Float)\n", f)) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestDouble(d)));

    assert((n = snprintf(ebuf, _BUF_SIZE, "%lf (Double)\n", d)) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestChar32(c32)));

    assert((n = snprintf(ebuf, _BUF_SIZE, "\'%lc\' (Char32)\n", c32)) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestString(s)));

    assert((n = snprintf(ebuf, _BUF_SIZE, "\"%s\" (String)\n", static_cast<char const *>(s))) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestBoolean(b)));

    assert((n = snprintf(ebuf, _BUF_SIZE, "%s (Boolean)\n", _GetString(b))) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestEMuid(emuid)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "{\n"
                    "  \"mData1\": %u,\n"
                    "  \"mData2\": %u,\n"
                    "  \"mData3\": %u,\n"
                    "  \"mData4\": [\n"
                    "    %u,\n"
                    "    %u,\n"
                    "    %u,\n"
                    "    %u,\n"
                    "    %u,\n"
                    "    %u,\n"
                    "    %u,\n"
                    "    %u\n"
                    "  ]\n"
                    "} (EMuid)\n",
                    emuid.mData1,
                    emuid.mData2,
                    emuid.mData3,
                    emuid.mData4[0],
                    emuid.mData4[1],
                    emuid.mData4[2],
                    emuid.mData4[3],
                    emuid.mData4[4],
                    emuid.mData4[5],
                    emuid.mData4[6],
                    emuid.mData4[7])) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestEGuid(eguid)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "{\n"
                    "  \"mClsid\": {\n"
                    "    \"mData1\": %u,\n"
                    "    \"mData2\": %u,\n"
                    "    \"mData3\": %u,\n"
                    "    \"mData4\": [\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u\n"
                    "    ]\n"
                    "  },\n"
                    "  \"mUunm\": \"%s\",\n"
                    "  \"mCarcode\": %u,\n"
                    "} (EGuid)\n",
                    eguid.mClsid.mData1,
                    eguid.mClsid.mData2,
                    eguid.mClsid.mData3,
                    eguid.mClsid.mData4[0],
                    eguid.mClsid.mData4[1],
                    eguid.mClsid.mData4[2],
                    eguid.mClsid.mData4[3],
                    eguid.mClsid.mData4[4],
                    eguid.mClsid.mData4[5],
                    eguid.mClsid.mData4[6],
                    eguid.mClsid.mData4[7],
                    eguid.mUunm,
                    eguid.mCarcode)) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestECode(ecode)));

    assert((n = snprintf(ebuf, _BUF_SIZE, "%d (ECode)\n", ecode)) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestLocalPtr(ptr)));

    assert((n = snprintf(ebuf, _BUF_SIZE, "%p (LocalPtr (PVoid))\n", ptr)) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestEnum(etest)));

    assert((n = snprintf(ebuf, _BUF_SIZE, "%s (Enum (ETest))\n", _GetString(etest))) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestCARArray(i16s)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "[\n"
                    "  %d,\n"
                    "  %d\n"
                    "] (ArrayOf<Int16>)\n",
                    i16s[0],
                    i16s[1])) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestCARArray(i32s)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "[\n"
                    "  %d,\n"
                    "  %d\n"
                    "] (ArrayOf<Int32>)\n",
                    i32s[0],
                    i32s[1])) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestCARArray(i64s)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "[\n"
                    "  %lld,\n"
                    "  %lld\n"
                    "] (ArrayOf<Int64>)\n",
                    i64s[0],
                    i64s[1])) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestCARArray(bytes)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "[\n"
                    "  \'%c\',\n"
                    "  \'%c\'\n"
                    "] (ArrayOf<Byte>)\n",
                    bytes[0],
                    bytes[1])) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestCARArray(fs)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "[\n"
                    "  %f,\n"
                    "  %f\n"
                    "] (ArrayOf<Float>)\n",
                    fs[0],
                    fs[1])) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestCARArray(ds)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "[\n"
                    "  %lf,\n"
                    "  %lf\n"
                    "] (ArrayOf<Double>)\n",
                    ds[0],
                    ds[1])) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestCARArrayOfEnum(etests)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "[\n"
                    "  %s,\n"
                    "  %s\n"
                    "] (ArrayOf<ETest>)\n",
                    _GetString(etests[0]),
                    _GetString(etests[1]))) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestCARArray(c32s)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "[\n"
                    "  \'%lc\',\n"
                    "  \'%lc\'\n"
                    "] (ArrayOf<Char32>)\n",
                    c32s[0],
                    c32s[1])) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestCARArray(ss)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "[\n"
                    "  \"%s\",\n"
                    "  \"%s\"\n"
                    "] (ArrayOf<String>)\n",
                    static_cast<char const *>(ss[0]),
                    static_cast<char const *>(ss[1]))) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestCARArrayOfBoolean(bs)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "[\n"
                    "  %s,\n"
                    "  %s\n"
                    "] (ArrayOf<Boolean>)\n",
                    _GetString(bs[0]),
                    _GetString(bs[1]))) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestCARArray(emuids)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "[\n"
                    "  {\n"
                    "    \"mData1\": %u,\n"
                    "    \"mData2\": %u,\n"
                    "    \"mData3\": %u,\n"
                    "    \"mData4\": [\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u\n"
                    "    ]\n"
                    "  },\n"
                    "  {\n"
                    "    \"mData1\": %u,\n"
                    "    \"mData2\": %u,\n"
                    "    \"mData3\": %u,\n"
                    "    \"mData4\": [\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u\n"
                    "    ]\n"
                    "  }\n"
                    "] (ArrayOf<EMuid>)\n",
                    emuids[0].mData1,
                    emuids[0].mData2,
                    emuids[0].mData3,
                    emuids[0].mData4[0],
                    emuids[0].mData4[1],
                    emuids[0].mData4[2],
                    emuids[0].mData4[3],
                    emuids[0].mData4[4],
                    emuids[0].mData4[5],
                    emuids[0].mData4[6],
                    emuids[0].mData4[7],
                    emuids[1].mData1,
                    emuids[1].mData2,
                    emuids[1].mData3,
                    emuids[1].mData4[0],
                    emuids[1].mData4[1],
                    emuids[1].mData4[2],
                    emuids[1].mData4[3],
                    emuids[1].mData4[4],
                    emuids[1].mData4[5],
                    emuids[1].mData4[6],
                    emuids[1].mData4[7])) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestCARArray(eguids)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "[\n"
                    "  {\n"
                    "    \"mClsid\": {\n"
                    "      \"mData1\": %u,\n"
                    "      \"mData2\": %u,\n"
                    "      \"mData3\": %u,\n"
                    "      \"mData4\": [\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u\n"
                    "      ]\n"
                    "    },\n"
                    "    \"mUunm\": \"%s\",\n"
                    "    \"mCarcode\": %u,\n"
                    "  },\n"
                    "  {\n"
                    "    \"mClsid\": {\n"
                    "      \"mData1\": %u,\n"
                    "      \"mData2\": %u,\n"
                    "      \"mData3\": %u,\n"
                    "      \"mData4\": [\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u\n"
                    "      ]\n"
                    "    },\n"
                    "    \"mUunm\": \"%s\",\n"
                    "    \"mCarcode\": %u,\n"
                    "  }\n"
                    "] (ArrayOf<EGuid>)\n",
                    eguids[0].mClsid.mData1,
                    eguids[0].mClsid.mData2,
                    eguids[0].mClsid.mData3,
                    eguids[0].mClsid.mData4[0],
                    eguids[0].mClsid.mData4[1],
                    eguids[0].mClsid.mData4[2],
                    eguids[0].mClsid.mData4[3],
                    eguids[0].mClsid.mData4[4],
                    eguids[0].mClsid.mData4[5],
                    eguids[0].mClsid.mData4[6],
                    eguids[0].mClsid.mData4[7],
                    eguids[0].mUunm,
                    eguids[0].mCarcode,
                    eguids[1].mClsid.mData1,
                    eguids[1].mClsid.mData2,
                    eguids[1].mClsid.mData3,
                    eguids[1].mClsid.mData4[0],
                    eguids[1].mClsid.mData4[1],
                    eguids[1].mClsid.mData4[2],
                    eguids[1].mClsid.mData4[3],
                    eguids[1].mClsid.mData4[4],
                    eguids[1].mClsid.mData4[5],
                    eguids[1].mClsid.mData4[6],
                    eguids[1].mClsid.mData4[7],
                    eguids[1].mUunm,
                    eguids[1].mCarcode)) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestCARArrayOfECode(ecodes)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "[\n"
                    "  %d,\n"
                    "  %d\n"
                    "] (ArrayOf<ECode>)\n",
                    ecodes[0],
                    ecodes[1])) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    _itests[0] = itest;
    _itests[1] = itest;

    assert(!FAILED(itest->TestCARArray(itests)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "[\n"
                    "  %p,\n"
                    "  %p\n"
                    "] (ArrayOf<ITest>)\n",
                    itests[0],
                    itests[1])) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestCARArray(stests)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "[\n"
                    "  {\n"
                    "    \"i16\": %d,\n"
                    "    \"i32\": %d,\n"
                    "    \"i64\": %lld,\n"
                    "    \"byte\": \'%c\',\n"
                    "    \"f\": %f,\n"
                    "    \"d\": %lf,\n"
                    "    \"c32\": \'%lc\',\n"
                    "    \"b\": %s,\n"
                    "    \"emuid\": {\n"
                    "      \"mData1\": %u,\n"
                    "      \"mData2\": %u,\n"
                    "      \"mData3\": %u,\n"
                    "      \"mData4\": [\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u\n"
                    "      ]\n"
                    "    },\n"
                    "    \"eguid\": {\n"
                    "      \"mClsid\": {\n"
                    "        \"mData1\": %u,\n"
                    "        \"mData2\": %u,\n"
                    "        \"mData3\": %u,\n"
                    "        \"mData4\": [\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u\n"
                    "        ]\n"
                    "      },\n"
                    "      \"mUunm\": \"%s\",\n"
                    "      \"mCarcode\": %u,\n"
                    "    },\n"
                    "    \"ecode\": %d,\n"
                    "    \"ptr\": %p,\n"
                    "    \"etest\": %s,\n"
                    "    \"stest\": {\n"
                    "      \"i16\": %d\n"
                    "    }\n"
                    "    \"vi16\": [\n"
                    "      %d,\n"
                    "      %d\n"
                    "    ],\n"
                    "    \"vi32\": [\n"
                    "      %d,\n"
                    "      %d\n"
                    "    ],\n"
                    "    \"vi64\": [\n"
                    "      %lld,\n"
                    "      %lld\n"
                    "    ],\n"
                    "    \"vbyte\": [\n"
                    "      \'%c\',\n"
                    "      \'%c\'\n"
                    "    ],\n"
                    "    \"vf\": [\n"
                    "      %f,\n"
                    "      %f\n"
                    "    ],\n"
                    "    \"vd\": [\n"
                    "      %lf,\n"
                    "      %lf\n"
                    "    ],\n"
                    "    \"vc32\": [\n"
                    "      \'%lc\',\n"
                    "      \'%lc\'\n"
                    "    ],\n"
                    "    \"vb\": [\n"
                    "      %s,\n"
                    "      %s\n"
                    "    ],\n"
                    "    \"vemuid\": [\n"
                    "      {\n"
                    "        \"mData1\": %u,\n"
                    "        \"mData2\": %u,\n"
                    "        \"mData3\": %u,\n"
                    "        \"mData4\": [\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u\n"
                    "        ]\n"
                    "      },\n"
                    "      {\n"
                    "        \"mData1\": %u,\n"
                    "        \"mData2\": %u,\n"
                    "        \"mData3\": %u,\n"
                    "        \"mData4\": [\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u\n"
                    "        ]\n"
                    "      }\n"
                    "    ],\n"
                    "    \"veguid\": [\n"
                    "      {\n"
                    "        \"mClsid\": {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        },\n"
                    "        \"mUunm\": \"%s\",\n"
                    "        \"mCarcode\": %u,\n"
                    "      },\n"
                    "      {\n"
                    "        \"mClsid\": {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        },\n"
                    "        \"mUunm\": \"%s\",\n"
                    "        \"mCarcode\": %u,\n"
                    "      }\n"
                    "    ],\n"
                    "    \"vecode\": [\n"
                    "      %d,\n"
                    "      %d\n"
                    "    ],\n"
                    "    \"vptr\": [\n"
                    "      %p,\n"
                    "      %p\n"
                    "    ],\n"
                    "    \"vetest\": [\n"
                    "      %s,\n"
                    "      %s\n"
                    "    ],\n"
                    "    \"vstest\": [\n"
                    "      {\n"
                    "        \"i16\": %d\n"
                    "      },\n"
                    "      {\n"
                    "        \"i16\": %d\n"
                    "      }\n"
                    "    ],\n"
                    "    \"vvi16\": [\n"
                    "      [\n"
                    "        %d,\n"
                    "        %d\n"
                    "      ],\n"
                    "      [\n"
                    "        %d,\n"
                    "        %d\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvi32\": [\n"
                    "      [\n"
                    "        %d,\n"
                    "        %d\n"
                    "      ],\n"
                    "      [\n"
                    "        %d,\n"
                    "        %d\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvi64\": [\n"
                    "      [\n"
                    "        %lld,\n"
                    "        %lld\n"
                    "      ],\n"
                    "      [\n"
                    "        %lld,\n"
                    "        %lld\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvbyte\": [\n"
                    "      [\n"
                    "        \'%c\',\n"
                    "        \'%c\'\n"
                    "      ],\n"
                    "      [\n"
                    "        \'%c\',\n"
                    "        \'%c\'\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvf\": [\n"
                    "      [\n"
                    "        %f,\n"
                    "        %f\n"
                    "      ],\n"
                    "      [\n"
                    "        %f,\n"
                    "        %f\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvd\": [\n"
                    "      [\n"
                    "        %lf,\n"
                    "        %lf\n"
                    "      ],\n"
                    "      [\n"
                    "        %lf,\n"
                    "        %lf\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvc32\": [\n"
                    "      [\n"
                    "        \'%c\',\n"
                    "        \'%c\'\n"
                    "      ],\n"
                    "      [\n"
                    "        \'%c\',\n"
                    "        \'%c\'\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvb\": [\n"
                    "      [\n"
                    "        %s,\n"
                    "        %s\n"
                    "      ],\n"
                    "      [\n"
                    "        %s,\n"
                    "        %s\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvemuid\": [\n"
                    "      [\n"
                    "        {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        },\n"
                    "        {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        }\n"
                    "      ],\n"
                    "      [\n"
                    "        {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        },\n"
                    "        {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        }\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vveguid\": [\n"
                    "      [\n"
                    "        {\n"
                    "          \"mClsid\": {\n"
                    "            \"mData1\": %u,\n"
                    "            \"mData2\": %u,\n"
                    "            \"mData3\": %u,\n"
                    "            \"mData4\": [\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u\n"
                    "            ]\n"
                    "          },\n"
                    "          \"mUunm\": \"%s\",\n"
                    "          \"mCarcode\": %u,\n"
                    "        },\n"
                    "        {\n"
                    "          \"mClsid\": {\n"
                    "            \"mData1\": %u,\n"
                    "            \"mData2\": %u,\n"
                    "            \"mData3\": %u,\n"
                    "            \"mData4\": [\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u\n"
                    "            ]\n"
                    "          },\n"
                    "          \"mUunm\": \"%s\",\n"
                    "          \"mCarcode\": %u,\n"
                    "        }\n"
                    "      ],\n"
                    "      [\n"
                    "        {\n"
                    "          \"mClsid\": {\n"
                    "            \"mData1\": %u,\n"
                    "            \"mData2\": %u,\n"
                    "            \"mData3\": %u,\n"
                    "            \"mData4\": [\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u\n"
                    "            ]\n"
                    "          },\n"
                    "          \"mUunm\": \"%s\",\n"
                    "          \"mCarcode\": %u,\n"
                    "        },\n"
                    "        {\n"
                    "          \"mClsid\": {\n"
                    "            \"mData1\": %u,\n"
                    "            \"mData2\": %u,\n"
                    "            \"mData3\": %u,\n"
                    "            \"mData4\": [\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u\n"
                    "            ]\n"
                    "          },\n"
                    "          \"mUunm\": \"%s\",\n"
                    "          \"mCarcode\": %u,\n"
                    "        }\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvecode\": [\n"
                    "      [\n"
                    "        %d,\n"
                    "        %d\n"
                    "      ],\n"
                    "      [\n"
                    "        %d,\n"
                    "        %d\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvptr\": [\n"
                    "      [\n"
                    "        %p,\n"
                    "        %p\n"
                    "      ],\n"
                    "      [\n"
                    "        %p,\n"
                    "        %p\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvetest\": [\n"
                    "      [\n"
                    "        %s,\n"
                    "        %s\n"
                    "      ],\n"
                    "      [\n"
                    "        %s,\n"
                    "        %s\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvstest\": [\n"
                    "      [\n"
                    "        {\n"
                    "          \"i16\": %d\n"
                    "        },\n"
                    "        {\n"
                    "          \"i16\": %d\n"
                    "        }\n"
                    "      ],\n"
                    "      [\n"
                    "        {\n"
                    "          \"i16\": %d\n"
                    "        },\n"
                    "        {\n"
                    "          \"i16\": %d\n"
                    "        }\n"
                    "      ]\n"
                    "    ]\n"
                    "  },\n"
                    "  {\n"
                    "    \"i16\": %d,\n"
                    "    \"i32\": %d,\n"
                    "    \"i64\": %lld,\n"
                    "    \"byte\": \'%c\',\n"
                    "    \"f\": %f,\n"
                    "    \"d\": %lf,\n"
                    "    \"c32\": \'%lc\',\n"
                    "    \"b\": %s,\n"
                    "    \"emuid\": {\n"
                    "      \"mData1\": %u,\n"
                    "      \"mData2\": %u,\n"
                    "      \"mData3\": %u,\n"
                    "      \"mData4\": [\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u\n"
                    "      ]\n"
                    "    },\n"
                    "    \"eguid\": {\n"
                    "      \"mClsid\": {\n"
                    "        \"mData1\": %u,\n"
                    "        \"mData2\": %u,\n"
                    "        \"mData3\": %u,\n"
                    "        \"mData4\": [\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u\n"
                    "        ]\n"
                    "      },\n"
                    "      \"mUunm\": \"%s\",\n"
                    "      \"mCarcode\": %u,\n"
                    "    },\n"
                    "    \"ecode\": %d,\n"
                    "    \"ptr\": %p,\n"
                    "    \"etest\": %s,\n"
                    "    \"stest\": {\n"
                    "      \"i16\": %d\n"
                    "    }\n"
                    "    \"vi16\": [\n"
                    "      %d,\n"
                    "      %d\n"
                    "    ],\n"
                    "    \"vi32\": [\n"
                    "      %d,\n"
                    "      %d\n"
                    "    ],\n"
                    "    \"vi64\": [\n"
                    "      %lld,\n"
                    "      %lld\n"
                    "    ],\n"
                    "    \"vbyte\": [\n"
                    "      \'%c\',\n"
                    "      \'%c\'\n"
                    "    ],\n"
                    "    \"vf\": [\n"
                    "      %f,\n"
                    "      %f\n"
                    "    ],\n"
                    "    \"vd\": [\n"
                    "      %lf,\n"
                    "      %lf\n"
                    "    ],\n"
                    "    \"vc32\": [\n"
                    "      \'%lc\',\n"
                    "      \'%lc\'\n"
                    "    ],\n"
                    "    \"vb\": [\n"
                    "      %s,\n"
                    "      %s\n"
                    "    ],\n"
                    "    \"vemuid\": [\n"
                    "      {\n"
                    "        \"mData1\": %u,\n"
                    "        \"mData2\": %u,\n"
                    "        \"mData3\": %u,\n"
                    "        \"mData4\": [\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u\n"
                    "        ]\n"
                    "      },\n"
                    "      {\n"
                    "        \"mData1\": %u,\n"
                    "        \"mData2\": %u,\n"
                    "        \"mData3\": %u,\n"
                    "        \"mData4\": [\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u\n"
                    "        ]\n"
                    "      }\n"
                    "    ],\n"
                    "    \"veguid\": [\n"
                    "      {\n"
                    "        \"mClsid\": {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        },\n"
                    "        \"mUunm\": \"%s\",\n"
                    "        \"mCarcode\": %u,\n"
                    "      },\n"
                    "      {\n"
                    "        \"mClsid\": {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        },\n"
                    "        \"mUunm\": \"%s\",\n"
                    "        \"mCarcode\": %u,\n"
                    "      }\n"
                    "    ],\n"
                    "    \"vecode\": [\n"
                    "      %d,\n"
                    "      %d\n"
                    "    ],\n"
                    "    \"vptr\": [\n"
                    "      %p,\n"
                    "      %p\n"
                    "    ],\n"
                    "    \"vetest\": [\n"
                    "      %s,\n"
                    "      %s\n"
                    "    ],\n"
                    "    \"vstest\": [\n"
                    "      {\n"
                    "        \"i16\": %d\n"
                    "      },\n"
                    "      {\n"
                    "        \"i16\": %d\n"
                    "      }\n"
                    "    ],\n"
                    "    \"vvi16\": [\n"
                    "      [\n"
                    "        %d,\n"
                    "        %d\n"
                    "      ],\n"
                    "      [\n"
                    "        %d,\n"
                    "        %d\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvi32\": [\n"
                    "      [\n"
                    "        %d,\n"
                    "        %d\n"
                    "      ],\n"
                    "      [\n"
                    "        %d,\n"
                    "        %d\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvi64\": [\n"
                    "      [\n"
                    "        %lld,\n"
                    "        %lld\n"
                    "      ],\n"
                    "      [\n"
                    "        %lld,\n"
                    "        %lld\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvbyte\": [\n"
                    "      [\n"
                    "        \'%c\',\n"
                    "        \'%c\'\n"
                    "      ],\n"
                    "      [\n"
                    "        \'%c\',\n"
                    "        \'%c\'\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvf\": [\n"
                    "      [\n"
                    "        %f,\n"
                    "        %f\n"
                    "      ],\n"
                    "      [\n"
                    "        %f,\n"
                    "        %f\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvd\": [\n"
                    "      [\n"
                    "        %lf,\n"
                    "        %lf\n"
                    "      ],\n"
                    "      [\n"
                    "        %lf,\n"
                    "        %lf\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvc32\": [\n"
                    "      [\n"
                    "        \'%c\',\n"
                    "        \'%c\'\n"
                    "      ],\n"
                    "      [\n"
                    "        \'%c\',\n"
                    "        \'%c\'\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvb\": [\n"
                    "      [\n"
                    "        %s,\n"
                    "        %s\n"
                    "      ],\n"
                    "      [\n"
                    "        %s,\n"
                    "        %s\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvemuid\": [\n"
                    "      [\n"
                    "        {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        },\n"
                    "        {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        }\n"
                    "      ],\n"
                    "      [\n"
                    "        {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        },\n"
                    "        {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        }\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vveguid\": [\n"
                    "      [\n"
                    "        {\n"
                    "          \"mClsid\": {\n"
                    "            \"mData1\": %u,\n"
                    "            \"mData2\": %u,\n"
                    "            \"mData3\": %u,\n"
                    "            \"mData4\": [\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u\n"
                    "            ]\n"
                    "          },\n"
                    "          \"mUunm\": \"%s\",\n"
                    "          \"mCarcode\": %u,\n"
                    "        },\n"
                    "        {\n"
                    "          \"mClsid\": {\n"
                    "            \"mData1\": %u,\n"
                    "            \"mData2\": %u,\n"
                    "            \"mData3\": %u,\n"
                    "            \"mData4\": [\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u\n"
                    "            ]\n"
                    "          },\n"
                    "          \"mUunm\": \"%s\",\n"
                    "          \"mCarcode\": %u,\n"
                    "        }\n"
                    "      ],\n"
                    "      [\n"
                    "        {\n"
                    "          \"mClsid\": {\n"
                    "            \"mData1\": %u,\n"
                    "            \"mData2\": %u,\n"
                    "            \"mData3\": %u,\n"
                    "            \"mData4\": [\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u\n"
                    "            ]\n"
                    "          },\n"
                    "          \"mUunm\": \"%s\",\n"
                    "          \"mCarcode\": %u,\n"
                    "        },\n"
                    "        {\n"
                    "          \"mClsid\": {\n"
                    "            \"mData1\": %u,\n"
                    "            \"mData2\": %u,\n"
                    "            \"mData3\": %u,\n"
                    "            \"mData4\": [\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u,\n"
                    "              %u\n"
                    "            ]\n"
                    "          },\n"
                    "          \"mUunm\": \"%s\",\n"
                    "          \"mCarcode\": %u,\n"
                    "        }\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvecode\": [\n"
                    "      [\n"
                    "        %d,\n"
                    "        %d\n"
                    "      ],\n"
                    "      [\n"
                    "        %d,\n"
                    "        %d\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvptr\": [\n"
                    "      [\n"
                    "        %p,\n"
                    "        %p\n"
                    "      ],\n"
                    "      [\n"
                    "        %p,\n"
                    "        %p\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvetest\": [\n"
                    "      [\n"
                    "        %s,\n"
                    "        %s\n"
                    "      ],\n"
                    "      [\n"
                    "        %s,\n"
                    "        %s\n"
                    "      ]\n"
                    "    ],\n"
                    "    \"vvstest\": [\n"
                    "      [\n"
                    "        {\n"
                    "          \"i16\": %d\n"
                    "        },\n"
                    "        {\n"
                    "          \"i16\": %d\n"
                    "        }\n"
                    "      ],\n"
                    "      [\n"
                    "        {\n"
                    "          \"i16\": %d\n"
                    "        },\n"
                    "        {\n"
                    "          \"i16\": %d\n"
                    "        }\n"
                    "      ]\n"
                    "    ]\n"
                    "  }\n"
                    "] (ArrayOf<STest>)\n",
                    stests[0].i16,
                    stests[0].i32,
                    stests[0].i64,
                    stests[0].byte,
                    stests[0].f,
                    stests[0].d,
                    stests[0].c32,
                    _GetString(stests[0].b),
                    stests[0].emuid.mData1,
                    stests[0].emuid.mData2,
                    stests[0].emuid.mData3,
                    stests[0].emuid.mData4[0],
                    stests[0].emuid.mData4[1],
                    stests[0].emuid.mData4[2],
                    stests[0].emuid.mData4[3],
                    stests[0].emuid.mData4[4],
                    stests[0].emuid.mData4[5],
                    stests[0].emuid.mData4[6],
                    stests[0].emuid.mData4[7],
                    stests[0].eguid.mClsid.mData1,
                    stests[0].eguid.mClsid.mData2,
                    stests[0].eguid.mClsid.mData3,
                    stests[0].eguid.mClsid.mData4[0],
                    stests[0].eguid.mClsid.mData4[1],
                    stests[0].eguid.mClsid.mData4[2],
                    stests[0].eguid.mClsid.mData4[3],
                    stests[0].eguid.mClsid.mData4[4],
                    stests[0].eguid.mClsid.mData4[5],
                    stests[0].eguid.mClsid.mData4[6],
                    stests[0].eguid.mClsid.mData4[7],
                    stests[0].eguid.mUunm,
                    stests[0].eguid.mCarcode,
                    stests[0].ecode,
                    stests[0].ptr,
                    _GetString(stests[0].etest),
                    stests[0].stest.i16,
                    stests[0].vi16[0],
                    stests[0].vi16[1],
                    stests[0].vi32[0],
                    stests[0].vi32[1],
                    stests[0].vi64[0],
                    stests[0].vi64[1],
                    stests[0].vbyte[0],
                    stests[0].vbyte[1],
                    stests[0].vf[0],
                    stests[0].vf[1],
                    stests[0].vd[0],
                    stests[0].vd[1],
                    stests[0].vc32[0],
                    stests[0].vc32[1],
                    _GetString(stests[0].vb[0]),
                    _GetString(stests[0].vb[1]),
                    stests[0].vemuid[0].mData1,
                    stests[0].vemuid[0].mData2,
                    stests[0].vemuid[0].mData3,
                    stests[0].vemuid[0].mData4[0],
                    stests[0].vemuid[0].mData4[1],
                    stests[0].vemuid[0].mData4[2],
                    stests[0].vemuid[0].mData4[3],
                    stests[0].vemuid[0].mData4[4],
                    stests[0].vemuid[0].mData4[5],
                    stests[0].vemuid[0].mData4[6],
                    stests[0].vemuid[0].mData4[7],
                    stests[0].vemuid[1].mData1,
                    stests[0].vemuid[1].mData2,
                    stests[0].vemuid[1].mData3,
                    stests[0].vemuid[1].mData4[0],
                    stests[0].vemuid[1].mData4[1],
                    stests[0].vemuid[1].mData4[2],
                    stests[0].vemuid[1].mData4[3],
                    stests[0].vemuid[1].mData4[4],
                    stests[0].vemuid[1].mData4[5],
                    stests[0].vemuid[1].mData4[6],
                    stests[0].vemuid[1].mData4[7],
                    stests[0].veguid[0].mClsid.mData1,
                    stests[0].veguid[0].mClsid.mData2,
                    stests[0].veguid[0].mClsid.mData3,
                    stests[0].veguid[0].mClsid.mData4[0],
                    stests[0].veguid[0].mClsid.mData4[1],
                    stests[0].veguid[0].mClsid.mData4[2],
                    stests[0].veguid[0].mClsid.mData4[3],
                    stests[0].veguid[0].mClsid.mData4[4],
                    stests[0].veguid[0].mClsid.mData4[5],
                    stests[0].veguid[0].mClsid.mData4[6],
                    stests[0].veguid[0].mClsid.mData4[7],
                    stests[0].veguid[0].mUunm,
                    stests[0].veguid[0].mCarcode,
                    stests[0].veguid[1].mClsid.mData1,
                    stests[0].veguid[1].mClsid.mData2,
                    stests[0].veguid[1].mClsid.mData3,
                    stests[0].veguid[1].mClsid.mData4[0],
                    stests[0].veguid[1].mClsid.mData4[1],
                    stests[0].veguid[1].mClsid.mData4[2],
                    stests[0].veguid[1].mClsid.mData4[3],
                    stests[0].veguid[1].mClsid.mData4[4],
                    stests[0].veguid[1].mClsid.mData4[5],
                    stests[0].veguid[1].mClsid.mData4[6],
                    stests[0].veguid[1].mClsid.mData4[7],
                    stests[0].veguid[1].mUunm,
                    stests[0].veguid[1].mCarcode,
                    stests[0].vecode[0],
                    stests[0].vecode[1],
                    stests[0].vptr[0],
                    stests[0].vptr[1],
                    _GetString(stests[0].vetest[0]),
                    _GetString(stests[0].vetest[1]),
                    stests[0].vstest[0].i16,
                    stests[0].vstest[1].i16,
                    stests[0].vvi16[0][0],
                    stests[0].vvi16[0][1],
                    stests[0].vvi16[1][0],
                    stests[0].vvi16[1][1],
                    stests[0].vvi32[0][0],
                    stests[0].vvi32[0][1],
                    stests[0].vvi32[1][0],
                    stests[0].vvi32[1][1],
                    stests[0].vvi64[0][0],
                    stests[0].vvi64[0][1],
                    stests[0].vvi64[1][0],
                    stests[0].vvi64[1][1],
                    stests[0].vvbyte[0][0],
                    stests[0].vvbyte[0][1],
                    stests[0].vvbyte[1][0],
                    stests[0].vvbyte[1][1],
                    stests[0].vvf[0][0],
                    stests[0].vvf[0][1],
                    stests[0].vvf[1][0],
                    stests[0].vvf[1][1],
                    stests[0].vvd[0][0],
                    stests[0].vvd[0][1],
                    stests[0].vvd[1][0],
                    stests[0].vvd[1][1],
                    stests[0].vvc32[0][0],
                    stests[0].vvc32[0][1],
                    stests[0].vvc32[1][0],
                    stests[0].vvc32[1][1],
                    _GetString(stests[0].vvb[0][0]),
                    _GetString(stests[0].vvb[0][1]),
                    _GetString(stests[0].vvb[1][0]),
                    _GetString(stests[0].vvb[1][1]),
                    stests[0].vvemuid[0][0].mData1,
                    stests[0].vvemuid[0][0].mData2,
                    stests[0].vvemuid[0][0].mData3,
                    stests[0].vvemuid[0][0].mData4[0],
                    stests[0].vvemuid[0][0].mData4[1],
                    stests[0].vvemuid[0][0].mData4[2],
                    stests[0].vvemuid[0][0].mData4[3],
                    stests[0].vvemuid[0][0].mData4[4],
                    stests[0].vvemuid[0][0].mData4[5],
                    stests[0].vvemuid[0][0].mData4[6],
                    stests[0].vvemuid[0][0].mData4[7],
                    stests[0].vvemuid[0][1].mData1,
                    stests[0].vvemuid[0][1].mData2,
                    stests[0].vvemuid[0][1].mData3,
                    stests[0].vvemuid[0][1].mData4[0],
                    stests[0].vvemuid[0][1].mData4[1],
                    stests[0].vvemuid[0][1].mData4[2],
                    stests[0].vvemuid[0][1].mData4[3],
                    stests[0].vvemuid[0][1].mData4[4],
                    stests[0].vvemuid[0][1].mData4[5],
                    stests[0].vvemuid[0][1].mData4[6],
                    stests[0].vvemuid[0][1].mData4[7],
                    stests[0].vvemuid[1][0].mData1,
                    stests[0].vvemuid[1][0].mData2,
                    stests[0].vvemuid[1][0].mData3,
                    stests[0].vvemuid[1][0].mData4[0],
                    stests[0].vvemuid[1][0].mData4[1],
                    stests[0].vvemuid[1][0].mData4[2],
                    stests[0].vvemuid[1][0].mData4[3],
                    stests[0].vvemuid[1][0].mData4[4],
                    stests[0].vvemuid[1][0].mData4[5],
                    stests[0].vvemuid[1][0].mData4[6],
                    stests[0].vvemuid[1][0].mData4[7],
                    stests[0].vvemuid[1][1].mData1,
                    stests[0].vvemuid[1][1].mData2,
                    stests[0].vvemuid[1][1].mData3,
                    stests[0].vvemuid[1][1].mData4[0],
                    stests[0].vvemuid[1][1].mData4[1],
                    stests[0].vvemuid[1][1].mData4[2],
                    stests[0].vvemuid[1][1].mData4[3],
                    stests[0].vvemuid[1][1].mData4[4],
                    stests[0].vvemuid[1][1].mData4[5],
                    stests[0].vvemuid[1][1].mData4[6],
                    stests[0].vvemuid[1][1].mData4[7],
                    stests[0].vveguid[0][0].mClsid.mData1,
                    stests[0].vveguid[0][0].mClsid.mData2,
                    stests[0].vveguid[0][0].mClsid.mData3,
                    stests[0].vveguid[0][0].mClsid.mData4[0],
                    stests[0].vveguid[0][0].mClsid.mData4[1],
                    stests[0].vveguid[0][0].mClsid.mData4[2],
                    stests[0].vveguid[0][0].mClsid.mData4[3],
                    stests[0].vveguid[0][0].mClsid.mData4[4],
                    stests[0].vveguid[0][0].mClsid.mData4[5],
                    stests[0].vveguid[0][0].mClsid.mData4[6],
                    stests[0].vveguid[0][0].mClsid.mData4[7],
                    stests[0].vveguid[0][0].mUunm,
                    stests[0].vveguid[0][0].mCarcode,
                    stests[0].vveguid[0][1].mClsid.mData1,
                    stests[0].vveguid[0][1].mClsid.mData2,
                    stests[0].vveguid[0][1].mClsid.mData3,
                    stests[0].vveguid[0][1].mClsid.mData4[0],
                    stests[0].vveguid[0][1].mClsid.mData4[1],
                    stests[0].vveguid[0][1].mClsid.mData4[2],
                    stests[0].vveguid[0][1].mClsid.mData4[3],
                    stests[0].vveguid[0][1].mClsid.mData4[4],
                    stests[0].vveguid[0][1].mClsid.mData4[5],
                    stests[0].vveguid[0][1].mClsid.mData4[6],
                    stests[0].vveguid[0][1].mClsid.mData4[7],
                    stests[0].vveguid[0][1].mUunm,
                    stests[0].vveguid[0][1].mCarcode,
                    stests[0].vveguid[1][0].mClsid.mData1,
                    stests[0].vveguid[1][0].mClsid.mData2,
                    stests[0].vveguid[1][0].mClsid.mData3,
                    stests[0].vveguid[1][0].mClsid.mData4[0],
                    stests[0].vveguid[1][0].mClsid.mData4[1],
                    stests[0].vveguid[1][0].mClsid.mData4[2],
                    stests[0].vveguid[1][0].mClsid.mData4[3],
                    stests[0].vveguid[1][0].mClsid.mData4[4],
                    stests[0].vveguid[1][0].mClsid.mData4[5],
                    stests[0].vveguid[1][0].mClsid.mData4[6],
                    stests[0].vveguid[1][0].mClsid.mData4[7],
                    stests[0].vveguid[1][0].mUunm,
                    stests[0].vveguid[1][0].mCarcode,
                    stests[0].vveguid[1][1].mClsid.mData1,
                    stests[0].vveguid[1][1].mClsid.mData2,
                    stests[0].vveguid[1][1].mClsid.mData3,
                    stests[0].vveguid[1][1].mClsid.mData4[0],
                    stests[0].vveguid[1][1].mClsid.mData4[1],
                    stests[0].vveguid[1][1].mClsid.mData4[2],
                    stests[0].vveguid[1][1].mClsid.mData4[3],
                    stests[0].vveguid[1][1].mClsid.mData4[4],
                    stests[0].vveguid[1][1].mClsid.mData4[5],
                    stests[0].vveguid[1][1].mClsid.mData4[6],
                    stests[0].vveguid[1][1].mClsid.mData4[7],
                    stests[0].vveguid[1][1].mUunm,
                    stests[0].vveguid[1][1].mCarcode,
                    stests[0].vvecode[0][0],
                    stests[0].vvecode[0][1],
                    stests[0].vvecode[1][0],
                    stests[0].vvecode[1][1],
                    stests[0].vvptr[0][0],
                    stests[0].vvptr[0][1],
                    stests[0].vvptr[1][0],
                    stests[0].vvptr[1][1],
                    _GetString(stests[0].vvetest[0][0]),
                    _GetString(stests[0].vvetest[0][1]),
                    _GetString(stests[0].vvetest[1][0]),
                    _GetString(stests[0].vvetest[1][1]),
                    stests[0].vvstest[0][0].i16,
                    stests[0].vvstest[0][1].i16,
                    stests[0].vvstest[1][0].i16,
                    stests[0].vvstest[1][1].i16,
                    stests[1].i16,
                    stests[1].i32,
                    stests[1].i64,
                    stests[1].byte,
                    stests[1].f,
                    stests[1].d,
                    stests[1].c32,
                    _GetString(stests[1].b),
                    stests[1].emuid.mData1,
                    stests[1].emuid.mData2,
                    stests[1].emuid.mData3,
                    stests[1].emuid.mData4[0],
                    stests[1].emuid.mData4[1],
                    stests[1].emuid.mData4[2],
                    stests[1].emuid.mData4[3],
                    stests[1].emuid.mData4[4],
                    stests[1].emuid.mData4[5],
                    stests[1].emuid.mData4[6],
                    stests[1].emuid.mData4[7],
                    stests[1].eguid.mClsid.mData1,
                    stests[1].eguid.mClsid.mData2,
                    stests[1].eguid.mClsid.mData3,
                    stests[1].eguid.mClsid.mData4[0],
                    stests[1].eguid.mClsid.mData4[1],
                    stests[1].eguid.mClsid.mData4[2],
                    stests[1].eguid.mClsid.mData4[3],
                    stests[1].eguid.mClsid.mData4[4],
                    stests[1].eguid.mClsid.mData4[5],
                    stests[1].eguid.mClsid.mData4[6],
                    stests[1].eguid.mClsid.mData4[7],
                    stests[1].eguid.mUunm,
                    stests[1].eguid.mCarcode,
                    stests[1].ecode,
                    stests[1].ptr,
                    _GetString(stests[1].etest),
                    stests[1].stest.i16,
                    stests[1].vi16[0],
                    stests[1].vi16[1],
                    stests[1].vi32[0],
                    stests[1].vi32[1],
                    stests[1].vi64[0],
                    stests[1].vi64[1],
                    stests[1].vbyte[0],
                    stests[1].vbyte[1],
                    stests[1].vf[0],
                    stests[1].vf[1],
                    stests[1].vd[0],
                    stests[1].vd[1],
                    stests[1].vc32[0],
                    stests[1].vc32[1],
                    _GetString(stests[1].vb[0]),
                    _GetString(stests[1].vb[1]),
                    stests[1].vemuid[0].mData1,
                    stests[1].vemuid[0].mData2,
                    stests[1].vemuid[0].mData3,
                    stests[1].vemuid[0].mData4[0],
                    stests[1].vemuid[0].mData4[1],
                    stests[1].vemuid[0].mData4[2],
                    stests[1].vemuid[0].mData4[3],
                    stests[1].vemuid[0].mData4[4],
                    stests[1].vemuid[0].mData4[5],
                    stests[1].vemuid[0].mData4[6],
                    stests[1].vemuid[0].mData4[7],
                    stests[1].vemuid[1].mData1,
                    stests[1].vemuid[1].mData2,
                    stests[1].vemuid[1].mData3,
                    stests[1].vemuid[1].mData4[0],
                    stests[1].vemuid[1].mData4[1],
                    stests[1].vemuid[1].mData4[2],
                    stests[1].vemuid[1].mData4[3],
                    stests[1].vemuid[1].mData4[4],
                    stests[1].vemuid[1].mData4[5],
                    stests[1].vemuid[1].mData4[6],
                    stests[1].vemuid[1].mData4[7],
                    stests[1].veguid[0].mClsid.mData1,
                    stests[1].veguid[0].mClsid.mData2,
                    stests[1].veguid[0].mClsid.mData3,
                    stests[1].veguid[0].mClsid.mData4[0],
                    stests[1].veguid[0].mClsid.mData4[1],
                    stests[1].veguid[0].mClsid.mData4[2],
                    stests[1].veguid[0].mClsid.mData4[3],
                    stests[1].veguid[0].mClsid.mData4[4],
                    stests[1].veguid[0].mClsid.mData4[5],
                    stests[1].veguid[0].mClsid.mData4[6],
                    stests[1].veguid[0].mClsid.mData4[7],
                    stests[1].veguid[0].mUunm,
                    stests[1].veguid[0].mCarcode,
                    stests[1].veguid[1].mClsid.mData1,
                    stests[1].veguid[1].mClsid.mData2,
                    stests[1].veguid[1].mClsid.mData3,
                    stests[1].veguid[1].mClsid.mData4[0],
                    stests[1].veguid[1].mClsid.mData4[1],
                    stests[1].veguid[1].mClsid.mData4[2],
                    stests[1].veguid[1].mClsid.mData4[3],
                    stests[1].veguid[1].mClsid.mData4[4],
                    stests[1].veguid[1].mClsid.mData4[5],
                    stests[1].veguid[1].mClsid.mData4[6],
                    stests[1].veguid[1].mClsid.mData4[7],
                    stests[1].veguid[1].mUunm,
                    stests[1].veguid[1].mCarcode,
                    stests[1].vecode[0],
                    stests[1].vecode[1],
                    stests[1].vptr[0],
                    stests[1].vptr[1],
                    _GetString(stests[1].vetest[0]),
                    _GetString(stests[1].vetest[1]),
                    stests[1].vstest[0].i16,
                    stests[1].vstest[1].i16,
                    stests[1].vvi16[0][0],
                    stests[1].vvi16[0][1],
                    stests[1].vvi16[1][0],
                    stests[1].vvi16[1][1],
                    stests[1].vvi32[0][0],
                    stests[1].vvi32[0][1],
                    stests[1].vvi32[1][0],
                    stests[1].vvi32[1][1],
                    stests[1].vvi64[0][0],
                    stests[1].vvi64[0][1],
                    stests[1].vvi64[1][0],
                    stests[1].vvi64[1][1],
                    stests[1].vvbyte[0][0],
                    stests[1].vvbyte[0][1],
                    stests[1].vvbyte[1][0],
                    stests[1].vvbyte[1][1],
                    stests[1].vvf[0][0],
                    stests[1].vvf[0][1],
                    stests[1].vvf[1][0],
                    stests[1].vvf[1][1],
                    stests[1].vvd[0][0],
                    stests[1].vvd[0][1],
                    stests[1].vvd[1][0],
                    stests[1].vvd[1][1],
                    stests[1].vvc32[0][0],
                    stests[1].vvc32[0][1],
                    stests[1].vvc32[1][0],
                    stests[1].vvc32[1][1],
                    _GetString(stests[1].vvb[0][0]),
                    _GetString(stests[1].vvb[0][1]),
                    _GetString(stests[1].vvb[1][0]),
                    _GetString(stests[1].vvb[1][1]),
                    stests[1].vvemuid[0][0].mData1,
                    stests[1].vvemuid[0][0].mData2,
                    stests[1].vvemuid[0][0].mData3,
                    stests[1].vvemuid[0][0].mData4[0],
                    stests[1].vvemuid[0][0].mData4[1],
                    stests[1].vvemuid[0][0].mData4[2],
                    stests[1].vvemuid[0][0].mData4[3],
                    stests[1].vvemuid[0][0].mData4[4],
                    stests[1].vvemuid[0][0].mData4[5],
                    stests[1].vvemuid[0][0].mData4[6],
                    stests[1].vvemuid[0][0].mData4[7],
                    stests[1].vvemuid[0][1].mData1,
                    stests[1].vvemuid[0][1].mData2,
                    stests[1].vvemuid[0][1].mData3,
                    stests[1].vvemuid[0][1].mData4[0],
                    stests[1].vvemuid[0][1].mData4[1],
                    stests[1].vvemuid[0][1].mData4[2],
                    stests[1].vvemuid[0][1].mData4[3],
                    stests[1].vvemuid[0][1].mData4[4],
                    stests[1].vvemuid[0][1].mData4[5],
                    stests[1].vvemuid[0][1].mData4[6],
                    stests[1].vvemuid[0][1].mData4[7],
                    stests[1].vvemuid[1][0].mData1,
                    stests[1].vvemuid[1][0].mData2,
                    stests[1].vvemuid[1][0].mData3,
                    stests[1].vvemuid[1][0].mData4[0],
                    stests[1].vvemuid[1][0].mData4[1],
                    stests[1].vvemuid[1][0].mData4[2],
                    stests[1].vvemuid[1][0].mData4[3],
                    stests[1].vvemuid[1][0].mData4[4],
                    stests[1].vvemuid[1][0].mData4[5],
                    stests[1].vvemuid[1][0].mData4[6],
                    stests[1].vvemuid[1][0].mData4[7],
                    stests[1].vvemuid[1][1].mData1,
                    stests[1].vvemuid[1][1].mData2,
                    stests[1].vvemuid[1][1].mData3,
                    stests[1].vvemuid[1][1].mData4[0],
                    stests[1].vvemuid[1][1].mData4[1],
                    stests[1].vvemuid[1][1].mData4[2],
                    stests[1].vvemuid[1][1].mData4[3],
                    stests[1].vvemuid[1][1].mData4[4],
                    stests[1].vvemuid[1][1].mData4[5],
                    stests[1].vvemuid[1][1].mData4[6],
                    stests[1].vvemuid[1][1].mData4[7],
                    stests[1].vveguid[0][0].mClsid.mData1,
                    stests[1].vveguid[0][0].mClsid.mData2,
                    stests[1].vveguid[0][0].mClsid.mData3,
                    stests[1].vveguid[0][0].mClsid.mData4[0],
                    stests[1].vveguid[0][0].mClsid.mData4[1],
                    stests[1].vveguid[0][0].mClsid.mData4[2],
                    stests[1].vveguid[0][0].mClsid.mData4[3],
                    stests[1].vveguid[0][0].mClsid.mData4[4],
                    stests[1].vveguid[0][0].mClsid.mData4[5],
                    stests[1].vveguid[0][0].mClsid.mData4[6],
                    stests[1].vveguid[0][0].mClsid.mData4[7],
                    stests[1].vveguid[0][0].mUunm,
                    stests[1].vveguid[0][0].mCarcode,
                    stests[1].vveguid[0][1].mClsid.mData1,
                    stests[1].vveguid[0][1].mClsid.mData2,
                    stests[1].vveguid[0][1].mClsid.mData3,
                    stests[1].vveguid[0][1].mClsid.mData4[0],
                    stests[1].vveguid[0][1].mClsid.mData4[1],
                    stests[1].vveguid[0][1].mClsid.mData4[2],
                    stests[1].vveguid[0][1].mClsid.mData4[3],
                    stests[1].vveguid[0][1].mClsid.mData4[4],
                    stests[1].vveguid[0][1].mClsid.mData4[5],
                    stests[1].vveguid[0][1].mClsid.mData4[6],
                    stests[1].vveguid[0][1].mClsid.mData4[7],
                    stests[1].vveguid[0][1].mUunm,
                    stests[1].vveguid[0][1].mCarcode,
                    stests[1].vveguid[1][0].mClsid.mData1,
                    stests[1].vveguid[1][0].mClsid.mData2,
                    stests[1].vveguid[1][0].mClsid.mData3,
                    stests[1].vveguid[1][0].mClsid.mData4[0],
                    stests[1].vveguid[1][0].mClsid.mData4[1],
                    stests[1].vveguid[1][0].mClsid.mData4[2],
                    stests[1].vveguid[1][0].mClsid.mData4[3],
                    stests[1].vveguid[1][0].mClsid.mData4[4],
                    stests[1].vveguid[1][0].mClsid.mData4[5],
                    stests[1].vveguid[1][0].mClsid.mData4[6],
                    stests[1].vveguid[1][0].mClsid.mData4[7],
                    stests[1].vveguid[1][0].mUunm,
                    stests[1].vveguid[1][0].mCarcode,
                    stests[1].vveguid[1][1].mClsid.mData1,
                    stests[1].vveguid[1][1].mClsid.mData2,
                    stests[1].vveguid[1][1].mClsid.mData3,
                    stests[1].vveguid[1][1].mClsid.mData4[0],
                    stests[1].vveguid[1][1].mClsid.mData4[1],
                    stests[1].vveguid[1][1].mClsid.mData4[2],
                    stests[1].vveguid[1][1].mClsid.mData4[3],
                    stests[1].vveguid[1][1].mClsid.mData4[4],
                    stests[1].vveguid[1][1].mClsid.mData4[5],
                    stests[1].vveguid[1][1].mClsid.mData4[6],
                    stests[1].vveguid[1][1].mClsid.mData4[7],
                    stests[1].vveguid[1][1].mUunm,
                    stests[1].vveguid[1][1].mCarcode,
                    stests[1].vvecode[0][0],
                    stests[1].vvecode[0][1],
                    stests[1].vvecode[1][0],
                    stests[1].vvecode[1][1],
                    stests[1].vvptr[0][0],
                    stests[1].vvptr[0][1],
                    stests[1].vvptr[1][0],
                    stests[1].vvptr[1][1],
                    _GetString(stests[1].vvetest[0][0]),
                    _GetString(stests[1].vvetest[0][1]),
                    _GetString(stests[1].vvetest[1][0]),
                    _GetString(stests[1].vvetest[1][1]),
                    stests[1].vvstest[0][0].i16,
                    stests[1].vvstest[0][1].i16,
                    stests[1].vvstest[1][0].i16,
                    stests[1].vvstest[1][1].i16)) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    assert(!FAILED(itest->TestStruct(stest)));

    assert((n = snprintf(ebuf, _BUF_SIZE, ""
                    "{\n"
                    "  \"i16\": %d,\n"
                    "  \"i32\": %d,\n"
                    "  \"i64\": %lld,\n"
                    "  \"byte\": \'%c\',\n"
                    "  \"f\": %f,\n"
                    "  \"d\": %lf,\n"
                    "  \"c32\": \'%lc\',\n"
                    "  \"b\": %s,\n"
                    "  \"emuid\": {\n"
                    "    \"mData1\": %u,\n"
                    "    \"mData2\": %u,\n"
                    "    \"mData3\": %u,\n"
                    "    \"mData4\": [\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u,\n"
                    "      %u\n"
                    "    ]\n"
                    "  },\n"
                    "  \"eguid\": {\n"
                    "    \"mClsid\": {\n"
                    "      \"mData1\": %u,\n"
                    "      \"mData2\": %u,\n"
                    "      \"mData3\": %u,\n"
                    "      \"mData4\": [\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u\n"
                    "      ]\n"
                    "    },\n"
                    "    \"mUunm\": \"%s\",\n"
                    "    \"mCarcode\": %u,\n"
                    "  },\n"
                    "  \"ecode\": %d,\n"
                    "  \"ptr\": %p,\n"
                    "  \"etest\": %s,\n"
                    "  \"stest\": {\n"
                    "    \"i16\": %d\n"
                    "  }\n"
                    "  \"vi16\": [\n"
                    "    %d,\n"
                    "    %d\n"
                    "  ],\n"
                    "  \"vi32\": [\n"
                    "    %d,\n"
                    "    %d\n"
                    "  ],\n"
                    "  \"vi64\": [\n"
                    "    %lld,\n"
                    "    %lld\n"
                    "  ],\n"
                    "  \"vbyte\": [\n"
                    "    \'%c\',\n"
                    "    \'%c\'\n"
                    "  ],\n"
                    "  \"vf\": [\n"
                    "    %f,\n"
                    "    %f\n"
                    "  ],\n"
                    "  \"vd\": [\n"
                    "    %lf,\n"
                    "    %lf\n"
                    "  ],\n"
                    "  \"vc32\": [\n"
                    "    \'%lc\',\n"
                    "    \'%lc\'\n"
                    "  ],\n"
                    "  \"vb\": [\n"
                    "    %s,\n"
                    "    %s\n"
                    "  ],\n"
                    "  \"vemuid\": [\n"
                    "    {\n"
                    "      \"mData1\": %u,\n"
                    "      \"mData2\": %u,\n"
                    "      \"mData3\": %u,\n"
                    "      \"mData4\": [\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u\n"
                    "      ]\n"
                    "    },\n"
                    "    {\n"
                    "      \"mData1\": %u,\n"
                    "      \"mData2\": %u,\n"
                    "      \"mData3\": %u,\n"
                    "      \"mData4\": [\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u,\n"
                    "        %u\n"
                    "      ]\n"
                    "    }\n"
                    "  ],\n"
                    "  \"veguid\": [\n"
                    "    {\n"
                    "      \"mClsid\": {\n"
                    "        \"mData1\": %u,\n"
                    "        \"mData2\": %u,\n"
                    "        \"mData3\": %u,\n"
                    "        \"mData4\": [\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u\n"
                    "        ]\n"
                    "      },\n"
                    "      \"mUunm\": \"%s\",\n"
                    "      \"mCarcode\": %u,\n"
                    "    },\n"
                    "    {\n"
                    "      \"mClsid\": {\n"
                    "        \"mData1\": %u,\n"
                    "        \"mData2\": %u,\n"
                    "        \"mData3\": %u,\n"
                    "        \"mData4\": [\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u\n"
                    "        ]\n"
                    "      },\n"
                    "      \"mUunm\": \"%s\",\n"
                    "      \"mCarcode\": %u,\n"
                    "    }\n"
                    "  ],\n"
                    "  \"vecode\": [\n"
                    "    %d,\n"
                    "    %d\n"
                    "  ],\n"
                    "  \"vptr\": [\n"
                    "    %p,\n"
                    "    %p\n"
                    "  ],\n"
                    "  \"vetest\": [\n"
                    "    %s,\n"
                    "    %s\n"
                    "  ],\n"
                    "  \"vstest\": [\n"
                    "    {\n"
                    "      \"i16\": %d\n"
                    "    },\n"
                    "    {\n"
                    "      \"i16\": %d\n"
                    "    }\n"
                    "  ],\n"
                    "  \"vvi16\": [\n"
                    "    [\n"
                    "      %d,\n"
                    "      %d\n"
                    "    ],\n"
                    "    [\n"
                    "      %d,\n"
                    "      %d\n"
                    "    ]\n"
                    "  ],\n"
                    "  \"vvi32\": [\n"
                    "    [\n"
                    "      %d,\n"
                    "      %d\n"
                    "    ],\n"
                    "    [\n"
                    "      %d,\n"
                    "      %d\n"
                    "    ]\n"
                    "  ],\n"
                    "  \"vvi64\": [\n"
                    "    [\n"
                    "      %lld,\n"
                    "      %lld\n"
                    "    ],\n"
                    "    [\n"
                    "      %lld,\n"
                    "      %lld\n"
                    "    ]\n"
                    "  ],\n"
                    "  \"vvbyte\": [\n"
                    "    [\n"
                    "      \'%c\',\n"
                    "      \'%c\'\n"
                    "    ],\n"
                    "    [\n"
                    "      \'%c\',\n"
                    "      \'%c\'\n"
                    "    ]\n"
                    "  ],\n"
                    "  \"vvf\": [\n"
                    "    [\n"
                    "      %f,\n"
                    "      %f\n"
                    "    ],\n"
                    "    [\n"
                    "      %f,\n"
                    "      %f\n"
                    "    ]\n"
                    "  ],\n"
                    "  \"vvd\": [\n"
                    "    [\n"
                    "      %lf,\n"
                    "      %lf\n"
                    "    ],\n"
                    "    [\n"
                    "      %lf,\n"
                    "      %lf\n"
                    "    ]\n"
                    "  ],\n"
                    "  \"vvc32\": [\n"
                    "    [\n"
                    "      \'%c\',\n"
                    "      \'%c\'\n"
                    "    ],\n"
                    "    [\n"
                    "      \'%c\',\n"
                    "      \'%c\'\n"
                    "    ]\n"
                    "  ],\n"
                    "  \"vvb\": [\n"
                    "    [\n"
                    "      %s,\n"
                    "      %s\n"
                    "    ],\n"
                    "    [\n"
                    "      %s,\n"
                    "      %s\n"
                    "    ]\n"
                    "  ],\n"
                    "  \"vvemuid\": [\n"
                    "    [\n"
                    "      {\n"
                    "        \"mData1\": %u,\n"
                    "        \"mData2\": %u,\n"
                    "        \"mData3\": %u,\n"
                    "        \"mData4\": [\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u\n"
                    "        ]\n"
                    "      },\n"
                    "      {\n"
                    "        \"mData1\": %u,\n"
                    "        \"mData2\": %u,\n"
                    "        \"mData3\": %u,\n"
                    "        \"mData4\": [\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u\n"
                    "        ]\n"
                    "      }\n"
                    "    ],\n"
                    "    [\n"
                    "      {\n"
                    "        \"mData1\": %u,\n"
                    "        \"mData2\": %u,\n"
                    "        \"mData3\": %u,\n"
                    "        \"mData4\": [\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u\n"
                    "        ]\n"
                    "      },\n"
                    "      {\n"
                    "        \"mData1\": %u,\n"
                    "        \"mData2\": %u,\n"
                    "        \"mData3\": %u,\n"
                    "        \"mData4\": [\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u,\n"
                    "          %u\n"
                    "        ]\n"
                    "      }\n"
                    "    ]\n"
                    "  ],\n"
                    "  \"vveguid\": [\n"
                    "    [\n"
                    "      {\n"
                    "        \"mClsid\": {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        },\n"
                    "        \"mUunm\": \"%s\",\n"
                    "        \"mCarcode\": %u,\n"
                    "      },\n"
                    "      {\n"
                    "        \"mClsid\": {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        },\n"
                    "        \"mUunm\": \"%s\",\n"
                    "        \"mCarcode\": %u,\n"
                    "      }\n"
                    "    ],\n"
                    "    [\n"
                    "      {\n"
                    "        \"mClsid\": {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        },\n"
                    "        \"mUunm\": \"%s\",\n"
                    "        \"mCarcode\": %u,\n"
                    "      },\n"
                    "      {\n"
                    "        \"mClsid\": {\n"
                    "          \"mData1\": %u,\n"
                    "          \"mData2\": %u,\n"
                    "          \"mData3\": %u,\n"
                    "          \"mData4\": [\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u,\n"
                    "            %u\n"
                    "          ]\n"
                    "        },\n"
                    "        \"mUunm\": \"%s\",\n"
                    "        \"mCarcode\": %u,\n"
                    "      }\n"
                    "    ]\n"
                    "  ],\n"
                    "  \"vvecode\": [\n"
                    "    [\n"
                    "      %d,\n"
                    "      %d\n"
                    "    ],\n"
                    "    [\n"
                    "      %d,\n"
                    "      %d\n"
                    "    ]\n"
                    "  ],\n"
                    "  \"vvptr\": [\n"
                    "    [\n"
                    "      %p,\n"
                    "      %p\n"
                    "    ],\n"
                    "    [\n"
                    "      %p,\n"
                    "      %p\n"
                    "    ]\n"
                    "  ],\n"
                    "  \"vvetest\": [\n"
                    "    [\n"
                    "      %s,\n"
                    "      %s\n"
                    "    ],\n"
                    "    [\n"
                    "      %s,\n"
                    "      %s\n"
                    "    ]\n"
                    "  ],\n"
                    "  \"vvstest\": [\n"
                    "    [\n"
                    "      {\n"
                    "        \"i16\": %d\n"
                    "      },\n"
                    "      {\n"
                    "        \"i16\": %d\n"
                    "      }\n"
                    "    ],\n"
                    "    [\n"
                    "      {\n"
                    "        \"i16\": %d\n"
                    "      },\n"
                    "      {\n"
                    "        \"i16\": %d\n"
                    "      }\n"
                    "    ]\n"
                    "  ]\n"
                    "} (Struct (STest))\n",
                    stest.i16,
                    stest.i32,
                    stest.i64,
                    stest.byte,
                    stest.f,
                    stest.d,
                    stest.c32,
                    _GetString(stest.b),
                    stest.emuid.mData1,
                    stest.emuid.mData2,
                    stest.emuid.mData3,
                    stest.emuid.mData4[0],
                    stest.emuid.mData4[1],
                    stest.emuid.mData4[2],
                    stest.emuid.mData4[3],
                    stest.emuid.mData4[4],
                    stest.emuid.mData4[5],
                    stest.emuid.mData4[6],
                    stest.emuid.mData4[7],
                    stest.eguid.mClsid.mData1,
                    stest.eguid.mClsid.mData2,
                    stest.eguid.mClsid.mData3,
                    stest.eguid.mClsid.mData4[0],
                    stest.eguid.mClsid.mData4[1],
                    stest.eguid.mClsid.mData4[2],
                    stest.eguid.mClsid.mData4[3],
                    stest.eguid.mClsid.mData4[4],
                    stest.eguid.mClsid.mData4[5],
                    stest.eguid.mClsid.mData4[6],
                    stest.eguid.mClsid.mData4[7],
                    stest.eguid.mUunm,
                    stest.eguid.mCarcode,
                    stest.ecode,
                    stest.ptr,
                    _GetString(stest.etest),
                    stest.stest.i16,
                    stest.vi16[0],
                    stest.vi16[1],
                    stest.vi32[0],
                    stest.vi32[1],
                    stest.vi64[0],
                    stest.vi64[1],
                    stest.vbyte[0],
                    stest.vbyte[1],
                    stest.vf[0],
                    stest.vf[1],
                    stest.vd[0],
                    stest.vd[1],
                    stest.vc32[0],
                    stest.vc32[1],
                    _GetString(stest.vb[0]),
                    _GetString(stest.vb[1]),
                    stest.vemuid[0].mData1,
                    stest.vemuid[0].mData2,
                    stest.vemuid[0].mData3,
                    stest.vemuid[0].mData4[0],
                    stest.vemuid[0].mData4[1],
                    stest.vemuid[0].mData4[2],
                    stest.vemuid[0].mData4[3],
                    stest.vemuid[0].mData4[4],
                    stest.vemuid[0].mData4[5],
                    stest.vemuid[0].mData4[6],
                    stest.vemuid[0].mData4[7],
                    stest.vemuid[1].mData1,
                    stest.vemuid[1].mData2,
                    stest.vemuid[1].mData3,
                    stest.vemuid[1].mData4[0],
                    stest.vemuid[1].mData4[1],
                    stest.vemuid[1].mData4[2],
                    stest.vemuid[1].mData4[3],
                    stest.vemuid[1].mData4[4],
                    stest.vemuid[1].mData4[5],
                    stest.vemuid[1].mData4[6],
                    stest.vemuid[1].mData4[7],
                    stest.veguid[0].mClsid.mData1,
                    stest.veguid[0].mClsid.mData2,
                    stest.veguid[0].mClsid.mData3,
                    stest.veguid[0].mClsid.mData4[0],
                    stest.veguid[0].mClsid.mData4[1],
                    stest.veguid[0].mClsid.mData4[2],
                    stest.veguid[0].mClsid.mData4[3],
                    stest.veguid[0].mClsid.mData4[4],
                    stest.veguid[0].mClsid.mData4[5],
                    stest.veguid[0].mClsid.mData4[6],
                    stest.veguid[0].mClsid.mData4[7],
                    stest.veguid[0].mUunm,
                    stest.veguid[0].mCarcode,
                    stest.veguid[1].mClsid.mData1,
                    stest.veguid[1].mClsid.mData2,
                    stest.veguid[1].mClsid.mData3,
                    stest.veguid[1].mClsid.mData4[0],
                    stest.veguid[1].mClsid.mData4[1],
                    stest.veguid[1].mClsid.mData4[2],
                    stest.veguid[1].mClsid.mData4[3],
                    stest.veguid[1].mClsid.mData4[4],
                    stest.veguid[1].mClsid.mData4[5],
                    stest.veguid[1].mClsid.mData4[6],
                    stest.veguid[1].mClsid.mData4[7],
                    stest.veguid[1].mUunm,
                    stest.veguid[1].mCarcode,
                    stest.vecode[0],
                    stest.vecode[1],
                    stest.vptr[0],
                    stest.vptr[1],
                    _GetString(stest.vetest[0]),
                    _GetString(stest.vetest[1]),
                    stest.vstest[0].i16,
                    stest.vstest[1].i16,
                    stest.vvi16[0][0],
                    stest.vvi16[0][1],
                    stest.vvi16[1][0],
                    stest.vvi16[1][1],
                    stest.vvi32[0][0],
                    stest.vvi32[0][1],
                    stest.vvi32[1][0],
                    stest.vvi32[1][1],
                    stest.vvi64[0][0],
                    stest.vvi64[0][1],
                    stest.vvi64[1][0],
                    stest.vvi64[1][1],
                    stest.vvbyte[0][0],
                    stest.vvbyte[0][1],
                    stest.vvbyte[1][0],
                    stest.vvbyte[1][1],
                    stest.vvf[0][0],
                    stest.vvf[0][1],
                    stest.vvf[1][0],
                    stest.vvf[1][1],
                    stest.vvd[0][0],
                    stest.vvd[0][1],
                    stest.vvd[1][0],
                    stest.vvd[1][1],
                    stest.vvc32[0][0],
                    stest.vvc32[0][1],
                    stest.vvc32[1][0],
                    stest.vvc32[1][1],
                    _GetString(stest.vvb[0][0]),
                    _GetString(stest.vvb[0][1]),
                    _GetString(stest.vvb[1][0]),
                    _GetString(stest.vvb[1][1]),
                    stest.vvemuid[0][0].mData1,
                    stest.vvemuid[0][0].mData2,
                    stest.vvemuid[0][0].mData3,
                    stest.vvemuid[0][0].mData4[0],
                    stest.vvemuid[0][0].mData4[1],
                    stest.vvemuid[0][0].mData4[2],
                    stest.vvemuid[0][0].mData4[3],
                    stest.vvemuid[0][0].mData4[4],
                    stest.vvemuid[0][0].mData4[5],
                    stest.vvemuid[0][0].mData4[6],
                    stest.vvemuid[0][0].mData4[7],
                    stest.vvemuid[0][1].mData1,
                    stest.vvemuid[0][1].mData2,
                    stest.vvemuid[0][1].mData3,
                    stest.vvemuid[0][1].mData4[0],
                    stest.vvemuid[0][1].mData4[1],
                    stest.vvemuid[0][1].mData4[2],
                    stest.vvemuid[0][1].mData4[3],
                    stest.vvemuid[0][1].mData4[4],
                    stest.vvemuid[0][1].mData4[5],
                    stest.vvemuid[0][1].mData4[6],
                    stest.vvemuid[0][1].mData4[7],
                    stest.vvemuid[1][0].mData1,
                    stest.vvemuid[1][0].mData2,
                    stest.vvemuid[1][0].mData3,
                    stest.vvemuid[1][0].mData4[0],
                    stest.vvemuid[1][0].mData4[1],
                    stest.vvemuid[1][0].mData4[2],
                    stest.vvemuid[1][0].mData4[3],
                    stest.vvemuid[1][0].mData4[4],
                    stest.vvemuid[1][0].mData4[5],
                    stest.vvemuid[1][0].mData4[6],
                    stest.vvemuid[1][0].mData4[7],
                    stest.vvemuid[1][1].mData1,
                    stest.vvemuid[1][1].mData2,
                    stest.vvemuid[1][1].mData3,
                    stest.vvemuid[1][1].mData4[0],
                    stest.vvemuid[1][1].mData4[1],
                    stest.vvemuid[1][1].mData4[2],
                    stest.vvemuid[1][1].mData4[3],
                    stest.vvemuid[1][1].mData4[4],
                    stest.vvemuid[1][1].mData4[5],
                    stest.vvemuid[1][1].mData4[6],
                    stest.vvemuid[1][1].mData4[7],
                    stest.vveguid[0][0].mClsid.mData1,
                    stest.vveguid[0][0].mClsid.mData2,
                    stest.vveguid[0][0].mClsid.mData3,
                    stest.vveguid[0][0].mClsid.mData4[0],
                    stest.vveguid[0][0].mClsid.mData4[1],
                    stest.vveguid[0][0].mClsid.mData4[2],
                    stest.vveguid[0][0].mClsid.mData4[3],
                    stest.vveguid[0][0].mClsid.mData4[4],
                    stest.vveguid[0][0].mClsid.mData4[5],
                    stest.vveguid[0][0].mClsid.mData4[6],
                    stest.vveguid[0][0].mClsid.mData4[7],
                    stest.vveguid[0][0].mUunm,
                    stest.vveguid[0][0].mCarcode,
                    stest.vveguid[0][1].mClsid.mData1,
                    stest.vveguid[0][1].mClsid.mData2,
                    stest.vveguid[0][1].mClsid.mData3,
                    stest.vveguid[0][1].mClsid.mData4[0],
                    stest.vveguid[0][1].mClsid.mData4[1],
                    stest.vveguid[0][1].mClsid.mData4[2],
                    stest.vveguid[0][1].mClsid.mData4[3],
                    stest.vveguid[0][1].mClsid.mData4[4],
                    stest.vveguid[0][1].mClsid.mData4[5],
                    stest.vveguid[0][1].mClsid.mData4[6],
                    stest.vveguid[0][1].mClsid.mData4[7],
                    stest.vveguid[0][1].mUunm,
                    stest.vveguid[0][1].mCarcode,
                    stest.vveguid[1][0].mClsid.mData1,
                    stest.vveguid[1][0].mClsid.mData2,
                    stest.vveguid[1][0].mClsid.mData3,
                    stest.vveguid[1][0].mClsid.mData4[0],
                    stest.vveguid[1][0].mClsid.mData4[1],
                    stest.vveguid[1][0].mClsid.mData4[2],
                    stest.vveguid[1][0].mClsid.mData4[3],
                    stest.vveguid[1][0].mClsid.mData4[4],
                    stest.vveguid[1][0].mClsid.mData4[5],
                    stest.vveguid[1][0].mClsid.mData4[6],
                    stest.vveguid[1][0].mClsid.mData4[7],
                    stest.vveguid[1][0].mUunm,
                    stest.vveguid[1][0].mCarcode,
                    stest.vveguid[1][1].mClsid.mData1,
                    stest.vveguid[1][1].mClsid.mData2,
                    stest.vveguid[1][1].mClsid.mData3,
                    stest.vveguid[1][1].mClsid.mData4[0],
                    stest.vveguid[1][1].mClsid.mData4[1],
                    stest.vveguid[1][1].mClsid.mData4[2],
                    stest.vveguid[1][1].mClsid.mData4[3],
                    stest.vveguid[1][1].mClsid.mData4[4],
                    stest.vveguid[1][1].mClsid.mData4[5],
                    stest.vveguid[1][1].mClsid.mData4[6],
                    stest.vveguid[1][1].mClsid.mData4[7],
                    stest.vveguid[1][1].mUunm,
                    stest.vveguid[1][1].mCarcode,
                    stest.vvecode[0][0],
                    stest.vvecode[0][1],
                    stest.vvecode[1][0],
                    stest.vvecode[1][1],
                    stest.vvptr[0][0],
                    stest.vvptr[0][1],
                    stest.vvptr[1][0],
                    stest.vvptr[1][1],
                    _GetString(stest.vvetest[0][0]),
                    _GetString(stest.vvetest[0][1]),
                    _GetString(stest.vvetest[1][0]),
                    _GetString(stest.vvetest[1][1]),
                    stest.vvstest[0][0].i16,
                    stest.vvstest[0][1].i16,
                    stest.vvstest[1][0].i16,
                    stest.vvstest[1][1].i16)) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    itest_ = itest;

    assert(!FAILED(itest->TestInterface(itest_)));

    assert((n = snprintf(ebuf, _BUF_SIZE, "%p (Interface (ITest))\n", itest_)) >= 0);

    rewind(output);

    assert(fread(obuf, 1, n + 1, output) == static_cast<size_t>(n));

    assert(strcmp(obuf, ebuf) == 0);

    assert(printf("%s\n", obuf) >= 0);

    return 0;
}

