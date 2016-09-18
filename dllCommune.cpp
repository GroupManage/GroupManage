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

#define WINVER 0x0500
#define _WIN32_IE 0x0500

#include <windows.h>
#include <string>
#include "Shlobj.h"
#include <vector>
#include <Windowsx.h>
#include <algorithm>
#include <sstream>

using namespace std;

#include "typesData.h"
#include "Exeptions.h"
#include "resource.h"

#include "dllCommune.h"

#define DLLEXPORT __declspec (dllexport)

template<typename T> string to_string(T const &var)
{
    ostringstream oss;
    oss<<var;
    return oss.str();
}

BOOL APIENTRY DllMain (HINSTANCE hInst     /* Library instance handle. */ ,
                       DWORD reason        /* Reason this function is being called. */ ,
                       LPVOID reserved     /* Not used. */ )
{
    switch (reason)
    {
      case DLL_PROCESS_ATTACH:
          break;

      case DLL_PROCESS_DETACH:
          break;

      case DLL_THREAD_ATTACH:
          break;

      case DLL_THREAD_DETACH:
          break;
    }

    /* Returns TRUE on success, FALSE on failure */
    return TRUE;
}

void DLLEXPORT AfficheNouveauTab(HWND const &hwnd, HWND const &hListboxNom, HWND const &hListboxClasse, HWND const &hstatu,
                                 HWND const &hButtonAdd, HWND const &hCheckboxCasesVide, HWND const &hScroll, HWND const &hComboActi,
                                 vector<HWND> &hComboV1, vector<HWND> &hComboV2, vector<HWND> &hComboV3,
                                 vector<enfantActi> const &enfants, vector<InfoActivite> const &activite,
                                 UINT const &nbLigne, UINT &iItemSel,
                                 string const &pathFileOpen)
{
    while(hComboV1.size())
	{
	    DestroyWindow(hComboV1.back());
	    hComboV1.pop_back();
	    DestroyWindow(hComboV2.back());
	    hComboV2.pop_back();
	    DestroyWindow(hComboV3.back());
	    hComboV3.pop_back();
	}

	SendMessage(hListboxNom,LB_RESETCONTENT,0,0);
	SendMessage(hListboxClasse,LB_RESETCONTENT,0,0);
	/*char a[3];
	itoa(hComboV1.size(),a,10);
	MessageBox(0,a,0,0);*/
	RECT rect;
	GetWindowRect(hListboxNom,&rect);
	SetWindowPos(hListboxNom,0,0,0,rect.right-rect.left,0,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	GetWindowRect(hListboxClasse,&rect);
	SetWindowPos(hListboxClasse,0,0,0,rect.right-rect.left,0,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	SetWindowPos(hButtonAdd,0,8,50,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	LONG Styles=WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_GROUP | CBS_DROPDOWNLIST | CBS_SORT ;//CBS_DROPDOWNLIST |

	if(enfants.size()!=0)
 	{

		RECT rectHwnd;
		GetClientRect(hwnd,&rectHwnd);
	    POINT pt={0,0};
	    hComboV1.push_back(CreateWindowEx(0,"COMBOBOX","Activite",Styles,30+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*0-1,30+20,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,(HMENU)ID_COMBOV1,0,0));
		//hCombo[i]=CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST ,10+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*j-1,10+20+23*i,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,0,0,0);
		SendMessage(hComboV1[0],CB_ADDSTRING,(WPARAM) 0,(Button_GetCheck(hCheckboxCasesVide)==BST_CHECKED) ? (LPARAM)"Pas de choix" : (LPARAM)" ");
		for(int j=0;j<activite.size();j++)
		{
		    SendMessage(hComboV1[0],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
		}
		SendMessage(hwnd, MSG_SUBCLASSCONTROL, MAKEWPARAM(SUB_COMBOVOEU,0), (LPARAM)hComboV1.back());

		SendMessage(hComboV1[0],CB_SETCURSEL,SendMessage(hComboV1[0],CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)activite[enfants[0].voeux[0].voeu1].nom.c_str()),0);
		hComboV2.push_back(CreateWindowEx(0,"COMBOBOX","Activite", Styles,30+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)-1,30+20,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,(HMENU)ID_COMBOV2,0,0));
		//hCombo[i]=CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST ,10+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*j-1,10+20+23*i,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,0,0,0);
		SendMessage(hComboV2[0],CB_ADDSTRING,(WPARAM) 0,(Button_GetCheck(hCheckboxCasesVide)==BST_CHECKED) ? (LPARAM)"Pas de choix" : (LPARAM)" ");
		for(int j=0;j<activite.size();j++)
		{
		    SendMessage(hComboV2[0],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
		}
		SendMessage(hComboV2[0],CB_SETCURSEL,SendMessage(hComboV2[0],CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)activite[enfants[0].voeux[0].voeu2].nom.c_str()),0);
		SendMessage(hwnd, MSG_SUBCLASSCONTROL,MAKEWPARAM(SUB_COMBOVOEU,0), (LPARAM)hComboV2.back());


		hComboV3.push_back(CreateWindowEx(0,"COMBOBOX","Activite",Styles,30+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*2-1,30+20,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3+3,20,hwnd,(HMENU)ID_COMBOV3,0,0));
		SendMessage(hComboV3[0],CB_ADDSTRING,(WPARAM) 0,(Button_GetCheck(hCheckboxCasesVide)==BST_CHECKED) ? (LPARAM)"Pas de choix" : (LPARAM)" ");
		for(int j=0;j<activite.size();j++)
		{
		    SendMessage(hComboV3[0],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
		}
		SendMessage(hComboV3[0],CB_SETCURSEL,SendMessage(hComboV3[0],CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)activite[enfants[0].voeux[0].voeu3].nom.c_str()),0);
		SendMessage(hwnd, MSG_SUBCLASSCONTROL,MAKEWPARAM(SUB_COMBOVOEU,0), (LPARAM)hComboV3.back());


		SendMessage(hListboxNom,LB_ADDSTRING,0,(LPARAM)enfants[0].nom.c_str());
		SendMessage(hListboxNom,LB_SETITEMDATA,0,0);
		SendMessage(hListboxNom,LB_SETITEMHEIGHT,0,23);

		SendMessage(hListboxClasse,LB_ADDSTRING,0,(LPARAM)enfants[0].classe.c_str());
		SendMessage(hListboxClasse,LB_SETITEMDATA,0,0);
		SendMessage(hListboxClasse,LB_SETITEMHEIGHT,0,23);

		GetWindowRect(hListboxNom,&rect);
		SetWindowPos(hListboxNom,0,0,0,rect.right-rect.left,rect.bottom-rect.top+25,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

		GetWindowRect(hListboxClasse,&rect);
		SetWindowPos(hListboxClasse,0,0,0,rect.right-rect.left,rect.bottom-rect.top+25,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

		GetWindowRect(hButtonAdd,&rect);
		pt.x=rect.left;
		pt.y=rect.top;
		ScreenToClient(hwnd,&pt);
		SetWindowPos(hButtonAdd,0,pt.x,pt.y+23,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		if(enfants.size()<nbLigne)
		EnableWindow(hScroll,false);
		else
		EnableWindow(hScroll,true);
		for(int i=1;i<enfants.size();i++)
		{
		    if(i>=nbLigne-1)
		    {
			    break;
			}
		    RECT rectItem;
		    POINT pt={0,0};
		    GetWindowRect(hComboV1[0],&rectItem);
	 		pt.x=rectItem.left;
		    ScreenToClient(hwnd,&pt);

		    hComboV1.push_back(CreateWindowEx(0,"COMBOBOX","Activite",Styles,pt.x,30+20+23*i,rectItem.right-rectItem.left,20,hwnd,(HMENU)ID_COMBOV1,0,0));
			//hCombo[i]=CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST ,10+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*j-1,10+20+23*i,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,0,0,0);
			SendMessage(hComboV1[i],CB_ADDSTRING,(WPARAM) 0,(Button_GetCheck(hCheckboxCasesVide)==BST_CHECKED) ? (LPARAM)"Pas de choix" : (LPARAM)" ");
			for(int j=0;j<activite.size();j++)
			{
			    SendMessage(hComboV1[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
			}
			SendMessage(hwnd, MSG_SUBCLASSCONTROL,MAKEWPARAM(SUB_COMBOVOEU,0), (LPARAM)hComboV1.back());
			//SendMessage(hCombo[i],CB_SETCURSEL,0,0);
			SendMessage(hComboV1[i],CB_SETCURSEL,SendMessage(hComboV1[i],CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)activite[enfants[i].voeux[0].voeu1].nom.c_str()),0);

			GetWindowRect(hComboV2[0],&rectItem);
	 		pt.x=rectItem.left;
		    ScreenToClient(hwnd,&pt);

			hComboV2.push_back(CreateWindowEx(0,"COMBOBOX","Activite",Styles,pt.x,30+20+23*i,rectItem.right-rectItem.left,20,hwnd,(HMENU)ID_COMBOV2,0,0));
			//hCombo[i]=CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST ,10+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*j-1,10+20+23*i,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,0,0,0);
			SendMessage(hComboV2[i],CB_ADDSTRING,(WPARAM) 0,(Button_GetCheck(hCheckboxCasesVide)==BST_CHECKED) ? (LPARAM)"Pas de choix" : (LPARAM)" ");
			for(int j=0;j<activite.size();j++)
			{
			    SendMessage(hComboV2[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
			}
			SendMessage(hwnd, MSG_SUBCLASSCONTROL,MAKEWPARAM(SUB_COMBOVOEU,0), (LPARAM)hComboV2.back());
			SendMessage(hComboV2[i],CB_SETCURSEL,SendMessage(hComboV2[i],CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)activite[enfants[i].voeux[0].voeu2].nom.c_str()),0);

			GetWindowRect(hComboV3[0],&rectItem);
	 		pt.x=rectItem.left;
		    ScreenToClient(hwnd,&pt);

			hComboV3.push_back(CreateWindowEx(0,"COMBOBOX","Activite",Styles,pt.x,30+20+23*i,rectItem.right-rectItem.left,20,hwnd,(HMENU)ID_COMBOV3,0,0));
			SendMessage(hComboV3[i],CB_ADDSTRING,(WPARAM) 0,(Button_GetCheck(hCheckboxCasesVide)==BST_CHECKED) ? (LPARAM)"Pas de choix" : (LPARAM)" ");
			for(int j=0;j<activite.size();j++)
			{
			    SendMessage(hComboV3[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
			}
			SendMessage(hwnd, MSG_SUBCLASSCONTROL,MAKEWPARAM(SUB_COMBOVOEU,0), (LPARAM)hComboV3.back());
			SendMessage(hComboV3[i],CB_SETCURSEL,SendMessage(hComboV3[i],CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)activite[enfants[i].voeux[0].voeu3].nom.c_str()),0);

			SendMessage(hListboxNom,LB_ADDSTRING,0,(LPARAM)enfants[i].nom.c_str());
			SendMessage(hListboxNom,LB_SETITEMDATA,i, i);
			SendMessage(hListboxNom,LB_SETITEMHEIGHT,i,23);

			SendMessage(hListboxClasse,LB_ADDSTRING,0,(LPARAM)enfants[i].classe.c_str());
			SendMessage(hListboxClasse,LB_SETITEMDATA,enfants.size(),enfants.size());
			SendMessage(hListboxClasse,LB_SETITEMHEIGHT,enfants.size(),23);

			RECT rect;
			GetWindowRect(hListboxNom,&rect);
			SetWindowPos(hListboxNom,0,0,0,rect.right-rect.left,rect.bottom-rect.top+25,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

			GetWindowRect(hListboxClasse,&rect);
			SetWindowPos(hListboxClasse,0,0,0,rect.right-rect.left,rect.bottom-rect.top+25,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
			if(i<nbLigne-2)
			{
				GetWindowRect(hButtonAdd,&rect);
				pt.x=rect.left;
				pt.y=rect.top;
				ScreenToClient(hwnd,&pt);
				SetWindowPos(hButtonAdd,0,pt.x,pt.y+23,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}
		if(enfants.size()>nbLigne-1)
		{
	 	    for(int i=nbLigne-1;i<enfants.size();i++)
	 	    {
			    SendMessage(hListboxNom,LB_ADDSTRING,0,(LPARAM)enfants[i].nom.c_str());
				SendMessage(hListboxNom,LB_SETITEMDATA,i,i);
				SendMessage(hListboxNom,LB_SETITEMHEIGHT,i,23);

				SendMessage(hListboxClasse,LB_ADDSTRING,0,(LPARAM)enfants[i].classe.c_str());
				SendMessage(hListboxClasse,LB_SETITEMDATA,i,i);
				SendMessage(hListboxClasse,LB_SETITEMHEIGHT,i,23);
			}
			SCROLLINFO infoScroll;
			infoScroll.cbSize=sizeof(SCROLLINFO);

			//GetScrollInfo(hScroll,SB_CTL,infoScroll);

			infoScroll.fMask=SIF_ALL;
			infoScroll.nMin=0;
			infoScroll.nMax=enfants.size();//rectHwnd.bottom-83;
			infoScroll.nPage=nbLigne;//infoScroll.nPage*nbLigne-1/nbLigne;//rectHwnd.bottom-83-22*(nbItem-nbLigne);
			infoScroll.nPos=0;
			SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);
			EnableWindow(hScroll,true);
		}
	}
	string nbEnfants;
	int nbEnfantJour=0;
	for(int j=0;j<enfants.size();j++)
	{
	    if(enfants[j].nom!="")
	    {
		    nbEnfantJour++;
		}
	}
	nbEnfants=to_string(nbEnfantJour);
	string lineStatu="Nombre d'enfants en tout: ";
	lineStatu+=nbEnfants;
	SendMessage(hstatu,SB_SETTEXT,MAKELONG(0,0),(LPARAM)const_cast<char*>(lineStatu.c_str()));
	lineStatu="Nombre d'enfants présents le ";
	string nmJour[4]={"lundi: ","mardi: ","jeudi: ","vendredi: "};
	nbEnfantJour=0;
	for(int jour=0;jour<4;jour++)
	{
	    for(int i=0;i<enfants.size();i++)
	    {
	        if((enfants[i].voeux[jour].voeu1!=-1 || enfants[i].voeux[jour].voeu2!=-1 || enfants[i].voeux[jour].voeu3!=-1) && enfants[i].nom!="")
	        {
  			    nbEnfantJour++;
  			}
	    }
	    lineStatu="Nombre d'enfants présents le ";
	    lineStatu+=nmJour[jour];
	    nbEnfants=to_string(nbEnfantJour);
	    lineStatu+=nbEnfants;
	    nbEnfantJour=0;

	    SendMessage(hstatu,SB_SETTEXT,MAKELONG(jour+1,0),(LPARAM)const_cast<char*>(lineStatu.c_str()));
	}
	string titre="Gestion des groupes - ";
	titre+=pathFileOpen;
	//attributs
	WIN32_FILE_ATTRIBUTE_DATA InfoFichier;
    ZeroMemory (&InfoFichier, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
    if (GetFileAttributesEx( pathFileOpen.c_str(), GetFileExInfoStandard, &InfoFichier ))
    {
        if(InfoFichier.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
        {
            titre+=" (lecture seule)";
        }

    }
	SetWindowText(hwnd,titre.c_str());

	iItemSel=0;
    ComboBox_ResetContent(hComboActi);
	int iItemAdd;
    for(int i=0;i<activite.size();i++)
	{
	    iItemAdd=SendMessage(hComboActi,CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[i].nom.c_str());
	    ComboBox_SetItemData(hComboActi,iItemAdd,(LPARAM)i);
	}

	ComboBox_SetCurSel(hComboActi,0);

	SendMessage(hListboxNom,LB_SETCURSEL,0,0);
	SendMessage(hListboxClasse,LB_SETCURSEL,0,0);
}

void DLLEXPORT RemplirListeClasse(vector<enfantActi> const &enfants, vector<InfoActivite> const &activite, vector<string> &nomClasses)
{
    size_t found=0;
    size_t foundNew=0;
    bool classeNonPresente=true;
    string str;
    //recherche dans les activités
    for(int i=0;i<activite.size();i++)
    {
	    found=0;
	    foundNew=0;
		foundNew=activite[i].classe.find('\t',found);
		while(foundNew!=std::string::npos)
		{
			/*if(foundNew==std::string::npos)
			{
			 continue;
			}*/
			str=activite[i].classe.substr(found,foundNew-found);
			//str+='\0';
			for(int j=0;j<nomClasses.size();j++)
			{
				if(nomClasses[j].compare(str)==0)
				{
				    classeNonPresente=false;
				    break;
				}
			}
			if(classeNonPresente)
			{
		 	    nomClasses.push_back(str);
			}
			classeNonPresente=true;
			found=foundNew+1;
			foundNew=activite[i].classe.find('\t',found);
		}
	}
	//recherche dans les élèves
	classeNonPresente=true;
	for(int jour=0;jour<4;jour++)
	{
	    for(int i=0;i<enfants.size();i++)
	    {
			for(int j=0;j<nomClasses.size();j++)
			{
 			    if(nomClasses[j].compare(enfants[i].classe)==0)
			 	{
     			    classeNonPresente=false;
			        break;
			 	}
			}
			if(classeNonPresente)
			{
 			    nomClasses.push_back(string(enfants[i].classe));
			}
			classeNonPresente=true;
		}//fin for(int i=0;i<enfants[jour];i++)
	}//fin for(int jour=0;jour<4;jour++)
}

bool DLLEXPORT CreateToolTip(HWND hwnd, HWND &hwndToolTips, HINSTANCE const &hinstance)
{
    hwndToolTips = CreateWindow(TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON | TTS_CLOSE ,
				 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, hinstance, NULL);

    int x,y;
    RECT rect;
    GetWindowRect(hwnd,&rect);
    x=rect.left+20;
    y=rect.left+40;

    TOOLINFO toolInfo = { 0 };
    toolInfo.cbSize = sizeof(toolInfo);
    toolInfo.hwnd = hwnd;
    toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRACK;
    toolInfo.uId = (UINT_PTR)hwnd;
    toolInfo.lpszText = (LPSTR)"Enregistrez cette feuille si vous êtes sur qu'elle convient.\n\
Enregistrez-là grâce à \"Enregistrer sous...\".";
    SendMessage(hwndToolTips, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

    SendMessage(hwndToolTips,TTM_SETTITLEA,1,(LPARAM)"Enregistrer la feuille");

    SendMessage(hwndToolTips,TTM_TRACKPOSITION,0,MAKELPARAM(x,y));
    SendMessage(hwndToolTips, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&toolInfo);
    return 0;
}
bool DLLEXPORT UpdateStatusBar(HWND const &hstatu, vector<enfantActi> const &enfants, HWND const &hWndMain)
{
    int nWidth,cParts=5;
    int paParts[5];
    RECT rectHwnd;
    GetClientRect(hWndMain,&rectHwnd);
    rectHwnd.bottom-=20;
    nWidth = rectHwnd.right / cParts;
    int rightEdge = nWidth;
    for (int i=0;i<cParts;i++)
	{
       paParts[i] = rightEdge;
       rightEdge += nWidth;
    }

    // Tell the status bar to create the window parts.
    SendMessage(hstatu, SB_SETPARTS, (WPARAM) cParts, (LPARAM)paParts);
    //status bar
	string nbEnfants;
	int nbEnfantJour=0;
	for(int j=0;j<enfants.size();j++)
	{
	    if(enfants[j].nom!="")
	    {
		    nbEnfantJour++;
		}
	}
	nbEnfants=to_string(nbEnfantJour);
	string lineStatu="Nombre d'enfants en tout: ";
	lineStatu+=nbEnfants;
	SendMessage(hstatu,SB_SETTEXT,MAKELONG(0,0),(LPARAM)lineStatu.c_str());
	lineStatu="Nombre d'enfants présents le ";
	string nmJour[4]={"lundi: ","mardi: ","jeudi: ","vendredi: "};
	nbEnfantJour=0;
	for(int jour=0;jour<4;jour++)
	{
	    for(int i=0;i<enfants.size();i++)
	    {
	        if((enfants[i].voeux[jour].voeu1!=-1 || enfants[i].voeux[jour].voeu2!=-1 || enfants[i].voeux[jour].voeu3!=-1)&& enfants[i].nom!="")
	        {
  			    nbEnfantJour++;
  			}
	    }
	    lineStatu="Nombre d'enfants présents le ";
	    lineStatu+=nmJour[jour];
	    nbEnfants=to_string(nbEnfantJour);
	    lineStatu+=nbEnfants;
	    nbEnfantJour=0;
	    SendMessage(hstatu,SB_SETTEXT,MAKELONG(jour+1,0),(LPARAM)lineStatu.c_str());
	}
	//fin status bar
	return true;
}

bool DLLEXPORT UpdateStatusBar(HWND const &hstatu, HWND const &hListboxNom)
{
    int count=SendMessage(hListboxNom,LB_GETSELCOUNT,0,0);
    int *list=new int[count+1];
    SendMessage(hListboxNom,LB_GETSELITEMS,count,(LPARAM)list);

    vector<int> myList(list,list+count);
    sort(myList.begin(),myList.end());

    string a;
    a=to_string(count);
    string sSel="Sélectionnés: "+a;
    a=to_string(myList[0]+1);
    string sRange="De "+a;
    a=to_string(myList[myList.size()-1]+1);
    sRange+=" à "+a;

    delete[] list;

    const int cParts=2;
    int paParts[2];
    // Calculate the right edge coordinate for each part, and
    // copy the coordinates to the array.
    paParts[0]=100;
    paParts[1]=200;

    // Tell the status bar to create the window parts.
    SendMessage(hstatu, SB_SETPARTS, (WPARAM) cParts, (LPARAM)paParts);

    SendMessage(hstatu,SB_SETTEXT,MAKELONG(0,0),(LPARAM)sSel.c_str());
    SendMessage(hstatu,SB_SETTEXT,MAKELONG(1,SBT_POPOUT),(LPARAM)sRange.c_str());
}

void DLLEXPORT VoirErreur(unsigned long error)//VoirErreur(GetLastError());
{
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER| FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);
    MessageBox(NULL,(LPCTSTR )lpMsgBuf, 0, MB_OK | MB_ICONERROR);
    LocalFree(lpMsgBuf);
}

void DLLEXPORT VoirErreur(unsigned long error,const char *titre)//VoirErreur(GetLastError(),titre);
{
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER| FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);
    MessageBox(NULL,(LPCTSTR )lpMsgBuf, titre, MB_OK | MB_ICONERROR);
    LocalFree(lpMsgBuf);
}

int DLLEXPORT stricmp(string a, string b)
{
    string outa;
    string outb;

    transform(a.begin(), a.end(), outa.begin(), ::tolower);
    transform(b.begin(), b.end(), outb.begin(), ::tolower);

    return outa.compare(outb);
}
