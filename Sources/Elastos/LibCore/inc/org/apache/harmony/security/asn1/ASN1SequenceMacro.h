
#include "Asn1TypeMacro.h"

#define ASN1SEQUENCE_METHODS_DECL()                                 \
    ASN1TYPE_METHODS_DECL()                                         \
                                                                    \
    CARAPI SetType(                                                 \
        /* [in] */ ArrayOf<IASN1Type *>* type);                     \
                                                                    \
    CARAPI GetType(                                                 \
        /* [out, callee] */ ArrayOf<IASN1Type *>** type);           \
                                                                    \
    CARAPI SetOPTIONAL(                                             \
        /* [in] */ ArrayOf<Boolean>* optional);                     \
                                                                    \
    CARAPI GetOPTIONAL(                                             \
        /* [out, callee] */ ArrayOf<Boolean>** optional);           \
                                                                    \
    CARAPI SetDEFAULT(                                              \
        /* [in] */ ArrayOf<IInterface *>* def);                     \
                                                                    \
    CARAPI GetDEFAULT(                                              \
        /* [out, callee] */ ArrayOf<IInterface *>** def);           \
                                                                    \
    CARAPI GetValues(                                               \
        /* [in] */ IInterface* object,                              \
        /* [in] */ ArrayOf<IInterface*>* values);

#define ASN1SEQUENCE_METHODS_IMPL(className, superClass)             \
    ECode className::GetId(                                          \
        /* [out] */ Int32 *id)                                       \
    {                                                                \
        return superClass::GetId(id);                                \
    }                                                                \
                                                                     \
    ECode className::GetConstrId(                                    \
        /* [out] */ Int32* constrId)                                 \
    {                                                                \
        return superClass::GetConstrId(constrId);                    \
    }                                                                \
                                                                     \
    ECode className::Decode(                                         \
        /* [in] */ ArrayOf<Byte>* encoded,                           \
        /* [out] */ IInterface** object)                             \
    {                                                                \
        return superClass::Decode(encoded, object);                  \
    }                                                                \
                                                                     \
    ECode className::DecodeEx(                                       \
        /* [in] */ ArrayOf<Byte>* encoded,                           \
        /* [in] */ Int32 offset,                                     \
        /* [in] */ Int32 encodingLen,                                \
        /* [out] */ IInterface** object)                             \
    {                                                                \
        return superClass::DecodeEx(encoded, offset, encodingLen, object); \
    }                                                                \
                                                                     \
    ECode className::DecodeEx2(                                      \
        /* [in] */ IInputStream* is,                                 \
        /* [out] */ IInterface** object)                             \
    {                                                                \
        return superClass::DecodeEx2(is, object);                    \
    }                                                                \
                                                                     \
    ECode className::Verify(                                         \
        /* [in] */ ArrayOf<Byte>* encoded)                           \
    {                                                                \
        return superClass::Verify(encoded);                          \
    }                                                                \
                                                                     \
    ECode className::VerifyEx(                                       \
        /* [in] */ IInputStream* is)                                 \
    {                                                                \
        return superClass::VerifyEx(is);                             \
    }                                                                \
                                                                     \
    ECode className::Encode(                                         \
        /* [in] */ IInterface* object,                               \
        /* [out, callee] */ ArrayOf<Byte>** encode)                  \
    {                                                                \
        return superClass::Encode(object, encode);                   \
    }                                                                \
#ifndef FOR_CX509CERTPATHIMPL                                        \
    ECode className::DecodeEx3(                                      \
        /* [in] */ IBerInputStream* bis,                             \
        /* [out] */ IInterface** object)                             \
    {                                                                \
        return superClass::DecodeEx3(bis, object);                   \
    }                                                                \
#endif                                                               \
                                                                     \
    ECode className::CheckTag(                                       \
        /* [in] */ Int32 identifier,                                 \
        /* [out] */ Boolean* checkTag)                               \
    {                                                                \
        return superClass::(identifier, checkTag);                   \
    }                                                                \
                                                                     \
    ECode className::EncodeASN(                                      \
        /* [in] */ IBerOutputStream* bos)                            \
    {                                                                \
        return superClass::EncodeASN(bos);                           \
    }                                                                \
                                                                     \
    ECode className::EncodeContent(                                  \
        /* [in] */ IBerOutputStream* bos)                            \
    {                                                                \
        return superClass::EncodeContent(bos);                       \
    }                                                                \
    ECode className::SetEncodingContent(                             \
        /* [in] */ IBerOutputStream* bos)                            \
    {                                                                \
        return superClass::SetEncodingContent(bos);                  \
    }                                                                \
    ECode className::GetEncodedLength(                               \
        /* [in] */ IBerOutputStream* bos,                            \
        /* [out] */ Int32* length)                                   \
    {                                                                \
        return superClass::GetEncodedLength(bos, length);            \
    }                                                                \
                                                                     \
    ECode className::ToString(                                       \
        /* [out] */ String *result)                                  \
    {                                                                \
        return superClass::ToString(result);                         \
    }                                                                \
    CARAPI className::SetType(                                                 \
        /* [in] */ ArrayOf<IASN1Type *>* type)                      \
    {                                                               \
        return superClass::SetType(type);                           \
    }                                                               \
                                                                    \
    CARAPI className::GetType(                                                 \
        /* [out, callee] */ ArrayOf<IASN1Type *>** type)            \
    {                                                               \
        return superClass::GetType(type);                           \
    }                                                               \
                                                                    \
    CARAPI className::SetOPTIONAL(                                             \
        /* [in] */ ArrayOf<Boolean>* optional)                      \
    {                                                               \
        return superClass::SetOPTIONAL(optional);                   \
    }                                                               \
                                                                    \
    CARAPI className::GetOPTIONAL(                                             \
        /* [out, callee] */ ArrayOf<Boolean>** optional)            \
    {                                                               \
        return superClass::GetOPTIONAL(optional);                   \
    }                                                               \
                                                                    \
    CARAPI className::SetDEFAULT(                                              \
        /* [in] */ ArrayOf<IInterface *>* def)                      \
    {                                                               \
        return superClass::SetDEFAULT(def);                         \
    }                                                               \
                                                                    \
    CARAPI className::GetDEFAULT(                                              \
        /* [out, callee] */ ArrayOf<IInterface *>** def)            \
    {                                                               \
        return superClass::GetDEFAULT(def);                         \
    }


