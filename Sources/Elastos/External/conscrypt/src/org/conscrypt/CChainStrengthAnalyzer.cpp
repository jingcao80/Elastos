#include "org/conscrypt/CChainStrengthAnalyzer.h"
#include "org/conscrypt/ChainStrengthAnalyzer.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(CChainStrengthAnalyzer)

CAR_INTERFACE_IMPL(CChainStrengthAnalyzer, Singleton, IChainStrengthAnalyzer)

ECode CChainStrengthAnalyzer::Check(
    /* [in] */  ArrayOf<IX509Certificate*>* chain)
{
    return ChainStrengthAnalyzer::Check(chain);
}

} // namespace Conscrypt
} // namespace Org
