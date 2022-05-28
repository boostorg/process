
#include <string>

int main(int argc, char * argv[])
{
    std::string mode = argv[1];
    if (mode == "exit-code")
        return std::stoi(argv[2]);

    return 0;
}