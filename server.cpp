#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#define SIZE 1024
using namespace std;

class Server{
private:
    struct sockaddr_in address;
    char* buffer;
    int option_value, server_fd, new_socket, addrlen;	
public:
    Server(){
        buffer = new char[SIZE];
        addrlen = sizeof(address);
        option_value = 1;
    }
    ~Server(){
		delete[] buffer;
        close(new_socket);
        shutdown(server_fd, SHUT_RDWR);
    }
    void createSocket(){
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            cout << "Socket creation failed" << '\n';
            exit(EXIT_FAILURE);
        }
    }
    void setSocketOptions(int port){
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option_value,sizeof(option_value))) {
            cout << "Error: Setting socket options failed" << '\n';
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
    }
    void bindSocket(){
        if (bind(server_fd, (struct sockaddr*)&address,sizeof(address)) < 0) {
            cout << "Bind failed" << '\n';
            exit(EXIT_FAILURE);
        }
    }
    void listenSocket(){
        if (listen(server_fd, 3) < 0) {
            cout << "Error: Socket listening failed" << '\n';
            exit(EXIT_FAILURE);
        }
    }
    void acceptSocket(){
		cout << "Waiting for client programm." << '\n'; 
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            cout << "Error: Socket accepting failed" << '\n';
            exit(EXIT_FAILURE);
        }
    }
    void recieveData(){
        int strLength, sum;
		cout << "Waiting for data." << '\n';
        while(true){
			if(recv(new_socket, buffer, SIZE, 0) <= 0){
				cout << "Error: couldn't read data from client" << '\n';
				exit(EXIT_FAILURE);
			}
            sum = atoi(buffer);
            cout << sum << '\n';
            strLength = strlen(buffer);
            if (strLength > 2 && sum % 32 == 0) cout << "Correct data" << '\n';
            else cout << "Wrong data" << '\n';
        }
    }
};

int main()
{
    Server server;
    server.createSocket();
    server.setSocketOptions(8080);
    server.bindSocket();
    server.listenSocket();
    server.acceptSocket();
    server.recieveData();
    return 0;
}
