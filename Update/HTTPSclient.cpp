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

#include "HTTPSclient.h"

using boost::asio::ip::tcp;


//contructeur et destructeur
HTTPSclient::HTTPSclient(boost::asio::io_service& io_service, std::string& host)
		:m_ctx(boost::asio::ssl::context::sslv23), m_socket(io_service, m_ctx), m_resolver(io_service), m_query(host.c_str(), "https"), m_host(host)
{
    try
    {
        std::cout<<"Création de la demande..."<<std::endl;
        m_iterator = m_resolver.resolve(m_query);

        m_ctx.set_default_verify_paths();
        m_ctx.add_certificate_authority(boost::asio::const_buffer(m_cert, strlen(m_cert)));

        //m_socket=boost::asio::ssl::stream<boost::asio::ip::tcp::socket>(io_service, ctx);

        std::cout<<"Vérification du certificat de sécurité..."<<std::endl;
        m_socket.set_verify_mode(boost::asio::ssl::verify_fail_if_no_peer_cert | boost::asio::ssl::verify_peer);
        //m_socket.set_verify_mode(boost::asio::ssl::verify_none);
        m_socket.set_verify_callback(boost::bind(&HTTPSclient::verify_certificate, this, _1, _2));//make_verbose_verification(boost::asio::ssl::rfc2818_verification(host)));//
        std::cout<<"Connection..."<<std::endl;
        boost::asio::connect(m_socket.lowest_layer(), m_iterator, m_error);//, boost::bind(&HTTPSclient::handle_connect, this, boost::asio::placeholders::error));
        if (m_error)
        {
            std::cerr << "Connect failed: " << m_error.message() << std::endl;
            throw boost::system::system_error(m_error);
        }
        std::cout<<"Liaison..."<<std::endl;
        m_socket.handshake(boost::asio::ssl::stream_base::client, m_error);//, boost::bind(&HTTPSclient::handle_handshake, this, boost::asio::placeholders::error));
        if (m_error)
        {
            std::cerr << "Handshake failed: " << m_error.message() << std::endl;
            throw boost::system::system_error(m_error);
        }
        boost::asio::socket_base::keep_alive option(true);
        m_socket.lowest_layer().set_option(option);
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception constructor: " << e.what() << std::endl;
        throw;
    }
}

HTTPSclient::HTTPSclient(boost::asio::io_service& io_service, std::string host)
		:m_ctx(boost::asio::ssl::context::sslv23), m_socket(io_service, m_ctx), m_resolver(io_service), m_query(host.c_str(), "https"), m_host(host)
{
    try
    {
        std::cout<<"Création de la demande..."<<std::endl;
        m_iterator = m_resolver.resolve(m_query);

        m_ctx.set_default_verify_paths();
        //m_ctx.load_verify_file("DigiCertHighAssuranceEVRootCA.crt");
        m_ctx.add_certificate_authority(boost::asio::const_buffer(m_cert, strlen(m_cert)));

        //m_socket=boost::asio::ssl::stream<boost::asio::ip::tcp::socket>(io_service, ctx);

        std::cout<<"Vérification du certificat de sécurité..."<<std::endl;
        m_socket.set_verify_mode(boost::asio::ssl::verify_fail_if_no_peer_cert | boost::asio::ssl::verify_peer);
        //m_socket.set_verify_mode(boost::asio::ssl::verify_none);
        m_socket.set_verify_callback(boost::bind(&HTTPSclient::verify_certificate, this, _1, _2));//make_verbose_verification(boost::asio::ssl::rfc2818_verification(host)));//

        std::cout<<"Connection..."<<std::endl;
        boost::asio::connect(m_socket.lowest_layer(), m_iterator, m_error);//, boost::bind(&HTTPSclient::handle_connect, this, boost::asio::placeholders::error));
        if (m_error)
        {
            std::cerr << "Connect failed: " << m_error.message() << std::endl;
            throw boost::system::system_error(m_error);
        }

        std::cout<<"Liaison..."<<std::endl;
        m_socket.handshake(boost::asio::ssl::stream_base::client, m_error);//, boost::bind(&HTTPSclient::handle_handshake, this, boost::asio::placeholders::error));
        if (m_error)
        {
            std::cerr << "Handshake failed: " << m_error.message() << std::endl;
            throw boost::system::system_error(m_error);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception constructor: " << e.what() << std::endl;
        throw;
    }
}

/*HTTPSclient::HTTPSclient(boost::asio::io_service& io_service, boost::asio::ssl::context& ctx, std::string& host)
		:m_ctx(ctx), m_socket(io_service, ctx), m_resolver(io_service), m_query(host.c_str(), "https"), m_host(host)
{
    try
    {
        std::cout<<"Création de la demande..."<<std::endl;
        m_iterator = m_resolver.resolve(m_query);

        ctx.set_default_verify_paths();
        //m_socket=boost::asio::ssl::stream<boost::asio::ip::tcp::socket>(io_service, ctx);

        m_socket.set_verify_mode(boost::asio::ssl::verify_peer);
        m_socket.set_verify_mode(boost::asio::ssl::verify_none);
        m_socket.set_verify_callback(boost::bind(&HTTPSclient::verify_certificate, this, _1, _2));//make_verbose_verification(boost::asio::ssl::rfc2818_verification(host)));//
        boost::asio::connect(m_socket.lowest_layer(), m_iterator, boost::bind(&HTTPSclient::handle_connect, this, boost::asio::placeholders::error));
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }
}*/


HTTPSclient::~HTTPSclient()
{
    m_socket.lowest_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    m_socket.lowest_layer().close();
}

//fonctions membres

bool HTTPSclient::verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx)
{
    char subject_name[256];
    X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
    std::cout<<m_host<<std::endl;
    bool verified = boost::asio::ssl::rfc2818_verification(m_host)(preverified, ctx);
    std::boolalpha(std::cout);
    std::cout << "Verifying: " << subject_name << std::endl << "Verified: " << verified << std::endl;

    return verified;
}

size_t HTTPSclient::SendGetFileRequest(std::string path, std::string connection)
{

    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "GET " << path << " HTTP/1.1\r\n";
    request_stream << "Host: " << m_host << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: "<<connection<<"\r\n\r\n";//keep-alive

    std::cout<<"Envoi de la requette: "<<path<<std::endl;
    // Send the request.
    return boost::asio::write(m_socket, request);
}

size_t HTTPSclient::completion_condition(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(bytes_transferred==m_Length)
    {
        return 0;
    }
    else
    {
        return m_Length-bytes_transferred;
    }
}

void HTTPSclient::flush_socket()
{
    boost::asio::streambuf response;
    boost::system::error_code error;
    boost::asio::read(m_socket, response, boost::asio::transfer_at_least(1), error);
    /*//m_socket.lowest_layer()
    boost::asio::streambuf b;
    boost::asio::streambuf::mutable_buffers_type bufs = b.prepare(BUFFER_SIZE);
    std::size_t bytes = m_socket.receive(bufs); // !!! This will block until some data becomes available
    b.commit(bytes);
    boost::asio::socket_base::bytes_readable command(true);
    m_socket.io_control(command);

    while(command.get())
    {
        bufs = b.prepare(BUFFER_SIZE);
        bytes = m_socket.receive(bufs);
        b.commit(bytes);
        m_socket.io_control(command); // reset for bytes pending
    }*/
    return;
}

size_t HTTPSclient::RecvFile(std::string& fileContent)
{
    // Read the response status line. The response streambuf will automatically
    // grow to accommodate the entire line. The growth may be limited by passing
    // a maximum size to the streambuf constructor.
    boost::asio::streambuf response;
    boost::asio::read_until(m_socket, response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
        std::cerr << "Invalid response\n";
        flush_socket();
        return 1;
    }
    if (status_code != 200)
    {
        std::cerr << "Response returned with status code " << status_code << std::endl;
        flush_socket();
        return status_code;
    }

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(m_socket, response, "\r\n\r\n");

    // Process the response headers.
    std::string header;
    LONG64 m_Length=0;
    bool chunked=false;
    while (std::getline(response_stream, header) && header != "\r")
    {
        //std::cout << header << std::endl;
        if(header.compare(0,15,"Content-Length:")==0)
        {
            m_Length=string_to<LONG64>(header.substr(15));
        }
        else if(header.compare(0,26,"Transfer-Encoding: chunked")==0)
        {
            chunked=true;
            m_Length=4;
        }
    }
    //std::cout << "\n";
    if(m_Length>100000000)
    {
        boost::system::error_code error;
        while (boost::asio::read(m_socket, response,boost::asio::transfer_at_least(1), error))continue;
        return boost::asio::error::message_size;
    }

    // Write whatever content we already have to output.
    fileContent="";
    bool chaineLue=false;
    if (response.size() > 0)
    {
        //std::cout << &response;
        if(!chunked)
        {
            std::ostringstream  ss;
            ss << &response;
            fileContent = ss.str();
            m_Length-=fileContent.length();
        }
        else
        {
            std::stringstream  ss;
            ss << &response;

            std::string str;
            while(std::getline(ss, str))
            {
                std::stringstream sshex;
                //std::getline(ss, str);

                sshex << std::hex << str;
                sshex >> m_Length;
                chaineLue=false;
                if(std::getline(ss,str))
                {
                    fileContent+=str;
                    chaineLue=true;
                    m_Length=4;
                }
                if(fileContent.size()>100000000)//100 000 000
                {
                    boost::system::error_code error;
                    while (boost::asio::read(m_socket, response,boost::asio::transfer_at_least(1), error))continue;
                    return boost::asio::error::message_size;
                }
            }
            //m_Length-=fileContent.length();
        }
        //std::cout<<(fileContent.find("\r\n\r\n")==std::string::npos)<<std::endl;
    }

    // Read until EOF, writing data to output as we go.
    boost::system::error_code error=boost::asio::error::eof;
    if(!chunked)
    {
        size_t envoi;

        while (m_Length>0 && (envoi=boost::asio::read(m_socket, response, boost::asio::transfer_at_least(1), error)))//boost::asio::transfer_exactly(m_Length)
        {
            std::ostringstream ss;
            ss << &response;
            fileContent += ss.str();
            m_Length-=envoi;
        }
    }
    else
    {

        while (m_Length>0 && boost::asio::read(m_socket, response, boost::asio::transfer_exactly(m_Length), error))//boost::bind(&HTTPSclient::completion_condition, this, _1, _2)
        {
            if(chaineLue)
            {
                std::stringstream ss;
                std::string str;
                ss << &response;
                std::getline(ss,str);
                std::stringstream sshex;
                sshex << std::hex << str;
                sshex >> m_Length;
                chaineLue=false;
            }
            else
            {
                std::ostringstream ss;
                ss << &response;
                fileContent+=ss.str();
                chaineLue=true;
                m_Length=4;
                if(fileContent.size()>100000000)//100 000 000
                {
                    boost::system::error_code error;
                    while (boost::asio::read(m_socket, response,boost::asio::transfer_at_least(1), error))continue;

                    return boost::asio::error::message_size;
                }
            }
        }
    }

    if (error && error != boost::asio::error::eof)
    {
        std::cerr<<error<<std::endl;
        throw boost::system::system_error(error);
    }
    return 0;
}

size_t HTTPSclient::SaveFile(std::string path, std::string fileName, std::string connection)
{
    ///Save a file from a server to computer
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "GET " << path << fileName << " HTTP/1.1\r\n";
    request_stream << "Host: " << m_host << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: "<<connection<<"\r\n\r\n";

    std::cout<<"Envoi de la requette : "<<fileName<<std::endl;
    // Send the request.
    boost::asio::write(m_socket, request);

    // Read the response status line. The response streambuf will automatically
    // grow to accommodate the entire line. The growth may be limited by passing
    // a maximum size to the streambuf constructor.
    std::ofstream Fichier;
    boost::asio::streambuf response;
    boost::asio::read_until(m_socket, response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    //std::cout<<&response<<std::endl;
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
        std::cerr << "Invalid response\n";
        flush_socket();
        return 1;
    }
    if (status_code != 200)
    {
        std::cerr << "Response returned with status code " << status_code <<":" << status_message << std::endl;;

        //On vide la socket
        flush_socket();
        return status_code;
    }

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(m_socket, response, "\r\n\r\n");

    // Process the response headers.
    std::string header;
    LONG64 m_Length=0;
    bool chunked=false;
    while (std::getline(response_stream, header) && header != "\r")
    {
        //std::cout << header << std::endl;
        if(header.compare(0,15,"Content-Length:")==0)
        {
            m_Length=string_to<LONG64>(header.substr(15));
        }
        else if(header.compare(0,26,"Transfer-Encoding: chunked")==0)
        {
            chunked=true;
            m_Length=4;
        }
    }

    //Création du chemin de destination
    std::string cheminFichier="Update/";
    cheminFichier+=fileName;
    std::replace(cheminFichier.begin(),cheminFichier.end(),'/','\\');

    //Vérification et création des dossiers de destination
    #ifdef _WIN32
    {
        size_t pos=0;
        pos=cheminFichier.find('\\', pos+1);
        while(pos!=std::string::npos)
        {
            CreateDirectory(cheminFichier.substr(0,cheminFichier.find('\\', pos)).c_str(),NULL);
            pos=cheminFichier.find('\\', pos+1);
        }
    }

    #else
        boost::filesystem::path dir(cheminFichier);//cheminFichier);
        boost::filesystem::create_directory(dir.parent_path());
    #endif

    //std::cout<<cheminFichier.substr(0,cheminFichier.rfind('\\'))<<std::endl;

    //Ouverture du fichier en écriture
    Fichier.open(cheminFichier.c_str(), std::ofstream::out | std::ofstream::binary);

    // Write whatever content we already have to output.
    bool chaineLue=false;
    if (response.size() > 0)
    {
        if(!chunked)
        {
            Fichier << &response;
            m_Length-=Fichier.tellp();
        }
        else
        {
            std::stringstream  ss;
            ss << &response;

            std::string str;
            while(std::getline(ss, str))
            {
                std::stringstream sshex;

                sshex << std::hex << str;
                sshex >> m_Length;
                chaineLue=false;
                if(std::getline(ss,str))
                {
                    Fichier << str;
                    chaineLue=true;
                    m_Length=4;
                }
            }
        }
    }

    // Read until EOF, writing data to output as we go.
    boost::system::error_code error;
    if(!chunked)
    {
        size_t envoi;
        while (m_Length>0 && (envoi=boost::asio::read(m_socket, response, boost::asio::transfer_at_least(1), error)))//boost::asio::transfer_exactly(m_Length)
        {
            Fichier << &response;
            m_Length-=envoi;
        }
    }
    else
    {

        while (m_Length>0 && boost::asio::read(m_socket, response,boost::asio::transfer_exactly(m_Length), error))//boost::bind(&HTTPSclient::completion_condition, this, _1, _2)
        {
            if(chaineLue)
            {
                std::stringstream ss;
                std::string str;
                ss << &response;
                std::getline(ss,str);
                std::stringstream sshex;
                sshex << std::hex << str;
                sshex >> m_Length;
                chaineLue=false;
            }
            else
            {
                Fichier << &response;
                chaineLue=true;
                m_Length=4;
            }
        }
    }
    if (error && error != boost::asio::error::eof)
        throw boost::system::system_error(error);
    return 0;
}
