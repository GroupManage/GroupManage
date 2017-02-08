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
#include <vector>
#include <string>
#include <math.h>
#include <time.h>
#include <algorithm>
#include <cstdio>
#include <sstream>

using namespace std;

#include "typesData.h"
#include "Exeptions.h"
#include "resource.h"

#include "dllTraitement.h"

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

bool DejasFait(int Jour, int acti, enfantActi enfantEnCour)
{
    char a[4];
    //jour=2;
    for(int jour=0;jour<Jour;jour++)//i<Jour
    {
	 		/*itoa(i,a,10);
	 		MessageBox(0,a,"DejasFait() 2",0);*/
        if(enfantEnCour.affectation[jour]==acti)
        {
		    return true;
		}
        /*if(enfants.size()!=0)
        {
		    for(int i=0;i<enfants.size();i++)
		    {
	 		    if(enfants[i].ref==enfantEnCour.ref)
	 		    {
				    if(enfants[i].affectation==acti)
				    {
        		        return true;
					}
					break;
				}
	        }
		}//fin if(nbEnfants[i]!=0)*/
	}
	return false;

}

void Traitement(vector<enfantActi> &enfants, vector<InfoActivite> &activite)
{
    string test;
    int nbEnfantsAttrib[4]={0,0,0,0};
    int nbEnfantsAttribAvant[4]={-1,-1,-1,-1};
    char a[3];

    int nbEnfants[4];
    for(int jour=0;jour<4;jour++)
    {
	    int nbEnfantJour=0;
	    for(int i=0;i<enfants.size();i++)
	    {
	        if((enfants[i].voeux[jour].voeu1!=-1 || enfants[i].voeux[jour].voeu2!=-1 || enfants[i].voeux[jour].voeu3!=-1) && enfants[i].nom!="")
	        {
			    nbEnfantJour++;
			}
	    }
	    nbEnfants[jour]=nbEnfantJour;
	}


    for(int jour=0;jour<4;jour++)
    {
	    for(int k=0;k<activite.size();k++)
	    {
		    activite[k].placesRestantes=activite[k].placesDispo;
			activite[k].demande=0;
		}
	    if(nbEnfants[jour]!=0)
	    {
		    for(int i=0;i<4;i++)
		    {
		        nbEnfantsAttrib[i]=0;
		        nbEnfantsAttribAvant[i]=-1;
			}
		    //while(nbEnfantsAttrib[jour]!=nbEnfantsAttribAvant[jour])
		    //{
 		    //nbEnfantsAttribAvant[jour]=nbEnfantsAttrib[jour];
			//nbEnfantsAttrib[jour]=0;
		    //Tout ceux pouvant être attribués d'office à leur voeu 1
		    for(int i=0;i<enfants.size();i++)
		    {
		    	{
				    if(enfants[i].affectation[jour]==-1 && enfants[i].voeux[jour].voeu1!=-1 && !DejasFait(jour, enfants[i].voeux[jour].voeu1 , enfants[i]))
				    {
				        activite[enfants[i].voeux[jour].voeu1].demande++;
					}
				}
			}

			for(int i=0;i<activite.size();i++)
			{
			    if(activite[i].demande<=activite[i].placesRestantes && activite[i].demande!=0)
			    {
				    activite[i].placesRestantes=activite[i].placesRestantes-activite[i].demande;
	  			    for(int j=0;j<enfants.size();j++)
		    		{
					    if(enfants[j].affectation[jour]==-1 && enfants[j].voeux[jour].voeu1==i && !DejasFait(jour, i , enfants[j]))
					    {
		 				    enfants[j].affectation[jour]=i;
		 				    enfants[j].point++;
		 				    enfants[j].denominateur+=3;
		 				    nbEnfantsAttrib[jour]++;
		 				}
					}
	  			}
	  			activite[i].demande=0;
			}
				/*//Tout ceux pouvant être attribués à leurs voeu 2 en bouchants les trous des activités
				for(int i=0;i<enfants[jour].size();i++)
			    {
				    if(enfants[jour][i].affectation==-1 && enfants[jour][i].voeu2!=-1 && !DejasFait(jour, enfants[jour][i].voeu2 , enfants[jour][i]))
				    {
					    if(activite[enfants[jour][i].voeu2].placesRestantes<activite[enfants[jour][i].voeu2].placesDispo)
					    {
				            activite[enfants[jour][i].voeu2].demande++;
						}
					}
				}
				for(int i=0;i<activite.size();i++)
				{
				    if(activite[i].demande<=activite[i].placesRestantes && activite[i].demande!=0)
				    {
		  			    activite[i].placesRestantes-=activite[i].demande;
		  			    for(int j=0;j<enfants[jour].size();j++)
			    		{
						    if(enfants[jour][j].affectation==-1 && enfants[jour][j].voeu2==i && !DejasFait(jour, i , enfants[jour][j]))
						    {
			 				    enfants[jour][j].affectation=i;
			 				    referenceEleve[enfants[jour][j].ref].point+=2;
			 				    referenceEleve[enfants[jour][j].ref].denominateur+=3;
			 				    nbEnfantsAttrib[jour]++;
			 				}
						}
		  			}
		  			activite[i].demande=0;
				}
				if(jour==1)
				{
	   			    string test="";
					for(int i=0;i<enfants[jour].size();i++)
					{
	 				    if(enfants[jour][i].affectation!=-1)
	 				    {
						    test+=string(enfants[jour][i].np)+"->"+string(activite[enfants[jour][i].affectation].nom)+"\n";
						}
					}
					MessageBox(0,test.c_str(),0,0);
				}*/
			//}

			srand(time(NULL));

			//préparation au tirage aléatoir : on remplit un tableau
			int nbEnfantsNonAttrib=0;
			int eleveChoisi;
			vector<int> vectNA;
			for(int k=0;k<activite.size();k++)
			{
	  		    if(activite[k].placesRestantes!=0)
	  			{
					for(int i=0;i<enfants.size();i++)
					{
					    if(enfants[i].affectation[jour]==-1 && enfants[i].voeux[jour].voeu1==k && !DejasFait(jour, enfants[i].voeux[jour].voeu1 , enfants[i]))
					    {
						    for(int l=0;l<((enfants[i].point==0) ? 1 : (enfants[i].point));l++)
						    {
	        				    vectNA.push_back(i);
							}
					    }
					}
					//tirage
					if(vectNA.begin()!=vectNA.end())
					{
						random_shuffle(vectNA.begin(), vectNA.end());
						for(int i=0;activite[k].placesRestantes>0 && i<vectNA.size();i++)
						{
			 			    if(enfants[vectNA[i]].affectation[jour]==-1)
			 			    {
					    	    enfants[vectNA[i]].affectation[jour]=k;
					    	    enfants[vectNA[i]].point+=1;
					    	    enfants[vectNA[i]].denominateur+=3;
					    	    nbEnfantsAttrib[jour]++;
					    	    activite[k].placesRestantes--;
							}
						}
					}
				}
				vectNA.clear();
			}

			/*test="";
			for(int i=0;i<nbEnfants[jour];i++)
			{
			        itoa(referenceEleve[enfants[jour][i].ref].point,a,10);
			        //strcat((char*)test.c_str(),a);
			        test=test+enfants[jour][i].np+" : "+a+"\n";
			}
			MessageBox(0,test.c_str(),"Après tirage au sort V1",0);*/

			//affectation voeu 2
			for(int k=0;k<activite.size();k++)
			{
 			    activite[k].demande=0;
 			    if(activite[k].placesRestantes!=0)
	  			{
					for(int i=0;i<enfants.size();i++)
					{
		 			    if(enfants[i].affectation[jour]==-1 && enfants[i].voeux[jour].voeu2==k && !DejasFait(jour, enfants[i].voeux[jour].voeu2 , enfants[i]))
		   				{
						    activite[k].demande++;
						    for(int l=0;l<((enfants[i].point==0) ? 1 : (enfants[i].point));l++)
						    {
	        				    vectNA.push_back(i);
							}
						}
					}
					if(activite[k].demande<=activite[k].placesRestantes && activite[k].demande!=0)
					{
					    activite[k].placesRestantes-=activite[k].demande;
   	 					for(int i=0;i<enfants.size();i++)
						{
			 		        if(enfants[i].affectation[jour]==-1 && enfants[i].voeux[jour].voeu2==k && !DejasFait(jour, enfants[i].voeux[jour].voeu2 , enfants[i]))
		   					{
	 				   	        enfants[i].affectation[jour]=k;
 				    		    enfants[i].point+=2;
 				    		    enfants[i].denominateur+=3;
		 				        nbEnfantsAttrib[jour]++;
							}
 						}

					}
					//tirage
					else if(activite[k].demande>activite[k].placesRestantes && activite[k].demande!=0)
					{
						if(vectNA.begin()!=vectNA.end())
						{
							random_shuffle(vectNA.begin(), vectNA.end());
							for(int i=0;activite[k].placesRestantes>0 && i<vectNA.size();i++)
							{
				 			    if(enfants[vectNA[i]].affectation[jour]==-1)
				 			    {
						    	    enfants[vectNA[i]].affectation[jour]=k;
						    	    enfants[vectNA[i]].point+=2;
						    	    enfants[vectNA[i]].denominateur+=3;
						    	    nbEnfantsAttrib[jour]++;
						    	    activite[k].placesRestantes--;
								}
							}
						}
					}
					activite[k].demande=0;
				}
				vectNA.clear();
			}
			/*test="";
			for(int i=0;i<nbEnfants[jour];i++)
			{
			        itoa(referenceEleve[enfants[jour][i].ref].point,a,10);
			        //strcat((char*)test.c_str(),a);
			        test=test+enfants[jour][i].np+" : "+a+"\n";
			}
			MessageBox(0,test.c_str(),"Après tirage au sort V2",0);*/

			//affectation voeu 3
			for(int k=0;k<activite.size();k++)
			{
 			    activite[k].demande=0;
 			    if(activite[k].placesRestantes!=0)
	  			{
					for(int i=0;i<enfants.size();i++)
					{
		 			    if(enfants[i].affectation[jour]==-1 && enfants[i].voeux[jour].voeu3==k && !DejasFait(jour, enfants[i].voeux[jour].voeu3 , enfants[i]))
		   				{
						    activite[k].demande++;
						    for(int l=0;l<((enfants[i].point==0) ? 1 : (enfants[i].point));l++)
						    {
	        				    vectNA.push_back(i);
							}
						}
					}
					if(activite[k].demande<=activite[k].placesRestantes && activite[k].demande!=0)
					{
					    activite[k].placesRestantes-=activite[k].demande;
					    for(int i=0;i<enfants.size();i++)
						{
			 		        if(enfants[i].affectation[jour]==-1 && enfants[i].voeux[jour].voeu3==k && !DejasFait(jour, enfants[i].voeux[jour].voeu3 , enfants[i]))
		   					{
	 				   	        enfants[i].affectation[jour]=k;
 				    			enfants[i].point+=3;
 				    			enfants[i].denominateur+=3;
		 				    	nbEnfantsAttrib[jour]++;
							}
 						}
					}
					//tirage
					else if(activite[k].demande!=0)
					{
						if(vectNA.begin()!=vectNA.end())
						{
							random_shuffle(vectNA.begin(), vectNA.end());
							for(int i=0;activite[k].placesRestantes>0 && i<vectNA.size();i++)
							{
				 			    if(enfants[vectNA[i]].affectation[jour]==-1)
				 			    {
						    	    enfants[vectNA[i]].affectation[jour]=k;
						    	    enfants[vectNA[i]].point+=3;
						    	    enfants[vectNA[i]].denominateur+=3;
						    	    nbEnfantsAttrib[jour]++;
						    	    activite[k].placesRestantes--;
								}
							}
						}
					}
					activite[k].demande=0;
				}
				vectNA.clear();
			}

			/*test="";
			{
			    char a[10];
			    itoa(enfants[0].affectation[jour],a,10);
                test=enfants[0].nom+" : "+((enfants[0].affectation[jour]!=-1) ? activite[enfants[0].affectation[jour]].nom : "-1")+" = "+a+"\n";
            }
			MessageBox(0,test.c_str(),"Après tirage au sort V3",0);*/

			//on re regarde si des places ont été libérées
			for(int i=0;i<enfants.size();i++)
			{
			    if(enfants[i].affectation[jour]==-1)
			    {
				    //char a[10];
				    //itoa(activite[enfants[jour][i].voeu2].placesRestantes,a,10);
			        //MessageBox(0,a,activite[enfants[jour][i].voeu2].nom,0);
			        //MessageBox(0,enfants[jour][i].np,0,0);
			        if(activite[enfants[i].voeux[jour].voeu1].placesRestantes!=0)
			        {
				   	    enfants[i].affectation[jour]=enfants[i].voeux[jour].voeu1;
				   	    enfants[i].point+=1;
			    	    enfants[i].denominateur+=3;
			    	    nbEnfantsAttrib[jour]++;
			    	    activite[enfants[i].voeux[jour].voeu1].placesRestantes--;
				   	}
				   	else if(activite[enfants[i].voeux[jour].voeu2].placesRestantes!=0)
			        {
				   	    enfants[i].affectation[jour]=enfants[i].voeux[jour].voeu2;
				   	    enfants[i].point+=2;
			    	    enfants[i].denominateur+=3;
			    	    nbEnfantsAttrib[jour]++;
			    	    activite[enfants[i].voeux[jour].voeu2].placesRestantes--;
				   	}
				   	else if(activite[enfants[i].voeux[jour].voeu3].placesRestantes!=0)
			        {
				   	    enfants[i].affectation[jour]=enfants[i].voeux[jour].voeu3;
				   	    enfants[i].point+=3;
			    	    enfants[i].denominateur+=3;
			    	    nbEnfantsAttrib[jour]++;
			    	    activite[enfants[i].voeux[jour].voeu3].placesRestantes--;
				   	}
				}
			}
			//on essai d'attribuer ceux qui n'ont pas put l'être
			/*
			for(int i=0;i<enfants[jour].size();i++)
			{
 			    if(enfants[jour][i].affectation==-1)
 			    {
				    for(int j=0;j<enfants[jour].size();j++)
				    {
 					    //test pour son voeu 1
 					    if(enfants[jour][j].affectation==enfants[jour][i].voeu1 && enfants[jour][i].voeu1!=-1 && !DejasFait(jour, enfants[jour][i].voeu1 , enfants[jour][i]))
 					    {
						    //si il reste des places dans le voeu 2 et si celui dont on va changer l'affectation ne l'a pas déjà fait...
						    if(enfants[jour][j].voeu2!=-1 && enfants[jour][j].affectation!=enfants[jour][j].voeu2 && activite[enfants[jour][j].voeu2].placesRestantes>0 && !DejasFait(jour, enfants[jour][j].voeu2 , enfants[jour][j]))
						    {
						  	    //si son nombre de point <= 2/3 4/6 6/9 8/12
						  	    if(referenceEleve[enfants[jour][j].ref].denominateur!=0 && float(referenceEleve[enfants[jour][j].ref].point*1.0/referenceEleve[enfants[jour][j].ref].denominateur*1.0)<=2.0/3.0)
						  	    {
							  	    enfants[jour][i].affectation=enfants[jour][i].voeu1;
							  	    referenceEleve[enfants[jour][i].ref].point+=1;
							  	    referenceEleve[enfants[jour][i].ref].denominateur+=3;

							  	    if(enfants[jour][j].affectation==enfants[jour][j].voeu1)
							  	    {
							  	        referenceEleve[enfants[jour][j].ref].point-=1;
									}
									else
									{
							 		    referenceEleve[enfants[jour][j].ref].point-=3;
									}
							  	    referenceEleve[enfants[jour][j].ref].point+=2;
							  	    enfants[jour][j].affectation=enfants[jour][j].voeu2;
							  	    activite[enfants[jour][j].voeu2].placesRestantes--;

							  	    break;
								}
						  	}
						  	//si il reste des places dans le voeu 3 et si celui dont on va changer l'affectation ne l'a pas déjà fait...
						  	if(enfants[jour][j].voeu3!=-1 && enfants[jour][j].affectation!=enfants[jour][j].voeu3 && activite[enfants[jour][j].voeu3].placesRestantes>0 && !DejasFait(jour, enfants[jour][j].voeu3 , enfants[jour][j]))
						    {
						  	    //si son nombre de point <= 2/3 4/6 6/9 8/12
						  	    if(float(referenceEleve[enfants[jour][j].ref].point*1.0/referenceEleve[enfants[jour][j].ref].denominateur*1.0)<=2/3.0)
						  	    {
							  	    enfants[jour][i].affectation=enfants[jour][i].voeu1;
							  	    referenceEleve[enfants[jour][i].ref].point+=1;
								  	referenceEleve[enfants[jour][i].ref].denominateur+=3;

							  	    if(enfants[jour][j].affectation==enfants[jour][j].voeu1)
							  	    {
									    referenceEleve[enfants[jour][j].ref].point-=1;
									}
									else
									{
							 		    referenceEleve[enfants[jour][j].ref].point-=2;
									}
							  	    enfants[jour][j].affectation=enfants[jour][j].voeu3;
							  	    referenceEleve[enfants[jour][j].ref].point+=3;
							  	    activite[enfants[jour][j].voeu3].placesRestantes--;

							  	    break;
								}
						  	}
						}
						//test pour son voeu 2
						if(enfants[jour][j].affectation==enfants[jour][i].voeu2 && enfants[jour][i].voeu2!=-1 && !DejasFait(jour, enfants[jour][i].voeu2 , enfants[jour][i]))
 					    {
						    //si il reste des places dans le voeu 2 et si celui dont on va changer l'affectation ne l'a pas déjà fait...
						    if(enfants[jour][j].voeu2!=-1 && enfants[jour][j].affectation!=enfants[jour][j].voeu2 && activite[enfants[jour][j].voeu2].placesRestantes>0 && !DejasFait(jour, enfants[jour][j].voeu2 , enfants[jour][j]))
						    {
						  	    //si son nombre de point <= 2/3 4/6 6/9 8/12
						  	    if(float(referenceEleve[enfants[jour][j].ref].point*1.0/referenceEleve[enfants[jour][j].ref].denominateur*1.)<=2/3.0)
						  	    {
							  	    enfants[jour][i].affectation=enfants[jour][i].voeu2;
							  	    referenceEleve[enfants[jour][i].ref].point+=2;
							  	    referenceEleve[enfants[jour][i].ref].denominateur+=3;

							  	    if(enfants[jour][j].affectation==enfants[jour][j].voeu1)
							  	    {
							  	        referenceEleve[enfants[jour][j].ref].point-=1;
									}
									else
									{
							 		    referenceEleve[enfants[jour][j].ref].point-=3;
									}
							  	    enfants[jour][j].affectation=enfants[jour][j].voeu2;
							  	    referenceEleve[enfants[jour][j].ref].point+=2;
							  	    activite[enfants[jour][j].voeu2].placesRestantes--;

							  	    break;
								}
						  	}
						  	//si il reste des places dans le voeu 3 et si celui dont on va changer l'affectation ne l'a pas déjà fait...
						  	if(enfants[jour][j].voeu3!=-1 && enfants[jour][j].affectation!=enfants[jour][j].voeu3 && activite[enfants[jour][j].voeu3].placesRestantes>0 && !DejasFait(jour, enfants[jour][j].voeu3 , enfants[jour][j]))
						    {
						  	    //si son nombre de point <= 2/3 4/6 6/9 8/12
						  	    if(float(referenceEleve[enfants[jour][j].ref].point*1./referenceEleve[enfants[jour][j].ref].denominateur*1.)<=2/3.0)
						  	    {
							  	    enfants[jour][i].affectation=enfants[jour][i].voeu2;
							  	    referenceEleve[enfants[jour][i].ref].point+=2;
							  	    referenceEleve[enfants[jour][i].ref].denominateur+=3;

							  	    if(enfants[jour][j].affectation==enfants[jour][j].voeu1)
							  	    {
									    referenceEleve[enfants[jour][j].ref].point-=1;
									}
									else
									{
							 		    referenceEleve[enfants[jour][j].ref].point-=2;
									}
							  	    enfants[jour][j].affectation=enfants[jour][j].voeu3;
							  	    referenceEleve[enfants[jour][j].ref].point+=3;
							  	    activite[enfants[jour][j].voeu3].placesRestantes--;

							  	    break;
								}
						  	}
						}
						//test pour son voeu 3
						if(enfants[jour][j].affectation==enfants[jour][i].voeu3 && enfants[jour][i].voeu3!=-1 && !DejasFait(jour, enfants[jour][i].voeu3 , enfants[jour][i]))
 					    {
						    //si il reste des places dans le voeu 2 et si celui dont on va changer l'affectation ne l'a pas déjà fait...
						    if(enfants[jour][j].voeu2!=-1 && enfants[jour][j].affectation!=enfants[jour][j].voeu2 && activite[enfants[jour][j].voeu2].placesRestantes>0 && !DejasFait(jour, enfants[jour][j].voeu2 , enfants[jour][j]))
						    {
						  	    //si son nombre de point <= 2/3 4/6 6/9 8/12
						  	    if(float(referenceEleve[enfants[jour][j].ref].point*1./referenceEleve[enfants[jour][j].ref].denominateur*1.)<=2/3.0)
						  	    {
							  	    enfants[jour][i].affectation=enfants[jour][i].voeu3;
							  	    referenceEleve[enfants[jour][i].ref].point+=3;
							  	    referenceEleve[enfants[jour][i].ref].denominateur+=3;

							  	    if(enfants[jour][j].affectation==enfants[jour][j].voeu1)
							  	    {
							  	        referenceEleve[enfants[jour][j].ref].point-=1;
									}
									else
									{
							 		    referenceEleve[enfants[jour][j].ref].point-=3;
									}
							  	    enfants[jour][j].affectation=enfants[jour][j].voeu2;
							  	    referenceEleve[enfants[jour][j].ref].point+=2;
							  	    activite[enfants[jour][j].voeu2].placesRestantes--;

							  	    break;
								}
						  	}
						  	//si il reste des places dans le voeu 3 et si celui dont on va changer l'affectation ne l'a pas déjà fait...
						  	if(enfants[jour][j].voeu3!=-1 && enfants[jour][j].affectation!=enfants[jour][j].voeu3 && activite[enfants[jour][j].voeu3].placesRestantes>0 && !DejasFait(jour, enfants[jour][j].voeu3 , enfants[jour][j]))
						    {
						  	    //si son nombre de point <= 2/3 4/6 6/9 8/12
						  	    if(float(referenceEleve[enfants[jour][j].ref].point/referenceEleve[enfants[jour][j].ref].denominateur)<=2/3.0)
						  	    {
							  	    enfants[jour][i].affectation=enfants[jour][i].voeu3;
							  	    referenceEleve[enfants[jour][i].ref].point+=3;
							  	    referenceEleve[enfants[jour][i].ref].denominateur+=3;

							  	    if(enfants[jour][j].affectation==enfants[jour][j].voeu1)
							  	    {
									    referenceEleve[enfants[jour][j].ref].point-=1;
									}
									else
									{
							 		    referenceEleve[enfants[jour][j].ref].point-=2;
									}
							  	    enfants[jour][j].affectation=enfants[jour][j].voeu3;
							  	    referenceEleve[enfants[jour][j].ref].point+=3;
							  	    activite[enfants[jour][j].voeu3].placesRestantes--;

							  	    break;
								}
						  	}
						}
					}//fin for
				}
			}//fin attrib pas put*/
			/*test="";
			for(int i=0;i<enfants[jour].size();i++)
			{
			    if(enfants[jour][i].affectation !=-1)
			    {
				    test+=string(enfants[jour][i].np)+"->"+string(activite[enfants[jour][i].affectation].nom)+"\n";
				}
			}
			MessageBox(0,test.c_str(),"test",0);*/

			for(int i=0;i<enfants.size();i++)
			{
			    if(enfants[i].affectation[jour]==-1)
			    {
				 	enfants[i].point+=4;
				 	enfants[i].denominateur+=3;
				}
				//itoa(referenceEleve[enfants[jour][i].ref].point,a,10);
        		//test=test+enfants[jour][i].np+" : "+a+"\n";
			}
			//MessageBox(0,test.c_str(),"Fin jour",0);

		}//fin if(nbEnfants[jour]!=0)
	}//fin boucle des jours
}

int DLLEXPORT Initialisation(vector<enfantActi> &enfants, vector<enfantActi> &enfantsAttributed, vector<InfoActivite> &activite,
                              char &inferieurA, bool &traitementLance, bool &arretTraitement,
                              HWND &hDlgTraitementEnCours)
{
    for(int i=0;i<enfants.size();)
	{
	    if(enfants[i].nom=="")
	    {
		    enfants.erase(enfants.begin()+i);
		}
		else
		{
	 	    i++;
	 	}
	}

	//On supprime les élèves qui n'ont rien
	/*for(int i=0;i<enfants.size();)
	{
	    if(enfants[i].voeux[0].voeu1==-1 && enfants[i].voeux[0].voeu2==-1 && enfants[i].voeux[0].voeu3==-1 &&
	       enfants[i].voeux[1].voeu1==-1 && enfants[i].voeux[1].voeu2==-1 && enfants[i].voeux[1].voeu3==-1 &&
	       enfants[i].voeux[2].voeu1==-1 && enfants[i].voeux[2].voeu2==-1 && enfants[i].voeux[2].voeu3==-1 &&
	       enfants[i].voeux[3].voeu1==-1 && enfants[i].voeux[3].voeu2==-1 && enfants[i].voeux[3].voeu3==-1)
	    {
 		    enfants.erase(enfants.begin()+i);
 		}
	    else
		{
	 	    i++;
	 	}
	}*/

    int nbEleveMax=enfants.size();

    for(int j=0;j<4;j++)
    {
        for(int i=0;i<enfants.size();i++)
        {
            enfants[i].affectation[j]=-1;
        }
    }

	/*Pour calculer le nombre de boucle:
		 p=1-q**n -> probabilité d'avoir au moins une fois un évenement élémentaire par rapport à un nombrede tirage
		 avec:
		 	  q=1-probabilité d'un évenement élémentaire, (r-1)/r avec r=nombre d'évenement élémentaire
		 	  n=nombre de tirage
		 	  p=probabilité d'obtenir au moins une fois un évenement élémentaire
         n=log(1-p)/log(q)
         soit:
		 	  n=log(0.1)/log((r-1)/r)
	     donc:
		 	  int nbBoucle=log(0.1)/log(((3**nbEleveMax)-1)/log(3**nbEleveMax))+1;

	*/
	register unsigned long long nbCycles=0;
	register float moyennePoints=10.0;
	inferieurA=1;
	unsigned long long nbBoucleFiable[9];
	bool changeTextFiable=false;
	for(int i=0;i<9;i++)
	{
	    nbBoucleFiable[i]=(unsigned long long)((long double)log(1.0-(i+1)/10.0)/log((pow(3,nbEleveMax)-1)/pow(3,nbEleveMax))+1);
	}
	FILE *pTempFile=NULL;
	char tempFileName[MAX_PATH];

  	GetTempPath(sizeof(tempFileName),tempFileName);
  	GetTempFileName(tempFileName,"temp",0,tempFileName);

	//pTempFile=tmpfile();
	pTempFile=fopen(tempFileName,"w+");
 	if(pTempFile==NULL)
 	{
    	SetLastError(ERROR_OPEN_FAILED);
    	traitementLance=false;
  	    return 1;
	}
	fpos_t pos;
	fgetpos(pTempFile,&pos);
	arretTraitement=false;


	while(!arretTraitement)
	{
 	    //initialisation
        for(int i=0;i<enfants.size();i++)
        {
		    enfants[i].point=0;
	    	enfants[i].denominateur=0;
		}
		for(int jour=0;jour<4;jour++)
		{
		    for(int i=0;i<enfants.size();i++)
		    {
			   enfants[i].affectation[jour]=-1;
			}
		}
		for(int i=0;i<activite.size();i++)
		{
		    activite[i].placesRestantes=activite[i].placesDispo;
    	    activite[i].demande=0;
		}

        Traitement(enfants, activite);

        //calcul de la moyenne
        float moyenne=0.0;
        int nbElevesPresents=0;
        for(int i=0;i<enfants.size();i++)
        {
		    if(enfants[i].denominateur!=0)
		    {
		        moyenne+=float(enfants[i].point*1.0/enfants[i].denominateur*1.0);
		        nbElevesPresents++;

			}
		}
		moyenne/=nbElevesPresents;
        //sauvegarde du résultat (si meilleur: moyenne < moyennePoints)
        if(moyenne<moyennePoints)
        {
	 	    moyennePoints=moyenne;
	 	    fclose (pTempFile);
	 	    pTempFile=NULL;
			pTempFile=fopen(tempFileName,"w+");
  			if(pTempFile==NULL)
		 	{
		  	    arretTraitement=true;
		    	traitementLance=false;
		    	SetLastError(ERROR_OPEN_FAILED);
		  	    return 1;
			}


	     	for(int i=0;i<enfants.size();i++)
        	{
 			    fprintf(pTempFile,"%d/%d\n",enfants[i].point,enfants[i].denominateur);
 			}
		    for(int jour=0;jour<4;jour++)
		    {
			    for(int i=0;i<enfants.size();i++)
			    {
	     	        fprintf(pTempFile,"%d\n",enfants[i].affectation[jour]);
				}
			}
			//affichage moyenne voeux
			if(hDlgTraitementEnCours!=NULL)
		 	{
	   		    string sMoyenneVoeux="Voeu attribué en moyenne: ";
	   		    sMoyenneVoeux+=to_string(moyennePoints*3);
	   		    //char smoyenne[10];
	   		    //sprintf(smoyenne,"%.4f",moyennePoints*3);
	   		    //sMoyenneVoeux+=smoyenne;
		 	    SetDlgItemText(hDlgTraitementEnCours,IDT_MOYENNEVOEUX,sMoyenneVoeux.c_str());
			}

	 	}
	 	//actualisation du nombre de cycles


	 	nbCycles++;


	 	if(hDlgTraitementEnCours!=NULL)
	 	{
            string scycles=to_string(nbCycles);
            //sprintf(scycles,"%llu",nbCycles);
            SetDlgItemText(hDlgTraitementEnCours,IDT_CYCLE,scycles.c_str());
		}
		/*string fiable="Fiabilité estimée: ";

		if(nbCycles>=nbBoucleFiable[0] && nbCycles<nbBoucleFiable[1] && inferieurA<2)
		{
  		    fiable+="entre 10% et 20%";
  		    inferieurA=2;
  		    changeTextFiable=true;
		}
		else if(nbCycles>=nbBoucleFiable[1] && nbCycles<nbBoucleFiable[2] && inferieurA<3)
		{
	 	    fiable+="entre 20% et 30%";
	 	    inferieurA=3;
	 	    changeTextFiable=true;
		}
		else if(nbCycles>=nbBoucleFiable[2] && nbCycles<nbBoucleFiable[3] && inferieurA<4)
		{
	 	    fiable+="entre 30% et 40%";
	 	    inferieurA=4;
	 	    changeTextFiable=true;
		}
		else if(nbCycles>=nbBoucleFiable[3] && nbCycles<nbBoucleFiable[4] && inferieurA<5)
		{
	 	    fiable+="entre 40% et 50%";
	 	    inferieurA=5;
	 	    changeTextFiable=true;
		}
		else if(nbCycles>=nbBoucleFiable[4] && nbCycles<nbBoucleFiable[5] && inferieurA<6)
		{
	 	    fiable+="entre 50% et 60%";
	 	    inferieurA=6;
	 	    changeTextFiable=true;
		}
		else if(nbCycles>=nbBoucleFiable[5] && nbCycles<nbBoucleFiable[6] && inferieurA<7)
		{
	 	    fiable+="entre 60% et 70%";
	 	    inferieurA=7;
	 	    changeTextFiable=true;
		}
		else if(nbCycles>=nbBoucleFiable[6] && nbCycles<nbBoucleFiable[7] && inferieurA<8)
		{
	 	    fiable+="entre 70% et 80%";
	 	    inferieurA=8;
	 	    changeTextFiable=true;
		}
		else if(nbCycles>=nbBoucleFiable[7] && nbCycles<nbBoucleFiable[8] && inferieurA<9)
		{
	 	    fiable+="entre 80% et 90%";
	 	    inferieurA=9;
	 	    changeTextFiable=true;
		}
		else if(nbCycles>=nbBoucleFiable[8] && inferieurA==9)
		{
	 	    fiable+="supérieure à 90%";
	 	    inferieurA=10;
	 	    changeTextFiable=true;
		}
		if(changeTextFiable==true)
		{
		    SetDlgItemText(hDlgTraitementEnCours,IDT_FIABILITE,fiable.c_str());
		    changeTextFiable=false;
		}*/
	}
	fflush (pTempFile);

   	fsetpos(pTempFile,&pos);
    for(int i=0;i<enfants.size();i++)
	{
	    int point,denominateur;
	    fscanf(pTempFile,"%d/%d\n",&point,&denominateur);
		enfants[i].point=point;
	 	enfants[i].denominateur=denominateur;
	}

	for(int Jour=0;Jour<4;Jour++)
    {
	    for(int i=0;i<enfants.size();i++)
	    {
	 		int affectation;
 	        fscanf(pTempFile,"%d\n",&affectation);
 			enfants[i].affectation[Jour]=affectation;
		}
	}

	enfantsAttributed=enfants;

	fclose (pTempFile);
	remove(tempFileName);
}
