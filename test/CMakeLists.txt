find_package(Catch2 REQUIRED)

add_library(config_test_wf700tk INTERFACE)

target_link_libraries(config_test_wf700tk INTERFACE
    config_wf700tk
    Catch2::Catch2
)

add_executable(test_wf700tk
    main.cpp
    ticket_channel.cpp
    credit_channel.cpp
    response_parser.cpp
)

target_link_libraries(test_wf700tk
    config_test_wf700tk
    wf700tk
)

add_subdirectory(detail)

include(CTest)
include(Catch)
catch_discover_tests(test_wf700tk)
