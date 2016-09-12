
#ifndef __ORG_CONSCRYPT_CCHAINSTRENGTHANALYZER_H__
#define __ORG_CONSCRYPT_CCHAINSTRENGTHANALYZER_H__

#include "_Org_Conscrypt_CChainStrengthAnalyzer.h"
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/Singleton.h>

using Elastos::Security::Cert::IX509Certificate;

namespace Org {
namespace Conscrypt {

CarClass(CChainStrengthAnalyzer)
    , public Singleton
    , public IChainStrengthAnalyzer
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Check(
        /* [in] */  ArrayOf<IX509Certificate*>* chain);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CCHAINSTRENGTHANALYZER_H__
