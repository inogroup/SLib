set (SLIB_PATH "${CMAKE_CURRENT_LIST_DIR}/../..")

include("${CMAKE_CURRENT_LIST_DIR}/common.cmake")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DSLIB_COMPILE_LIB")

# enable asm
enable_language(ASM)
if (ANDROID AND SLIB_X86)
 enable_language (ASM_NASM)
 set (
  CMAKE_ASM_NASM_COMPILE_OBJECT
  "<CMAKE_ASM_NASM_COMPILER> <FLAGS> -o <OBJECT> <SOURCE>"
 )
endif()

if (ANDROID)
 include_directories (
  "${SLIB_PATH}/include"
  "${SLIB_PATH}/external/include"
  "${SLIB_PATH}/external/src/android"
 )
else()
 include_directories (
  "${SLIB_PATH}/include"
  "${SLIB_PATH}/external/include"
  "${SLIB_PATH}/external/include/glib"
  "${SLIB_PATH}/external/include/gtk"
 )
endif()

link_directories(
 "${SLIB_LIB_PATH}"
)

set (SLIB_CORE_FILES
 "${SLIB_PATH}/src/slib/core/animation.cpp"
 "${SLIB_PATH}/src/slib/core/app.cpp"
 "${SLIB_PATH}/src/slib/core/async.cpp"
 "${SLIB_PATH}/src/slib/core/async_epoll.cpp"
 "${SLIB_PATH}/src/slib/core/async_unix.cpp"
 "${SLIB_PATH}/src/slib/core/atomic.cpp"
 "${SLIB_PATH}/src/slib/core/base.cpp"
 "${SLIB_PATH}/src/slib/core/charset.cpp"
 "${SLIB_PATH}/src/slib/core/charset_ext.cpp"
 "${SLIB_PATH}/src/slib/core/collection.cpp"
 "${SLIB_PATH}/src/slib/core/console.cpp"
 "${SLIB_PATH}/src/slib/core/container.cpp"
 "${SLIB_PATH}/src/slib/core/content_type.cpp"
 "${SLIB_PATH}/src/slib/core/dispatch.cpp"
 "${SLIB_PATH}/src/slib/core/dynamic_library.cpp"
 "${SLIB_PATH}/src/slib/core/dynamic_library_unix.cpp"
 "${SLIB_PATH}/src/slib/core/event.cpp"
 "${SLIB_PATH}/src/slib/core/file.cpp"
 "${SLIB_PATH}/src/slib/core/file_unix.cpp"
 "${SLIB_PATH}/src/slib/core/hash.cpp"
 "${SLIB_PATH}/src/slib/core/io.cpp"
 "${SLIB_PATH}/src/slib/core/json.cpp"
 "${SLIB_PATH}/src/slib/core/locale.cpp"
 "${SLIB_PATH}/src/slib/core/log.cpp"
 "${SLIB_PATH}/src/slib/core/math.cpp"
 "${SLIB_PATH}/src/slib/core/memory.cpp"
 "${SLIB_PATH}/src/slib/core/mutex.cpp"
 "${SLIB_PATH}/src/slib/core/named_instance.cpp"
 "${SLIB_PATH}/src/slib/core/object.cpp"
 "${SLIB_PATH}/src/slib/core/parse.cpp"
 "${SLIB_PATH}/src/slib/core/pipe.cpp"
 "${SLIB_PATH}/src/slib/core/performance.cpp"
 "${SLIB_PATH}/src/slib/core/preference.cpp"
 "${SLIB_PATH}/src/slib/core/process.cpp"
 "${SLIB_PATH}/src/slib/core/process_unix.cpp"
 "${SLIB_PATH}/src/slib/core/red_black_tree.cpp"
 "${SLIB_PATH}/src/slib/core/ref.cpp"
 "${SLIB_PATH}/src/slib/core/regex.cpp"
 "${SLIB_PATH}/src/slib/core/resource.cpp"
 "${SLIB_PATH}/src/slib/core/rw_lock.cpp"
 "${SLIB_PATH}/src/slib/core/safe_static.cpp"
 "${SLIB_PATH}/src/slib/core/service.cpp"
 "${SLIB_PATH}/src/slib/core/service_manager.cpp"
 "${SLIB_PATH}/src/slib/core/setting.cpp"
 "${SLIB_PATH}/src/slib/core/spin_lock.cpp"
 "${SLIB_PATH}/src/slib/core/string.cpp"
 "${SLIB_PATH}/src/slib/core/string_buffer.cpp"
 "${SLIB_PATH}/src/slib/core/string_param.cpp"
 "${SLIB_PATH}/src/slib/core/system.cpp"
 "${SLIB_PATH}/src/slib/core/system_unix.cpp"
 "${SLIB_PATH}/src/slib/core/thread.cpp"
 "${SLIB_PATH}/src/slib/core/thread_unix.cpp"
 "${SLIB_PATH}/src/slib/core/time.cpp"
 "${SLIB_PATH}/src/slib/core/time_unix.cpp"
 "${SLIB_PATH}/src/slib/core/timer.cpp"
 "${SLIB_PATH}/src/slib/core/variant.cpp"
 "${SLIB_PATH}/src/slib/core/xml.cpp"

 "${SLIB_PATH}/src/slib/crypto/aes.cpp"
 "${SLIB_PATH}/src/slib/crypto/asn1.cpp"
 "${SLIB_PATH}/src/slib/crypto/base64.cpp"
 "${SLIB_PATH}/src/slib/crypto/block_cipher.cpp"
 "${SLIB_PATH}/src/slib/crypto/blowfish.cpp"
 "${SLIB_PATH}/src/slib/crypto/certificate.cpp"
 "${SLIB_PATH}/src/slib/crypto/chacha.cpp"
 "${SLIB_PATH}/src/slib/crypto/compress.cpp"
 "${SLIB_PATH}/src/slib/crypto/crc32c.cpp"
 "${SLIB_PATH}/src/slib/crypto/des.cpp"
 "${SLIB_PATH}/src/slib/crypto/ecc.cpp"
 "${SLIB_PATH}/src/slib/crypto/gcm.cpp"
 "${SLIB_PATH}/src/slib/crypto/jwt.cpp"
 "${SLIB_PATH}/src/slib/crypto/jwt_openssl.cpp"
 "${SLIB_PATH}/src/slib/crypto/lzw.cpp"
 "${SLIB_PATH}/src/slib/crypto/md5.cpp"
 "${SLIB_PATH}/src/slib/crypto/openssl.cpp"
 "${SLIB_PATH}/src/slib/crypto/openssl_crypto.cpp"
 "${SLIB_PATH}/src/slib/crypto/openssl_chacha_poly1305.cpp"
 "${SLIB_PATH}/src/slib/crypto/poly1305.cpp"
 "${SLIB_PATH}/src/slib/crypto/rc2.cpp"
 "${SLIB_PATH}/src/slib/crypto/rc4.cpp"
 "${SLIB_PATH}/src/slib/crypto/rsa.cpp"
 "${SLIB_PATH}/src/slib/crypto/sha1.cpp"
 "${SLIB_PATH}/src/slib/crypto/sha2.cpp"
 "${SLIB_PATH}/src/slib/crypto/sha3.cpp"
 "${SLIB_PATH}/src/slib/crypto/tls.cpp"
 "${SLIB_PATH}/src/slib/crypto/zlib.cpp"
 "${SLIB_PATH}/src/slib/crypto/zstd.cpp"

 "${SLIB_PATH}/src/slib/math/bigint.cpp"
 "${SLIB_PATH}/src/slib/math/calculator.cpp"
 "${SLIB_PATH}/src/slib/math/decimal128.cpp"
 "${SLIB_PATH}/src/slib/math/fft.cpp"
 "${SLIB_PATH}/src/slib/math/int128.cpp"
 "${SLIB_PATH}/src/slib/math/plot.cpp"
 "${SLIB_PATH}/src/slib/math/plot_ui.cpp"

 "${SLIB_PATH}/src/slib/network/arp.cpp"
 "${SLIB_PATH}/src/slib/network/dbip.cpp"
 "${SLIB_PATH}/src/slib/network/dns.cpp"
 "${SLIB_PATH}/src/slib/network/ethernet.cpp"
 "${SLIB_PATH}/src/slib/network/firewall.cpp"
 "${SLIB_PATH}/src/slib/network/http_common.cpp"
 "${SLIB_PATH}/src/slib/network/http_io.cpp"
 "${SLIB_PATH}/src/slib/network/http_server.cpp"
 "${SLIB_PATH}/src/slib/network/http_openssl.cpp"
 "${SLIB_PATH}/src/slib/network/icmp.cpp"
 "${SLIB_PATH}/src/slib/network/ipc.cpp"
 "${SLIB_PATH}/src/slib/network/ip_address.cpp"
 "${SLIB_PATH}/src/slib/network/mac_address.cpp"
 "${SLIB_PATH}/src/slib/network/nat.cpp"
 "${SLIB_PATH}/src/slib/network/net_capture.cpp"
 "${SLIB_PATH}/src/slib/network/network_async.cpp"
 "${SLIB_PATH}/src/slib/network/network_async_unix.cpp"
 "${SLIB_PATH}/src/slib/network/network_os.cpp"
 "${SLIB_PATH}/src/slib/network/p2p.cpp"
 "${SLIB_PATH}/src/slib/network/packet_analyzer.cpp"
 "${SLIB_PATH}/src/slib/network/pseudo_tcp.cpp"
 "${SLIB_PATH}/src/slib/network/pseudo_tcp_message.cpp"
 "${SLIB_PATH}/src/slib/network/smb.cpp"
 "${SLIB_PATH}/src/slib/network/socket.cpp"
 "${SLIB_PATH}/src/slib/network/socket_address.cpp"
 "${SLIB_PATH}/src/slib/network/socket_event.cpp"
 "${SLIB_PATH}/src/slib/network/stun.cpp"
 "${SLIB_PATH}/src/slib/network/tap.cpp"
 "${SLIB_PATH}/src/slib/network/tcpip.cpp"
 "${SLIB_PATH}/src/slib/network/url.cpp"
 "${SLIB_PATH}/src/slib/network/url_request.cpp"
 "${SLIB_PATH}/src/slib/network/url_request_param.cpp"
 "${SLIB_PATH}/src/slib/network/url_request_curl.cpp"
)
if (ANDROID)
 set (SLIB_CORE_PLATFORM_FILES
  "${SLIB_PATH}/src/slib/core/app_android.cpp"
  "${SLIB_PATH}/src/slib/core/asset_android.cpp"
  "${SLIB_PATH}/src/slib/core/charset_android.cpp"
  "${SLIB_PATH}/src/slib/core/preference_android.cpp"
  "${SLIB_PATH}/src/slib/core/system_android.cpp"
  "${SLIB_PATH}/src/slib/network/url_request_android.cpp"
 )
else()
 set (SLIB_CORE_PLATFORM_FILES
  "${SLIB_PATH}/src/slib/core/dl_linux_cap.cpp"
  "${SLIB_PATH}/src/slib/core/dl_linux_glib.cpp"
  "${SLIB_PATH}/src/slib/core/dl_linux_libc.cpp"
  "${SLIB_PATH}/src/slib/core/dl_linux_rt.cpp"
  "${SLIB_PATH}/src/slib/network/dl_linux_curl.cpp"
  "${SLIB_PATH}/src/slib/network/dl_linux_pcap.cpp"

  "${SLIB_PATH}/src/slib/core/app_linux.cpp"
  "${SLIB_PATH}/src/slib/core/asset.cpp"
  "${SLIB_PATH}/src/slib/core/charset_icu.cpp"
  "${SLIB_PATH}/src/slib/core/preference_linux.cpp"
  "${SLIB_PATH}/src/slib/core/service_manager_linux.cpp"
  "${SLIB_PATH}/src/slib/core/wrapped_symbols.cpp"
  "${SLIB_PATH}/src/slib/network/packet_analyzer.cpp"
  "${SLIB_PATH}/src/slib/network/pcap.cpp"
  "${SLIB_PATH}/src/slib/network/tap_unix.cpp"
 )
endif()

set (SLIB_EXTRA_FILES
 "${SLIB_PATH}/src/slib/graphics/bitmap.cpp"
 "${SLIB_PATH}/src/slib/graphics/bitmap_data.cpp"
 "${SLIB_PATH}/src/slib/graphics/bitmap_ext.cpp"
 "${SLIB_PATH}/src/slib/graphics/bitmap_format.cpp"
 "${SLIB_PATH}/src/slib/graphics/brush.cpp"
 "${SLIB_PATH}/src/slib/graphics/canvas.cpp"
 "${SLIB_PATH}/src/slib/graphics/canvas_ext.cpp"
 "${SLIB_PATH}/src/slib/graphics/color.cpp"
 "${SLIB_PATH}/src/slib/graphics/drawable.cpp"
 "${SLIB_PATH}/src/slib/graphics/drawable_ext.cpp"
 "${SLIB_PATH}/src/slib/graphics/emoji.cpp"
 "${SLIB_PATH}/src/slib/graphics/emoji_png.cpp"
 "${SLIB_PATH}/src/slib/graphics/font.cpp"
 "${SLIB_PATH}/src/slib/graphics/font_atlas.cpp"
 "${SLIB_PATH}/src/slib/graphics/freetype.cpp"
 "${SLIB_PATH}/src/slib/graphics/graphics_path.cpp"
 "${SLIB_PATH}/src/slib/graphics/graphics_resource.cpp"
 "${SLIB_PATH}/src/slib/graphics/graphics_text.cpp"
 "${SLIB_PATH}/src/slib/graphics/graphics_util.cpp"
 "${SLIB_PATH}/src/slib/graphics/image.cpp"
 "${SLIB_PATH}/src/slib/graphics/image_canvas.cpp"
 "${SLIB_PATH}/src/slib/graphics/image_jpeg.cpp"
 "${SLIB_PATH}/src/slib/graphics/image_png.cpp"
 "${SLIB_PATH}/src/slib/graphics/image_stb.cpp"
 "${SLIB_PATH}/src/slib/graphics/jpeg.cpp"
 "${SLIB_PATH}/src/slib/graphics/pen.cpp"
 "${SLIB_PATH}/src/slib/graphics/sfnt.cpp"
 "${SLIB_PATH}/src/slib/graphics/zxing.cpp"

 "${SLIB_PATH}/src/slib/render/opengl_gl.cpp"
 "${SLIB_PATH}/src/slib/render/opengl_gles.cpp"
 "${SLIB_PATH}/src/slib/render/render_base.cpp"
 "${SLIB_PATH}/src/slib/render/render_buffer.cpp"
 "${SLIB_PATH}/src/slib/render/render_canvas.cpp"
 "${SLIB_PATH}/src/slib/render/render_drawable.cpp"
 "${SLIB_PATH}/src/slib/render/render_engine.cpp"
 "${SLIB_PATH}/src/slib/render/render_program.cpp"
 "${SLIB_PATH}/src/slib/render/render_resource.cpp"
 "${SLIB_PATH}/src/slib/render/texture.cpp"
 
 "${SLIB_PATH}/src/slib/media/audio_codec.cpp"
 "${SLIB_PATH}/src/slib/media/audio_data.cpp"
 "${SLIB_PATH}/src/slib/media/audio_device.cpp"
 "${SLIB_PATH}/src/slib/media/audio_format.cpp"
 "${SLIB_PATH}/src/slib/media/audio_util.cpp"
 "${SLIB_PATH}/src/slib/media/camera.cpp"
 "${SLIB_PATH}/src/slib/media/codec_opus.cpp"
 "${SLIB_PATH}/src/slib/media/codec_vpx.cpp"
 "${SLIB_PATH}/src/slib/media/media_player.cpp"
 "${SLIB_PATH}/src/slib/media/media_player_ffmpeg.cpp"
 "${SLIB_PATH}/src/slib/media/video_capture.cpp"
 "${SLIB_PATH}/src/slib/media/video_codec.cpp"
 "${SLIB_PATH}/src/slib/media/video_frame.cpp"
 
 "${SLIB_PATH}/src/slib/device/device.cpp"
 "${SLIB_PATH}/src/slib/device/sensor.cpp"

 "${SLIB_PATH}/src/slib/storage/fuse.cpp"
 "${SLIB_PATH}/src/slib/storage/file_system.cpp"
 "${SLIB_PATH}/src/slib/storage/file_system_logger.cpp"
 "${SLIB_PATH}/src/slib/storage/file_system_mirror.cpp"

 "${SLIB_PATH}/src/slib/db/database.cpp"
 "${SLIB_PATH}/src/slib/db/database_cursor.cpp"
 "${SLIB_PATH}/src/slib/db/database_expression.cpp"
 "${SLIB_PATH}/src/slib/db/database_sql.cpp"
 "${SLIB_PATH}/src/slib/db/database_statement.cpp"
 "${SLIB_PATH}/src/slib/db/data_store.cpp"
 "${SLIB_PATH}/src/slib/db/document_store.cpp"
 "${SLIB_PATH}/src/slib/db/leveldb.cpp"
 "${SLIB_PATH}/src/slib/db/lmdb.cpp"
 "${SLIB_PATH}/src/slib/db/log_package.cpp"
 "${SLIB_PATH}/src/slib/db/key_value_store.cpp"
 "${SLIB_PATH}/src/slib/db/object_store.cpp"
 "${SLIB_PATH}/src/slib/db/redis.cpp"
 "${SLIB_PATH}/src/slib/db/sqlite.cpp"

 "${SLIB_PATH}/src/slib/doc/pdf.cpp"
 "${SLIB_PATH}/src/slib/doc/rar.cpp"
 "${SLIB_PATH}/src/slib/doc/zip.cpp"

 "${SLIB_PATH}/src/slib/geo/earth.cpp"
 "${SLIB_PATH}/src/slib/geo/geo_line.cpp"
 "${SLIB_PATH}/src/slib/geo/geo_location.cpp"
 "${SLIB_PATH}/src/slib/geo/geo_rectangle.cpp"
 "${SLIB_PATH}/src/slib/geo/globe.cpp"
 "${SLIB_PATH}/src/slib/geo/latlon.cpp"
 
 "${SLIB_PATH}/src/slib/service/captcha.cpp"
 "${SLIB_PATH}/src/slib/service/chat.cpp"
 "${SLIB_PATH}/src/slib/service/chat_client.cpp"
 "${SLIB_PATH}/src/slib/service/chat_sqlite.cpp"
 "${SLIB_PATH}/src/slib/service/fcm_service.cpp"
 "${SLIB_PATH}/src/slib/service/ginger.cpp"
 "${SLIB_PATH}/src/slib/service/keygen.cpp"
 "${SLIB_PATH}/src/slib/service/push_notification.cpp"
 "${SLIB_PATH}/src/slib/service/web_service.cpp"
 "${SLIB_PATH}/src/slib/service/xgpush_service.cpp"

 "${SLIB_PATH}/src/slib/social/alipay.cpp"
 "${SLIB_PATH}/src/slib/social/alipay_openssl.cpp"
 "${SLIB_PATH}/src/slib/social/alipay_sdk.cpp"
 "${SLIB_PATH}/src/slib/social/contact.cpp"
 "${SLIB_PATH}/src/slib/social/ebay.cpp"
 "${SLIB_PATH}/src/slib/social/etsy.cpp"
 "${SLIB_PATH}/src/slib/social/etsy_ui.cpp"
 "${SLIB_PATH}/src/slib/social/facebook.cpp"
 "${SLIB_PATH}/src/slib/social/facebook_ui.cpp"
 "${SLIB_PATH}/src/slib/social/facebook_sdk.cpp"
 "${SLIB_PATH}/src/slib/social/instagram.cpp"
 "${SLIB_PATH}/src/slib/social/linkedin.cpp"
 "${SLIB_PATH}/src/slib/social/linkedin_ui.cpp"
 "${SLIB_PATH}/src/slib/social/oauth.cpp"
 "${SLIB_PATH}/src/slib/social/oauth_ui.cpp"
 "${SLIB_PATH}/src/slib/social/oauth_server.cpp"
 "${SLIB_PATH}/src/slib/social/oauth_server_openssl.cpp"
 "${SLIB_PATH}/src/slib/social/paypal.cpp"
 "${SLIB_PATH}/src/slib/social/paypal_ui.cpp"
 "${SLIB_PATH}/src/slib/social/pinterest.cpp"
 "${SLIB_PATH}/src/slib/social/pinterest_ui.cpp"
 "${SLIB_PATH}/src/slib/social/twitter.cpp"
 "${SLIB_PATH}/src/slib/social/wechat.cpp"
 "${SLIB_PATH}/src/slib/social/wechat_sdk.cpp"

 "${SLIB_PATH}/src/slib/storage/disk.cpp"

 "${SLIB_PATH}/src/slib/ui/button.cpp"
 "${SLIB_PATH}/src/slib/ui/camera_view.cpp"
 "${SLIB_PATH}/src/slib/ui/clipboard.cpp"
 "${SLIB_PATH}/src/slib/ui/chat_view.cpp"
 "${SLIB_PATH}/src/slib/ui/check_box.cpp"
 "${SLIB_PATH}/src/slib/ui/chromium.cpp"
 "${SLIB_PATH}/src/slib/ui/collection_view.cpp"
 "${SLIB_PATH}/src/slib/ui/combo_box.cpp"
 "${SLIB_PATH}/src/slib/ui/common_dialogs.cpp"
 "${SLIB_PATH}/src/slib/ui/common_dialogs_prompt.cpp"
 "${SLIB_PATH}/src/slib/ui/cursor.cpp"
 "${SLIB_PATH}/src/slib/ui/date_picker.cpp"
 "${SLIB_PATH}/src/slib/ui/drawer.cpp"
 "${SLIB_PATH}/src/slib/ui/edit_view.cpp"
 "${SLIB_PATH}/src/slib/ui/gesture.cpp"
 "${SLIB_PATH}/src/slib/ui/global_event_monitor.cpp"
 "${SLIB_PATH}/src/slib/ui/group_box.cpp"
 "${SLIB_PATH}/src/slib/ui/image_view.cpp"
 "${SLIB_PATH}/src/slib/ui/image_view_url.cpp"
 "${SLIB_PATH}/src/slib/ui/label_list.cpp"
 "${SLIB_PATH}/src/slib/ui/label_view.cpp"
 "${SLIB_PATH}/src/slib/ui/line_view.cpp"
 "${SLIB_PATH}/src/slib/ui/linear_layout.cpp"
 "${SLIB_PATH}/src/slib/ui/list_box.cpp"
 "${SLIB_PATH}/src/slib/ui/list_control.cpp"
 "${SLIB_PATH}/src/slib/ui/list_view.cpp"
 "${SLIB_PATH}/src/slib/ui/mobile_app.cpp"
 "${SLIB_PATH}/src/slib/ui/mobile_game.cpp"
 "${SLIB_PATH}/src/slib/ui/motion_tracker.cpp"
 "${SLIB_PATH}/src/slib/ui/pdf_view.cpp"
 "${SLIB_PATH}/src/slib/ui/picker_view.cpp"
 "${SLIB_PATH}/src/slib/ui/progress_bar.cpp"
 "${SLIB_PATH}/src/slib/ui/qr_code_scanner.cpp"
 "${SLIB_PATH}/src/slib/ui/radio_button.cpp"
 "${SLIB_PATH}/src/slib/ui/refresh_view.cpp"
 "${SLIB_PATH}/src/slib/ui/render_view.cpp"
 "${SLIB_PATH}/src/slib/ui/screen_capture.cpp"
 "${SLIB_PATH}/src/slib/ui/scroll_bar.cpp"
 "${SLIB_PATH}/src/slib/ui/scroll_view.cpp"
 "${SLIB_PATH}/src/slib/ui/select_view.cpp"
 "${SLIB_PATH}/src/slib/ui/slider.cpp"
 "${SLIB_PATH}/src/slib/ui/split_layout.cpp"
 "${SLIB_PATH}/src/slib/ui/system_tray_icon.cpp"
 "${SLIB_PATH}/src/slib/ui/switch_view.cpp"
 "${SLIB_PATH}/src/slib/ui/tab_view.cpp"
 "${SLIB_PATH}/src/slib/ui/table_layout.cpp"
 "${SLIB_PATH}/src/slib/ui/text_view.cpp"
 "${SLIB_PATH}/src/slib/ui/tile_layout.cpp"
 "${SLIB_PATH}/src/slib/ui/toast.cpp"
 "${SLIB_PATH}/src/slib/ui/transition.cpp"
 "${SLIB_PATH}/src/slib/ui/tree_view.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_adapter.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_animation.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_app.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_core.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_core_common.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_event.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_menu.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_notification.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_notification_gtk.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_notification_fcm.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_notification_xgpush.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_photo.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_platform.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_resource.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_sound.cpp"
 "${SLIB_PATH}/src/slib/ui/ui_text.cpp"
 "${SLIB_PATH}/src/slib/ui/video_view.cpp"
 "${SLIB_PATH}/src/slib/ui/view.cpp"
 "${SLIB_PATH}/src/slib/ui/view_page.cpp"
 "${SLIB_PATH}/src/slib/ui/view_page_navigation.cpp"
 "${SLIB_PATH}/src/slib/ui/view_pager.cpp"
 "${SLIB_PATH}/src/slib/ui/web_view.cpp"
 "${SLIB_PATH}/src/slib/ui/window.cpp"

 "${SLIB_PATH}/src/slib/uix/x_button.cpp"
 "${SLIB_PATH}/src/slib/uix/x_control.cpp"
 "${SLIB_PATH}/src/slib/uix/x_edit_view.cpp"

 "${SLIB_PATH}/src/res/gen/colors.cpp"
 "${SLIB_PATH}/src/res/gen/drawables.cpp"
 "${SLIB_PATH}/src/res/gen/layouts.cpp"
 "${SLIB_PATH}/src/res/gen/menus.cpp"
 "${SLIB_PATH}/src/res/gen/raws.cpp"
 "${SLIB_PATH}/src/res/gen/strings.cpp"

)

if (ANDROID)
 set (SLIB_EXTRA_PLATFORM_FILES
  "${SLIB_PATH}/src/slib/graphics/bitmap_android.cpp"
  "${SLIB_PATH}/src/slib/graphics/brush_android.cpp"
  "${SLIB_PATH}/src/slib/graphics/canvas_android.cpp"
  "${SLIB_PATH}/src/slib/graphics/drawable_android.cpp"
  "${SLIB_PATH}/src/slib/graphics/font_android.cpp"
  "${SLIB_PATH}/src/slib/graphics/graphics_path_android.cpp"
  "${SLIB_PATH}/src/slib/graphics/pen_android.cpp"

  "${SLIB_PATH}/src/slib/media/audio_device_android.cpp"
  "${SLIB_PATH}/src/slib/media/camera_android.cpp"
  "${SLIB_PATH}/src/slib/media/media_player_android.cpp"

  "${SLIB_PATH}/src/slib/device/device_android.cpp"
  "${SLIB_PATH}/src/slib/device/sensor_android.cpp"

  "${SLIB_PATH}/src/slib/social/alipay_android.cpp"
  "${SLIB_PATH}/src/slib/social/facebook_android.cpp"
  "${SLIB_PATH}/src/slib/social/instagram_android.cpp"
  "${SLIB_PATH}/src/slib/social/wechat_android.cpp"

  "${SLIB_PATH}/src/slib/ui/clipboard_android.cpp"
  "${SLIB_PATH}/src/slib/ui/common_dialogs_android.cpp"
  "${SLIB_PATH}/src/slib/ui/edit_view_android.cpp"
  "${SLIB_PATH}/src/slib/ui/refresh_view_android.cpp"
  "${SLIB_PATH}/src/slib/ui/render_view_android.cpp"
  "${SLIB_PATH}/src/slib/ui/scroll_view_android.cpp"
  "${SLIB_PATH}/src/slib/ui/select_view_android.cpp"
  "${SLIB_PATH}/src/slib/ui/ui_animation_android.cpp"
  "${SLIB_PATH}/src/slib/ui/ui_core_android.cpp"
  "${SLIB_PATH}/src/slib/ui/ui_event_android.cpp"
  "${SLIB_PATH}/src/slib/ui/ui_notification_android.cpp"
  "${SLIB_PATH}/src/slib/ui/ui_notification_fcm_android.cpp"
  "${SLIB_PATH}/src/slib/ui/ui_notification_xgpush_android.cpp"
  "${SLIB_PATH}/src/slib/ui/ui_photo_android.cpp"
  "${SLIB_PATH}/src/slib/ui/view_android.cpp"
  "${SLIB_PATH}/src/slib/ui/web_view_android.cpp"
  "${SLIB_PATH}/src/slib/ui/window_android.cpp"

  "${SLIB_PATH}/external/src/android/cpu-features.c"
 )
else()
 set (SLIB_EXTRA_PLATFORM_FILES
  "${SLIB_PATH}/src/slib/graphics/dl_linux_cairo.cpp"
  "${SLIB_PATH}/src/slib/render/dl_linux_gl.cpp"
  "${SLIB_PATH}/src/slib/media/dl_linux_alsa.cpp"
  "${SLIB_PATH}/src/slib/ui/dl_linux_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/dl_linux_gdk.cpp"
  "${SLIB_PATH}/src/slib/ui/dl_linux_x11.cpp"
  "${SLIB_PATH}/src/slib/ui/dl_linux_app_indicator.cpp"

  "${SLIB_PATH}/src/slib/db/mongodb.cpp"
  "${SLIB_PATH}/src/slib/db/mysql.cpp"
  "${SLIB_PATH}/src/slib/db/mysql_data.cpp"
  "${SLIB_PATH}/src/slib/db/postgresql.cpp"
  "${SLIB_PATH}/src/slib/db/rocksdb.cpp"

  "${SLIB_PATH}/src/slib/graphics/bitmap_cairo.cpp"
  "${SLIB_PATH}/src/slib/graphics/canvas_cairo.cpp"
  "${SLIB_PATH}/src/slib/graphics/drawable_cairo.cpp"
  "${SLIB_PATH}/src/slib/graphics/font_pango.cpp"

  "${SLIB_PATH}/src/slib/render/opengl_glx.cpp"

  "${SLIB_PATH}/src/slib/media/audio_device_linux.cpp"
  "${SLIB_PATH}/src/slib/media/camera_linux.cpp"
  "${SLIB_PATH}/src/slib/media/media_player_linux.cpp"

  "${SLIB_PATH}/src/slib/device/device_linux.cpp"

  "${SLIB_PATH}/src/slib/storage/storage_volume_linux.cpp"

  "${SLIB_PATH}/src/slib/ui/button_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/check_box_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/clipboard_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/common_dialogs_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/combo_box_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/edit_view_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/global_event_monitor_x11.cpp"
  "${SLIB_PATH}/src/slib/ui/list_control_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/radio_button_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/render_view_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/screen_capture_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/scroll_view_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/select_view_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/system_tray_icon_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/tab_view_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/ui_core_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/ui_event_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/ui_menu_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/view_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/web_view_gtk.cpp"
  "${SLIB_PATH}/src/slib/ui/window_gtk.cpp"
 )
endif()

if (ANDROID)
 set (SLIB_JAVA_FILES
  "${SLIB_PATH}/src/slib/core/java.cpp"
  "${SLIB_PATH}/src/slib/core/java_lang.cpp"
  "${SLIB_PATH}/src/slib/core/java_io.cpp"
  "${SLIB_PATH}/src/slib/core/java_util.cpp"
 )
else()
 set (SLIB_JAVA_FILES "")
endif()

if (ANDROID)
 set (SLIB_ANDROID_FILES
  "${SLIB_PATH}/src/slib/core/android_context.cpp"
  "${SLIB_PATH}/src/slib/core/android_log.cpp"
  "${SLIB_PATH}/src/slib/core/android_platform.cpp"
  "${SLIB_PATH}/src/slib/core/android_view.cpp"
 )
else()
 set (SLIB_JAVA_FILES "")
endif()

if (SLIB_X86_64)
 SET_PROPERTY( SOURCE ${SLIB_PATH}/src/slib/crypto/crc32c.cpp PROPERTY COMPILE_FLAGS -msse4.2 )
endif()

set (EXTERNAL_SRC_DIR "${SLIB_PATH}/external/src")
set (
 SLIB_EXTERNAL_FILES
 "${EXTERNAL_SRC_DIR}/zlib/zlib_unity.c"
 "${EXTERNAL_SRC_DIR}/libpng/libpng_unity.c"
 "${EXTERNAL_SRC_DIR}/libjpeg/libjpeg_unity1.c"
 "${EXTERNAL_SRC_DIR}/libjpeg/libjpeg_unity2.c"
 "${EXTERNAL_SRC_DIR}/freetype/freetype_unity.c"
 "${EXTERNAL_SRC_DIR}/zstd/zstd_unity.c"
 "${EXTERNAL_SRC_DIR}/brotli/brotli_unity.c"
 "${EXTERNAL_SRC_DIR}/snappy/snappy_unity.cc"
 "${EXTERNAL_SRC_DIR}/lz4/lz4_unity.c"
 "${EXTERNAL_SRC_DIR}/lmdb/lmdb_unity.c"
 "${EXTERNAL_SRC_DIR}/leveldb/leveldb_unity.cc"
 "${EXTERNAL_SRC_DIR}/noto_emoji/noto_emoji.cpp"
 "${EXTERNAL_SRC_DIR}/noto_emoji/noto_emoji_png.cpp"
)

add_library (
 slib STATIC
 ${SLIB_CORE_FILES}
 ${SLIB_CORE_PLATFORM_FILES}
 ${SLIB_EXTRA_FILES}
 ${SLIB_EXTRA_PLATFORM_FILES}
 ${SLIB_JAVA_FILES}
 ${SLIB_ANDROID_FILES}
 ${SLIB_EXTERNAL_FILES}
)
set_target_properties (
 slib
 PROPERTIES
 ARCHIVE_OUTPUT_DIRECTORY "${SLIB_LIB_PATH}"
)

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
 include("${CMAKE_CURRENT_LIST_DIR}/sapp.cmake")
endif()
