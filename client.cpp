#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#define SIZE 1024
using namespace std;

class Client{
private:
    struct sockaddr_in serv_addr;
    int client_fd, sum;
    mutex m;
    condition_variable cond_var;
    char* buffer;
    bool flag;
    string str;
public:
    Client(){
        buffer = new char[SIZE];
        flag = false;
    }
    ~Client(){
        close(client_fd);
        delete[] buffer;
    }
    bool checkNumber(string &str){
        return !str.empty() && str.find_first_not_of("0123456789") == std::string::npos && str.length() < 64;
    }
    void replaceNum(string &str){
        int i = 0;
        while (i < str.length()){
            if (int(str[i] - '0') % 2 == 0){
                str.replace(i, 1, "KB");
                i += 2;
            }
            else i++;
        }    
    }
    void digitSum(string str){
        sum = 0;
        for (char c : str){
            if(isdigit(c)) sum += int(c - '0');
        }
    }
    void createSocket(int port){
        if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            cout << "Socket creation error" << '\n';
            exit(EXIT_FAILURE);
        }
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
    }
    void convertAddress(const char *c){
        if (inet_pton(AF_INET, c, &serv_addr.sin_addr) <= 0) {
            cout << "Invalid address or Address not supported" << '\n';
            exit(EXIT_FAILURE);
        }
    }
    void connectToServer(){
        if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            cout << "Connection failed" << '\n';
            exit(EXIT_FAILURE);
        }
		else cout << "Connection to server completed" << '\n';
    }
    void setData(){
        while(true){
            this_thread::sleep_for(chrono::milliseconds(200));
            unique_lock<mutex> lock(m);
            getline(cin, str);
            if (!checkNumber(str)){
                cout << "Wrong data. Try again." << '\n';
                continue;
            }
            sort(str.begin(), str.end(), [this](char &i, char &j){return i > j;});
            replaceNum(str);
            flag = true;
            cond_var.notify_one();
        }
    }
    void sendData(){
        while(true){
            unique_lock<mutex> lock(m);
            cond_var.wait(lock, [this]{return flag;});
            cout << str << '\n';
            digitSum(str);
            str = to_string(sum);
            strcpy(buffer, str.c_str());
            if(send(client_fd, buffer, 1024, 0) < 0) cout << "Error" << '\n';
            else cout << "Transfer completed" << '\n';
            flag = false;
        }
    }

};
int main(){
    Client client;
    thread th1(&Client::setData, &client);
    thread th2(&Client::sendData, &client);
    client.createSocket(8080);
    client.convertAddress("127.0.0.1");
    client.connectToServer();
    th1.join();
    th2.join();
    return 0;
}
