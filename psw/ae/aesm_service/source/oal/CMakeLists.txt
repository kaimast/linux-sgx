if(WIN32)
	aux_source_directory(windows OAL_LIB_SRCS)
else()
	aux_source_directory(linux OAL_LIB_SRCS)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
endif()
add_library(oal SHARED ${OAL_LIB_SRCS}
	"../../../../../common/src/se_thread.c"
	"../../../../../common/src/se_trace.c"
	"../../../../../common/src/se_time.c")

target_include_directories(oal PRIVATE
	${PROJECT_SOURCE_DIR}/common
	${PROJECT_SOURCE_DIR}/common/oal
	${PROJECT_SOURCE_DIR}/../../inc
	${PROJECT_SOURCE_DIR}/../../inc/internal
	${PROJECT_SOURCE_DIR}/../../../../common/inc
	${PROJECT_SOURCE_DIR}/../../../../common/inc/internal
	${PROJECT_SOURCE_DIR}/../../../../external/epid-sdk
	${PROJECT_SOURCE_DIR}/../../data/constants/linux
)

target_compile_definitions(oal PRIVATE
	$<$<CONFIG:Debug>:DBG_LOG>
)

target_link_libraries(oal Threads::Threads ${CMAKE_DL_LIBS})
set_property(TARGET oal APPEND_STRING PROPERTY LINK_FLAGS " -Wl,-z,defs")
