enable_testing()


add_executable(boost_process_sparring_partner sparring_partner.cpp )
target_link_libraries(boost_process_sparring_partner Boost::process Boost::lambda Boost::program_options Boost::filesystem Boost::iostreams)

add_executable(boost_process_exit_argc exit_argc.cpp)

add_executable(boost_process_sub_launch sub_launcher.cpp)
target_link_libraries(boost_process_sub_launch Boost::process Boost::program_options Boost::filesystem Boost::iostreams Boost::system)

function(process_standalone_test name )
    add_executable(boost_process_${name} ${name}.cpp)
    target_link_libraries(boost_process_${name} Boost::process Boost::system Boost::filesystem Boost::unit_test_framework)
    add_test(NAME boost_process_${name} COMMAND $<TARGET_FILE:boost_process_${name}> )
endfunction()

process_standalone_test(environment)
process_standalone_test(async_pipe)
process_standalone_test(pipe)

function(process_sub_launch_test name )
    add_executable(boost_process_${name} ${name}.cpp)
    target_link_libraries(boost_process_${name} Boost::process Boost::system Boost::filesystem Boost::scope_exit Boost::thread Boost::unit_test_framework)
    add_test(NAME boost_process_${name} COMMAND $<TARGET_FILE:boost_process_${name}> $<TARGET_FILE:boost_process_sub_launch> )
endfunction()

process_sub_launch_test(group)
process_sub_launch_test(group_wait)

function(process_sparring_partner_launch name )
    add_executable(boost_process_${name} ${name}.cpp)
    target_link_libraries(boost_process_${name} Boost::process Boost::system Boost::filesystem Boost::thread Boost::unit_test_framework Boost::program_options)
    add_test(NAME boost_process_${name} COMMAND $<TARGET_FILE:boost_process_${name}> $<TARGET_FILE:boost_process_sparring_partner> )
endfunction()

process_sparring_partner_launch(async)
process_sparring_partner_launch(async_fut)
process_sparring_partner_launch(args_handling)
process_sparring_partner_launch(args_cmd)
process_sparring_partner_launch(wargs_cmd)
process_sparring_partner_launch(bind_stderr)
process_sparring_partner_launch(bind_stdin)
process_sparring_partner_launch(bind_stdin_stdout)
process_sparring_partner_launch(bind_stdout)
process_sparring_partner_launch(bind_stdout_stderr)
process_sparring_partner_launch(pipe_fwd)
process_sparring_partner_launch(cmd_test)
process_sparring_partner_launch(close_stderr)
process_sparring_partner_launch(close_stdin)
process_sparring_partner_launch(close_stdout)
process_sparring_partner_launch(error)
process_sparring_partner_launch(exit_code)
process_sparring_partner_launch(extensions)
process_sparring_partner_launch(env)
process_sparring_partner_launch(limit_fd)
process_sparring_partner_launch(run_exe)
process_sparring_partner_launch(run_exe_path)
process_sparring_partner_launch(search_path)
process_sparring_partner_launch(shell)
process_sparring_partner_launch(shell_path)
process_sparring_partner_launch(system_test1)
process_sparring_partner_launch(system_test2)
process_sparring_partner_launch(spawn)
process_sparring_partner_launch(start_dir)
process_sparring_partner_launch(terminate)
process_sparring_partner_launch(throw_on_error)
process_sparring_partner_launch(wait)
process_sparring_partner_launch(wait_for)
process_sparring_partner_launch(on_exit)
process_sparring_partner_launch(on_exit2)
process_sparring_partner_launch(on_exit3)

if(WIN32)
    process_sparring_partner_launch(windows_specific)
else()
    process_sparring_partner_launch(posix_specific)
endif()

