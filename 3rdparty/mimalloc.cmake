set(MI_XMALLOC ON CACHE INTERNAL "" FORCE)
set(MI_PADDING OFF CACHE INTERNAL "" FORCE)

if(SCORE_DEBUG)
  set(MI_SECURE ON CACHE INTERNAL "" FORCE)
  set(MI_DEBUG_FULL ON CACHE INTERNAL "" FORCE)
  set(MI_SHOW_ERRORS ON CACHE INTERNAL "" FORCE)
else()
  set(MI_SECURE OFF CACHE INTERNAL "" FORCE)
  set(MI_DEBUG_FULL OFF CACHE INTERNAL "" FORCE)
  set(MI_SHOW_ERRORS OFF CACHE INTERNAL "" FORCE)
endif()
set(MI_OVERRIDE ON CACHE INTERNAL "" FORCE)
set(MI_OSX_INTERPOSE ON CACHE INTERNAL "" FORCE)
set(MI_OSX_ZONE ON CACHE INTERNAL "" FORCE)
set(MI_LOCAL_DYNAMIC_TLS ON CACHE INTERNAL "" FORCE)
set(MI_BUILD_OBJECT OFF CACHE INTERNAL "" FORCE)
set(MI_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(MI_SKIP_COLLECT_ON_EXIT ON CACHE INTERNAL "" FORCE)

if(SCORE_STATIC_PLUGINS)
  set(MI_BUILD_SHARED OFF CACHE INTERNAL "" FORCE)
  set(MI_BUILD_STATIC ON CACHE INTERNAL "" FORCE)
else()
  set(MI_BUILD_SHARED ON CACHE INTERNAL "" FORCE)
  set(MI_BUILD_STATIC OFF CACHE INTERNAL "" FORCE)
endif()

add_subdirectory(3rdparty/mimalloc)

if(TARGET mimalloc)
  set_target_properties(mimalloc PROPERTIES UNITY_BUILD 0)
elseif(TARGET mimalloc-static)
  set_target_properties(mimalloc-static PROPERTIES UNITY_BUILD 0)
endif()
