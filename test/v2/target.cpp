#include <iostream>
#include <string>
#include <boost/process/v2/environment.hpp>

#if defined(BOOST_PROCESS_V2_WINDOWS)
#include <windows.h>
#else
#include <unistd.h>
#endif


int main(int argc, char * argv[])
{
    std::string mode = argv[1];
    if (mode == "exit-code")
        return std::stoi(argv[2]);
    else if (mode == "print-args")
        for (auto i = 0; i < argc; i++)
        {
            std::cout << argv[i] << std::endl;
            std::cerr << argv[i] << std::endl;
            if (!std::cout || !std::cerr)
                return 1;

        }
    else if (mode == "echo")
        std::cout << std::cin.rdbuf();
    else if (mode == "print-cwd")
    {
#if defined(BOOST_PROCESS_V2_WINDOWS)
        wchar_t buf[65535];
        const auto sz = ::GetCurrentDirectoryW(sizeof(buf), buf);
        std::wcout << boost::process::v2::wstring_view(buf, sz) << std::flush;
#else
        char buf[65535];
        printf(::getcwd(buf, sizeof(buf)));
#endif
    }
    else if (mode == "check-eof")
    {
        std::string st;
        std::cin >> st;
        return std::cin.eof() ? 0 : 1;
    }
    else if (mode == "print-env")
    {
        auto p = ::getenv(argv[2]);
        assert(printf("%s", p) > 0);
    }
#if defined(BOOST_PROCESS_V2_WINDOWS)
    else if (mode == "showwindow")
    {
        STARTUPINFO si;
        GetStartupInfo(&si);   
        return static_cast<int>(si.wShowWindow);
    }
    else if (mode == "creation-flags")
    {
        STARTUPINFO si;
        GetStartupInfo(&si);   
        return static_cast<int>(si.dwFlags);
    }
#endif
    else
        return 34;
        
    return 0;
}