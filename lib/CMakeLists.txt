#Author: Elena Ceseracciu

set(Concurrency_HEADERS include/rtb/concurrency/Latch.h
                        include/rtb/concurrency/Queue.h
                        include/rtb/concurrency/Concurrency.h)

set(Concurrency_TEMPLATE_IMPLEMENTATIONS include/rtb/concurrency/Queue.cpp)
set_source_files_properties(${Concurrency_TEMPLATE_IMPLEMENTATIONS} PROPERTIES HEADER_FILE_ONLY TRUE)

set(Concurrency_SOURCES Latch.cpp)

source_group("Header files" FILES ${Concurrency_HEADERS})
source_group("Source files" FILES ${Concurrency_TEMPLATE_IMPLEMENTATIONS} ${Concurrency_SOURCES})


add_library(Concurrency ${Concurrency_HEADERS} ${Concurrency_TEMPLATE_IMPLEMENTATIONS} ${Concurrency_SOURCES})
target_link_libraries(Concurrency Threads::Threads)

target_include_directories(Concurrency PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)


install(FILES ${Concurrency_HEADERS} ${Concurrency_TEMPLATE_IMPLEMENTATIONS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rtb/concurrency)

install(TARGETS Concurrency
        EXPORT ConcurrencyTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
