
#ifndef __ORG_APACHE_HTTP_PARAMS_CDefaultedHttpParams_H_
#define __ORG_APACHE_HTTP_PARAMS_CDefaultedHttpParams_H_

#include "_Org_Apache_Http_Params_CDefaultedHttpParams.h"
#include "org/apache/http/params/AbstractHttpParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

/**
 * {@link HttpParams} implementation that delegates resolution of a parameter
 * to the given default {@link HttpParams} instance if the parameter is not
 * present in the local one. The state of the local collection can be mutated,
 * whereas the default collection is treated as read-only.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 610763 $
 */
CarClass(CDefaultedHttpParams)
    , public AbstractHttpParams
    , public IDefaultedHttpParams
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Creates a copy of the local collection with the same default
     */
    CARAPI Copy(
        /* [out] */ IHttpParams** httpParams);

    /**
     * Retrieves the value of the parameter from the local collection and, if the
     * parameter is not set locally, delegates its resolution to the default
     * collection.
     */
    CARAPI GetParameter(
        /* [in] */ const String& name,
        /* [out] */ IInterface** parameter);

    /**
     * Attempts to remove the parameter from the local collection. This method
     * <i>does not</i> modify the default collection.
     */
    CARAPI RemoveParameter(
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    /**
     * Sets the parameter in the local collection. This method <i>does not</i>
     * modify the default collection.
     */
    CARAPI SetParameter(
        /* [in] */ const String& name,
        /* [in] */ IInterface* value,
        /* [out] */ IHttpParams** httpParams);

    /**
     * Copies the locally defined parameters to the argument parameters.
     * This method is called from {@link #copy()}.
     *
     * @param target    the parameters to which to copy
     */
    CARAPI GetDefaults(
        /* [out] */ IHttpParams** defaults);

    CARAPI constructor(
        /* [in] */ IHttpParams* local,
        /* [in] */ IHttpParams* defaults);

private:
    AutoPtr<IHttpParams> mLocal;
    AutoPtr<IHttpParams> mDefaults;
};

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PARAMS_CDefaultedHttpParams_H_
