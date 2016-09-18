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
#include "resource.h"
#include <fstream>
//#include <iostream>
//#include <stdio.h>
#include "Shlobj.h"
//#include <ctime>
#include <algorithm>
#include <Windowsx.h>
//#include <commctrl.h>
#include <fcntl.h>
//#include <cmath>
#include <sstream>
//STL
#include <string>
#include <vector>

using namespace std;
#include "typesData.h"
#include "Exeptions.h"

//DLLs.h
#include "dllTraitement.h"
#include "dllCommune.h"
#include "dllIO.h"

template<typename T> string to_string(T const &var)
{
    ostringstream oss;
    oss<<var;
    return oss.str();
}

//#include <Commctrl.h>

UINT nbLigne=32;
UINT intervalTimer=60000;//60000
HBRUSH hbrBkgnd= CreateSolidBrush(RGB(235,232,216));
const SHORT Mask = 32768;
//ofstream fLog("log.txt",ios::trunc);


HINSTANCE hinstance;
POINT Ecran;
POINT tailleFenetre;
static HWND hListboxEnCours;
static HWND hWndMain;
static HWND hwndToolTips;
static HWND hDlgTraitementEnCours=NULL;

static HWND hlunch;
static HWND hListviewL;
static HWND hHeader;
static HWND hstatu;
static HWND hListboxNom;
static HWND hListboxClasse;
static HWND hEditList;
static HWND hButtonAdd;
static HWND hScroll;
static HWND hTabs;

static HWND hGroupActi;
static HWND hSaticNomActi;
static HWND hComboActi;
static HWND hButtonNewActi;
static HWND hStaticNbPlaces;
static HWND hEditUpDown;
static HWND hUpDown;
static HWND hStaticJour;
static HWND hComboJour;
static HWND hButtonClasse;
static HWND hButtonRecActi;

static HWND hGroupFind;
static HWND hStaticFindLine;
static HWND hEditFindLine;
static HWND hButtonGoLine;
static HWND hStaticFindName;
static HWND hEditFindName;
static HWND hButtonFindName;
static HWND hButtonFindNext;

static HWND hCheckboxCasesVide;
static HWND hCheckboxBackUp;
static HWND hStaticAjoutClasse;
static HWND hComboAjoutClasse;

vector<HWNDcontrol> hControl;

vector<HWND> hComboV1;
vector<HWND> hComboV2;
vector<HWND> hComboV3;
vector<HWND> hCheckBoxClasses;
vector<string> nomClasses;

UINT cxyAvant;
UINT iItemEdit;
UINT iItemSel;
UINT tempsEcoule=0;
int iActiEnCour=0;
int iItemModifActi;
char inferieurA=1;
unsigned char jourEnCourSaisie=0;
char backUpActivite=-1;
bool nouvActi=false;
bool nouvJour=false;
bool traitementLance=false;
bool arretTraitement=false;
bool destroyEdit=false;
bool editActiActivate=false;
bool lectureSeule=false;
bool isFileSaved=true;
string pathFileOpen;
string actiFileOpen;


vector< enfantActi > enfants;
vector< enfantActi > enfantsAttributed;

vector<InfoActivite> activite;

//HINSTANCE hinst;
/*  Declare Windows procedure  */


void RemplacerCasesVides(int index, char *sRemplacer, char *sRemplacement)
{
    if(ComboBox_FindStringExact(hComboV1[index],-1,sRemplacer)!=CB_ERR)
	{
	    SendMessage(hComboV1[index],CB_DELETESTRING,(WPARAM) ComboBox_FindStringExact(hComboV1[index],-1,sRemplacer),0);
	    SendMessage(hComboV1[index],CB_ADDSTRING,(WPARAM) 0,(LPARAM)sRemplacement);
	}
	if(ComboBox_FindStringExact(hComboV2[index],-1,sRemplacer)!=CB_ERR)
	{
	    SendMessage(hComboV2[index],CB_DELETESTRING,(WPARAM) ComboBox_FindStringExact(hComboV2[index],-1,sRemplacer),0);
	    SendMessage(hComboV2[index],CB_ADDSTRING,(WPARAM) 0,(LPARAM)sRemplacement);
	}
	if(ComboBox_FindStringExact(hComboV3[index],-1,sRemplacer)!=CB_ERR)
	{
	    SendMessage(hComboV3[index],CB_DELETESTRING,(WPARAM) ComboBox_FindStringExact(hComboV3[index],-1,sRemplacer),0);
	    SendMessage(hComboV3[index],CB_ADDSTRING,(WPARAM) 0,(LPARAM)sRemplacement);
	}
}
void AjoutTabStop()
{
    LONG lStyle = GetWindowLongPtr(hButtonAdd, GWL_STYLE);
    lStyle |= WS_TABSTOP;
    SetWindowLongPtr(hButtonAdd, GWL_STYLE, lStyle);

    if(hComboV1.size()!=0)
    {
        lStyle = GetWindowLongPtr(hComboV1[0], GWL_STYLE);
 	    lStyle |=WS_TABSTOP;
	}
    for(int i=0;i<hComboV1.size();i++)
    {
	 	SetWindowLongPtr(hComboV1[i], GWL_STYLE, lStyle);
	 	SetWindowLongPtr(hComboV2[i], GWL_STYLE, lStyle);
	 	SetWindowLongPtr(hComboV3[i], GWL_STYLE, lStyle);
	}
}


//---------------------------------------------------------------------------
LRESULT CALLBACK new_subclassed_listbox_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC previous_boring_subclassed_listbox_proc;//global variable to save a copy of the default control procedure
void subclass_the_listbox(HWND h_list)
{
    previous_boring_subclassed_listbox_proc = (WNDPROC)GetWindowLongPtr(h_list, GWL_WNDPROC);//get the default procedure
    SetWindowLongPtr(h_list, GWLP_WNDPROC, (LONG_PTR)new_subclassed_listbox_proc);//set your custom procedure :)
}
//---------------------------------------------------------------------------
LRESULT CALLBACK new_subclassed_edit_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC previous_boring_subclassed_edit_proc;//global variable to save a copy of the default control procedure
void subclass_the_edit(HWND h_edit)
{
    previous_boring_subclassed_edit_proc = (WNDPROC)GetWindowLongPtr(h_edit, GWL_WNDPROC);//get the default procedure
    SetWindowLongPtr(h_edit, GWLP_WNDPROC, (LONG_PTR)new_subclassed_edit_proc);//set your custom procedure :)

    //enlever le style WS_TABSTOP aux combos et au boutton
    SetWindowLongPtr(hButtonAdd, GWL_STYLE, GetWindowLongPtr(hButtonAdd, GWL_STYLE)&~WS_TABSTOP);
    //SetWindowPos(hButtonAdd,NULL, 0,0,0,0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    LONG lStyle;
    if(hComboV1.size()!=0)
    {
        lStyle = GetWindowLongPtr(hComboV1[0], GWL_STYLE);
 	    lStyle = lStyle&~WS_TABSTOP;
	}
    for(int i=0;i<hComboV1.size();i++)
    {
	 	SetWindowLongPtr(hComboV1[i], GWL_STYLE, lStyle);
	 	SetWindowLongPtr(hComboV2[i], GWL_STYLE, lStyle);
	 	SetWindowLongPtr(hComboV3[i], GWL_STYLE, lStyle);
	}
}
//---------------------------------------------------------------------------
LRESULT CALLBACK new_subclassed_comboActiEdit(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC previous_boring_subclassed_comboActiEdit;//global variable to save a copy of the default control procedure
void subclass_the_comboActiEdit(HWND h_edit)
{
    previous_boring_subclassed_comboActiEdit = (WNDPROC)GetWindowLongPtr(h_edit, GWL_WNDPROC);//get the default procedure
    SetWindowLongPtr(h_edit, GWLP_WNDPROC, (LONG_PTR)new_subclassed_comboActiEdit);//set your custom procedure :)
}
//---------------------------------------------------------------------------
LRESULT CALLBACK new_subclassed_comboBoxActi(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC previous_boring_subclassed_comboBoxActi;//global variable to save a copy of the default control procedure
void subclass_the_comboBoxActi(HWND h_combo)
{
    previous_boring_subclassed_comboBoxActi = (WNDPROC)GetWindowLongPtr(h_combo, GWL_WNDPROC);//get the default procedure
    SetWindowLongPtr(h_combo, GWLP_WNDPROC, (LONG_PTR)new_subclassed_comboBoxActi);//set your custom procedure :)
}
//---------------------------------------------------------------------------
LRESULT CALLBACK new_subclassed_editGoLine_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC previous_boring_subclassed_editGoLine_proc;//global variable to save a copy of the default control procedure
void subclass_the_editGoLine(HWND h_edit)
{
    previous_boring_subclassed_editGoLine_proc = (WNDPROC)GetWindowLongPtr(h_edit, GWL_WNDPROC);//get the default procedure
    SetWindowLongPtr(h_edit, GWLP_WNDPROC, (LONG_PTR)new_subclassed_editGoLine_proc);//set your custom procedure :)
}
//---------------------------------------------------------------------------
LRESULT CALLBACK new_subclassed_editFindName_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC previous_boring_subclassed_editFindName_proc;//global variable to save a copy of the default control procedure
void subclass_the_editFindName(HWND h_edit)
{
    previous_boring_subclassed_editFindName_proc = (WNDPROC)GetWindowLongPtr(h_edit, GWL_WNDPROC);//get the default procedure
    SetWindowLongPtr(h_edit, GWLP_WNDPROC, (LONG_PTR)new_subclassed_editFindName_proc);//set your custom procedure :)
}
//---------------------------------------------------------------------------
LRESULT CALLBACK new_subclassed_editComboAjoutClasse_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC previous_boring_subclassed_editComboAjoutClasse_proc;//global variable to save a copy of the default control procedure
void subclass_the_editComboAjoutClasse(HWND h_edit)
{
    previous_boring_subclassed_editComboAjoutClasse_proc = (WNDPROC)GetWindowLongPtr(h_edit, GWL_WNDPROC);//get the default procedure
    SetWindowLongPtr(h_edit, GWLP_WNDPROC, (LONG_PTR)new_subclassed_editComboAjoutClasse_proc);//set your custom procedure :)
}
//---------------------------------------------------------------------------
LRESULT CALLBACK new_subclassed_ComboVoeu_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC previous_boring_subclassed_ComboVoeu_proc;//global variable to save a copy of the default control procedure
void subclass_the_ComboVoeu(HWND h_combo)
{
    previous_boring_subclassed_ComboVoeu_proc = (WNDPROC)GetWindowLongPtr(h_combo, GWL_WNDPROC);//get the default procedure
    SetWindowLongPtr(h_combo, GWLP_WNDPROC, (LONG_PTR)new_subclassed_ComboVoeu_proc);//set your custom procedure :)
}
//---------------------------------------------------------------------------

/********************************************************déclarations des fonctions********************************************************/

/************************************************************fonctions externes************************************************************/

//void Traitement(vector<enfantActi> &enfants, vector<InfoActivite> &activite);
/*int OuvertureFichier(HWND hwnd, HWND hTabs, HWND hwndToolTips,
                     HINSTANCE const &hinstance,
                     vector<enfantActi> &enfants, vector<InfoActivite> &activite, vector<string> &nomClasses,
                     string &pathFileOpen);*/

/************************************************************fonctions internes************************************************************/

int OuvrirFeuille(HWND hwnd);
int OuvrirActivite(HWND hwnd);
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI OrganisationDesDonnees(LPVOID lpParameter);
DWORD WINAPI SauvegardeBackUp(LPVOID lpParameter);
long GetHeigthTray(BOOL VIEWERROR);
int SortieMain();
string GetClipboardText();
int SetClipboardText(char const *text);
void AssociationExtProg(const char *ext, const char *path);
/*void AfficheNouveauTab(HWND hwnd);
void RemplirListeClasse();
bool CreateToolTip(HWND hwnd);*/
bool ActualiserTabCombo(int top);

BOOL APIENTRY DialogSortieProc(HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY DialogBadaffectProc(HWND,UINT, WPARAM, LPARAM);
BOOL APIENTRY DialogVerifRempliProc(HWND,UINT, WPARAM, LPARAM);
BOOL APIENTRY DialogFindNameProc(HWND,UINT, WPARAM, LPARAM);
BOOL APIENTRY DialogGoLineProc(HWND,UINT, WPARAM, LPARAM);
BOOL APIENTRY DialogVerifPlacesProc(HWND,UINT, WPARAM, LPARAM);
BOOL APIENTRY DialogSelectClasseProc(HWND,UINT, WPARAM, LPARAM);
BOOL APIENTRY DialogTraitementEnCours(HWND,UINT, WPARAM, LPARAM);
BOOL APIENTRY DialogVerifActi(HWND,UINT, WPARAM, LPARAM);
BOOL APIENTRY DialogModifActi(HWND,UINT, WPARAM, LPARAM);
BOOL APIENTRY DialogCountAppliProc(HWND,UINT, WPARAM, LPARAM);
BOOL APIENTRY DialogAboutProc(HWND,UINT, WPARAM, LPARAM);

BOOL CALLBACK EnumChildProc(HWND hwnd,LPARAM lParam);
BOOL CALLBACK EnumChildStartPosProc(HWND hwnd,LPARAM lParam);

/*  Make the class name into a global variable  */
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hprev, LPSTR cmd, int show)
{
 	if(strcmp(cmd,"")!=0)
 	{
  	    for(unsigned int i=1;i<strlen(cmd)-1;i++)
  	    {
	        pathFileOpen+=cmd[i];
		}
 	    //strcpy(pathFileOpen,cmd);
	}

	///Manipulation du registre
    /*
	//regarde si l'extension .grma est associée à GroupManage
	{
    	HKEY key;
        DWORD kSize=MAX_PATH;
        char mem[MAX_PATH] = "";
        RegOpenKeyEx(HKEY_CLASSES_ROOT, ".grma", 0, KEY_ALL_ACCESS, &key);
        RegQueryValueEx(key, NULL, 0, NULL, (BYTE*)mem, &kSize);
        strcat(mem, "\\Shell\\Open\\command");
        RegOpenKeyEx(HKEY_CLASSES_ROOT, mem, 0, KEY_ALL_ACCESS, &key);
        kSize=MAX_PATH;
        RegQueryValueEx(key, NULL, 0, NULL, (BYTE*)mem, &kSize);
        RegCloseKey(key);


    	//Si non, manipulation du registre
    	char buffer[MAX_PATH+5];
    	GetModuleFileName(0, buffer, sizeof(buffer));
    	char chaine[MAX_PATH+7]="\"";
    	strcat(chaine,buffer);
    	strcat(chaine,"\" \"%1\"");

    	string str1=string(chaine);
    	string str2=string(mem);
    	if(str1.compare(str2)!=0)//strcmp(mem,chaine)!=0);
    	{
    		AssociationExtProg(".grma",buffer);
    	}
    }
	//regarde si l'extension .grma2 est associée à GroupManage
	{
    	HKEY key;
        DWORD kSize=MAX_PATH;
        char mem[MAX_PATH] = "";
        RegOpenKeyEx(HKEY_CLASSES_ROOT, ".grma2", 0, KEY_ALL_ACCESS, &key);
        RegQueryValueEx(key, NULL, 0, NULL, (BYTE*)mem, &kSize);
        strcat(mem, "\\Shell\\Open\\command");
        RegOpenKeyEx(HKEY_CLASSES_ROOT, mem, 0, KEY_ALL_ACCESS, &key);
        kSize=MAX_PATH;
        RegQueryValueEx(key, NULL, 0, NULL, (BYTE*)mem, &kSize);
        RegCloseKey(key);

    	//Si non, manipulation du registre
    	char buffer[MAX_PATH+5];
    	GetModuleFileName(0, buffer, sizeof(buffer));
    	char chaine[MAX_PATH+7]="\"";
    	strcat(chaine,buffer);
    	strcat(chaine,"\" \"%1\"");

    	string str1=string(chaine);
    	string str2=string(mem);
    	if(str1.compare(str2)!=0)
    	{
    		AssociationExtProg(".grma2",buffer);
    	}
    }
    //choisi l'icone pour les fichiers

    map<string,int> mapAssociationIcon;
    mapAssociationIcon[".grma"]=1;
    mapAssociationIcon[".grma2"]=1;
    mapAssociationIcon[".eac"]=2;

    for (map<string,int>::iterator it=mapAssociationIcon.begin(); it!=mapAssociationIcon.end(); ++it)
    {
        HKEY key;
        DWORD kSize=MAX_PATH;
        char mem[MAX_PATH] = "";

        RegOpenKeyEx(HKEY_CLASSES_ROOT, it->first.c_str(), 0, KEY_ALL_ACCESS, &key);
        RegQueryValueEx(key, NULL, 0, NULL, (BYTE*)mem, &kSize);

        strcat(mem, "\\DefaultIcon");
        RegCreateKeyEx( HKEY_CLASSES_ROOT, mem, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &key, NULL );

        //récupération du chemin de l'exécutable
        char pathExecutable[MAX_PATH+2]="";
        GetModuleFileName(NULL, pathExecutable, sizeof(pathExecutable));

        string pathIconInExe=string(pathExecutable)+",";
        pathIconInExe+=to_string(it->second);
        RegSetValueEx(key, "", 0, REG_SZ, (LPBYTE)pathIconInExe.c_str(), (DWORD)pathIconInExe.size());
        RegCloseKey(key);
    }*/

    ///Création de l'hinstance de fenêtre
	//MessageBox(0,pathFileOpen,0,0);
    HACCEL haccel;
 	::Ecran.x  = GetSystemMetrics(SM_CXSCREEN);
	::Ecran.y  = GetSystemMetrics(SM_CYSCREEN);
	// Déclaration et initialisation de la structure WNDCLASSEX:
 	hinstance=hinst;
	WNDCLASS wc;//WNDCLASSEX
	ZeroMemory(&wc,sizeof(WNDCLASSEX));
	//wc.cbSize=sizeof(WNDCLASSEX);
	wc.hInstance=hinst;
	wc.lpszMenuName = "ID_MENU";
	wc.lpszClassName="fenetre";
	wc.lpfnWndProc=WinProc;
	wc.hCursor=LoadCursor(0,IDC_ARROW);
	wc.hIcon=LoadIcon(wc.hInstance,MAKEINTRESOURCE(IDI_ICON));//IDI_APPLICATION
	wc.hbrBackground=CreateSolidBrush(RGB(235,232,216));
	wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.style = 0 ;
	// Enregistrement de notre classe de fenêtre:
	RegisterClass(&wc);//RegisterClassEx(&wc);
	// Création de notre fenêtre:
	DWORD styles=WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE | WS_MAXIMIZEBOX;//| WS_MAXIMIZEBOX | WS_SIZEBOX | WS_HSCROLL | WS_VSCROLL;

	hWndMain=CreateWindowEx(WS_EX_APPWINDOW,"fenetre","Gestion des groupes",styles, -3,-3,Ecran.x+6,Ecran.y-GetHeigthTray(false)+6,NULL,NULL,hinst,NULL);//500

	/*RECT rect;
	GetClientRect(hWndMain,&rect);
	::tailleFenetre.x=rect.right;
	::tailleFenetre.y=rect.bottom;
	//Scroll Bar vertical de la fenêtre
	{
		SCROLLINFO infoScroll;
		infoScroll.cbSize=sizeof(SCROLLINFO);

		//GetScrollInfo(hScroll,SB_CTL,infoScroll);

		infoScroll.fMask=SIF_ALL;
		infoScroll.nMin=1;
		infoScroll.nMax=1;
		infoScroll.nPage=1;
		infoScroll.nPos=1;
		SetScrollInfo(hWndMain,SB_VERT,&infoScroll,true);
	}
	//Scroll Bar horizontal de la fenêtre
	{
		SCROLLINFO infoScroll;
		infoScroll.cbSize=sizeof(SCROLLINFO);

		//GetScrollInfo(hScroll,SB_CTL,infoScroll);

		infoScroll.fMask=SIF_ALL;
		infoScroll.nMin=1;
		infoScroll.nMax=1;
		infoScroll.nPage=1;
		infoScroll.nPos=1;
		SetScrollInfo(hWndMain,SB_HORZ,&infoScroll,true);
	}*/

	/*RECT rectMain;
	GetWindowRect(hWndMain,&rectMain);
	SetWindowPos(hWndMain,0,0,0,rectMain.right,rectMain.bottom-GetHeigthTray(false),SWP_NOMOVE | SWP_NOZORDER);*/
	ShowWindow(hWndMain,SW_SHOWMAXIMIZED);
	//UpdateWindow(hWndMain);
	/*HMENU menu = LoadMenu(hinst, "ID_MENU");//MAKEINTRESOURCE(ID_MENU));
    SetMenu(hwnd, menu);*/
    haccel = LoadAccelerators(hinstance, "LesAccel");
	// Boucle des messages:
	MSG msg;
	try
	{
        while(GetMessage(&msg,0,0,0))
        {
            //if(!IsDialogMessage(hwnd,&msg))
            if (!TranslateAccelerator(hWndMain, haccel, &msg) && IsDialogMessage(hWndMain, &msg) == 0)
            {
                DispatchMessage(&msg);
                TranslateMessage(&msg);
            }
        }
	}
	catch(...)
	{
	    return GetLastError();
	}
	// Sortie du programme:
	return 0;
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Déclaration des variables en static:
	//HWND cur=FindWindow("#32770",NULL);
	//char name[100];
	//SetWindowText(cur,"coucou");
	//HWND hwPython=FindWindow("#32770",NULL);
	switch(message)
	{
 	    /*case WM_PAINT:
		{
  		    PAINTSTRUCT ps;
        	HDC hdc ;
			HPEN hpen;
			HBRUSH hpPlein;
			hdc = BeginPaint(hwnd, &ps);
		    //hpen = CreatePen(PS_SOLID, 1, 0x00FF0000);
			//SelectObject(hdc,hpen);
			SetBkMode(hdc, TRANSPARENT);

			RECT rectHwnd;
	 	    GetClientRect(hwnd,&rectHwnd);

			MoveToEx(hdc, 30, 30, NULL);
   			LineTo(hdc, rectHwnd.right*5/8+50, 30);
   			LineTo(hdc, rectHwnd.right*5/8+50, rectHwnd.bottom-53);
   			LineTo(hdc, 30, rectHwnd.bottom-53);
   			LineTo(hdc, 30, 30);
			EndPaint(hwnd, &ps);
			//MessageBox(0,0,0,0);
			return 0;
 		}*/
		case WM_CREATE:
		{
	 	    jourEnCourSaisie=0;
	 	    InitCommonControls();
	 	    RECT rectHwnd;
	 	    GetClientRect(hwnd,&rectHwnd);
	 	    rectHwnd.bottom-=20;

	 	    ::nbLigne=(rectHwnd.bottom-50)/23+1;

	 	    /*char a[5];
	 	    itoa(rectHwnd.right*55/717,a,10);
			MessageBox(0,a,0,0);*/
			//   737 pixels: touts les combos

			hlunch=CreateWindowEx(0,"button", "Lancer le traitement", WS_CHILD |WS_VISIBLE,rectHwnd.right-165,(rectHwnd.bottom-25)*98/100,150,25,hwnd,(HMENU)ID_LUNCH,0,0);

			hTabs = CreateWindowEx(0 , WC_TABCONTROL, "", WS_CHILD | WS_VISIBLE, 30,6,rectHwnd.right*5/8+23, ((rectHwnd.bottom-50)/23)*23+48, hwnd, NULL, hinstance, NULL);
			hListviewL=NULL;//CreateWindowEx(0,WC_LISTVIEW,0,WS_VISIBLE | WS_CHILD |LVS_REPORT |  LVS_EDITLABELS | WS_BORDER | WS_VSCROLL | LVS_ALIGNLEFT ,10, 10, rectHwnd.right*5/8, rectHwnd.bottom-50,hwnd,(HMENU) IDE_LISTAPP, 0, NULL);
			hHeader=CreateWindowEx(0,WC_HEADER,0,WS_VISIBLE | WS_CHILD | WS_BORDER | HDS_HORZ | HDS_BUTTONS,30,30,rectHwnd.right*5/8+2,21,hwnd,(HMENU)ID_HEADER,0,NULL);
			hListboxNom=CreateWindowEx(0,"listbox",0,WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY | LBS_EXTENDEDSEL,30,50,rectHwnd.right*5/24*2/3,0,hwnd,(HMENU)ID_LISTBOX,0,0);
			hListboxClasse=CreateWindowEx(0,"listbox",0,WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOTIFY | LBS_EXTENDEDSEL,30+rectHwnd.right*5/24*2/3,50,rectHwnd.right*5/24*1/3,0,hwnd,(HMENU)ID_LISTBOXCLASSE,0,0);
			hButtonAdd=CreateWindowEx(0,"button", "+", WS_CHILD | WS_VISIBLE | WS_TABSTOP  ,8,50,22,22,hwnd,(HMENU)ID_BUTTONADD,0,0);
			hScroll=CreateWindowEx(0,"SCROLLBAR",0,WS_CHILD | SBS_VERT ,32+rectHwnd.right*5/8,31,20,((rectHwnd.bottom-50)/23)*23+19,hwnd,0,0,0);

			hGroupActi=CreateWindowEx(0,"button","Propriétés des activités",WS_VISIBLE | WS_CHILD | BS_GROUPBOX,60+rectHwnd.right*5/8,rectHwnd.bottom*1/20,rectHwnd.right-(60+rectHwnd.right*5/8)-10,210,hwnd,0,0,0);
			hSaticNomActi=CreateWindowEx(0,"static","Nom de l'activité : ",WS_VISIBLE | WS_CHILD,70+rectHwnd.right*5/8,rectHwnd.bottom*1/20+43,120,25,hwnd,0,0,0);
			hComboActi=CreateWindowEx(0,"COMBOBOX","Activité",WS_VISIBLE | WS_CHILD | CBS_DROPDOWN | CBS_HASSTRINGS | CBS_SORT | CBS_AUTOHSCROLL ,200+rectHwnd.right*5/8,rectHwnd.bottom*1/20+40,rectHwnd.right*100/717,20,hwnd,0,hinstance,0);//(HMENU)ID_COMBOACTI
			hButtonNewActi=CreateWindowEx(0,"button", "Nouvelle activité", WS_CHILD | WS_VISIBLE | BS_TEXT | BS_MULTILINE ,(200+rectHwnd.right*5/8)+(rectHwnd.right*100/717)+15,rectHwnd.bottom*1/20+30,rectHwnd.right-((200+rectHwnd.right*5/8)+(rectHwnd.right*100/717)+15)-12,44,hwnd,(HMENU)ID_BUTTONNEWACTI,0,0);//65 MINIMUM, 110 idéal---> rectHwnd.right*55/717
			hStaticNbPlaces=CreateWindowEx(0,"static","Nombres de places disponibles pour l'activité : ",WS_VISIBLE | WS_CHILD,70+rectHwnd.right*5/8,rectHwnd.bottom*1/20+80,320,40,hwnd,0,0,0);//rectHwnd.bottom*1/8+35
			hEditUpDown=CreateWindowEx(0,"edit",0,WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,400+rectHwnd.right*5/8,rectHwnd.bottom*1/20+80, 50, 20, hwnd, (HMENU) ID_EDITNBPLACES,hinstance, NULL);
			hUpDown=CreateWindowEx(0,UPDOWN_CLASS, NULL, WS_VISIBLE | WS_CHILD| UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS ,0, 0, 0, 0, hwnd, NULL, hinstance, NULL);
			hStaticJour=CreateWindowEx(0,"static","Jour de l'activité :",WS_VISIBLE | WS_CHILD,70+rectHwnd.right*5/8,rectHwnd.bottom*1/20+110,120,25,hwnd,0,0,0);
			hComboJour=CreateWindowEx(0,"COMBOBOX","Jour",WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | CBS_HASSTRINGS | CBS_AUTOHSCROLL ,200+rectHwnd.right*5/8,rectHwnd.bottom*1/20+110,rectHwnd.right*100/717,20,hwnd,0,hinstance,0);
			//hButtonNewJour=CreateWindowEx(0,"button", "Nouveau jour", WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_TEXT | BS_MULTILINE ,(200+rectHwnd.right*5/8)+(rectHwnd.right*100/717)+15,rectHwnd.bottom*1/20+115,rectHwnd.right-((200+rectHwnd.right*5/8)+(rectHwnd.right*100/717)+15)-12,44,hwnd,(HMENU)ID_BUTTONNEWDAY,0,0);
			hButtonClasse=CreateWindowEx(0,"button", "Sélectionner les classes correspondantes", WS_CHILD | WS_VISIBLE | BS_TEXT | BS_MULTILINE ,70+rectHwnd.right*5/8,rectHwnd.bottom*1/20+140,rectHwnd.right-(70+rectHwnd.right*5/8)-12,44,hwnd,(HMENU)ID_BUTTONCLASSES,0,0);
			//hButtonRecActi=CreateWindowEx(0,"button", "Enregistrer l'activité", WS_CHILD | WS_TABSTOP |WS_VISIBLE ,200+rectHwnd.right*5/8,rectHwnd.bottom*1/8+130,150,22,hwnd,(HMENU)ID_BUTTONRECACTI,0,0);

			hGroupFind=CreateWindowEx(0,"button","Recherche",WS_VISIBLE | WS_CHILD | BS_GROUPBOX,60+rectHwnd.right*5/8,rectHwnd.bottom*4/9-50,rectHwnd.right-(60+rectHwnd.right*5/8)-10,170,hwnd,0,0,0);
			hStaticFindLine=CreateWindowEx(0,"static","Aller à la ligne : ",WS_VISIBLE | WS_CHILD,70+rectHwnd.right*5/8,rectHwnd.bottom*4/9-15,150,25,hwnd,0,0,0);
			hEditFindLine=CreateWindowEx(0,"edit","0",WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,175+rectHwnd.right*5/8,rectHwnd.bottom*4/9-17, 70, 20, hwnd, NULL,hinstance, NULL);
			hButtonGoLine=CreateWindowEx(0,"button", "Go", WS_CHILD |WS_VISIBLE ,245+rectHwnd.right*5/8,rectHwnd.bottom*4/9-18,30,22,hwnd,(HMENU)ID_BUTTONGOLINE,0,NULL);
			hStaticFindName=CreateWindowEx(0,"static","Rechercher : ",WS_VISIBLE | WS_CHILD,70+rectHwnd.right*5/8,rectHwnd.bottom*4/9+30,100,25,hwnd,0,0,0);
			hEditFindName=CreateWindowEx(0,"edit",0,WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL ,175+rectHwnd.right*5/8,rectHwnd.bottom*4/9+27, 170, 20, hwnd, NULL,hinstance, NULL);
			hButtonFindName=CreateWindowEx(0,"button", "Rechercher", WS_CHILD |WS_VISIBLE ,345+rectHwnd.right*5/8,rectHwnd.bottom*4/9+26,90,22,hwnd,(HMENU)ID_BUTTONFINDNAME,0,NULL);
			hButtonFindNext=CreateWindowEx(0,"button", "Rechercher suivant", WS_CHILD |WS_VISIBLE ,175+rectHwnd.right*5/8,rectHwnd.bottom*4/9+50,145,22,hwnd,(HMENU)ID_BUTTONFINDNEXT,0,NULL);

			hCheckboxCasesVide=CreateWindowEx(0,"button", "Cases vides remplacées par \"Pas de choix\"", WS_CHILD |WS_VISIBLE | BS_AUTOCHECKBOX ,100+rectHwnd.right*5/8,rectHwnd.bottom*8/13,310,22,hwnd,(HMENU)ID_CHECKBOX,0,NULL);
			hStaticAjoutClasse=CreateWindowEx(0,"static","Ajouter une classe : ",WS_VISIBLE | WS_CHILD,70+rectHwnd.right*5/8,2+rectHwnd.bottom*6/9,150,25,hwnd,0,0,0);
			hComboAjoutClasse=CreateWindowEx(0,"COMBOBOX","Ajouter une classe",WS_VISIBLE | WS_CHILD | CBS_SIMPLE | CBS_HASSTRINGS | CBS_SORT | CBS_AUTOHSCROLL ,200+rectHwnd.right*5/8,rectHwnd.bottom*6/9,rectHwnd.right*100/717,20,hwnd,(HMENU)ID_COMBOAPPCLASS,hinstance,0);

			//hCheckboxBackUp=CreateWindowEx(0,"button", "Faire automatiquement une sauvegarde de sécuritée", WS_CHILD | WS_TABSTOP |WS_VISIBLE | BS_AUTOCHECKBOX ,100+rectHwnd.right*5/8,rectHwnd.bottom*6/9+30,310,22,hwnd,(HMENU)ID_CHECKBOXBACKUP,0,NULL);
			//Button_SetCheck(hCheckboxCasesVide,BST_CHECKED);

			SendMessage(hUpDown,UDM_SETBUDDY,(WPARAM)hEditUpDown,0);
			SendMessage(hUpDown, UDM_SETRANGE, 0, MAKELPARAM(999, 0));

			//fin de l'enregistrement des HWND et des tailles

			subclass_the_editGoLine(hEditFindLine);
			subclass_the_editFindName(hEditFindName);

			{
			    COMBOBOXINFO infoCombo;
			    infoCombo.cbSize=sizeof(COMBOBOXINFO);
			    GetComboBoxInfo(hComboAjoutClasse,&infoCombo);
			    subclass_the_editComboAjoutClasse(infoCombo.hwndItem);
			}
			/*hwndToolTips = CreateWindow(TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
			TOOLINFO ti;
		    ti.cbSize   = sizeof(ti);
		    ti.uFlags   = TTF_CENTERTIP;
		    ti.hwnd     = hButtonRecActi;
		    ti.uId      = 0;
		    ti.hinst    = NULL;
		    ti.lpszText = "L'activité a bien été enregistrée.";
		    GetClientRect(hButtonRecActi, &ti.rect);
		    SendMessage(hwndToolTips, TTM_ADDTOOL, 0, (LPARAM) &ti );*/
		    SendMessage(hComboJour,CB_ADDSTRING,(WPARAM) 0,(LPARAM) "Lundi");
		    SendMessage(hComboJour,CB_ADDSTRING,(WPARAM) 0,(LPARAM) "Mardi");
		    SendMessage(hComboJour,CB_ADDSTRING,(WPARAM) 0,(LPARAM) "Jeudi");
		    SendMessage(hComboJour,CB_ADDSTRING,(WPARAM) 0,(LPARAM) "Vendredi");

		    EnableWindow(hScroll,false);

		    //status bar
		    hstatu=CreateStatusWindow(WS_CHILD | WS_VISIBLE, NULL, hwnd,ID_STATUS);
		    int nWidth,cParts=5;

		    int paParts[5];
		    // Calculate the right edge coordinate for each part, and
		    // copy the coordinates to the array.
		    nWidth = rectHwnd.right / cParts;
		    int rightEdge = nWidth;
		    for (int i=0;i<cParts;i++)
			{
		       paParts[i] = rightEdge;
		       rightEdge += nWidth;
		    }

		    // Tell the status bar to create the window parts.
		    SendMessage(hstatu, SB_SETPARTS, (WPARAM) cParts, (LPARAM)paParts);

		    SendMessage(hstatu,SB_SETTEXT,MAKELONG(0,0),(LPARAM)"Nombre d'enfants en tout: 0");
		    SendMessage(hstatu,SB_SETTEXT,MAKELONG(1,SBT_POPOUT),(LPARAM)"Nombre d'enfants présents le lundi: 0");
		    SendMessage(hstatu,SB_SETTEXT,MAKELONG(2,SBT_POPOUT),(LPARAM)"Nombre d'enfants présents le mardi: 0");
		    SendMessage(hstatu,SB_SETTEXT,MAKELONG(3,SBT_POPOUT),(LPARAM)"Nombre d'enfants présents le jeudi: 0");
		    SendMessage(hstatu,SB_SETTEXT,MAKELONG(4,SBT_POPOUT),(LPARAM)"Nombre d'enfants présents le vendredi: 0");

		    //fin status bar
		    //SendMessage(hstatu,SB_SETTEXT,MAKELONG(2,0),(LPARAM)"");

			{
			    COMBOBOXINFO infoCombo;
			    infoCombo.cbSize=sizeof(COMBOBOXINFO);
			    GetComboBoxInfo(hComboActi,&infoCombo);
			    subclass_the_comboActiEdit(infoCombo.hwndItem);
			    subclass_the_comboBoxActi(hComboActi);
			}

			{
				SCROLLINFO infoScroll;
				infoScroll.cbSize=sizeof(SCROLLINFO);
				infoScroll.fMask=SIF_ALL;
				infoScroll.nMin=1;
				infoScroll.nMax=1;
				infoScroll.nPage=1;
				infoScroll.nPos=1;

				SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);
				//EnableScrollBar(hScroll,SB_CTL,ESB_DISABLE_BOTH);
				ShowWindow(hScroll, SW_SHOW);
			}

			subclass_the_listbox(hListboxNom);
			subclass_the_listbox(hListboxClasse);

			/*SendMessage(hListboxNom,LB_SETITEMHEIGHT,0,22);
			SendMessage(hListboxNom,LB_ADDSTRING,0,(LPARAM)"Machin Truc CP");
			SendMessage(hListboxNom,LB_SETITEMDATA,0,0);
			SendMessage(hListboxNom,LB_SETITEMHEIGHT,1,22);
			SendMessage(hListboxNom,LB_ADDSTRING,0,(LPARAM)"Bidule Chose CM2");
			SendMessage(hListboxNom,LB_SETITEMDATA,1,1);
			*/

			HDITEM item={HDI_FORMAT | HDI_WIDTH | HDI_TEXT};
			item.cxy=rectHwnd.right*5/24*2/3;
			item.pszText=(LPSTR)"Nom Prénom";
			item.fmt=HDF_CENTER | HDF_STRING;
			Header_InsertItem(hHeader,0,&item);

			item.cxy=rectHwnd.right*5/24*1/3;
			item.pszText=(LPSTR)"Classe";
			Header_InsertItem(hHeader,1,&item);

			item.cxy=((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3;
			item.pszText=(LPSTR)"Voeu 1";
			Header_InsertItem(hHeader,2,&item);

			item.cxy=((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3;
			item.pszText=(LPSTR)"Voeu 2";
			Header_InsertItem(hHeader,3,&item);

			item.cxy=((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3+3;
			item.pszText=(LPSTR)"Voeu 3";
			Header_InsertItem(hHeader,4,&item);

            TC_ITEM tie;
			tie.mask = TCIF_TEXT;
			tie.pszText = (LPSTR)"Lundi";
   			TabCtrl_InsertItem(hTabs, 1, &tie);
      		tie.pszText = (LPSTR)"Mardi";
        	TabCtrl_InsertItem(hTabs, 2, &tie);
        	tie.pszText = (LPSTR)"Jeudi";
        	TabCtrl_InsertItem(hTabs, 3, &tie);
        	tie.pszText = (LPSTR)"Vendredi";
        	TabCtrl_InsertItem(hTabs, 4, &tie);

			/*//temporaire
			InfoActivite tempActi;
			activite.push_back(tempActi);
			activite[0].nom="Photo";
			activite.push_back(tempActi);
			activite[1].nom="Echec";
			activite.push_back(tempActi);
			activite[2].nom="Jeu";
			activite.push_back(tempActi);
			activite[3].nom="Math";
			//fin temporaire*/

			/*for(int i=0;i<2;i++)
			{
 			    hComboV1.push_back(CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_SORT ,30+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*0-1,30+20+23*i,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,0,0,0));
				//hCombo[i]=CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST ,10+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*j-1,10+20+23*i,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,0,0,0);
				SendMessage(hComboV1[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) "Pas de choix");
				for(int j=0;j<activite.size();j++)
				{
				    SendMessage(hComboV1[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
				}
				//SendMessage(hCombo[i],CB_SETCURSEL,0,0);

				hComboV2.push_back(CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_SORT,30+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)-1,30+20+23*i,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,0,0,0));
				//hCombo[i]=CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST ,10+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*j-1,10+20+23*i,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,0,0,0);
				SendMessage(hComboV2[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) "Pas de choix");
				for(int j=0;j<activite.size();j++)
				{
				    SendMessage(hComboV2[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
				}
				hComboV3.push_back(CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_SORT,30+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*2-1,30+20+23*i,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3+3,20,hwnd,0,0,0));
				SendMessage(hComboV3[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) "Pas de choix");
				for(int j=0;j<activite.size();j++)
				{
				    SendMessage(hComboV3[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
				}
			}*/

			//Listview
			LVCOLUMN lvc;
    		int iCol=0;
    		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT ;
    		lvc.iSubItem = 0;
        	lvc.pszText = (LPSTR)"Nom Prénom Classe";
        	lvc.cx = rectHwnd.right*5/24;
        	lvc.fmt = LVCFMT_LEFT;
        	ListView_InsertColumn(hListviewL, iCol++, &lvc);

			lvc.iSubItem = 1;
        	lvc.pszText = (LPSTR)"Voeu 1";
        	lvc.cx = ((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3;
			ListView_InsertColumn(hListviewL, iCol++, &lvc);

			lvc.iSubItem = 2;
        	lvc.pszText = (LPSTR)"Voeu 2";
        	lvc.cx = ((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3;
			ListView_InsertColumn(hListviewL, iCol++, &lvc);

			lvc.iSubItem = 3;
        	lvc.pszText = (LPSTR)"Voeu 3";
        	lvc.cx = ((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3;
			ListView_InsertColumn(hListviewL, iCol++, &lvc);

			HFONT font=CreateFont(20,0,0,0,FW_BOLD ,FALSE , TRUE , FALSE ,DEFAULT_CHARSET ,OUT_DEFAULT_PRECIS ,CLIP_DEFAULT_PRECIS ,ANTIALIASED_QUALITY ,FF_ROMAN ,"Comic Sans MS");
			SendMessage (hHeader, WM_SETFONT, (WPARAM) font, 0);


			LVITEM lvI;
	  		lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
	  		lvI.stateMask = 0;
	  		lvI.iSubItem  = 0;
	  		lvI.state     = 0;
			lvI.iItem  = 0;

			ListView_InsertItem(hListviewL, &lvI);
			ListView_SetItemText(hListviewL,0,0, "a")
			ListView_SetItemText(hListviewL,0,1, "a")
			ListView_SetItemText(hListviewL,0,2, "a")
			ListView_SetItemText(hListviewL,0,3, "a")

			lvI.iItem  = 1;
			lvI.iSubItem  = 0;

			ListView_InsertItem(hListviewL, &lvI);
			ListView_SetItemText(hListviewL,1,0, "b")
			ListView_SetItemText(hListviewL,1,1, "b")
			ListView_SetItemText(hListviewL,1,2, "b")
			ListView_SetItemText(hListviewL,1,3, "b")

			//Fin Listview
		    string sBackUp="~&Nouveau document GRMA.grma";
			if(access(sBackUp.c_str(),F_OK)!=-1)
			{
			    if(MessageBox(hwnd,"La sauvegarde automatique de la dernière utilisation de GroupManage a été détectée.\n\
Voulez-vous la récupérer ?","Sauvegarde automatique détectée",MB_ICONINFORMATION | MB_YESNO)==IDYES)
			    {
				    pathFileOpen=sBackUp;
				    CreateToolTip(hwnd, hwndToolTips, hinstance);
			    }
			}
			else
			{
	 		    char szPath[MAX_PATH] = "";
		    	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szPath);
		    	sBackUp=string(szPath)+"\\~&Nouveau document GRMA.grma";
		    	if(access(sBackUp.c_str(),F_OK)!=-1)
				{
				    if(MessageBox(hwnd,"La sauvegarde automatique de la dernière utilisation de GroupManage a été détectée.\n\
Voulez-vous la récupérer ?","Sauvegarde automatique détectée",MB_ICONINFORMATION | MB_YESNO)==IDYES)
				    {
					    pathFileOpen=sBackUp;
					    CreateToolTip(hwnd, hwndToolTips, hinstance);
				    }
				}
			}
			sBackUp="~&Nouveau document GRMA.grma2";
			if(access(sBackUp.c_str(),F_OK)!=-1)
			{
			    if(MessageBox(hwnd,"La sauvegarde automatique de la dernière utilisation de GroupManage a été détectée.\n\
Voulez-vous la récupérer ?","Sauvegarde automatique détectée",MB_ICONINFORMATION | MB_YESNO)==IDYES)
			    {
				    pathFileOpen=sBackUp;
				    CreateToolTip(hwnd, hwndToolTips, hinstance);
			    }
			}
			else
			{
	 		    char szPath[MAX_PATH] = "";
		    	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szPath);
		    	sBackUp=string(szPath)+"\\~&Nouveau document GRMA.grma2";
		    	if(access(sBackUp.c_str(),F_OK)!=-1)
				{
				    if(MessageBox(hwnd,"La sauvegarde automatique de la dernière utilisation de GroupManage a été détectée.\n\
Voulez-vous la récupérer ?","Sauvegarde automatique détectée",MB_ICONINFORMATION | MB_YESNO)==IDYES)
				    {
					    pathFileOpen=sBackUp;
					    CreateToolTip(hwnd, hwndToolTips, hinstance);
				    }
				}
			}
			//si l'application à été lancé par un double clic sur un fichier .grma	   }
			//strcpy(pathFileOpen,"G:/Personel/C++/gestion des groupes/test.grma");

			if(pathFileOpen!="")
			{
				OuvrirFeuille(hwnd);
			}

			SetTimer(hwnd, ID_TIMER_BACKUP, intervalTimer, NULL);

			EnumChildWindows(hwnd,(WNDENUMPROC)&EnumChildStartPosProc,(LPARAM)0);

			return 0;
		}
		case WM_NOTIFY:
	 	{//MessageBox(0,0,0,0);
	 	    NMHEADER *pnmheader=(LPNMHEADER) lParam;
	 	    if(pnmheader->hdr.code==HDN_BEGINTRACK)
			{
   			    //cxyAvant=pnmheader->pitem->cxy;
   			    return true;
			}
			if(pnmheader->hdr.code==HDN_ENDTRACK)
			{
		 	    /*HDITEM item={0};
		 	    //Header_GetItem(pnmheader->hdr.hwndFrom,pnmheader->iItem,&item);
		 	    item.mask=HDI_WIDTH;
   			    item.cxy=cxyAvant;
   			    Header_SetItem(pnmheader->hdr.hwndFrom,pnmheader->iItem,&item);*/
   			    return true;
   			}
   			if(pnmheader->hdr.code==HDN_ITEMCLICK)
   			{
                switch (pnmheader->iItem)
                {
                    case 0:
                    {
                        bool arret=false;
                        for(int j=0;j<2 && !arret;j++)
                        {
                            for(int i=0;i<enfants.size()-1;)
                            {
                                int iEnfant1=SendMessage(hListboxNom,LB_GETITEMDATA,i,0);
                                int iEnfant2=SendMessage(hListboxNom,LB_GETITEMDATA,i+1,0);

                                if((j==0)? (enfants[iEnfant1].nom > enfants[iEnfant2].nom) : (enfants[iEnfant1].nom < enfants[iEnfant2].nom))
                                {
                                    //Inversion des individus
                                    //individu 2
                                    SendMessage(hListboxNom,LB_DELETESTRING,i,0);
                         		    SendMessage(hListboxNom,LB_INSERTSTRING,i,(LPARAM)enfants[iEnfant2].nom.c_str());
                         		    SendMessage(hListboxNom,LB_SETITEMDATA,i, (LPARAM)iEnfant2);

                         		    SendMessage(hListboxClasse,LB_DELETESTRING,i,0);
                         		    SendMessage(hListboxClasse,LB_INSERTSTRING,i,(LPARAM)enfants[iEnfant2].classe.c_str());
                         		    SendMessage(hListboxClasse,LB_SETITEMDATA,i, (LPARAM)iEnfant2);

                         		    //Individu 1
                         		    SendMessage(hListboxNom,LB_DELETESTRING,i+1,0);
                         		    SendMessage(hListboxNom,LB_INSERTSTRING,i+1,(LPARAM)enfants[iEnfant1].nom.c_str());
                         		    SendMessage(hListboxNom,LB_SETITEMDATA,i+1, (LPARAM)iEnfant1);

                         		    SendMessage(hListboxClasse,LB_DELETESTRING,i+1,0);
                         		    SendMessage(hListboxClasse,LB_INSERTSTRING,i+1,(LPARAM)enfants[iEnfant1].classe.c_str());
                         		    SendMessage(hListboxClasse,LB_SETITEMDATA,i+1, (LPARAM)iEnfant1);
                         		    arret=true;
                         		    if(i>=1)
                         		    {
                         		        --i;
                                    }
                                    else
                                    {
                                        ++i;
                                    }
                                }
                                else if(i<=enfants.size()-2)
                                {
                                    ++i;
                                }
                            }
                        }
                        ActualiserTabCombo(SendMessage(hListboxNom,LB_GETTOPINDEX,0,0));
                        return true;
                    }
                    case 1:
                    {
                        bool arret=false;
                        for(int j=0;j<2 && !arret;j++)
                        {
                            for(int i=0;i<enfants.size()-1;)
                            {
                                int iEnfant1=SendMessage(hListboxNom,LB_GETITEMDATA,i,0);
                                int iEnfant2=SendMessage(hListboxNom,LB_GETITEMDATA,i+1,0);

                                if((j==0)? (enfants[iEnfant1].classe > enfants[iEnfant2].classe) : (enfants[iEnfant1].classe < enfants[iEnfant2].classe))
                                {
                                    //Inversion des individus
                                    //individu 2
                                    SendMessage(hListboxNom,LB_DELETESTRING,i,0);
                         		    SendMessage(hListboxNom,LB_INSERTSTRING,i,(LPARAM)enfants[iEnfant2].nom.c_str());
                         		    SendMessage(hListboxNom,LB_SETITEMDATA,i, (LPARAM)iEnfant2);

                         		    SendMessage(hListboxClasse,LB_DELETESTRING,i,0);
                         		    SendMessage(hListboxClasse,LB_INSERTSTRING,i,(LPARAM)enfants[iEnfant2].classe.c_str());
                         		    SendMessage(hListboxClasse,LB_SETITEMDATA,i, (LPARAM)iEnfant2);

                         		    //Individu 1
                         		    SendMessage(hListboxNom,LB_DELETESTRING,i+1,0);
                         		    SendMessage(hListboxNom,LB_INSERTSTRING,i+1,(LPARAM)enfants[iEnfant1].nom.c_str());
                         		    SendMessage(hListboxNom,LB_SETITEMDATA,i+1, (LPARAM)iEnfant1);

                         		    SendMessage(hListboxClasse,LB_DELETESTRING,i+1,0);
                         		    SendMessage(hListboxClasse,LB_INSERTSTRING,i+1,(LPARAM)enfants[iEnfant1].classe.c_str());
                         		    SendMessage(hListboxClasse,LB_SETITEMDATA,i+1, (LPARAM)iEnfant1);
                         		    arret=true;
                         		    if(i>=1)
                         		    {
                         		        --i;
                                    }
                                    else
                                    {
                                        ++i;
                                    }
                                }
                                else if(i<=enfants.size()-2)
                                {
                                    ++i;
                                }
                            }
                        }
                        ActualiserTabCombo(SendMessage(hListboxNom,LB_GETTOPINDEX,0,0));
                        return true;
                    }
                    case 2:
                    {
                        bool arret=false;
                        for(int j=0;j<2 && !arret;j++)
                        {
                            for(int i=0;i<enfants.size()-1;)
                            {
                                int iEnfant1=SendMessage(hListboxNom,LB_GETITEMDATA,i,0);
                                int iEnfant2=SendMessage(hListboxNom,LB_GETITEMDATA,i+1,0);

                                if((j==0)? (enfants[iEnfant1].voeux[jourEnCourSaisie].voeu1 > enfants[iEnfant2].voeux[jourEnCourSaisie].voeu1) : (enfants[iEnfant1].voeux[jourEnCourSaisie].voeu1 < enfants[iEnfant2].voeux[jourEnCourSaisie].voeu1))
                                {
                                    //Inversion des individus
                                    //individu 2
                                    SendMessage(hListboxNom,LB_DELETESTRING,i,0);
                         		    SendMessage(hListboxNom,LB_INSERTSTRING,i,(LPARAM)enfants[iEnfant2].nom.c_str());
                         		    SendMessage(hListboxNom,LB_SETITEMDATA,i, (LPARAM)iEnfant2);

                         		    SendMessage(hListboxClasse,LB_DELETESTRING,i,0);
                         		    SendMessage(hListboxClasse,LB_INSERTSTRING,i,(LPARAM)enfants[iEnfant2].classe.c_str());
                         		    SendMessage(hListboxClasse,LB_SETITEMDATA,i, (LPARAM)iEnfant2);

                         		    //Individu 1
                         		    SendMessage(hListboxNom,LB_DELETESTRING,i+1,0);
                         		    SendMessage(hListboxNom,LB_INSERTSTRING,i+1,(LPARAM)enfants[iEnfant1].nom.c_str());
                         		    SendMessage(hListboxNom,LB_SETITEMDATA,i+1, (LPARAM)iEnfant1);

                         		    SendMessage(hListboxClasse,LB_DELETESTRING,i+1,0);
                         		    SendMessage(hListboxClasse,LB_INSERTSTRING,i+1,(LPARAM)enfants[iEnfant1].classe.c_str());
                         		    SendMessage(hListboxClasse,LB_SETITEMDATA,i+1, (LPARAM)iEnfant1);
                         		    arret=true;
                         		    if(i>=1)
                         		    {
                         		        --i;
                                    }
                                    else
                                    {
                                        ++i;
                                    }
                                }
                                else if(i<=enfants.size()-2)
                                {
                                    ++i;
                                }
                            }
                        }
                        ActualiserTabCombo(SendMessage(hListboxNom,LB_GETTOPINDEX,0,0));
                        return true;
                    }
                    case 3:
                    {
                        bool arret=false;
                        for(int j=0;j<2 && !arret;j++)
                        {
                            for(int i=0;i<enfants.size()-1;)
                            {
                                int iEnfant1=SendMessage(hListboxNom,LB_GETITEMDATA,i,0);
                                int iEnfant2=SendMessage(hListboxNom,LB_GETITEMDATA,i+1,0);

                                if((j==0)? (enfants[iEnfant1].voeux[jourEnCourSaisie].voeu2 > enfants[iEnfant2].voeux[jourEnCourSaisie].voeu2) : (enfants[iEnfant1].voeux[jourEnCourSaisie].voeu2 < enfants[iEnfant2].voeux[jourEnCourSaisie].voeu2))
                                {
                                    //Inversion des individus
                                    //individu 2
                                    SendMessage(hListboxNom,LB_DELETESTRING,i,0);
                         		    SendMessage(hListboxNom,LB_INSERTSTRING,i,(LPARAM)enfants[iEnfant2].nom.c_str());
                         		    SendMessage(hListboxNom,LB_SETITEMDATA,i, (LPARAM)iEnfant2);

                         		    SendMessage(hListboxClasse,LB_DELETESTRING,i,0);
                         		    SendMessage(hListboxClasse,LB_INSERTSTRING,i,(LPARAM)enfants[iEnfant2].classe.c_str());
                         		    SendMessage(hListboxClasse,LB_SETITEMDATA,i, (LPARAM)iEnfant2);

                         		    //Individu 1
                         		    SendMessage(hListboxNom,LB_DELETESTRING,i+1,0);
                         		    SendMessage(hListboxNom,LB_INSERTSTRING,i+1,(LPARAM)enfants[iEnfant1].nom.c_str());
                         		    SendMessage(hListboxNom,LB_SETITEMDATA,i+1, (LPARAM)iEnfant1);

                         		    SendMessage(hListboxClasse,LB_DELETESTRING,i+1,0);
                         		    SendMessage(hListboxClasse,LB_INSERTSTRING,i+1,(LPARAM)enfants[iEnfant1].classe.c_str());
                         		    SendMessage(hListboxClasse,LB_SETITEMDATA,i+1, (LPARAM)iEnfant1);
                         		    arret=true;
                         		    if(i>=1)
                         		    {
                         		        --i;
                                    }
                                    else
                                    {
                                        ++i;
                                    }
                                }
                                else if(i<=enfants.size()-2)
                                {
                                    ++i;
                                }
                            }
                        }
                        ActualiserTabCombo(SendMessage(hListboxNom,LB_GETTOPINDEX,0,0));
                        return true;
                    }
                    case 4:
                    {
                        bool arret=false;
                        for(int j=0;j<2 && !arret;j++)
                        {
                            for(int i=0;i<enfants.size()-1;)
                            {
                                int iEnfant1=SendMessage(hListboxNom,LB_GETITEMDATA,i,0);
                                int iEnfant2=SendMessage(hListboxNom,LB_GETITEMDATA,i+1,0);

                                if((j==0)? (enfants[iEnfant1].voeux[jourEnCourSaisie].voeu3 > enfants[iEnfant2].voeux[jourEnCourSaisie].voeu3) : (enfants[iEnfant1].voeux[jourEnCourSaisie].voeu3 < enfants[iEnfant2].voeux[jourEnCourSaisie].voeu3))
                                {
                                    //Inversion des individus
                                    //individu 2
                                    SendMessage(hListboxNom,LB_DELETESTRING,i,0);
                         		    SendMessage(hListboxNom,LB_INSERTSTRING,i,(LPARAM)enfants[iEnfant2].nom.c_str());
                         		    SendMessage(hListboxNom,LB_SETITEMDATA,i, (LPARAM)iEnfant2);

                         		    SendMessage(hListboxClasse,LB_DELETESTRING,i,0);
                         		    SendMessage(hListboxClasse,LB_INSERTSTRING,i,(LPARAM)enfants[iEnfant2].classe.c_str());
                         		    SendMessage(hListboxClasse,LB_SETITEMDATA,i, (LPARAM)iEnfant2);

                         		    //Individu 1
                         		    SendMessage(hListboxNom,LB_DELETESTRING,i+1,0);
                         		    SendMessage(hListboxNom,LB_INSERTSTRING,i+1,(LPARAM)enfants[iEnfant1].nom.c_str());
                         		    SendMessage(hListboxNom,LB_SETITEMDATA,i+1, (LPARAM)iEnfant1);

                         		    SendMessage(hListboxClasse,LB_DELETESTRING,i+1,0);
                         		    SendMessage(hListboxClasse,LB_INSERTSTRING,i+1,(LPARAM)enfants[iEnfant1].classe.c_str());
                         		    SendMessage(hListboxClasse,LB_SETITEMDATA,i+1, (LPARAM)iEnfant1);
                         		    arret=true;
                         		    if(i>=1)
                         		    {
                         		        --i;
                                    }
                                    else
                                    {
                                        ++i;
                                    }
                                }
                                else if(i<=enfants.size()-2)
                                {
                                    ++i;
                                }
                            }
                        }
                        ActualiserTabCombo(SendMessage(hListboxNom,LB_GETTOPINDEX,0,0));
                        return true;
                    }
                    default:
                    {
                        return true;
                    }
                    return true;
                }
            }
			/*if(pnmheader->hdr.code==HDN_TRACK)
			{
   			    for(int i=0;i<hComboV1.size();i++)
   			    {
				    RECT rect;
				    RECT rectItem;
				    int longueurItem;
				    if(pnmheader->iItem==0)
				    {
					    GetWindowRect(hListboxNom,&rect);
   					    SetWindowPos(hListboxNom,0,0,0,pnmheader->pitem->cxy,rect.bottom-rect.top,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

   					    SetWindowPos(hComboV1[i],0,30+pnmheader->pitem->cxy,50+23*i,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);//rect.left,rect.top

   					    GetWindowRect(hComboV1[0],&rectItem);
   					    longueurItem=rectItem.right-rectItem.left;
					    SetWindowPos(hComboV2[i],0,rectItem.right-3,50+23*i,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);//10+pnmheader->pitem->cxy+longueurItem

					    GetWindowRect(hComboV2[0],&rectItem);
					    HDITEM ItemComboEnCour;
					    SendMessage(hHeader,HDM_GETITEM,3,(LPARAM)&ItemComboEnCour);
   					    SetWindowPos(hComboV3[i],0,rectItem.right-3,50+23*i,ItemComboEnCour.cxy,22,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);//10+pnmheader->pitem->cxy+longueurItem+rectItem.right-rectItem.left

   					    /*GetWindowRect(hComboV3[i],&rectItem);
   					    POINT pt;
   					    pt.x=rectItem.right;
   					    pt.y=rectItem.left;
   					    ScreenToClient(hwnd,&pt);
   					    RECT rectHwnd;
   					    GetClientRect(hwnd,&rectHwnd);
					    if(pt.x>rectHwnd.right*5/8+30)
					    {
						 						   /*char a[4];
   					    itoa(pt.x,a,10);
   					    MessageBox(0,a,0,0);*/
					   	    /*SetWindowPos(hComboV3[i],0,0,0,(rectHwnd.right*5/8+30)-pt.y-27,rectItem.bottom-rectItem.top,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
						}*/
					/*}
				    else if(pnmheader->iItem==1)
				    {
					    GetWindowRect(hComboV1[i],&rect);//hCombo[i][pnmheader->iItem-1]
				        SetWindowPos(hComboV1[i],0,0,0,pnmheader->pitem->cxy,rect.bottom-rect.top,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);//rect.left,rect.top
				        ShowWindow(hComboV1[i], SW_SHOW);

						SetWindowPos(hComboV2[i],0,rect.right-3,50+23*i,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

					    GetWindowRect(hComboV2[0],&rectItem);
   					    SetWindowPos(hComboV3[i],0,rectItem.right-3,50+23*i,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
						//InvalidateRect(hwnd, 0, 1);
					}
					else if(pnmheader->iItem==2)
					{
		 	 		    GetWindowRect(hComboV2[i],&rect);//hCombo[i][pnmheader->iItem-1]
				        SetWindowPos(hComboV2[i],0,0,0,pnmheader->pitem->cxy,rect.bottom-rect.top,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);//rect.left,rect.top
				        ShowWindow(hComboV2[i], SW_SHOW);

				        SetWindowPos(hComboV3[i],0,rect.right-3,50+23*i,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
						//InvalidateRect(hwnd, 0, 1);
					}
					else if(pnmheader->iItem==3)
					{
		 	 		    GetWindowRect(hComboV3[i],&rect);//hCombo[i][pnmheader->iItem-1]
				        SetWindowPos(hComboV3[i],0,0,0,pnmheader->pitem->cxy,rect.bottom-rect.top,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);//rect.left,rect.top
				        ShowWindow(hComboV3[i], SW_SHOW);
						//InvalidateRect(hwnd, 0, 1);
					}

				}
				InvalidateRect(hHeader, 0, 1);
			}*/
			LPNMHDR pnmhdr = (LPNMHDR)lParam  ;
			if(pnmhdr->code == TCN_SELCHANGE)
			{
				if(TabCtrl_GetCurSel(hTabs) == 0)
				{
				    ::jourEnCourSaisie=0;
				}
				if(TabCtrl_GetCurSel(hTabs) == 1)
				{
				    ::jourEnCourSaisie=1;
				}
				if(TabCtrl_GetCurSel(hTabs) == 2)
				{
				    ::jourEnCourSaisie=2;
				}
				if(TabCtrl_GetCurSel(hTabs) == 3)
				{
				    ::jourEnCourSaisie=3;
				}
				if(enfants.size()>0)
				{
					/*SCROLLINFO infoScroll;
			 	    infoScroll.cbSize=sizeof(infoScroll);
			 	    infoScroll.fMask=SIF_ALL;
		    		GetScrollInfo(hScroll,SB_CTL,&infoScroll);
					//infoScroll.nPos=0;
					infoScroll.fMask = SIF_POS;
					//SendMessage(hListboxNom,LB_SETTOPINDEX,0,0);
					//SendMessage(hListboxClasse,LB_SETTOPINDEX,0,0);*/

					/* DONE (Simon#1#):  Modifier ActualiserTabCombo() pour pouvoir gérer
					                    les listes désordonnées */

					int top=SendMessage(hListboxClasse,LB_GETTOPINDEX,0,0);

					//char a[10];
					//itoa(activite.size(),a,10);
					//MessageBox(0,a,0,0);

					//SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);

					ActualiserTabCombo(top);
					if (enfants.size()<nbLigne)
					{
					    EnableWindow(hScroll,false);
					}
				}
				return 0;
			}
   		    /*if(pnmheader->hdr.code==HDN_ENDTRACK)
			{
   			    //char a[4];
   				//itoa(pnmheader->pitem->cxy,a,10);
				//MessageBox(0,a,0,0);
   			    for(int i=0;i<hComboV1.size();i++)
   			    {
				    RECT rect;
				    if(pnmheader->iItem==1)
				    {
					    GetWindowRect(hComboV1[i],&rect);//hCombo[i][pnmheader->iItem-1]
				        SetWindowPos(hComboV1[i],0,0,0,pnmheader->pitem->cxy,rect.bottom-rect.top,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);//rect.left,rect.top
				        ShowWindow(hComboV1[i], SW_SHOW);
						//InvalidateRect(hwnd, 0, 1);
					}
					else if(pnmheader->iItem==2)
					{
		 	 		    GetWindowRect(hComboV2[i],&rect);//hCombo[i][pnmheader->iItem-1]
				        SetWindowPos(hComboV2[i],0,0,0,pnmheader->pitem->cxy,rect.bottom-rect.top,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);//rect.left,rect.top
				        ShowWindow(hComboV2[i], SW_SHOW);
						//InvalidateRect(hwnd, 0, 1);
					}
					else if(pnmheader->iItem==3)
					{
		 	 		    GetWindowRect(hComboV3[i],&rect);//hCombo[i][pnmheader->iItem-1]
				        SetWindowPos(hComboV3[i],0,0,0,pnmheader->pitem->cxy,rect.bottom-rect.top,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);//rect.left,rect.top
				        ShowWindow(hComboV3[i], SW_SHOW);
						//InvalidateRect(hwnd, 0, 1);
					}

				}

			}*/
 	 	    /*LPNMHDR pnmhdr = (LPNMHDR)lParam  ;
            if(pnmhdr->code == TCN_SELCHANGE && TabCtrl_GetCurSel(hTabs) == 0)
            {
                /*ShowWindow(hCB,SW_HIDE);
                ShowWindow(hstatic,SW_HIDE);
                ShowWindow(hEdit,SW_SHOW);
                ShowWindow(hlunch,SW_HIDE);
            }
            if(pnmhdr->code == TCN_SELCHANGE && TabCtrl_GetCurSel(hTabs) == 1)
            {
                /*ShowWindow(hEdit,SW_HIDE);
                ShowWindow(hstatic,SW_SHOW);
                ShowWindow(hCB,SW_SHOW);
                ShowWindow(hlunch,SW_HIDE);
            }
            if(pnmhdr->code == TCN_SELCHANGE && TabCtrl_GetCurSel(hTabs) == 2)
            {
                /*ShowWindow(hEdit,SW_HIDE);
                ShowWindow(hstatic,SW_SHOW);
                ShowWindow(hCB,SW_SHOW);
                ShowWindow(hlunch,SW_HIDE);
            }
            if(pnmhdr->code == TCN_SELCHANGE && TabCtrl_GetCurSel(hTabs) == 3)
            {
                /*ShowWindow(hEdit,SW_HIDE);
                ShowWindow(hstatic,SW_SHOW);
                ShowWindow(hCB,SW_SHOW);
                ShowWindow(hlunch,SW_HIDE);
            }
            if(pnmhdr->code == TCN_SELCHANGE && TabCtrl_GetCurSel(hTabs) == 4)
            {
			    ShowWindow(hlunch,SW_SHOW);
			}*/
            return 0;
        }
		case WM_COMMAND:
	 	{
 	        if(LOWORD(wParam)==ID_LUNCH)
 	 		{
			    /*string texte;
			    for(int i=0;i<enfants[0].size();i++)
			    {
		 		    texte=texte+enfants[0][i].np+"\t"+activite[enfants[0][i].voeu1].nom+"\n";
				}
				MessageBox(0,texte.c_str(),"enfants enregistrés",0);*/
				if(enfants.size()==0)
    			{
	    		    return 0;
				}
		 		//fLog<<"i: "<<endl<<i<<endl;
     		    DWORD threadID;
		 		CreateThread(NULL, 0, OrganisationDesDonnees,NULL, 0, &threadID);

		 		DialogBox(hinstance, "DIALOGTRAITEMENTENCOURS" , hwnd, (DLGPROC)DialogTraitementEnCours);
		 		//réouverture du fichier

				string sBackUp=pathFileOpen;
				size_t found = sBackUp.rfind('\\');
				sBackUp.replace (found+1,2,"~&");
				if(access(sBackUp.c_str(),F_OK)!=-1)
				{
				    remove(sBackUp.c_str());
				}

				OuvrirFeuille(hwnd);
			}
			if(LOWORD(wParam)==ID_BUTTONGOLINE)
			{
   			    int lEditbox=GetWindowTextLength(hEditFindLine)+1;
   			    //char *sEditbox=new char[lEditbox];
   			    vector<char> sEditbox(lEditbox);
   			    GetWindowText(hEditFindLine,&sEditbox[0],lEditbox);
   			    int ligne=(atoi(&sEditbox[0])==0)?0:atoi(&sEditbox[0])-1;

				SendMessage(hListboxNom,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxNom,LB_GETCOUNT,0,0)));
				SendMessage(hListboxClasse,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxClasse,LB_GETCOUNT,0,0)));
				SendMessage(hListboxNom,LB_SETSEL,true,ligne);
				SendMessage(hListboxClasse,LB_SETSEL,true,ligne);
   			    //SendMessage(hListboxNom,LB_SETCURSEL,ligne,0);
   			    //SendMessage(hListboxClasse,LB_SETCURSEL,ligne,0);
		 		SendMessage(hwnd,WM_COMMAND,MAKEWPARAM(ID_LISTBOX,LBN_SELCHANGE),0);

   			    //delete[]sEditbox;
			}
			if(LOWORD(wParam)==ID_BUTTONFINDNAME)
			{
                /* DONE (Simon#1#): Modifier la procédure ID_BUTTONFINDNAME pour
                                    que iItemFind gère la liste désordonnée */

		 	    int lEditbox=GetWindowTextLength(hEditFindName)+1;
		 	    if(lEditbox==1)return true;
   			    char *sEditbox=new char[lEditbox];
   			    GetWindowText(hEditFindName,sEditbox,lEditbox);
   			    //strcat(sEditbox,'\0');

				int iItemFind=SendMessage(hListboxNom,LB_FINDSTRING,(WPARAM)-1,(LPARAM)sEditbox);
				if(iItemFind==LB_ERR)
				{
			 	    for(int i=0;i<enfants.size();i++)
			 	    {
	 				    if(enfants[i].nom.find(sEditbox)!=string::npos)//strstr()
	 				    {
						    for(int j=0;j<enfants.size();j++)
						    {
                                if(SendMessage(hListboxNom,LB_GETITEMDATA,j,0)==i)
                                {
                                    iItemFind=j;
                                    break;
                                }
                            }
						    break;
						}
					}
					if(iItemFind==LB_ERR)
					{
				 	    for(int i=0;i<enfants.size();i++)
				 	    {
							string chaine1=enfants[i].nom;
							transform(chaine1.begin(), chaine1.end(), chaine1.begin(), ::tolower);
							/*for(int j=0;j<enfants[i].nom.size();j++)
		 				    {
		 					    chaine1[j]=tolower(chaine1[j]);
							}*/
                            for(int j=0;j<lEditbox;j++)
		 				    {
		 					    sEditbox[j]=tolower(sEditbox[j]);
							}

							//MessageBox(0,chaine1,sEditbox,0);
		 				    if(chaine1.find(sEditbox)!=string::npos)
		 				    {
							    for(int j=0;j<enfants.size();j++)
    						    {
                                    if(SendMessage(hListboxNom,LB_GETITEMDATA,j,0)==i)
                                    {
                                        iItemFind=j;
                                        break;
                                    }
                                }
							    break;
							}
						}
					}
					if(iItemFind==LB_ERR)
					{
				 	    iItemFind=SendMessage(hListboxClasse,LB_FINDSTRING,(WPARAM)-1,(LPARAM)sEditbox);
				 	    if(iItemFind==LB_ERR)
						{
					 	    for(int i=0;i<enfants.size();i++)
					 	    {
			 				    if(enfants[i].classe.find(sEditbox)!=string::npos)
			 				    {
								    for(int j=0;j<enfants.size();j++)
        						    {
                                        if(SendMessage(hListboxNom,LB_GETITEMDATA,j,0)==i)
                                        {
                                            iItemFind=j;
                                            break;
                                        }
                                    }
								    break;
								}
							}
							if(iItemFind==LB_ERR)
							{
						 	    for(int i=0;i<enfants.size();i++)
						 	    {

									string chaine1=enfants[i].classe;
                                    transform(chaine1.begin(), chaine1.end(), chaine1.begin(), ::tolower);
									/*for(int j=0;j<enfants[i].classe.size();j++)
				 				    {
				 					    chaine1[j]=tolower(chaine1[j]);
									}*/
									for(int j=0;j<lEditbox;j++)
				 				    {
				 					    sEditbox[j]=tolower(sEditbox[j]);
									}

									//MessageBox(0,chaine1,sEditbox,0);
				 				    if(chaine1.find(sEditbox)!=string::npos)
				 				    {
									    for(int j=0;j<enfants.size();j++)
            						    {
                                            if(SendMessage(hListboxNom,LB_GETITEMDATA,j,0)==i)
                                            {
                                                iItemFind=j;
                                                break;
                                            }
                                        }
									    break;
									}
								}
							}
							if(iItemFind==LB_ERR)
							{

						 	    string retour="Le texte \""+string(sEditbox)+"\" n'a pas été trouvé.";
						 	    MessageBox(0,retour.c_str(),"Information",MB_ICONINFORMATION);
				 	    		delete[]sEditbox;
						 	    return 0;
							}
						}
					}
				}
				/* DONE (Simon#1#): SETCURSEL */

		 		SendMessage(hListboxNom,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxNom,LB_GETCOUNT,0,0)));
				SendMessage(hListboxClasse,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxClasse,LB_GETCOUNT,0,0)));

				SendMessage(hListboxNom,LB_SETSEL,true,iItemFind);
				SendMessage(hListboxClasse,LB_SETSEL,true,iItemFind);

		 		SendMessage(hwnd,WM_COMMAND,MAKEWPARAM(ID_LISTBOX,LBN_SELCHANGE),0);

		 	    delete[]sEditbox;
			}
			if(LOWORD(wParam)==ID_BUTTONFINDNEXT)
			{
                /* DONE (Simon#1#): Modifier ID_BUTTONFINDNEXT pour
                               que iItemFind gère la liste désordonnée */

		 	    int lEditbox=GetWindowTextLength(hEditFindName)+1;
		 	    if(lEditbox==1)return true;
   			    char *sEditbox=new char[lEditbox];
   			    GetWindowText(hEditFindName,sEditbox,lEditbox);
   			    //strcat(sEditbox,'\0');

				int iItemFind=SendMessage(hListboxNom,LB_FINDSTRING,(WPARAM)SendMessage(hListboxNom,LB_GETCURSEL,0,0),(LPARAM)sEditbox);
				if(iItemFind==LB_ERR)
				{
			 	    for(int i=SendMessage(hListboxNom,LB_GETCURSEL,0,0)+1;i<enfants.size();i++)
			 	    {
	 				    if(enfants[i].nom.find(sEditbox)!=string::npos)
	 				    {
						    for(int j=0;j<enfants.size();j++)
						    {
                                if(SendMessage(hListboxNom,LB_GETITEMDATA,j,0)==i)
                                {
                                    iItemFind=j;
                                    break;
                                }
                            }
						    break;
						}
					}
					if(iItemFind==LB_ERR)
					{
				 	    for(int i=SendMessage(hListboxNom,LB_GETCURSEL,0,0)+1;i<enfants.size();i++)
				 	    {
		 				    string chaine1=enfants[i].nom;
		 				    transform(chaine1.begin(), chaine1.end(), chaine1.begin(), ::tolower);
							/*for(int j=0;j<enfants[i].nom.size();j++)
		 				    {
		 					    chaine1[j]=tolower(chaine1[j]);
							}*/
							for(int j=0;j<lEditbox;j++)
		 				    {
		 					    sEditbox[j]=tolower(sEditbox[j]);
							}

		 				    if(chaine1.find(sEditbox)!=string::npos)
		 				    {
							    for(int j=0;j<enfants.size();j++)
    						    {
                                    if(SendMessage(hListboxNom,LB_GETITEMDATA,j,0)==i)
                                    {
                                        iItemFind=j;
                                        break;
                                    }
                                }
							    break;
							}
						}
					}
					if(iItemFind==LB_ERR)
					{
				 	    iItemFind=SendMessage(hListboxClasse,LB_FINDSTRING,(WPARAM)SendMessage(hListboxNom,LB_GETCURSEL,0,0),(LPARAM)sEditbox);

						if(iItemFind==LB_ERR)
						{
					 	    for(int i=SendMessage(hListboxNom,LB_GETCURSEL,0,0)+1;i<enfants.size();i++)
					 	    {
			 				    if(enfants[i].classe.find(sEditbox)!=string::npos)
			 				    {
								    for(int j=0;j<enfants.size();j++)
        						    {
                                        if(SendMessage(hListboxNom,LB_GETITEMDATA,j,0)==i)
                                        {
                                            iItemFind=j;
                                            break;
                                        }
                                    }
								    break;
								}
							}
							if(iItemFind==LB_ERR)
							{
						 	    for(int i=SendMessage(hListboxNom,LB_GETCURSEL,0,0)+1;i<enfants.size();i++)
						 	    {
				 				    string chaine1=enfants[i].classe;
				 				    transform(chaine1.begin(), chaine1.end(), chaine1.begin(), ::tolower);
									/*for(int j=0;j<chaine1.size();j++)
				 				    {
				 					    chaine1[j]=tolower(chaine1[j]);
									}*/
									for(int j=0;j<lEditbox;j++)
				 				    {
				 					    sEditbox[j]=tolower(sEditbox[j]);
									}

				 				    if(chaine1.find(sEditbox)!=string::npos)
				 				    {
									    for(int j=0;j<enfants.size();j++)
            						    {
                                            if(SendMessage(hListboxNom,LB_GETITEMDATA,j,0)==i)
                                            {
                                                iItemFind=j;
                                                break;
                                            }
                                        }
									    break;
									}
								}
							}
							if(iItemFind==LB_ERR)
							{
						 	    string retour="Le texte \""+string(sEditbox)+"\" n'a pas été trouvé.";
						 	    MessageBox(0,retour.c_str(),"Information",MB_ICONINFORMATION);
				 	    		delete[]sEditbox;
						 	    return 0;
						 	}
						}
				 	}
				}
				/* DONE (Simon#1#): SETCURSEL */

   			    SendMessage(hListboxNom,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxNom,LB_GETCOUNT,0,0)));
				SendMessage(hListboxClasse,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxClasse,LB_GETCOUNT,0,0)));

				SendMessage(hListboxNom,LB_SETSEL,true,iItemFind);
				SendMessage(hListboxClasse,LB_SETSEL,true,iItemFind);

		 		SendMessage(hwnd,WM_COMMAND,MAKEWPARAM(ID_LISTBOX,LBN_SELCHANGE),0);

		 	    delete[]sEditbox;
			}
			if(LOWORD(wParam)==ID_EDITNBPLACES && HIWORD(wParam)==EN_CHANGE)
			{
   			    if(activite.size()==0)
   			    {
   				    return false;
   				}
   			    int lEditbox = GetWindowTextLength(hEditUpDown)+1;
			    char *sEditbox=new char[lEditbox];
			    GetWindowText(hEditUpDown,sEditbox,lEditbox);
			    activite[iActiEnCour].placesDispo=atoi(sEditbox);
			    delete[] sEditbox;

			    //actualisation du titre
                int tLen=GetWindowTextLength(hWndMain)+2;
	  		    char *titreFenetre=new char[tLen];
	  		    GetWindowText(hWndMain,titreFenetre,tLen);
	  		    if(titreFenetre[strlen(titreFenetre)-1]!='*')
	  		    {
	  		        strcat(titreFenetre,"*");
				}
	  		    SetWindowText(hWndMain,titreFenetre);
	  		    delete[] titreFenetre;
	  		    ::isFileSaved=false;

                /* TODO (Simon#1#): Modifer activité doit modifier le status de sauvegarde */

			}
			/*if(LOWORD(wParam)==ID_BUTTONRECACTI)
			{
			    COMBOBOXINFO infoCombo;
				infoCombo.cbSize=sizeof(COMBOBOXINFO);
				GetComboBoxInfo(hComboActi,&infoCombo);
			    int lEditbox = GetWindowTextLength(infoCombo.hwndItem)+1;
	 		    if(lEditbox==1)
	 		    {
	 			    MessageBox(hwnd,"Veuillez sélectionner une activité, ou en créer une nouvelle","Pas de nom d'activité",MB_ICONERROR);
	 			    SetFocus(infoCombo.hwndItem);
 			   	    break;
				}
				char *sEditbox =new char[lEditbox];
   				GetWindowText(infoCombo.hwndItem,sEditbox,lEditbox);

   				if(!nouvActi && activite.size()!=0)
   				{
   				    strcpy(activite[iActiEnCour].nom,sEditbox);
   				    delete[] sEditbox;
   				    lEditbox = GetWindowTextLength(hEditUpDown)+1;
   				    sEditbox=new char[lEditbox];
   				    GetWindowText(hEditUpDown,sEditbox,lEditbox);
   				    activite[iActiEnCour].placesDispo=atoi(sEditbox);
   				    delete[] sEditbox;
   				    //jour
   				    int iItem=SendMessage(hComboJour,CB_GETCURSEL,0,0);
			 	    int tLen=SendMessage(hComboJour,CB_GETLBTEXTLEN,iItem,0)+1;
			 	    char *sListbox=new char[tLen];
			 	    SendMessage(hComboJour,CB_GETLBTEXT,iItem,(LPARAM)sListbox);

			 	    string jour[4]={"Lundi","Mardi","Jeudi","Vendredi"};
			 	    for(int i=0;i<4;i++)
			 	    {
	 				    if(strcmp(jour[i].c_str(),sListbox)==0)
	 				    {
	   					    activite[iActiEnCour].jour=i;
	   					    break;
	   					}
					}

			 		delete[] sListbox;
				}
				else
				{
		 		    InfoActivite tempActi;
		 		    strcpy(tempActi.nom,sEditbox);
					delete[] sEditbox;
   				    lEditbox = GetWindowTextLength(hEditUpDown)+1;
   				    sEditbox=new char[lEditbox];
   				    GetWindowText(hEditUpDown,sEditbox,lEditbox);
   				    tempActi.placesDispo=atoi(sEditbox);
		 		    delete[] sEditbox;
		 		    //jour
		 		    int iItem=SendMessage(hComboJour,CB_GETCURSEL,0,0);
			 	    int tLen=SendMessage(hComboJour,CB_GETLBTEXTLEN,iItem,0)+1;
			 	    char *sListbox=new char[tLen];
			 	    SendMessage(hComboJour,CB_GETLBTEXT,iItem,(LPARAM)sListbox);

			 	    string jour[4]={"Lundi","Mardi","Jeudi","Vendredi"};
			 	    for(int i=0;i<4;i++)
			 	    {
	 				    if(strcmp(jour[i].c_str(),sListbox)==0)
	 				    {
	   					    tempActi.jour=i;
	   					    break;
	   					}
					}

			 		delete[] sListbox;
			 		activite.push_back(tempActi);
		 		    ::nouvActi=false;
				}

				int tLen=GetWindowTextLength(hWndMain)+2;
	  		    char *titreFenetre=new char[tLen];
	  		    GetWindowText(hWndMain,titreFenetre,tLen);
	  		    if(titreFenetre[strlen(titreFenetre)-1]!='*')
	  		    {
	  		        strcat(titreFenetre,"*");
				}
	  		    SetWindowText(hWndMain,titreFenetre);
	  		    delete[] titreFenetre;
				/*POINT pos;
    		 	GetCursorPos(&pos);
    		 	ScreenToClient(hwnd,&pos);*/
    		 	//RECT rect={};
    		 	//CreateToolTipForRect(hwnd,"L'activité a bien été enregistrée.");

   				/*int iItemNom=SendMessage(hComboActi,CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)sEditbox);
			    if(iItemNom!=CB_ERR)
				{
		  		    //SendMessage(hComboActi,CB_SETCURSEL,iItemNom,0);
		  		    for(int i=0;i<activite.size();i++)
		  			{
	 				    if(stricmp(activite[i].nom.c_str(),sEditbox)==0)
	 				    {

			 			    //hEditUpDown
			 			}
					}
				}
				else
				{
	   				InfoActivite tempActi;
					activite.push_back(tempActi);
	   				activite[activite.size()-1].nom=sEditbox;
	   				SendMessage(hComboActi,CB_ADDSTRING,(WPARAM) 0,(LPARAM) sEditbox);
				}*//*
			}*/
			if(LOWORD(wParam)==ID_BUTTONCLASSES)
			{
			    if(DialogBox(hinstance, "DIALOGSELECTCLASSE" , 0, (DLGPROC)DialogSelectClasseProc)==IDOK)
                {
                    int tLen=GetWindowTextLength(hWndMain)+2;
                    char *titreFenetre=new char[tLen];
                    GetWindowText(hWndMain,titreFenetre,tLen);
                    if(titreFenetre[strlen(titreFenetre)-1]!='*')
                    {
                        strcat(titreFenetre,"*");
                    }
                    SetWindowText(hWndMain,titreFenetre);
                    delete[] titreFenetre;

                    ::isFileSaved=false;
                }
			}
		 	if(LOWORD(wParam)==ID_BUTTONADD)
		 	{
    		    RECT rectHwnd;
	 	    	GetClientRect(hwnd,&rectHwnd);
			    int i=hComboV1.size();
			    /*char a[3];
			    itoa(nbLigne,a,10);
			    MessageBox(0,a,0,0);*/

			    if(i==0)
			    {
		     	    POINT pt={0,0};
	 		 	    hComboV1.push_back(CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_SORT ,30+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*0-1,30+20,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,(HMENU)ID_COMBOV1,0,0));
					//hCombo[i]=CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST ,10+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*j-1,10+20+23*i,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,0,0,0);
					SendMessage(hComboV1[0],CB_ADDSTRING,(WPARAM) 0,(Button_GetCheck(hCheckboxCasesVide)==BST_CHECKED) ? (LPARAM)"Pas de choix" : (LPARAM)" ");
					for(int j=0;j<activite.size();j++)
					{
					    SendMessage(hComboV1[0],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
					}
					subclass_the_ComboVoeu(hComboV1.back());
					//SendMessage(hCombo[i],CB_SETCURSEL,0,0);

					hComboV2.push_back(CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_SORT,30+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)-1,30+20,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,(HMENU)ID_COMBOV2,0,0));
					//hCombo[i]=CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST ,10+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*j-1,10+20+23*i,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,0,0,0);
					SendMessage(hComboV2[0],CB_ADDSTRING,(WPARAM) 0,(Button_GetCheck(hCheckboxCasesVide)==BST_CHECKED) ? (LPARAM)"Pas de choix" : (LPARAM)" ");
					for(int j=0;j<activite.size();j++)
					{
					    SendMessage(hComboV2[0],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
					}
					subclass_the_ComboVoeu(hComboV2.back());

					hComboV3.push_back(CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_SORT,30+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*2-1,30+20,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3+3,20,hwnd,(HMENU)ID_COMBOV3,0,0));
					SendMessage(hComboV3[0],CB_ADDSTRING,(WPARAM) 0,(Button_GetCheck(hCheckboxCasesVide)==BST_CHECKED) ? (LPARAM)"Pas de choix" : (LPARAM)" ");
					for(int j=0;j<activite.size();j++)
					{
					    SendMessage(hComboV3[0],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
					}
					subclass_the_ComboVoeu(hComboV3.back());

					SendMessage(hListboxNom,LB_ADDSTRING,0,(LPARAM)"");
					SendMessage(hListboxNom,LB_SETITEMDATA,0,0);
					SendMessage(hListboxNom,LB_SETITEMHEIGHT,0,23);

					SendMessage(hListboxClasse,LB_ADDSTRING,0,(LPARAM)"");
					SendMessage(hListboxClasse,LB_SETITEMDATA,0,0);
					SendMessage(hListboxClasse,LB_SETITEMHEIGHT,0,23);

					RECT rect;
					GetWindowRect(hListboxNom,&rect);
					SetWindowPos(hListboxNom,0,0,0,rect.right-rect.left,rect.bottom-rect.top+25,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
					GetWindowRect(hListboxClasse,&rect);
					SetWindowPos(hListboxClasse,0,0,0,rect.right-rect.left,rect.bottom-rect.top+25,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

					GetWindowRect(hButtonAdd,&rect);
					pt.x=rect.left;
					pt.y=rect.top;
					ScreenToClient(hwnd,&pt);
					SetWindowPos(hButtonAdd,0,pt.x,pt.y+23,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
					EnableWindow(hScroll,false);
				}
			    else if(i<nbLigne-1)
			    {
	 			    RECT rectItem;
	 			    POINT pt={0,0};
	 			    GetWindowRect(hComboV1[0],&rectItem);
			 		pt.x=rectItem.left;
	 			    ScreenToClient(hwnd,&pt);

				    hComboV1.push_back(CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_GROUP | CBS_DROPDOWNLIST | CBS_SORT,pt.x,30+20+23*i,rectItem.right-rectItem.left,20,hwnd,(HMENU)ID_COMBOV1,0,0));
					//hCombo[i]=CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST ,10+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*j-1,10+20+23*i,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,0,0,0);
					SendMessage(hComboV1[i],CB_ADDSTRING,(WPARAM) 0,(Button_GetCheck(hCheckboxCasesVide)==BST_CHECKED) ? (LPARAM)"Pas de choix" : (LPARAM)" ");
					for(int j=0;j<activite.size();j++)
					{
					    SendMessage(hComboV1[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
					}
					subclass_the_ComboVoeu(hComboV1.back());
					//SendMessage(hCombo[i],CB_SETCURSEL,0,0);

					GetWindowRect(hComboV2[0],&rectItem);
			 		pt.x=rectItem.left;
	 			    ScreenToClient(hwnd,&pt);

					hComboV2.push_back(CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_SORT,pt.x,30+20+23*i,rectItem.right-rectItem.left,20,hwnd,(HMENU)ID_COMBOV2,0,0));
					//hCombo[i]=CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST ,10+rectHwnd.right*5/24+(((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3)*j-1,10+20+23*i,((rectHwnd.right*5/8)-(rectHwnd.right*5/24))/3,20,hwnd,0,0,0);
					SendMessage(hComboV2[i],CB_ADDSTRING,(WPARAM) 0,(Button_GetCheck(hCheckboxCasesVide)==BST_CHECKED) ? (LPARAM)"Pas de choix" : (LPARAM)" ");
					for(int j=0;j<activite.size();j++)
					{
					    SendMessage(hComboV2[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
					}
					subclass_the_ComboVoeu(hComboV2.back());

					GetWindowRect(hComboV3[0],&rectItem);
			 		pt.x=rectItem.left;
	 			    ScreenToClient(hwnd,&pt);

					hComboV3.push_back(CreateWindowEx(0,"COMBOBOX","Activite",WS_VISIBLE | WS_CHILD | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_SORT,pt.x,30+20+23*i,rectItem.right-rectItem.left,20,hwnd,(HMENU)ID_COMBOV3,0,0));
					SendMessage(hComboV3[i],CB_ADDSTRING,(WPARAM) 0,(Button_GetCheck(hCheckboxCasesVide)==BST_CHECKED) ? (LPARAM)"Pas de choix" : (LPARAM)" ");
					for(int j=0;j<activite.size();j++)
					{
					    SendMessage(hComboV3[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
					}
					subclass_the_ComboVoeu(hComboV3.back());

					SendMessage(hListboxNom,LB_ADDSTRING,0,(LPARAM)"");
					SendMessage(hListboxNom,LB_SETITEMDATA,enfants.size(),enfants.size());
					SendMessage(hListboxNom,LB_SETITEMHEIGHT,enfants.size(),23);

					SendMessage(hListboxClasse,LB_ADDSTRING,0,(LPARAM)"");
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
					EnableWindow(hScroll,false);
				}
				else
				{
		 		    SendMessage(hListboxNom,LB_ADDSTRING,0,(LPARAM)"");
		 		    SendMessage(hListboxClasse,LB_ADDSTRING,0,(LPARAM)"");
		 		    int nbItem=SendMessage(hListboxNom,LB_GETCOUNT,0,0);
					SendMessage(hListboxNom,LB_SETITEMDATA, enfants.size(), enfants.size());
					SendMessage(hListboxNom,LB_SETITEMHEIGHT,enfants.size(),23);

					SendMessage(hListboxClasse,LB_SETITEMDATA,enfants.size(),enfants.size());
					SendMessage(hListboxClasse,LB_SETITEMHEIGHT,enfants.size(),23);


					SCROLLINFO infoScroll;
					infoScroll.cbSize=sizeof(SCROLLINFO);

					//GetScrollInfo(hScroll,SB_CTL,infoScroll);

					infoScroll.fMask=SIF_ALL;
					infoScroll.nMin=0;
					infoScroll.nMax=nbItem;//rectHwnd.bottom-83;
					infoScroll.nPage=nbLigne;//infoScroll.nPage*nbLigne-1/nbLigne;//rectHwnd.bottom-83-22*(nbItem-nbLigne);
					infoScroll.nPos=nbItem;
					SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);
					//char a[3];
					//itoa(SendMessage(hListboxNom,LB_GETTOPINDEX,0,0),a,10);
					//MessageBox(0,a,0,0);
					SendMessage(hListboxNom,LB_SETTOPINDEX,nbItem-1,0);
					SendMessage(hListboxClasse,LB_SETTOPINDEX,nbItem-1,0);
					int iItemTop=SendMessage(hListboxNom,LB_GETTOPINDEX,0,0);

					ActualiserTabCombo(iItemTop-1);

					SendMessage(hComboV1.back(),CB_SELECTSTRING,(WPARAM)-1,(LPARAM)" ");
					SendMessage(hComboV2.back(),CB_SELECTSTRING,(WPARAM)-1,(LPARAM)" ");
					SendMessage(hComboV3.back(),CB_SELECTSTRING,(WPARAM)-1,(LPARAM)" ");
					EnableWindow(hScroll,true);
		 		}
		 		enfantActi tempEnfants;
		 		tempEnfants.nom="";
		 		tempEnfants.classe="";
		 		for(int i=0;i<4;i++)
		 		{
		 		    tempEnfants.voeux[i].voeu1=-1;
			 		tempEnfants.voeux[i].voeu2=-1;
			 		tempEnfants.voeux[i].voeu3=-1;

			 		tempEnfants.affectation[i]=-1;
				}
		 		tempEnfants.ref=SendMessage(hListboxNom,LB_GETCOUNT,0,0)-1;
		 		tempEnfants.point=0;
		 		tempEnfants.denominateur=0;

		 		enfants.push_back(tempEnfants);


				RECT rect;

				UpdateStatusBar(hstatu, enfants, hWndMain);

		 		int tLen=GetWindowTextLength(hWndMain)+2;
	  		    char *titreFenetre=new char[tLen];
	  		    GetWindowText(hWndMain,titreFenetre,tLen);
	  		    if(titreFenetre[strlen(titreFenetre)-1]!='*')
	  		    {
	  		        strcat(titreFenetre,"*");
				}
	  		    SetWindowText(hWndMain,titreFenetre);
	  		    delete[] titreFenetre;

	  		    ::isFileSaved=false;
				/*int firstItem=SendMessage(hListboxNom,LB_GETTOPINDEX,0,0)-1;
				for(int j=0;j<i;j++)
				{
    			    if(enfants[jourEnCourSaisie][firstItem+j].voeu1==-1 && enfants[jourEnCourSaisie][firstItem+j].voeu2==-1 && enfants[jourEnCourSaisie][firstItem+j].voeu3==-1)
    			    {
  					    SendMessage(hListboxNom,LB_GETITEMRECT,firstItem+j,(LPARAM)&rect);
  					    SendMessage(hListboxNom,WM_CHANGECOLOR,(WPARAM)firstItem+j,(LPARAM)&rect);

					}
				}*/
				iItemEdit=SendMessage(hListboxNom,LB_GETCOUNT,0,0)-1;
				SendMessage(hListboxNom,LB_GETITEMRECT,iItemEdit,(LPARAM)&rect);
				hEditList=CreateWindowEx(0,"edit",0,WS_CHILD | WS_VISIBLE | WS_BORDER | ES_WANTRETURN | ES_AUTOHSCROLL ,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,hListboxNom,NULL,0,0);
				subclass_the_edit(hEditList);
				Edit_LimitText(hEditList,66);
				ShowWindow(hEditList,SW_SHOW);
				SetFocus(hEditList);
				hListboxEnCours=hListboxNom;

				SendMessage(hListboxNom,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxNom,LB_GETCOUNT,0,0)));
				SendMessage(hListboxClasse,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxClasse,LB_GETCOUNT,0,0)));

				SendMessage(hListboxNom,LB_SETSEL,true,iItemEdit);
				SendMessage(hListboxClasse,LB_SETSEL,true,iItemEdit);

	 	    }
	 	    if(LOWORD(wParam)==ID_BUTTONNEWACTI)
	 	    {
				if(activite.size()==-1)
				{
	   			    VoirErreur(GetLastError(),(char*)"Erreur lors de la création de l'activité");
	   			    return false;
				}
				::iActiEnCour=activite.size();
			    InfoActivite tempActi;
			    //jour
 	 		    int iItem=SendMessage(hComboJour,CB_GETCURSEL,0,0);
		 	    int tLen=SendMessage(hComboJour,CB_GETLBTEXTLEN,iItem,0)+1;
		 	    char *sListbox=new char[tLen];
		 	    SendMessage(hComboJour,CB_GETLBTEXT,iItem,(LPARAM)sListbox);

		 	    string jour[4]={"Lundi","Mardi","Jeudi","Vendredi"};
		 	    for(int i=0;i<4;i++)
		 	    {
 				    if(strcmp(jour[i].c_str(),sListbox)==0)
 				    {
   					    tempActi.jour=i;
   					    break;
   					}
				}
		 		delete[] sListbox;

		 		if(tempActi.jour<0)
                {
                    tempActi.jour=0;
                }

			    //places
			    int lEditbox = GetWindowTextLength(hEditUpDown)+1;
			    char *sEditbox=new char[lEditbox];
			    GetWindowText(hEditUpDown,sEditbox,lEditbox);
			    tempActi.placesDispo=atoi(sEditbox);
			    delete[] sEditbox;
			    if(tempActi.placesDispo<0)
                {
                    tempActi.placesDispo=0;
                }

				activite.push_back(tempActi);
			    COMBOBOXINFO infoCombo;
				infoCombo.cbSize=sizeof(COMBOBOXINFO);
				GetComboBoxInfo(hComboActi,&infoCombo);
			    SetFocus(infoCombo.hwndItem);
			    ::nouvActi=true;

			    tLen=GetWindowTextLength(hWndMain)+2;
	  		    char *titreFenetre=new char[tLen];
	  		    GetWindowText(hWndMain,titreFenetre,tLen);
	  		    if(titreFenetre[strlen(titreFenetre)-1]!='*')
	  		    {
	  		        strcat(titreFenetre,"*");
				}
	  		    SetWindowText(hWndMain,titreFenetre);
	  		    delete[] titreFenetre;
	  		    ::isFileSaved=false;
			}
		 	if(HIWORD(wParam)==LBN_DBLCLK && LOWORD(wParam)==ID_LISTBOX)
		 	{
                /* DONE (Simon#1#): Modifier la subclass du hEditList */

    		    RECT rect;
   		     	POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(hListboxNom,&pt);
				iItemEdit=SendMessage(hListboxNom,LB_ITEMFROMPOINT,0,MAKELPARAM(pt.x,pt.y));
				SendMessage(hListboxNom,LB_GETITEMRECT,iItemEdit,(LPARAM)&rect);
				//char a[4];
				//itoa(rect.bottom,a,10);
				char *contenuLigne=new char[SendMessage(hListboxNom,LB_GETTEXTLEN,iItemEdit,0)];
				SendMessage(hListboxNom,LB_GETTEXT,iItemEdit,(LPARAM)contenuLigne);
				//MessageBox(0,a,0,0);
				hEditList=CreateWindowEx(0,"edit",contenuLigne,WS_CHILD | WS_VISIBLE | WS_BORDER | ES_WANTRETURN | ES_AUTOHSCROLL,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,hListboxNom,NULL,0,0);
				Edit_LimitText(hEditList,66);
				subclass_the_edit(hEditList);
				ShowWindow(hEditList,SW_SHOW);
				SetFocus(hEditList);
				delete[] contenuLigne;
				hListboxEnCours=hListboxNom;
			}
			if(HIWORD(wParam)==LBN_DBLCLK && LOWORD(wParam)==ID_LISTBOXCLASSE)
		 	{
                /* DONE (Simon#1#): Modifier la subclass du hEditList */
    		    RECT rect;
   		     	POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(hListboxClasse,&pt);
				iItemEdit=SendMessage(hListboxClasse,LB_ITEMFROMPOINT,0,MAKELPARAM(pt.x,pt.y));
				SendMessage(hListboxClasse,LB_GETITEMRECT,iItemEdit,(LPARAM)&rect);
				//char a[4];
				//itoa(rect.bottom,a,10);
				char *contenuLigne=new char[SendMessage(hListboxClasse,LB_GETTEXTLEN,iItemEdit,0)];
				SendMessage(hListboxClasse,LB_GETTEXT,iItemEdit,(LPARAM)contenuLigne);
				//MessageBox(0,a,0,0);
				hEditList=CreateWindowEx(0,"edit",contenuLigne,WS_CHILD | WS_VISIBLE | WS_BORDER | ES_WANTRETURN | ES_AUTOHSCROLL,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,hListboxClasse,NULL,0,0);
				Edit_LimitText(hEditList,34);
				subclass_the_edit(hEditList);
				ShowWindow(hEditList,SW_SHOW);
				SetFocus(hEditList);
				delete[] contenuLigne;
				hListboxEnCours=hListboxClasse;
			}
			if(HIWORD(wParam)==LBN_SELCHANGE && LOWORD(wParam)==ID_LISTBOX)
			{
		 	    SCROLLINFO infoScroll;
		 	    infoScroll.cbSize=sizeof(infoScroll);
		 	    infoScroll.fMask=SIF_ALL;
 				GetScrollInfo(hScroll,SB_CTL,&infoScroll);
				infoScroll.nPos=SendMessage(hListboxNom,LB_GETTOPINDEX,0,0);
				infoScroll.fMask = SIF_POS;
				SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);
				if(enfants.size()<nbLigne)
				{
				    EnableWindow(hScroll,false);
				}
				else
				{
		 		    SendMessage(hListboxClasse,LB_SETTOPINDEX,infoScroll.nPos,0);
		 		    ActualiserTabCombo(infoScroll.nPos);
				}
				/* DONE (Simon#1#): Modifier LB_SETCURSEL
				                    partout */
                SendMessage(hListboxClasse,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxClasse,LB_GETCOUNT,0,0)));
                int *selected=new int(SendMessage(hListboxNom,LB_GETSELCOUNT,0,0)+1);
                SendMessage(hListboxNom,LB_GETSELITEMS,SendMessage(hListboxNom,LB_GETSELCOUNT,0,0),(LPARAM)selected);
                for(int i=0;i<SendMessage(hListboxNom,LB_GETSELCOUNT,0,0);i++)
                {
                    SendMessage(hListboxClasse,LB_SETSEL,true,selected[i]);
                }
                delete[] selected;

                if(SendMessage(hListboxNom,LB_GETSELCOUNT,0,0)>1)
                {
        		    UpdateStatusBar(hstatu, hListboxNom);
                }
                else
                {
                    UpdateStatusBar(hstatu, enfants, hWndMain);
                }
                /*else
                {
                    SendMessage(hListboxClasse,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxClasse,LB_GETCOUNT,0,0)));
                    SendMessage(hListboxClasse,LB_SETSEL,true,SendMessage(hListboxNom,LB_GETCURSEL,0,0));
                }*/
			}
			if(HIWORD(wParam)==LBN_SELCHANGE && LOWORD(wParam)==ID_LISTBOXCLASSE)
			{
		 	    SCROLLINFO infoScroll;
		 	    infoScroll.cbSize=sizeof(infoScroll);
		 	    infoScroll.fMask=SIF_ALL;
 				GetScrollInfo(hScroll,SB_CTL,&infoScroll);
				infoScroll.nPos=SendMessage(hListboxClasse,LB_GETTOPINDEX,0,0);
				infoScroll.fMask = SIF_POS;
				SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);
				if(enfants.size()<nbLigne)
				{
				    EnableWindow(hScroll,false);
				}
				else
				{
		 		    SendMessage(hListboxNom,LB_SETTOPINDEX,infoScroll.nPos,0);
		 		    ActualiserTabCombo(infoScroll.nPos);
				}
				SendMessage(hListboxNom,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxNom,LB_GETCOUNT,0,0)));
                int *selected=new int(SendMessage(hListboxClasse,LB_GETSELCOUNT,0,0)+1);
                SendMessage(hListboxClasse,LB_GETSELITEMS,SendMessage(hListboxClasse,LB_GETSELCOUNT,0,0),(LPARAM)selected);
                for(int i=0;i<SendMessage(hListboxClasse,LB_GETSELCOUNT,0,0);i++)
                {
                    SendMessage(hListboxNom,LB_SETSEL,true,selected[i]);
                }
                delete[] selected;

                if(SendMessage(hListboxNom,LB_GETSELCOUNT,0,0)>1)
                {
        		    UpdateStatusBar(hstatu, hListboxNom);
                }
                else
                {
                    UpdateStatusBar(hstatu, enfants, hWndMain);
                }
			}
			if(HIWORD(wParam)==CBN_SETFOCUS && LOWORD(wParam)==ID_COMBOAPPCLASS)
			{
			    RemplirListeClasse(enfants, activite, nomClasses);
			    ComboBox_ResetContent(hComboAjoutClasse);
			    for(int i=0;i<nomClasses.size();i++)
			    {
 				    SendMessage(hComboAjoutClasse,CB_ADDSTRING,(WPARAM) 0,(LPARAM)nomClasses[i].c_str());
				}
			    RECT rect;
				GetWindowRect(hComboAjoutClasse,&rect);
			    SetWindowPos(hComboAjoutClasse,0,0,0,rect.right-rect.left,50,SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
			}
			if(HIWORD(wParam)==CBN_KILLFOCUS && LOWORD(wParam)==ID_COMBOAPPCLASS)
			{
			    RECT rect;
				GetWindowRect(hComboAjoutClasse,&rect);
			    SetWindowPos(hComboAjoutClasse,0,0,0,rect.right-rect.left,20,SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
			}
			if(HIWORD(wParam)==CBN_CLOSEUP)
			{
				if (SendMessage((HWND)lParam,CB_GETCURSEL,0,0)==-1)
				{
 				    SendMessage((HWND)lParam,CB_SETCURSEL,(WPARAM)iItemSel,0);
 				    return 0;
				}

				int tLen=GetWindowTextLength(hWndMain)+2;
	  		    char *titreFenetre=new char[tLen];
	  		    GetWindowText(hWndMain,titreFenetre,tLen);
	  		    if(titreFenetre[strlen(titreFenetre)-1]!='*')
	  		    {
	  		        strcat(titreFenetre,"*");
				}
	  		    SetWindowText(hWndMain,titreFenetre);
	  		    delete[] titreFenetre;
	  		    ::isFileSaved=false;
            }
			if(HIWORD(wParam)==CBN_SELCHANGE && (HWND)lParam==hComboActi)
			{
   			    int iItem=SendMessage(hComboActi,CB_GETCURSEL,0,0);
		 	    /*int tLen=SendMessage(hComboActi,CB_GETLBTEXTLEN,iItem,0)+1;
		 	    char *sListbox=new char[tLen];
		 	    SendMessage(hComboActi,CB_GETLBTEXT,iItem,(LPARAM)sListbox);
		 	    for(int i=0;i<activite.size();i++)
		 	    {
   				    if(strcmp(activite[i].nom,sListbox)==0)//stricmp
   				    {
					    ::iActiEnCour=i;
					}
				}*/
				::iActiEnCour=int(ComboBox_GetItemData(hComboActi,iItem));
				string sPlacesDispo=to_string(activite[iActiEnCour].placesDispo);
				SetWindowText(hEditUpDown,sPlacesDispo.c_str());
				string jour[4]={"Lundi","Mardi","Jeudi","Vendredi"};
				ComboBox_SelectString(hComboJour,-1,jour[(activite[iActiEnCour].jour<0)? 0 : activite[iActiEnCour].jour].c_str());
				::nouvActi=false;
		 		//delete[] sListbox;
			}
			else if(HIWORD(wParam)==CBN_SELCHANGE && (HWND)lParam==hComboJour)
			{
 	 		    if(activite.size()==0)return 0;
 	 		    int iItem=SendMessage(hComboJour,CB_GETCURSEL,0,0);
		 	    int tLen=SendMessage(hComboJour,CB_GETLBTEXTLEN,iItem,0)+1;
		 	    char *sListbox=new char[tLen];
		 	    SendMessage(hComboJour,CB_GETLBTEXT,iItem,(LPARAM)sListbox);

		 	    string jour[4]={"Lundi","Mardi","Jeudi","Vendredi"};
		 	    for(int i=0;i<4;i++)
		 	    {
 				    if(strcmp(jour[i].c_str(),sListbox)==0)
 				    {
   					    activite[iActiEnCour].jour=i;
   					    break;
   					}
				}

		 		delete[] sListbox;
			}
			else if(HIWORD(wParam)==CBN_SELCHANGE && LOWORD(wParam)==ID_COMBOV1)
			{
 	 		    for(int i=0;i<hComboV1.size();i++)
 	 		    {
 				    if(hComboV1[i]==HWND(lParam))
   					{
		 			    int enfantChoisi=SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)SendMessage(hListboxNom,LB_GETTOPINDEX,0,0)+i,0);
		 			    int tLen=ComboBox_GetTextLength(hComboV1[i])+1;
		 	    		char *sListbox=new char[tLen];
		 			    ComboBox_GetText(hComboV1[i],sListbox,tLen);
		 			    if(tLen==1 || strcmp(sListbox,"Pas de choix" )==0 || strcmp(sListbox," " )==0)
		 			    {
				 		    enfants[enfantChoisi].voeux[jourEnCourSaisie].voeu1=-1;
						}
						else
						{
			 			    for(int j=0;j<activite.size();j++)
			 			    {
	 						    if(activite[j].nom==sListbox)
	 						    {
						   		    enfants[enfantChoisi].voeux[jourEnCourSaisie].voeu1=j;
						   		    break;
						   		}
						    }
						}
						delete[] sListbox;
		 			}
				}
		 		UpdateStatusBar(hstatu, enfants, hWndMain);
			}
			else if(HIWORD(wParam)==CBN_SELCHANGE && LOWORD(wParam)==ID_COMBOV2)
			{
 	 		    for(int i=0;i<hComboV2.size();i++)
 	 		    {
 				    if(hComboV2[i]==HWND(lParam))
   					{
		 			    int enfantChoisi=SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)SendMessage(hListboxNom,LB_GETTOPINDEX,0,0)+i,0);
		 			    int tLen=ComboBox_GetTextLength(hComboV2[i])+1;
		 	    		char *sListbox=new char[tLen];
		 			    ComboBox_GetText(hComboV2[i],sListbox,tLen);
		 			    if(tLen==1 || strcmp(sListbox,"Pas de choix")==0 || strcmp(sListbox," " )==0)
		 			    {
				 		    enfants[enfantChoisi].voeux[jourEnCourSaisie].voeu2=-1;
						}
						else
						{
			 			    for(int j=0;j<activite.size();j++)
			 			    {
	 						    if(activite[j].nom==sListbox)
	 						    {
						   		    enfants[enfantChoisi].voeux[jourEnCourSaisie].voeu2=j;
						   		    break;
						   		}
						    }
						}
						delete[] sListbox;
		 			}
				}
		 		UpdateStatusBar(hstatu, enfants, hWndMain);
			}
			else if(HIWORD(wParam)==CBN_SELCHANGE && LOWORD(wParam)==ID_COMBOV3)
			{
 	 		    for(int i=0;i<hComboV3.size();i++)
 	 		    {
 				    if(hComboV3[i]==HWND(lParam))
   					{
		 			    int enfantChoisi=SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)SendMessage(hListboxNom,LB_GETTOPINDEX,0,0)+i,0);
		 			    int tLen=ComboBox_GetTextLength(hComboV3[i])+1;
		 	    		char *sListbox=new char[tLen];
		 			    ComboBox_GetText(hComboV3[i],sListbox,tLen);
		 			    if(tLen==1 || strcmp(sListbox,"Pas de choix")==0 || strcmp(sListbox," " )==0)
		 			    {
				 		    enfants[enfantChoisi].voeux[jourEnCourSaisie].voeu3=-1;
						}
						else
						{
			 			    for(int j=0;j<activite.size();j++)
			 			    {
	 						    if(activite[j].nom==sListbox)
	 						    {
						   		    enfants[enfantChoisi].voeux[jourEnCourSaisie].voeu3=j;
						   		}
						    }
						}
						delete[] sListbox;
		 			}
				}
		 		UpdateStatusBar(hstatu, enfants, hWndMain);
			}
			if(HIWORD(wParam)==CBN_DROPDOWN && (HWND)lParam!=hComboActi && (HWND)lParam!=hComboJour && (HWND)lParam!=hComboAjoutClasse)
			{
			    ::iItemSel=SendMessage((HWND)lParam,CB_GETCURSEL,0,0);
			    ComboBox_ResetContent((HWND)lParam);
			    SendMessage((HWND)lParam,CB_ADDSTRING,(WPARAM) 0,(Button_GetCheck(hCheckboxCasesVide)==BST_CHECKED) ? (LPARAM)"Pas de choix" : (LPARAM)" ");

			   	int enfantChoisi=0;
			   	for(int i=0;i<hComboV1.size();i++)
 	 		    {
 				    if(hComboV1[i]==HWND(lParam) || hComboV2[i]==HWND(lParam) || hComboV3[i]==HWND(lParam))
   					{
		 			    enfantChoisi=SendMessage(hListboxNom,LB_GETTOPINDEX,0,0)+i;
		 			    break;
		 			}
				}

   			    //size_t found=0;
			    //size_t foundNew=0;
			    //bool classeNonPresente=true;
			    string str;
			    for(int i=0;i<activite.size();i++)
				{
				    //found=0;
				    //foundNew=0;
					//foundNew=activite[i].classe.find('\t',found);
					str=enfants[enfantChoisi].classe+"\t";
					if(activite[i].classe.find(str)!=std::string::npos && activite[i].jour==jourEnCourSaisie)
					{
					    SendMessage((HWND)lParam,CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[i].nom.c_str());
					}
					/*while(foundNew!=std::string::npos)
					{
						str=activite[i].classe.substr(found,foundNew-found);
						if(str.compare(enfants[jourEnCourSaisie][enfantChoisi].classe)==0 && activite[i].jour==jourEnCourSaisie)
						{
				  		    SendMessage((HWND)lParam,CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[i].nom);
				  		    break;
						}
						found=foundNew+1;
						foundNew=activite[i].classe.find('\t',found);
					}*/
				}//fin for activite
			}

			if(HIWORD(wParam)==CBN_DROPDOWN && (HWND)lParam==hComboActi)
			{
			    ::iItemSel=(SendMessage((HWND)lParam,CB_GETCURSEL,0,0)<0)? 0: SendMessage((HWND)lParam,CB_GETCURSEL,0,0);
			    //MessageBox(0,to_string(iItemSel).c_str(),0,0);
			    ComboBox_ResetContent((HWND)lParam);
			    int iItemAdd;
			    for(int i=0;i<activite.size();i++)
				{
				    iItemAdd=SendMessage((HWND)lParam,CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[i].nom.c_str());
				    ComboBox_SetItemData((HWND)lParam,iItemAdd,(LPARAM)i);
				}
                ComboBox_SetCurSel((HWND)lParam, iItemSel);
			}

			if(LOWORD(wParam)==ID_CHANGEITEMTABPLUS)
			{
			    if(TabCtrl_GetCurSel(hTabs)==3)
			    {
			        TabCtrl_SetCurSel(hTabs,0);
				}
				else
				{
		 		    TabCtrl_SetCurSel(hTabs,TabCtrl_GetCurSel(hTabs)+1);
				}
				NMHDR nmHdr={hTabs,0,TCN_SELCHANGE};
				SendMessage(hwnd,WM_NOTIFY,(WPARAM)NULL,(LPARAM)&nmHdr);
			}
			if(LOWORD(wParam)==ID_CHANGEITEMTABMOINS)
			{
		 	    if(TabCtrl_GetCurSel(hTabs)==0)
			    {
			        TabCtrl_SetCurSel(hTabs,3);
				}
				else
				{
		 		    TabCtrl_SetCurSel(hTabs,TabCtrl_GetCurSel(hTabs)-1);
				}
				NMHDR nmHdr={hTabs,0,TCN_SELCHANGE};
				SendMessage(hwnd,WM_NOTIFY,(WPARAM)NULL,(LPARAM)&nmHdr);
			}

			//######################################################### MENU ###############################################################

			///Menu Fichier
			if(LOWORD(wParam)==IDM_OPEN)
			{
			    /*int tLen=GetWindowTextLength(hWndMain)+2;
	  		    char *titreFenetre=new char[tLen];
	  		    GetWindowText(hWndMain,titreFenetre,tLen);
	  		    if(titreFenetre[strlen(titreFenetre)-1]=='*')
	  		    {
	  		        if(MessageBox(hwnd,"Votre travail n'a pas été enregistré.\n\
Etes vous sur de vouloir ouvrir un autre tableau?","Fichier non sauvegardé",MB_YESNO | MB_ICONWARNING)!=IDYES){return 0;}
   		  	    }
   		  	    delete[] titreFenetre;*/
   		  	    if(!isFileSaved)
                {
                    if(MessageBox(hwnd,"Votre travail n'a pas été enregistré.\n\
Etes vous sur de vouloir ouvrir un autre tableau?","Fichier non sauvegardé",MB_YESNO | MB_ICONWARNING)!=IDYES){return 0;}
                }


				//FILE * pFile;
			    char pathFile[MAX_PATH]={0};
	 	 	    OPENFILENAME ofn;
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = pathFile;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFilter =
				"Fichiers GroupManage (*.grma;*.grma2)\0*.grma;*.grma2\0";
				ofn.nFilterIndex = 1;
				ofn.Flags =
				OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
				if (GetOpenFileName(&ofn)==TRUE)
				{
				    ::traitementLance=false;
				    string sBackUp;
				    if(pathFileOpen!="")
					{
						sBackUp=pathFileOpen;
			   			size_t found = sBackUp.rfind('\\');
					    sBackUp.replace (found+1,2,"~&");
						if(access(sBackUp.c_str(),F_OK)!=-1)
						{
						    remove(sBackUp.c_str());
						}
					}
					sBackUp="~&Nouveau document GRMA.grma";
					if(access(sBackUp.c_str(),F_OK)!=-1)
					{
					    remove(sBackUp.c_str());
					}
					sBackUp="~&Nouveau document GRMA.grma2";
					if(access(sBackUp.c_str(),F_OK)!=-1)
					{
					    remove(sBackUp.c_str());
					}
					else
					{
					 	char szPath[MAX_PATH] = "";
				    	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szPath);
				    	sBackUp=string(szPath)+"\\~&Nouveau document GRMA.grma";
				    	if(access(sBackUp.c_str(),F_OK)!=-1)
						{
						    remove(sBackUp.c_str());
						}
						sBackUp=string(szPath)+"\\~&Nouveau document GRMA.grma2";
				    	if(access(sBackUp.c_str(),F_OK)!=-1)
						{
						    remove(sBackUp.c_str());
						}
					}
					if(ofn.Flags & OFN_READONLY)
					{
                        ::lectureSeule=true;
                    }
					pathFileOpen=string(pathFile);
				    OuvrirFeuille(hwnd);
				    ::isFileSaved=true;
				}
				return 0;
			}
			if(LOWORD(wParam)==IDM_SAVEAS && !traitementLance)
			{
			    char pathFile[MAX_PATH]={0};
	 	 	    OPENFILENAME ofn;
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = pathFile;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFilter =
				"Fichiers GroupManage version 3.4 et +  (*.grma2)\0*.grma2\0\
Fichiers GroupManage version 3.3 et +  (*.grma)\0*.grma\0\
Fichiers GroupManage version 2.2 et +  (*.grma)\0*.grma\0";
				ofn.nFilterIndex = 1;
				ofn.Flags =
				OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
				if (GetSaveFileName(&ofn)==TRUE)
				{
                    switch (ofn.nFilterIndex)
                    {
                        case 1:
                        {
                            if(ofn.nFileExtension)
				            {
                                string ext=string(pathFile).substr(ofn.nFileExtension);
                                if(ext!="grma2")
        				        {
        	 			   		    string spath=string(pathFile).substr(0,ofn.nFileExtension)+"grma2";
        		 				    strcpy(pathFile,spath.c_str());
        	 			   		}
                            }
                            else
        				    {
        	   				    strcat(pathFile,".grma2");
        					}
                        }
                        case 2:
                        {
                            if(ofn.nFileExtension)
				            {
                                string ext=string(pathFile).substr(ofn.nFileExtension);
                                if(ext!="grma")
        				        {
        	 			   		    string spath=string(pathFile).substr(0,ofn.nFileExtension)+"grma";
        		 				    strcpy(pathFile,spath.c_str());
        	 			   		}
                            }
                            else
        				    {
        	   				    strcat(pathFile,".grma");
        					}
                        }
                        case 3:
                        {
                            if(ofn.nFileExtension)
				            {
                                string ext=string(pathFile).substr(ofn.nFileExtension);
                                if(ext!="grma")
        				        {
        	 			   		    string spath=string(pathFile).substr(0,ofn.nFileExtension)+"grma";
        		 				    strcpy(pathFile,spath.c_str());
        	 			   		}
                            }
                            else
        				    {
        	   				    strcat(pathFile,".grma");
        					}
                        }
                    }
				    /*if(ofn.nFileExtension)
				    {
			     	    string ext=string(pathFile).substr(ofn.nFileExtension);
						if(ext!="grma")
				        {
	 			   		    string spath=string(pathFile).substr(0,ofn.nFileExtension)+"grma";
		 				    strcpy(pathFile,spath.c_str());
	 			   		}
					}
				    if(!ofn.nFileExtension)
				    {
	   				    strcat(pathFile,".grma");
					}*/

					try
					{
				 	    SauvegardeFeuille(pathFile, enfants, activite, ofn.nFilterIndex);
                    }
                    catch(myExeptions::Erreur er)
                    {
                        er.AfficheMessage(hwnd, "Erreur lors de l'écriture dans le fichier");
                    }
				    /*ofstream FichierOut(pathFile,ios::trunc);
				    if(!FichierOut)
					{
	   				    VoirErreur(GetLastError(),"Erreur lors de l'écriture dans le fichier");
	 			   	    return false;
					}

					FichierOut<<"[Fichier GRMA]"<<endl;
					FichierOut<<"[Informations activités]"<<endl;

					for(int i=0;i<activite.size();i++)
			     	{
			     	    //fwrite(&activite[i],sizeof(InfoActivite),1,pFile);
			     	    FichierOut<<activite[i].nom<<endl<<activite[i].classe<<endl<<activite[i].placesDispo<<endl<<activite[i].jour<<endl<<endl;
					}
					FichierOut<<endl<<"[Informations élèves]"<<endl<<endl;

					for(int i=0;i<enfants.size();i++)
					{
		 			    FichierOut<<enfants[i].nom<<endl<<enfants[i].classe<<endl;
		 			    for(int jour=0;jour<4;jour++)
		 			    {
						    FichierOut<<enfants[i].voeux[jour].voeu1<<" "<<enfants[i].voeux[jour].voeu2<<" "<<enfants[i].voeux[jour].voeu3<<endl;
						}
						FichierOut<<endl;
					}
				    FichierOut.close();*/

					char titre[101];
					strcpy(titre,"Gestion des groupes - ");
					strcat(titre,pathFile);
					SetWindowText(hwnd,titre);
					pathFileOpen=string(pathFile);
					::backUpActivite=1;
					::isFileSaved=true;
				}

			}
			if(LOWORD(wParam)==IDM_SAVE && !traitementLance)
			{
			    if(pathFileOpen=="")
			    {
	  			    SendMessage(hwnd,WM_COMMAND,LOWORD(IDM_SAVEAS),0);
	  			    return 0;
	  			}

	  			//attributs
				WIN32_FILE_ATTRIBUTE_DATA InfoFichier;
                ZeroMemory (&InfoFichier, sizeof(WIN32_FILE_ATTRIBUTE_DATA));

                if (GetFileAttributesEx( pathFileOpen.c_str(), GetFileExInfoStandard, &InfoFichier ) || lectureSeule)
                {
                    if((InfoFichier.dwFileAttributes & FILE_ATTRIBUTE_READONLY) || lectureSeule)
                    {
                        SendMessage(hwnd,WM_COMMAND,LOWORD(IDM_SAVEAS),0);
	  			        return 0;
                    }

                }

                DWORD nFilter=1;
                if(pathFileOpen[pathFileOpen.size()-1]=='a')nFilter=2;

	  			//ouverture
	  			try
				{
			 	    SauvegardeFeuille(pathFileOpen, enfants, activite, nFilter);
                }
                catch(myExeptions::Erreur er)
                {
                    er.AfficheMessage(hwnd, "Erreur lors de l'écriture dans le fichier");
                }

				int tLen=GetWindowTextLength(hWndMain)+1;
	  		    char *titreFenetre=new char[tLen];
	  		    GetWindowText(hWndMain,titreFenetre,tLen);
	  		    if(titreFenetre[strlen(titreFenetre)-1]=='*')
	  		    {
	   			    titreFenetre[strlen(titreFenetre)-1]=' ';
	  		        SetWindowText(hWndMain,titreFenetre);
				}
	  		    delete[] titreFenetre;
	  		    ::isFileSaved=true;
	  		    ::backUpActivite=1;
			}
			if(LOWORD(wParam)==IDM_SAVEASTABL && !traitementLance)
			{
			    char pathFile[MAX_PATH]={0};
	 	 	    OPENFILENAME ofn;
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = pathFile;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFilter =
				"Fichiers texte (*.txt)\0*.txt\0";
				ofn.nFilterIndex = 1;
				ofn.Flags =
				OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
				if (GetSaveFileName(&ofn)==TRUE)
				{
				    if(ofn.nFileExtension)
				    {
			     	    string ext=string(pathFile).substr(ofn.nFileExtension);
						if(ext!="txt")
				        {
	 			   		    string spath=string(pathFile).substr(0,ofn.nFileExtension)+"txt";
		 				    strcpy(pathFile,spath.c_str());
	 			   		}
					}

				    if(!ofn.nFileExtension)
				    {
	   				    strcat(pathFile,".txt");
					}

					ofstream FichierOut(pathFile,ios::trunc);
				    if(!FichierOut)
					{
	   				    VoirErreur(GetLastError(),(char*)"Erreur lors de l'écriture dans le fichier");
	 			   	    return false;
					}
				    FichierOut<<"Nom, Prénom\tClasse\tVoeu 1 Lundi\tVoeu 2 Lundi\tVoeu 3 Lundi\t\t\tVoeu 1 Mardi\tVoeu 2 Mardi\tVoeu 3 Mardi\t\t\tVoeu 1 Jeudi\tVoeu 2 Jeudi\tVoeu 3 Jeudi\t\t\tVoeu 1 Vendredi\tVoeu 2 Vendredi\tVoeu 3 Vendredi"<<endl;
 				    for(int i=0;i<enfants.size();i++)
 				    {
		     	        FichierOut<<enfants[i].nom<<"\t"<<enfants[i].classe<<"\t";
		     	        for(int jour=0;jour<4;jour++)
		     	        {
		     	            FichierOut<<((enfants[i].voeux[jour].voeu1!=-1) ? activite[enfants[i].voeux[jour].voeu1].nom : " ");
				 			FichierOut<<"\t"<<((enfants[i].voeux[jour].voeu2!=-1) ? activite[enfants[i].voeux[jour].voeu2].nom : " ");
				 			FichierOut<<"\t"<<((enfants[i].voeux[jour].voeu3!=-1) ? activite[enfants[i].voeux[jour].voeu3].nom : " ");
				 			FichierOut<<"\t\t\t";
						}
						FichierOut<<endl;
					}
					FichierOut.close();
				}
	  		}
			if(LOWORD(wParam)==IDM_OPENACTI)
			{

			    char pathFile[MAX_PATH]={0};
	 	 	    OPENFILENAME ofn;
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = pathFile;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFilter =
				"Fichiers GroupManage (*.eac)\0*.eac\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrTitle="Ouvrir une liste d'activités...";
				ofn.Flags =
				OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
				if (GetOpenFileName(&ofn)==TRUE)
				{
				    string sBackUp;
				    size_t found;
					if(actiFileOpen!="")
					{
						sBackUp=actiFileOpen;
			   			found = sBackUp.rfind('\\');
					    sBackUp.replace(found+1,2,"~&");

					    if(access(sBackUp.c_str(),F_OK)!=-1)
					    {
						    remove(sBackUp.c_str());
						}
					}

					actiFileOpen=string(pathFile);

					OuvrirActivite(hwnd);

				}
				return 0;
			}
			if(LOWORD(wParam)==IDM_SAVEACTIAS)
			{
			    char pathFile[MAX_PATH]={0};
	 	 	    OPENFILENAME ofn;
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hwnd;
				ofn.lpstrFile = pathFile;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFilter =
				"Fichiers GroupManage Compatibles 3.4 et +(*.eac)\0*.eac\0\
Fichiers GroupManage Compatibles 2.2 et + (*.eac)\0*.eac\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrTitle="Enregistrer la liste d'activité sous...";
				ofn.Flags =
				OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
				if (GetSaveFileName(&ofn)==TRUE)
				{
                    if(ofn.nFileExtension)
		            {
                        string ext=string(pathFile).substr(ofn.nFileExtension);
                        if(ext!="eac")
				        {
	 			   		    string spath=string(pathFile).substr(0,ofn.nFileExtension)+"eac";
		 				    strcpy(pathFile,spath.c_str());
	 			   		}
                    }
                    else
				    {
	   				    strcat(pathFile,".eac");
					}

					//ouverture
					try
                    {
                        SauvegardeActivite(pathFile, activite, ofn.nFilterIndex);
                    }
                    catch(myExeptions::Erreur er)
                    {
                        er.AfficheMessage(hwnd, "Erreur lors de l'écriture dans le fichier");
                    }

					/*ofstream FichierOut(pathFile,ios::trunc);
				    if(!FichierOut)
					{
					    VoirErreur(GetLastError(),"Erreur lors de l'écriture dans le fichier");
				   	    return false;
					}

					FichierOut<<"[Fichier EAC]"<<endl;
					FichierOut<<"[Informations activités]"<<endl;

					for(int i=0;i<activite.size();i++)
			     	{
			     	    FichierOut<<activite[i].nom<<endl<<activite[i].classe<<endl<<activite[i].placesDispo<<endl<<activite[i].jour<<endl<<endl;
					}

					FichierOut.close();*/

					actiFileOpen=string(pathFile);
					::backUpActivite=0;
				}
	  		}
			if(LOWORD(wParam)==IDM_SAVEACTI)
			{
			    if(actiFileOpen=="")
			    {
	  			    SendMessage(hwnd,WM_COMMAND,LOWORD(IDM_SAVEACTIAS),0);
	  			    return 0;
	  			}

	  			//attributs
				WIN32_FILE_ATTRIBUTE_DATA InfoFichier;
                ZeroMemory (&InfoFichier, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
                if (GetFileAttributesEx( pathFileOpen.c_str(), GetFileExInfoStandard, &InfoFichier ))
                {
                    if(InfoFichier.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
                    {
                        SendMessage(hwnd,WM_COMMAND,LOWORD(IDM_SAVEACTIAS),0);
	  			        return 0;
                    }

                }

                //ouverture

                try
                {
                    SauvegardeActivite(actiFileOpen, activite);
                }
                catch(myExeptions::Erreur er)
                {
                    er.AfficheMessage(hwnd, "Erreur lors de l'écriture dans le fichier");
                }

				::backUpActivite=0;
			}

			if(LOWORD(wParam)==IDM_QUIT)
			{
	  		    SendMessage(hwnd,WM_CLOSE,0,0);
			}

			if(LOWORD(wParam)==IDM_NEW)
			{
                string pathGM="";
                char buffer[MAX_PATH];
               	GetModuleFileName(NULL, buffer, sizeof(buffer));
               	pathGM=string(buffer)+"\0";
                ShellExecute(hwnd, "open", pathGM.c_str(), NULL, NULL, SW_SHOW);
            }

			///Menu Edition
			if(LOWORD(wParam)==IDM_FINDNAME)
			{
			    DialogBox(hinstance, "DIALOGFINDNAME" , hwnd, (DLGPROC)DialogFindNameProc);
			}

			if(LOWORD(wParam)==IDM_GOLINE)
			{
	  		    DialogBox(hinstance, "DIALOGGOLINE" , hwnd, (DLGPROC)DialogGoLineProc);
			}

			if(LOWORD(wParam)==IDM_PASTE)
			{
		 		string contenu=GetClipboardText();
		 	    if(contenu=="")return 0;
		 	    int retourDialog=DialogBox(hinstance, "DIALOGCOLLER" , hwnd, (DLGPROC)DialogVerifPlacesProc);
			    if(retourDialog==IDCANCEL)
			    {
				    return 0;
				}

		        size_t nbLigneColler=0, found=0;

		        /*for(;found!=string::npos;++nbLigneColler)
                {
                    found=contenu.find('\n', found+1);
                }*/
                nbLigneColler=count(contenu.begin(), contenu.end(), '\n');

		        found=0;

		        vector<string> tabCollerNom;
		        vector<string> tabCollerClasse;

		        if(nbLigneColler==0)
		        {
				    nbLigneColler=1;
				    tabCollerNom.push_back(contenu);
				    tabCollerClasse.push_back(string(""));
				}
				else
				{
			        tabCollerNom.resize(nbLigneColler);//contient tout au début
			        //token=strtok(contenu,"\n");
			        tabCollerNom[0]=contenu.substr(0, contenu.find('\n'));//string(token);
                    found=contenu.find('\n');
			        for(int i=1;i<nbLigneColler;i++)
			        {
	   				    tabCollerNom[i]=contenu.substr(found+1, contenu.find('\n', found+1)-found-1);//string(token);
	   				    found=contenu.find('\n', found+1);
	   				    //token=strtok(NULL,"\n");
	   				}
	   				tabCollerClasse.resize(nbLigneColler);
				}
				found=0;
   				string bufferInter;
   				string str;
   				size_t pos;
   				for(int i=0;i<nbLigneColler;i++)
   				{
	 			    pos=tabCollerNom[i].find("\t");
	 			    if(pos==string::npos)
 				    {
 			   		    //tabCollerNom[i]=bufferInter;
    				    tabCollerClasse[i]="";
				   	}
				   	else
				   	{
					 	//token=strtok((char*)tabCollerNom[i].c_str(),"\t");
	 				    //bufferInter=string(token);

	 				    //token=strtok(NULL,"\t");
	 				    //str=string(token);
			    		//pos=str.find("\r");
	 				    /*if(pos!=string::npos)
	 				    {
        			        str.replace(pos,1,"\0");
						}*/
        			    tabCollerClasse[i]=tabCollerNom[i].substr(pos+1);
        			    found=tabCollerClasse[i].find_first_of("\n\r");

        			    while(found!=string::npos)
                        {
                            tabCollerClasse[i].erase (found);
                            found=tabCollerClasse[i].find_first_of("\n\r");
                        }

        			    tabCollerNom[i]=tabCollerNom[i].substr(0,pos);
					}
				}
    			if(retourDialog==IDC_RADIO1)//le tableau est éffacé
				{
			        enfantActi enfantInter;
			        enfants.clear();

			        for(size_t i=0;i<nbLigneColler;i++)
			        {
 					    enfantInter.nom=tabCollerNom[i];
 					    enfantInter.classe=tabCollerClasse[i];
 					    enfantInter.ref=i;
 					    enfantInter.point=0;
 					    enfantInter.denominateur=0;
 					    for(int jour=0;jour<4;jour++)
 					    {
	 					    enfantInter.voeux[jour].voeu1=-1;
	 					    enfantInter.voeux[jour].voeu2=-1;
	 					    enfantInter.voeux[jour].voeu3=-1;
	 					    enfantInter.affectation[jour]=-1;
						}

			            enfants.push_back(enfantInter);
					}

					/*string strTest="";
					for(size_t i=0;i<enfants.size();i++)
                    {
                        strTest+=enfants[i].nom+"----"+enfants[i].classe+"\n";
                    }
                    MessageBox(0,strTest.c_str(),to_string(activite.size()).c_str(),0);*/

					AfficheNouveauTab(hwnd, hListboxNom, hListboxClasse, hstatu, hButtonAdd, hCheckboxCasesVide, hScroll, hComboActi,
                                      hComboV1, hComboV2, hComboV3,
                                      enfants, activite,
                                      nbLigne, iItemSel,
                                      pathFileOpen);
				}
			    else if(retourDialog==IDC_RADIO2)//le tableau est conservé, ajouté à la suite
				{
    			    enfantActi enfantInter;
    			    size_t sizeOfEnfants=enfants.size();
    			    for(int i=0;i<nbLigneColler;i++)
    			    {
	 				    enfantInter.nom=tabCollerNom[i];
 					    enfantInter.classe=tabCollerClasse[i];
 					    enfantInter.ref=i+sizeOfEnfants;
 					    enfantInter.point=0;
 					    enfantInter.denominateur=0;
 					    for(int jour=0;jour<4;jour++)
 					    {
	 					    enfantInter.voeux[jour].voeu1=-1;
	 					    enfantInter.voeux[jour].voeu2=-1;
	 					    enfantInter.voeux[jour].voeu3=-1;
	 					    enfantInter.affectation[jour]=-1;
						}

			            enfants.push_back(enfantInter);
					}
					AfficheNouveauTab(hwnd, hListboxNom, hListboxClasse, hstatu, hButtonAdd, hCheckboxCasesVide, hScroll, hComboActi,
                                      hComboV1, hComboV2, hComboV3,
                                      enfants, activite,
                                      nbLigne, iItemSel,
                                      pathFileOpen);
		 		}
		 		int tLen=GetWindowTextLength(hWndMain)+2;
	  		    char *titreFenetre=new char[tLen];
	  		    GetWindowText(hWndMain,titreFenetre,tLen);
	  		    if(titreFenetre[strlen(titreFenetre)-1]!='*')
	  		    {
	  		        strcat(titreFenetre,"*");
				}
	  		    SetWindowText(hWndMain,titreFenetre);
	  		    delete[] titreFenetre;
	  		    ::isFileSaved=false;
			}

			///Menu Réviser
			if(LOWORD(wParam)==IDM_VERIF_REMPLI)
			{
			    DialogBox(hinstance, "DIALOGVERIFREMPLI" , 0, (DLGPROC)DialogVerifRempliProc);
			}

			if(LOWORD(wParam)==IDM_VERIF_PLACES)
			{
			    int retourDialog=DialogBox(hinstance, "DIALOGVERIFPLACES" , hwnd, (DLGPROC)DialogVerifPlacesProc);
			    if(retourDialog==IDCANCEL)
			    {
				    return 0;
				}
				int nbPlaces=0;
				int nbEnfants=0;

				for(int i=0;i<activite.size();i++)
    			{
			        nbPlaces+=activite[i].placesDispo;
				}
				string snbPlacesActi=to_string(nbPlaces);
				string snbEleves;
				int tabNbEnfants[4]={0};
   				if(retourDialog==IDC_RADIO1)//RADIO1 : prendre tout en compte
   				{
 	     		    for(int i=0;i<enfants.size();i++)
 	     		    {
				        if(enfants[i].nom!="")
				        {
					  	    nbEnfants++;
					  	}
					}
				    snbEleves=to_string(nbEnfants);
				}
				else if (retourDialog==IDC_RADIO2)//RADIO2 : prendre suelement les élèves présents
				{
	 	 		    for(int jour=0;jour<4;jour++)
					{
					    for(int i=0;i<enfants.size();i++)
					    {
					        if((enfants[i].voeux[jour].voeu1!=-1 || enfants[i].voeux[jour].voeu2!=-1 || enfants[i].voeux[jour].voeu3!=-1) && enfants[i].nom!="")
					        {
				 			    tabNbEnfants[jour]++;
							}
						}
					}
					if(tabNbEnfants[0]>=tabNbEnfants[1])
					{
					    if(tabNbEnfants[0]>=tabNbEnfants[2])
					    {
						    if(tabNbEnfants[0]>=tabNbEnfants[3])
						    {
							    nbEnfants=tabNbEnfants[0];
							}
							else
							{
					 		    nbEnfants=tabNbEnfants[3];
							}
						}
						else
						{
				 		    if(tabNbEnfants[2]>=tabNbEnfants[3])
						    {
						        nbEnfants=tabNbEnfants[2];
							}
							else
							{
					 		    nbEnfants=tabNbEnfants[3];
							}
						}
					}
					else
					{
					 	if(tabNbEnfants[1]>=tabNbEnfants[2])
					    {
						    if(tabNbEnfants[1]>=tabNbEnfants[3])
						    {
							    nbEnfants=tabNbEnfants[1];
							}
							else
							{
					 		    nbEnfants=tabNbEnfants[3];
							}
						}
						else
						{
				 		    if(tabNbEnfants[2]>=tabNbEnfants[3])
						    {
						        nbEnfants=tabNbEnfants[2];
							}
							else
							{
					 		    nbEnfants=tabNbEnfants[3];
							}
						}
					}
					snbEleves=to_string(nbEnfants);
				}

			    if(nbPlaces<nbEnfants)
			    {
	  			    char texteFinal[130]="Attention, il n'y a pas assez de places par rapport au nombre d'élève;\n\
Il y a ";
			    	strcat(texteFinal,snbPlacesActi.c_str());
			    	if(nbPlaces>1)
			    	strcat(texteFinal," places pour ");
			    	else
			    	strcat(texteFinal," place pour ");
			    	strcat(texteFinal,snbEleves.c_str());
			    	if(nbEnfants>1)
			    	strcat(texteFinal," élèves, soit ");
			    	else
			    	strcat(texteFinal," élève, soit ");
			    	snbPlacesActi=to_string(nbEnfants-nbPlaces);
			    	strcat(texteFinal,snbPlacesActi.c_str());
			    	if(nbEnfants-nbPlaces>1)
			    	strcat(texteFinal," places manquantes.");
			    	else
			    	strcat(texteFinal," place manquante.");

	  			    MessageBox(hwnd,texteFinal,"Nombre de places insuffisantes!",MB_ICONWARNING);
	  			}
	  			else
	  			{
				    nbEnfants=0;
				    nbPlaces=0;
				    RemplirListeClasse(enfants, activite, nomClasses);
		 		    vector <int> nbEnfantsClasse(nomClasses.size());
			        for(int i=0;i<enfants.size();i++)
			        {
				 		for(int j=0;j<nomClasses.size();j++)
				 		{
					 		if(enfants[i].classe==nomClasses[j])
					 		{
			 			        nbEnfantsClasse[j]++;
							}
						}
					}
					size_t found=0;
				    size_t foundNew=0;
				    bool classeNonPresente=true;
				    string str;
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
				}
				/*else if(nbPlaces==nbEnfants)
	  			{
      			    char texteFinal[100]="Le nombre de places est le même que le nombre d'élève, à savoir: ";
      			    strcat(texteFinal,snbPlacesActi);
			    	if(nbPlaces>1)
			    	strcat(texteFinal," places pour ");
			    	else
			    	strcat(texteFinal," place pour ");
			    	strcat(texteFinal,snbEleves);
			    	if(nbEnfants>1)
			    	strcat(texteFinal," élèves.");
			    	else
			    	strcat(texteFinal," élève.");

			    	MessageBox(hwnd,texteFinal,"Même nombre de places que d'élèves!",MB_ICONINFORMATION);
				}
				else if(nbPlaces>nbEnfants)
				{
	 	 		    char texteFinal[150]="Le nombre de places est suffisant pour le nombre d'élèves, mais il y a plus de places que d'élèves.\n\
Il y a ";
   	   	            strcat(texteFinal,snbPlacesActi);
			    	if(nbPlaces>1)
			    	strcat(texteFinal," places pour ");
			    	else
			    	strcat(texteFinal," place pour ");
			    	strcat(texteFinal,snbEleves);
			    	if(nbEnfants>1)
			    	strcat(texteFinal," élèves, soit ");
			    	else
			    	strcat(texteFinal," élève, soit ");
			    	itoa(nbPlaces-nbEnfants,snbPlacesActi,10);
			    	strcat(texteFinal,snbPlacesActi);
			    	if(nbPlaces-nbEnfants>1)
			    	strcat(texteFinal," places en plus.");
			    	else
			    	strcat(texteFinal," place en plus.");

			    	MessageBox(hwnd,texteFinal,"Plus de places que d'élèves!",MB_ICONINFORMATION);
				}*/
			}
			if(LOWORD(wParam)==IDM_COUNT_APPLICATION)
			{
                DialogBox(hinstance, "DIALOGCOUNTAPPLICATION" , hwnd, (DLGPROC)DialogCountAppliProc);
            }

			/*if(HIWORD(wParam)==CBN_EDITCHANGE && HWND(lParam)==hComboActi)
			{
	  		    MessageBox(0,0,0,0);
			}*/

			if(LOWORD(wParam)==IDM_VERIF_ACTI)
			{
	  		    DialogBox(hinstance, "DIALOGVERIFACTI" , hwnd, (DLGPROC)DialogVerifActi);
			}

			///Menu ?
			if(LOWORD(wParam)==IDM_ABOUT)
			{
		 	    DialogBox(hinstance, "DIALOGABOUT" , hwnd, (DLGPROC)DialogAboutProc);
		 	    /*MessageBox(hwnd,"Ce programme a été conçut et élaboré par Simon Vareille\npour un usage strictement non-règlementé.\n\
Cette version peut éventuellement présenter d'occasionnelles et rarissimes erreurs, merci de les signaler.","A-propos de GroupManage...",MB_ICONQUESTION);*/
			}
			if(LOWORD(wParam)==IDM_HELP)
			{
   			    SendMessage(hwnd,WM_HELP,0,0);
			}
 	 		return 0;
	 	}
	 	//###################################################### MESSAGES DES DLLS ##########################################################
	 	case MSG_SUBCLASSCONTROL:
        {
            switch(LOWORD(wParam))
   		    {
                case SUB_LISTBOX:
                {
                    subclass_the_listbox((HWND)lParam);
                    break;
                }
                case SUB_EDIT:
                {
                    subclass_the_edit((HWND)lParam);
                    break;
                }
                case SUB_COMBOACTIEDIT:
                {
                    subclass_the_comboActiEdit((HWND)lParam);
                    break;
                }
                case SUB_COMBOACTI:
                {
                    subclass_the_comboBoxActi((HWND)lParam);
                    break;
                }
                case SUB_EDITGOLINE:
                {
                    subclass_the_editGoLine((HWND)lParam);
                    break;
                }
                case SUB_EDITFINDNAME:
                {
                    subclass_the_editFindName((HWND)lParam);
                    break;
                }
                case SUB_EDITAJOUTCLASSE:
                {
                    subclass_the_editComboAjoutClasse((HWND)lParam);
                    break;
                }
                case SUB_COMBOVOEU:
                {
                    subclass_the_ComboVoeu((HWND)lParam);
                    break;
                }
            }
            return 0;
        }
	 	//######################################################### SCROLL BAR ##############################################################
	 	case WM_VSCROLL:
	 	{
   		    switch(LOWORD(wParam))
   		    {
   			    case SB_LINEUP:
		 		{
		 	 	    if(HWND(lParam)==hScroll)
		 	 	    {
				 	    SCROLLINFO infoScroll;
				 	    infoScroll.cbSize=sizeof(infoScroll);
				 	    infoScroll.fMask=SIF_ALL;
			    		GetScrollInfo(hScroll,SB_CTL,&infoScroll);
			    		if(infoScroll.nPos==0)
			    		{
			  			    return 0;
						}
						infoScroll.nPos--;
						infoScroll.fMask = SIF_POS;
						SendMessage(hListboxNom,LB_SETTOPINDEX,infoScroll.nPos,0);
						SendMessage(hListboxClasse,LB_SETTOPINDEX,infoScroll.nPos,0);
						SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);

						ActualiserTabCombo(infoScroll.nPos);
					}
					/*else if(HWND(lParam)==NULL);
					{
		 	 		    SCROLLINFO infoScroll;
				 	    infoScroll.cbSize=sizeof(infoScroll);
				 	    infoScroll.fMask=SIF_ALL;
			    		GetScrollInfo(hwnd,SB_VERT,&infoScroll);

						infoScroll.nPos--;
						if(infoScroll.nPos<infoScroll.nMin)
						{
					 	    infoScroll.nPos=infoScroll.nMin;
						}
						infoScroll.fMask = SIF_POS;
						SetScrollInfo(hwnd,SB_VERT,&infoScroll,true);

						EnumChildWinLParam lParamEnumChild={&infoScroll,SB_VERT,SB_LINEUP};

						EnumChildWindows(hwnd,(WNDENUMPROC)&EnumChildProc,(LPARAM)&lParamEnumChild);
						InvalidateRect(hwnd, NULL, 0);
					}*/
					break;
				}
				case SB_LINEDOWN:
			 	{
		 	 	    if(HWND(lParam)==hScroll)
		 	 	    {
				 	    SCROLLINFO infoScroll;
				 	    infoScroll.cbSize=sizeof(infoScroll);
				 	    infoScroll.fMask=SIF_ALL;
	  		    		GetScrollInfo(hScroll,SB_CTL,&infoScroll);

	  		    		if(infoScroll.nPos==infoScroll.nMax-nbLigne+1)
	  		    		{
	    				    return 0;
						}
						infoScroll.nPos++;
						infoScroll.fMask = SIF_POS;
						SendMessage(hListboxNom,LB_SETTOPINDEX,infoScroll.nPos,0);
						SendMessage(hListboxClasse,LB_SETTOPINDEX,infoScroll.nPos,0);
						SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);
						ActualiserTabCombo(infoScroll.nPos);
					}
					/*else if(HWND(lParam)==NULL);
					{
		 	 		    SCROLLINFO infoScroll;
				 	    infoScroll.cbSize=sizeof(infoScroll);
				 	    infoScroll.fMask=SIF_ALL;
			    		GetScrollInfo(hwnd,SB_VERT,&infoScroll);

						infoScroll.nPos++;
						if(infoScroll.nPos>infoScroll.nMax)
						{
					 	    infoScroll.nPos=infoScroll.nMax;
						}
						infoScroll.fMask = SIF_POS;
						SetScrollInfo(hwnd,SB_VERT,&infoScroll,true);

						EnumChildWinLParam lParamEnumChild={&infoScroll,SB_VERT,SB_LINEDOWN};

						EnumChildWindows(hwnd,(WNDENUMPROC)&EnumChildProc,(LPARAM)&lParamEnumChild);//MAKELPARAM((DWORD)&infoScrollLineDown,SB_VERT));

						InvalidateRect(hwnd, NULL, 0);
					}*/
					break;
			 	}
			 	case SB_PAGEUP:
			 	{
		 	 	    if(HWND(lParam)==hScroll)
		 	 	    {
			 	 	    SCROLLINFO infoScroll;
				 	    infoScroll.cbSize=sizeof(infoScroll);
				 	    infoScroll.fMask=SIF_ALL;
			    		GetScrollInfo(hScroll,SB_CTL,&infoScroll);
			    		if(infoScroll.nPos==0)
			    		{
			  			    return 0;
						}
						infoScroll.nPos-=infoScroll.nPage;
						infoScroll.fMask = SIF_POS;
						if(infoScroll.nPos<0)infoScroll.nPos=0;
						SendMessage(hListboxNom,LB_SETTOPINDEX,infoScroll.nPos,0);
						SendMessage(hListboxClasse,LB_SETTOPINDEX,infoScroll.nPos,0);
						SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);

						ActualiserTabCombo(infoScroll.nPos);
					}
		 	 	    break;
				}
				case SB_PAGEDOWN:
			 	{
		 	 	    if(HWND(lParam)==hScroll)
		 	 	    {
			 	 	    SCROLLINFO infoScroll;
				 	    infoScroll.cbSize=sizeof(infoScroll);
				 	    infoScroll.fMask=SIF_ALL;
	  		    		GetScrollInfo(hScroll,SB_CTL,&infoScroll);
	  		    		if(infoScroll.nPos==infoScroll.nMax-nbLigne+1)
	  		    		{
	    				    return 0;
						}
						infoScroll.nPos+=infoScroll.nPage;
						if(infoScroll.nPos>infoScroll.nMax-nbLigne+1)infoScroll.nPos=infoScroll.nMax-nbLigne+1;
						infoScroll.fMask = SIF_POS;
						SendMessage(hListboxNom,LB_SETTOPINDEX,infoScroll.nPos,0);
						SendMessage(hListboxClasse,LB_SETTOPINDEX,infoScroll.nPos,0);
						SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);

						ActualiserTabCombo(infoScroll.nPos);
					}
		 	 	    break;
				}
   			    case SB_THUMBTRACK:
		 		{
		 	 	    if(HWND(lParam)==hScroll)
		 	 	    {
			 		    SendMessage(hListboxNom,LB_SETTOPINDEX,HIWORD(wParam),0);
			 		    SendMessage(hListboxClasse,LB_SETTOPINDEX,HIWORD(wParam),0);
			 		    ActualiserTabCombo(HIWORD(wParam));
					}
		 		    break;
				}
		 	    case SB_THUMBPOSITION:
   		    	{
	 	 		    if(HWND(lParam)==hScroll)
		 	 	    {
				 	    SendMessage(hListboxNom,LB_SETTOPINDEX,HIWORD(wParam),0);
				 	    SendMessage(hListboxClasse,LB_SETTOPINDEX,HIWORD(wParam),0);
				 	    SCROLLINFO infoScroll;
	  		    		GetScrollInfo(hScroll,SB_CTL,&infoScroll);
				 	    infoScroll.fMask = SIF_POS;
						infoScroll.nPos=HIWORD(wParam);
						SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);

						ActualiserTabCombo(HIWORD(wParam));
					}
					break;
				}
		 	}
   		    return 0;
	 	}
	 	case WM_MOUSEWHEEL:
	 	{
 	 	    if(enfants.size()>nbLigne-1)
 	 	    {
	 	 	    int zDelta=GET_WHEEL_DELTA_WPARAM(wParam);
	 	 	    if (GetKeyState(VK_CONTROL) & Mask)
	 	 	    {
                    if(zDelta>0)
    	 	 	    {
                        SCROLLINFO infoScroll;
    			 	    infoScroll.cbSize=sizeof(infoScroll);
    			 	    infoScroll.fMask=SIF_ALL;
    		    		GetScrollInfo(hScroll,SB_CTL,&infoScroll);
    		    		if(infoScroll.nPos==0)
    		    		{
    		  			    return 0;
    					}
    					infoScroll.nPos-=infoScroll.nPage;
    					infoScroll.fMask = SIF_POS;
    					if(infoScroll.nPos<0)infoScroll.nPos=0;
    					SendMessage(hListboxNom,LB_SETTOPINDEX,infoScroll.nPos,0);
    					SendMessage(hListboxClasse,LB_SETTOPINDEX,infoScroll.nPos,0);
    					SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);
    					ActualiserTabCombo(infoScroll.nPos);
    				}
    				else
    				{
    		 		    SCROLLINFO infoScroll;
    			 	    infoScroll.cbSize=sizeof(infoScroll);
    			 	    infoScroll.fMask=SIF_ALL;
    		    		GetScrollInfo(hScroll,SB_CTL,&infoScroll);
    		    		if(infoScroll.nPos==infoScroll.nMax-nbLigne+1)
      		    		{
        				    return 0;
    					}
    					infoScroll.nPos+=infoScroll.nPage;
    					infoScroll.fMask = SIF_POS;
    					if(infoScroll.nPos>infoScroll.nMax-nbLigne+1)infoScroll.nPos=infoScroll.nMax-nbLigne+1;
    					SendMessage(hListboxNom,LB_SETTOPINDEX,infoScroll.nPos,0);
    					SendMessage(hListboxClasse,LB_SETTOPINDEX,infoScroll.nPos,0);
    					SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);
    					ActualiserTabCombo(infoScroll.nPos);
    				}
                }
                else
                {
    	 	 	    if(zDelta>0)
    	 	 	    {
    				    SCROLLINFO infoScroll;
    			 	    infoScroll.cbSize=sizeof(infoScroll);
    			 	    infoScroll.fMask=SIF_ALL;
    		    		GetScrollInfo(hScroll,SB_CTL,&infoScroll);
    		    		if(infoScroll.nPos==0)
    		    		{
    		  			    return 0;
    					}
    					infoScroll.nPos--;
    					infoScroll.fMask = SIF_POS;
    					if(infoScroll.nPos<0)infoScroll.nPos=0;
    					SendMessage(hListboxNom,LB_SETTOPINDEX,infoScroll.nPos,0);
    					SendMessage(hListboxClasse,LB_SETTOPINDEX,infoScroll.nPos,0);
    					SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);
    					ActualiserTabCombo(infoScroll.nPos);
    				}
    				else
    				{
    		 		    SCROLLINFO infoScroll;
    			 	    infoScroll.cbSize=sizeof(infoScroll);
    			 	    infoScroll.fMask=SIF_ALL;
    		    		GetScrollInfo(hScroll,SB_CTL,&infoScroll);
    		    		if(infoScroll.nPos==infoScroll.nMax-nbLigne+1)
      		    		{
        				    return 0;
    					}
    					infoScroll.nPos++;
    					infoScroll.fMask = SIF_POS;
    					if(infoScroll.nPos>infoScroll.nMax-nbLigne+1)infoScroll.nPos=infoScroll.nMax-nbLigne+1;
    					SendMessage(hListboxNom,LB_SETTOPINDEX,infoScroll.nPos,0);
    					SendMessage(hListboxClasse,LB_SETTOPINDEX,infoScroll.nPos,0);
    					SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);
    					ActualiserTabCombo(infoScroll.nPos);
    				}
                }
			}
			else
			{
	 		    EnableWindow(hScroll,false);
			}
 	 	    return 0;
	 	}
	 	/*case WM_SIZE:
		{
	 	    if(wParam==SIZE_MAXIMIZED)
	 		{
			 						 // MessageBox(0,0,"SIZE_MAXIMIZED",0);
			    //Verticale
		  	    SCROLLINFO infoScroll;
				infoScroll.cbSize=sizeof(SCROLLINFO);

				//GetScrollInfo(hScroll,SB_CTL,infoScroll);

				infoScroll.fMask=SIF_ALL;
				infoScroll.nMin=0;
				infoScroll.nMax=0;
				infoScroll.nPage=1;
				infoScroll.nPos=0;
				SetScrollInfo(hWndMain,SB_VERT,&infoScroll,true);

				//Horizontale
				infoScroll.cbSize=sizeof(SCROLLINFO);

				infoScroll.fMask=SIF_ALL;
				infoScroll.nMin=0;
				infoScroll.nMax=0;
				infoScroll.nPage=1;
				infoScroll.nPos=0;
				SetScrollInfo(hWndMain,SB_HORZ,&infoScroll,true);

				//::tailleFenetre.x=LOWORD(lParam);
				//::tailleFenetre.y=HIWORD(lParam);
  			}
		  	else if(wParam==SIZE_RESTORED)
	 	    {
			 	 //MessageBox(0,0,"SIZE_RESTORED",0);
	 	 	    //Verticale
	 	 	    SCROLLINFO infoScroll;
				infoScroll.cbSize=sizeof(SCROLLINFO);

				//GetScrollInfo(hScroll,SB_CTL,infoScroll);

				infoScroll.fMask=SIF_ALL;
				infoScroll.nMin=0;
				infoScroll.nMax=tailleFenetre.y;//tailleFenetre.y-HIWORD(lParam);
				infoScroll.nPage=HIWORD(lParam);
				infoScroll.nPos=0;
				SetScrollInfo(hWndMain,SB_VERT,&infoScroll,true);

				//Horizontale
				infoScroll.cbSize=sizeof(SCROLLINFO);

				infoScroll.fMask=SIF_ALL;
				infoScroll.nMin=0;
				infoScroll.nMax=tailleFenetre.x;
				infoScroll.nPage=LOWORD(lParam);
				infoScroll.nPos=0;
				SetScrollInfo(hWndMain,SB_HORZ,&infoScroll,true);


				//::tailleFenetre.x=LOWORD(lParam);
				//::tailleFenetre.y=HIWORD(lParam);
	 		}
	 	    return 0;
	 	}*/
	 	case WM_CTLCOLORSTATIC:
		{
	 	    HDC hdcStatic = (HDC) wParam;
	 	    SetBkColor(hdcStatic,RGB(235,232,216));
		    return (INT_PTR)hbrBkgnd;
		    //return 0;
		}
	 	case WM_TIMER:
		{
	 	    if(wParam == ID_TIMER_BACKUP)//touts les minutes
	 	    {
	 	        DWORD threadID;
				CreateThread(NULL, 0, SauvegardeBackUp,NULL, 0, &threadID);
			}
			return 0;
		}
	 	case WM_HELP:
		{
	 	    ShellExecute(0,"open","Aide/helpGroupManage.chm",0,0,SW_NORMAL);
	 	    return 0;
  		}
  		case WM_RBUTTONUP:
	 	{
 	 	    POINT pt;
 	 	    GetCursorPos(&pt);
	  		ScreenToClient(hwnd,&pt);
	  		//CreateToolTip(hwnd);
 	 	    //if(pt.x
 	 	    return 0;
		}
		case WM_CLOSE:
	 	{
		    // Détruire la fenêtre:
   			/*int tLen=GetWindowTextLength(hwnd)+2;
		    char *titreFenetre=new char[tLen];
		    GetWindowText(hwnd,titreFenetre,tLen);
		    if(titreFenetre[strlen(titreFenetre)-1]=='*')
		    {
			    int result=MessageBox(hwnd,"Voulez-vous enregistrer votre travail?\n\
Tout travail non enregistré sera sinon perdu.","Travail non enregistré",MB_ICONWARNING | MB_YESNOCANCEL);
		        if(result==IDYES)
		        {
			        SendMessage(hwnd,WM_COMMAND,LOWORD(IDM_SAVE),0);
			    }
			    else if(result==IDCANCEL)
			    {
				    return 0;
				}
			}
			delete[] titreFenetre;*/
			if(!isFileSaved)
            {
                int result=MessageBox(hwnd,"Voulez-vous enregistrer votre travail?\n\
Tout travail non enregistré sera sinon perdu.","Travail non enregistré",MB_ICONWARNING | MB_YESNOCANCEL);
		        if(result==IDYES)
		        {
			        SendMessage(hwnd,WM_COMMAND,LOWORD(IDM_SAVE),0);
			    }
			    else if(result==IDCANCEL)
			    {
				    return 0;
				}
            }


			KillTimer(hwnd,ID_TIMER_BACKUP);

			string sBackUp;
			size_t found;
			if(actiFileOpen!="")
			{
				sBackUp=actiFileOpen;
	   			found = sBackUp.rfind('\\');
			    sBackUp.replace(found+1,2,"~&");

			    if(access(sBackUp.c_str(),F_OK)!=-1)
			    {
				    remove(sBackUp.c_str());
				}
			}
			if(pathFileOpen!="")
			{
				sBackUp=pathFileOpen;
	   			found = sBackUp.rfind('\\');
			    sBackUp.replace (found+1,2,"~&");
				if(access(sBackUp.c_str(),F_OK)!=-1)
				{
				    remove(sBackUp.c_str());
				}
			}

   			sBackUp="~&Nouveau document GRMA.grma2";
			if(access(sBackUp.c_str(),F_OK)!=-1)
			{
			    remove(sBackUp.c_str());
			}
			else
			{
			 	char szPath[MAX_PATH] = "";
		    	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szPath);
		    	sBackUp=string(szPath)+"\\~&Nouveau document GRMA.grma2";
		    	if(access(sBackUp.c_str(),F_OK)!=-1)
				{
				    remove(sBackUp.c_str());
				}
			}

			//fLog.close();

		    DestroyWindow(hwnd);
		    return 0;
		}
		case WM_DESTROY:
		    // Forcer la sortie de la boucle des messages:
		    PostQuitMessage(0);
		    return 0;
		default:
		    break;
	}
	// Appeler la procedure par défaut de la fenêtre:
	return DefWindowProc(hwnd, message, wParam, lParam);
}

DWORD WINAPI OrganisationDesDonnees(LPVOID lpParameter)
{
    if(enfants.size()==0)
    {
	    return FALSE;
	}
    /*int tLen=GetWindowTextLength(hWndMain)+2;
    char *titreFenetre=new char[tLen];
    GetWindowText(hWndMain,titreFenetre,tLen);
    if(titreFenetre[strlen(titreFenetre)-1]=='*')
    {
	    int result=MessageBox(hWndMain,"Voulez-vous enregistrer votre travail?\n\
Tout travail non enregistré sera sinon perdu.","Travail non enregistré",MB_ICONWARNING | MB_YESNOCANCEL);
        if(result==IDYES)
        {
	        SendMessage(hWndMain,WM_COMMAND,LOWORD(IDM_SAVE),0);
	    }
	    else if(result==IDCANCEL)
	    {
		    return 0;
		}
		titreFenetre[strlen(titreFenetre)-1]=' ';
        SetWindowText(hWndMain,titreFenetre);
	}
	delete[] titreFenetre;*/

	if(!isFileSaved)
    {
        int result=MessageBox(hWndMain,"Voulez-vous enregistrer votre travail?\n\
Tout travail non enregistré sera sinon perdu.","Travail non enregistré",MB_ICONWARNING | MB_YESNOCANCEL);
        if(result==IDYES)
        {
	        SendMessage(hWndMain,WM_COMMAND,LOWORD(IDM_SAVE),0);
	    }
	    else if(result==IDCANCEL)
	    {
		    return 0;
		}
    }

	::traitementLance=true;
	Initialisation(enfants, enfantsAttributed, activite, inferieurA, traitementLance, arretTraitement, hDlgTraitementEnCours);

	EndDialog(hDlgTraitementEnCours,IDOK);
    SortieMain();
    //fLog<<"Après SortieMain\n";
    ::traitementLance=false;
    return 0;
}

DWORD WINAPI SauvegardeBackUp(LPVOID lpParameter)
{
    return BackUp(pathFileOpen, actiFileOpen, enfants, activite, backUpActivite, traitementLance);
}


    //ShellExecute(0,"open","nbDeBase.exe",0,0,SW_NORMAL);
    //system("nbDeBase.exe");
    /*PROCESS_INFORMATION Process = {0};
	STARTUPINFO Start = {0};
	Start.cb = sizeof(STARTUPINFO);
	Start.lpReserved = NULL;
	Start.lpReserved2 = NULL;
	Start.cbReserved2 = 0;
	Start.lpDesktop = NULL;
	Start.dwFlags = 0;

	// Lance process
	int ReturnProcess = CreateProcess(NULL,("SansNom1.exe" ), NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &Start, &Process);

	// Vérifie si l'opération s'est bien déroulée
	if (ReturnProcess == 0)
	          MessageBox(0,"Erreur, impossible to run Process !", "Erreur", MB_OK + MB_ICONEXCLAMATION);

	DWORD ReturnValue = WaitForSingleObject(Process.hProcess, INFINITE); */
    /*ifstream FichierEntrer("donées.txt",ios::binary);
    string a;
    FichierEntrer>>a;
    MessageBox(0,0,0,0);*/


/*  //ShellExecute(0,"open","nbDeBase.exe",0,0,SW_NORMAL);
    //system("nbDeBase.exe");
    PROCESS_INFORMATION Process = {0};
	STARTUPINFO Start = {0};
	Start.cb = sizeof(STARTUPINFO);
	Start.lpReserved = NULL;
	Start.lpReserved2 = NULL;
	Start.cbReserved2 = 0;
	Start.lpDesktop = NULL;
	Start.dwFlags = 0;

	// Lance process
	int ReturnProcess = CreateProcess(NULL,("SansNom1.exe" ), NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &Start, &Process);

	// Vérifie si l'opération s'est bien déroulée
	if (ReturnProcess == 0)
	          MessageBox(0,"Erreur, impossible to run Process !", "Erreur", MB_OK + MB_ICONEXCLAMATION);

	DWORD ReturnValue = WaitForSingleObject(Process.hProcess, INFINITE); */

int OuvrirFeuille(HWND hwnd)
{
    OuvertureFeuille(hwnd, hwndToolTips, hinstance, enfants, activite, pathFileOpen);

    RemplirListeClasse(enfants, activite, nomClasses);
    AfficheNouveauTab(hwnd, hListboxNom, hListboxClasse, hstatu, hButtonAdd, hCheckboxCasesVide, hScroll, hComboActi,
                      hComboV1, hComboV2, hComboV3,
                      enfants, activite,
                      nbLigne, iItemSel,
                      pathFileOpen);
    backUpActivite=1;
    TabCtrl_SetCurSel(hTabs,0);
    jourEnCourSaisie=0;
}

int OuvrirActivite(HWND hwnd)
{
    OuvertureActivite(hwnd, hwndToolTips, hinstance, activite, actiFileOpen);
    RemplirListeClasse(enfants, activite, nomClasses);

    ::iItemSel=0;
    ComboBox_ResetContent(hComboActi);
    for(int i=0;i<activite.size();i++)
	{
	    SendMessage(hComboActi,CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[i].nom.c_str());
	}
	ComboBox_SetCurSel(hComboActi,iItemSel);
	::backUpActivite=0;
}

long GetHeigthTray(BOOL VIEWERROR)
{
    RECT tray;
    HWND hWnd;

    if (NULL ==(hWnd = FindWindow("Shell_TrayWnd", NULL)))
    {
        if(VIEWERROR)
            VoirErreur(GetLastError());
        return -1;
    }
    if(!GetWindowRect(hWnd, &tray))
    {
        if(VIEWERROR)
            VoirErreur(GetLastError());
        return -1;
    }
    return tray.bottom - tray.top;
}

LRESULT CALLBACK new_subclassed_listbox_proc(HWND hList, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
	    case WM_COMMAND:
		{
  		    if(HIWORD(wParam)==EN_KILLFOCUS && (HWND)lParam==hEditList)
	 		{
    		    DestroyWindow(hEditList);
	        }
	        if(LOWORD(wParam)==IDM_CHANGENAME)
	        {
		  	    RECT rect;
				POINT pt={GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
				ScreenToClient(hList,&pt);
				iItemEdit=SendMessage(hList,LB_ITEMFROMPOINT,0,MAKELONG(pt.x,pt.y));
				SendMessage(hList,LB_GETITEMRECT,iItemEdit,(LPARAM)&rect);
				char *contenuLigne=new char[SendMessage(hList,LB_GETTEXTLEN,iItemEdit,0)+1];
				SendMessage(hList,LB_GETTEXT,iItemEdit,(LPARAM)contenuLigne);

				hEditList=CreateWindowEx(0,"edit",contenuLigne,WS_CHILD | WS_VISIBLE | WS_BORDER | ES_WANTRETURN | ES_AUTOHSCROLL ,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,hList,NULL,0,0);
				if(hList==hListboxNom)
				{
				    Edit_LimitText(hEditList,66);
				}
				else if(hList==hListboxClasse)
				{
				    Edit_LimitText(hEditList,34);
				}
				subclass_the_edit(hEditList);
				ShowWindow(hEditList,SW_SHOW);
				SetFocus(hEditList);
				hListboxEnCours=hList;
				delete[] contenuLigne;
		  	}
		  	if(LOWORD(wParam)==IDM_ABSENT)
		  	{
		  	    POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(hList,&pt);
				int iItem=SendMessage(hList,LB_ITEMFROMPOINT,0,MAKELPARAM(pt.x,pt.y));
				int iEnfant=SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)SendMessage(hList,LB_ITEMFROMPOINT,0,MAKELPARAM(pt.x,pt.y)),0);
				enfants[iEnfant].voeux[jourEnCourSaisie].voeu1=-1;
				enfants[iEnfant].voeux[jourEnCourSaisie].voeu2=-1;
				enfants[iEnfant].voeux[jourEnCourSaisie].voeu3=-1;
				int topIndex=SendMessage(hList,LB_GETTOPINDEX,0,0);
				EnableWindow(hComboV1[iItem-topIndex],false);
				EnableWindow(hComboV2[iItem-topIndex],false);
				EnableWindow(hComboV3[iItem-topIndex],false);
			}
			if(LOWORD(wParam)==IDM_SUPPRSEM)
			{

			    int iItem=SendMessage(hList,LB_ITEMFROMPOINT,0,lParam);
			    int iEnfant=SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)SendMessage(hList,LB_ITEMFROMPOINT,0,lParam),0);
			    SendMessage(hListboxNom,LB_DELETESTRING,iItem,0);
			    SendMessage(hListboxClasse,LB_DELETESTRING,iItem,0);

			    int iItemTop=SendMessage(hListboxNom,LB_GETTOPINDEX,0,0)-1;
			    SendMessage(hListboxNom,LB_SETTOPINDEX,iItemTop,0);
			    SendMessage(hListboxClasse,LB_SETTOPINDEX,iItemTop,0);

	 		    if(enfants.size()>0)
	 		    {
	    		    enfants.erase(enfants.begin()+iEnfant);
	    		    for(int i=0;i<enfants.size();i++)
	    		    {
		 		        enfants[i].ref=i;
		 		        if(SendMessage(hListboxNom,LB_GETITEMDATA,i,0)>=iEnfant)
		 		        {
                            SendMessage(hListboxNom,LB_SETITEMDATA,i,SendMessage(hListboxNom,LB_GETITEMDATA,i,0)-1);
                            SendMessage(hListboxClasse,LB_SETITEMDATA,i,SendMessage(hListboxClasse,LB_GETITEMDATA,i,0)-1);
                        }
					}
				}

				if(enfants.size()<nbLigne)
				{
					RECT rect;
					GetWindowRect(hListboxNom,&rect);
					SetWindowPos(hListboxNom,0,0,0,rect.right-rect.left,rect.bottom-rect.top-23,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
					GetWindowRect(hListboxClasse,&rect);
					SetWindowPos(hListboxClasse,0,0,0,rect.right-rect.left,rect.bottom-rect.top-23,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
					GetWindowRect(hButtonAdd,&rect);
					POINT pt;
					pt.x=rect.left;
					pt.y=rect.top;
					ScreenToClient(hWndMain,&pt);
					SetWindowPos(hButtonAdd,0,pt.x,pt.y-23,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

					DestroyWindow(hComboV1.back());
					hComboV1.pop_back();
					DestroyWindow(hComboV2.back());
					hComboV2.pop_back();
					DestroyWindow(hComboV3.back());
					hComboV3.pop_back();
					EnableWindow(hScroll,false);
				}
				ActualiserTabCombo(iItemTop);

				SCROLLINFO infoScroll;
				infoScroll.cbSize=sizeof(SCROLLINFO);

				GetScrollInfo(hScroll,SB_CTL,&infoScroll);

				infoScroll.fMask=SIF_ALL;
				infoScroll.nMin=0;
				infoScroll.nMax=enfants.size();//rectHwnd.bottom-83;
				//infoScroll.nPage=nbLigne;//infoScroll.nPage*nbLigne-1/nbLigne;//rectHwnd.bottom-83-22*(nbItem-nbLigne);
				infoScroll.nPos=iItemTop;
				SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);

				if(enfants.size()<nbLigne-1)
				{
				    EnableWindow(hScroll,false);
				}

	 			UpdateStatusBar(hstatu, enfants, hWndMain);

	 			int tLen=GetWindowTextLength(hWndMain)+2;
	  		    char *titreFenetre=new char[tLen];
	  		    GetWindowText(hWndMain,titreFenetre,tLen);
	  		    if(titreFenetre[strlen(titreFenetre)-1]!='*')
	  		    {
	  		        strcat(titreFenetre,"*");
				}
	  		    SetWindowText(hWndMain,titreFenetre);
	  		    delete[] titreFenetre;
	  		    ::isFileSaved=false;
			}
	        break;
        }
        case WM_RBUTTONDOWN:
	 	{
 	 	    POINT pt;
 	 	    GetCursorPos(&pt);
 	 	    ScreenToClient(hList,&pt);
 	 	    SendMessage(hList,WM_LBUTTONDOWN, MK_LBUTTON,MAKELONG(pt.x, pt.y));
	 		break;
		}
        case WM_RBUTTONUP:
	 	{
 	 	    POINT pt;
 	 	    GetCursorPos(&pt);
 	 	    POINT ptScreen=pt;

 	 	    HMENU hPopupMenu = CreatePopupMenu();
 	 	    //InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDM_SUPPRJOUR, "Supprimer pour ce jour");
    		AppendMenu(hPopupMenu, MF_BYPOSITION | MF_STRING, IDM_SUPPRSEM, "Supprimer (pour toute la semaine)");

			ScreenToClient(hList,&pt);
			int iItem=SendMessage(hList,LB_ITEMFROMPOINT,0,MAKELPARAM(pt.x,pt.y));
    		/*if(enfants[jourEnCourSaisie][iItem].voeu1==-1 && enfants[jourEnCourSaisie][iItem].voeu2==-1 && enfants[jourEnCourSaisie][iItem].voeu3==-1)
    		{
	  		    AppendMenu(hPopupMenu, MF_BYPOSITION | MF_STRING, IDM_PRESENT, "Eleve présent");
	  		}
	  		else
	  		{
    		    AppendMenu(hPopupMenu, MF_BYPOSITION | MF_STRING, IDM_ABSENT, "Eleve absent ce jour");
			}*/
			//AppendMenu(hPopupMenu, MF_BYPOSITION | MF_STRING, IDM_CHANGENAME, "Changer le nom pour le jour (nouvel enfant)");
    		//SetForegroundWindow(hwnd);
    		SendMessage(hList,WM_COMMAND,TrackPopupMenu(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD, ptScreen.x, ptScreen.y, 0, hList, NULL),MAKELONG(pt.x, pt.y));
 	 	    break;
		}
		case WM_LBUTTONDOWN:
	 	{
	 	    POINT pt;
 	     	GetCursorPos(&pt);
 	     	ScreenToClient(hList,&pt);
 	 		if(hList==hListboxNom)
 	 		{
                if(!(GetKeyState(VK_CONTROL)& Mask) && !(GetKeyState(VK_SHIFT)& Mask))
                {
                    SendMessage(hListboxClasse,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxClasse,LB_GETCOUNT,0,0)));
                }
                else if(GetKeyState(VK_SHIFT)& Mask)
                {
                    int first;
                    SendMessage(hListboxNom,LB_GETSELITEMS,1,(LPARAM)&first);
                    SendMessage(hListboxClasse,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxClasse,LB_GETCOUNT,0,0)));
                    SendMessage(hListboxClasse,LB_SELITEMRANGE, true, MAKELPARAM(first,SendMessage(hList,LB_ITEMFROMPOINT,0,MAKELPARAM(pt.x,pt.y))));
                }
                SendMessage(hListboxClasse,LB_SETSEL,true,SendMessage(hList,LB_ITEMFROMPOINT,0,MAKELPARAM(pt.x,pt.y)));

                if(SendMessage(hListboxNom,LB_GETSELCOUNT,0,0)>1)
                {
        		    UpdateStatusBar(hstatu, hListboxNom);
                }
                else
                {
                    UpdateStatusBar(hstatu, enfants, hWndMain);
                }
     		    //SendMessage(hListboxClasse,LB_SETCURSEL,SendMessage(hList,LB_ITEMFROMPOINT,0,MAKELPARAM(pt.x,pt.y)),0);
     		    /*SendMessage(hListboxClasse,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxClasse,LB_GETCOUNT,0,0)));
                int *selected=new int(SendMessage(hListboxNom,LB_GETSELCOUNT,0,0)+1);
                SendMessage(hListboxNom,LB_GETSELITEMS,SendMessage(hListboxNom,LB_GETSELCOUNT,0,0),(LPARAM)selected);
                for(int i=0;i<SendMessage(hListboxNom,LB_GETSELCOUNT,0,0);i++)
                {
                    SendMessage(hListboxClasse,LB_SETSEL,true,selected[i]);
                }
                delete[] selected;*/
		 	}
		 	else if(hList==hListboxClasse)
 	 		{
     		    //SendMessage(hListboxNom,LB_SETCURSEL,SendMessage(hList,LB_ITEMFROMPOINT,0,MAKELPARAM(pt.x,pt.y)),0);
     		    if(!(GetKeyState(VK_CONTROL)& Mask) && !(GetKeyState(VK_SHIFT)& Mask))
                {
                    SendMessage(hListboxNom,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxNom,LB_GETCOUNT,0,0)));
                }
                else if(GetKeyState(VK_SHIFT)& Mask)
                {
                    int first;
                    SendMessage(hListboxClasse,LB_GETSELITEMS,1,(LPARAM)&first);
                    SendMessage(hListboxNom,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxNom,LB_GETCOUNT,0,0)));
                    SendMessage(hListboxNom,LB_SELITEMRANGE, true, MAKELPARAM(first,SendMessage(hList,LB_ITEMFROMPOINT,0,MAKELPARAM(pt.x,pt.y))));
                }
                SendMessage(hListboxNom,LB_SETSEL,true,SendMessage(hList,LB_ITEMFROMPOINT,0,MAKELPARAM(pt.x,pt.y)));

                if(SendMessage(hListboxNom,LB_GETSELCOUNT,0,0)>1)
                {
        		    UpdateStatusBar(hstatu, hListboxNom);
                }
                else
                {
                    UpdateStatusBar(hstatu, enfants, hWndMain);
                }
		 	}
		 	break;
		}
		case WM_KILLFOCUS:
	 	{
 	 	    break;
	 	}
	 	case WM_MOUSEWHEEL:
	 	{
 	 	    if(enfants.size()>nbLigne-1)
 	 	    {
	 	 	    int zDelta=GET_WHEEL_DELTA_WPARAM(wParam);
	 	 	    if(zDelta>0)
	 	 	    {
				    SCROLLINFO infoScroll;
			 	    infoScroll.cbSize=sizeof(infoScroll);
			 	    infoScroll.fMask=SIF_ALL;
		    		GetScrollInfo(hScroll,SB_CTL,&infoScroll);
		    		if(infoScroll.nPos==0)
  		    		{
    				    return 0;
					}
					infoScroll.nPos--;
					infoScroll.fMask = SIF_POS;
					if(infoScroll.nPos<0)infoScroll.nPos=0;
					SendMessage(hListboxNom,LB_SETTOPINDEX,infoScroll.nPos,0);
					SendMessage(hListboxClasse,LB_SETTOPINDEX,infoScroll.nPos,0);
					SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);
					ActualiserTabCombo(infoScroll.nPos);
				}
				else
				{
		 		    SCROLLINFO infoScroll;
			 	    infoScroll.cbSize=sizeof(infoScroll);
			 	    infoScroll.fMask=SIF_ALL;
		    		GetScrollInfo(hScroll,SB_CTL,&infoScroll);
		    		if(infoScroll.nPos==infoScroll.nMax-nbLigne+1)
  		    		{
    				    return 0;
					}
					infoScroll.nPos++;
					infoScroll.fMask = SIF_POS;
					SendMessage(hListboxNom,LB_SETTOPINDEX,infoScroll.nPos,0);
					SendMessage(hListboxClasse,LB_SETTOPINDEX,infoScroll.nPos,0);
					SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);
					ActualiserTabCombo(infoScroll.nPos);
				}
			}
 	 	    break;
	 	}
	 	case WM_CHANGECOLOR:
	 	{
   		    HDC hDC;
	  		hDC = GetWindowDC(hList);
		    HBRUSH couleur=CreateSolidBrush(RGB(255,50,50));
	  		FillRect(hDC,(LPRECT)lParam,couleur);
		    break;
   		    //DeleteDC(hDC);
	 	}
	 	case WM_KEYUP:
	 	{
 	 	    if(wParam==VK_TAB)
 	 	    {
	 	 	    if(hListboxEnCours==hListboxNom)
				{
				    RECT rect;
					SendMessage(hListboxClasse,LB_GETITEMRECT,iItemEdit,(LPARAM)&rect);
					//char a[4];
					//itoa(rect.bottom,a,10);
					char *contenuLigne=new char[SendMessage(hListboxClasse,LB_GETTEXTLEN,iItemEdit,0)];
					SendMessage(hListboxClasse,LB_GETTEXT,iItemEdit,(LPARAM)contenuLigne);
					//MessageBox(0,a,0,0);
					hEditList=CreateWindowEx(0,"edit",contenuLigne,WS_CHILD | WS_VISIBLE | WS_BORDER | ES_WANTRETURN | ES_AUTOHSCROLL,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,hListboxClasse,NULL,0,0);
					Edit_LimitText(hEditList,34);
					subclass_the_edit(hEditList);
					ShowWindow(hEditList,SW_SHOW);
					delete[] contenuLigne;
					hListboxEnCours=hListboxClasse;
					SetFocus(hEditList);
				}
				else if(hListboxEnCours==hListboxClasse)
				{
		 		    SetFocus(hWndMain);
		 		    AjoutTabStop();
				}
			}
			else if(wParam==VK_DELETE)
			{
			    int iItem=SendMessage(hList,LB_GETCURSEL,0,0);
			    int iEnfant=SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)SendMessage(hList,LB_GETCURSEL,0,0),0);
			    if(iItem==LB_ERR)
			    {
				    break;
				}
			    SendMessage(hListboxNom,LB_DELETESTRING,iItem,0);
			    SendMessage(hListboxClasse,LB_DELETESTRING,iItem,0);

		        int iItemTop=SendMessage(hListboxNom,LB_GETTOPINDEX,0,0)-1;
			    SendMessage(hListboxNom,LB_SETTOPINDEX,iItemTop,0);
			    SendMessage(hListboxClasse,LB_SETTOPINDEX,iItemTop,0);

	 		    if(enfants.size()>0)
	 		    {
	    		    enfants.erase(enfants.begin()+iEnfant);
	    		    for(int i=0;i<enfants.size();i++)
	    		    {
		 		        enfants[i].ref=i;
		 		        if(SendMessage(hListboxNom,LB_GETITEMDATA,i,0)>=iEnfant)
		 		        {
                            SendMessage(hListboxNom,LB_SETITEMDATA,i,SendMessage(hListboxNom,LB_GETITEMDATA,i,0)-1);
                            SendMessage(hListboxClasse,LB_SETITEMDATA,i,SendMessage(hListboxClasse,LB_GETITEMDATA,i,0)-1);
                        }
					}
				}

				if(enfants.size()<nbLigne)
				{
					RECT rect;
					GetWindowRect(hListboxNom,&rect);
					SetWindowPos(hListboxNom,0,0,0,rect.right-rect.left,rect.bottom-rect.top-23,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
					GetWindowRect(hListboxClasse,&rect);
					SetWindowPos(hListboxClasse,0,0,0,rect.right-rect.left,rect.bottom-rect.top-23,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
					GetWindowRect(hButtonAdd,&rect);
					POINT pt;
					pt.x=rect.left;
					pt.y=rect.top;
					ScreenToClient(hWndMain,&pt);
					SetWindowPos(hButtonAdd,0,pt.x,pt.y-23,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

					DestroyWindow(hComboV1.back());
					hComboV1.pop_back();
					DestroyWindow(hComboV2.back());
					hComboV2.pop_back();
					DestroyWindow(hComboV3.back());
					hComboV3.pop_back();
					EnableWindow(hScroll,false);
				}
				ActualiserTabCombo(iItemTop);

				SCROLLINFO infoScroll;
				infoScroll.cbSize=sizeof(SCROLLINFO);

				GetScrollInfo(hScroll,SB_CTL,&infoScroll);

				infoScroll.fMask=SIF_ALL;
				infoScroll.nMin=0;
				infoScroll.nMax=enfants.size();//rectHwnd.bottom-83;
				infoScroll.nPage=nbLigne;//infoScroll.nPage*nbLigne-1/nbLigne;//rectHwnd.bottom-83-22*(nbItem-nbLigne);
				infoScroll.nPos=iItemTop;
				SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);

				if(enfants.size()<nbLigne-1)
				{
				    EnableWindow(hScroll,false);
				}

	 			//status bar
				UpdateStatusBar(hstatu, enfants, hWndMain);
				//fin status bar
	 			int tLen=GetWindowTextLength(hWndMain)+2;
	  		    char *titreFenetre=new char[tLen];
	  		    GetWindowText(hWndMain,titreFenetre,tLen);
	  		    if(titreFenetre[strlen(titreFenetre)-1]!='*')
	  		    {
	  		        strcat(titreFenetre,"*");
				}
	  		    SetWindowText(hWndMain,titreFenetre);
	  		    delete[] titreFenetre;
	  		    ::isFileSaved=false;
			}
 	 	    break;
	 	}
	}
	return CallWindowProc(previous_boring_subclassed_listbox_proc, hList, msg, wParam, lParam);
}

LRESULT CALLBACK new_subclassed_edit_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
	    case WM_KEYUP:
		{
	 	    if(wParam==VK_RETURN)
	 	    {
	 		    //fLog<<"VK_RETURN"<<endl;
	 		    ::destroyEdit=true;
	 		    int iEnfant=SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)iItemEdit,0);

	 		    int lEditbox = GetWindowTextLength(hwnd)+1;
				char *sEditbox =new char[lEditbox];
	 		    GetWindowText(hwnd,sEditbox,lEditbox);

	 		    if(lEditbox==1)
	 		    {
	   			    if(MessageBox(hWndMain,"Attention, vous n'avez rien entré dans ce champ.\n\
Si le nom est vide, l'élève sera supprimé .\n\
Voulez-vous vraiment supprimer cet élève?","Champ vide",MB_ICONWARNING | MB_OKCANCEL)==IDOK)
		            {
			 		    DestroyWindow(hwnd);

			 		    for(int jour=0;jour<4;jour++)
					 	{
				 		    if(enfants.size()>0)
		 		    		{
				    		    enfants.erase(enfants.begin()+iEnfant);
				    		    for(int i=0;i<enfants.size();i++)
				    		    {
					 		        enfants[i].ref=i;
					 		        if(SendMessage(hListboxNom,LB_GETITEMDATA,i,0)>=iEnfant)
            		 		        {
                                        SendMessage(hListboxNom,LB_SETITEMDATA,i,SendMessage(hListboxNom,LB_GETITEMDATA,i,0)-1);
                                        SendMessage(hListboxClasse,LB_SETITEMDATA,i,SendMessage(hListboxClasse,LB_GETITEMDATA,i,0)-1);
                                    }
								}
							}
						}//fin for

						SendMessage(hListboxNom,LB_DELETESTRING,iItemEdit,0);
						SendMessage(hListboxClasse,LB_DELETESTRING,iItemEdit,0);

						int iItemTop=SendMessage(hListboxNom,LB_GETTOPINDEX,0,0)-1;
			    		SendMessage(hListboxNom,LB_SETTOPINDEX,iItemTop,0);
			    		SendMessage(hListboxClasse,LB_SETTOPINDEX,iItemTop,0);

						if(enfants.size()<nbLigne)
						{
							RECT rect;
							GetWindowRect(hListboxNom,&rect);
							SetWindowPos(hListboxNom,0,0,0,rect.right-rect.left,rect.bottom-rect.top-23,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
							GetWindowRect(hListboxClasse,&rect);
							SetWindowPos(hListboxClasse,0,0,0,rect.right-rect.left,rect.bottom-rect.top-23,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
							GetWindowRect(hButtonAdd,&rect);
							POINT pt;
							pt.x=rect.left;
							pt.y=rect.top;
							ScreenToClient(hWndMain,&pt);
							SetWindowPos(hButtonAdd,0,pt.x,pt.y-23,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

							DestroyWindow(hComboV1.back());
							hComboV1.pop_back();
							DestroyWindow(hComboV2.back());
							hComboV2.pop_back();
							DestroyWindow(hComboV3.back());
							hComboV3.pop_back();
							EnableWindow(hScroll,false);
						}
						ActualiserTabCombo(iItemTop);

						SCROLLINFO infoScroll;
						infoScroll.cbSize=sizeof(SCROLLINFO);

						GetScrollInfo(hScroll,SB_CTL,&infoScroll);

						infoScroll.fMask=SIF_ALL;
						infoScroll.nMin=0;
						infoScroll.nMax=enfants.size();//rectHwnd.bottom-83;
						infoScroll.nPage=nbLigne;//infoScroll.nPage*nbLigne-1/nbLigne;//rectHwnd.bottom-83-22*(nbItem-nbLigne);
						infoScroll.nPos=iItemTop;
						SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);

						if(enfants.size()<nbLigne-1)
						{
						    EnableWindow(hScroll,false);
						}

						//status bar
						UpdateStatusBar(hstatu, enfants, hWndMain);
						//fin status bar

			  		    int tLen=GetWindowTextLength(hWndMain)+2;
			  		    char *titreFenetre=new char[tLen];
			  		    GetWindowText(hWndMain,titreFenetre,tLen);
			  		    if(titreFenetre[strlen(titreFenetre)-1]!='*')
			  		    {
			  		        strcat(titreFenetre,"*");
						}
			  		    SetWindowText(hWndMain,titreFenetre);
			  		    delete[] titreFenetre;
			  		    ::isFileSaved=false;
					}
					AjoutTabStop();
					return 0;
				}//fin supprimer éleve
	 		    SendMessage(hListboxEnCours,LB_DELETESTRING,iItemEdit,0);

	 		    SendMessage(hListboxEnCours,LB_INSERTSTRING,iItemEdit,(LPARAM)sEditbox);
	 		    SendMessage(hListboxEnCours,LB_SETITEMDATA,iItemEdit, (LPARAM)iEnfant);
	 		    //MessageBox(0,sEditbox,0,0);
	 		    DestroyWindow(hwnd);

	 		    if(hListboxEnCours==hListboxNom)
	 		    {
  		    	    enfants[iEnfant].nom=string(sEditbox);
				}
				else if(hListboxEnCours==hListboxClasse)
				{
    			    enfants[iEnfant].classe=string(sEditbox);
				}

				delete[] sEditbox;

				UpdateStatusBar(hstatu, enfants, hWndMain);

	  		    int tLen=GetWindowTextLength(hWndMain)+2;
	  		    char *titreFenetre=new char[tLen];
	  		    GetWindowText(hWndMain,titreFenetre,tLen);
	  		    if(titreFenetre[strlen(titreFenetre)-1]!='*')
	  		    {
	  		        strcat(titreFenetre,"*");
				}
	  		    SetWindowText(hWndMain,titreFenetre);
	  		    delete[] titreFenetre;
	  		    ::isFileSaved=false;
				/*else
				{
				 	int lEditbox = GetWindowTextLength(hwnd)+1;
					char *sEditbox =new char[lEditbox];
		 		    GetWindowText(hwnd,sEditbox,lEditbox);

		 		    SendMessage(hListboxNom,LB_DELETESTRING,iItemEdit,0);
		 		    SendMessage(hListboxNom,LB_INSERTSTRING,iItemEdit,(LPARAM)sEditbox);
		 		    //MessageBox(0,sEditbox,0,0);
		 		    DestroyWindow(hwnd);
		 		    delete[] sEditbox;

		 		    bool presentAutreJour=false;

		 		    for(int i=0;i<4;i++)
		 		    {
		 			    if(i!=jourEnCourSaisie)
		 			    {
			 			    for(int j=0;j<itemReferenceEleve[i].size();j++)
			 			    {
				 		        if(itemReferenceEleve[i][j].ref==itemReferenceEleve[jourEnCourSaisie][iItemEdit].ref)
				 		        {
						 		    presentAutreJour=true;
						 		}
							}
						}
					}
					if(presentAutreJour==true)
					{
		   			    ::nbElevesTotal++;
		   			    itemReferenceEleve[jourEnCourSaisie][iItemEdit].ref=nbElevesTotal;
		   			    SendMessage(hListboxNom,LB_SETITEMDATA,iItemEdit,nbElevesTotal);
					}
		 		    //SendMessage(hListboxNom,LB_SETITEMDATA,iItemEdit,);
				 	::changeNom=false;
				}*/
				AjoutTabStop();
				break;
	 		}
	 		if(wParam==VK_TAB)
	 	    {
		  	    //fLog<<"VK_TAB"<<endl;
		  	    ::destroyEdit=true;
		  	    /*LONG lStyle = GetWindowLongPtr(hButtonAdd, GWL_STYLE);
			    lStyle |= WS_TABSTOP;
			    SetWindowLongPtr(hButtonAdd, GWL_STYLE, lStyle);

			    if(hComboV1.size()!=0)
			    {
			        lStyle = GetWindowLongPtr(hComboV1[0], GWL_STYLE);
	  		 	    lStyle |=WS_TABSTOP;
				}
			    for(int i=0;i<hComboV1.size();i++)
			    {
		    	 	SetWindowLongPtr(hComboV1[i], GWL_STYLE, lStyle);
		    	 	SetWindowLongPtr(hComboV2[i], GWL_STYLE, lStyle);
		    	 	SetWindowLongPtr(hComboV3[i], GWL_STYLE, lStyle);
				}*/

				int iEnfant=SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)iItemEdit,0);

	 		    int lEditbox = GetWindowTextLength(hwnd)+1;
				char *sEditbox =new char[lEditbox];
	 		    GetWindowText(hwnd,sEditbox,lEditbox);

	 		    if(lEditbox==1)
	 		    {
	   			    if(MessageBox(hWndMain,"Attention, vous n'avez rien entré dans ce champ.\n\
Si le nom est vide, l'élève sera supprimé .\n\
Voulez-vous vraiment supprimer cet élève?","Champ vide",MB_ICONWARNING | MB_OKCANCEL)==IDOK)
		            {
			 		    DestroyWindow(hwnd);
			 		    if(enfants.size()>0)
	 		    		{
			    		    enfants.erase(enfants.begin()+iEnfant);
			    		    for(int i=0;i<enfants.size();i++)
			    		    {
				 		        enfants[i].ref=i;
				 		        if(SendMessage(hListboxNom,LB_GETITEMDATA,i,0)>=iEnfant)
        		 		        {
                                    SendMessage(hListboxNom,LB_SETITEMDATA,i,SendMessage(hListboxNom,LB_GETITEMDATA,i,0)-1);
                                    SendMessage(hListboxClasse,LB_SETITEMDATA,i,SendMessage(hListboxClasse,LB_GETITEMDATA,i,0)-1);
                                }
							}
						}

						SendMessage(hListboxNom,LB_DELETESTRING,iItemEdit,0);
						SendMessage(hListboxClasse,LB_DELETESTRING,iItemEdit,0);

						int iItemTop=SendMessage(hListboxNom,LB_GETTOPINDEX,0,0)-1;
			    		SendMessage(hListboxNom,LB_SETTOPINDEX,iItemTop,0);
			    		SendMessage(hListboxClasse,LB_SETTOPINDEX,iItemTop,0);

						if(enfants.size()<nbLigne)
						{
							RECT rect;
							GetWindowRect(hListboxNom,&rect);
							SetWindowPos(hListboxNom,0,0,0,rect.right-rect.left,rect.bottom-rect.top-23,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
							GetWindowRect(hListboxClasse,&rect);
							SetWindowPos(hListboxClasse,0,0,0,rect.right-rect.left,rect.bottom-rect.top-23,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
							GetWindowRect(hButtonAdd,&rect);
							POINT pt;
							pt.x=rect.left;
							pt.y=rect.top;
							ScreenToClient(hWndMain,&pt);
							SetWindowPos(hButtonAdd,0,pt.x,pt.y-23,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

							DestroyWindow(hComboV1.back());
							hComboV1.pop_back();
							DestroyWindow(hComboV2.back());
							hComboV2.pop_back();
							DestroyWindow(hComboV3.back());
							hComboV3.pop_back();
							EnableWindow(hScroll,false);
						}
						ActualiserTabCombo(iItemTop);

						SCROLLINFO infoScroll;
						infoScroll.cbSize=sizeof(SCROLLINFO);

						GetScrollInfo(hScroll,SB_CTL,&infoScroll);

						infoScroll.fMask=SIF_ALL;
						infoScroll.nMin=0;
						infoScroll.nMax=enfants.size();//rectHwnd.bottom-83;
						infoScroll.nPage=nbLigne;//infoScroll.nPage*nbLigne-1/nbLigne;//rectHwnd.bottom-83-22*(nbItem-nbLigne);
						infoScroll.nPos=iItemTop;
						SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);

						if(enfants.size()<nbLigne-1)
						{
						    EnableWindow(hScroll,false);
						}
						//status bar
						UpdateStatusBar(hstatu, enfants, hWndMain);
						//fin status bar

			  		    int tLen=GetWindowTextLength(hWndMain)+2;
			  		    char *titreFenetre=new char[tLen];
			  		    GetWindowText(hWndMain,titreFenetre,tLen);
			  		    if(titreFenetre[strlen(titreFenetre)-1]!='*')
			  		    {
			  		        strcat(titreFenetre,"*");
						}
			  		    SetWindowText(hWndMain,titreFenetre);
			  		    delete[] titreFenetre;
			  		    ::isFileSaved=false;
					}
					AjoutTabStop();
					return 0;
				}
	 		    SendMessage(hListboxEnCours,LB_DELETESTRING,iItemEdit,0);

	 		    SendMessage(hListboxEnCours,LB_INSERTSTRING,iItemEdit,(LPARAM)sEditbox);
	 		    SendMessage(hListboxEnCours,LB_SETITEMDATA,iItemEdit, (LPARAM)iEnfant);
	 		    //MessageBox(0,sEditbox,0,0);
	 		    DestroyWindow(hwnd);

	 		    if(hListboxEnCours==hListboxNom)
	 		    {
  		    	    enfants[iEnfant].nom=string(sEditbox);
				}
				else if(hListboxEnCours==hListboxClasse)
				{
    			    enfants[iEnfant].classe=string(sEditbox);
				}

				delete[] sEditbox;

				UpdateStatusBar(hstatu, enfants, hWndMain);

	  		    int tLen=GetWindowTextLength(hWndMain)+2;
	  		    char *titreFenetre=new char[tLen];
	  		    GetWindowText(hWndMain,titreFenetre,tLen);
	  		    if(titreFenetre[strlen(titreFenetre)-1]!='*')
	  		    {
	  		        strcat(titreFenetre,"*");
				}
	  		    SetWindowText(hWndMain,titreFenetre);
	  		    delete[] titreFenetre;
	  		    ::isFileSaved=false;
	  		    //sélectionne la case de droite

				if(hListboxEnCours==hListboxNom)
				{
				    RECT rect;
					SendMessage(hListboxClasse,LB_GETITEMRECT,iItemEdit,(LPARAM)&rect);
					//char a[4];
					//itoa(rect.bottom,a,10);
					char *contenuLigne=new char[SendMessage(hListboxClasse,LB_GETTEXTLEN,iItemEdit,0)];
					SendMessage(hListboxClasse,LB_GETTEXT,iItemEdit,(LPARAM)contenuLigne);
					//MessageBox(0,a,0,0);
					hEditList=CreateWindowEx(0,"edit",contenuLigne,WS_CHILD | WS_VISIBLE | WS_BORDER | ES_WANTRETURN | ES_AUTOHSCROLL,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,hListboxClasse,NULL,0,0);
					Edit_LimitText(hEditList,34);
					subclass_the_edit(hEditList);
					ShowWindow(hEditList,SW_SHOW);
					delete[] contenuLigne;
					hListboxEnCours=hListboxClasse;
				}
	  		    SetFocus(hEditList);
			}
	 	    break;
        }
        case WM_KILLFOCUS:
		{
	 	    //fLog<<"WM_KILLFOCUS"<<endl;
	 	    if(destroyEdit==true)
	 	    {
	 		    ::destroyEdit=false;
	 		    break;
	 		}

	 	    /*LONG lStyle = GetWindowLongPtr(hButtonAdd, GWL_STYLE);
		    lStyle |= WS_TABSTOP;
		    SetWindowLongPtr(hButtonAdd, GWL_STYLE, lStyle);

		    if(hComboV1.size()!=0)
		    {
		        lStyle = GetWindowLongPtr(hComboV1[0], GWL_STYLE);
  		 	    lStyle |=WS_TABSTOP;
			}
		    for(int i=0;i<hComboV1.size();i++)
		    {
	    	 	SetWindowLongPtr(hComboV1[i], GWL_STYLE, lStyle);
	    	 	SetWindowLongPtr(hComboV2[i], GWL_STYLE, lStyle);
	    	 	SetWindowLongPtr(hComboV3[i], GWL_STYLE, lStyle);
			}*/
 	 		int iEnfant=SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)iItemEdit,0);
 		    int lEditbox = GetWindowTextLength(hwnd)+1;
			char *sEditbox =new char[lEditbox];
 		    GetWindowText(hwnd,sEditbox,lEditbox);

 		    if(lEditbox==1)
 		    {
   			    if(MessageBox(hWndMain,"Attention, vous n'avez rien entré dans ce champ.\n\
Si le nom est vide, l'élève sera supprimé .\n\
Voulez-vous vraiment supprimer cet élève?","Champ vide",MB_ICONWARNING | MB_OKCANCEL)==IDOK)
	            {
		 		    DestroyWindow(hwnd);

		 		    if(enfants.size()>0)
	 		    	{
		    		    enfants.erase(enfants.begin()+iEnfant);
		    		    for(int i=0;i<enfants.size();i++)
		    		    {
			 		        enfants[i].ref=i;
			 		        if(SendMessage(hListboxNom,LB_GETITEMDATA,i,0)>=iEnfant)
    		 		        {
                                SendMessage(hListboxNom,LB_SETITEMDATA,i,SendMessage(hListboxNom,LB_GETITEMDATA,i,0)-1);
                                SendMessage(hListboxClasse,LB_SETITEMDATA,i,SendMessage(hListboxClasse,LB_GETITEMDATA,i,0)-1);
                            }
						}
					}

					SendMessage(hListboxNom,LB_DELETESTRING,iItemEdit,0);
					SendMessage(hListboxClasse,LB_DELETESTRING,iItemEdit,0);

					int iItemTop=SendMessage(hListboxNom,LB_GETTOPINDEX,0,0)-1;
		    		SendMessage(hListboxNom,LB_SETTOPINDEX,iItemTop,0);
		    		SendMessage(hListboxClasse,LB_SETTOPINDEX,iItemTop,0);

					if(enfants.size()<nbLigne)
					{
						RECT rect;
						GetWindowRect(hListboxNom,&rect);
						SetWindowPos(hListboxNom,0,0,0,rect.right-rect.left,rect.bottom-rect.top-23,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
						GetWindowRect(hListboxClasse,&rect);
						SetWindowPos(hListboxClasse,0,0,0,rect.right-rect.left,rect.bottom-rect.top-23,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
						GetWindowRect(hButtonAdd,&rect);
						POINT pt;
						pt.x=rect.left;
						pt.y=rect.top;
						ScreenToClient(hWndMain,&pt);
						SetWindowPos(hButtonAdd,0,pt.x,pt.y-23,0,0,SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

						DestroyWindow(hComboV1.back());
						hComboV1.pop_back();
						DestroyWindow(hComboV2.back());
						hComboV2.pop_back();
						DestroyWindow(hComboV3.back());
						hComboV3.pop_back();
						EnableWindow(hScroll,false);
					}
					ActualiserTabCombo(iItemTop);

					SCROLLINFO infoScroll;
					infoScroll.cbSize=sizeof(SCROLLINFO);

					GetScrollInfo(hScroll,SB_CTL,&infoScroll);

					infoScroll.fMask=SIF_ALL;
					infoScroll.nMin=0;
					infoScroll.nMax=enfants.size();//rectHwnd.bottom-83;
					infoScroll.nPage=nbLigne;//infoScroll.nPage*nbLigne-1/nbLigne;//rectHwnd.bottom-83-22*(nbItem-nbLigne);
					infoScroll.nPos=iItemTop;
					SetScrollInfo(hScroll,SB_CTL,&infoScroll,true);

					if(enfants.size()<nbLigne-1)
					{
					    EnableWindow(hScroll,false);
					}

					//status bar
					UpdateStatusBar(hstatu, enfants, hWndMain);

		  		    int tLen=GetWindowTextLength(hWndMain)+2;
		  		    char *titreFenetre=new char[tLen];
		  		    GetWindowText(hWndMain,titreFenetre,tLen);
		  		    if(titreFenetre[strlen(titreFenetre)-1]!='*')
		  		    {
		  		        strcat(titreFenetre,"*");
					}
		  		    SetWindowText(hWndMain,titreFenetre);
		  		    delete[] titreFenetre;
		  		    ::isFileSaved=false;
		  		    //fin status Bar
				}
				AjoutTabStop();
				return CallWindowProc(previous_boring_subclassed_edit_proc, hwnd, msg, wParam, lParam);
			}
 		    SendMessage(hListboxEnCours,LB_DELETESTRING,iItemEdit,0);

 		    SendMessage(hListboxEnCours,LB_INSERTSTRING,iItemEdit,(LPARAM)sEditbox);
 		    SendMessage(hListboxEnCours,LB_SETITEMDATA,iItemEdit, (LPARAM)iEnfant);
 		    //MessageBox(0,sEditbox,0,0);
 		    DestroyWindow(hwnd);

 		    if(hListboxEnCours==hListboxNom)
 		    {
 		        enfants[iEnfant].nom=string(sEditbox);
			}
			else if(hListboxEnCours==hListboxClasse)
			{
   			    enfants[iEnfant].classe=string(sEditbox);
			}

			delete[] sEditbox;

			//status bar
			UpdateStatusBar(hstatu, enfants, hWndMain);
			//fin status bar

  		    int tLen=GetWindowTextLength(hWndMain)+2;
  		    char *titreFenetre=new char[tLen];
  		    GetWindowText(hWndMain,titreFenetre,tLen);
  		    if(titreFenetre[strlen(titreFenetre)-1]!='*')
  		    {
  		        strcat(titreFenetre,"*");
			}
  		    SetWindowText(hWndMain,titreFenetre);
  		    delete[] titreFenetre;
  		    ::isFileSaved=false;
  		    AjoutTabStop();
			break;
		}
        default:
				return CallWindowProc(previous_boring_subclassed_edit_proc, hwnd, msg, wParam, lParam);
	}
	return CallWindowProc(previous_boring_subclassed_edit_proc, hwnd, msg, wParam, lParam);
}
LRESULT CALLBACK new_subclassed_comboActiEdit(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
	    /*case WM_KEYDOWN:
		{
	 	    MessageBox(0,0,0,0);
	 	    if(wParam==VK_RETURN)
	 	    {
	 		    MessageBox(0,0,"1",0);
	 		    return 0;
			}
			//MessageBox(0,0,0,0);
			break;
		}*/
		/*case WM_CHAR:
	 	{
 	 	    MessageBox(0,0,0,0);
		}*/
		/*case WM_KEYDOWN:
            switch (wParam)
            {
                case VK_TAB:
                    //SendMessage(hwndMain, WM_TAB, 0, 0);
                    MessageBox(0,0,0,0);
                    return 0;
                case VK_ESCAPE:
                    //SendMessage(hwndMain, WM_ESC, 0, 0);
                    MessageBox(0,0,0,0);
                    return 0;
                case VK_RETURN:
                    //SendMessage(hwndMain, WM_ENTER, 0, 0);
                    MessageBox(0,0,0,0);
                    return 0;
            }
            break; */
 		/*case WM_PAINT:
		{
	 	    HDC hdc = (HDC)wParam;
	 		SetBkColor(hdc, RGB(0,150,255));
	 	    //return (INT_PTR)CreateSolidBrush(RGB(0,150,255));
	 	    break;
		}*/
   		/*case WM_PAINT:
		{
  		    PAINTSTRUCT ps;
        	HDC hdc ;
			hdc = BeginPaint(hwnd, &ps);
		    //hpen = CreatePen(PS_SOLID, 1, 0x00FF0000);
			//SelectObject(hdc,hpen);

			//SetBkMode(hdc, TRANSPARENT);

			RECT rect;
			GetClientRect(hwnd,&rect);
			HBRUSH couleur=CreateSolidBrush(RGB(139,255,139));
			FillRect(hdc,&rect,couleur);
			SetBkColor(hdc, RGB(139,255,139));
			SetDCBrushColor(hdc,RGB(139,255,139));

			EndPaint(hwnd, &ps);
			//MessageBox(0,0,0,0);
			return 0;
		}*/
		case WM_SETFOCUS:
	 	{
 	 	    ::editActiActivate=true;
 	 	    break;
	 	}
	 	case WM_KILLFOCUS:
	 	{
 	 	    ::nouvActi=false;
 	 	    ::editActiActivate=false;

 	 	    int lEditbox = GetWindowTextLength(hwnd)+1;
 		    if(lEditbox==1)
 		    {
 			    if(MessageBox(hWndMain,"Laisser cette case vide supprimera cette activité.\nVoulez vous vraiment la supprimer ?","Suppression de cette activité ?",MB_YESNO | MB_ICONWARNING)==IDYES)
 			    {
				    if(activite.size()!=0)
				    activite.erase(activite.begin()+iActiEnCour);
				}

		   	    break;
			}
			LPSTR sEditbox =(LPSTR)malloc(lEditbox);
			GetWindowText(hwnd,sEditbox,lEditbox);

			/*int iItemNom=SendMessage(hComboActi,CB_FINDSTRINGEXACT,(WPARAM)-1,(LPARAM)sEditbox);
		    if(iItemNom!=CB_ERR)
			{
	  		    SendMessage(hComboActi,CB_SETCURSEL,iItemNom,0);
			}
			else
			{*/

			if(activite.size()==0)
			{
	  		    InfoActivite actiInter;
	  		    activite.push_back(actiInter);
			}
			activite[iActiEnCour].nom=string(sEditbox);
   				//SendMessage(hComboActi,CB_ADDSTRING,(WPARAM) 0,(LPARAM) sEditbox);
			//}
			::iItemSel=iActiEnCour;

			free(sEditbox);

 	 	    break;
	 	}
        case WM_KEYUP:
 		{
   		    if(wParam==VK_RETURN)
	 	    {
			    SetFocus(hEditUpDown);
	 		    break;
			}
           /* switch (wParam)
            {
                case VK_TAB:
                case VK_ESCAPE:
                case VK_RETURN:
					 MessageBox(0,0,0,0);
                    return 0;
            } */
		}
    }
    return CallWindowProc(previous_boring_subclassed_comboActiEdit, hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK new_subclassed_comboBoxActi(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
	    /*case WM_PAINT:
		{

	 	    /*HDC hDC;
			hDC = GetWindowDC(hwnd);
			PAINTSTRUCT Ps;
			//hDC = BeginPaint(hwnd, &Ps);

			RECT rect;
			GetClientRect(hwnd,&rect);
			rect.left++;
			rect.top++;
			rect.right-=18;
			rect.bottom--;
			HBRUSH couleur=CreateSolidBrush(RGB(255,50,50));
			FillRect(hDC,&rect,couleur);*/

			//EndPaint(hwnd, &Ps);
	 	    /*break;
		}*/

		case WM_CTLCOLOREDIT:
	 	{
 	 	    if(editActiActivate==true)
 	 	    {
	 	 	    if(nouvActi==false)
	 	 	    {
	 	 	        SetBkColor((HDC)wParam, RGB(255,165,1));
	 	 	        return (LRESULT)CreateSolidBrush(RGB(255,165,1));
				}
				else if(nouvActi==true)
				{
	 	 		    SetBkColor((HDC)wParam, RGB(139,255,139));
	 	 	        return (LRESULT)CreateSolidBrush(RGB(139,255,139));
				}
			}
 	 	    break;
	 	}
		default:
		{
		    return CallWindowProc(previous_boring_subclassed_comboBoxActi, hwnd, msg, wParam, lParam);
		}
	}
    return CallWindowProc(previous_boring_subclassed_comboBoxActi, hwnd, msg, wParam, lParam);
}
LRESULT CALLBACK new_subclassed_editGoLine_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
	    case WM_KEYUP:
 		{
   		    if(wParam==VK_RETURN)
	 	    {
		 	    SendMessage(hWndMain,WM_COMMAND,LOWORD(ID_BUTTONGOLINE),0);
		 	}
		 	break;
		}
	    default:
		{
		    return CallWindowProc(previous_boring_subclassed_editGoLine_proc, hwnd, msg, wParam, lParam);
		}
	}
    return CallWindowProc(previous_boring_subclassed_editGoLine_proc, hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK new_subclassed_editFindName_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
	    case WM_KEYUP:
 		{
   		    if(wParam==VK_RETURN)
	 	    {
		 	    SendMessage(hWndMain,WM_COMMAND,LOWORD(ID_BUTTONFINDNAME),0);
		 	}
		 	break;
		}
	    default:
		{
		    return CallWindowProc(previous_boring_subclassed_editFindName_proc, hwnd, msg, wParam, lParam);
		}
	}
    return CallWindowProc(previous_boring_subclassed_editFindName_proc, hwnd, msg, wParam, lParam);
}
LRESULT CALLBACK new_subclassed_editComboAjoutClasse_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
	    case WM_KEYUP:
 		{
   		    if(wParam==VK_RETURN)
	 	    {
		 	    int lEditbox = GetWindowTextLength(hwnd)+1;
		 	    if(lEditbox==1)
		 	    {
 			   	    SetFocus(hWndMain);
 			   	    break;
				}
		 	    char *sEditbox =new char[lEditbox];
   				GetWindowText(hwnd,sEditbox,lEditbox);
		 	    SendMessage(hwnd,CB_ADDSTRING,(WPARAM) 0,(LPARAM)sEditbox);
		 	    bool classeNonPresente=true;
		 	    for(int j=0;j<nomClasses.size();j++)
				{
				    if(nomClasses[j].compare(sEditbox)==0)
				 	{
					    classeNonPresente=false;
				        break;
				 	}
				}
				if(classeNonPresente)
				{
				    nomClasses.push_back(string(sEditbox));
				}
		 	    delete[]sEditbox;
		 	    //SetFocus(hWndMain);
	 	     	SendMessage(hWndMain,WM_COMMAND,MAKEWPARAM(ID_COMBOAPPCLASS,CBN_KILLFOCUS),(LPARAM)hwnd);
		 	    SendMessage(hWndMain,WM_COMMAND,MAKEWPARAM(ID_COMBOAPPCLASS,CBN_SETFOCUS),(LPARAM)hwnd);
		 	}
		 	break;
		}
	    default:
		{
		    return CallWindowProc(previous_boring_subclassed_editComboAjoutClasse_proc, hwnd, msg, wParam, lParam);
		}
	}
    return CallWindowProc(previous_boring_subclassed_editComboAjoutClasse_proc, hwnd, msg, wParam, lParam);
}
LRESULT CALLBACK new_subclassed_ComboVoeu_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        /*case WM_PAINT:
        {
            PAINTSTRUCT paint;
            BeginPaint(hwnd,&paint);
            //SetBkColor((HDC)wParam, RGB(49,106,197));
            //SetTextColor((HDC)wParam, RGB(255,255,255));
            EndPaint(hwnd,&paint);
            break;
        }*/
        case WM_MOUSEWHEEL:
        {
            PostMessage(hWndMain,WM_MOUSEWHEEL,wParam,lParam);
            return 0;
        }
        case WM_CTLCOLOREDIT:
	 	{
 	 	    //if(nouvActi==false)
 	 	    //{
 	 	        SetBkColor((HDC)wParam, RGB(49,106,197));
 	 	        SetTextColor((HDC)wParam, RGB(255,255,255));
 	 	        return (LRESULT)CreateSolidBrush(RGB(49,106,197));
			/*}
			else if(nouvActi==true)
			{
 	 		    SetBkColor((HDC)wParam, RGB(49,106,197));
 	 	        return (LRESULT)CreateSolidBrush(RGB(139,255,139));
			}*/
 	 	    break;
	 	}
        default:
		{
		    return CallWindowProc(previous_boring_subclassed_ComboVoeu_proc, hwnd, msg, wParam, lParam);
		}
	}
    return CallWindowProc(previous_boring_subclassed_ComboVoeu_proc, hwnd, msg, wParam, lParam);
}

/***************************************************************Sortie des résultats*******************************************************/


int SortieMain()
{
 	//fLog<<"Début SortieMain\n";
 	DialogBox(hinstance, "DIALOGSORTIE" , 0, (DLGPROC)DialogSortieProc);
 	//fLog<<"Fin SortieMain\n";
 	//enfantsAttributed
    return 0;
}
HWND CreateListView (HWND hwndParent,int x=0,int y=0,int width=100,int height=100)
{
    InitCommonControls();

    RECT rcClient;                       // The parent window's client area.

    GetClientRect (hwndParent, &rcClient);

    // Create the list-view window in report view with label editing enabled.
    HWND hWndListView = CreateWindowEx(0,WC_LISTVIEW, "",WS_VISIBLE | WS_CHILD | LVS_REPORT ,(rcClient.right - rcClient.left)*x/100, (rcClient.bottom - rcClient.top)*y/100,((rcClient.right - rcClient.left)*width/100)-1,(rcClient.bottom - rcClient.top)*height/100,hwndParent,(HMENU)ID_LISTVIEW ,0,NULL);

    return (hWndListView);
}

HWND CreateListView (HWND hwndParent, bool headerClickable)
{
    InitCommonControls();

    RECT rcClient;                       // The parent window's client area.

    GetClientRect (hwndParent, &rcClient);

    // Create the list-view window in report view with label editing enabled.
    HWND hWndListView = CreateWindowEx(0,WC_LISTVIEW, "",WS_VISIBLE | WS_CHILD | LVS_REPORT ,0, 0,((rcClient.right - rcClient.left))-1,(rcClient.bottom - rcClient.top),hwndParent,(HMENU)ID_LISTVIEW ,0,NULL);

	if(!headerClickable)
	{
	    HWND hHeader=ListView_GetHeader(hWndListView);

	    //enlever le style WS_TABSTOP aux combos et au boutton
	    SetWindowLongPtr(hHeader, GWL_STYLE, GetWindowLongPtr(hHeader, GWL_STYLE)&~HDS_BUTTONS);
	}

    return (hWndListView);
}

template <class T> void Trier(T& liste, int champ)
{
    for(int i=0;i<liste.size()-1;)
	{
	    if(stricmp(liste[i].tableau[champ],liste[i+1].tableau[champ])>0)//2 doit être avant 1
	    {
	        swap(liste[i],liste[i+1]);
	        if(i>0)i--;
		}
		else if(i<liste.size()-2)
		{
 		    i++;
		}
	}
}

int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static HWND hListView;
BOOL APIENTRY DialogSortieProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:
      	{
	 	    //création du menu
	 	    HMENU menu = LoadMenu(hinstance, "MENUDIALOG");//MAKEINTRESOURCE(ID_MENU));
	 	    SetMenu(hDlg,menu);
    	    //Création de la ListView
			hListView=CreateListView(hDlg);
			//Création des colones
			LVCOLUMN lvc;
    		int iCol=0;
    		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT ;
    		lvc.iSubItem = 0;
        	lvc.pszText = (LPSTR)"Nom Prénom";
        	lvc.cx = 100;
        	lvc.fmt = LVCFMT_LEFT;
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 1;
        	lvc.pszText = (LPSTR)"Classe";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 2;
        	lvc.cx = 150;
        	lvc.pszText = (LPSTR)"Affectation Lundi";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 3;
        	lvc.cx = 100;
        	lvc.pszText = (LPSTR)"Voeu attribué Lundi";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 4;
        	lvc.pszText = (LPSTR)"Affectation Mardi";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 5;
        	lvc.cx = 150;
        	lvc.pszText = (LPSTR)"Voeu attribué Mardi";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 6;
        	lvc.cx = 100;
        	lvc.pszText = (LPSTR)"Affectation Jeudi";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 7;
        	lvc.cx = 150;
        	lvc.pszText = (LPSTR)"Voeu attribué Jeudi";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 8;
        	lvc.pszText = (LPSTR)"Affectation Vendredi";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 9;
        	lvc.pszText = (LPSTR)"Voeu attribué Vendredi";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	//Création des lignes
        	LVITEM lvI={0};
 			//lvI.mask      = LVIF_TEXT |LVIF_STATE | LVIF_PARAM;
	  		//lvI.stateMask = 0;
	  		//lvI.iSubItem  = 0;
	  		//lvI.state     = 0;
			//lvI.iItem  = 0;
			/*for(int i=0;i<enfants[0].size();i++)
			{
 		        lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
 		        lvI.iItem  = i;
 		        lvI.lParam=i;
 		        lvI.iSubItem  = 0;
 		        lvI.pszText=" ";
 		        ListView_InsertItem(hListView, &lvI);
 		        ListView_SetItemText(hListView,i,0,enfants[0][i].np);
			}*/
			//fLog<<"Avant boucle dans WM_INITDIALOG\n";
			int compteurItem=-1;
			for(int i=0;i<enfantsAttributed.size();i++)//for(int i=0;i<nbElevesTotal-1;i++)
			{

 			    if(enfantsAttributed[i].denominateur==0)
 			    {
		 		    continue;
		 		}
 			    compteurItem++;
 			    //fLog<<"Dans boucle : "<<i<<endl;
 		        lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
 		        lvI.iItem  = compteurItem;
 		        lvI.lParam=compteurItem;
 		        lvI.iSubItem  = 0;
 		        lvI.pszText= (LPSTR)"";
 		        ListView_InsertItem(hListView, &lvI);

				/*char nom[100]={0};
				char *classe;
				classe=strrchr(enfants[0][i].np,' ');
				for(int j=0;j<classe-enfants[0][i].np+1;j++)
				{
	 			    nom[j]=enfants[0][i].np[j];
				}
				//strcpy(nom,enfants[0][i].np);
				ListView_SetItemText(hListView,i,0, nom);
				ListView_SetItemText(hListView,i,1, classe);
				ListView_SetItemText(hListView,i,2, (enfants[0][i].affectation!=-1)?activite[enfants[0][i].affectation].nom:(char*)"Pas d'attribution");
				if(enfants[0][i].affectation==enfants[0][i].voeu1)
				{
				    ListView_SetItemText(hListView,i,3, "1");
				}
				else if(enfants[0][i].affectation==enfants[0][i].voeu2)
				{
			 	    ListView_SetItemText(hListView,i,3, "2");
				}
				else if(enfants[0][i].affectation==enfants[0][i].voeu3)
				{
			 	    ListView_SetItemText(hListView,i,3, "3");
				}
				else
				{
				 	ListView_SetItemText(hListView,i,3, "-");
				}*/
				/*char a[4];
				itoa(enfantsAttributed[0].size(),a,10);
				MessageBox(0,a,"1",0);*/

				ListView_SetItemText(hListView,compteurItem,0, const_cast<char*>(enfantsAttributed[i].nom.c_str()));//const_cast<char*>(buffer.c_str()));
				ListView_SetItemText(hListView,compteurItem,1, const_cast<char*>(enfantsAttributed[i].classe.c_str()));

				for(int jour=0;jour<4;jour++)
				{
	 			    if(enfantsAttributed[i].affectation[jour]==-1)
	 			    {
		  			    if(enfantsAttributed[i].voeux[jour].voeu1==-1 && enfantsAttributed[i].voeux[jour].voeu2==-1 && enfantsAttributed[i].voeux[jour].voeu3==-1)
		  			    {
		  				    ListView_SetItemText(hListView,compteurItem,2+jour*2, (char*)"Absent");
		  				}
		  				else
		  				{
					        ListView_SetItemText(hListView,compteurItem,2+jour*2, (char*)"Pas d'attribution");
						}
					}
					else
					{
			 		    ListView_SetItemText(hListView,compteurItem,2+jour*2, const_cast<char*>(activite[enfantsAttributed[i].affectation[jour]].nom.c_str()));
					}
		   			if(enfantsAttributed[i].affectation[jour]==-1)
				    {
					    ListView_SetItemText(hListView,compteurItem,3+jour*2, "-");
					}
		   			else if(enfantsAttributed[i].affectation[jour]==enfantsAttributed[i].voeux[jour].voeu1)
					{
		 	 		    ListView_SetItemText(hListView,compteurItem,3+jour*2, "1");
					}
					else if(enfantsAttributed[i].affectation[jour]==enfantsAttributed[i].voeux[jour].voeu2)
					{
		 	 		    ListView_SetItemText(hListView,compteurItem,3+jour*2, "2");
					}
					else if(enfantsAttributed[i].affectation[jour]==enfantsAttributed[i].voeux[jour].voeu3)
					{
		 	 		    ListView_SetItemText(hListView,compteurItem,3+jour*2, "3");
					}
					else
					{
			 		    ListView_SetItemText(hListView,compteurItem,3+jour*2, "-");
					}

					char textLigne[100];
					ListView_GetItemText(hListView,compteurItem,2+jour*2,textLigne,100);
					if(strcmp(textLigne,"")==0)
					{
		   			    ListView_SetItemText(hListView,compteurItem,2+jour*2, "Absent");
		   			    ListView_SetItemText(hListView,compteurItem,3+jour*2, "Absent");
					}
				}

				/*for(int jour=0;jour<4;jour++)
				{
	 			    for(int j=0;j<enfantsAttributed.size();j++)
	 			    {
	 			        if(enfantsAttributed[j].ref==i)
	 			        {
			 			    /*string buffer="";
			 			    buffer=string(enfantsAttributed[jour][j].np);
			 			    buffer.push_back('\0');*/
						    /*ListView_SetItemText(hListView,compteurItem,0, enfantsAttributed[jour][j].np);//const_cast<char*>(buffer.c_str()));

							ListView_SetItemText(hListView,compteurItem,1, enfantsAttributed[jour][j].classe);
							ListView_SetItemText(hListView,compteurItem,2+jour*2, (enfantsAttributed[jour][j].affectation!=-1)?activite[enfantsAttributed[jour][j].affectation].nom:(char*)"Pas d'attribution");
				   			if(enfantsAttributed[jour][j].affectation==-1)
	   					    {
							    ListView_SetItemText(hListView,compteurItem,3+jour*2, "-");
							}
				   			else if(enfantsAttributed[jour][j].affectation==enfantsAttributed[jour][j].voeu1)
							{
				 	 		    ListView_SetItemText(hListView,compteurItem,3+jour*2, "1");
							}
							else if(enfantsAttributed[jour][j].affectation==enfantsAttributed[jour][j].voeu2)
							{
				 	 		    ListView_SetItemText(hListView,compteurItem,3+jour*2, "2");
							}
							else if(enfantsAttributed[jour][j].affectation==enfantsAttributed[jour][j].voeu3)
							{
				 	 		    ListView_SetItemText(hListView,compteurItem,3+jour*2, "3");
							}
							else
							{
					 		    ListView_SetItemText(hListView,compteurItem,3+jour*2, "-");
							}
							break;
	   					}
					}//fin for enfantsAttributed
					char textLigne[100];
					ListView_GetItemText(hListView,compteurItem,2+jour*2,textLigne,100);
					if(strcmp(textLigne,"")==0)
					{
		   			    ListView_SetItemText(hListView,compteurItem,2+jour*2, "Absent");
		   			    ListView_SetItemText(hListView,compteurItem,3+jour*2, "Absent");
					}
				}//fin for jour
				//fLog<<"Fin boucle : "<<i<<endl;*/
			}
			/*char *texteNom=new char[66];
			for(int i=0;i<nbElevesTotal;i++)
			{
				ListView_GetItemText(hListView,i,0,texteNom,66);
				if(strcmp(texteNom," ")==0 || strcmp(texteNom,"\t")==0 || strcmp(texteNom,"")==0)//deux caractères différents: premier=espace deuxième=
				{
		  		    ListView_DeleteItem(hListView,i);
	   			}
			}
			delete[]texteNom;*/
			//fLog<<"Fin WM_INITDIALOG";
 	  	    return TRUE;
	  	}
	  	case WM_NOTIFY:
		{
	 	    NMLISTVIEW *nmlv = (NMLISTVIEW*) lParam;

	 	    if(nmlv->hdr.code==LVN_COLUMNCLICK)
	 	    {
       		    ListView_SortItems(hListView,CompareFunc,(LPARAM)nmlv->iSubItem);
  			}
	 	    return TRUE;
		}
	  	case WM_COMMAND:
		{
	 	    if(LOWORD(wParam)==IDM_SAVEAS)
			{
			    char pathFile[MAX_PATH]={0};
	 	 	    OPENFILENAME ofn;
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hDlg;
				ofn.lpstrFile = pathFile;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFilter =
				"Fichiers texte (*.txt)\0*.txt\0";
				ofn.nFilterIndex = 1;
				ofn.Flags =
				OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
				int compt=0;
				if (GetSaveFileName(&ofn)==TRUE)
				{
				    if(!ofn.nFileExtension)
				    {
	   				    strcat(pathFile,".txt");
					}
				    ofstream FichierS(pathFile);

				    if(!FichierS)
				 	{
	 			   	    VoirErreur(GetLastError(), (char*)"Erreur lors de l'écriture dans le fichier");
				  	    return false;
					}

				    FichierS<<"Nom Prénom\tClasse\tAffectation Lundi\tVoeu attribué Lundi\tAffectation Mardi\tVoeu attribué Mardi\tAffectation Jeudi\tVoeu attribué Jeudi\tAffectation Vendredi\tVoeu attribué Vendredi\n";
				    int compteurItem=-1;
					for(int i=0;i<enfantsAttributed.size();i++)
				    {
		 			    if(enfantsAttributed[i].denominateur==0)
		 			    {
				 		    continue;
				 		}
				 		compteurItem++;
		 			    for(int j=0;j<10;j++)
		 			    {
		 			        char textLigne[100]={0};///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						    ListView_GetItemText(hListView,compteurItem,j,textLigne,100);//////////////Trouver comment ne pas limiter
						    FichierS<<textLigne<<"\t";/////////////////////////////////////////////////la longueur du texte à recevoir
						    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						}
						FichierS<<"\n";
					}
				    FichierS.close();
				}
	  		    return TRUE;
		 	}
		 	if(LOWORD(wParam)==IDM_SAVEASFORMAT)
		 	{
			    char pathFile[MAX_PATH]={0};
	 	 	    OPENFILENAME ofn;
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = hDlg;
				ofn.lpstrFile = pathFile;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFilter =
				"Fichiers texte (*.txt)\0*.txt\0";
				ofn.nFilterIndex = 1;
				ofn.Flags =
				OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
				int compt=0;
				if (GetSaveFileName(&ofn)==TRUE)
				{
				    if(!ofn.nFileExtension)
				    {
	   				    strcat(pathFile,".txt");
					}
					ofstream FichierS(pathFile);

					if(!FichierS)
				 	{
	 			   	    VoirErreur(GetLastError(),(char*)"Erreur lors de l'écriture dans le fichier");
				  	    return false;
					}

					FichierS<<"Nom, Prénom\tClasse\tAffectation lundi\tAffectation mardi\tAffectation jeudi\tAffectation vendredi\t\t\t\t";
					FichierS<<"Classe\tNom, Prénom\tAffectation lundi\tAffectation mardi\tAffectation jeudi\tAffectation vendredi\n";

					vector<SortieToutEleve> sortieElevesParNom;
					vector<SortieToutEleve> sortieElevesParClasse;

					//Les noms triés par activité sont enregistrés dans un fchier à part
					vector<SortieToutEleve> sortieElevesParActivite;

					for(int i=0;i<enfantsAttributed.size();i++)
					{
		 			    if(enfantsAttributed[i].denominateur==0)
		 			    {
				 		    continue;
				 		}

		 			    SortieToutEleve tabSortieEleveTout;
		 			    SortieToutEleve tabSortieEleveParClasse;

		 			    for(int jour=0;jour<4;jour++)
						{
			 			    SortieToutEleve tabSortieEleveParActivite;

			 			    tabSortieEleveTout.tableau[0]=enfantsAttributed[i].nom;
							tabSortieEleveTout.tableau[1]=enfantsAttributed[i].classe;

							tabSortieEleveParClasse.tableau[1]=enfantsAttributed[i].nom;
							tabSortieEleveParClasse.tableau[0]=enfantsAttributed[i].classe;

							tabSortieEleveParActivite.tableau[0]=(enfantsAttributed[i].affectation[jour]!=-1) ? activite[enfantsAttributed[i].affectation[jour]].nom : "-";
							tabSortieEleveParActivite.tableau[3]=enfantsAttributed[i].nom;
							tabSortieEleveParActivite.tableau[2]=enfantsAttributed[i].classe;

							string nomsJours[4]={"Lundi","Mardi","Jeudi","Vendredi"};
							tabSortieEleveParActivite.tableau[1]=nomsJours[jour];

							tabSortieEleveTout.tableau[jour+2]=(enfantsAttributed[i].affectation[jour]!=-1) ? activite[enfantsAttributed[i].affectation[jour]].nom : "-";

							tabSortieEleveParClasse.tableau[jour+2]=(enfantsAttributed[i].affectation[jour]!=-1) ? activite[enfantsAttributed[i].affectation[jour]].nom : "-";

							if(tabSortieEleveParActivite.tableau[1]!="")
							{
							    sortieElevesParActivite.push_back(tabSortieEleveParActivite);
							}
						}

		 			    /*for(int jour=0;jour<4;jour++)
						{
			 		 	    SortieToutEleve tabSortieEleveParActivite;
			 			    for(int j=0;j<enfantsAttributed.size();j++)
			 			    {
			 			        if(enfantsAttributed[j].ref==i)
			 			        {
									tabSortieEleveTout.tableau[0]=string(enfantsAttributed[jour][j].np);
									tabSortieEleveTout.tableau[1]=string(enfantsAttributed[jour][j].classe);

									tabSortieEleveParClasse.tableau[1]=string(enfantsAttributed[jour][j].np);
									tabSortieEleveParClasse.tableau[0]=string(enfantsAttributed[jour][j].classe);

									tabSortieEleveParActivite.tableau[0]=string((enfantsAttributed[jour][j].affectation!=-1) ? activite[enfantsAttributed[jour][j].affectation].nom : "-");
									tabSortieEleveParActivite.tableau[3]=string(enfantsAttributed[jour][j].np);
									tabSortieEleveParActivite.tableau[2]=string(enfantsAttributed[jour][j].classe);

									string nomsJours[4]={"Lundi","Mardi","Jeudi","Vendredi"};
									tabSortieEleveParActivite.tableau[1]=string(nomsJours[jour]);

									tabSortieEleveTout.tableau[jour+2]=string((enfantsAttributed[jour][j].affectation!=-1) ? activite[enfantsAttributed[jour][j].affectation].nom : "-");

									tabSortieEleveParClasse.tableau[jour+2]=string((enfantsAttributed[jour][j].affectation!=-1) ? activite[enfantsAttributed[jour][j].affectation].nom : "-");

									break;
								}
							}//fin for(int j=0;j<enfantsAttributed[jour].size();j++)
							if(tabSortieEleveParActivite.tableau[1]!="")
							{
							    sortieElevesParActivite.push_back(tabSortieEleveParActivite);
							}
						}//fin for(int jour=0;jour<4;jour++)*/
						sortieElevesParNom.push_back(tabSortieEleveTout);
						sortieElevesParClasse.push_back(tabSortieEleveParClasse);
					}

					//Arrangement de sortieElevesParNom
					Trier(sortieElevesParNom,0);//par nom

					//Arrangemet de sortieElevesParClasse
					Trier(sortieElevesParClasse,1);//par nom
					Trier(sortieElevesParClasse,0);//par classe

					//Arrangement de sortieElevesParActivite
					Trier(sortieElevesParActivite,3);//par nom
					Trier(sortieElevesParActivite,2);//par classe
					Trier(sortieElevesParActivite,1);//par jour
					Trier(sortieElevesParActivite,0);//par activité

					for(int i=0;i<sortieElevesParNom.size();i++)
					{
		 			    for (int j=0;j<6;j++)
		 			    {
					        FichierS<<((strcmp(sortieElevesParNom[i].tableau[j].c_str(),"")!=0) ? sortieElevesParNom[i].tableau[j] : "-")<<"\t";
						}
						FichierS<<"\t\t\t";
						for (int j=0;j<6;j++)
		 			    {
					        FichierS<<((strcmp(sortieElevesParClasse[i].tableau[j].c_str(),"")!=0) ? sortieElevesParClasse[i].tableau[j] : "-")<<"\t";
						}
						FichierS<<"\n";
					}
					FichierS.close();

					string cheminFichier=string(pathFile);
					cheminFichier.replace(cheminFichier.size()-4,0," par activité");

					FichierS.open(cheminFichier.c_str());
					if(!FichierS)
				 	{
	 			   	    VoirErreur(GetLastError(), (char*)"Erreur lors de l'écriture dans le fichier");
				  	    return false;
					}
					FichierS<<"Activité\tJour\tClasse\tNom, Prénom\n";

					for(int i=0;i<sortieElevesParActivite.size();i++)
					{
		 			    for(int j=0;j<4;j++)
		 			    {
		 			        FichierS<<sortieElevesParActivite[i].tableau[j].c_str()<<"\t";
						}
						FichierS<<"\n";
					}

					FichierS.close();
				}
			    return TRUE;
			}
		 	if(LOWORD(wParam)==IDM_BADAFFECT)
		 	{
    		    DialogBox(hinstance, "DIALOGBADAFFECT" , hDlg, (DLGPROC)DialogBadaffectProc);
			    return TRUE;
			}
	 		if (LOWORD(wParam)==IDCANCEL)
	 		{
	 		    EndDialog(hDlg,IDCANCEL);
        		return TRUE;
			}
	 	}
	 	default :
		{
  		    return FALSE;
	    }
    }
}
int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
 	//MessageBox(0,0,0,0);
    LVFINDINFO findInfo;
    findInfo.flags=LVFI_PARAM;
    findInfo.lParam=lParam1;

    char a[4];

    /*LVITEM item1={0};
    char pszText1[100];
    item1.mask=LVIF_TEXT | LVIF_STATE;
    item1.iItem=ListView_FindItem(hListView,-1,&findInfo);
    item1.iSubItem=(int)lParamSort;
    item1.pszText=pszText1;
    item1.cchTextMax=100;
    ListView_GetItem(hListView,&item1);

    findInfo.lParam=lParam2;
    LVITEM item2={0};
    char pszText2[100];
    item2.mask=LVIF_TEXT | LVIF_STATE;
    item2.iItem=ListView_FindItem(hListView,-1,&findInfo);
    item2.iSubItem=(int)lParamSort;
    item2.pszText=pszText2;
    item2.cchTextMax=100;
    ListView_GetItem(hListView,&item2);
    //itoa(item2.iItem,a,0);
    //MessageBox(0,pszText2,pszText1,0);*/
    char pszText1[100];
    char pszText2[100];
    ListView_GetItemText(hListView,ListView_FindItem(hListView,-1,&findInfo),(int)lParamSort,pszText1,100);
    findInfo.lParam=lParam2;
    ListView_GetItemText(hListView,ListView_FindItem(hListView,-1,&findInfo),(int)lParamSort,pszText2,100);

	if((int)lParamSort%2!=0 && (int)lParamSort!=1)
	{
 	    int nb1=atoi(pszText1);
 	    int nb2=atoi(pszText2);
 	    return nb1-nb2;
	}
	else
	{
	    //MessageBox(0,item1.pszText,item2.pszText,0);
        return stricmp(pszText1,pszText2);
	}

    return 0;
}
int CALLBACK ComparePointFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static HWND hListViewBA;
static HWND hListViewGA;
static HWND hDLG;
BOOL APIENTRY DialogBadaffectProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

    switch (uMsg)
    {
        case WM_INITDIALOG:
      	{
		 	 hDLG=hDlg;
	 	    //création listview mauvaise attribution
		    hListViewBA=CreateListView (hDlg,0,0,50);
		    //création colones
		    LVCOLUMN lvc;
    		int iCol=0;
    		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT ;
    		lvc.iSubItem = 0;
        	lvc.pszText = (LPSTR)"Nom Prénom";
        	lvc.cx = 100;
        	lvc.fmt = LVCFMT_LEFT;
        	ListView_InsertColumn(hListViewBA, iCol++, &lvc);

        	lvc.iSubItem = 1;
        	lvc.pszText = (LPSTR)"Classe";
        	ListView_InsertColumn(hListViewBA, iCol++, &lvc);

        	lvc.iSubItem = 2;
        	lvc.pszText = (LPSTR)"Ratio points";
        	ListView_InsertColumn(hListViewBA, iCol++, &lvc);

        	//création listview bonnes attributions
        	hListViewGA=CreateListView (hDlg,50,0,50);
        	//création colones
        	iCol=0;
    		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT ;
    		lvc.iSubItem = 0;
        	lvc.pszText = (LPSTR)"Nom Prénom";
        	lvc.cx = 100;
        	lvc.fmt = LVCFMT_LEFT;
        	ListView_InsertColumn(hListViewGA, iCol++, &lvc);

        	lvc.iSubItem = 1;
        	lvc.pszText = (LPSTR)"Classe";
        	ListView_InsertColumn(hListViewGA, iCol++, &lvc);

        	lvc.iSubItem = 2;
        	lvc.pszText = (LPSTR)"Ratio points";
        	ListView_InsertColumn(hListViewGA, iCol++, &lvc);

        	//Création des lignes
        	LVITEM lvI;
	  		lvI.mask      = LVIF_TEXT |LVIF_STATE;
	  		lvI.stateMask = 0;
	  		lvI.iSubItem  = 0;
	  		lvI.state     = 0;
			lvI.iItem  = 0;

			int compteurItem=-1;
        	for(int i=0;i<enfantsAttributed.size();i++)
			{

 			    if(enfantsAttributed[i].denominateur==0)
 			    {
		 		    continue;
		 		}
		 		compteurItem++;

 		        lvI.mask   = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
 		        lvI.iItem  = compteurItem;
 		        lvI.lParam = compteurItem;
 		        lvI.iSubItem  = 0;
 		        lvI.pszText= (LPSTR)"";
				ListView_InsertItem(hListViewBA, &lvI);
				ListView_InsertItem(hListViewGA, &lvI);

				//char nom[66]={0};
				//char classe[34]={0};

				for(int jour=0;jour<4;jour++)
				{
	 			    //strcpy(nom,enfantsAttributed[i].nom.c_str());
					//strcpy(classe,enfantsAttributed[i].classe.c_str());
					ListView_SetItemText(hListViewBA,compteurItem,0, const_cast<char*>(enfantsAttributed[i].nom.c_str()));
					ListView_SetItemText(hListViewGA,compteurItem,0, const_cast<char*>(enfantsAttributed[i].nom.c_str()));
					ListView_SetItemText(hListViewBA,compteurItem,1, const_cast<char*>(enfantsAttributed[i].classe.c_str()));
					ListView_SetItemText(hListViewGA,compteurItem,1, const_cast<char*>(enfantsAttributed[i].classe.c_str()));
					string points;
					string chiffre=to_string(enfantsAttributed[i].point);
					points=chiffre;
					points+="/";
					chiffre=to_string(enfantsAttributed[i].denominateur);
					points+=chiffre;
					ListView_SetItemText(hListViewBA,compteurItem,2, const_cast<char*>(points.c_str()));
					ListView_SetItemText(hListViewGA,compteurItem,2, const_cast<char*>(points.c_str()));

					if(enfantsAttributed[i].nom[0]!='\0')break;
				}

				/*for(int jour=0;jour<4;jour++)
				{
	 			    for(int j=0;j<enfantsAttributed[jour].size();j++)
	 			    {
	 				    if(enfantsAttributed[jour][j].ref==i)
	 				    {
							strcpy(nom,enfantsAttributed[jour][j].np);
							strcpy(classe,enfantsAttributed[jour][j].classe);
							ListView_SetItemText(hListViewBA,compteurItem,0, nom);
							ListView_SetItemText(hListViewGA,compteurItem,0, nom);
							ListView_SetItemText(hListViewBA,compteurItem,1, classe);
							ListView_SetItemText(hListViewGA,compteurItem,1, classe);
							char points[10];
							char chiffre[4];
							itoa(referenceEleve[i].point,chiffre,10);
							strcpy(points,chiffre);
							strcat(points,"/");
							itoa(referenceEleve[i].denominateur,chiffre,10);
							strcat(points,chiffre);
							ListView_SetItemText(hListViewBA,compteurItem,2, points);
							ListView_SetItemText(hListViewGA,compteurItem,2, points);
							break;
						}
					}//fin for enfantsAttributed
					if(nom[0]!='\0')break;
				}*/

			}
			char *texteNom=new char[66];
			compteurItem=-1;
        	for(int i=0;i<enfantsAttributed.size();i++)
			{
 			    if(enfantsAttributed[i].denominateur==0)
 			    {
		 		    continue;
		 		}
		 		compteurItem++;

		 	    ListView_GetItemText(hListViewBA,compteurItem,0,texteNom,66);
				if(strcmp(texteNom,"")==0)
				{
		  		    ListView_DeleteItem(hListViewBA,compteurItem);
	   			}
			}
			compteurItem=-1;
        	for(int i=0;i<enfantsAttributed.size();i++)
			{
 			    if(enfantsAttributed[i].denominateur==0)
 			    {
		 		    continue;
		 		}
		 		compteurItem++;
		 	    ListView_GetItemText(hListViewBA,compteurItem,0,texteNom,66);
				if(strcmp(texteNom,"")==0)
				{
		  		    ListView_DeleteItem(hListViewGA,compteurItem);
	   			}
			}
			delete[]texteNom;
			ListView_SortItems(hListViewBA,ComparePointFunc,0);
			ListView_SortItems(hListViewGA,ComparePointFunc,1);
		    return TRUE;
		}
		case WM_PAINT:
	 	{
 	 	    PAINTSTRUCT ps;
        	HDC hdc ;
			HPEN hpen;
			HBRUSH hpPlein;
			hdc = BeginPaint(hDlg, &ps);
		    //hpen = CreatePen(PS_SOLID, 1, 0x00FF0000);
			//SelectObject(hdc,hpen);
			SetBkMode(hdc, TRANSPARENT);

			RECT rectHwnd;
	 	    GetClientRect(hDlg,&rectHwnd);

			MoveToEx(hdc, (rectHwnd.right-rectHwnd.left)/2, 0, NULL);
   			LineTo(hdc, (rectHwnd.right-rectHwnd.left)/2, (rectHwnd.bottom-rectHwnd.top));
   			/*LineTo(hdc, rectHwnd.right*5/8+50, rectHwnd.bottom-53);
   			LineTo(hdc, 30, rectHwnd.bottom-53);
   			LineTo(hdc, 30, 30);*/
			EndPaint(hDlg, &ps);
 	 	    return true;
	 	}
		case WM_COMMAND:
		{
	 	    if (LOWORD(wParam)==IDCANCEL)
	 		{
	 		    EndDialog(hDlg,IDCANCEL);
        		return TRUE;
			}
		}
		default :
         return FALSE;
	}
}

int CALLBACK ComparePointFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
 	/*char a[6];
	char b[6];
	//itoa(,a,10);
	//itoa(,b,10);
	//sprintf(a,"%.2f",3*1.0/6);
	sprintf(a,"%.2f",(referenceEleve[lParam1].point*1.0/referenceEleve[lParam1].denominateur) - (referenceEleve[lParam2].point*1.0/referenceEleve[lParam2].denominateur));
	//sprintf(b,"%.2f",(referenceEleve[lParam2].point-1.0/referenceEleve[lParam2].denominateur) - (referenceEleve[lParam1].point*1.0/referenceEleve[lParam1].denominateur));
	MessageBox(hDLG,0,a,0);
	itoa(referenceEleve[lParam1].point,a,10);
	itoa(referenceEleve[lParam1].denominateur,b,10);
	MessageBox(hDLG,b,a,0);*/
 	if((int)lParamSort==0)
 	{
  	    /*char a[6];
  	    char b[6];
  	    itoa((referenceEleve[lParam2].point*1.0/referenceEleve[lParam2].denominateur),a,10);
  	    itoa((referenceEleve[lParam1].point*1.0/referenceEleve[lParam1].denominateur),b,10);
  	    MessageBox(hDLG,b,a,0);*/
  	    //return int((referenceEleve[lParam2].point*1.0/referenceEleve[lParam2].denominateur) - (referenceEleve[lParam1].point*1.0/referenceEleve[lParam1].denominateur));
  	    if(enfantsAttributed[lParam1].denominateur==0 || enfantsAttributed[lParam2].denominateur==0)
  	    {
  		    return 0;
  		}
	    if((enfantsAttributed[lParam2].point*1.0/enfantsAttributed[lParam2].denominateur) < (enfantsAttributed[lParam1].point*1.0/enfantsAttributed[lParam1].denominateur))
	    {
		    return -1;
		}
		else if((enfantsAttributed[lParam2].point*1.0/enfantsAttributed[lParam2].denominateur) == (enfantsAttributed[lParam1].point*1.0/enfantsAttributed[lParam1].denominateur))
	    {
		    return 0;
		}
		else if((enfantsAttributed[lParam2].point*1.0/enfantsAttributed[lParam2].denominateur) > (enfantsAttributed[lParam1].point*1.0/enfantsAttributed[lParam1].denominateur))
	    {
		    return 1;
		}
	}
	else if((int)lParamSort==1)
	{
	    if(enfantsAttributed[lParam1].denominateur==0 || enfantsAttributed[lParam2].denominateur==0)
  	    {
  		    return 0;
  		}
 	    //return int((referenceEleve[lParam1].point*1.0/referenceEleve[lParam1].denominateur) - (referenceEleve[lParam2].point*1.0/referenceEleve[lParam2].denominateur));
 	    if((enfantsAttributed[lParam2].point*1.0/enfantsAttributed[lParam2].denominateur) < (enfantsAttributed[lParam1].point*1.0/enfantsAttributed[lParam1].denominateur))
	    {
		    return 1;
		}
		else if((enfantsAttributed[lParam2].point*1.0/enfantsAttributed[lParam2].denominateur) == (enfantsAttributed[lParam1].point*1.0/enfantsAttributed[lParam1].denominateur))
	    {
		    return 0;
		}
		else if((enfantsAttributed[lParam2].point*1.0/enfantsAttributed[lParam2].denominateur) > (enfantsAttributed[lParam1].point*1.0/enfantsAttributed[lParam1].denominateur))
	    {
		    return -1;
		}
	}
 	return 0;
}
//dialog des vérification de voeu
BOOL APIENTRY DialogVerifRempliProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

    switch (uMsg)
    {
        case WM_INITDIALOG:
      	{
		    hListView=CreateListView (hDlg, false);

		    LVCOLUMN lvc;
    		int iCol=0;
    		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT ;
    		lvc.iSubItem = 0;
        	lvc.pszText = (LPSTR)"Nom Prénom";
        	lvc.cx = 100;
        	lvc.fmt = LVCFMT_LEFT;
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 1;
        	lvc.pszText = (LPSTR)"Classe";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 2;
        	lvc.pszText = (LPSTR)"Jour";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 3;
        	lvc.pszText = (LPSTR)"Voeu 1";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 4;
        	lvc.pszText = (LPSTR)"Voeu 2";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 5;
        	lvc.pszText = (LPSTR)"Voeu 3";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 6;
        	lvc.pszText = (LPSTR)"Ligne:";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	//Création des lignes
        	LVITEM lvI={0};
	  		lvI.mask      = LVIF_TEXT |LVIF_STATE;
	  		lvI.stateMask = 0;
	  		lvI.iSubItem  = 0;
	  		lvI.state     = 0;
			lvI.iItem  = 0;
			lvI.lParam=0;
			int nbItem=0;
			bool voeuNonRempli=false;

			for(int jour=0;jour<4;jour++)
			{
 			    //test pour voir si un jour est vide
 			    int nbEnfantJour=0;
 			    for(int i=0;i<enfants.size();i++)
	    		{
		        	if(enfants[i].voeux[jour].voeu1!=-1 || enfants[i].voeux[jour].voeu2!=-1 || enfants[i].voeux[jour].voeu3!=-1)
		        	{
	  			        nbEnfantJour++;
	  				}
	    		}
	    		if(nbEnfantJour==0)continue;
 			    for(int i=0;i<enfants.size();i++)
 			    {
 				    lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
 				    if(enfants[i].voeux[jour].voeu1==-1)
 				    {
	  				    voeuNonRempli=true;
		 			}
		 			if(enfants[i].voeux[jour].voeu2==-1)
 				    {
	  				    voeuNonRempli=true;
		 			}
		 			if(enfants[i].voeux[jour].voeu3==-1)
 				    {
	  				    voeuNonRempli=true;
		 			}

					if(voeuNonRempli==true)
					{
			 			ListView_InsertItem(hListView, &lvI);

						ListView_SetItemText(hListView,lvI.iItem,0,const_cast<char*>(enfants[i].nom.c_str()));
						ListView_SetItemText(hListView,lvI.iItem,1, const_cast<char*>(enfants[i].classe.c_str()));

						string nmJour[4]={"lundi","mardi","jeudi","vendredi"};
						ListView_SetItemText(hListView,lvI.iItem,2, const_cast<char*>(nmJour[jour].c_str()));


						ListView_SetItemText(hListView,lvI.iItem,3, (enfants[i].voeux[jour].voeu1!=-1) ? const_cast<char*>(activite[enfants[i].voeux[jour].voeu1].nom.c_str()) : (char*)"Pas de choix");
						ListView_SetItemText(hListView,lvI.iItem,4, (enfants[i].voeux[jour].voeu2!=-1) ? const_cast<char*>(activite[enfants[i].voeux[jour].voeu2].nom.c_str()) : (char*)"Pas de choix");
						ListView_SetItemText(hListView,lvI.iItem,5, (enfants[i].voeux[jour].voeu3!=-1) ? const_cast<char*>(activite[enfants[i].voeux[jour].voeu3].nom.c_str()) : (char*)"Pas de choix");

						string ligne=to_string(i+1);
						ListView_SetItemText(hListView,lvI.iItem,6,const_cast<char*>(ligne.c_str()));

	  				    lvI.iItem++;
		        		lvI.lParam++;
		        		lvI.iSubItem=0;

		        		voeuNonRempli=false;
					}
				}
			}
		    return TRUE;
		}
		case WM_NOTIFY:
	 	{
   		    NMLISTVIEW *nmlv = (NMLISTVIEW*) lParam;
 	 	    if(nmlv->hdr.code==NM_DBLCLK)
 	 	    {
		 	    /*char a[4];
		 	    char b[4];
		 	    itoa(nmlv->iItem,a,10);
		 	    itoa(nmlv->iSubItem,b,10);
		 	    MessageBox(0,b,a,0);*/
		 	    if(nmlv->iItem!=-1)
		 	    {
	  			    char sligne[5];
	  			    ListView_GetItemText(hListView,nmlv->iItem,6,sligne,5);

	   			    int ligne=atoi(sligne)-1;

	   			    SendMessage(hListboxNom,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxNom,LB_GETCOUNT,0,0)));
    				SendMessage(hListboxClasse,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxClasse,LB_GETCOUNT,0,0)));

    				SendMessage(hListboxNom,LB_SETSEL,true,ligne);
    				SendMessage(hListboxClasse,LB_SETSEL,true,ligne);

    		 		SendMessage(hWndMain,WM_COMMAND,MAKEWPARAM(ID_LISTBOX,LBN_SELCHANGE),0);
	  			}
	  			else
	  			{
	     		    LVHITTESTINFO infoItem={0};
	     			infoItem.pt=nmlv->ptAction;
	     			infoItem.pt.x=5;

			 	    char sligne[5];
	  			    ListView_GetItemText(hListView,SendMessage(hListView,LVM_HITTEST,0,(LPARAM)&infoItem),6,sligne,5);

	   			    int ligne=atoi(sligne)-1;

					SendMessage(hListboxNom,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxNom,LB_GETCOUNT,0,0)));
    				SendMessage(hListboxClasse,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxClasse,LB_GETCOUNT,0,0)));

    				SendMessage(hListboxNom,LB_SETSEL,true,ligne);
    				SendMessage(hListboxClasse,LB_SETSEL,true,ligne);

    		 		SendMessage(hWndMain,WM_COMMAND,MAKEWPARAM(ID_LISTBOX,LBN_SELCHANGE),0);
				}
		 	}
		 	//if(nmlv->hdr.code==NM_RCLICK)
		 	//{

			//}

 	 	    return TRUE;
	 	}
		case WM_COMMAND:
		{
	 	    if (LOWORD(wParam)==IDCANCEL)
	 		{
	 		    EndDialog(hDlg,IDCANCEL);
        		return TRUE;
			}
		}
		default :
         return FALSE;
	}
}
BOOL APIENTRY DialogFindNameProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

    switch (uMsg)
    {
        case WM_INITDIALOG:
      	{
		    return TRUE;
		}
		case WM_COMMAND:
		{
	 	    if(LOWORD(wParam)==IDOK)
	 	    {
                /* DONE (Simon#1#): Modifier DialogFindNameProc pour que iItemFind gere la liste */

			    HWND hEditbox=GetDlgItem(hDlg,IDE_EDITFINDNAME);
			    int lEditbox=GetWindowTextLength(hEditbox)+1;
		 	    if(lEditbox==1)return true;
   			    char *sEditbox=new char[lEditbox];
   			    GetWindowText(hEditbox,sEditbox,lEditbox);
   			    SetWindowText(hEditFindName,sEditbox);
   			    //strcat(sEditbox,'\0');

				int iItemFind=SendMessage(hListboxNom,LB_FINDSTRING,(WPARAM)-1,(LPARAM)sEditbox);
				if(iItemFind==LB_ERR)
				{
			 	    for(int i=0;i<enfants.size();i++)
			 	    {
	 				    if(enfants[i].nom.find(sEditbox)!=string::npos)
	 				    {
						    for(int j=0;j<enfants.size();j++)
						    {
                                if(SendMessage(hListboxNom,LB_GETITEMDATA,j,0)==i)
                                {
                                    iItemFind=j;
                                    break;
                                }
                            }
						    break;
						}
					}
					if(iItemFind==LB_ERR)
					{
				 	    for(int i=0;i<enfants.size();i++)
				 	    {
		 				    string chaine1;
		 				    chaine1=enfants[i].nom;
		 				    transform(chaine1.begin(), chaine1.end(), chaine1.begin(), ::tolower);
		 				    /*for(int j=0;j<strlen(chaine1);j++)
		 				    {
		 					    tolower(chaine1[i]);
							}*/
							for(int j=0;j<strlen(sEditbox);j++)
		 				    {
		 					    tolower(sEditbox[i]);
							}
		 				    if(chaine1.find(sEditbox)!=string::npos)
		 				    {
							    for(int j=0;j<enfants.size();j++)
    						    {
                                    if(SendMessage(hListboxNom,LB_GETITEMDATA,j,0)==i)
                                    {
                                        iItemFind=j;
                                        break;
                                    }
                                }
							    break;
							}
						}
					}
					if(iItemFind==LB_ERR)
					{
				 	    iItemFind=SendMessage(hListboxClasse,LB_FINDSTRING,(WPARAM)-1,(LPARAM)sEditbox);
						if(iItemFind==LB_ERR)
						{
					 	    for(int i=0;i<enfants.size();i++)
					 	    {
			 				    if(enfants[i].classe.find(sEditbox)!=string::npos)
			 				    {
								    for(int j=0;j<enfants.size();j++)
        						    {
                                        if(SendMessage(hListboxNom,LB_GETITEMDATA,j,0)==i)
                                        {
                                            iItemFind=j;
                                            break;
                                        }
                                    }
								    break;
								}
							}
							if(iItemFind==LB_ERR)
							{
						 	    for(int i=0;i<enfants.size();i++)
						 	    {
				 				    string chaine1=enfants[i].classe;
				 				    transform(chaine1.begin(), chaine1.end(), chaine1.begin(), ::tolower);
				 				    /*for(int j=0;j<strlen(chaine1);j++)
				 				    {
				 					    tolower(chaine1[i]);
									}*/
									for(int j=0;j<strlen(sEditbox);j++)
				 				    {
				 					    tolower(sEditbox[i]);
									}
				 				    if(chaine1.find(sEditbox)!=string::npos)
				 				    {
									    for(int j=0;j<enfants.size();j++)
            						    {
                                            if(SendMessage(hListboxNom,LB_GETITEMDATA,j,0)==i)
                                            {
                                                iItemFind=j;
                                                break;
                                            }
                                        }
									    break;
									}
								}
							}
							if(iItemFind==LB_ERR)return 0;
						}
				 	}
				}
   			    SendMessage(hListboxNom,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxNom,LB_GETCOUNT,0,0)));
				SendMessage(hListboxClasse,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxClasse,LB_GETCOUNT,0,0)));

				SendMessage(hListboxNom,LB_SETSEL,true,iItemFind);
				SendMessage(hListboxClasse,LB_SETSEL,true,iItemFind);

		 		SendMessage(hWndMain,WM_COMMAND,MAKEWPARAM(ID_LISTBOX,LBN_SELCHANGE),0);

		 	    delete[]sEditbox;

			    EndDialog(hDlg,IDOK);
			    return TRUE;
			}
	 	    if (LOWORD(wParam)==IDCANCEL)
	 		{
	 		    EndDialog(hDlg,IDCANCEL);
        		return TRUE;
			}
		}
		default :
         return FALSE;
	}
}

BOOL APIENTRY DialogGoLineProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

    switch (uMsg)
    {
        case WM_INITDIALOG:
      	{
		    return TRUE;
		}
		case WM_COMMAND:
		{
	 	    if(LOWORD(wParam)==IDOK)
	 	    {
			    HWND hEditbox=GetDlgItem(hDlg,IDE_EDITFINDNAME);
			    int lEditbox=GetWindowTextLength(hEditbox)+1;
   			    char *sEditbox=new char[lEditbox];
   			    GetWindowText(hEditbox,sEditbox,lEditbox);
   			    int ligne=(atoi(sEditbox)-1==-1)?0:atoi(sEditbox)-1;
				/* DONE (Simon#1#): SETCURSEL */

   			    SendMessage(hListboxNom,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxNom,LB_GETCOUNT,0,0)));
				SendMessage(hListboxClasse,LB_SELITEMRANGE, false, MAKELPARAM(0,SendMessage(hListboxClasse,LB_GETCOUNT,0,0)));

				SendMessage(hListboxNom,LB_SETSEL,true,ligne);
				SendMessage(hListboxClasse,LB_SETSEL,true,ligne);

		 		SendMessage(hWndMain,WM_COMMAND,MAKEWPARAM(ID_LISTBOX,LBN_SELCHANGE),0);

   			    delete[]sEditbox;

			    EndDialog(hDlg,IDOK);
			    return TRUE;
			}
	 	    if (LOWORD(wParam)==IDCANCEL)
	 		{
	 		    EndDialog(hDlg,IDCANCEL);
        		return TRUE;
			}
		}
		default :
         return FALSE;
	}
}
BOOL APIENTRY DialogVerifPlacesProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

    switch (uMsg)
    {
        case WM_INITDIALOG:
      	{
		    CheckDlgButton(hDlg, IDC_RADIO1, BST_CHECKED);
		    return TRUE;
		}
		case WM_COMMAND:
		{
	 	    /*if (LOWORD(wParam)==IDC_RADIO1)
	 		{
	   		    //CheckDlgButton(hDlg, IDC_RADIO1, BST_CHECKED);

	   		    //CheckDlgButton(hDlg, IDC_RADIO2, BST_UNCHECKED);
	   		    return TRUE;
	   		}
	   		if (LOWORD(wParam)==IDC_RADIO2)
	 		{
	   		    //CheckDlgButton(hDlg, IDC_RADIO2, BST_CHECKED);

	   		    //CheckDlgButton(hDlg, IDC_RADIO1, BST_UNCHECKED);
	   		    return TRUE;
	   		}*/
	   		if(LOWORD(wParam)==IDOK)
	   		{
				if(IsDlgButtonChecked(hDlg, IDC_RADIO1) == BST_CHECKED)
    			{
				    EndDialog(hDlg,IDC_RADIO1);
				}
				if(IsDlgButtonChecked(hDlg, IDC_RADIO2) == BST_CHECKED)
    			{
				    EndDialog(hDlg,IDC_RADIO2);
				}
        		return TRUE;
			}
	 	    if (LOWORD(wParam)==IDCANCEL)
	 		{
	 		    EndDialog(hDlg,IDCANCEL);
        		return TRUE;
			}
		}
		default :
         return FALSE;
	}
}

BOOL APIENTRY DialogCountAppliProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch (uMsg)
    {
        //static vector<HWND> hStaticsDemandeActivite;
        //static vector<HWND> hGroupDemandeActivite;
        case WM_INITDIALOG:
      	{
            /*for(int i=0;i<activite.size();i++)
            {
                HWND hGroupTemp=CreateWindowEx(0,"button","Propriétés des activités",WS_VISIBLE | WS_CHILD | BS_GROUPBOX,60+rectHwnd.right*5/8,rectHwnd.bottom*1/20,rectHwnd.right-(60+rectHwnd.right*5/8)-10,210,hDlg,0,0,0);;
                hGroupDemandeActivite.push_back(hGroupTemp);
                HWND hStaticTemp=CreateWindowEx(0,"static"," : ",WS_VISIBLE | WS_CHILD,70+rectHwnd.right*5/8,2+rectHwnd.bottom*6/9,150,25,hDlg,0,0,0);
                hStaticsDemandeActivite.push_back(hStaticTemp);
            }*/
            return TRUE;
        }
        case WM_COMMAND:
		{
            if(LOWORD(wParam)==IDOK)
	 		{
      		    EndDialog(hDlg,IDCANCEL);
    			return TRUE;
  		    }
            if (LOWORD(wParam)==IDCANCEL)
	 		{
	 		    EndDialog(hDlg,IDCANCEL);
        		return TRUE;
			}
        }
		default :
         return FALSE;
    }
}

BOOL APIENTRY DialogSelectClasseProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

    switch (uMsg)
    {
        case WM_INITDIALOG:
      	{
		    //CheckDlgButton(hDlg, IDC_RADIO1, BST_CHECKED);
		    RemplirListeClasse(enfants, activite, nomClasses);

		    for(int i=0;i<nomClasses.size();i++)
		    {
	 		    hCheckBoxClasses.push_back(CreateWindowEx(0,"button", nomClasses[i].c_str(), WS_CHILD | WS_TABSTOP |WS_VISIBLE | BS_AUTOCHECKBOX ,25+100*(i%4),70+30*(i/4),100,22,hDlg,(HMENU)ID_CHECKBOX,0,NULL));
			}
			int nbClasseSelected=0;
			for(int i=0;i<nomClasses.size();i++)
		    {
      		    if(activite[iActiEnCour].classe.find(nomClasses[i]+"\t")!=std::string::npos)
      		    {
		 		    Button_SetCheck(hCheckBoxClasses[i],BST_CHECKED);
		 		    nbClasseSelected++;
		 		}
 			}
 			if(nbClasseSelected==nomClasses.size() && nomClasses.size()!=0)
 			{
		  	    SendDlgItemMessage(hDlg,IDC_RADIO1,BM_SETCHECK,BST_CHECKED,0);
			}
		    return TRUE;
		}
		case WM_COMMAND:
		{
	 	    if(LOWORD(wParam)==IDC_RADIO1)
	 	    {
  			    if(IsDlgButtonChecked(hDlg, IDC_RADIO1) == BST_CHECKED)
  			    {
				    for(int i=0;i<hCheckBoxClasses.size();i++)
		    		{
					    Button_SetCheck(hCheckBoxClasses[i],BST_CHECKED);
					}
				}
				else
				{
		 		    for(int i=0;i<hCheckBoxClasses.size();i++)
		    		{
					    Button_SetCheck(hCheckBoxClasses[i],BST_UNCHECKED);
					}
				}
  			}
  			if(LOWORD(wParam)==ID_CHECKBOX)
  			{
		   	    for(int i=0;i<hCheckBoxClasses.size();i++)
   				{
		 		    if(Button_GetCheck(hCheckBoxClasses[i])==BST_UNCHECKED)
		 		    {
			    	    CheckDlgButton(hDlg, IDC_RADIO1, BST_UNCHECKED);
				   	}
				}
			}
	   		if(LOWORD(wParam)==IDOK)
	   		{

			    if(activite.size()==0)
			    {
			        hCheckBoxClasses.clear();
			        EndDialog(hDlg,IDCANCEL);
			        return TRUE;
			    }
			    if(IsDlgButtonChecked(hDlg, IDC_RADIO1) == BST_CHECKED)
			    {

				    activite[iActiEnCour].classe="";
				    for(int i=0;i<nomClasses.size();i++)
		    		{
   	    			    activite[iActiEnCour].classe+=nomClasses[i]+"\t";
					}
					hCheckBoxClasses.clear();
				    EndDialog(hDlg,IDOK);
				}
				else
				{
		 		    activite[iActiEnCour].classe="";
				 	for(int i=0;i<hCheckBoxClasses.size();i++)
		    		{
			 		    if(Button_GetCheck(hCheckBoxClasses[i])==BST_CHECKED)
			 		    {
					   	    activite[iActiEnCour].classe+=nomClasses[i]+"\t";
					   	}
					}
					hCheckBoxClasses.clear();
		 		    EndDialog(hDlg,IDOK);
				}
        		return TRUE;
			}
	 	    if (LOWORD(wParam)==IDCANCEL)
	 		{
			    hCheckBoxClasses.clear();
	 		    EndDialog(hDlg,IDCANCEL);
        		return TRUE;
			}
		}
		default :
         return FALSE;
	}
}
BOOL APIENTRY DialogTraitementEnCours(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

    switch (uMsg)
    {
        case WM_INITDIALOG:
      	{
	 	    inferieurA=1;
		    HFONT fond=CreateFont(20,0,0,0,FW_BOLD,FALSE,TRUE,0,0,OUT_DEFAULT_PRECIS,0,0,FF_ROMAN,"Time New Roman");
		    HWND hText=GetDlgItem(hDlg,IDT_MAINTXT);
		    SendMessage(hText,WM_SETFONT,(WPARAM)fond,(LPARAM)TRUE);

			HWND hProgress=GetDlgItem(hDlg,IDE_PB1);
			SendMessage(hProgress,PBM_SETMARQUEE,1,0);
			::tempsEcoule=0;
			hDlgTraitementEnCours=hDlg;
		    SetTimer(hDlg, ID_TIMER, 1000, NULL);
		    return TRUE;
		}
		case WM_COMMAND:
		{
	   		if(LOWORD(wParam)==IDOK)
	   		{
			    KillTimer(hDlg,ID_TIMER);
			    ::arretTraitement=true;
        		return TRUE;
			}
	 	    if (LOWORD(wParam)==IDCANCEL)
	 		{
 		 	    KillTimer(hDlg,ID_TIMER);
 		 	    ::arretTraitement=true;
	 		    EndDialog(hDlg,IDCANCEL);
        		return TRUE;
			}
			return 0;
		}
		case WM_CTLCOLORSTATIC:
		{
  		    DWORD CtrlID = GetDlgCtrlID((HWND)lParam); //Window Control ID
		    if (CtrlID == IDT_MAINTXT) //If desired control
		    {
		        HDC hdcStatic = (HDC) wParam;
		        SetBkColor(hdcStatic, COLORREF( GetSysColor( COLOR_3DFACE ) ) );
		        SetTextColor(hdcStatic, RGB(0,0,255));
		        //SetBkColor(hdcStatic, RGB(230,230,230));
		        //SetBkMode(hdcStatic, TRANSPARENT);
		        return (INT_PTR)GetSysColorBrush( COLOR_3DFACE );
		    }
		    if (CtrlID == IDT_FIABILITE) //If desired control
		    {
 		   	    //inferieurA
 		   	    int R=255,V=0;
 		   	    if(255-51*(inferieurA-1)>0)
 		   	    {
	 			    V=51*(inferieurA-1);
				}
				else
				{
		 		    R=255-51*(inferieurA-5);
		 		    V=200;
				}
 		   	    HDC hdcStatic = (HDC) wParam;
		        SetBkColor(hdcStatic, COLORREF( GetSysColor( COLOR_3DFACE ) ) );
		        SetTextColor(hdcStatic, RGB(R,V,0));
		        //SetBkColor(hdcStatic, RGB(230,230,230));
		        //SetBkMode(hdcStatic, TRANSPARENT);
		        return (INT_PTR)GetSysColorBrush( COLOR_3DFACE );
			}
		    return 0;
		}
		case WM_TIMER:
	 	{
 	 	    if(wParam == ID_TIMER)
 	 	    {
	  		    HWND hTextTime=GetDlgItem(hDlg,IDT_TEMPS);
	  		    ::tempsEcoule++;
	  		    string stemps="Temps écoulé : ";
	  		    string secondes;
	  		    string minutes;
	  		    string heures;
	  		    heures=to_string(int(tempsEcoule/3600));
	  		    minutes=to_string(int(int(tempsEcoule%3600)/60));
	  		    secondes=to_string(int(int(tempsEcoule%3600)%60));
	  		    stemps+=heures+":"+minutes+":"+secondes;
	  		    SetDlgItemText(hDlg,IDT_TEMPS,stemps.c_str());
			}
			return 0;
	 	}
	 	case WM_SETFOCUS:
		{

		     return 0;
	 	}
		default :
         return FALSE;
	}

}
BOOL APIENTRY DialogVerifActi(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

    switch (uMsg)
    {
        case WM_INITDIALOG:
      	{
		    hListView=CreateListView (hDlg, false);

		    LVCOLUMN lvc;
    		int iCol=0;
    		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT ;
    		lvc.iSubItem = 0;
        	lvc.pszText = (LPSTR)"Nom de l'activité";
        	lvc.cx = 150;
        	lvc.fmt = LVCFMT_LEFT;
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 1;
        	lvc.pszText = (LPSTR)"Nombre de places";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 2;
        	lvc.pszText = (LPSTR)"Jour";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	lvc.iSubItem = 3;
        	lvc.cx = 300;
        	lvc.pszText = (LPSTR)"Classes";
        	ListView_InsertColumn(hListView, iCol++, &lvc);

        	//Création des lignes
        	LVITEM lvI={0};
	  		lvI.mask      = LVIF_TEXT |LVIF_STATE;
	  		lvI.stateMask = 0;
	  		lvI.iSubItem  = 0;
	  		lvI.state     = 0;
			lvI.iItem  = 0;
			lvI.lParam=0;
			int nbItem=0;
			bool voeuNonRempli=false;

			for(int i=0;i<activite.size();i++)
			{
 			    //test pour voir si un jour est vide
   				lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
				ListView_InsertItem(hListView, &lvI);

				ListView_SetItemText(hListView,lvI.iItem,0,const_cast<char*>(activite[i].nom.c_str()));

				string nbPlaces=to_string(activite[i].placesDispo);
				ListView_SetItemText(hListView,lvI.iItem,1,const_cast<char*>(nbPlaces.c_str()));

				string nmJour[4]={"lundi","mardi","jeudi","vendredi"};
				ListView_SetItemText(hListView,lvI.iItem,2, const_cast<char*>(nmJour[activite[i].jour].c_str()));

				ListView_SetItemText(hListView,lvI.iItem,3,const_cast<char*>(activite[i].classe.c_str()));

			    lvI.iItem++;
        		lvI.lParam++;
        		lvI.iSubItem=0;
			}
		    return TRUE;
		}
		case WM_NOTIFY:
	 	{
 	        //fLog<<"Début"<<endl;
 	 		//fLog.flush();
   		    NMLISTVIEW *nmlv = (NMLISTVIEW*) lParam;
   		    //fLog<<nmlv->hdr.code<<endl;
 	 		//fLog.flush();
 	 	    if(nmlv->hdr.code==NM_DBLCLK)
 	 	    {
		 	    //fLog<<"NM_BDLCLK"<<endl;
 	 			//fLog.flush();
		 	    if(nmlv->iItem!=-1)
		 	    {
	  			    ::iItemModifActi=nmlv->iItem;
	  			    if(iItemModifActi==-1)
	  			    {
				  	    iItemModifActi=activite.size()-1;
				  	}
	  			}
	  			else
	  			{
	     		    LVHITTESTINFO infoItem={0};
	     			infoItem.pt=nmlv->ptAction;
	     			infoItem.pt.x=5;

	  			    ::iItemModifActi=SendMessage(hListView,LVM_HITTEST,0,(LPARAM)&infoItem);
	  			    if(iItemModifActi==-1)
	  			    {
				  	    iItemModifActi=activite.size()-1;
				  	}
				}
				//fLog<<"Après if(nmlv->iItem!=-1)"<<endl;
 	 			//fLog.flush();
				if(activite.size()==0)
				{
		  		    return true;
				}
				int iActiEnCourTemp=iActiEnCour;
				::iActiEnCour=iItemModifActi;

				string classe=activite[iItemModifActi].classe;

				if(DialogBox(hinstance, "DIALOGMODIFACTI" , hDlg, (DLGPROC)DialogModifActi)==IDCANCEL)
				{
				    activite[iItemModifActi].classe=classe;
				}
				::iActiEnCour=iActiEnCourTemp;

				ListView_SetItemText(hListView,iItemModifActi,0,const_cast<char*>(activite[iItemModifActi].nom.c_str()));
				string nbPlaces=to_string(activite[iItemModifActi].placesDispo);
				ListView_SetItemText(hListView,iItemModifActi,1,const_cast<char*>(nbPlaces.c_str()));
				string nmJour[4]={"lundi","mardi","jeudi","vendredi"};
				ListView_SetItemText(hListView,iItemModifActi,2, const_cast<char*>(nmJour[activite[iItemModifActi].jour].c_str()));
				ListView_SetItemText(hListView,iItemModifActi,3,const_cast<char*>(activite[iItemModifActi].classe.c_str()));

			}
			//delete nmlv
 	 	    //fLog<<"Fin"<<endl;
 	 		//fLog.flush();
 	 	    return TRUE;
	 	}
		case WM_COMMAND:
		{
	 	    if (LOWORD(wParam)==IDCANCEL)
	 		{
	 		    EndDialog(hDlg,IDCANCEL);
        		return TRUE;
			}
			return 0;
		}
		default :
         return FALSE;
    }
}
BOOL APIENTRY DialogModifActi(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

    switch (uMsg)
    {
        case WM_INITDIALOG:
      	{
	 	    SendDlgItemMessage(hDlg,IDE_UPDOWN,UDM_SETBUDDY,(WPARAM)GetDlgItem(hDlg,IDE_EDITMODIFPLACES),0);
			SendDlgItemMessage(hDlg,IDE_UPDOWN, UDM_SETRANGE, 0, MAKELPARAM(999, 0));
			if(activite.size()==0)
			{
	  		    return true;
			}
			SetDlgItemText(hDlg,IDE_EDITMODIFNAME,activite[iItemModifActi].nom.c_str());
			SetDlgItemInt(hDlg,IDE_EDITMODIFPLACES,activite[iItemModifActi].placesDispo,false);

			SendDlgItemMessage(hDlg,IDE_COMBOBOX,CB_ADDSTRING,(WPARAM) 0,(LPARAM) "Lundi");
		    SendDlgItemMessage(hDlg,IDE_COMBOBOX,CB_ADDSTRING,(WPARAM) 0,(LPARAM) "Mardi");
		    SendDlgItemMessage(hDlg,IDE_COMBOBOX,CB_ADDSTRING,(WPARAM) 0,(LPARAM) "Jeudi");
		    SendDlgItemMessage(hDlg,IDE_COMBOBOX,CB_ADDSTRING,(WPARAM) 0,(LPARAM) "Vendredi");

		    string jour[4]={"Lundi","Mardi","Jeudi","Vendredi"};
			ComboBox_SelectString(GetDlgItem(hDlg,IDE_COMBOBOX),-1,jour[activite[iItemModifActi].jour].c_str());
	 	    //iItemModifActi;
	 	    return true;
		}
		case WM_COMMAND:
		{
	 	    if(LOWORD(wParam)==ID_BUTTONCLASSES)
	 	    {
	 		    DialogBox(hinstance, "DIALOGSELECTCLASSE" , 0, (DLGPROC)DialogSelectClasseProc);
	 		    return TRUE;
			}
	 	    if(LOWORD(wParam)==IDOK)
	 		{
			    if(activite.size()==0)
				{
		  		    EndDialog(hDlg,IDCANCEL);
        			return TRUE;
		  		}
		  		char nom[100];
			    GetDlgItemText(hDlg,IDE_EDITMODIFNAME,nom,99);
			    activite[iItemModifActi].nom=string(nom);
			    activite[iItemModifActi].placesDispo=GetDlgItemInt(hDlg,IDE_EDITMODIFPLACES,NULL,false);

 	 		    int iItem=SendDlgItemMessage(hDlg,IDE_COMBOBOX,CB_GETCURSEL,0,0);
		 	    int tLen=SendDlgItemMessage(hDlg,IDE_COMBOBOX,CB_GETLBTEXTLEN,iItem,0)+1;
		 	    char *sListbox=new char[tLen];
		 	    SendDlgItemMessage(hDlg,IDE_COMBOBOX,CB_GETLBTEXT,iItem,(LPARAM)sListbox);

		 	    string jour[4]={"Lundi","Mardi","Jeudi","Vendredi"};
		 	    for(int i=0;i<4;i++)
		 	    {
 				    if(strcmp(jour[i].c_str(),sListbox)==0)
 				    {
   					    activite[iItemModifActi].jour=i;
   					    break;
   					}
				}
	 		    EndDialog(hDlg,IDOK);
        		return TRUE;
			}

			return 0;
		}
		default :
         return FALSE;
    }
}

LRESULT CALLBACK WndProcNewEditAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC WndProcOriginEditAbout;

BOOL APIENTRY DialogAboutProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    static HBITMAP hLogo;
    static HWND hEditAbout;
    static HWND hTabsAbout;
    switch (uMsg)
    {
        case WM_INITDIALOG:
      	{
      	    ifstream file("readme.txt");
      	    string content;
      	    string line;
            if(!file)
            {
                char erreurTitre[]="Erreur de lecture du fichier readme.txt";
                VoirErreur(GetLastError(),erreurTitre);
                content="Description";
            }
            while(getline(file, line))
            {
                content+=line+"\r\n";
            }

      	    hLogo=(HBITMAP)LoadImage(NULL,"resources/Images/logo.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
      	    if(hLogo==NULL)
            {
                char erreurTitre[]="Erreur de lecture du fichier resources/Images/logo.bmp";
                VoirErreur(GetLastError(),erreurTitre);
            }
            hTabsAbout = CreateWindowEx(0 , WC_TABCONTROL, "", WS_CHILD | WS_VISIBLE, 20, 280, 560, 320, hDlg, NULL, hinstance, NULL);
            hEditAbout = CreateWindowEx(0,"edit", content.c_str(), WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_LEFT | ES_AUTOVSCROLL | ES_WANTRETURN, 25, 308, 550, 280, hDlg, NULL ,hinstance, NULL);
            WndProcOriginEditAbout = (WNDPROC)SetWindowLong(hEditAbout, GWL_WNDPROC, (LONG)WndProcNewEditAbout);

            TC_ITEM tie;
			tie.mask = TCIF_TEXT;
			tie.pszText = (LPSTR)"Description";
   			TabCtrl_InsertItem(hTabsAbout, 1, &tie);
      		tie.pszText = (LPSTR)"Informations";
        	TabCtrl_InsertItem(hTabsAbout, 2, &tie);
        	tie.pszText = (LPSTR)"Remerciements";
        	TabCtrl_InsertItem(hTabsAbout, 3, &tie);
        	tie.pszText = (LPSTR)"Licence";
        	TabCtrl_InsertItem(hTabsAbout, 4, &tie);
        	tie.pszText = (LPSTR)"Important";
        	TabCtrl_InsertItem(hTabsAbout, 4, &tie);

      	    return TRUE;
      	}
      	case WM_PAINT :
        {

            HDC hdc;
            RECT rect;
            GetClientRect(hDlg, &rect);
            PAINTSTRUCT ps;

            hdc = BeginPaint(hDlg, &ps);

            DrawState(hdc,NULL,NULL,(long)hLogo,0,rect.right/2-128,12,0,0,DST_BITMAP);

            EndPaint(hDlg, &ps);



            return 0;
        }
        case WM_SIZE:
        {
            int width=LOWORD(lParam);
            int height=HIWORD(lParam);
            if(width< 300)width=300;
            if(height< 300)height=300;
            //MoveWindow(hLogo, , 12 , 0, 0, TRUE);
            SetWindowPos(GetDlgItem(hDlg, ID_ETCHEDFRAME),0,0,0,width-40,276,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
            //MoveWindow(GetDlgItem(hDlg, ID_ETCHEDFRAME), 20, 4, width-40, 276, FALSE);
            SetWindowPos(hTabsAbout,0,0,0,width-40,height-280,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
            //MoveWindow(hTabsAbout, 20, 280, width-40, height-280, FALSE);
            SetWindowPos(hEditAbout,0,0,0,width-50,height-320,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
            //MoveWindow(hEditAbout, 25, 308, width-50, height-320, FALSE);

            InvalidateRect(hDlg, NULL, FALSE);
            return 0;
        }
        case WM_NOTIFY:
        {
            LPNMHDR pnmhdr = (LPNMHDR)lParam  ;
			if(pnmhdr->code == TCN_SELCHANGE)
			{
				if(TabCtrl_GetCurSel(hTabsAbout) == 0)
				{
                    ifstream file("readme.txt");
                    string content;
                    string line;
                    if(!file)
                    {
                        char erreurTitre[]="Erreur de lecture du fichier readme.txt";
                        VoirErreur(GetLastError(),erreurTitre);
                        content="Description";
                    }
                    while(getline(file, line))
                    {
                        content+=line+"\r\n";
                    }

                    SetWindowText(hEditAbout, content.c_str());
				}
				if(TabCtrl_GetCurSel(hTabsAbout) == 1)
				{
                    ifstream file("resources/Files/Informations.txt");
                    string content;
                    string line;
                    if(!file)
                    {
                        char erreurTitre[]="Erreur de lecture du fichier resources/Files/Informations.txt";
                        VoirErreur(GetLastError(),erreurTitre);
                        content="Informations";
                    }
                    while(getline(file, line))
                    {
                        content+=line+"\r\n";
                    }

                    SetWindowText(hEditAbout, content.c_str());
				}
				if(TabCtrl_GetCurSel(hTabsAbout) == 2)
				{
                    ifstream file("resources/Files/Remerciements.txt");
                    string content;
                    string line;
                    if(!file)
                    {
                        char erreurTitre[]="Erreur de lecture du fichier resources/Files/Remerciements.txt";
                        VoirErreur(GetLastError(),erreurTitre);
                        content="Remerciements";
                    }
                    while(getline(file, line))
                    {
                        content+=line+"\r\n";
                    }

                    SetWindowText(hEditAbout, content.c_str());
				}
				if(TabCtrl_GetCurSel(hTabsAbout) == 3)
				{
                    ifstream file("LICENCE.txt");
                    string content;
                    string line;
                    if(!file)
                    {
                        char erreurTitre[]="Erreur de lecture du fichier LICENCE.txt";
                        VoirErreur(GetLastError(),erreurTitre);
                        content="Licence";
                    }
                    while(getline(file, line))
                    {
                        content+=line+"\r\n";
                    }

                    SetWindowText(hEditAbout, content.c_str());
				}
				if(TabCtrl_GetCurSel(hTabsAbout) == 4)
				{
                    ifstream file("resources/Files/IMPORTANT.txt");
                    string content;
                    string line;
                    if(!file)
                    {
                        char erreurTitre[]="Erreur de lecture du fichier resources/Files/IMPORTANT.txt";
                        VoirErreur(GetLastError(),erreurTitre);
                        content="Important";
                    }
                    while(getline(file, line))
                    {
                        content+=line+"\r\n";
                    }

                    SetWindowText(hEditAbout, content.c_str());
				}
			}
            return TRUE;
        }
      	case WM_COMMAND:
        {
            if (LOWORD(wParam)==IDCANCEL)
            {
                DeleteObject(hLogo);
                EndDialog(hDlg,IDCANCEL);
                return TRUE;
            }
        }
        default :
         return FALSE;
    }
}

LRESULT APIENTRY WndProcNewEditAbout(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if ( uMsg == WM_CHAR || uMsg == WM_KEYDOWN)
                                                                       return 0;//uMsg == WM_SETFOCUS || uMsg == WM_CONTEXTMENU ||
    switch (uMsg)
    {
        /*case WM_CONTEXTMENU:
        {
            HMENU hPopupMenu = CreatePopupMenu();
 	 	    //InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, IDM_SUPPRJOUR, "Supprimer pour ce jour");

            AppendMenu(hPopupMenu, MF_BYPOSITION | MF_STRING | MF_GRAYED, WM_UNDO, "Annuler");
            AppendMenu(hPopupMenu, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
    		AppendMenu(hPopupMenu, MF_BYPOSITION | MF_STRING, WM_COPY, "Copier");

            PostMessage(hwnd, TrackPopupMenuEx(hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD | TPM_NONOTIFY, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hwnd, NULL), 0, 0);
            return TRUE;
        }*/
        case WM_CUT:
        {
            return TRUE;
        }
        case WM_CLEAR:
        {
            return TRUE;
        }
        case WM_PASTE:
        {
            return TRUE;
        }
        default:
            return CallWindowProc(WndProcOriginEditAbout, hwnd, uMsg, wParam, lParam);
    }
   return CallWindowProc(WndProcOriginEditAbout, hwnd, uMsg, wParam, lParam);
}


string GetClipboardText()
{
    //char *text;
    char *textMem;
    string retour;
    if(IsClipboardFormatAvailable(CF_TEXT))
 	{
	    if(OpenClipboard(NULL))
	    {
	        textMem = (char*)GetClipboardData(CF_TEXT);

	        /* free() devra être utilisé une fois le texte utilisé */
	        //text = new char[lstrlen(textMem)+1];
	        //strcpy(text, textMem);
	    }

	    CloseClipboard();
    }
    retour=string(textMem);
    free(textMem);
    return retour;
}

int SetClipboardText(char const *text)
{
    int ret = 0;

    HANDLE hGlobalMem = GlobalAlloc(GHND, lstrlen(text)+1);
    char *lpGlobalMem = NULL;

    if(hGlobalMem!=NULL)
    {
	    lpGlobalMem = (char*)GlobalLock(hGlobalMem);

	    if(lpGlobalMem != NULL)
	    {
		    lstrcpy(lpGlobalMem, text);

		    GlobalUnlock(hGlobalMem);

		    if(OpenClipboard(NULL));
		 	{
			    EmptyClipboard();

			    if(SetClipboardData(CF_TEXT, hGlobalMem) != NULL)
			        ret = 1;

			    CloseClipboard();
			}
		}
    }
    return ret; /* 1 : Succeed, 0 : Failure */
}

int SetClipboardText(string const &text)
{
    int ret = 0;

    HANDLE hGlobalMem = GlobalAlloc(GHND, text.size()+1);
    char *lpGlobalMem = NULL;

    if(hGlobalMem!=NULL)
    {
	    lpGlobalMem = (char*)GlobalLock(hGlobalMem);

	    if(lpGlobalMem != NULL)
	    {
		    lstrcpy(lpGlobalMem, text.c_str());

		    GlobalUnlock(hGlobalMem);

		    if(OpenClipboard(NULL));
		 	{
			    EmptyClipboard();

			    if(SetClipboardData(CF_TEXT, hGlobalMem) != NULL)
			        ret = 1;

			    CloseClipboard();
			}
		}
    }
    return ret; /* 1 : Succeed, 0 : Failure */
}

void AssociationExtProg(const char *ext, const char *path)
{
    HKEY key=NULL;
    DWORD kSize=MAX_PATH;
    char mem[MAX_PATH] = "";

    if(RegOpenKeyEx(HKEY_CLASSES_ROOT, ext, 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS)
    {
    	RegQueryValueEx(key, NULL, 0, NULL, (BYTE*)mem, &kSize);
        strcat(mem, "\\shell\\open\\command");
        if(RegOpenKeyEx(HKEY_CLASSES_ROOT, mem, 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS)
        {
            sprintf(mem,"\"%s\" \"%%1\"", path);
            RegSetValueEx(key, NULL, 0, REG_SZ, (BYTE*)mem, strlen(mem)+1);
        }
    }
    RegCloseKey(key);
    return;
}

BOOL CALLBACK EnumChildProc(HWND hwnd,LPARAM lParam)
{
    EnumChildWinLParam *lParamEnumChild=(EnumChildWinLParam*)lParam;
    SCROLLINFO *infoScroll;
	infoScroll=lParamEnumChild->infoScroll;

    //char a[10];
    infoScroll->cbSize=sizeof(SCROLLINFO);
    //itoa(infoScroll->nPos,a,10);
    if(lParamEnumChild->scroll==SB_VERT)
    {
	    for(int i=0;i<hControl.size();i++)
	    {
		    if(hwnd==hControl[i].hwnd)
		    {
			    RECT rect;
			    POINT pt;
			    GetWindowRect(hwnd,&rect);
			    pt.x=rect.left;
			    pt.y=rect.top;
			    ScreenToClient(hWndMain,&pt);

		  	    SetWindowPos(hwnd,0,pt.x,hControl[i].rect.top-infoScroll->nPos,0,0,SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
		  	    break;
		  	}
		}
	}
	if(lParamEnumChild->scroll==SB_HORZ)
    {
	    for(int i=0;i<hControl.size();i++)
	    {
		    if(hwnd==hControl[i].hwnd)
		    {
			    RECT rect;
			    POINT pt;
			    GetWindowRect(hwnd,&rect);
			    pt.x=rect.left;
			    pt.y=rect.top;
			    ScreenToClient(hWndMain,&pt);

		  	    SetWindowPos(hwnd,0,hControl[i].rect.left-infoScroll->nPos,pt.y,0,0,SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
		  	    break;
		  	}
		}
	}
	    /*if(lParamEnumChild->event==SB_LINEDOWN && infoScroll->nPos<infoScroll->nMax)
	    {
		    RECT rect;
		    POINT pt;
		    GetWindowRect(hwnd,&rect);
		    pt.x=rect.left;
		    pt.y=rect.top;
		    ScreenToClient(hWndMain,&pt);
		    SetWindowPos(hwnd,0,pt.x,pt.y-1,0,0,SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
		}
		else if(lParamEnumChild->event==SB_LINEUP && infoScroll->nPos>infoScroll->nMin)
	    {
		    RECT rect;
		    POINT pt;
		    GetWindowRect(hwnd,&rect);
		    pt.x=rect.left;
		    pt.y=rect.top;
		    ScreenToClient(hWndMain,&pt);
		    SetWindowPos(hwnd,0,pt.x,pt.y-1,0,0,SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
		}
	}*/
    return TRUE;
}
BOOL CALLBACK EnumChildStartPosProc(HWND hwnd,LPARAM lParam)
{
    HWNDcontrol hControlTemp={0};
	hControlTemp.hwnd=hwnd;

	RECT rectTemp;
	GetWindowRect(hwnd,&rectTemp);
	POINT pt;
	pt.x=rectTemp.left;
    pt.y=rectTemp.top;
    ScreenToClient(hWndMain,&pt);
    rectTemp.left=pt.x;
    rectTemp.top=pt.y;

    pt.x=rectTemp.right;
    pt.y=rectTemp.bottom;
    ScreenToClient(hWndMain,&pt);
    rectTemp.right=pt.x;
    rectTemp.bottom=pt.y;

	hControlTemp.rect=rectTemp;
	hControl.push_back(hControlTemp);
    return TRUE;
}

bool ActualiserTabCombo(int top)
{

    if(Button_GetCheck(hCheckboxCasesVide)==BST_CHECKED)
    {
	    for(int i=0;i<hComboV1.size();i++)
	    {
		    RemplacerCasesVides(i, (char*)" ", (char*)"Pas de choix");

		    //réinitialisation
		    ComboBox_ResetContent(hComboV1[i]);
			ComboBox_ResetContent(hComboV2[i]);
			ComboBox_ResetContent(hComboV3[i]);

		    SendMessage(hComboV1[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Pas de choix");
		    SendMessage(hComboV2[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Pas de choix");
		    SendMessage(hComboV3[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM)"Pas de choix");

		    string str;
		    for(int j=0;j<activite.size();j++)
			{
				str=enfants[SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)top+i,0)].classe;
				if(activite[j].classe.find(str)!=std::string::npos && activite[j].jour==jourEnCourSaisie)
				{
				    SendMessage(hComboV1[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
				    SendMessage(hComboV2[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
				    SendMessage(hComboV3[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
				}
			}//fin for activite
			//affichage
	        SendMessage(hComboV1[i],CB_SELECTSTRING,(WPARAM)-1,(enfants[SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)top+i,0)].voeux[jourEnCourSaisie].voeu1!=-1) ? (LPARAM)activite[enfants[SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)top+i,0)].voeux[jourEnCourSaisie].voeu1].nom.c_str() :(LPARAM) "Pas de choix");
	        SendMessage(hComboV2[i],CB_SELECTSTRING,(WPARAM)-1,(enfants[SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)top+i,0)].voeux[jourEnCourSaisie].voeu2!=-1) ? (LPARAM)activite[enfants[SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)top+i,0)].voeux[jourEnCourSaisie].voeu2].nom.c_str() :(LPARAM) "Pas de choix");
			SendMessage(hComboV3[i],CB_SELECTSTRING,(WPARAM)-1,(enfants[SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)top+i,0)].voeux[jourEnCourSaisie].voeu3!=-1) ? (LPARAM)activite[enfants[SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)top+i,0)].voeux[jourEnCourSaisie].voeu3].nom.c_str() :(LPARAM) "Pas de choix");
		}
	}
	if(Button_GetCheck(hCheckboxCasesVide)==BST_UNCHECKED)
	{
	    for(int i=0;i<hComboV1.size();i++)
	    {
		    RemplacerCasesVides(i, (char*)"Pas de choix", (char*)" ");

		    //réinitialisation
		    ComboBox_ResetContent(hComboV1[i]);
			ComboBox_ResetContent(hComboV2[i]);
			ComboBox_ResetContent(hComboV3[i]);

		    SendMessage(hComboV1[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM)" ");
		    SendMessage(hComboV2[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM)" ");
		    SendMessage(hComboV3[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM)" ");

		    string str;
		    for(int j=0;j<activite.size();j++)
			{
				str=enfants[SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)top+i,0)].classe;
				if(activite[j].classe.find(str)!=std::string::npos && activite[j].jour==jourEnCourSaisie)
				{
				    SendMessage(hComboV1[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
				    SendMessage(hComboV2[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
				    SendMessage(hComboV3[i],CB_ADDSTRING,(WPARAM) 0,(LPARAM) activite[j].nom.c_str());
				}
			}//fin for activite

		    //affichage
	        SendMessage(hComboV1[i],CB_SELECTSTRING,(WPARAM)-1,(enfants[SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)top+i,0)].voeux[jourEnCourSaisie].voeu1!=-1) ? (LPARAM)activite[enfants[SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)top+i,0)].voeux[jourEnCourSaisie].voeu1].nom.c_str() :(LPARAM) " ");
	        SendMessage(hComboV2[i],CB_SELECTSTRING,(WPARAM)-1,(enfants[SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)top+i,0)].voeux[jourEnCourSaisie].voeu2!=-1) ? (LPARAM)activite[enfants[SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)top+i,0)].voeux[jourEnCourSaisie].voeu2].nom.c_str() :(LPARAM) " ");
			SendMessage(hComboV3[i],CB_SELECTSTRING,(WPARAM)-1,(enfants[SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)top+i,0)].voeux[jourEnCourSaisie].voeu3!=-1) ? (LPARAM)activite[enfants[SendMessage(hListboxNom,LB_GETITEMDATA,(WPARAM)top+i,0)].voeux[jourEnCourSaisie].voeu3].nom.c_str() :(LPARAM) " ");
		}
	}

    return 0;
}
