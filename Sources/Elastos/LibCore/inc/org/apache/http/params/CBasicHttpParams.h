
#ifndef __ORG_APACHE_HTTP_PARAMS_CBASICHTTPPARAMS_H_
#define __ORG_APACHE_HTTP_PARAMS_CBASICHTTPPARAMS_H_

#include "Elastos.CoreLibrary.Utility.h"
#include "_Org_Apache_Http_Params_CBasicHttpParams.h"
#include "org/apache/http/params/AbstractHttpParams.h"

using Elastos::Core::ICloneable;
using Elastos::Utility::IHashMap;

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

/**
 * This class represents a collection of HTTP protocol parameters.
 * Protocol parameters may be linked together to form a hierarchy.
 * If a particular parameter value has not been explicitly defined
 * in the collection itself, its value will be drawn from the parent
 * collection of parameters.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 610464 $
 */
CarClass(CBasicHttpParams)
    , public AbstractHttpParams
    , public IBasicHttpParams
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetParameter(
        /* [in] */ const String& name,
        /* [out] */ IInterface** parameter);

    /**
     * Assigns the value to the parameter with the given name.
     *
     * @param name parameter name
     * @param value parameter value
     */
    CARAPI SetParameter(
        /* [in] */ const String& name,
        /* [in] */ IInterface* value,
        /* [out] */ IHttpParams** httpParams);

    CARAPI RemoveParameter(
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    /**
     * Assigns the value to all the parameter with the given names
     *
     * @param names array of parameter name
     * @param value parameter value
     */
    CARAPI SetParameters(
        /* [in] */ ArrayOf<String>* names,
        /* [in] */ IInterface* value);

    CARAPI IsParameterSet(
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    CARAPI IsParameterSetLocally(
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    /**
     * Removes all parameters from this collection.
     */
    CARAPI Clear();

    /**
     * Creates a copy of these parameters.
     * The implementation here instantiates {@link BasicHttpParams},
     * then calls {@link #copyParams(HttpParams)} to populate the copy.
     *
     * @return  a new set of params holding a copy of the
     *          <i>local</i> parameters in this object.
     */
    CARAPI Copy(
        /* [out] */ IHttpParams** httpParams);

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    /**
     * Copies the locally defined parameters to the argument parameters.
     * This method is called from {@link #copy()}.
     *
     * @param target    the parameters to which to copy
     */
    CARAPI CopyParams(
        /* [in] */ IHttpParams* target);

    CARAPI constructor();

private:
    static const Int64 serialVersionUID = -7086398485908701455L;

    /** Map of HTTP parameters that this collection contains. */
    AutoPtr<IHashMap> mParameters;
};

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PARAMS_CBASICHTTPPARAMS_H_
