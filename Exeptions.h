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

#ifndef _GESTION_DES_EXEPTIONS_
#define _GESTION_DES_EXEPTIONS_

#include <string>
#include <windows.h>

namespace myExeptions
{
    class Erreur
    {
        public:
            Erreur(const char* Message);
            Erreur(unsigned long error);
            void AfficheMessage(HWND hParent);//virtual void AfficheMessage()=0;
            void AfficheMessage(HWND hParent, const char *titre);
        protected:
            char m_message[100];
    };

    /*class ErreurWSA: public Erreur
    {
        public:
               virtual void AfficheMessage();
    }

    class ErreurSocket: public Erreur
    {
        public:
               virtual void AfficheMessage();
    }*/
}

#endif
