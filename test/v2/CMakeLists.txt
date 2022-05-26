enable_testing()

add_library(boost_process_v2_test_impl test_impl.cpp)
target_link_libraries(boost_process_v2_test_impl Boost::unit_test_framework Boost::process)
target_compile_definitions(boost_process_v2_test_impl PUBLIC -DBOOST_PROCESS_V2_SEPARATE_COMPILATION=1)
target_include_directories(boost_process_v2_test_impl PUBLIC ../../include)

function(boost_process_v2_standalone_test name)
    add_executable(boost_process_v2_${name} ${name}.cpp)
    target_link_libraries(boost_process_v2_${name} Boost::system Boost::filesystem boost_process_v2_test_impl)
    add_test(NAME boost_process_v2_${name} COMMAND $<TARGET_FILE:boost_process_v2_${name}> )
endfunction()

boost_process_v2_standalone_test(utf8)
boost_process_v2_standalone_test(cstring_ref)
boost_process_v2_standalone_test(pid)
boost_process_v2_standalone_test(environment)

add_library(boost_process_v2_header_test header_1.cpp header_2.cpp)