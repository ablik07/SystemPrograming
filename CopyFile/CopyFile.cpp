#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

const int PORT = 27015;
const int BUFFER_SIZE = 8192;

class ChatClient
{
private:
    SOCKET sock;
    string name;
    HANDLE recvThread;
    bool running;

    void SendCommand(const string& cmd)
    {
        send(sock, cmd.c_str(), (int)cmd.size() + 1, 0);
    }

    void ReceiveFile(const string& header)
    {
        // FILE:size:filename
        size_t p1 = header.find(':', 5);
        long size = stol(header.substr(5, p1 - 5));
        string filename = header.substr(p1 + 1);

        char* buffer = new char[size];
        int received = 0;
        while (received < size)
        {
            int r = recv(sock, buffer + received, size - received, 0);
            if (r <= 0) break;
            received += r;
        }

        ofstream file(filename, ios::binary);
        file.write(buffer, size);
        file.close();
        delete[] buffer;

        char ack[10];
        recv(sock, ack, 10, 0);
        cout << "File received: " << filename << endl;
    }

    void UploadFile(const string& filename)
    {
        ifstream file(filename, ios::binary);
        if (!file.is_open())
        {
            cout << "Cannot open file" << endl;
            return;
        }

        file.seekg(0, ios::end);
        long size = (long)file.tellg();
        file.seekg(0, ios::beg);

        char* buffer = new char[size];
        file.read(buffer, size);
        file.close();

        string header = "UPLOAD:" + to_string(size) + ":" + filename;
        SendCommand(header);
        Sleep(100);
        send(sock, buffer, size, 0);
        delete[] buffer;

        char response[20];
        recv(sock, response, 20, 0);
        cout << "Upload: " << response << endl;
    }

    static DWORD WINAPI ReceiveThread(LPVOID param)
    {
        ChatClient* client = (ChatClient*)param;
        char buffer[BUFFER_SIZE];

        while (client->IsRunning())
        {
            int bytes = recv(client->GetSocket(), buffer, BUFFER_SIZE - 1, 0);
            if (bytes <= 0) break;

            buffer[bytes] = '\0';
            string msg(buffer);

            if (msg.substr(0, 5) == "FILE:")
            {
                client->ReceiveFile(msg);
            }
            else if (msg == "FILE_OK" || msg == "UPLOAD_OK")
            {
                // handled elsewhere
            }
            else
            {
                cout << "\n" << msg << endl;
                cout << "> ";
                cout.flush();
            }
        }
        return 0;
    }

public:
    ChatClient() : sock(INVALID_SOCKET), running(true) {}

    ~ChatClient() { Close(); }

    bool Connect(const string& serverIP)
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false;

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) return false;

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        addr.sin_addr.s_addr = inet_addr(serverIP.c_str());

        if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) return false;

        cout << "Enter your name: ";
        getline(cin, name);
        send(sock, name.c_str(), (int)name.size() + 1, 0);

        recvThread = CreateThread(NULL, 0, ReceiveThread, this, 0, NULL);

        return true;
    }

    void Run()
    {
        string input;
        cout << "Connected! Commands: /msg <text>, /file <name>, /upload <name>, /process <file> <n>, /history, /exit" << endl;
        cout << "> ";

        while (running)
        {
            getline(cin, input);

            if (input == "/exit")
            {
                SendCommand("exit");
                running = false;
                break;
            }
            else if (input.substr(0, 4) == "/msg")
            {
                string msg = input.substr(5);
                SendCommand("MSG:" + msg);
            }
            else if (input.substr(0, 5) == "/file")
            {
                string filename = input.substr(6);
                SendCommand("FILE:" + filename);
            }
            else if (input.substr(0, 7) == "/upload")
            {
                string filename = input.substr(8);
                UploadFile(filename);
            }
            else if (input.substr(0, 8) == "/process")
            {
                size_t p1 = input.find(' ', 9);
                string filename = input.substr(9, p1 - 9);
                string num = input.substr(p1 + 1);
                SendCommand("PROCESS:" + filename + " " + num);
            }
            else if (input == "/history")
            {
                SendCommand("HISTORY");
            }
            else if (!input.empty() && input[0] != '/')
            {
                SendCommand("MSG:" + input);
            }

            cout << "> ";
        }
    }

    void Close()
    {
        running = false;
        if (sock != INVALID_SOCKET)
            closesocket(sock);
        WSACleanup();
    }

    SOCKET GetSocket() { return sock; }
    bool IsRunning() { return running; }
};

int main(int argc, char* argv[])
{
    string serverIP = "127.0.0.1";
    if (argc > 1) serverIP = argv[1];

    ChatClient client;
    if (!client.Connect(serverIP))
    {
        cout << "Failed to connect to server" << endl;
        return -1;
    }

    client.Run();
    return 0;
}