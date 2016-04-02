#include "elastos/droid/server/firewall/CategoryFilter.h"

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

IFilter* CategoryFilter::FACTORY_FilterFactory::NewFilter(
    /* in */ IXmlPullParser* parser)
{
    String categoryName;

    ECode ec = parser->GetAttributeValue(String(NULL), CategoryFilter::ATTR_NAME, &categoryName);

    if (FAILED(ec) || categoryName == NULL) {
        //throw new XmlPullParserException("Category name must be specified.",
        //        parser, null);
        return NULL;
    }
    return new CategoryFilter(categoryName);
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
    AutoPtr<ArrayOf<String> > categories;

    ECode ec = intent->GetCategories((ArrayOf<String>**)&categories);
    if (FAILED(ec) || categories == NULL) {
        *ret = FALSE;
        return NOERROR;
    }
    *ret = (*categories).Contains(mCategoryName);

    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
