if(CONFIG_ZMK_DISPLAY AND CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM AND ((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL))
    zephyr_library()
    zephyr_library_sources(${ZEPHYR_BASE}/misc/empty_file.c)
    zephyr_library_include_directories(${ZEPHYR_LVGL_MODULE_DIR})
    zephyr_library_include_directories(${ZEPHYR_BASE}/lib/gui/lvgl/)
    zephyr_library_include_directories(${ZEPHYR_BASE}/drivers)
    zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/include);
    target_sources(app PRIVATE zmk_ui.c)
    target_include_directories(app PRIVATE ${ZEPHYR_BASE}/lib/gui/lvgl

    # 실제 폴더에 존재하는 파일만 등록
    zephyr_library_sources(widgets/output_status.c)
    zephyr_library_sources(widgets/output_status_sym.c)
    
    if (CONFIG_ZMK_BATTERY)
        zephyr_library_sources(widgets/battery_status.c)
    endif()
endif()
)
