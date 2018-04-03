#ifndef ANDROID_WEBVIEW_NATIVE_AW_PASSWORD_HANDLER_DELEGATE_IMPL_H_
#define ANDROID_WEBVIEW_NATIVE_AW_PASSWORD_HANDLER_DELEGATE_IMPL_H_

#include <jni.h>
#include <string>

#include "base/android/jni_android.h"
#include "base/android/scoped_java_ref.h"
#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "base/strings/string16.h"
#include "android_webview/native/password_handler_delegate.h"
#include "content/public/browser/web_contents_user_data.h"

namespace content {
class WebContents;
}

namespace autofill {
struct PasswordForm;
}

namespace android_webview {

class AwPasswordHandlerDelegateImpl
    : public android_webview::PasswordHandlerDelegate,
      public content::WebContentsUserData<AwPasswordHandlerDelegateImpl> {
public:
    virtual ~AwPasswordHandlerDelegateImpl();
  
    // PasswordHandlerDelegate implementation.
    virtual void FillPasswordForm(
        const autofill::PasswordFormFillData& form_data) OVERRIDE;
    virtual void AddSavePasswordPopupIfPermitted(
                  const autofill::PasswordForm& info, bool permitted) OVERRIDE;
    virtual void GetPasswordRequest(
                  const autofill::PasswordForm& info,
                  const GetRequestedData& callback) OVERRIDE;
    virtual bool DidLastPageLoadEncounterSSLErrors() OVERRIDE;
    virtual bool IsSavePasswordEnabled() OVERRIDE;
  
    void OnRequestDone(
          JNIEnv* env, jobject obj, jobjectArray jdata, jobjectArray jdata16);

private:
    void InitHandlerIfNeed();

    explicit AwPasswordHandlerDelegateImpl(content::WebContents* web_contents);
    friend class content::WebContentsUserData<AwPasswordHandlerDelegateImpl>;

    content::WebContents* web_contents_;
    base::android::ScopedJavaGlobalRef<jobject> aw_password_handler_;
    GetRequestedData callback_;
    base::string16 username_element;
    base::string16 password_element;
};

bool RegisterAwPasswordHandlerDelegateImpl(JNIEnv* env);

}

#endif  // ANDROID_WEBVIEW_NATIVE_AW_PASSWORD_HANDLER_DELEGATE_IMPL_H_
