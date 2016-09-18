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

#ifndef _DLLTRAITEMENT_H_
#define _DLLTRAITEMENT_H_

#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

//void DLLIMPORT Traitement(vector<enfantActi> &enfants, vector<InfoActivite> &activite);
int DLLIMPORT Initialisation(vector<enfantActi> &enfants, vector<enfantActi> &enfantsAttributed, vector<InfoActivite> &activite,
                              char &inferieurA, bool &traitementLance, bool &arretTraitement,
                              HWND hDlgTraitementEnCours);

#endif /* _DLL_H_ */
