#ifndef __ELASTOS_DROID_VIEW_CONTEXTTHEMEWRAPPER_H__
#define __ELASTOS_DROID_VIEW_CONTEXTTHEMEWRAPPER_H__

#include "Elastos.Droid.View.h"
#include <elastos/droid/content/ContextWrapper.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ContextWrapper;

namespace Elastos {
namespace Droid {
namespace View {

class ECO_PUBLIC ContextThemeWrapper
    : public ContextWrapper
    , public IContextThemeWrapper
{
public:
    CAR_INTERFACE_DECL()

    ContextThemeWrapper();

    virtual ~ContextThemeWrapper();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* base,
        /* [in] */ Int32 themeres);

    CARAPI constructor(
        /* [in] */ IContext* base,
        /* [in] */ Int32 themeres,
        /* [in] */ Boolean holdBaseContext);

    /**
     * Call to set an "override configuration" on this context -- this is
     * a configuration that replies one or more values of the standard
     * configuration that is applied to the context.  See
     * {@link Context#createConfigurationContext(Configuration)} for more
     * information.
     *
     * <p>This method can only be called once, and must be called before any
     * calls to {@link #getResources()} are made.
     */
    CARAPI ApplyOverrideConfiguration(
        /* [in] */ IConfiguration* overrideConfiguration);

    CARAPI GetResources(
        /* [out] */ IResources** resources);

    CARAPI SetTheme(
        /* [in] */ Int32 resid);

    CARAPI GetThemeResId(
        /* [out] */ Int32* resId);

    CARAPI GetTheme(
        /* [out] */ IResourcesTheme** theme);

    CARAPI GetSystemService(
        /* [in] */ const String& name,
        /* [out] */ IInterface** object);

protected:

    virtual CARAPI OnApplyThemeResource(
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ Int32 resid,
        /* [in] */ Boolean first);

private:
    ECO_LOCAL CARAPI InitializeTheme();

protected:
    Int32 mThemeResource;
    AutoPtr<IResourcesTheme> mTheme;
    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<IConfiguration> mOverrideConfiguration;
    AutoPtr<IResources> mResources;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CONTEXTTHEMEWRAPPER_H__
