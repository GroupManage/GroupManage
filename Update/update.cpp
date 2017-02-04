/*
GroupManage was built in order to simplify the creation of groups. Please refer to the documentation for more details.

Copyright (C) 2016-2017  Simon Vareille

This file is part of GroupManage.

GroupManage is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

GroupManage is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GroupManage. If not, see <http://www.gnu.org/licenses/>.

In addition, as a special exception, the copyright holders give
permission to link the code of portions of this program with the
OpenSSL library. You must obey the GNU General Public License in
all respects for all of the code used other than OpenSSL. If you
modify file(s) with this exception, you may extend this exception
to your version of the file(s), but you are not obligated to do so.
If you do not wish to do so, delete this exception statement from
your version. If you delete this exception statement from all source
files in the program, then also delete it here.

For any questions or suggestions, please contact me at <groupmanage.assistance@gmail.com>
*/

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <locale>
#include <thread>
#include <sstream>
#include <sstream>

//conteneurs de la STL
#include <string>
#include <tuple>
#include <atomic>
#include <condition_variable>
#include <mutex>

//boost
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/algorithm/string.hpp>

#ifdef _WIN32
#define _WIN32_IE 0x0500
//#define WINVER    0x0500
#include <windows.h>
#include <Windowsx.h>
#include <commctrl.h>
#include <Psapi.h>
#include <Shellapi.h>
#include <fcntl.h>
#include "resource.h"

#define WM_NOTIFICON WM_USER+1
#else
#include <boost/filesystem.hpp>
#endif //WIN32

//LZMA
#include <C/Util/7z/7zlib/7zlib.h>

//Autre
#include "HTTPSclient.h"
#include "EmpreinteSHA256.h"

#ifndef CHAR_PATH_SEPARATOR

#ifdef _WIN32
#define CHAR_PATH_SEPARATOR '\\'
#else
#define CHAR_PATH_SEPARATOR '/'
#endif // _WIN32

#endif // CHAR_PATH_SEPARATOR

#define SOFTWARE_NAME "GroupManage"

struct FilesInfo
{
    std::string path;
    int compressed=false;
};

/*static WRes MyCreateDir(std::string const &name)
{
    #ifdef USE_WINDOWS_FILE

    return CreateDirectoryW((LPCWSTR)name, NULL) ? 0 : GetLastError();

    #else

    CBuf buf;
    WRes res;
    Buf_Init(&buf);
    RINOK(Utf16_To_Char(&buf, name MY_FILE_CODE_PAGE_PARAM));

    res =
    #ifdef _WIN32
    _mkdir((const char *)buf.data)
    #else
    mkdir((const char *)buf.data, 0777)
    #endif
    == 0 ? 0 : errno;
    Buf_Free(&buf, &g_Alloc);
    return res;

    #endif
}*/

/**< Lancer avec la commande :"Update.exe >output.txt 2>&1" */

#ifdef _WIN32
#define WM_USERCREATE WM_USER+100

static HWND hWnd;
std::atomic<bool> is_running(true);
std::atomic<bool> is_update_ready(false);
std::atomic<short> userAnswer;
std::mutex mtx;
std::condition_variable notifyUserAnswer;
LRESULT CALLBACK WinMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
int ProcessMessage(HWND &hWnd, NOTIFYICONDATA &icone);
bool CreateToolTip(HWND hwnd, HWND &hwndToolTips, HINSTANCE const &hinstance, char* text, char* title, POINT &pt);
long GetHeigthTray();

LRESULT CALLBACK new_subclassed_tooltip(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC previous_subclassed_tooltip;//global variable to save a copy of the default control procedure
void subclass_the_tooltip(HWND h_tooltip)
{
    previous_subclassed_tooltip = (WNDPROC)GetWindowLongPtr(h_tooltip, GWL_WNDPROC);//get the default procedure
    SetWindowLongPtr(h_tooltip, GWLP_WNDPROC, (LONG_PTR)new_subclassed_tooltip);//set your custom procedure :)
}
#endif // _WIN32

int main(int numArgs, const char* args[])
{
    std::locale::global(std::locale(""));

    if(numArgs<=1)
    {
        #ifdef _WIN32
        int retour=MessageBox(0,"Voulez-vous réparer " SOFTWARE_NAME " ?","Réparer le logiciel ?",MB_ICONQUESTION | MB_YESNO);
        if(retour==IDNO)
        {
            return 0;
        }
        #endif // _WIN32
    }
    try
    {
        #ifdef _WIN32
        {
            std::cout<<"> Test de droits d'écriture..."<<std::endl;
            //boost::filesystem::path path=boost::filesystem::current_path();
            char path[MAX_PATH];
            GetCurrentDirectory(MAX_PATH, path);
            std::string fileName("testWriteFile.tfw");
            std::ofstream ecriture(fileName.c_str(), std::ios::ate);
            if(!ecriture)
            {
                std::cout<<"Update.exe n'a pas les droits pour écrire dans le dossier "<<path<<std::endl;
                return 4;
            }
            ecriture.close();
            remove(fileName.c_str());
            std::cout<<"Update.exe a les droits d'écriture dans le dossier "<<path<<std::endl;
        }
        #endif // _WIN32

        //std::ofstream SortieCoutCerrClog;
        ///Variables permanentes
        std::string currentVersion("");
        int searchButNotDownload=false;
        int searchButNotDownload_Notify=false;
        #ifdef _WIN32
        NOTIFYICONDATA icone;
        std::thread iconThread;
        #endif // _WIN32

        #ifdef _WIN32
        iconThread=std::thread(ProcessMessage, std::ref(hWnd), std::ref(icone));
        #endif // _WIN32

        ///Récupération de la version courante

        std::cout<<"> Arguments:"<<std::endl;
        for(int i=0;i<numArgs;i++)
        {
            std::cout<<args[i]<<std::endl;
            if(std::string(args[i]).compare("-update")==0)
            {
                std::ifstream IFileVersion("Version.vers");
                if(IFileVersion)
                    IFileVersion>>currentVersion;
                IFileVersion.close();
            }
            else if(std::string(args[i]).compare("-deleteUpdate")==0)
            {
                std::cout<<std::endl<<"> Supression de UpdateUpdate.exe"<<std::endl;
                remove("UpdateUpdate.exe");
                #ifdef _WIN32
                is_running.store(false, std::memory_order_relaxed);
                PostThreadMessage(GetWindowThreadProcessId(hWnd, NULL), WM_QUIT, 0, 0);
                Shell_NotifyIcon(NIM_DELETE, &icone);
                #endif // _WIN32
                return 0;
            }
            else if(std::string(args[i]).compare("-searchButNotDownload")==0)//Dans ce cas il faut le nom de classe de fenêtre de l'appelant en paramètre suivant.
            {
                searchButNotDownload=i+1;

                std::ifstream IFileVersion("Version.vers");
                if(IFileVersion)
                    IFileVersion>>currentVersion;
                IFileVersion.close();
            }
            else if(std::string(args[i]).compare("-searchButNotDownload_Notify")==0)
            {
                searchButNotDownload_Notify=i+1;
                searchButNotDownload=false;

                std::ifstream IFileVersion("Version.vers");
                if(IFileVersion)
                    IFileVersion>>currentVersion;
                IFileVersion.close();

            }

        }

        std::cout<<std::endl<<"> Lancement du service..."<<std::endl;

        boost::asio::io_service io_service;
        //groupmanage.github.io
        HTTPSclient client(io_service, "raw.githubusercontent.com");//raw.githubusercontent.com

        //client.SaveFile("/GroupManage/Update/master","/Pack/Traitement.7z");
        // /Update.txt
        //https://github.com/GroupManage/GroupManage/raw/master/bin/Commune.dll
        //https://raw.githubusercontent.com/GroupManage/GroupManage/master/
        ///Réception et traitement du fichier de gestion des mises à jour
        client.SendGetFileRequest("/GroupManage/Update/master/README.md", "keep-alive");
        std::string readmeContents;
        client.RecvFile(readmeContents);
        size_t pos(0);

        ///Vérification de la version
        pos=readmeContents.find("\n[Version ");
        std::string versionMAJ="";
        if(pos!=std::string::npos)
        {
            if(readmeContents.find("]", pos+10)==std::string::npos)
            {
                std::cerr<<"Erreur de formatage du fichier de gestion de mises à jour"<<std::endl;
                #ifdef _WIN32
                is_running.store(false, std::memory_order_relaxed);
                PostThreadMessage(GetWindowThreadProcessId(hWnd, NULL), WM_QUIT, 0, 0);
                Shell_NotifyIcon(NIM_DELETE, &icone);
                #endif // _WIN32
                return 0;
            }
            versionMAJ=readmeContents.substr(pos+10, readmeContents.find("]", pos+10)-pos-10);
            std::cout<<std::endl<<"> Version de la mise à jour : "<<versionMAJ<<std::endl;


            if(versionMAJ.compare(currentVersion)==0)
            {
                std::cout<<"Le logiciel est à jour"<<std::endl;
                if(searchButNotDownload)
                {
                    #ifdef _WIN32
                    MessageBox(0,"Le logiciel est à jour.","Logiciel à jour",MB_ICONINFORMATION);
                    #endif // _WIN32
                }
                else if(searchButNotDownload_Notify)
                {
                    #ifdef _WIN32
                    is_running.store(false, std::memory_order_relaxed);
                    is_update_ready.store(false, std::memory_order_relaxed);

                    POINT pt={GetSystemMetrics(SM_CXSCREEN)-50, 0};
                    SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(CREATETOOLTIP,0), (LPARAM)&pt);

                    Sleep(30000);
                    PostThreadMessage(GetWindowThreadProcessId(hWnd, NULL), WM_QUIT, 0, 0);
                    iconThread.join();
                    Shell_NotifyIcon(NIM_DELETE, &icone);
                    #endif //_Win32
                    return 2;
                }
                #ifdef _WIN32
                is_running.store(false, std::memory_order_relaxed);
                PostThreadMessage(GetWindowThreadProcessId(hWnd, NULL), WM_QUIT, 0, 0);
                Shell_NotifyIcon(NIM_DELETE, &icone);
                iconThread.join();
                #endif // _WIN32
                return 2;
            }
            if(searchButNotDownload)
            {
                #ifdef _WIN32
                if(MessageBox(0,"Une mise à jour est prète!\nInstaller maintenant ?","Logiciel à jour",MB_ICONINFORMATION | MB_YESNO)==IDNO)
                {
                    is_running.store(false, std::memory_order_relaxed);
                    PostThreadMessage(GetWindowThreadProcessId(hWnd, NULL), WM_QUIT, 0, 0);
                    Shell_NotifyIcon(NIM_DELETE, &icone);
                    iconThread.join();
                    return 1;
                }
                if(searchButNotDownload<numArgs && args[searchButNotDownload][0]!='-')
                {
                    SendMessage(FindWindow(args[searchButNotDownload], NULL ), WM_CLOSE, 0, 0);
                }
                #endif // _WIN32
            }
            else if(searchButNotDownload_Notify)
            {
                #ifdef _WIN32
                is_running.store(false,std::memory_order_relaxed);
                is_update_ready.store(true, std::memory_order_relaxed);
                POINT pt={GetSystemMetrics(SM_CXSCREEN)-50, 0};
                SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(CREATETOOLTIP,0), (LPARAM)&pt);

                std::unique_lock<std::mutex> lck(mtx);
                notifyUserAnswer.wait(lck);
                if(userAnswer.load(std::memory_order_relaxed)==IDNO || userAnswer.load(std::memory_order_relaxed)==-1)
                {
                    iconThread.join();
                    Shell_NotifyIcon(NIM_DELETE, &icone);
                    return 1;
                }

                is_running.store(true,std::memory_order_relaxed);
                #endif //_Win32

            }
            #ifdef _WIN32
            is_update_ready.store(true, std::memory_order_relaxed);
            #endif // _WIN32

        }
        else
        {
            std::cerr<<"Erreur de formatage du fichier de gestion de mises à jour"<<std::endl;
            #ifdef _WIN32
            is_running.store(false, std::memory_order_relaxed);
            PostThreadMessage(GetWindowThreadProcessId(hWnd, NULL), WM_QUIT, 0, 0);
            Shell_NotifyIcon(NIM_DELETE, &icone);
            iconThread.join();
            #endif // _WIN32
            return -1;
        }
        ///On ferme l'application à mettre à jour
        #ifdef _WIN32
        while(FindWindow("GroupManageMainWindow", NULL))
        {
            SendMessage(FindWindow("GroupManageMainWindow", NULL), WM_CLOSE, 0, 0);
        }
        #endif // _WIN32

        ///Chemin du pack
        std::string pathPack;
        ///Chemin des fichiers
        std::vector<FilesInfo> pathFiles;
        ///Chemin des fichiers dans le pack
        std::vector<std::string> pathFilesInPack;

        ///Téléchargement du pack compressé
        if(currentVersion.size()!=0)
        {
            std::cout<<std::endl<<"> Recherche du pack compressé..."<<std::endl;
            pos=readmeContents.find("\n**Pack :**\n");
            if(pos!=std::string::npos)
            {
                std::string strPack=readmeContents.substr(pos+12, readmeContents.find("\n**",pos+12)-pos-12);
                //std::cout<<std::endl<<std::endl<<strPack<<std::endl<<std::endl<<std::endl;
                pos=strPack.find("\n/")+1;
                if(pos!=std::string::npos)
                {
                    pathPack=strPack.substr(pos, strPack.find("  \n", pos)-pos);
                    //std::cout<<std::endl<<std::endl<<pathPack<<std::endl<<std::endl;
                    if(client.SaveFile("/GroupManage/Update/master",pathPack))
                    {
                        pathPack="";
                    }
                }
                else
                {
                    std::cout<<"Pas de Pack compressé"<<std::endl;
                }
            }
            else
            {
                std::cout<<"Pas de Pack compressé"<<std::endl;
            }
        }

        ///Si réparration nécessaire :
        size_t packSize(0);
        if(currentVersion.size()==0)
        {
            pos=readmeContents.find("\n**Pack :**\n");
            if(pos!=std::string::npos)
            {
                std::string strPack=readmeContents.substr(pos+12, readmeContents.find("\n**",pos+12)-pos-12);
                pos=strPack.find("\n> Size: ");
                if(pos!=std::string::npos)
                {
                    packSize=string_to<size_t>(strPack.substr(pos+9, strPack.find("  \n", pos+11)-pos-9));
                }
            }
        }

        std::cout<<std::endl<<"> Vérification des fichiers..."<<std::endl;
        ///Vérification de touts les fichiers
        size_t totalSize(0);


        pos=readmeContents.find("\n**All :**\n");
        if(pos!=std::string::npos)
        {
            std::string strAll=readmeContents.substr(pos+11, readmeContents.find("\n**",pos+11)-pos-11);
            pos=0;
            size_t posFirstLine=0;
            //std::vector<std::string> tabPathFiles;
            while(pos!=std::string::npos)
            {
                pos=strAll.find("\n/", pos)+1;
                posFirstLine=pos;
                std::string strFile=strAll.substr(pos, strAll.find("\n/", pos)-pos);
                pos=strFile.find("  \n");
                FilesInfo temp;
                temp.path=strFile.substr(1, pos);
                temp.compressed=false;
                pathFiles.push_back(temp);
                std::cout<<">>>"<<pathFiles.back().path<<std::endl;


                ///Fichier dans le Pack ?
                bool isInPack=false;
                pos=strFile.find("\n> Pack: ");
                if(pos!=std::string::npos && strFile.substr(pos+9, strFile.find("  \n", pos+11)-pos-9)=="yes")
                {
                    isInPack=true;
                }

                ///Taille du fichier
                size_t fileSize;
                pos=strFile.find("\n> Size: ");
                if(pos!=std::string::npos)
                {
                    fileSize=string_to<size_t>(strFile.substr(pos+9, strFile.find("  \n", pos+11)-pos-9));
                }

                ///Décompression nesséssaire ?
                pos=strFile.find("\n> Compressed: ");
                if(pos!=std::string::npos && strFile.substr(pos+15, strFile.find("  \n", pos+17)-pos-15)=="yes")
                {
                    pathFiles.back().compressed=true;
                }

                ///Comparaison des epreintes SHA256
                pos=strFile.find("\n> SHA256: ");
                bool areSha256Equal;
                if(pathFiles.back().path.compare(0,16,"UpdateUpdate.exe")==0 || pathFiles.back().path.compare(0,15,"UpdateUpdate.7z")==0)
                {
                    std::string completPath="Update";
                    completPath+=CHAR_PATH_SEPARATOR;
                    completPath+=pathFiles.back().path;
                    areSha256Equal=boost::iequals(strFile.substr(pos+11, strFile.find("  \n", pos+11)-pos-11), FileSha256(completPath));
                }
                else if(pathFiles.back().path.compare(0,9,"Update.7z")==0)
                {
                    areSha256Equal=boost::iequals(strFile.substr(pos+11, strFile.find("  \n", pos+11)-pos-11), FileSha256("Update.exe"));
                }
                else
                {
                    areSha256Equal=boost::iequals(strFile.substr(pos+11, strFile.find("  \n", pos+11)-pos-11), FileSha256(pathFiles.back().path));
                }
                if(pos!=std::string::npos && !areSha256Equal)
                {
                    if(!isInPack)
                    {
                        ///Enregistrement du fichier
                        size_t retour;
                        if((retour=client.SaveFile("/GroupManage/Update/master/",pathFiles.back().path)))
                        {
                            std::cerr<<"Erreur: "<<retour<<std::endl;
                            pathFiles.pop_back();
                        }
                    }
                    else if(currentVersion.size()==0)//Réparation nécessaire
                    {

                        if(packSize!=0)
                        {
                            totalSize+=fileSize;
                            pathFilesInPack.push_back(pathFiles.back().path);
                            pathFiles.pop_back();

                        }
                        else
                        {
                            size_t retour;
                            if((retour=client.SaveFile("/GroupManage/Update/master/",pathFiles.back().path)))
                            {
                                std::cerr<<"Erreur: "<<retour<<std::endl;
                                pathFiles.pop_back();
                            }
                        }
                    }
                    else
                    {
                        pathFiles.pop_back();
                    }
                }
                else
                {
                    std::cout<<"Fichier à jour."<<std::endl;
                    pathFiles.pop_back();
                }

                pos=strAll.find("\n/", posFirstLine+1);
            }

            if(currentVersion.size()==0 && packSize!=0)//Réparation nécessaire
            {
                if(packSize<=totalSize)
                {
                    pos=readmeContents.find("\n**Pack :**\n");
                    if(pos!=std::string::npos)
                    {
                        std::string strPack=readmeContents.substr(pos+12, readmeContents.find("\n**",pos+12)-pos-12);
                        pos=strPack.find("\n/")+1;
                        if(pos!=std::string::npos)
                        {
                            pathPack=strPack.substr(pos+1, strPack.find("  \n", pos)-pos);
                            client.SaveFile("/GroupManage/Update/master/",pathPack);
                        }
                    }
                }
                else
                {
                    for(size_t i=0;i<pathFilesInPack.size();i++)
                    {
                        client.SaveFile("/GroupManage/Update/master/",pathFilesInPack[i]);
                        FilesInfo temp;
                        temp.path=pathFilesInPack[i];
                        temp.compressed=false;
                        pathFiles.push_back(temp);
                    }
                }//fin else
            }

        }//fin if(pos!=std::string::npos)
        else
        {
            std::cout<<"Pas d'autres fichiers (Erreur ?)"<<std::endl;
        }

        ///Si tout s'est bien déroulé
        bool UpdateMySelf=false;
        if(pathPack.size()!=0)
        {
            std::cout<<std::endl<<"> Décompression du pack..."<<std::endl;
            ///Préparation à la décompression du pack
            pathPack.insert(0,"Update/");

            std::replace(pathPack.begin(), pathPack.end(), '/', CHAR_PATH_SEPARATOR);

            ///Décompression
            Decompress7zArchive(pathPack);
        }
        std::cout<<std::endl<<"> Installation..."<<std::endl;
        for(size_t i=0; i<pathFiles.size(); i++)
        {
            std::cout<<"Installation du fichier: "<<pathFiles[i].path<<std::endl;
            //application de la mise à jour
            //std::string newPath=pathFiles[i].substr(0,pathFiles[i].find("Update/"))+pathFiles[i].substr(pathFiles[i].find("Update/")+7);


            std::replace(pathFiles[i].path.begin(), pathFiles[i].path.end(), '/', CHAR_PATH_SEPARATOR);

            std::string totalPath="Update";
            totalPath+=CHAR_PATH_SEPARATOR;
            totalPath+=pathFiles[i].path;

            if(pathFiles[i].path.compare(0,9,"Update.7z")==0)
            {
                Decompress7zArchive(totalPath, "Update");
                pathFiles[i].path="Update.exe";
            }

            if(pathFiles[i].path.compare(0,10,"Update.exe")==0)
            {
                UpdateMySelf=true;
                continue;
            }

            /*if(pathFiles[i][0]=='\\' || pathFiles[i][0]=='/')
                pathFiles[i].erase(0,1);*/

            ///Création du dossier
            #ifdef WIN32
            {
                size_t pos=0;
                pos=pathFiles[i].path.find('\\', pos+1);
                while(pos!=std::string::npos)
                {
                    CreateDirectory(pathFiles[i].path.substr(0,pathFiles[i].path.find('\\', pos)).c_str(),NULL);
                    pos=pathFiles[i].path.find('\\', pos+1);
                }
            }

            #else
                boost::filesystem::path dir(pathFiles[i].path);
                boost::filesystem::create_directory(dir.parent_path());
            #endif

            remove(pathFiles[i].path.c_str());

            if(pathFiles[i].compressed)
            {
                Decompress7zArchive(totalPath);
            }
            else if(rename(totalPath.c_str(), pathFiles[i].path.c_str()))
            {
                perror("Erreur de redirection du fichier");
            }
        }

        //client.SaveFile("/GroupManage/Update/master","/bin/Traitement.dll");

        if(UpdateMySelf)
        {
            ///Décompression de l'exe de remplacement
            //Decompress7zArchive("Update/UpdateUpdate.7z");
            std::cout<<std::endl<<"> Mise a jour de Update.exe..."<<std::endl;
            ///Lancement de l'exe de remplacement
            PROCESS_INFORMATION Process = {0};
            STARTUPINFO Start = {0};
            Start.cb = sizeof(STARTUPINFO);
            Start.lpReserved = NULL;
            Start.lpReserved2 = NULL;
            Start.cbReserved2 = 0;
            Start.lpDesktop = NULL;
            Start.dwFlags = 0;

            std::string sparam="UpdateUpdate.exe ";
            DWORD processId=GetCurrentProcessId();
            sparam+=to_string(processId);
            std::vector<char> parameters(sparam.begin(), sparam.end());
            parameters.push_back('\0');

            CreateProcess("UpdateUpdate.exe", &parameters[0], NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &Start, &Process);
            CloseHandle(Process.hProcess);
            CloseHandle(Process.hThread);
        }

        std::ofstream OFileVersion("Version.vers");
        OFileVersion<<versionMAJ;
        OFileVersion.close();

        #ifdef _WIN32
        is_running.store(false, std::memory_order_relaxed);
        PostThreadMessage(GetWindowThreadProcessId(hWnd, NULL), WM_QUIT, 0, 0);
        Shell_NotifyIcon(NIM_DELETE, &icone);
        iconThread.join();
        #endif // _WIN32
    }
    catch (std::exception& e)
    {
        std::cerr << "/!\\Exception: " << e.what() << "\n";
        return -1;
    }
    return 0;
}


#ifdef _WIN32
int ProcessMessage(HWND &hWnd, NOTIFYICONDATA &icone)
{
    hWnd=CreateWindowEx(WS_EX_NOACTIVATE, "STATIC", "", 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, NULL);
    SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WinMsgProc);

    icone.cbSize = sizeof(icone);
    icone.uID = IDI_ICON;
    icone.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    icone.uCallbackMessage = WM_NOTIFICON;
    strcpy(icone.szTip,"GroupManage Update");
    icone.hWnd = hWnd;
    icone.hIcon = LoadIcon((HINSTANCE) GetModuleHandle (NULL),MAKEINTRESOURCE(IDI_ICON));
    icone.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIcon(NIM_ADD, &icone);
    Shell_NotifyIcon(NIM_SETVERSION, &icone);
    MSG msg;
    while(GetMessage(&msg,0,0,0))
    {
        if (IsDialogMessage(hWnd, &msg) == 0)
        {
            DispatchMessage(&msg);
            TranslateMessage(&msg);
        }
    }


    if(is_running.load(std::memory_order_relaxed))
    {
        Shell_NotifyIcon(NIM_DELETE, &icone);
        exit(3);
    }
    return 0;
}
LRESULT CALLBACK WinMsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HWND static hwndToolTips;
    switch (msg)
    {
        case WM_NOTIFICON:
        {
            if (LOWORD(lParam)==WM_LBUTTONUP)
            {
                POINT position_souris;
                GetCursorPos(&position_souris);
                SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(CREATETOOLTIP,0), (LPARAM)&position_souris);

                /*RECT rect;
                GetWindowRect(hwndToolTips, &rect);*/

                //MessageBox(0, "Le logiciel est à jour.", "Logiciel à jour", MB_ICONINFORMATION);
                //PostQuitMessage(2);
                return 0;
            }
            if (LOWORD(lParam)==WM_RBUTTONDOWN)
            {
                POINT position_souris;
                GetCursorPos(&position_souris);

                HMENU hPopupMenu = CreatePopupMenu();
                InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, WM_QUIT, "Quitter");
                if(is_update_ready.load(std::memory_order_relaxed) && !is_running.load(std::memory_order_relaxed))
                {
                    InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_BUTTONINSTALL, "Installer");
                }

                /*if(TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD,
                   position_souris.x, position_souris.y, 0, hwnd, NULL)==SW_RESTORE)
                {
                    ShowWindow(hwnd,SW_RESTORE);
                    return 0;
                }*/
                if(TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD,
                   position_souris.x, position_souris.y, 0, hwnd, NULL)==WM_QUIT)
                {
                    std::unique_lock<std::mutex> lck(mtx);
                    userAnswer.store(-1, std::memory_order_relaxed);
                    notifyUserAnswer.notify_all();
                    DestroyWindow(hwnd);
                    PostQuitMessage (2); /* send a WM_QUIT to the message queue */
                    return 0;
                }
                if(TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD,
                   position_souris.x, position_souris.y, 0, hwnd, NULL)==ID_BUTTONINSTALL)
                {
                    std::unique_lock<std::mutex> lck(mtx);
                    userAnswer.store(IDYES, std::memory_order_relaxed);
                    notifyUserAnswer.notify_all();
                    //DestroyWindow(hwnd);
                    return 0;
                }
                //SendMessage(hwnd, WM_COMMAND,TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD, position_souris.x, position_souris.y, 0, hwnd, NULL),0);
            }
            return 0;
        }
        case WM_COMMAND:
        {
            if(LOWORD(wParam)==CREATETOOLTIP)
            {
                POINT *pt=(POINT*)lParam;
                std::string stext;
                std::string stitle;
                if(is_update_ready.load(std::memory_order_relaxed) && !is_running.load(std::memory_order_relaxed))
                {
                    stitle="Nouvelle mise à jour";
                    stext="Une mise à jour est prête pour "+std::string(SOFTWARE_NAME)+".\n\
Voulez-vous l'installer maintenant ?\n\n";
                }
                else if(!is_update_ready.load(std::memory_order_relaxed) && !is_running.load(std::memory_order_relaxed))
                {
                    stitle="Logiciel à jour";
                    stext="Le logiciel est à jour.";
                }
                else if(!is_update_ready.load(std::memory_order_relaxed) && is_running.load(std::memory_order_relaxed))
                {
                    stitle="Recherche de mise à jour...";
                    stext="Recherche de mise à jour pour le logiciel...\n\
Veuillez patienter.";
                }
                else if(is_update_ready.load(std::memory_order_relaxed) && is_running.load(std::memory_order_relaxed))
                {
                    stitle="Mise à jour du logiciel..";
                    stext="La mise à jour du logiciel est en cours...\n\
Veuillez patienter.";
                }
                std::vector<char> text(stext.begin(), stext.end());
                text.push_back(0);
                std::vector<char> title(stitle.begin(), stitle.end());
                title.push_back(0);
                pt->y=GetSystemMetrics(SM_CYSCREEN)-GetHeigthTray()-1;
                CreateToolTip(hwnd, hwndToolTips, GetModuleHandle(NULL), &text[0], &title[0], *pt);
            }
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_QUIT:
        {
            return 0;
        }
        case WM_TIMER:
        {
            if(wParam == ID_TIMER_TOOLTIP)
	 	    {
                //ShowWindow(hwndToolTips, SW_HIDE);
                AnimateWindow(hwndToolTips, 500, AW_CENTER | AW_HIDE );
                KillTimer(hwnd, ID_TIMER_TOOLTIP);
	 	    }
	 	    return 0;
        }
        default:
            return 0;
    }
    return 0;
}
//#endif
bool CreateToolTip(HWND hwnd, HWND &hwndToolTips, HINSTANCE const &hinstance, char* text, char* title, POINT &pt)
{
    InitCommonControls();
    hwndToolTips = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,  WS_POPUP | TTS_BALLOON | TTS_CLOSE | TTS_NOPREFIX | TTS_ALWAYSTIP  ,
				 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, hinstance, NULL);//WS_POPUP | TTS_ALWAYSTIP |

    subclass_the_tooltip(hwndToolTips);

    TOOLINFO toolInfo = { 0 };
    toolInfo.cbSize = sizeof(toolInfo);
    toolInfo.hwnd = hwnd;
    toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRACK | NIF_INFO;
    toolInfo.uId = (UINT_PTR)hwndToolTips;//hwnd;
    toolInfo.lpszText = (LPSTR)text;
    SendMessage(hwndToolTips, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

    SendMessage(hwndToolTips,TTM_SETTITLEA, 1,(LPARAM)title);

    SendMessage(hwndToolTips,TTM_TRACKPOSITION,0,MAKELPARAM(pt.x,pt.y));
    SendMessage(hwndToolTips, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&toolInfo);

    SendMessage(hwndToolTips, WM_USERCREATE, 0, 0);

    SetTimer(hwnd, ID_TIMER_TOOLTIP, 30000, NULL);//timer de 30 sec pour la disparition

    return 0;
}

long GetHeigthTray()
{
    RECT tray;
    HWND hWnd;

    if (NULL ==(hWnd = FindWindow("Shell_TrayWnd", NULL)))
    {
        return -1;
    }
    if(!GetWindowRect(hWnd, &tray))
    {
        return -1;
    }
    return tray.bottom - tray.top;
}

LRESULT CALLBACK new_subclassed_tooltip(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND  hButtonTooltipYes;
    static HWND  hButtonTooltipNo;
    switch(msg)
    {
        case WM_SHOWWINDOW:
        {
            /*if(!is_running.load(std::memory_order_relaxed))
            {
                std::unique_lock<std::mutex> lck(mtx);
                userAnswer.store(IDNO, std::memory_order_relaxed);
                notifyUserAnswer.notify_all();
                DestroyWindow(hwnd);
                PostQuitMessage (2);
            }*/
            break;
        }
        /*case WM_PAINT:
        {
            PAINTSTRUCT paint;
            BeginPaint(hwnd,&paint);
            //SetBkColor((HDC)wParam, RGB(49,106,197));
            //SetTextColor((HDC)wParam, RGB(255,255,255));
            EndPaint(hwnd,&paint);
            break;
        }*/
        case WM_USERCREATE:
        {
            if(is_update_ready.load(std::memory_order_relaxed) && !is_running.load(std::memory_order_relaxed))
            {
                RECT crect;
                RECT wrect;
                GetClientRect(hwnd, &crect);
                GetWindowRect(hwnd, &wrect);
                //MoveWindow(hwnd, wrect.left, wrect.top-30, wrect.right, wrect.bottom+30, true);
                //SetWindowPos(hwnd, HWND_TOPMOST, 20, 20, wrect.right, wrect.bottom+30, SWP_SHOWWINDOW);
                hButtonTooltipYes=CreateWindowEx(0,"button", "Installer", WS_CHILD |WS_VISIBLE , crect.right/2-65, crect.bottom-50,60,22,hwnd,(HMENU)ID_BUTTONINSTALL,0,NULL);
                hButtonTooltipNo =CreateWindowEx(0,"button", "Plus tard", WS_CHILD |WS_VISIBLE , crect.right/2+5, crect.bottom-50,60,22,hwnd,(HMENU)ID_BUTTONNOTINSTALL,0,NULL);
                //ShowWindow(hwnd, SW_SHOWNORMAL);
            }
            break;
        }
        case WM_COMMAND:
        {
            if(LOWORD(wParam)==ID_BUTTONINSTALL)
            {
                std::unique_lock<std::mutex> lck(mtx);
                userAnswer.store(IDYES, std::memory_order_relaxed);
                notifyUserAnswer.notify_all();
                DestroyWindow(hwnd);
            }
            if(LOWORD(wParam)==ID_BUTTONNOTINSTALL)
            {
                std::unique_lock<std::mutex> lck(mtx);
                userAnswer.store(IDNO, std::memory_order_relaxed);
                notifyUserAnswer.notify_all();
                DestroyWindow(hwnd);
                DestroyWindow(hWnd);
            }
            break;
        }
        /*case WM_RBUTTONUP:
        {
            if(is_update_ready.load(std::memory_order_relaxed))
            {
                std::unique_lock<std::mutex> lck(mtx);
                userAnswer.store(IDYES, std::memory_order_relaxed);
                notifyUserAnswer.notify_all();
                DestroyWindow(hwnd);
            }
            return 0;
        }*/
        default:
		{
		    return CallWindowProc(previous_subclassed_tooltip, hwnd, msg, wParam, lParam);
		}
	}
    return CallWindowProc(previous_subclassed_tooltip, hwnd, msg, wParam, lParam);
}

#endif // _WIN32
