#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>


int main() {

  struct sockaddr_in server_address;

  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
        perror("socket creation error");
        exit(errno);
  }


  server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(8888);

  int connected = connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    if (connected == -1) {
        perror("connection failed");
        exit(errno);
    }
    std::cout << "type any message or type 'exit' if you want to disconect from the server\n";
    while (true) { 
        std::string msg;
        std::getline(std::cin,msg);
        if (msg == "exit")
            break;

        if (send(client_socket, msg.c_str(), msg.size(), 0) == -1 ) {
            perror("Send failed");
            break; 
        }
    }  
  close(client_socket);
  return 0;
}



