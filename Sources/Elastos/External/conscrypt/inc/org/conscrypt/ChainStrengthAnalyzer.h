
#ifndef __ORG_CONSCRYPT_CHAINSTRENGTHANALYZER_H__
#define __ORG_CONSCRYPT_CHAINSTRENGTHANALYZER_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Security.h>

using Elastos::Security::Cert::IX509Certificate;

namespace Org {
namespace Conscrypt {

class ChainStrengthAnalyzer
{
public:
    static CARAPI Check(
        /* [in] */  ArrayOf<IX509Certificate*>* chain);

private:
    static CARAPI CheckCert(
        /* [in] */ IX509Certificate* cert);

    static CARAPI CheckModulusLength(
        /* [in] */ IX509Certificate* cert);

    static CARAPI CheckNotMD5(
        /* [in] */ IX509Certificate* cert);

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitOID_BLACKLIST();

private:
    static const Int32 MIN_MODULUS = 1024;
    static AutoPtr<ArrayOf<String> > OID_BLACKLIST; // = {"1.2.840.113549.1.1.4"}; // MD5withRSA
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CHAINSTRENGTHANALYZER_H__
