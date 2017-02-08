#include <windows.h>
#include <iostream>
#include <sstream>
#include <cstdio>
//Conteneurs de la STL
#include <string>
#include <vector>

#ifdef _WIN32
#define CHAR_PATH_SEPARATOR '\\'
#else
#define CHAR_PATH_SEPARATOR '/'
#endif // _WIN32

template<typename T> std::string to_string(T const &var)
{
    std::ostringstream oss;
    oss<<var;
    return oss.str();
}

template<typename T> T string_to(std::string const &var)
{
    T retour;
    std::istringstream oss(var);
    oss>>retour;
    return retour;
}

int main(int numArgs, const char* args[])
{
    if(numArgs<2)
        return -1;
    std::cout<<std::endl<<"# UpdateUpdate.exe is running... #"<<std::endl;
    std::cout<<std::endl<<"> Arguments:"<<std::endl;
    for(int i=0;i<numArgs;i++)
    {
        std::cout<<args[i]<<std::endl;
    }

    WaitForSingleObject(OpenProcess(SYNCHRONIZE, FALSE, string_to<DWORD>(std::string(args[1]))), INFINITE);

    std::cout<<std::endl<<"> Remplacement de Update.exe..."<<std::endl;

    remove("Update.exe");
    int result;
    result=rename("Update\\Update.exe","Update.exe");
    if(result)
    {
        perror( "Error renaming file" );
        return -1;
    }
    else
    {
        PROCESS_INFORMATION Process = {0};
        STARTUPINFO Start = {0};
        Start.cb = sizeof(STARTUPINFO);
        Start.lpReserved = NULL;
        Start.lpReserved2 = NULL;
        Start.cbReserved2 = 0;
        Start.lpDesktop = NULL;
        Start.dwFlags = 0;

        std::string sparam="Update.exe -deleteUpdate";
        std::vector<char> parameters(sparam.begin(), sparam.end());
        parameters.push_back('\0');

        CreateProcess("Update.exe", &parameters[0], NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &Start, &Process);
        CloseHandle(Process.hProcess);
        CloseHandle(Process.hThread);
    }
    return 0;
}
