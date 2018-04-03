#include "android_webview/native/aw_password_handler_delegate_impl.h"

#include "base/android/jni_android.h"
#include "base/android/jni_array.h"
#include "base/android/jni_string.h"
#include "base/strings/string16.h"
#include "components/autofill/core/browser/autofill_manager.h"
#include "components/autofill/content/common/autofill_messages.h"
#include "components/autofill/core/common/form_field_data.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "components/autofill/core/common/password_form.h"
#include "content/public/common/ssl_status.h"
#include "jni/AwPasswordHandler_jni.h"
#include "net/cert/cert_status_flags.h"
#include "url/gurl.h"

DEFINE_WEB_CONTENTS_USER_DATA_KEY(android_webview::AwPasswordHandlerDelegateImpl);

namespace android_webview {

AwPasswordHandlerDelegateImpl::AwPasswordHandlerDelegateImpl(
    content::WebContents* web_contents)
    : web_contents_(web_contents) {
    InitHandlerIfNeed();
}

AwPasswordHandlerDelegateImpl::~AwPasswordHandlerDelegateImpl() {
}

void AwPasswordHandlerDelegateImpl::FillPasswordForm(
    const autofill::PasswordFormFillData& form_data) {
    LOG(WARNING) << "FillPasswordForm username element "
                 << form_data.basic_data.fields[0].name
                 << ", value is "
                 << form_data.basic_data.fields[0].value;
    LOG(WARNING) << "FillPasswordForm password element "
                 << form_data.basic_data.fields[1].name
                 << ", value is "
                 << form_data.basic_data.fields[1].value;
    LOG(WARNING) << "FillPasswordForm action is "
                 << form_data.basic_data.action.spec()
                 << ", origin is "
                 << form_data.basic_data.origin.spec();
    LOG(WARNING) << "FillPasswordForm Routing ID is "
                 << web_contents_->GetRenderViewHost()->GetRoutingID();
    web_contents_->GetRenderViewHost()->Send(
        new AutofillMsg_FillPasswordForm(
            web_contents_->GetRenderViewHost()->GetRoutingID(),
            form_data));
}

void AwPasswordHandlerDelegateImpl::AddSavePasswordPopupIfPermitted(
        const autofill::PasswordForm& info, bool permitted) {
    DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
    std::vector<std::string> data_str;
    std::vector<base::string16> data16_str;
    JNIEnv* env = base::android::AttachCurrentThread();

    data_str.push_back(info.signon_realm);
    data_str.push_back(info.original_signon_realm);
    data_str.push_back(info.origin.is_empty() ? "" : info.origin.spec());
    data_str.push_back(info.action.is_empty() ? "" : info.action.spec());

    data16_str.push_back(info.username_element);
    data16_str.push_back(info.username_value);
    data16_str.push_back(info.password_element);
    data16_str.push_back(info.password_value);
    ScopedJavaLocalRef<jobjectArray> j_data = 
        base::android::ToJavaArrayOfStrings(env, data_str);
    ScopedJavaLocalRef<jobjectArray> j_data16 = 
        base::android::ToJavaArrayOfStrings(env, data16_str);
    LOG(WARNING) << "AddSavePasswordPopupIfPermitted called!!";
    Java_AwPasswordHandler_showPasswordDialog(
        base::android::AttachCurrentThread(), 
        aw_password_handler_.obj(), j_data.obj(), j_data16.obj(), permitted);
}

void AwPasswordHandlerDelegateImpl::OnRequestDone(
        JNIEnv* env, jobject obj, jobjectArray jdata, jobjectArray jdata16) {
    autofill::PasswordFormFillData fill_data;

    LOG(WARNING) << "OnRequestDone called!!";

    autofill::FormFieldData username_field;
    /*ScopedJavaLocalRef<jstring> username_element_str(env,
        static_cast<jstring>(env->GetObjectArrayElement(jdata16, 0)));
    base::android::ConvertJavaStringToUTF16(
        env, username_element_str.obj(), &username_field.name);*/
    /*
     * sometimes the input element name changes when logins,
     * so use the requested input element name to match.
     * use username_element instead of jdata16[0].
     */
    username_field.name = username_element;

    ScopedJavaLocalRef<jstring> username_value_str(env,
        static_cast<jstring>(env->GetObjectArrayElement(jdata16, 1)));
    base::android::ConvertJavaStringToUTF16(
        env, username_value_str.obj(), &username_field.value);

    autofill::FormFieldData password_field;
    /*ScopedJavaLocalRef<jstring> password_element_str(env,
        static_cast<jstring>(env->GetObjectArrayElement(jdata16, 2)));
    base::android::ConvertJavaStringToUTF16(
        env, password_element_str.obj(), &password_field.name);*/
    /*
     * sometimes the input element name changes when logins,
     * so use the requested input element name to match.
     * use password_element instead of jdata16[2].
     */
    password_field.name = password_element;
    
    ScopedJavaLocalRef<jstring> password_value_str(env,
        static_cast<jstring>(env->GetObjectArrayElement(jdata16, 3)));
    base::android::ConvertJavaStringToUTF16(
        env, password_value_str.obj(), &password_field.value);
    
    password_field.form_control_type = "password";

    // Fill basic form data.
    ScopedJavaLocalRef<jstring> origin_str(env,
        static_cast<jstring>(env->GetObjectArrayElement(jdata, 2)));
    std::string ret = base::android::ConvertJavaStringToUTF8(
                        env, origin_str.obj());
    GURL* gen = new GURL(ret);
    fill_data.basic_data.origin = *gen;
    delete gen;
    gen = NULL;
    
    ScopedJavaLocalRef<jstring> action_str(env,
        static_cast<jstring>(env->GetObjectArrayElement(jdata, 3)));
    ret = base::android::ConvertJavaStringToUTF8(env, action_str.obj());
    gen = new GURL(ret);
    fill_data.basic_data.action = *gen;
    delete gen;
    gen = NULL;
    
    fill_data.basic_data.fields.push_back(username_field);
    fill_data.basic_data.fields.push_back(password_field);
    fill_data.wait_for_username = false;
    
    ScopedJavaLocalRef<jstring> original_signon_realm_str(env,
        static_cast<jstring>(env->GetObjectArrayElement(jdata, 1)));
    base::android::ConvertJavaStringToUTF8(
        env, original_signon_realm_str.obj(), 
        &fill_data.preferred_realm);
    callback_.Run(fill_data);
    FillPasswordForm(fill_data);
}

void AwPasswordHandlerDelegateImpl::GetPasswordRequest(
        const autofill::PasswordForm& info,
        const GetRequestedData& callback) {
    std::vector<std::string> data_str;
    std::vector<base::string16> data16_str;
    JNIEnv* env = base::android::AttachCurrentThread();
    callback_ = callback;

    data_str.push_back(info.signon_realm);
    data_str.push_back(info.original_signon_realm);
    data_str.push_back(info.origin.is_empty() ? "" : info.origin.spec());
    data_str.push_back(info.action.is_empty() ? "" : info.action.spec());

    data16_str.push_back(info.username_element);
    data16_str.push_back(info.username_value);
    data16_str.push_back(info.password_element);
    data16_str.push_back(info.password_value);
    ScopedJavaLocalRef<jobjectArray> j_data = 
        base::android::ToJavaArrayOfStrings(env, data_str);
    ScopedJavaLocalRef<jobjectArray> j_data16 = 
        base::android::ToJavaArrayOfStrings(env, data16_str);
    /*
     * sometimes the input element name changes when logins,
     * so save the requested input element name for data match.
     */
    username_element = info.username_element;
    password_element = info.password_element;
    LOG(WARNING) << "GetPasswordRequest called!!";
    Java_AwPasswordHandler_reqeustPassword(
        base::android::AttachCurrentThread(), 
        aw_password_handler_.obj(), reinterpret_cast<intptr_t>(this), j_data.obj(), j_data16.obj());
}

bool AwPasswordHandlerDelegateImpl::DidLastPageLoadEncounterSSLErrors() {
    content::NavigationEntry* entry =
        web_contents_->GetController().GetActiveEntry();
    if (!entry) {
        LOG(WARNING) << "DidLastPageLoadEncounterSSLErrors entry is null!!";
        NOTREACHED();
        return false;
    }

    return net::IsCertStatusError(entry->GetSSL().cert_status);
}

bool AwPasswordHandlerDelegateImpl::IsSavePasswordEnabled() {
    jboolean ret = Java_AwPasswordHandler_getSavePassword(
        base::android::AttachCurrentThread(), aw_password_handler_.obj());
    LOG(WARNING) << "IsSavePasswordEnabled ret is "
                 << ret;
    return ret;
}

void AwPasswordHandlerDelegateImpl::InitHandlerIfNeed() {
    JNIEnv* env = base::android::AttachCurrentThread();
    if (aw_password_handler_.obj() == NULL) {
        aw_password_handler_.Reset(Java_AwPasswordHandler_getInstance(env));
    }
}

bool RegisterAwPasswordHandlerDelegateImpl(JNIEnv* env) {
    return RegisterNativesImpl(env) >= 0;
}

}
