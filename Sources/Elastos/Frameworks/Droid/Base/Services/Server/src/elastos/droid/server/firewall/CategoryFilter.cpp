#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/server/firewall/CategoryFilter.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// CategoryFilter::FACTORY_FilterFactory
//------------------------------------------------------------------------------

CategoryFilter::FACTORY_FilterFactory::FACTORY_FilterFactory(
    /* [in] */ const String& tag)
{
    FilterFactory::constructor(tag);
}

ECode CategoryFilter::FACTORY_FilterFactory::NewFilter(
    /* in */ IXmlPullParser* parser,
    /* [out] */ IFilter** result)
{
    VALIDATE_NOT_NULL(result)
    String categoryName;

    ECode ec = parser->GetAttributeValue(String(NULL), CategoryFilter::ATTR_NAME, &categoryName);

    if (FAILED(ec) || categoryName.IsNull()) {
        Slogger::I("CategoryFilter", "Category name must be specified. %p",
                parser);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    AutoPtr<CategoryFilter> res = new CategoryFilter(categoryName);
    *result = IFilter::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//=======================================================================================
// CategoryFilter
//=======================================================================================

const String CategoryFilter::ATTR_NAME("name");
const AutoPtr<CategoryFilter::FACTORY_FilterFactory> CategoryFilter::FACTORY = new CategoryFilter::FACTORY_FilterFactory(String("category"));

CAR_INTERFACE_IMPL(CategoryFilter, Object, IFilter);

CategoryFilter::CategoryFilter(
    /* in */ const String& categoryName)
    : mCategoryName(categoryName)
{}

ECode CategoryFilter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr<ArrayOf<String> > categories;

    ECode ec = intent->GetCategories((ArrayOf<String>**)&categories);
    if (FAILED(ec) || categories == NULL) {
        *ret = FALSE;
        return NOERROR;
    }
    *ret = categories->Contains(mCategoryName);

    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
