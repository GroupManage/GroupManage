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

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include <openssl/sha.h>

std::string sha256(const std::string& str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string sha256(std::istream& stream)
{
    std::string str;
    {
        std::stringstream ss;
        ss<<stream.rdbuf();
        str=ss.str();
    }

    return sha256(str);
}

std::string FileSha256(std::string filePath)
{
    std::ifstream file(filePath.c_str(), std::ios::binary);
    if(!file)return "-1";
    return sha256(file);
}

bool AreSha256Equal(std::string const& str1, std::string const& str2)
{
    if(str1.size()!= str2.size())
    {
        return false;
    }
    else
    {
        return !sha256(str1).compare(sha256(str2));
    }
}

bool AreSha256Equal(std::istream& stream1, std::istream& stream2)
{
    //Taille du premier fichier
    stream1.seekg(0, stream1.end);
    unsigned long long length1 = stream1.tellg();
    stream1.seekg(0);

    //Taille du deuxième fichier
    stream2.seekg(0, stream2.end);
    unsigned long long length2 = stream2.tellg();
    stream2.seekg(0);

    if(length1!=length2)
    {
        return false;
    }
    else if(length1<=10000000)
    {
        return !sha256(stream1).compare(sha256(stream2));
    }
    else
    {
        bool areEqual=false;
        std::string str1;
        str1.reserve(10000000);
        std::string str2;
        str2.reserve(10000000);

        stream1.read(&str1[0], 10000000);
        stream2.read(&str2[0], 10000000);
        while((areEqual=AreSha256Equal(str1,str2)) && stream1.good())
        {
            stream1.read(&str1[0], 10000000);
            stream2.read(&str2[0], 10000000);
        }
        return areEqual;
    }
}

bool AreFilesSha256Equal(std::string filePath1, std::string filePath2)
{
    std::ifstream file1(filePath1.c_str(), std::ios::binary);
    if(!file1)
    {
        std::cerr<<"Erreur d'ouverture du premier fichier."<<std::endl;
        return false;
    }
    std::ifstream file2(filePath2.c_str(), std::ios::binary);
    if(!file2)
    {
        std::cerr<<"Erreur d'ouverture du deuxième fichier."<<std::endl;
        return false;
    }
    return AreSha256Equal(file1, file2);
}
