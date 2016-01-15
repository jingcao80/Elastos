
#include "CCertPathBuilderHelper.h"
#include "CertPathBuilder.h"

namespace Elastos {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL(CCertPathBuilderHelper, Singleton, ICertPathBuilderHelper)
CAR_SINGLETON_IMPL(CCertPathBuilderHelper)

ECode CCertPathBuilderHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ ICertPathBuilder **builder)
{
    return CertPathBuilder::GetInstance(algorithm, builder);
}

ECode CCertPathBuilderHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ ICertPathBuilder **builder)
{
    return CertPathBuilder::GetInstance(algorithm, provider, builder);
}

ECode CCertPathBuilderHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider *provider,
    /* [out] */ ICertPathBuilder **builder)
{
    return CertPathBuilder::GetInstance(algorithm, provider, builder);
}

ECode CCertPathBuilderHelper::GetDefaultType(
    /* [out] */ String *type)
{
    return CertPathBuilder::GetDefaultType(type);
}

}
}
}
