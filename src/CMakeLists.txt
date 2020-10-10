add_library(wf700tk
    device.cpp
    response_parser.cpp
    credit_channel.cpp
    ticket_channel.cpp
    response.cpp
    request_writer.cpp
)

target_link_libraries(wf700tk PUBLIC config_wf700tk)

add_subdirectory(detail)
