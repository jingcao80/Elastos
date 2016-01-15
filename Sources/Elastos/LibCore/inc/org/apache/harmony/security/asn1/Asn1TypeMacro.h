
#ifndef __ASN1TYPEMACRO_H__
#define __ASN1TYPEMACRO_H__

#define ASN1TYPE_METHODS_DECL()                                  \
    CARAPI GetId(                                                \
        /* [out] */ Int32* id);                                  \
                                                                 \
    CARAPI GetConstrId(                                          \
        /* [out] */ Int32* constrId);                            \
                                                                 \
    CARAPI Decode(                                               \
        /* [in] */ ArrayOf<Byte>* encoded,                       \
        /* [out] */ IInterface** object);                        \
                                                                 \
    CARAPI DecodeEx(                                             \
        /* [in] */ ArrayOf<Byte>* encoded,                       \
        /* [in] */ Int32 offset,                                 \
        /* [in] */ Int32 encodingLen,                            \
        /* [out] */ IInterface** object);                        \
                                                                 \
    CARAPI DecodeEx2(                                            \
        /* [in] */ IInputStream* is,                             \
        /* [out] */ IInterface** object);                        \
                                                                 \
    CARAPI Verify(                                               \
        /* [in] */ ArrayOf<Byte>* encoded);                      \
                                                                 \
    CARAPI VerifyEx(                                             \
        /* [in] */ IInputStream* is);                            \
                                                                 \
    CARAPI Encode(                                               \
        /* [in] */ IInterface* object,                           \
        /* [out, callee] */ ArrayOf<Byte>** encode);             \
                                                                 \
    CARAPI DecodeEx3(                                            \
        /* [in] */ IBerInputStream* bis,                         \
        /* [out] */ IInterface** object);                        \
                                                                 \
    CARAPI CheckTag(                                             \
        /* [in] */ Int32 identifier,                             \
        /* [out] */ Boolean* checkTag);                          \
                                                                 \
    CARAPI GetDecodedObject(                                     \
        /* [in] */ IBerInputStream* bis,                         \
        /* [out] */ IInterface** object);                        \
                                                                 \
    CARAPI EncodeASN(                                            \
        /* [in] */ IBerOutputStream* bos);                       \
                                                                 \
    CARAPI EncodeContent(                                        \
        /* [in] */ IBerOutputStream* bos);                       \
                                                                 \
    CARAPI SetEncodingContent(                                   \
        /* [in] */ IBerOutputStream* bos);                       \
                                                                 \
    CARAPI GetEncodedLength(                                     \
        /* [in] */ IBerOutputStream* bos,                        \
        /* [out] */ Int32* length);                              \
                                                                 \
    CARAPI ToString(                                             \
        /* [out] */ String* result);                             \


#define ASN1TYPE_METHODS_IMPL(className, superClass)             \
                                                                 \
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
                                                                 \
ECode className::DecodeEx3(                                      \
    /* [in] */ IBerInputStream* bis,                             \
    /* [out] */ IInterface** object)                             \
{                                                                \
    return superClass::DecodeEx3(bis, object);                   \
}                                                                \
                                                                 \
ECode className::CheckTag(                                       \
    /* [in] */ Int32 identifier,                                 \
    /* [out] */ Boolean* checkTag)                               \
{                                                                \
    return superClass::(identifier, checkTag);                   \
}                                                                \
#ifndef FOR_DERIVED                                              \
ECode className::GetDecodedObject(                               \
    /* [in] */ IBerInputStream* bis,                             \
    /* [out] */ IInterface** object)                             \
{                                                                \
    return superClass::GetDecodedObject(bis, object);            \
}                                                                \
#endif                                                           \
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
#ifndef FOR_DERIVED                                              \
ECode className::SetEncodingContent(                             \
    /* [in] */ IBerOutputStream* bos)                            \
{                                                                \
    return superClass::SetEncodingContent(bos);                  \
}                                                                \
#endif                                                           \
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
}

#endif  //__ASN1TYPEMACRO_H__