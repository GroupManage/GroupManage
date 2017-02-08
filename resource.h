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


#define TRI_ANSCII             0
#define TRI_NOMBRE             1


#define ID_LUNCH              10
#define ID_COMBOACTI          11
#define IDE_LISTAPP           12
#define ID_HEADER             13
#define ID_LISTBOX            14
#define ID_LISTBOXCLASSE      15
#define ID_EDITLIST           16
#define ID_BUTTONADD          17
#define ID_BUTTONNEWACTI      18
#define ID_COMBOV1            19
#define ID_COMBOV2            20
#define ID_COMBOV3            21
#define ID_STATUS             22
#define ID_LISTVIEW           23
#define ID_INSTALLUPDATE      24
#define ID_BUTTONGOLINE       25
#define ID_BUTTONFINDNAME     26
#define ID_BUTTONFINDNEXT     27
#define ID_CHECKBOX           28
#define ID_BUTTONCLASSES      29
#define ID_CHECKBOXBACKUP     30
#define ID_EDITNBPLACES       31
#define ID_CHANGEITEMTABPLUS  32
#define ID_CHANGEITEMTABMOINS 33
#define ID_COMBOAPPCLASS      34
#define ID_ETCHEDFRAME        35
#define ID_SEARCHUPDATE       36



#define IDM_ABSENT            100
#define IDM_SUPPRSEM          101
#define IDM_CHANGENAME        102
#define PBM_SETMARQUEE (WM_USER+10)

#define WM_CHANGECOLOR        WM_USER+1

#define IDM_PRESENT           103
#define IDM_HELP              104
#define IDM_ABOUT             105
#define IDM_BADAFFECT         106
#define IDM_VERIF_REMPLI      107
#define IDM_VERIF_PLACES      108
#define IDM_FINDNAME          109
#define IDM_GOLINE            110
#define IDM_PASTE             111
#define IDM_VERIF_ACTI        112
#define IDM_COUNT_APPLICATION 113
#define IDM_SETTINGS_UPDATE   114
#define IDM_SEARCHUPDATE      115


#define IDM_OPEN              200
#define IDM_SAVE              201
#define IDM_QUIT              202
#define IDM_SAVEAS            203
#define IDM_SAVEACTI          204
#define IDM_SAVEACTIAS        205
#define IDM_OPENACTI          206
#define IDM_SAVEASFORMAT      207
#define IDM_SAVEASTABL        208
#define IDM_NEW               209


#define IDC_RADIO1            300
#define IDC_RADIO2            301


#define IDE_EDITFINDNAME      400
#define IDE_PB1               401
#define IDE_EDITMODIFNAME     402
#define IDE_EDITMODIFPLACES   403
#define IDE_COMBOBOX          404
#define IDE_UPDOWN            405


#define ID_TIMER_BACKUP       501
#define ID_TIMER              502

#define IDT_MAINTXT           600
#define IDT_TEMPS             601
#define IDT_CYCLE             602
#define IDT_FIABILITET        603
#define IDT_FIABILITE         604
#define IDT_MOYENNEVOEUX      605
#define IDT_RESULTCHOICE      606


#define MSG_SUBCLASSCONTROL   (WM_USER+20)

#define SUB_LISTBOX            1
#define SUB_EDIT               2
#define SUB_COMBOACTIEDIT      3
#define SUB_COMBOACTI          4
#define SUB_EDITGOLINE         5
#define SUB_EDITFINDNAME       6
#define SUB_EDITAJOUTCLASSE    7
#define SUB_COMBOVOEU          8

#define IDI_ICON              1000
#define IDI_ICONGRMA          1001
#define IDI_ICONEAC           1002


//décarations systèmes

#ifndef PBS_MARQUEE
#define PBS_MARQUEE           0x08
#endif // PBS_MARQUEE

#ifndef GWL_WNDPROC
#define GWL_WNDPROC (-4)
#endif // GWL_WNDPROC


