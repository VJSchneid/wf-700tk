add_library(test_wf700tk_detail OBJECT
    basic_parser.cpp
)


target_link_libraries(test_wf700tk_detail config_test_wf700tk)
target_link_libraries(test_wf700tk test_wf700tk_detail)
