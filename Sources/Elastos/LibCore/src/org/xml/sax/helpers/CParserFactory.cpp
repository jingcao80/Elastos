#include "CParserFactory.h"
#include "NewInstanceHelper.h"
#include "CSystem.h"
#include <utils/Log.h>

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

CAR_INTERFACE_IMPL(CParserFactory, Singleton, IParserFactory)

CAR_SINGLETON_IMPL(CParserFactory)

ECode CParserFactory::MakeParser(
    /* [out] */ IParser** parser)
{
    return ParserFactory::MakeParser(parser);
}

ECode CParserFactory::MakeParser(
    /* [in] */ const String& className,
    /* [out] */ IParser** parser)
{
    return ParserFactory::MakeParser(className, parser);
}

ECode ParserFactory::MakeParser(
    /* [out] */ IParser** parser)
{
    VALIDATE_NOT_NULL(parser);
    *parser = NULL;

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    String className;
    system->GetProperty(String("org.xml.sax.parser"), &className);
    if (className.IsNullOrEmpty()) {
        ALOGE("ParserFactory::MakeParser NullPointerException: No value for sax.parser property!");
        return E_NULL_POINTER_EXCEPTION;
    } else {
        return ParserFactory::MakeParser(className, parser);
    }

    return NOERROR;
}

ECode ParserFactory::MakeParser(
    /* [in] */ const String& className,
    /* [out] */ IParser** parser)
{
    VALIDATE_NOT_NULL(parser);

    AutoPtr<IClassLoader> icl = NewInstanceHelper::GetClassLoader();
    AutoPtr<IInterface> ointer = NewInstanceHelper::NewInstance(icl, className);
    *parser = IParser::Probe(ointer.Get());
    REFCOUNT_ADD(*parser)

    return NOERROR;
}

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org
