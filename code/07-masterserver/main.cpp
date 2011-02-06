#include <iostream>
#include <string>
#include <list>
#include <SFML/Network.hpp>

static void GetError(sf::Http::Response::Status status, std::string& error)
{
    switch(status)
    {
    case sf::Http::Response::BadRequest:            error += "400 Bad Request"; break;
    case sf::Http::Response::Unauthorized:          error += "401 Unauthorized"; break;
    case sf::Http::Response::Forbidden:             error += "403 Forbidden"; break;
    case sf::Http::Response::NotFound:              error += "404 File Not Found"; break;
    case sf::Http::Response::InternalServerError:   error += "500 Internal Server Error"; break;
    case sf::Http::Response::NotImplemented:        error += "501 Not Implemented"; break;
    case sf::Http::Response::BadGateway:            error += "502 Bad Gateway"; break;
    case sf::Http::Response::ServiceNotAvailable:   error += "503 Service not Available"; break;
    case sf::Http::Response::ConnectionFailed:      error += "Connection Failed"; break;
    default:                                        error += "Unexpected error "; break; //TODO: Response.GetStatus();
    }
}

static const float TIMEOUT = 10.0f;

const bool RetrieveServers(const std::string& protocol, std::list<std::string>& servers, std::string& error)
{
    sf::Http Http;
    Http.SetHost("www.mrwonko.de");

    sf::Http::Request Request;
    Request.SetMethod(sf::Http::Request::Get);
    Request.SetURI("/masterserver/list.php?protocol=" + protocol);

    sf::Http::Response Response = Http.SendRequest(Request, TIMEOUT);
    if(Response.GetStatus() == sf::Http::Response::Ok)
    {
        if(Response.GetBody().substr(0, 6) == "Error:")
        {
            error = "Error in request:" + Response.GetBody().substr(6);
            return false;
        }
        std::string ips = Response.GetBody();
        std::string::size_type pos;
        while((pos = ips.find("\n")) != std::string::npos)
        {
            servers.push_back(ips.substr(0, pos));
            ips.erase(0, pos+1);
        }
        return true;
    }
    else
    {
        error = "Could not retrieve server list: ";
        GetError(Response.GetStatus(), error);
        return false;
    }
}

const bool SendHeartbeat(const std::string& protocol, const std::string& port, std::string& error)
{
    sf::Http Http;
    Http.SetHost("www.mrwonko.de");

    sf::Http::Request Request;
    Request.SetMethod(sf::Http::Request::Get);
    Request.SetURI("/masterserver/heartbeat.php?protocol=" + protocol + "&port=" + port);

    sf::Http::Response Response = Http.SendRequest(Request, TIMEOUT);
    if(Response.GetStatus() == sf::Http::Response::Ok)
    {
        if(Response.GetBody().substr(0, 6) == "Error:")
        {
            error = "Error in request:" + Response.GetBody().substr(6);
            return false;
        }
        else if(Response.GetBody() != "Ok.")
        {
            error = "Unexpected body: " + Response.GetBody();
            return false;
        }
        return true;
    }
    else
    {
        error = "Could not send heartbeat: ";
        GetError(Response.GetStatus(), error);
        return false;
    }
}

int main()
{
    std::list<std::string> servers;
    std::string error;
    if(!SendHeartbeat("1", "666", error))
    {
        std::cout<<error<<std::endl;
        return 0;
    }
    if(!RetrieveServers("1", servers, error))
    {
        std::cout<<error<<std::endl;
        return 0;
    }
    std::cout<<"Servers:\n";
    for(std::list<std::string>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        std::cout<<*it<<"\n";
    }
    std::cout<<std::endl;
    return 0;
}
