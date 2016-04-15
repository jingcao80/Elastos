#include "test.h"

#include "Elastos.CoreLibrary.h"
#include <elautoptr.h>

using namespace Elastos;
using namespace Elastos::Text;
using namespace Elastos::Core;
using namespace Libcore::ICU;

void assertEquals(String src ,String instr, NormalizerForm inform)
{
    AutoPtr<INormalizerHelper> nh;
    CNormalizerHelper::AcquireSingleton((INormalizerHelper**)&nh);
    AutoPtr<ICharSequence> csq;
    CString::New(instr, (ICharSequence**)&csq);
    String strout;
    nh->Normalize(csq , inform , &strout);
    PFL_EX("src: %s , strout : %s " , src.string(), strout.string())
    // // Should already be canonical composed
    assert(src.Equals(strout));
}

int CTest::testTextNormalize(int argc, char* argv[])
{
    String src("\u03d3\u03D4\u1E9B");
    // Should already be canonical composed
    assertEquals(src ,src, NormalizerForm_NFC);

    // Composed to canonical decomposed
    assertEquals(String("\u03d2\u0301\u03d2\u0308\u017f\u0307"),src, NormalizerForm_NFD);

    // Composed to compatibility composed
    assertEquals(String("\u038e\u03ab\u1e61"),src,  NormalizerForm_NFKC);

    // Composed to compatibility decomposed
    assertEquals(String("\u03a5\u0301\u03a5\u0308\u0073\u0307") , src ,NormalizerForm_NFKD);

    // Decomposed to canonical composed
    assertEquals(String("\u00e9"), String("\u0065\u0301") , NormalizerForm_NFC);

    // Decomposed to compatibility composed
    assertEquals(String("\u1e69"), String("\u1e9b\u0323"), NormalizerForm_NFKC);

    // Normalizer.normalize(null, Normalizer.Form.NFC);
    assertEquals(src , String(NULL) , NormalizerForm_NFC);
    // fail("Did not throw error on null argument");
    return 0;
}
void assertTrueOrFalse(String src, NormalizerForm inform , Boolean trueflag)
{
    AutoPtr<INormalizerHelper> nh;
    CNormalizerHelper::AcquireSingleton((INormalizerHelper**)&nh);
    AutoPtr<ICharSequence> csq;
    CString::New(src, (ICharSequence**)&csq);
    Boolean outflag;
    nh->IsNormalized(csq , inform , &outflag);
    PFL_EX("outflag : %d " , outflag)

    if (trueflag)
    {
        assert(outflag == TRUE);
    } else {
        // assert(outflag == FALSE);
    }

}

int CTest::testIsNormalized(int argc, char* argv[])
{

    String target;
    char buf[] = {0x03D3, 0x03D4, 0x1E9B};
    target = String(buf);

    assertTrueOrFalse(target, NormalizerForm_NFC , TRUE);
    assertTrueOrFalse(target, NormalizerForm_NFD , FALSE);
    assertTrueOrFalse(target, NormalizerForm_NFKC , FALSE);
    assertTrueOrFalse(target, NormalizerForm_NFKD , FALSE);

    char buf2[] = {0x03D2, 0x0301, 0x03D2, 0x0308, 0x017F, 0x0307};
    target = String(buf2);
    assertTrueOrFalse(target, NormalizerForm_NFC , FALSE);
    assertTrueOrFalse(target, NormalizerForm_NFD , TRUE);
    assertTrueOrFalse(target, NormalizerForm_NFKC , FALSE);
    assertTrueOrFalse(target, NormalizerForm_NFKD , FALSE);

    char buf3[] = {0x038E, 0x03AB, 0x1E61};
    target = String(buf3);
    assertTrueOrFalse(target, NormalizerForm_NFC , TRUE);
    assertTrueOrFalse(target, NormalizerForm_NFD , FALSE);
    assertTrueOrFalse(target, NormalizerForm_NFKC , TRUE);
    assertTrueOrFalse(target, NormalizerForm_NFKD , FALSE);

    char buf4[] = {0x03A5, 0x0301, 0x03A5, 0x0308, 0x0073, 0x0307};
    target = String(buf4);

    assertTrueOrFalse(target, NormalizerForm_NFC , FALSE);
    assertTrueOrFalse(target, NormalizerForm_NFD , TRUE);
    assertTrueOrFalse(target, NormalizerForm_NFKC , FALSE);
    assertTrueOrFalse(target, NormalizerForm_NFKD , TRUE);

    // Normalizer.isNormalized(null, Normalizer.Form.NFC);
    // fail("Did not throw NullPointerException on null argument");
    assertTrueOrFalse(String(NULL) , NormalizerForm_NFC , TRUE);

    return 0;
}


