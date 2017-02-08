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

#ifndef _DLLCOMMUNE_H_
#define _DLLCOMMUNE_H_

#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

void DLLIMPORT AfficheNouveauTab(HWND const &hwnd, HWND const &hListboxNom, HWND const &hListboxClasse, HWND const &hstatu,
                                 HWND const &hButtonAdd, HWND const &hCheckboxCasesVide, HWND const &hScroll, HWND const &hComboActi,
                                 vector<HWND> &hComboV1, vector<HWND> &hComboV2, vector<HWND> &hComboV3,
                                 vector<enfantActi> const &enfants, vector<InfoActivite> const &activite,
                                 UINT const &nbLigne, UINT &iItemSel, int &iActiEnCour,
                                 string const &pathFileOpen);
void DLLIMPORT RemplirListeClasse(vector<enfantActi> const &enfants, vector<InfoActivite> const &activite, vector<string> &nomClasses);
bool DLLIMPORT CreateToolTip(HWND hwnd, HWND &hwndToolTips, HINSTANCE const &hinstance);
bool DLLIMPORT UpdateStatusBar(HWND const &hStatus, vector<enfantActi> const &enfants, HWND const &hWndMain);
bool DLLIMPORT UpdateStatusBar(HWND const &hStatus, HWND const &hListboxNom);
void DLLIMPORT VoirErreur(unsigned long error,const char *titre);
void DLLIMPORT VoirErreur(unsigned long error);
int DLLIMPORT stricmp(string a, string b);


#endif /* _DLLCOMMUNE_H_ */
