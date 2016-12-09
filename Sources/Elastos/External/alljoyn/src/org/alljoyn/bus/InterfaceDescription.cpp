#include "org/alljoyn/bus/InterfaceDescription.h"
#include "org/alljoyn/bus/NativeApi.h"
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include "org/alljoyn/bus/NativeTranslator.h"
#include "org/alljoyn/bus/Signature.h"
#include "org/alljoyn/bus/Translator.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <alljoyn/AllJoynStd.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/InterfaceDescription.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

const Int32 InterfaceDescription::INVALID;
const Int32 InterfaceDescription::METHOD_CALL;
const Int32 InterfaceDescription::SIGNAL;
const Int32 InterfaceDescription::READ;
const Int32 InterfaceDescription::WRITE;
const Int32 InterfaceDescription::RW;
const Int32 InterfaceDescription::AJ_IFC_SECURITY_INHERIT;
const Int32 InterfaceDescription::AJ_IFC_SECURITY_REQUIRED;
const Int32 InterfaceDescription::AJ_IFC_SECURITY_OFF;
const String InterfaceDescription::TAG("InterfaceDescription");

CAR_INTERFACE_IMPL(InterfaceDescription, Object, IInterfaceDescription);

InterfaceDescription::InterfaceDescription()
    : mAnnounced(FALSE)
{}

ECode InterfaceDescription::Create(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ const String& name,
    /* [in] */ Int32 securePolicy,
    /* [in] */ Int32 numProps,
    /* [in] */ Int32 numMembers)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(busAttachment->mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "Create(): NULL bus pointer");
        return E_STATUS_FAIL;
    }

    ajn::InterfaceDescription* intf;
    QStatus status = busPtr->CreateInterface(name.string(), intf, static_cast<ajn::InterfaceSecurityPolicy>(securePolicy));
    if (ER_BUS_IFACE_ALREADY_EXISTS == status) {
        /*
         * We know that an interface exists with the same name, but it may differ in other parameters,
         * so confirm that other parameters are the same too before returning ER_OK.
         *
         * Note that we're not checking members or properties here, that check will be done later in
         * addMember and addProperty.
         */
        intf = (ajn::InterfaceDescription*)busPtr->GetInterface(name.string());
        assert(intf);
        if ((intf->GetSecurityPolicy() == static_cast<ajn::InterfaceSecurityPolicy>(securePolicy)) &&
            (intf->GetProperties() == (size_t)numProps) &&
            (intf->GetMembers() == (size_t)numMembers)) {
            status = ER_OK;
        }
        /*
         * When using org.freedesktop.DBus interfaces, we treat them as a special
         * case to remain backwards compatible. It cannot add the 'off' security
         * annotation. However, to work properly with object security, it must
         * still report its interface security as 'off'.
         */
        bool isDBusStandardIfac;
        if (name.string() == NULL) {     // passing NULL into strcmp is undefined behavior.
            isDBusStandardIfac = false;
        }
        else {
            isDBusStandardIfac = (strcmp(ajn::org::freedesktop::DBus::Introspectable::InterfaceName, name.string()) == 0) ||
                                 (strcmp(ajn::org::freedesktop::DBus::Peer::InterfaceName, name.string()) == 0) ||
                                 (strcmp(ajn::org::freedesktop::DBus::Properties::InterfaceName, name.string()) == 0) ||
                                 (strcmp(ajn::org::allseen::Introspectable::InterfaceName, name.string()) == 0);
        }
        if ((status != ER_OK) &&
            ((isDBusStandardIfac &&
              (intf->GetSecurityPolicy() == static_cast<ajn::InterfaceSecurityPolicy>(org_alljoyn_bus_InterfaceDescription_AJ_IFC_SECURITY_OFF))) ||
             (strcmp(ajn::org::alljoyn::Bus::Security::Application::InterfaceName, name.string()) == 0) ||
             (strcmp(ajn::org::alljoyn::Bus::Security::ClaimableApplication::InterfaceName, name.string()) == 0) ||
             (strcmp(ajn::org::alljoyn::Bus::Security::ManagedApplication::InterfaceName, name.string()) == 0) ||
             (strcmp(ajn::org::alljoyn::Bus::Application::InterfaceName, name.string()) == 0)
            )
            ) {
            status = ER_OK;
        }
    }
    if (ER_OK == status) {
        mHandle = reinterpret_cast<Int64>(intf);
    }

    return status;
}

ECode InterfaceDescription::AddMember(
    /* [in] */ Int32 type,
    /* [in] */ const String& name,
    /* [in] */ const String& inputSig,
    /* [in] */ const String& outSig,
    /* [in] */ Int32 annotation,
    /* [in] */ const String& accessPerm)
{
    ajn::InterfaceDescription* intf = reinterpret_cast<ajn::InterfaceDescription*>(mHandle);
    assert(intf);

    QStatus status = intf->AddMember((ajn::AllJoynMessageType)type, name.string(), inputSig.string(),
            outSig.string(), NULL, annotation, accessPerm.string());
    if (ER_BUS_MEMBER_ALREADY_EXISTS == status || ER_BUS_INTERFACE_ACTIVATED == status) {
        /*
         * We know that a member exists with the same name, but check that the other parameters
         * are identical as well before returning ER_OK.  See also the comment in create above.
         */
        const ajn::InterfaceDescription::Member* member = intf->GetMember(name.string());
        if (member &&
            (member->memberType == (ajn::AllJoynMessageType)type) &&
            (name.string() && member->name == name.string()) &&
            (inputSig.string() && member->signature == inputSig.string()) &&
            (outSig.string() && member->returnSignature == outSig.string())) {

            // for reverse compatibility:
            // two annotations can be represented in the int variable 'annotation': DEPRECATED and NOREPLY
            // make sure these int values matches with what's in the full annotations map
            bool annotations_match = true;
            if (annotation & ajn::MEMBER_ANNOTATE_DEPRECATED) {
                qcc::String val;
                if (!member->GetAnnotation(ajn::org::freedesktop::DBus::AnnotateDeprecated, val) || val != "true") {
                    annotations_match = false;
                }
            }

            if (annotation & ajn::MEMBER_ANNOTATE_NO_REPLY) {
                qcc::String val;
                if (!member->GetAnnotation(ajn::org::freedesktop::DBus::AnnotateNoReply, val) || val != "true") {
                    annotations_match = false;
                }
            }

            if (annotations_match) {
                status = ER_OK;
            }
        }
    }
    return status;
}

ECode InterfaceDescription::AddMemberAnnotation(
    /* [in] */ const String& member,
    /* [in] */ const String& annotation,
    /* [in] */ const String& value)
{
    ajn::InterfaceDescription* intf = reinterpret_cast<ajn::InterfaceDescription*>(mHandle);
    assert(intf);

    return intf->AddMemberAnnotation(member.string(), annotation.string(), value.string());
}

ECode InterfaceDescription::AddProperty(
    /* [in] */ const String& name,
    /* [in] */ const String& signature,
    /* [in] */ Int32 access,
    /* [in] */ Int32 annotation)
{
    ajn::InterfaceDescription* intf = reinterpret_cast<ajn::InterfaceDescription*>(mHandle);
    assert(intf);

    QStatus status = intf->AddProperty(name.string(), signature.string(), access);
    if (ER_BUS_PROPERTY_ALREADY_EXISTS == status || ER_BUS_INTERFACE_ACTIVATED == status) {
        /*
         * We know that a property exists with the same name, but check that the other parameters
         * are identical as well before returning ER_OK.  See also the comment in create above.
         */
        const ajn::InterfaceDescription::Property* prop = intf->GetProperty(name.string());
        if (prop &&
            (name.string() && prop->name == name.string()) &&
            (signature.string() && prop->signature == signature.string()) &&
            (prop->access == access)) {

            // for reverse compatibility:
            // two annotations can be represented in the int variable 'annotation': EMIT_CHANGED_SIGNAL and EMIT_CHANGED_SIGNAL_INVALIDATES
            // make sure these int values matches with what's in the full annotations map
            bool annotations_match = true;
            if (annotation & ajn::PROP_ANNOTATE_EMIT_CHANGED_SIGNAL) {
                qcc::String val;
                if (!prop->GetAnnotation(ajn::org::freedesktop::DBus::AnnotateEmitsChanged, val) || val != "true") {
                    annotations_match = false;
                }
            }

            if (annotation & ajn::PROP_ANNOTATE_EMIT_CHANGED_SIGNAL_INVALIDATES) {
                qcc::String val;
                if (!prop->GetAnnotation(ajn::org::freedesktop::DBus::AnnotateEmitsChanged, val) || val != "invalidates") {
                    annotations_match = false;
                }
            }

            if (annotations_match) {
                status = ER_OK;
            }
        }
    }
    return status;
}

ECode InterfaceDescription::AddPropertyAnnotation(
    /* [in] */ const String& property,
    /* [in] */ const String& annotation,
    /* [in] */ const String& value)
{
    ajn::InterfaceDescription* intf = reinterpret_cast<ajn::InterfaceDescription*>(mHandle);
    assert(intf);

    QStatus status = intf->AddPropertyAnnotation(property.string(), annotation.string(), value.string());
    return status;
}

ECode InterfaceDescription::AddAnnotation(
    /* [in] */ const String& annotation,
    /* [in] */ const String& value)
{
    ajn::InterfaceDescription* intf = reinterpret_cast<ajn::InterfaceDescription*>(mHandle);
    assert(intf);

    QStatus status = intf->AddAnnotation(annotation.string(), value.string());
    return status;
}

void InterfaceDescription::SetDescriptionLanguage(
    /* [in] */ const String& language)
{
    ajn::InterfaceDescription* intf = reinterpret_cast<ajn::InterfaceDescription*>(mHandle);
    assert(intf);

    intf->SetDescriptionLanguage(language.string());
}

void InterfaceDescription::SetDescription(
    /* [in] */ const String& description)
{
    ajn::InterfaceDescription* intf = reinterpret_cast<ajn::InterfaceDescription*>(mHandle);
    assert(intf);

    intf->SetDescription(description.string());
}

void InterfaceDescription::SetDescriptionTranslator(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ ITranslator* dt)
{
    ajn::InterfaceDescription* intf = reinterpret_cast<ajn::InterfaceDescription*>(mHandle);
    assert(intf);

    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(busAttachment->mHandle);

    /*
     * We don't want to force the user to constantly check for NULL return
     * codes, so if we have a problem, we throw an exception.
     */
    if (busPtr == NULL) {
        Logger::E(TAG, "InterfaceDescription_setDescriptionTranslator(): NULL bus pointer");
        return;
    }

    NativeTranslator* translator = NULL;
    if (dt) {
        busPtr->mBaCommonLock.Lock();
        busPtr->mTranslators.PushBack(dt);
        busPtr->mBaCommonLock.Unlock();

        /*
         * Get the C++ object that must be there backing the Java object
         */
        translator = reinterpret_cast<NativeTranslator*>(((Translator*)dt)->mHandle);
        assert(translator);
    }
    /*
     * Make the call into AllJoyn.
     */
    intf->SetDescriptionTranslator(translator);
}

ECode InterfaceDescription::SetMemberDescription(
    /* [in] */ const String& member,
    /* [in] */ const String& description,
    /* [in] */ Boolean isSessionlessSignal)
{
    ajn::InterfaceDescription* intf = reinterpret_cast<ajn::InterfaceDescription*>(mHandle);
    assert(intf);

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#if defined(QCC_OS_GROUP_WINDOWS)
#pragma warning(push)
#pragma warning(disable: 4996)
#endif
    /*
     * This call to a deprecated C++ function is required to support the
     * deprecated Java API.  This should be removed for the 17.10 release.
     */
    QStatus status = intf->SetMemberDescription(member.string(), description.string(), isSessionlessSignal);
#if defined(QCC_OS_GROUP_WINDOWS)
#pragma warning(pop)
#endif
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
    return status;
}

ECode InterfaceDescription::SetPropertyDescription(
    /* [in] */ const String& propName,
    /* [in] */ const String& description)
{
    ajn::InterfaceDescription* intf = reinterpret_cast<ajn::InterfaceDescription*>(mHandle);
    assert(intf);

    QStatus status = intf->SetPropertyDescription(propName.string(), description.string());
    return status;
}

void InterfaceDescription::Activate()
{
    ajn::InterfaceDescription* intf = reinterpret_cast<ajn::InterfaceDescription*>(mHandle);
    assert(intf);
    intf->Activate();
}

AutoPtr<IMethodInfo> InterfaceDescription::GetMember(
    /* [in] */ const String& name)
{
    List< AutoPtr<IMethodInfo> >::Iterator it = mMembers.Begin();
    for (; it != mMembers.End(); ++it) {
        IMethodInfo* m = *it;
        if (GetName(m).Equals(name))
            return m;
    }
    return NULL;
}

/**
 * Called by the native code when registering bus objects to obtain the property
 * implementations.
 */
AutoPtr<ArrayOf<IMethodInfo*> > InterfaceDescription::GetProperty(
    /* [in] */ const String& name)
{
    HashMap<String, AutoPtr<Property> >::Iterator it = mProperties.Begin();
    for (; it != mProperties.End(); ++it) {
        Property* p = it->mSecond;
        if (p->mName.Equals(name)) {
            AutoPtr<ArrayOf<IMethodInfo*> > array = ArrayOf<IMethodInfo*>::Alloc(2);
            array->Set(0, p->mGet);
            array->Set(1, p->mSet);
            return array;
        }
    }
    return NULL;
}

ECode InterfaceDescription::Create(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ IInterfaceInfo* busInterface)
{
    ECode status = GetProperties(busInterface);
    if (status != E_STATUS_OK) {
        return status;
    }
    ECode ec = GetMembers(busInterface);
    if (ec != E_STATUS_OK) {
        return ec;
    }

    Int32 securePolicy = AJ_IFC_SECURITY_INHERIT;
    AutoPtr<IAnnotationInfo> secureAnnotation;
    busInterface->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.Secure"), (IAnnotationInfo**)&secureAnnotation);
    if (secureAnnotation != NULL) {
        String value;
        secureAnnotation->GetValue(String("value"), &value);
        if (value.Equals("required")) {
            securePolicy = AJ_IFC_SECURITY_REQUIRED;
        }
        else if (value.Equals("off")) {
            securePolicy = AJ_IFC_SECURITY_OFF;
        }
        else {
            /*
             * In C++ if an interface provides an unknown security annotation
             * it automatically defaults to the inherit for security. For
             * that reason the Java code will do the same.
             */
            securePolicy = AJ_IFC_SECURITY_INHERIT;
        }
    }
    else {
        securePolicy = AJ_IFC_SECURITY_INHERIT;
    }
    ec = Create(busAttachment, GetName(busInterface), securePolicy,
            mProperties.GetSize(), mMembers.GetSize());
    if (ec != E_STATUS_OK) {
        return ec;
    }
    ec = AddProperties(busInterface);
    if (ec != E_STATUS_OK) {
        return ec;
    }
    ec = AddMembers(busInterface);
    if (ec != E_STATUS_OK) {
        return ec;
    }

    // now we need to add the DBus annotations for the interface;
    // this must be done *before* calling create
    // TODO:
    // BusAnnotations busAnnotations = busInterface.getAnnotation(BusAnnotations.class);
    // if (busAnnotations != null) {
    //     for (BusAnnotation annotation : busAnnotations.value()) {
    //         addAnnotation(annotation.name(), annotation.value());
    //     }
    // }

    ConfigureDescriptions(busAttachment, busInterface);

    AutoPtr<IAnnotationInfo> intf;
    busInterface->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusInterface"), (IAnnotationInfo**)&intf);
    if (intf != NULL) {
        String announced;
        intf->GetValue(String("announced"), &announced);
        if(announced.Equals("true")) {
            mAnnounced = TRUE;
        }
        else {
            mAnnounced = FALSE;
        }
    }
    else {
        mAnnounced = FALSE;
    }

    Activate();
    return E_STATUS_OK;
}

ECode InterfaceDescription::ConfigureDescriptions(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ IInterfaceInfo* busInterface)
{
    AutoPtr<IAnnotationInfo> ifcNote;
    busInterface->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusInterface"), (IAnnotationInfo**)&ifcNote);
    if(NULL == ifcNote)
        return NOERROR;

    Boolean hasDescriptions = FALSE;

    String description;
    ifcNote->GetValue(String("description"), &description);
    if(!description.IsNullOrEmpty()){
        SetDescription(description);
        hasDescriptions = TRUE;
    }

    Int32 count;
    busInterface->GetMethodCount(&count);
    AutoPtr< ArrayOf<IMethodInfo*> > methods = ArrayOf<IMethodInfo*>::Alloc(count);
    busInterface->GetAllMethodInfos(methods);
    for (Int32 i = 0; i < methods->GetLength(); i++) {
        IMethodInfo* method = (*methods)[i];
        String name = GetName(method);

        AutoPtr<IAnnotationInfo> methodNote;
        method->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusMethod"), (IAnnotationInfo**)&methodNote);
        String description;
        if (methodNote != NULL && (methodNote->GetValue(String("description"), &description), description.GetLength() > 0)) {
            SetMemberDescription(name, description, FALSE);
            hasDescriptions = TRUE;
        }

        AutoPtr<IAnnotationInfo> signalNote;
        method->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusSignal"), (IAnnotationInfo**)&signalNote);
        if (signalNote != NULL && (signalNote->GetValue(String("description"), &description), description.GetLength() > 0)) {
            String sessionless;
            signalNote->GetValue(String("sessionless"), &sessionless);
            SetMemberDescription(name, description, StringUtils::ParseBoolean(sessionless));
            hasDescriptions = TRUE;
        }

        AutoPtr<IAnnotationInfo> propNote;
        method->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusProperty"), (IAnnotationInfo**)&propNote);
        if (propNote != NULL && (propNote->GetValue(String("description"), &description), description.GetLength() > 0)) {
            SetPropertyDescription(name, description);
            hasDescriptions = TRUE;
        }
    }

    if (hasDescriptions) {
        String descriptionLanguage;
        ifcNote->GetValue(String("descriptionLanguage"), &descriptionLanguage);
        SetDescriptionLanguage(descriptionLanguage);
    }

    // try{
        String descriptionTranslator;
        ifcNote->GetValue(String("descriptionTranslator"), &descriptionTranslator);
        if (descriptionTranslator.GetLength() > 0) {
            //We store these so as not to create a separate instance each time it is used.
            //Although this means we'll be holding on to each instance forever this is probably
            //not a problem since most Translators will need to live forever anyway
            AutoPtr<ITranslator> dt = mTranslatorCache[descriptionTranslator];
            if (dt == NULL) {
                AutoPtr<IClassInfo> klass;
                GetModuleClassLoader()->LoadClass(descriptionTranslator, (IClassInfo**)&klass);
                AutoPtr<IInterface> object;
                klass->CreateObject((IInterface**)&object);
                dt = ITranslator::Probe(object);
                mTranslatorCache[descriptionTranslator] = dt;
            }
            SetDescriptionTranslator(busAttachment, dt);
        }
    // }catch(Exception e) {
    //     e.printStackTrace();
    // }
    return NOERROR;
}

ECode InterfaceDescription::GetProperties(
    /* [in] */ IInterfaceInfo* busInterface)
{
    Int32 count;
    busInterface->GetMethodCount(&count);
    AutoPtr< ArrayOf<IMethodInfo*> > methods = ArrayOf<IMethodInfo*>::Alloc(count);
    busInterface->GetAllMethodInfos(methods);
    for (Int32 i = 0; i < methods->GetLength(); i++) {
        IMethodInfo* method = (*methods)[i];
        AutoPtr<IAnnotationInfo> busProperty;
        method->GetAnnotation(String("Org.Alljoyn.Bus.BusProperty"), (IAnnotationInfo**)&busProperty);
        if (busProperty != NULL) {
            String name = GetName(method);
            AutoPtr<Property> property = mProperties[name];
            // AutoPtr<IAnnotationInfo> propertyAnnotations;
            // method->GetAnnotation(String("Org.Alljoyn.Bus.BusAnnotations"), (IAnnotationInfo**)&propertyAnnotations);
            AutoPtr<HashMap<String, String> > annotations = new HashMap<String, String>();
            // TODO:
            // if (propertyAnnotations != NULL) {
            //     for (BusAnnotation propertyAnnotation : propertyAnnotations.value()) {
            //         annotations.put(propertyAnnotation.name(), propertyAnnotation.value());
            //     }
            // }

            if (property == NULL) {
                property = new Property(name, GetPropertySig(method), annotations);
            }
            else if (!property->mSignature.Equals(GetPropertySig(method))) {
                return E_STATUS_BAD_ANNOTATION;
            }

            String methodName;
            method->GetName(&methodName);
            if (methodName.StartWith("Get")) {
                property->mGet = method;
            }
            else if (methodName.StartWith("Set")
                /*&& (method.getGenericReturnType().equals(void.class))*/) {
                property->mSet = method;
            }
            else {
                return E_STATUS_BAD_ANNOTATION;
            }
            mProperties[name] = property;
        }
    }
    return E_STATUS_OK;
}

ECode InterfaceDescription::AddProperties(
    /* [in] */ IInterfaceInfo* busInterface)
{
    Int32 count;
    busInterface->GetMethodCount(&count);
    AutoPtr< ArrayOf<IMethodInfo*> > methods = ArrayOf<IMethodInfo*>::Alloc(count);
    busInterface->GetAllMethodInfos(methods);

    HashMap<String, AutoPtr<Property> >::Iterator it = mProperties.Begin();
    for (; it != mProperties.End(); ++it) {
        Property* property = it->mSecond;
        Int32 access = ((property->mGet != NULL) ? READ : 0) | ((property->mSet != NULL) ? WRITE : 0);
        Int32 annotation = 0;

        for (Int32 i = 0; i < methods->GetLength(); i++) {
            IMethodInfo* method = (*methods)[i];
            AutoPtr<IAnnotationInfo> p;
            method->GetAnnotation(String("Org.Alljoyn.Bus.BusProperty"), (IAnnotationInfo**)&p);
            if (p != NULL) {
                if (GetName(method).Equals(property->mName)) {
                    String strAnnotation;
                    p->GetValue(String("annotation"), &strAnnotation);
                    annotation = StringUtils::ParseInt32(strAnnotation);
                }
            }
        }
        ECode status = AddProperty(property->mName, property->mSignature, access, annotation);
        if (status != E_STATUS_OK) {
            return status;
        }

        if (annotation == 1/*BusProperty.ANNOTATE_EMIT_CHANGED_SIGNAL*/) {
            (*property->mAnnotations)[String("org.freedesktop.DBus.Property.EmitsChangedSignal")] = String("true");
        }
        else if (annotation == 2/*BusProperty.ANNOTATE_EMIT_CHANGED_SIGNAL_INVALIDATES*/) {
            (*property->mAnnotations)[String("org.freedesktop.DBus.Property.EmitsChangedSignal")] = String("invalidates");
        }

        // loop through the map of properties and add them via native code
        HashMap<String, String>::Iterator it2 = property->mAnnotations->Begin();
        for(; it2 != property->mAnnotations->End(); ++it2) {
            AddPropertyAnnotation(property->mName, it2->mFirst, it2->mSecond);
        }
    }
    return E_STATUS_OK;
}

ECode InterfaceDescription::GetMembers(
    /* [in] */ IInterfaceInfo* busInterface)
{
    Int32 count;
    busInterface->GetMethodCount(&count);
    AutoPtr< ArrayOf<IMethodInfo*> > methods = ArrayOf<IMethodInfo*>::Alloc(count);
    busInterface->GetAllMethodInfos(methods);
    for (Int32 i = 0; i < methods->GetLength(); i++) {
        IMethodInfo* method = (*methods)[i];
        AutoPtr<IAnnotationInfo> busMethod;
        method->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusMethod"), (IAnnotationInfo**)&busMethod);
        AutoPtr<IAnnotationInfo> busSignal;
        if (busMethod != NULL) {
            mMembers.PushBack(method);
        }
        else if (method->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusSignal"),
            (IAnnotationInfo**)&busSignal), busSignal != NULL) {
            mMembers.PushBack(method);
        }
    }
    return E_STATUS_OK;
}

ECode InterfaceDescription::AddMembers(
    /* [in] */ IInterfaceInfo* busInterface)
{
    List< AutoPtr<IMethodInfo> >::Iterator it = mMembers.Begin();
    for (; it != mMembers.End(); ++it) {
        IMethodInfo* member = *it;
        Int32 type = INVALID;
        Int32 annotation = 0;
        String accessPerm;
        AutoPtr<IAnnotationInfo> m;
        member->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusMethod"), (IAnnotationInfo**)&m);
        AutoPtr<IAnnotationInfo> s;
        member->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusSignal"), (IAnnotationInfo**)&s);
        AutoPtr<IAnnotationInfo> ap;
        member->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.AccessPermission"), (IAnnotationInfo**)&ap);

        String value;
        if (m != NULL) {
            type = METHOD_CALL;
            m->GetValue(String("annotation"), &value);
            annotation = StringUtils::ParseInt32(value);
        }
        else if (s != NULL) {
            type = SIGNAL;
            s->GetValue(String("annotation"), &value);
            annotation = StringUtils::ParseInt32(value);
        }
        if (type != INVALID) {
            if(ap != NULL) {
                s->GetValue(String("value"), &accessPerm);
            }

            String memberName = GetName(member);
            ECode status = AddMember(type, memberName, GetInputSig(member),
                                      GetOutSig(member), annotation, accessPerm);
            if (status != E_STATUS_OK) {
                return status;
            }

            // pull out the DBus annotations
            AutoPtr<IAnnotationInfo> dbusAnnotations;
            member->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusAnnotations"),
                (IAnnotationInfo**)&dbusAnnotations);
            // if (dbusAnnotations != NULL) {
            //     for (BusAnnotation busAnnotation : dbusAnnotations.value()) {
            //         addMemberAnnotation(memberName, busAnnotation.name(), busAnnotation.value());
            //     }
            // }
        }
    }
    return E_STATUS_OK;
}

/**
 * Create the native interface descriptions needed by
 * busInterfaces.  The Java interface descriptions are returned
 * in the descs list.
 * @param busAttachment The connection the interfaces are on.
 * @param busInterfaces The interfaces.
 * @param descs The returned interface descriptions.
 */
ECode InterfaceDescription::Create(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ ArrayOf<IInterfaceInfo*>* busInterfaces,
    /* [in] */ IList* descs)
{
    for (Int32 i = 0; i < busInterfaces->GetLength(); i++) {
        IInterfaceInfo* intf = (*busInterfaces)[i];
        String fullName = GetName(intf);
        if (fullName.Equals("Org.Freedesktop.DBus.Properties")) {
            /* The Properties interface is handled automatically by the underlying library. */
            continue;
        }

        AutoPtr<IAnnotationInfo> ifcNote;
        intf->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusInterface"), (IAnnotationInfo**)&ifcNote);
        if (ifcNote != NULL) {
            AutoPtr<InterfaceDescription> desc = new InterfaceDescription();

            ECode ec = desc->Create(busAttachment, intf);
            if (ec != E_STATUS_OK) {
                return ec;
            }
            descs->Add((IInterfaceDescription*)desc.Get());
        }
    }
    return E_STATUS_OK;
}


String InterfaceDescription::GetName(
    /* [in] */ IInterfaceInfo* intf)
{
    AutoPtr<IAnnotationInfo> busIntf;
    intf->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusInterface"), (IAnnotationInfo**)&busIntf);
    String name;
    if (busIntf != NULL && (busIntf->GetValue(String("name"), &name), !name.IsNullOrEmpty())) {
        return name;
    }
    else {
        String ns, name;
        intf->GetNamespace(&ns);
        intf->GetName(&name);
        String fullName = ns + "." + name;
        return fullName;
    }
}

ECode InterfaceDescription::IsAnnounced(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mAnnounced;
    return NOERROR;
}

/**
 * Get the DBus member or property name.
 *
 * @param method The method.
 */
String InterfaceDescription::GetName(
    /* [in] */ IMethodInfo* method)
{
    AutoPtr<IAnnotationInfo> busMethod;
    method->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusMethod"), (IAnnotationInfo**)&busMethod);
    String name;
    if (busMethod != NULL && (busMethod->GetValue(String("name"), &name), !name.IsNullOrEmpty())) {
        return name;
    }
    AutoPtr<IAnnotationInfo> busSignal;
    method->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusSignal"), (IAnnotationInfo**)&busSignal);
    if (busSignal != NULL && (busSignal->GetValue(String("name"), &name), !name.IsNullOrEmpty())) {
        return name;
    }
    AutoPtr<IAnnotationInfo> busProperty;
    method->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusProperty"), (IAnnotationInfo**)&busProperty);
    if (busProperty != NULL) {
        busProperty->GetValue(String("name"), &name);
        if (!name.IsNullOrEmpty()) {
            return name;
        }
        else {
            /* The rest of the method name following the "get" or "set" prefix. */
            String name;
            method->GetName(&name);
            return name.Substring(3);
        }
    }
    method->GetName(&name);
    return name;
}

String InterfaceDescription::GetInputSig(
    /* [in] */ IMethodInfo* method)
{
    Int32 count;
    method->GetParamCount(&count);
    AutoPtr<ArrayOf<IParamInfo*> > paramInfos = ArrayOf<IParamInfo*>::Alloc(count);
    method->GetAllParamInfos(paramInfos);

    AutoPtr<IAnnotationInfo> busMethod;
    method->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusMethod"), (IAnnotationInfo**)&busMethod);
    String signature;
    if (busMethod != NULL && (busMethod->GetValue(String("signature"), &signature), signature.GetLength() > 0)) {
        return Signature::TypeSig(paramInfos, signature, TRUE);
    }
    AutoPtr<IAnnotationInfo> busSignal;
    method->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusSignal"), (IAnnotationInfo**)&busSignal);
    if (busSignal != NULL && (busSignal->GetValue(String("signature"), &signature), signature.GetLength() > 0)) {
        return Signature::TypeSig(paramInfos, signature, TRUE);
    }

    return Signature::TypeSig(paramInfos, String(NULL), TRUE);
}

String InterfaceDescription::GetOutSig(
    /* [in] */ IMethodInfo* method)
{
    Int32 count;
    method->GetParamCount(&count);
    AutoPtr<ArrayOf<IParamInfo*> > paramInfos = ArrayOf<IParamInfo*>::Alloc(count);
    method->GetAllParamInfos(paramInfos);

    AutoPtr<IAnnotationInfo> busMethod;
    method->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusMethod"), (IAnnotationInfo**)&busMethod);
    String replySignature;
    if (busMethod != NULL && (busMethod->GetValue(String("replySignature"), &replySignature), replySignature.GetLength() > 0)) {
        return Signature::TypeSig(paramInfos, replySignature, FALSE);
    }
    AutoPtr<IAnnotationInfo> busSignal;
    method->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusSignal"), (IAnnotationInfo**)&busSignal);
    if (busSignal != NULL && (busSignal->GetValue(String("replySignature"), &replySignature), replySignature.GetLength() > 0)) {
        return Signature::TypeSig(paramInfos, replySignature, FALSE);
    }

    return Signature::TypeSig(paramInfos, String(NULL), FALSE);
}

String InterfaceDescription::GetPropertySig(
    /* [in] */ IMethodInfo* method)
{
    String methodName;
    method->GetName(&methodName);
    Int32 count;
    method->GetParamCount(&count);
    AutoPtr<IDataTypeInfo> type;
    if (count == 1 && (methodName.StartWith("Get")
        || methodName.StartWith("Set"))) {
        AutoPtr<IParamInfo> paramInfo;
        method->GetParamInfoByIndex(0, (IParamInfo**)&paramInfo);
        paramInfo->GetTypeInfo((IDataTypeInfo**)&type);
    }
    AutoPtr<IAnnotationInfo> busProperty;
    method->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusProperty"), (IAnnotationInfo**)&busProperty);
    String signature;
    if (busProperty != NULL && (busProperty->GetValue(String("signature"), &signature),
        signature.GetLength() > 0)) {
        return Signature::TypeSig(type, signature);
    }
    return Signature::TypeSig(type, String(NULL));
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
