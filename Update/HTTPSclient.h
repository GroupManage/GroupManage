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

#ifndef DEF_HTTPS_CLIENT
#define DEF_HTTPS_CLIENT

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <algorithm>
//Boost
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#ifdef _WIN32
#include <windows.h>
#endif
#include <boost/filesystem.hpp>


template<typename T> std::string to_string(T const &var)
{
    std::ostringstream oss;
    oss<<var;
    return oss.str();
}

template<typename T> T string_to(std::string const &var)
{
    T retour;
    std::istringstream oss(var);
    oss>>retour;
    return retour;
}

class HTTPSclient
{
    public:
        HTTPSclient(boost::asio::io_service& io_service, std::string& host);
        HTTPSclient(boost::asio::io_service& io_service, std::string host);
        //HTTPSclient(boost::asio::io_service& io_service, boost::asio::ssl::context& ctx, std::string& host);

        ~HTTPSclient();

        bool verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx);

        size_t SendGetFileRequest(std::string path, std::string connection="keep-alive");
        size_t RecvFile(std::string& fileContent);

        size_t SaveFile(std::string path, std::string fileName, std::string connection="keep-alive");

    protected:
        size_t completion_condition(const boost::system::error_code& error, std::size_t bytes_transferred);
        void flush_socket();
    private:
        boost::asio::ssl::context m_ctx;
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket> m_socket;
        boost::asio::ip::tcp::resolver m_resolver;
        boost::asio::ip::tcp::resolver::query m_query;
        boost::asio::ip::tcp::resolver::iterator m_iterator;
        boost::system::error_code m_error;

        std::string m_host;
        std::string m_path;
        const char* m_cert="-----BEGIN CERTIFICATE-----\n\
MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n\
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n\
d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n\
ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n\
MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n\
LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n\
RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n\
+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n\
PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n\
xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n\
Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n\
hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n\
EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n\
MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n\
FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n\
nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n\
eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n\
hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n\
Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n\
vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n\
+OkuE6N36B9K\n\
-----END CERTIFICATE-----";

        LONG64 m_Length;
};


#endif // DEF_HTTPS_CLIENT
