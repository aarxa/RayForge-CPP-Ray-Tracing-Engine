# JPEG/CMakeLists.txt
add_library(JPEG STATIC
    ckconfig.cpp
    jcapimin.cpp
    jcapistd.cpp
    jccoefct.cpp
    jccolor.cpp
    jcdctmgr.cpp
    jchuff.cpp
    jcinit.cpp
    jcmainct.cpp
    jcmarker.cpp
    jcmaster.cpp
    jcomapi.cpp
    jcparam.cpp
    jcphuff.cpp
    jcprepct.cpp
    jcsample.cpp
    jctrans.cpp
    jdapimin.cpp
    jdapistd.cpp
    jdatadst.cpp
    jdatasrc.cpp
    jdcoefct.cpp
    jdcolor.cpp
    jddctmgr.cpp
    jdhuff.cpp
    jdinput.cpp
    jdmainct.cpp
    jdmarker.cpp
    jdmaster.cpp
    jdmerge.cpp
    jdphuff.cpp
    jdpostct.cpp
    jdsample.cpp
    jdtrans.cpp
    jerror.cpp
    jfdctflt.cpp
    jfdctfst.cpp
    jfdctint.cpp
    jidctflt.cpp
    jidctfst.cpp
    jidctint.cpp
    jidctred.cpp
    jmemmgr.cpp
    jmemnobs.cpp
    jquant1.cpp
    jquant2.cpp
    jutils.cpp
)

target_include_directories(JPEG PUBLIC ${SOURCE_DIR})
target_include_directories(JPEG PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})