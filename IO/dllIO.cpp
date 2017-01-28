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

#define WINVER 0x0500
#define _WIN32_IE 0x0500

#include <windows.h>
#include "resource.h"
#include <fstream>
#include <string>
#include <vector>
#include <fcntl.h>
#include "Shlobj.h"

using namespace std;

#include "typesData.h"
#include "Exeptions.h"
#include "dllCommune.h"

#define DLLEXPORT __declspec (dllexport)

int DLLEXPORT OuvertureFeuille(HWND hwnd, HWND hwndToolTips,
                     HINSTANCE const &hinstance,
                     vector<enfantActi> &enfants, vector<InfoActivite> &activite,
                     string &pathFileOpen)
{
    string sBackUp=pathFileOpen;
    size_t found = sBackUp.rfind('\\');
    if(sBackUp[found+1]!='~' && sBackUp[found+2]!='&')
    {
        sBackUp.replace (found+1,2,"~&");
        if(sBackUp[sBackUp.size()-1]=='a')
        {
            sBackUp+="2";
        }
        if(access(sBackUp.c_str(),F_OK)!=-1)
		{
		    if(MessageBox(hwnd,"La sauvegarde automatique de la dernière modification de ce fichier a été détectée.\n\
Voulez-vous la récupérer ?","Sauvegarde automatique détectée",MB_ICONINFORMATION | MB_YESNO)==IDYES)
		    {
			    pathFileOpen=sBackUp;
			    CreateToolTip(hwnd, hwndToolTips, hinstance);
		    }
		}
	}


	//lecture
    ifstream FichierIn(pathFileOpen.c_str());
    if(!FichierIn)
	{
 	    VoirErreur(GetLastError(),"Erreur lors de la lecture du fichier");
   	    return false;
	}
	string line1;
	getline(FichierIn,line1);
	FichierIn.close();

	if(line1=="[Fichier GRMA]" || line1=="[Fichier GRMA2]")
	{
 	    ifstream FichierIn(pathFileOpen.c_str());
 	    string line;
	    if(!FichierIn)
		{
		    VoirErreur(GetLastError(),"Erreur lors de la lecture du fichier");
	   	    return false;
		}
 	    while(getline(FichierIn,line))
 	    {
 		    if(line=="[Informations activités]")
 		    {
			    activite.clear();
			    while(getline(FichierIn,line))
 	    		{
				    while((line.empty() || line[0]=='#') && !FichierIn.eof())
				    {
	 			   	    getline(FichierIn,line);
					}
					if(line=="[Informations élèves]" || FichierIn.eof())break;

					InfoActivite tempActi;
					//MessageBox(0,line.c_str(),"1",0);
					if(line.substr(0,3)=="{n°")
					{
                        size_t pos=line.find("}");
                        tempActi.ref=atoi(line.substr(3,pos-3).c_str());
                        getline(FichierIn,line);
                    }
                    else
                    {
                        tempActi.ref=activite.size();
                    }

 				    tempActi.nom=line;

 				    getline(FichierIn,line);
 				    tempActi.classe=line;

 				    getline(FichierIn,line);
 				    tempActi.placesDispo=atoi(line.c_str());

 				    getline(FichierIn,line);
 				    tempActi.jour=atoi(line.c_str());

	    			tempActi.demande=0;
	    			tempActi.placesRestantes=0;

	    			activite.push_back(tempActi);
				}
			}
			if(line=="[Informations élèves]")
 		    {
			    enfants.clear();

			    while(getline(FichierIn,line))
				{
   				    while((line.empty() || line[0]=='#') && !FichierIn.eof())
				    {
	 			   	    getline(FichierIn,line);
					}
					if(line=="[Informations activités]" || FichierIn.eof())
					{
				 	    break;
					}

					enfantActi tempEnfantOuverture;
					tempEnfantOuverture.nom=line;

					getline(FichierIn,line);
					tempEnfantOuverture.classe=line;

					for(int jour=0;jour<4;jour++)
		    		{
					    FichierIn>>tempEnfantOuverture.voeux[jour].voeu1;
					    FichierIn>>tempEnfantOuverture.voeux[jour].voeu2;
					    FichierIn>>tempEnfantOuverture.voeux[jour].voeu3;

					    tempEnfantOuverture.affectation[jour]=-1;
					}

					tempEnfantOuverture.ref=enfants.size();
					enfants.push_back(tempEnfantOuverture);
				}
				/*if(FichierIn.good()==false)
				{
   			        stringstream stream;
   			        stream<<"eof()="<<FichierIn.eof()<<endl;
   			        stream<<"fail()="<<FichierIn.fail()<<endl;
   			        stream<<"bad()="<<FichierIn.bad()<<endl;
   			        char a[10];
   			        itoa(stream.eof(),a,10);
   			        MessageBox(0,a,"a",0);
   				    MessageBox(0,stream.str().c_str(),"Erreurs du fichier",0);

				}
				{
				 	string test="";

				    itoa(enfants.size(),const_cast<char*>(test.c_str()),10);
					MessageBox(0,test.c_str(),"Nombre d'enfants",0);
				}*/

			}
			if(line=="[Informations activités]")
 		    {
			    activite.clear();

			    while(getline(FichierIn,line))
 	    		{
				    while(line.empty() && !FichierIn.eof() && line[0]=='#')
				    {
	 			   	    getline(FichierIn,line);
					}
					if(line=="[Informations élèves]" || FichierIn.eof())break;

					InfoActivite tempActi;
					//MessageBox(0,line.c_str(),"2",0);
			 	    if(line.substr(0,3)=="{n°")
					{
                        size_t pos=line.find("}");
                        tempActi.ref=atoi(line.substr(3,pos-3).c_str());
                        getline(FichierIn,line);
                    }
                    else
                    {
                        tempActi.ref=activite.size();
                    }

 				    tempActi.nom=line;

 				    getline(FichierIn,line);
 				    tempActi.classe=line;

 				    getline(FichierIn,line);
 				    tempActi.placesDispo=atoi(line.c_str());

 				    getline(FichierIn,line);
 				    tempActi.jour=atoi(line.c_str());

	    			tempActi.demande=0;
	    			tempActi.placesRestantes=0;

	    			activite.push_back(tempActi);

				}
			}
 		}
		FichierIn.close();
	}
	else
	{
	    FILE * pFile;
	    int compt=0;
		int nbActi=0;
		int nbEnfantSave=0;
	    pFile = fopen (pathFileOpen.c_str() , "r");
	    if(pFile==NULL)
	    {
	   	    VoirErreur(GetLastError(),"Erreur lors de la lecture du fichier");
	   	    return false;
		}

	    fscanf(pFile,"%i",&nbActi);
	    fscanf(pFile,"%i",&nbEnfantSave);

	    fpos_t pos;
	    fgetpos (pFile,&pos);
	    pos+=2;
	    fsetpos (pFile,&pos);
	    //activite.erase(activite.begin(),activite.end());
	    activite.clear();
	    //fscanf(pFile,"%i");
	    InfoActivite tempActi;
	    tempActi.demande=0;

	    int nbChrClasses;
	    for(int i=0;i<nbActi;i++)
	    {
		    char nom[100];
	        fscanf(pFile,"%[^\n] %d\n%d\n",nom, &tempActi.placesDispo, &nbChrClasses);
	        if(ferror (pFile) || errno==EILSEQ)
	        {
		   	    return 1;
			}
			tempActi.nom=string(nom);
	        char *ligneClasse = new char[nbChrClasses+1];
	 		fscanf(pFile,"%[^\n] %d\n",ligneClasse, &tempActi.jour);
	 		if(ferror (pFile) || errno==EILSEQ)
	 		{
	   		    return 1;
			}
	 		tempActi.classe=string(ligneClasse);

	 		tempActi.ref=i;
	        activite.push_back(tempActi);
	        delete[]ligneClasse;
		}

		//fgetpos (pFile,&pos);
		enfantActi tempEnfantOuverture;
		int var=ftell(pFile);
		int result;
	    //fscanf(pFile,"\n");
	    char nom[100];
	    char classe[100];
	    enfants.clear();

	    for(int i=0;i<nbEnfantSave;i++)
	    {

	        fscanf(pFile,"%[^\n] %[^\n] %d %d %d %d\n",nom, classe, &tempEnfantOuverture.voeux[0].voeu1,
				               &tempEnfantOuverture.voeux[0].voeu2, &tempEnfantOuverture.voeux[0].voeu3, &tempEnfantOuverture.ref);
            tempEnfantOuverture.nom=string(nom);
            tempEnfantOuverture.classe=string(classe);
            for(int jour=0;jour<4;jour++)
            {
                tempEnfantOuverture.affectation[jour]=-1;
			}

			enfants.push_back(tempEnfantOuverture);
		}

		for(int jour=1;jour<4;jour++)
		{
		    for(int i=0;i<nbEnfantSave;i++)
		    {
		        int ref;
		        fscanf(pFile,"%[^\n] %[^\n] %d %d %d %d\n",nom, classe, &tempEnfantOuverture.voeux[jour].voeu1,
					               &tempEnfantOuverture.voeux[jour].voeu2, &tempEnfantOuverture.voeux[jour].voeu3, &ref);
				enfants[i].voeux[jour]=tempEnfantOuverture.voeux[jour];
			}
		}
	    fclose (pFile);
	}

    return 0;
}

int DLLEXPORT SauvegardeFeuille(string const &pathFileOpen,
                                const vector<enfantActi> &enfants, const vector<InfoActivite> &activite, DWORD nFilterIndex=1) throw(myExeptions::Erreur)
{
    ofstream FichierOut(pathFileOpen.c_str(),ios::trunc);
    if(!FichierOut)
	{
        throw myExeptions::Erreur(GetLastError());
	}
	if(nFilterIndex==1 || nFilterIndex==2)
	{
    	if(nFilterIndex==1)
    	{
    	    FichierOut<<"[Fichier GRMA2]"<<endl;
    	    FichierOut<<"[Informations activités]"<<endl;
    	    FichierOut<<endl<<"#{n°[numéro de l'activité]}"<<endl;
        }
        else if(nFilterIndex==2)
        {
            FichierOut<<"[Fichier GRMA]"<<endl;
            FichierOut<<"[Informations activités]"<<endl<<endl;
        }

    	FichierOut<<"#nom de l'activité"<<endl;
        FichierOut<<"#Classes séparées par une tabulation"<<endl;
        FichierOut<<"#Nombre de places disponibles"<<endl;
        FichierOut<<"#Jour de l'activité"<<endl<<endl;

    	if(nFilterIndex==1)
    	{
        	for(int i=0;i<activite.size();i++)
         	{
         	    FichierOut<<"{n°"<<activite[i].ref<<"}"<<endl<<activite[i].nom<<endl<<activite[i].classe<<endl<<activite[i].placesDispo<<endl<<activite[i].jour<<endl<<endl;
        	}
        }
        else if(nFilterIndex==2)
        {
            for(int i=0;i<activite.size();i++)
         	{
         	    FichierOut<<activite[i].nom<<endl<<activite[i].classe<<endl<<activite[i].placesDispo<<endl<<activite[i].jour<<endl<<endl;
        	}
        }

    	FichierOut<<endl<<"[Informations élèves]"<<endl<<endl;

    	FichierOut<<"#Nom de l'enfant"<<endl;
    	FichierOut<<"#Classe de l'enfant"<<endl;
    	FichierOut<<"#[voeu 1 du Lundi] [voeu 2 du Lundi] [voeu 3 du Lundi]"<<endl;
    	FichierOut<<"#[voeu 1 du Mardi] [voeu 2 du Mardi] [voeu 3 du Mardi]"<<endl;
    	FichierOut<<"#[voeu 1 du Jeudi] [voeu 2 du Jeudi] [voeu 3 du Jeudi]"<<endl;
    	FichierOut<<"#[voeu 1 du Vendredi] [voeu 2 du Vendredi] [voeu 3 du Vendredi]"<<endl<<endl;

    	for(int i=0;i<enfants.size();i++)
    	{
    	    FichierOut<<enfants[i].nom<<endl<<enfants[i].classe<<endl;
    	    for(int jour=0;jour<4;jour++)
    	    {
    		    FichierOut<<enfants[i].voeux[jour].voeu1<<" "<<enfants[i].voeux[jour].voeu2<<" "<<enfants[i].voeux[jour].voeu3<<endl;
    		}
    		FichierOut<<endl;
    	}
    }
    else if(nFilterIndex==3)
    {
        FichierOut<<activite.size()<<endl;
        FichierOut<<enfants.size()<<endl;

        for(int i=0;i<activite.size();i++)
        {
            FichierOut<<activite[i].nom<<endl<<activite[i].placesDispo<<endl<<activite[i].classe.size()<<endl<<activite[i].classe<<endl<<activite[i].jour<<endl;
        }

        for(int jour=0;jour<4;jour++)
        {
            for(int i=0;i<enfants.size();i++)
            {
                FichierOut<<enfants[i].nom<<endl<<enfants[i].classe<<endl<<enfants[i].voeux[jour].voeu1<<" "<<enfants[i].voeux[jour].voeu2<<" "<<enfants[i].voeux[jour].voeu3<<" "<<enfants[i].ref<<endl;
            }
        }
    }

    FichierOut.close();
}

int DLLEXPORT OuvertureActivite(HWND hwnd, HWND hwndToolTips,
                                HINSTANCE const &hinstance,
                                vector<InfoActivite> &activite,
                                string &actiFileOpen)
{
	string sBackUp=actiFileOpen;
    size_t found = sBackUp.rfind('\\');
    if(sBackUp[found+1]=='~' && sBackUp[found+2]=='&')
    {

	}
	else
	{
        sBackUp.replace (found+1,2,"~&");
        if(sBackUp[sBackUp.size()-1]=='a')
        {
            sBackUp+="2";
        }
        if(access(sBackUp.c_str(),F_OK)!=-1)
		{
		    if(MessageBox(hwnd,"La sauvegarde automatique de la dernière modification de ce fichier a été détectée.\n\
Voulez-vous la récupérer ?","Sauvegarde automatique détectée",MB_ICONINFORMATION | MB_YESNO)==IDYES)
		    {
			    actiFileOpen=sBackUp;
			    CreateToolTip(hwnd, hwndToolTips, hinstance);
		    }
		}
	}

	ifstream FichierIn(actiFileOpen.c_str());
    if(!FichierIn)
	{
	    VoirErreur(GetLastError(),"Erreur lors de la lecture du fichier");
   	    return false;
	}
	string line;
	getline(FichierIn,line);
	FichierIn.close();
	if(line=="[Fichier EAC]" || line=="[Fichier EAC2]")
	{
	    ifstream FichierIn(actiFileOpen.c_str());
	    if(!FichierIn)
		{
		    VoirErreur(GetLastError(),"Erreur lors de la lecture du fichier");
	   	    return false;
		}

		while(getline(FichierIn,line))
		{
  		    if(line=="[Informations activités]")
  		    {
			    activite.clear();

			    getline(FichierIn,line);
			    while(!FichierIn.eof())
 	    		{
   				    while((line.empty() || line[0]=='#') && !FichierIn.eof())
				    {
	 			   	    getline(FichierIn,line);
					}
					if(FichierIn.eof())break;

		    		InfoActivite tempActi;

		    		if(line.substr(0,3)=="{n°")
					{
                        size_t pos=line.find("}");
                        tempActi.ref=atoi(line.substr(3,pos-3).c_str());
                        getline(FichierIn,line);
                    }
                    else
                    {
                        tempActi.ref=activite.size();
                    }

				    tempActi.nom=line;

				    getline(FichierIn,line);
				    tempActi.classe=line;

				    FichierIn>>tempActi.placesDispo;
			     	FichierIn>>tempActi.jour;

			     	tempActi.demande=0;
	    			tempActi.placesRestantes=0;

	    			activite.push_back(tempActi);

	    			getline(FichierIn,line);
				}
			}
		}

		FichierIn.close();
	}
	else
	{
	    FILE * pFile;
		int nbActivite=0;
	    pFile = fopen (actiFileOpen.c_str() , "r");
	    if(pFile==NULL)
	    {
	   	    VoirErreur(GetLastError(),"Erreur lors de l'écriture dans le fichier");
	   	    return false;
		}

	    fscanf(pFile,"%i\n",&nbActivite);
	    //activite.erase(activite.begin(),activite.end());
	    activite.clear();
	    InfoActivite tempActi;
	    int nbChrClasses;
	    for(int i=0;i<nbActivite;i++)
	    {
		    char nom[100];
	        fscanf(pFile,"%[^\n] %d\n%d\n",&nom, &tempActi.placesDispo, &nbChrClasses);
	        char *ligneClasse = new char[nbChrClasses+1];
	 		fscanf(pFile,"%[^\n] %d\n",ligneClasse, &tempActi.jour);

	 		tempActi.nom=string(nom);
	 		tempActi.classe=string(ligneClasse);
	 		tempActi.ref=i;
	        activite.push_back(tempActi);
	        delete[]ligneClasse;
		}
	    fclose (pFile);
	}
}

int DLLEXPORT SauvegardeActivite(string const &actiFileOpen, vector<InfoActivite> &activite, DWORD nFilterIndex=1) throw(myExeptions::Erreur)
{
    ofstream FichierOut(actiFileOpen.c_str(),ios::trunc);
    if(!FichierOut)
	{
	    throw myExeptions::Erreur(GetLastError());
	}

	if(nFilterIndex==1 || nFilterIndex==2)
	{
        if(nFilterIndex==1)
        {
            FichierOut<<"[Fichier EAC2]"<<endl;
            FichierOut<<"[Informations activités]"<<endl;
            for(int i=0;i<activite.size();i++)
         	{
         	    FichierOut<<"{n°"<<activite[i].ref<<"}"<<endl<<activite[i].nom<<endl<<activite[i].classe<<endl<<activite[i].placesDispo<<endl<<activite[i].jour<<endl<<endl;
        	}
        }
        else if(nFilterIndex==2)
        {
    	    FichierOut<<"[Fichier EAC]"<<endl;
    	    FichierOut<<"[Informations activités]"<<endl;
    	    for(int i=0;i<activite.size();i++)
         	{
         	    FichierOut<<activite[i].nom<<endl<<activite[i].classe<<endl<<activite[i].placesDispo<<endl<<activite[i].jour<<endl<<endl;
        	}
        }
    }

	FichierOut.close();
}


bool DLLEXPORT BackUp(string const &pathFileOpen, string const &actiFileOpen,
                      vector<enfantActi> &enfants, vector<InfoActivite> &activite,
                      int const &backUpActivite, bool const &traitementLance)
{
    if(backUpActivite==-1 && !traitementLance)
    {
	    /*char currentDirectory[MAX_PATH];
	    GetCurrentDirectory(MAX_PATH,currentDirectory);
	    if(GetFileAttributes(currentDirectory)==FILE_ATTRIBUTE_READONLY)*/
	    string spathFileOpen;
		spathFileOpen="~&Nouveau document GRMA.grma2";

		try
		{
		    SauvegardeFeuille(spathFileOpen, enfants, activite);
        }
        catch(...){}

		SetFileAttributes(spathFileOpen.c_str(),FILE_ATTRIBUTE_HIDDEN);
	}
    else if(backUpActivite==0)
    {
	    string sactiFileOpen=actiFileOpen;
	    size_t found = sactiFileOpen.rfind('\\');
	    sactiFileOpen.replace (found+1,2,"~&");

	    try
		{
		    SauvegardeActivite(sactiFileOpen, activite);
        }
        catch(...){}

	    /*FILE *pFile;
	    int fildes;

	    pFile=fopen(sactiFileOpen.c_str(),"w");
	    if(pFile!=NULL)fclose(pFile);

	    fildes=open (sactiFileOpen.c_str(), O_WRONLY);//O_WRONLY|O_TRUNC );
	    pFile=fdopen (fildes, "w");
	    if(pFile==NULL)
	    {
	   	    return false;
		}
	    //pFile = fopen (sactiFileOpen.c_str() , "w");
	    fprintf(pFile,"%i\n",activite.size());
     	for(int i=0;i<activite.size();i++)
     	{
     	    fprintf(pFile,"%s\n%d\n%d\n%s\n%d\n", activite[i].nom.c_str(), activite[i].placesDispo, activite[i].classe.size(), activite[i].classe.c_str(), activite[i].jour);
		}
		fflush(pFile);
		fclose(pFile);
		close(fildes);;*/
		SetFileAttributes(sactiFileOpen.c_str(),FILE_ATTRIBUTE_HIDDEN);
	}
    else if(backUpActivite==1 && !traitementLance)
    {
	    string spathFileOpen=pathFileOpen;
	    size_t found = spathFileOpen.rfind('\\');
	    spathFileOpen.replace (found+1,2,"~&");
	    if(spathFileOpen[spathFileOpen.size()-1]=='a')
        {
            spathFileOpen+="2";
        }

	    try
		{
		    SauvegardeFeuille(spathFileOpen, enfants, activite);
        }
        catch(...){}

		SetFileAttributes(spathFileOpen.c_str(),FILE_ATTRIBUTE_HIDDEN);
	}
    return 0;
}
