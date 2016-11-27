//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CANNOTATIONINFO_H__
#define __CANNOTATIONINFO_H__

#include "CEntryList.h"

class CAnnotationInfo
    : public ElLightRefBase
    , public IAnnotationInfo
{
public:
    CAnnotationInfo(
        /* [in] */ AnnotationDescriptor* annoDesc);

    virtual ~CAnnotationInfo();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetNamespace(
        /* [out] */ String* ns);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI GetKeys(
        /* [out, callee] */ ArrayOf<String>** keys);

    CARAPI GetValue(
        /* [in] */ const String& key,
        /* [out] */ String* value);

    CARAPI GetValues(
        /* [out, callee] */ ArrayOf<String>** values);

private:
    CARAPI_(void) AcquireKeys();

    CARAPI_(void) AcquireValues();

private:
    AnnotationDescriptor* mAnnotationDescriptor;
    AutoPtr< ArrayOf<String> > mKeys;
    AutoPtr< ArrayOf<String> > mValues;
};

#endif // __CANNOTATIONINFO_H__
