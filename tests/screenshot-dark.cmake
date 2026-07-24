# CI fixture for the custom-LVGL build hook (CMAKE_PROJECT_INCLUDE).
#
# Injected exactly like the documented zephyr-with-custom-lvgl.cmake at the
# repository root, but with one concrete customization active: forcing LVGL's
# default theme to dark mode. The screenshot test then confirms the mechanism
# takes effect and produces the correct output, matched against a dedicated
# dark reference image (not merely "different from the default", which garbage
# output would also satisfy):
#
#   west build -b native_sim/native/64 -- -DEXTRA_CONF_FILE=tests/screenshot.conf \
#     -DCMAKE_PROJECT_INCLUDE=$PWD/tests/screenshot-dark.cmake
#   LV_Z_SCREENSHOT_REFERENCE=tests/screenshot-reference-dark ./build/zephyr/zephyr.exe
#
# The reference image is selected at run time (env var), NOT here, on purpose:
# it must stay independent of this hook. If it were set here and the hook ever
# silently stopped applying, the un-customized output would be compared against
# the default reference and pass -- hiding the very regression this guards.

if(NOT TARGET modules__lvgl)
  return()
endif()

# LV_THEME_DEFAULT_DARK is read both by the LVGL library (lv_display.c) and by
# the widgets demo, which is compiled into `app` -- so apply it to both targets.
foreach(_tgt modules__lvgl app)
  if(TARGET ${_tgt})
    target_compile_definitions(${_tgt} PRIVATE LV_THEME_DEFAULT_DARK=1)
  endif()
endforeach()

message(STATUS "[screenshot-dark] forced LV_THEME_DEFAULT_DARK=1")
