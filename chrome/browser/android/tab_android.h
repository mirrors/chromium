// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_ANDROID_TAB_ANDROID_H_
#define CHROME_BROWSER_ANDROID_TAB_ANDROID_H_

#include <jni.h>

#include "base/android/jni_helper.h"
#include "base/callback_forward.h"
#include "base/memory/scoped_ptr.h"
#include "base/strings/string16.h"
#include "chrome/browser/sessions/session_id.h"
#include "chrome/browser/sync/glue/synced_tab_delegate_android.h"
#include "chrome/browser/ui/tab_contents/core_tab_helper_delegate.h"
#include "chrome/browser/ui/toolbar/toolbar_model.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"

class GURL;
class Profile;
class SkBitmap;

namespace chrome {
struct NavigateParams;
}

namespace chrome {
namespace android {
class ChromeWebContentsDelegateAndroid;
}
}

namespace content {
class ContentViewCore;
class WebContents;
}

class TabAndroid : public CoreTabHelperDelegate,
                   public content::NotificationObserver {
 public:
  // Convenience method to retrieve the Tab associated with the passed
  // WebContents.  Can return NULL.
  static TabAndroid* FromWebContents(content::WebContents* web_contents);

  // Returns the native TabAndroid stored in the Java TabBase represented by
  // |obj|.
  static TabAndroid* GetNativeTab(JNIEnv* env, jobject obj);

  TabAndroid(JNIEnv* env, jobject obj);
  virtual ~TabAndroid();

  base::android::ScopedJavaLocalRef<jobject> GetJavaObject();

  // Return the WebContents, if any, currently owned by this TabAndroid.
  content::WebContents* web_contents() const { return web_contents_.get(); }

  // Return specific id information regarding this TabAndroid.
  const SessionID& session_id() const { return session_tab_id_; }
  int GetAndroidId() const;
  int GetSyncId() const;

  // Return the tab title.
  base::string16 GetTitle() const;

  // Return the tab url.
  GURL GetURL() const;

  // Restore the tab if it was unloaded from memory.
  bool RestoreIfNeeded();

  // Helper methods to make it easier to access objects from the associated
  // WebContents.  Can return NULL.
  content::ContentViewCore* GetContentViewCore() const;
  Profile* GetProfile() const;
  browser_sync::SyncedTabDelegate* GetSyncedTabDelegate() const;

  void SetSyncId(int sync_id);

  virtual void HandlePopupNavigation(chrome::NavigateParams* params);

  virtual void OnReceivedHttpAuthRequest(jobject auth_handler,
                                         const base::string16& host,
                                         const base::string16& realm);

  // Called when context menu option to create the bookmark shortcut on
  // homescreen is called.
  virtual void AddShortcutToBookmark(const GURL& url,
                                     const base::string16& title,
                                     const SkBitmap& skbitmap,
                                     int r_value,
                                     int g_value,
                                     int b_value);

  // Called when a bookmark node should be edited.
  virtual void EditBookmark(int64 node_id,
                            const base::string16& node_title,
                            bool is_folder,
                            bool is_partner_bookmark);

  // Called to notify that the new tab page has completely rendered.
  virtual void OnNewTabPageReady();

  // Called to determine if chrome://welcome should contain links to the terms
  // of service and the privacy notice.
  virtual bool ShouldWelcomePageLinkToTermsOfService();

  // Register the Tab's native methods through JNI.
  static bool RegisterTabAndroid(JNIEnv* env);

  // CoreTabHelperDelegate ----------------------------------------------------

  virtual void SwapTabContents(content::WebContents* old_contents,
                               content::WebContents* new_contents,
                               bool did_start_load,
                               bool did_finish_load) OVERRIDE;

  // NotificationObserver -----------------------------------------------------
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) OVERRIDE;

  // Methods called from Java via JNI -----------------------------------------

  virtual void Destroy(JNIEnv* env, jobject obj);
  virtual void InitWebContents(JNIEnv* env,
                               jobject obj,
                               jboolean incognito,
                               jobject jcontent_view_core,
                               jobject jweb_contents_delegate,
                               jobject jcontext_menu_populator);
  virtual void DestroyWebContents(JNIEnv* env,
                                  jobject obj,
                                  jboolean delete_native);
  base::android::ScopedJavaLocalRef<jobject> GetWebContents(JNIEnv* env,
                                                            jobject obj);
  base::android::ScopedJavaLocalRef<jobject> GetProfileAndroid(JNIEnv* env,
                                                               jobject obj);
  ToolbarModel::SecurityLevel GetSecurityLevel(JNIEnv* env, jobject obj);
  void SetActiveNavigationEntryTitleForUrl(JNIEnv* env,
                                           jobject obj,
                                           jstring jurl,
                                           jstring jtitle);
  bool Print(JNIEnv* env, jobject obj);

 private:
  JavaObjectWeakGlobalRef weak_java_tab_;

  SessionID session_tab_id_;

  content::NotificationRegistrar notification_registrar_;

  scoped_ptr<content::WebContents> web_contents_;
  scoped_ptr<chrome::android::ChromeWebContentsDelegateAndroid>
      web_contents_delegate_;

  scoped_ptr<browser_sync::SyncedTabDelegateAndroid> synced_tab_delegate_;

  DISALLOW_COPY_AND_ASSIGN(TabAndroid);
};

#endif  // CHROME_BROWSER_ANDROID_TAB_ANDROID_H_
