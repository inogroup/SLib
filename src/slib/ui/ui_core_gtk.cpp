/*
 *   Copyright (c) 2008-2018 SLIBIO <https://github.com/SLIBIO>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */

#include "slib/ui/definition.h"

#if defined(SLIB_UI_IS_GTK)

#include "ui_core_common.h"

#include "slib/ui/app.h"
#include "slib/ui/screen.h"
#include "slib/ui/gtk/gdbus.h"
#include "slib/graphics/image.h"
#include "slib/network/ipc.h"
#include "slib/core/json.h"
#include "slib/core/memory_output.h"
#include "slib/core/stringify.h"
#include "slib/core/safe_static.h"

namespace slib
{

	namespace priv
	{
		namespace ui_core
		{

			GtkApplication* g_app = sl_null;

			pthread_t g_threadMain = 0;
			sl_bool g_flagRunningAppLoop = sl_false;


			static String GetOpenIpcName(const StringParam& appId)
			{
				return String::concat(appId, ".ipc.ui.open");
			}

			class StaticContext
			{
			public:
				Ref<IPC> m_ipc;

			public:
				void initIPC(const StringParam& appId)
				{
					if (m_ipc.isNotNull()) {
						return;
					}
					IPCParam param;
					param.name = GetOpenIpcName(appId);
					param.onReceiveMessage = SLIB_FUNCTION_MEMBER(this, onReceiveIPC);
					m_ipc = IPC::create(param);
				}

				void onReceiveIPC(sl_uint8* data, sl_size size, MemoryOutput* output)
				{
					Json json;
					if (json.deserialize(data, size)) {
						String command = json["command"].getString();
						if (command == "open") {
							String args = json["args"].getString();
							UIApp::dispatchReopenToApp(args, sl_true);
							Json("ok").serialize(output);
						} else {
							Json("unknown_command").serialize(output);
						}
					} else {
						Json("failed_deserialize").serialize(output);
					}
				}

			};

			SLIB_SAFE_STATIC_GETTER(StaticContext, GetStaticContext)

			class ScreenImpl : public Screen
			{
			public:
				UIRect m_region;
				
			public:
				ScreenImpl()
				{
					GdkScreen* screen = gdk_screen_get_default();
					if (screen) {
						m_region.left = 0;
						m_region.top = 0;
						m_region.right = gdk_screen_get_width(screen);
						m_region.bottom = gdk_screen_get_height(screen);
					} else {
						m_region.setZero();
					}
				}

			public:
				UIRect getRegion() override
				{
					return m_region;
				}
				
			};

			static gboolean DispatchUrgentlyCallback(gpointer user_data)
			{
				Callable<void()>* callable = reinterpret_cast<Callable<void()>*>(user_data);
				callable->invoke();
				return sl_false;
			}

			static void DispatchUrgentlyDestroy(gpointer user_data)
			{
				Callable<void()>* callable = reinterpret_cast<Callable<void()>*>(user_data);
				callable->decreaseReference();
			}

			static void EventHandler(GdkEvent* event, gpointer data)
			{
				UIDispatcher::processCallbacks();
				gtk_main_do_event(event);
			}

			static void OnPixbufDestroyNotify(guchar* pixels, gpointer data)
			{
				Image* image = (Image*)data;
				if (image) {
					image->decreaseReference();
				}
			}

			static sl_bool GetActiveWindowInfo(String& cls, String& title)
			{
				auto funcCallSync = gio::getApi_g_dbus_connection_call_sync();
				if (!funcCallSync) {
					return sl_false;
				}
				GDBusConnection* connection = gtk::GDBus::getDefaultConnection();
				if (!connection) {
					return sl_false;
				}
				sl_bool bRet = sl_false;
				const char* script =
					SLIB_STRINGIFY(
						global.
							get_window_actors().
							map(a=>a.meta_window).
							map(w=>({has_focus: w.has_focus(), cls: w.get_wm_class(), title: w.get_title()})).
							find(w=>w.has_focus)
					);
				GVariant* result = funcCallSync(
					connection,
					"org.gnome.Shell", // bus_name
					"/org/gnome/Shell", // object_path
					"org.gnome.Shell", // interface
					"Eval", // method
					g_variant_new("(s)", script),
					sl_null, // reply_type
					G_DBUS_CALL_FLAGS_NONE,
					-1, // timeout
					sl_null, sl_null);
				if (result) {
					if (Base::equalsString(g_variant_get_type_string(result), "(bs)")) {
						GVariant* gstrResult = g_variant_get_child_value(result, 1);
						if (gstrResult) {
							gsize lenResult = 0;
							const gchar* szResult = g_variant_get_string(gstrResult, &lenResult);
							Json json = Json::parse(szResult);
							cls = json["cls"].getString();
							title = json["title"].getString();
							if (cls.isNotNull() || title.isNotNull()) {
								bRet = sl_true;
							}
							g_variant_unref(gstrResult);
						}
					}
					g_variant_unref(result);
				}
				g_object_unref(connection);
				return bRet;
			}

		}
	}

	using namespace priv::ui_core;

	Ref<Screen> UI::getPrimaryScreen()
	{
		return new ScreenImpl;
	}

	Ref<Screen> UI::getFocusedScreen()
	{
		return UI::getPrimaryScreen();
	}

	List< Ref<Screen> > UI::getScreens()
	{
		return List< Ref<Screen> >::createFromElement(getPrimaryScreen());
	}

	sl_bool UI::isUiThread()
	{
		return g_threadMain == pthread_self();
	}

	void UI::dispatchToUiThread(const Function<void()>& callback, sl_uint32 delayMillis)
	{
		if (callback.isNull()) {
			return;
		}
		if (delayMillis || !(UI::isUiThread())) {
			dispatchToUiThreadUrgently([callback]() {
				dispatchToUiThread(callback);
			}, delayMillis);
			return;
		}
		UIDispatcher::addCallback(callback);
		GdkEvent event;
		Base::zeroMemory(&event, sizeof(event));
		event.type = GDK_NOTHING;
		gdk_event_put(&event);
	}

	void UI::dispatchToUiThreadUrgently(const Function<void()>& callback, sl_uint32 delayMillis)
	{
		if (callback.isNull()) {
			return;
		}
		Callable<void()>* callable = callback.ref.get();
		callable->increaseReference();
		if (delayMillis) {
			g_timeout_add_full(G_PRIORITY_DEFAULT, delayMillis, DispatchUrgentlyCallback, callable, DispatchUrgentlyDestroy);
		} else {
			g_idle_add_full(G_PRIORITY_DEFAULT, DispatchUrgentlyCallback, callable, DispatchUrgentlyDestroy);
		}
	}

	void UI::openUrl(const StringParam& _url)
	{
		if (!(UI::isUiThread())) {
			String url = _url.toString();
			UI::dispatchToUiThread([url]() {
				UI::openUrl(url);
			});
			return;
		}
		StringCstr url(_url);
		GError* error = NULL;
		gtk_show_uri(NULL, url.getData(), GDK_CURRENT_TIME, &error);
	}

	void UI::getActiveApplicationAndWindow(String& appName, String& windowTitle)
	{
		if (!(GetActiveWindowInfo(appName, windowTitle))) {
			appName.setNull();
			windowTitle.setNull();
		}
	}

	String UI::getActiveApplicationName()
	{
		String cls, title;
		if (GetActiveWindowInfo(cls, title)) {
			return cls;
		}
		return sl_null;
	}

	String UI::getActiveWindowTitle()
	{
		String cls, title;
		if (GetActiveWindowInfo(cls, title)) {
			return title;
		}
		return sl_null;
	}

	sl_bool UIPlatform::initializeGtk()
	{
		g_thread_init(NULL);
		gdk_threads_init();
		if (gtk_init_check(sl_null, sl_null)) {
			return sl_true;
		}
		return sl_false;
	}

	sl_uint32 UIPlatform::getGtkMajorVersion()
	{
		auto func = gtk::getApi_gtk_get_major_version();
		if (func) {
			return (sl_uint32)(func());
		}
		if (gtk::getApi_gtk_init_check()) {
			return 2;
		}
		return 0;
	}

	sl_uint32 UIPlatform::getGtkMinorVersion()
	{
		auto func = gtk::getApi_gtk_get_minor_version();
		if (func) {
			return (sl_uint32)(func());
		}
		return 0;
	}

	sl_bool UIPlatform::isSupportedGtk(sl_uint32 major)
	{
		if (major >= 3) {
			auto funcMajor = gtk::getApi_gtk_get_major_version();
			if (!funcMajor) {
				return sl_false;
			}
			return (sl_uint32)(funcMajor()) >= major;
		} else {
			if (gtk::getApi_gtk_init_check()) {
				return sl_true;
			} else {
				return sl_false;
			}
		}
	}

	sl_bool UIPlatform::isSupportedGtk(sl_uint32 major, sl_uint32 minor)
	{
		if (major >= 3) {
			auto funcMajor = gtk::getApi_gtk_get_major_version();
			if (!funcMajor) {
				return sl_false;
			}
			sl_uint32 _major = (sl_uint32)(funcMajor());
			if (_major < major) {
				return sl_false;
			}
			if (_major > major) {
				return sl_true;
			}
			return (gtk::getApi_gtk_get_minor_version())() >= minor;
		} else {
			if (gtk::getApi_gtk_init_check()) {
				return sl_true;
			} else {
				return sl_false;
			}
		}
	}

	void UIPlatform::runLoop(sl_uint32 level)
	{
		gtk_main();
	}

	void UIPlatform::quitLoop()
	{
		gtk_main_quit();
	}

	void UIPlatform::initApp()
	{
		g_threadMain = pthread_self();
		UIPlatform::initializeGtk();
		GtkApplication* app = getApp();
		if (app) {
			auto funcRegister = gio::getApi_g_application_register();
			if (funcRegister) {
				funcRegister((GApplication*)app, sl_null, sl_null);
			}
		}
		gdk_event_handler_set(EventHandler, sl_null, sl_null);
	}

	void UIPlatform::runApp()
	{
		StaticContext* context = GetStaticContext();
		if (!context) {
			return;
		}
		{
			Ref<UIApp> app = UIApp::getApp();
			if (app.isNotNull()) {
				String appId = app->getApplicationId();
				if (appId.isNotEmpty()) {
					context->initIPC(appId);
				}
			}
		}

		UIApp::dispatchStartToApp();
		GtkApplication* app = getApp();
		if (!(UI::isQuitingApp())) {
			g_flagRunningAppLoop = sl_true;
			gdk_event_handler_set(EventHandler, sl_null, sl_null);
			gtk_main();
			g_flagRunningAppLoop = sl_false;
		}
		UIApp::dispatchExitToApp();
		if (app) {
			g_object_unref(app);
		}
		g_app = sl_null;
	}

	void UIPlatform::quitApp()
	{
		if (g_flagRunningAppLoop) {
			gtk_main_quit();
		}
	}

	GtkApplication* UIPlatform::getApp()
	{
		static sl_bool flagInit = sl_true;
		if (flagInit) {
			if (!(UI::isUiThread())) {
				return sl_null;
			}
			flagInit = sl_false;
			if (isSupportedGtk(3)) {
				StringCstr id;
				Ref<UIApp> app = UIApp::getApp();
				if (app.isNotNull()) {
					id = app->getApplicationId();
				}
				if (gtk::getApi_gtk_get_minor_version()() < 6) {
					if (id.isEmpty()) {
						id = String::concat("app.id", String::fromUint64(Time::now().toInt()));
					}
				}
				if (id.isEmpty()) {
					g_app = gtk::getApi_gtk_application_new()(sl_null, G_APPLICATION_FLAGS_NONE);
				} else {
					g_app = gtk::getApi_gtk_application_new()(id.getData(), G_APPLICATION_FLAGS_NONE);
				}
			}
		}
		return g_app;
	}

	void UIPlatform::getGdkColor(const Color& color, GdkColor* _out)
	{
		_out->pixel = 0;
		_out->red = (guint16)(color.r) * 257;
		_out->green = (guint16)(color.g) * 257;
		_out->blue = (guint16)(color.b) * 257;
	}

	GdkPixbuf* UIPlatform::createPixbuf(const Ref<Image>& image)
	{
		if (image.isNotNull()) {
			sl_uint32 width = image->getWidth();
			sl_uint32 height = image->getHeight();
			if (width && height) {
				sl_int32 stride = image->getStride();
				Color* colors = image->getColors();
				GdkPixbuf* ret = gdk_pixbuf_new_from_data((guchar*)(colors), GDK_COLORSPACE_RGB, sl_true, 8, (int)width, (int)height, stride << 2, NULL, image.get());
				if (ret) {
					image->increaseReference();
					return ret;
				}
			}
		}
		return sl_null;
	}

	sl_int32 UIApp::onExistingInstance()
	{
		String appId = getApplicationId();
		if (appId.isEmpty()) {
			return -1;
		}
		Ref<IPC> ipc = IPC::create();
		if (ipc.isNotNull()) {
			Json json;
			json.putItem_NoLock("command", "open");
			json.putItem_NoLock("args", getCommandLine());
			Ref<Event> ev = Event::create();
			ipc->sendMessage(GetOpenIpcName(appId), json.serialize(), [ev](sl_uint8*, sl_uint32) {
				ev->set();
			});
			ev->wait(3000);
			return 0;
		}
		return -1;
	}

}

#endif
