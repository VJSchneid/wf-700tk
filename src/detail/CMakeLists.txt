add_library(wf700tk_detail OBJECT
    basic_parser.cpp
    message_base.cpp
    basic_writer.cpp
)

target_link_libraries(wf700tk_detail PUBLIC config_wf700tk)
target_link_libraries(wf700tk PUBLIC wf700tk_detail)
