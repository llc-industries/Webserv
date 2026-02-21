/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atazzit <atazzit@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 15:34:07 by atazzit           #+#    #+#             */
/*   Updated: 2026/02/21 01:29:01 by atazzit          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port) : _epoll_fd(-1), _port(port) {
  (void)_port;
}

Server::~Server() {
  // Fermeture propre de tous les sockets serveurs et de l'instance epoll
  for (size_t i = 0; i < _server_fds.size(); i++)
    close(_server_fds[i]);
  if (_epoll_fd != -1)
    close(_epoll_fd);
}

void Server::setupServer(std::vector<int> ports) {
  // 1. Création de l'instance epoll 
  // epoll_create1(0) : Crée le moniteur d'événements.
  _epoll_fd = epoll_create1(0);
  if (_epoll_fd == -1) {
    throw SetupException("epoll_create1 failed");
  }
  // Boucle pour créer un socket d'écoute pour chaque port défini dans la config
  for (size_t i = 0; i < ports.size(); i++){
    // 2. Création du Socket
    // AF_INET : Protocole IPv4
    // SOCK_STREAM : Utilisation de TCP (Fiable, connecté, flux de données)
    // 0 : Le système choisit le protocole approprié (IPPROTO_TCP)
    int server_fd = socket(AF_INET, SOCK_STREAM, 0); //precise la norme de la famille d'adresse en ipv4
    if (server_fd == -1) {
      throw SetupException("Socket creation failed");
    }
    int opt = 1;
    // 3. Configuration des options du socket (setsockopt)
    // SOL_SOCKET : Niveau de l'option (niveau du socket lui-même)
    // SO_REUSEADDR : Permet de réutiliser le port immédiatement après avoir fermé le serveur.
    // Sans ça, le système bloque le port en état "TIME_WAIT" pendant ~1 minute après un crash.
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
      throw SetupException("socket option failed (re-use address)");
    }
    // 4. Préparation de la structure d'adresse (IP + Port)
    sockaddr_in address;
    address.sin_family = AF_INET;
    // htons (Host TO Network Short) : Convertit le port (entier) au format réseau (Big Endian)
    address.sin_port = htons(ports[i]);
    // INADDR_ANY : Le serveur écoute sur toutes les interfaces réseau (0.0.0.0)
    address.sin_addr.s_addr = INADDR_ANY;
    // 5. Attacher le socket au port et à l'IP (Bind)
    if (bind(server_fd, (sockaddr *)&address, sizeof(address)) < 0) {
      close(server_fd);
      throw SetupException("bind faailed");
    }
    // 6. Mettre le socket en mode "Écoute" (Listen)
    // SOMAXCONN : Constante système définissant la taille maximale de la file d'attente 
    // des connexions entrantes (celles qui attendent d'être acceptées).
    if (listen(server_fd, SOMAXCONN) < 0) {
      close(server_fd);
      throw SetupException("listen failed");
    }
    // 7. Rendre le socket NON-BLOQUANT (Crucial pour epoll)
    // fcntl (File Control) : Modifie les propriétés du file descriptor.
    // O_NONBLOCK : Fait en sorte que les fonctions comme accept() ou recv() 
    // n'endorment pas le programme si aucune donnée n'est disponible. Elles retourneront une erreur (EAGAIN).
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
    // Ajouter le socket serveur à epoll pour écouter les nouvelles connexions (EPOLLIN)
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    // EPOLL_CTL_ADD : Ajoute ce FD à la liste de surveillance d'epoll
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
      close(server_fd);
      throw SetupException("epoll control failed");
    }
    // On sauvegarde le socket serveur pour pouvoir le fermer plus tard
    _server_fds.push_back(server_fd);
  }
}

void Server::run() {
    std::cout << "[SERVER] Starting server..." << std::endl;

    while (true) {
      // Bloque jusqu'à ce qu'il se passe quelque chose sur l'un des sockets surveillés
        int nfds = epoll_wait(_epoll_fd, _events, MAX_EVENTS, -1);
        
        if (nfds == -1) {
            std::cerr << "[ERROR] epoll_wait failed" << std::endl;
            break;
        }
        // On parcourt tous les événements détectés par epoll
        for (int i = 0; i < nfds; i++) {
            int fd = _events[i].data.fd;
            bool is_server = false;//si c'est un serveur (socket d'écoute)
            // Vérifie si l'événement concerne l'un de nos sockets serveurs (nouvelle connexion)
            for (size_t j = 0; j < _server_fds.size(); j++) {
                if (_server_fds[j] == fd) {
                    is_server = true;
                    break;
                }
            }
            if (is_server) {
                acceptConnection(fd);// nouveau client
            } else if (_events[i].events & EPOLLIN) {
                handleClient(fd); //client existant
            } else if (_events[i].events & EPOLLOUT) {
                sendResponse(fd); //fin de lecture et env de reponse
            }
        }
    }
}

void Server::acceptConnection(int server_fd) {
    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int client_fd = accept(server_fd, (sockaddr *)&client_addr, &client_addr_len);
    
    if (client_fd == -1) {
        std::cerr << "[ERROR] accept failed" << std::endl;
        return;
    }
    // Rendre le socket du nouveau client non-bloquant
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    epoll_event ev;
    // Demander à epoll de nous prévenir quand ce client nous enverra des données (EPOLLIN)
    ev.events = EPOLLIN;
    ev.data.fd = client_fd;
    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
    // Initialiser les données (le parser de requête) pour ce client
   _clients[client_fd] = ClientData();

    std::cout << "[CONNECTION] Client connected: " << inet_ntoa(client_addr.sin_addr) 
              << ":" << ntohs(client_addr.sin_port) << std::endl;
}

void Server::handleClient(int client_fd)
{
  char buffer[4096];
  ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
  if (bytes <= 0)
  {
    if (bytes < 0) std::cerr << "[ERROR] recv failed \n";
    closeClient(client_fd);
    return;
  }
  // On donne les octets reçus à notre parser orienté objet.
  // Il va gérer lui-même l'accumulation et le découpage.
    _clients[client_fd].request.swallow(buffer, bytes);
     // La requête sait si elle a reçu tous ses headers et tout son body (Content-Length)
      if (_clients[client_fd].request.isComplete()){
        // La requête est complète, on demande à epoll de nous prévenir
        // dès qu'on peut écrire la réponse (passage en mode EPOLLOUT)
        epoll_event ev;
        ev.events = EPOLLOUT;
        ev.data.fd = client_fd;
        epoll_ctl(_epoll_fd, EPOLL_CTL_MOD ,client_fd, &ev);
      } 
}

void Server::closeClient(int client_fd)
{
  epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
  close(client_fd);
  _clients.erase(client_fd); //clean 
  std::cout << "[INFO] Connection closed for fd:" << client_fd << "\n";
}

void Server::sendResponse(int client_fd) {
  ClientData &client = _clients[client_fd];
  std::string method = client.request.getMethod();

  if (method == "GET") {
      handleGet(client_fd);
  } else if (method == "POST") {
      handlePost(client_fd);
  } else if (method == "DELETE") {
      handleDelete(client_fd);
  } else {
      // Si la méthode n'est pas reconnue
      HttpResponse res;
      res.setStatusCode(405);
      res.setHeader("Content-Type", "text/html");
      res.setBody("<html><body><h1>405 Method Not Allowed</h1></body></html>");
      std::string full_res = res.toString();
      send(client_fd, full_res.c_str(), full_res.size(), 0);
      closeClient(client_fd);
  }
}

void Server::handleGet(int client_fd) {
  ClientData &client = _clients[client_fd];
  //TODO remplace "./www" par la variable ex:_config.getRoot()
  std::string root = "./www";
  std::string path = client.request.getPath();
  std::string full_path = root + path;
  // Si on demande la racine, on sert l'index (TODO: lire l'index depuis la config)
  if (path == "/")
    full_path = root + "/index.html";
  
  std::ifstream file(full_path.c_str(), std::ios::binary);
  HttpResponse res;
  
  if (!file.is_open()) {
    res.setStatusCode(404);
    res.setHeader("Content-Type", "text/html");
    res.setBody("<html><body><h1>404 Not Found</h1></body></html>");
  } else {
    std::ostringstream file_content;
    file_content << file.rdbuf(); //lit tout le fichier
  
    res.setStatusCode(200);
    res.autoDetectContentType(full_path); // Trouve le bon type (ex: image/png, text/css)
    res.setBody(file_content.str());
  }
  
  std::string full_res = res.toString();
  send(client_fd, full_res.c_str(), full_res.size(), 0);
  closeClient(client_fd);
}

void Server::handlePost(int client_fd) {
  ClientData &client = _clients[client_fd];
 std::string body_content = client.request.getBody();
  //TODO: Nom dynamique ou extraction depuis multipart/form-data
  std::string save_path = "./www/uploaded_file.txt"; //HARDCODER
  std::ofstream outfile(save_path.c_str(), std::ios::binary);
  HttpResponse res;
  
  if (outfile.is_open()) {
    outfile.write(body_content.c_str(), body_content.size());
    outfile.close();
    
    res.setStatusCode(201);
    res.setHeader("Content-Type", "text/html");
    res.setBody("<html><body><h1>Upload reussi ! Fichier sauvegarde.</h1></body></html>");
  } else {
    res.setStatusCode(500);
    res.setHeader("Content-Type", "text/html");
    res.setBody("<html><body><h1>500 Internal Server Error</h1></body></html>");
  }
  
  std::string full_res = res.toString();
  send(client_fd, full_res.c_str(), full_res.size(), 0);
  closeClient(client_fd);
}


void Server::handleDelete(int client_fd) {
ClientData &client = _clients[client_fd];
  std::string root = "./www";
  std::string full_path = root + client.request.getPath();
  HttpResponse res;

  if (std::remove(full_path.c_str()) == 0) {
      res.setStatusCode(200);
      res.setHeader("Content-Type", "text/html");
      res.setBody("<html><body><h1>200 OK : Fichier supprime avec succes !</h1></body></html>");
  } else {
      res.setStatusCode(404);
      res.setHeader("Content-Type", "text/html");
      res.setBody("<html><body><h1>404 Not Found (ou droits insuffisants)</h1></body></html>");
  }
  
  std::string full_res = res.toString();
  send(client_fd, full_res.c_str(), full_res.size(), 0);
  closeClient(client_fd);
}
// TODO
// std::string Server::getContentType(const std::string& path) {
//   size_t dotPos = path.find_last_of(".");
//   if (dotPos == std::string::npos) return "text/plain";

//   std::string ext = path.substr(dotPos);
  
//   if (ext == ".html" || ext == ".htm") return "text/html";
//   if (ext == ".css") return "text/css";
//   if (ext == ".js") return "application/javascript";
//   if (ext == ".png") return "image/png";
//   if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
//   if (ext == ".gif") return "image/gif";
//   if (ext == ".ico") return "image/x-icon";
//   if (ext == ".pdf") return "application/pdf";
//   if (ext == ".txt") return "text/plain";

//   return "application/octet-stream";
// }