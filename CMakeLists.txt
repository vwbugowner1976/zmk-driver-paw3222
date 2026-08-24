if(CONFIG_PAW3222)
    zephyr_library()
    zephyr_library_sources(src/paw3222.c)
    zephyr_library_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
    zephyr_library_sources_ifdef(CONFIG_ZMK_PAW3222_CUSTOM_SETTINGS
        src/settings/paw3222_settings.c
    )
endif()
