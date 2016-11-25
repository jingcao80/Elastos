
#include "org/apache/harmony/xml/dom/CDOMConfigurationImpl.h"
#include "org/apache/harmony/xml/dom/CCDATASectionImpl.h"
#include "org/apache/harmony/xml/dom/TextImpl.h"
#include "org/apache/harmony/xml/dom/CCommentImpl.h"
#include "org/apache/harmony/xml/dom/CDOMErrorImpl.h"
#include "org/apache/harmony/xml/dom/CProcessingInstructionImpl.h"
#include "org/apache/harmony/xml/dom/CAttrImpl.h"
#include "elastos/utility/CTreeMap.h"
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Logger.h>

using Org::W3c::Dom::IDOMStringList;
using Org::W3c::Dom::IDOMError;
using Org::W3c::Dom::INode;
using Org::W3c::Dom::INamedNodeMap;
using Org::W3c::Dom::EIID_IDOMStringList;
using Org::W3c::Dom::EIID_IDOMConfiguration;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CTreeMap;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Dom {

CAR_INTERFACE_IMPL(CDOMConfigurationImpl::FixedParameter, Object, IParameter);

CDOMConfigurationImpl::FixedParameter::FixedParameter(
    /* [in] */ IObject* oValue)
    : onlyValue(oValue)
{
}

ECode CDOMConfigurationImpl::FixedParameter::Get(
    /* [in] */ IDOMConfigurationImpl* config,
    /* [out] */ IObject** result)
{
    VALIDATE_NOT_NULL(result);
    *result = onlyValue;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CDOMConfigurationImpl::FixedParameter::Set(
    /* [in] */ IDOMConfigurationImpl* config,
    /* [in] */ IObject* value)
{
    Boolean equals;
    if (!(onlyValue->Equals(value, &equals), equals)) {
        //throw new DOMException(DOMException.NOT_SUPPORTED_ERR, "Unsupported value: " + value);
        Logger::E("CDOMConfigurationImpl", "NOT_SUPPORTED_ERR value");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CDOMConfigurationImpl::FixedParameter::CanSet(
    /* [in] */ IDOMConfigurationImpl* config,
    /* [in] */ IObject* value,
    /* [out] */ Boolean* result)
{
    return onlyValue->Equals(value, result);
}

CAR_INTERFACE_IMPL(CDOMConfigurationImpl::BooleanParameter, Object, IParameter);

ECode CDOMConfigurationImpl::BooleanParameter::CanSet(
    /* [in] */ IDOMConfigurationImpl* config,
    /* [in] */ IObject* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IBoolean::Probe(value) != NULL;
    return NOERROR;
}
//======================
CAR_INTERFACE_IMPL(CDOMConfigurationImpl::InnerDOMStringList, Object, IDOMStringList);

CDOMConfigurationImpl::InnerDOMStringList::InnerDOMStringList(
    /* [in] */ ArrayOf<String>* keys)
    : result(keys)
{
}

ECode CDOMConfigurationImpl::InnerDOMStringList::Item(
   /* [in] */ Int32 index,
   /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);
    if (index < result->GetLength()) {
        *str = (*result)[index];
    }
    return NOERROR;
}

ECode CDOMConfigurationImpl::InnerDOMStringList::GetLength(
   /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = result->GetLength();
    return NOERROR;
}

ECode CDOMConfigurationImpl::InnerDOMStringList::Contains(
    /* [in] */ const String& str,
    /* [out] */ Boolean* value)
{
    return CDOMConfigurationImpl::PARAMETERS->ContainsKey(CoreUtils::Convert(str), value);
}

//===================

CAR_OBJECT_IMPL(CDOMConfigurationImpl);
CAR_INTERFACE_IMPL_2(CDOMConfigurationImpl, Object, IDOMConfiguration, IDOMConfigurationImpl);

static AutoPtr<IMap> InitMap()
{
    AutoPtr<IMap> parameters;
    CTreeMap::New(/*TODO String.CASE_INSENSITIVE_ORDER */ (IMap**)&parameters);
    //TODO
    ///*
    // * True to canonicalize the document (unsupported). This includes
    // * removing DocumentType nodes from the tree and removing unused
    // * namespace declarations. Setting this to true also sets these
    // * parameters:
    // *   entities = false
    // *   normalize-characters = false
    // *   cdata-sections = false
    // *   namespaces = true
    // *   namespace-declarations = true
    // *   well-formed = true
    // *   element-content-whitespace = true
    // * Setting these parameters to another value shall revert the canonical
    // * form to false.
    // */
    //PARAMETERS.put("canonical-form", new FixedParameter(false));

    ///*
    // * True to keep existing CDATA nodes; false to replace them/merge them
    // * into adjacent text nodes.
    // */
    //PARAMETERS.put("cdata-sections", new BooleanParameter() {
    //        public Object get(DOMConfigurationImpl config) {
    //        return config.cdataSections;
    //        }
    //        public void set(DOMConfigurationImpl config, Object value) {
    //        config.cdataSections = (Boolean) value;
    //        }
    //        });

    ///*
    // * True to check character normalization (unsupported).
    // */
    //PARAMETERS.put("check-character-normalization", new FixedParameter(false));

    ///*
    // * True to keep comments in the document; false to discard them.
    // */
    //PARAMETERS.put("comments", new BooleanParameter() {
    //        public Object get(DOMConfigurationImpl config) {
    //        return config.comments;
    //        }
    //        public void set(DOMConfigurationImpl config, Object value) {
    //        config.comments = (Boolean) value;
    //        }
    //        });

    ///*
    // * True to expose schema normalized values. Setting this to true sets
    // * the validate parameter to true. Has no effect when validate is false.
    // */
    //PARAMETERS.put("datatype-normalization", new BooleanParameter() {
    //        public Object get(DOMConfigurationImpl config) {
    //        return config.datatypeNormalization;
    //        }
    //        public void set(DOMConfigurationImpl config, Object value) {
    //        if ((Boolean) value) {
    //        config.datatypeNormalization = true;
    //        config.validate = true;
    //        } else {
    //        config.datatypeNormalization = false;
    //        }
    //        }
    //        });

    ///*
    // * True to keep whitespace elements in the document; false to discard
    // * them (unsupported).
    // */
    //PARAMETERS.put("element-content-whitespace", new FixedParameter(true));

    ///*
    // * True to keep entity references in the document; false to expand them.
    // */
    //PARAMETERS.put("entities", new BooleanParameter() {
    //        public Object get(DOMConfigurationImpl config) {
    //        return config.entities;
    //        }
    //        public void set(DOMConfigurationImpl config, Object value) {
    //        config.entities = (Boolean) value;
    //        }
    //        });

    ///*
    // * Handler to be invoked when errors are encountered.
    // */
    //PARAMETERS.put("error-handler", new Parameter() {
    //        public Object get(DOMConfigurationImpl config) {
    //        return config.errorHandler;
    //        }
    //        public void set(DOMConfigurationImpl config, Object value) {
    //        config.errorHandler = (DOMErrorHandler) value;
    //        }
    //        public boolean canSet(DOMConfigurationImpl config, Object value) {
    //        return value == null || value instanceof DOMErrorHandler;
    //        }
    //        });

    ///*
    // * Bulk alias to set the following parameter values:
    // *   validate-if-schema = false
    // *   entities = false
    // *   datatype-normalization = false
    // *   cdata-sections = false
    // *   namespace-declarations = true
    // *   well-formed = true
    // *   element-content-whitespace = true
    // *   comments = true
    // *   namespaces = true.
    // * Querying this returns true if all of the above parameters have the
    // * listed values; false otherwise.
    // */
    //PARAMETERS.put("infoset", new BooleanParameter() {
    //        public Object get(DOMConfigurationImpl config) {
    //        // validate-if-schema is always false
    //        // element-content-whitespace is always true
    //        // namespace-declarations is always true
    //        return !config.entities
    //        && !config.datatypeNormalization
    //        && !config.cdataSections
    //        && config.wellFormed
    //        && config.comments
    //        && config.namespaces;
    //        }
    //        public void set(DOMConfigurationImpl config, Object value) {
    //        if ((Boolean) value) {
    //        // validate-if-schema is always false
    //        // element-content-whitespace is always true
    //        // namespace-declarations is always true
    //        config.entities = false;
    //        config.datatypeNormalization = false;
    //        config.cdataSections = false;
    //        config.wellFormed = true;
    //        config.comments = true;
    //        config.namespaces = true;
    //        }
    //        }
    //});

    ///*
    // * True to perform namespace processing; false for none.
    // */
    //PARAMETERS.put("namespaces", new BooleanParameter() {
    //        public Object get(DOMConfigurationImpl config) {
    //        return config.namespaces;
    //        }
    //        public void set(DOMConfigurationImpl config, Object value) {
    //        config.namespaces = (Boolean) value;
    //        }
    //        });

    ///**
    // * True to include namespace declarations; false to discard them
    // * (unsupported). Even when namespace declarations are discarded,
    // * prefixes are retained.
    // *
    // * Has no effect if namespaces is false.
    // */
    //PARAMETERS.put("namespace-declarations", new FixedParameter(true));

    ///*
    // * True to fully normalize characters (unsupported).
    // */
    //PARAMETERS.put("normalize-characters", new FixedParameter(false));

    ///*
    // * A list of whitespace-separated URIs representing the schemas to validate
    // * against. Has no effect if schema-type is null.
    // */
    //PARAMETERS.put("schema-location", new Parameter() {
    //        public Object get(DOMConfigurationImpl config) {
    //        return config.schemaLocation;
    //        }
    //        public void set(DOMConfigurationImpl config, Object value) {
    //        config.schemaLocation = (String) value;
    //        }
    //        public boolean canSet(DOMConfigurationImpl config, Object value) {
    //        return value == null || value instanceof String;
    //        }
    //        });

    ///*
    // * URI representing the type of schema language, such as
    // * "http://www.w3.org/2001/XMLSchema" or "http://www.w3.org/TR/REC-xml".
    // */
    //PARAMETERS.put("schema-type", new Parameter() {
    //        public Object get(DOMConfigurationImpl config) {
    //        return config.schemaType;
    //        }
    //        public void set(DOMConfigurationImpl config, Object value) {
    //        config.schemaType = (String) value;
    //        }
    //        public boolean canSet(DOMConfigurationImpl config, Object value) {
    //        return value == null || value instanceof String;
    //        }
    //        });

    ///*
    // * True to split CDATA sections containing "]]>"; false to signal an
    // * error instead.
    // */
    //PARAMETERS.put("split-cdata-sections", new BooleanParameter() {
    //        public Object get(DOMConfigurationImpl config) {
    //        return config.splitCdataSections;
    //        }
    //        public void set(DOMConfigurationImpl config, Object value) {
    //        config.splitCdataSections = (Boolean) value;
    //        }
    //        });

    ///*
    // * True to require validation against a schema or DTD. Validation will
    // * recompute element content whitespace, ID and schema type data.
    // *
    // * Setting this unsets validate-if-schema.
    // */
    //PARAMETERS.put("validate", new BooleanParameter() {
    //        public Object get(DOMConfigurationImpl config) {
    //        return config.validate;
    //        }
    //        public void set(DOMConfigurationImpl config, Object value) {
    //        // validate-if-schema is always false
    //        config.validate = (Boolean) value;
    //        }
    //        });

    ///*
    // * True to validate if a schema was declared (unsupported). Setting this
    // * unsets validate.
    // */
    //PARAMETERS.put("validate-if-schema", new FixedParameter(false));

    ///*
    // * True to report invalid characters in node names, attributes, elements,
    // * comments, text, CDATA sections and processing instructions.
    // */
    //PARAMETERS.put("well-formed", new BooleanParameter() {
    //        public Object get(DOMConfigurationImpl config) {
    //        return config.wellFormed;
    //        }
    //        public void set(DOMConfigurationImpl config, Object value) {
    //        config.wellFormed = (Boolean) value;
    //        }
    //        });

    //// TODO add "resource-resolver" property for use with LS feature...
    return parameters;
}

AutoPtr<IMap> CDOMConfigurationImpl::PARAMETERS = InitMap();

CDOMConfigurationImpl::CDOMConfigurationImpl()
    : cdataSections(TRUE)
    , comments(TRUE)
    , datatypeNormalization(FALSE)
    , entities(TRUE)
    , namespaces(TRUE)
    , splitCdataSections(TRUE)
    , validate(FALSE)
    , wellFormed(TRUE)
{
}

ECode CDOMConfigurationImpl::SetParameter(
    /* [in] */ const String& name,
    /* [in] */ IObject * pValue)
{
    //Parameter parameter = PARAMETERS.get(name);
    AutoPtr<IInterface> value;
    PARAMETERS->Get(CoreUtils::Convert(name), (IInterface**)&value);
    IParameter* parameter = IParameter::Probe(value);
    if (parameter == NULL) {
        //throw new DOMException(DOMException.NOT_FOUND_ERR, "No such parameter: " + name);
        Logger::E("CDOMConfigurationImpl", "SetParameter, NOT_FOUND_ERR");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    //try {
    parameter->Set(this, pValue);
    //} catch (NullPointerException e) {
    //    throw new DOMException(DOMException.TYPE_MISMATCH_ERR,
    //            "Null not allowed for " + name);
    //} catch (ClassCastException e) {
    //    throw new DOMException(DOMException.TYPE_MISMATCH_ERR,
    //            "Invalid type for " + name + ": " + value.getClass());
    //}
    return NOERROR;
}

ECode CDOMConfigurationImpl::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ IObject ** ppObj)
{
    //Parameter parameter = PARAMETERS.get(name);
    AutoPtr<IInterface> value;
    PARAMETERS->Get(CoreUtils::Convert(name), (IInterface**)&value);
    IParameter* parameter = IParameter::Probe(value);
    if (parameter == NULL) {
        //throw new DOMException(DOMException.NOT_FOUND_ERR, "No such parameter: " + name);
        Logger::E("CDOMConfigurationImpl", "GetParameter, NOT_FOUND_ERR");
        assert(0);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return parameter->Get(this, ppObj);
}

ECode CDOMConfigurationImpl::CanSetParameter(
    /* [in] */ const String& name,
    /* [in] */ IObject * pValue,
    /* [out] */ Boolean * pReslut)
{
    VALIDATE_NOT_NULL(pReslut);
    AutoPtr<IInterface> value;
    PARAMETERS->Get(CoreUtils::Convert(name), (IInterface**)&value);
    IParameter* parameter = IParameter::Probe(value);
    Boolean canSet;
    *pReslut = parameter != NULL && (parameter->CanSet(this, pValue, &canSet), canSet);
    return NOERROR;
}

ECode CDOMConfigurationImpl::GetParameterNames(
    /* [out] */ Org::W3c::Dom::IDOMStringList ** ppDomlist)
{
    //final String[] result = PARAMETERS.keySet().toArray(new String[PARAMETERS.size()]);
    AutoPtr<ISet> set;
    PARAMETERS->GetKeySet((ISet**)&set);
    AutoPtr<ArrayOf<IInterface*> > outArray;
    Int32 size;
    PARAMETERS->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > inArray = ArrayOf<IInterface*>::Alloc(size);
    set->ToArray(inArray, (ArrayOf<IInterface*>**)&outArray);

    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<ICharSequence> ics = ICharSequence::Probe((*outArray)[i]);
        String value;
        ics->ToString(&value);
        result->Set(i, value);
    }

    AutoPtr<InnerDOMStringList> dl = new InnerDOMStringList(result);
    *ppDomlist = dl.Get();
    REFCOUNT_ADD(*ppDomlist);
    return NOERROR;
}

ECode CDOMConfigurationImpl::Normalize(
    /* [in] */ Org::W3c::Dom::INode * node)
{
    /*
     * Since we don't validate, this code doesn't take into account the
     * following "supported" parameters: datatype-normalization, entities,
     * schema-location, schema-type, or validate.
     *
     * TODO: normalize namespaces
     */

    Int16 nodeType;
    node->GetNodeType(&nodeType);
    switch (nodeType) {
        case INode::CDATA_SECTION_NODE:
        {
            AutoPtr<CCDATASectionImpl> cdata = (CCDATASectionImpl*) node;
            if (cdataSections) {
                Boolean needsSplitting;
                if (cdata->NeedsSplitting(&needsSplitting), needsSplitting) {
                    if (splitCdataSections) {
                        cdata->Split();
                        Report(IDOMError::IDOMError_SEVERITY_WARNING, String("cdata-sections-splitted"));
                    } else {
                        Report(IDOMError::IDOMError_SEVERITY_ERROR, String("wf-invalid-character"));
                    }
                }
                CheckTextValidity(cdata->buffer);
                break;
            }
            AutoPtr<IText> temp;
            cdata->ReplaceWithText((IText**)&temp);
            node = Org::W3c::Dom::INode::Probe(temp);
            // fall through
        }
        case INode::TEXT_NODE:
        {
            AutoPtr<TextImpl> text = (TextImpl*) node;
            AutoPtr<TextImpl> tmp;
            text->Minimize((TextImpl**)&tmp);
            if (tmp != NULL) {
                text = tmp;
                CheckTextValidity(text->buffer);
            }
            break;
        }
        case INode::COMMENT_NODE:
        {
            AutoPtr<CCommentImpl> comment = (CCommentImpl*) node;
            if (!comments) {
                AutoPtr<INode> node;
                comment->GetParentNode((INode**)&node);
                AutoPtr<INode> tmp;
                node->RemoveChild(comment, (INode**)&tmp);
                break;
            }
            Boolean containsDashDash;
            if (comment->ContainsDashDash(&containsDashDash), containsDashDash) {
                Report(IDOMError::IDOMError_SEVERITY_ERROR, String("wf-invalid-character"));
            }
            CheckTextValidity(comment->buffer);
            break;
        }
        case INode::PROCESSING_INSTRUCTION_NODE:
        {
            AutoPtr<CProcessingInstructionImpl> pii = (CProcessingInstructionImpl*)node;
            String data;
            pii->GetData(&data);
            CheckTextValidity(CoreUtils::Convert(data));
            break;
        }
        case INode::ATTRIBUTE_NODE:
        {
            AutoPtr<CAttrImpl> ai = (CAttrImpl*)node;
            String value;
            ai->GetValue(&value);
            CheckTextValidity(CoreUtils::Convert(value));
            break;
        }
        case INode::ELEMENT_NODE:
        {
            AutoPtr<CElementImpl> element = (CElementImpl*) node;
            AutoPtr<INamedNodeMap> attributes;
            element->GetAttributes((INamedNodeMap**)&attributes);
            Int32 length;
            attributes->GetLength(&length);
            for (Int32 i = 0; i < length; ++i) {
                AutoPtr<INode> inode;
                attributes->Item(i, (INode**)&inode);
                Normalize(inode);
            }
            // fall through
        }
        case INode::DOCUMENT_NODE:
        case INode::DOCUMENT_FRAGMENT_NODE:
        {
            AutoPtr<INode> next, child;
            for (node->GetFirstChild((INode**)&child); child != NULL; child = next) {
                // lookup next eagerly because normalize() may remove its subject
                child->GetNextSibling((INode**)&next);
                Normalize(child);
            }
            break;
        }
        case INode::NOTATION_NODE:
        case INode::DOCUMENT_TYPE_NODE:
        case INode::ENTITY_NODE:
        case INode::ENTITY_REFERENCE_NODE:
            break;

        default:
            //throw new DOMException(DOMException.NOT_SUPPORTED_ERR, "Unsupported node type " + node.getNodeType());
            Logger::E("CDOMConfigurationImpl", "Normalize Unsupported node type: %d", nodeType);
            assert(0);
            return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

void CDOMConfigurationImpl::CheckTextValidity(
    /* [in] */ ICharSequence* s)
{
    if (wellFormed && !IsValid(s)) {
        Report(IDOMError::IDOMError_SEVERITY_ERROR, String("wf-invalid-character"));
    }
}

Boolean CDOMConfigurationImpl::IsValid(
    /* [in] */ ICharSequence* text)
{
    Int32 length;
    text->GetLength(&length);
    for (Int32 i = 0; i < length; ++i) {
        String str;
        text->ToString(&str);
        Char32 c = str.GetChar(i);
        // as defined by http://www.w3.org/TR/REC-xml/#charsets.
        //TODO
        Boolean valid = c == 0x9 || c == 0xA || c == 0xD
            || (c >= 0x20 && c <= 0xd7ff)
            || (c >= 0xe000 && c <= 0xfffd);
        if (!valid) {
            return FALSE;
        }
    }
    return TRUE;
}

void CDOMConfigurationImpl::Report(
    /* [in] */ Int16 severity,
    /* [in] */ const String& type)
{
    if (errorHandler != NULL) {
        // TODO: abort if handleError returns false
        AutoPtr<CDOMErrorImpl> dei = new CDOMErrorImpl();
        dei->constructor(severity, type);
        Boolean result;
        errorHandler->HandleError(dei, &result);
    }
}

}
}
}
}
}

