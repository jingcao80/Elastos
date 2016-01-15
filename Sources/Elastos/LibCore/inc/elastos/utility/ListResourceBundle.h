#ifndef __ELASTOS_UTILITY_LISTRESOURCEBUNDLE_H__
#define __ELASTOS_UTILITY_LISTRESOURCEBUNDLE_H__

#include "ResourceBundle.h"

namespace Elastos{
namespace Utility{

class ListResourceBundle
    : public ResourceBundle
    , public IListResourceBundle
{
public:
    class _FirstEnumeration
        : public Object
        , public IEnumeration
    {
    public:
        CAR_INTERFACE_DECL()

        _FirstEnumeration(
            /* [in] */ ListResourceBundle* host);

        CARAPI HasMoreElements(
            /* [out] */ Boolean* res);

        CARAPI GetNextElement(
            /* [out] */ IInterface** res);

    private:
        CARAPI_(Boolean) FindNext();

    public:
        AutoPtr<ListResourceBundle> mHost;
        AutoPtr<IIterator> mLocal;
        AutoPtr<IEnumeration> mEnum;
        String mNextElement;
    };

    class _SecondEnumeration
        : public Object
        , public IEnumeration
    {
    public:
        CAR_INTERFACE_DECL()

        _SecondEnumeration(
            /* [in] */ ListResourceBundle* host);

        CARAPI HasMoreElements(
            /* [out] */ Boolean* res);

        CARAPI GetNextElement(
            /* [out] */ IInterface** res);

    private:
        AutoPtr<ListResourceBundle> mHost;
        AutoPtr<IIterator> mIt;
    };

public:
    CAR_INTERFACE_DECL()

    ListResourceBundle();

    CARAPI GetKeys(
        /* [out] */ IEnumeration** enu);

    CARAPI HandleGetObject(
        /* [in] */ String key,
        /* [out] */ IInterface** outface);

protected:
    CARAPI_(AutoPtr<ISet>) HandleKeySet();

    virtual CARAPI_(AutoPtr< ArrayOf< AutoPtr< ArrayOf<IInterface*> > > >) GetContents() = 0;

private:
    CARAPI InitializeTable();

public:
    AutoPtr<IHashMap> mTable;
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_LISTRESOURCEBUNDLE_H__