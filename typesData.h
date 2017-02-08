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

//Déclaration des types de données, des structures, des classes

struct Voeux
{
    short voeu1;
    short voeu2;
    short voeu3;
};

struct enfantActi
{
    string nom;
    string classe;
    Voeux voeux[4];
    short ref;
    short affectation[4];

    int point;
    int denominateur;
};

struct InfoActivite
{
    string nom;
    string classe;
    short placesDispo;
    short placesRestantes;
    short demande;
    short jour;
    short ref;
};

/*#else
struct enfantActi
{
    char np[66];
    char classe[34];
    short voeu1;
    short voeu2;
    short voeu3;
    short ref;
    short affectation;
};

struct InfoActivite
{
    char nom[100];
    string classe;
    short placesDispo;
    short placesRestantes;
    short demande;
    short jour;
};


struct infoEleve
{
    int point;
    int denominateur;
};
#endif*/


struct EnumChildWinLParam
{
    SCROLLINFO *infoScroll;
    int scroll;
    int event;
};

struct HWNDcontrol
{
    HWND hwnd;
    RECT rect;
};

struct SortieToutEleve
{
    string tableau[6];
};
