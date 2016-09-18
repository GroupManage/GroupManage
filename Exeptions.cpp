/*
GroupManage was built in order to simplify the creation of groups. Please refer to the documentation for more details.

Copyright (C) 2016  Simon Vareille

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

For any questions or suggestions, please contact me at <groupmanage.assistance@gmail.com>
*/

#include "Exeptions.h"

using namespace std;

namespace myExeptions
{
    Erreur::Erreur(const char* message)
    {
        strcpy(m_message,message);
    }

    Erreur::Erreur(unsigned long error)
    {
        LPVOID lpMsgBuf;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER| FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);
        strcpy(m_message,(LPCTSTR)lpMsgBuf);
        LocalFree(lpMsgBuf);
    }

    void Erreur::AfficheMessage(HWND hParent)
    {
        MessageBox(hParent,m_message,0,MB_ICONERROR);
    }

    void Erreur::AfficheMessage(HWND hParent, const char* titre)
    {
        MessageBox(hParent,m_message,titre,MB_ICONERROR);
    }

}
