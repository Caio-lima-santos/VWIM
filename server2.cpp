


#ifndef SERVER 


#undef UNICODE
#define SERVER
#define WIN32_LEAN_AND_MEAN
#include "myVulkan.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <map>
#include <omp.h>


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define PAUSE_TREAD TREAD_PAUSE






typedef std::map<const char*, SOCKET>::value_type sock;

#define  mk_par( x,y) std::map<const char*, SOCKET>::value_type(x,y)

#define   debugServer(x) mensagensS.push_back(aiString(x));

std::vector<aiString>mensagensS;

struct Netfila {

    bool em_uso_para_escrita;
    bool em_uso_para_leitura;
    std::vector<aiString>filaDeEnvio;
    std::vector<aiString>filaDeRecebimento;

    bool inserirR(char* data) {

        if (em_uso_para_escrita == false) {
            em_uso_para_escrita = true;
            filaDeRecebimento.push_back(aiString(data));
            em_uso_para_escrita = false;
            return true;
        }
        else {

            return false;
        }

    }

    aiString retiraR() {


        aiString temp;
        em_uso_para_leitura = true;
        temp = this->filaDeRecebimento[this->filaDeRecebimento.size()-1];
        this->filaDeRecebimento.pop_back();
        em_uso_para_leitura = false;
        return temp;

    }

    aiString retiraE() {


        aiString temp;
        em_uso_para_leitura = true;
        temp = this->filaDeEnvio[this->filaDeEnvio.size()-1];
        this->filaDeEnvio.pop_back();
        em_uso_para_leitura = false;
        return temp;

    }

    bool inserirE(char* data) {

        if (em_uso_para_escrita == false) {
            em_uso_para_escrita = true;
            filaDeEnvio.push_back(aiString(data));
            em_uso_para_escrita = false;
            return true;
        }
        else {

            return false;
        }

    }





};



bool connectado = false;
bool conectLoop = false;
bool listenb = false;
bool TREAD_PAUSE = false;



class server {

    
  
    WSADATA wsaData;
    int iResult;
    const char* cli_name = "cliente";
    const char* sendbuf = "caio";

    std::map<const char*, SOCKET> clientes;





    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    SOCKET ConnectSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL, * ptr = NULL;
    struct addrinfo hints;

    struct addrinfo* result2 = NULL, * ptr2 = NULL;
    struct addrinfo hints2;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;



    int bytes = 0;

    fd_set fd_r;
    fd_set fd_w;


    timeval tm;


    u_long mode = 0;

    char recvBuff[DEFAULT_BUFLEN];
    using framerate = std::chrono::duration<int, std::ratio<1, 20>>;
    using framerate2 = std::chrono::duration<int, std::ratio<1, 200>>;

    int N = 0;
    std::chrono::system_clock::duration sum{ 0 };
    







public:
    char* porta = (char*)DEFAULT_PORT;
    Netfila filaEO{};

    int __cdecl initS(const char* endereço = ("127.0.168.1"), int af = (AF_INET), int tipo = (SOCK_STREAM), int protocolo = (IPPROTO_TCP))
    {


        return true;

    }


    bool conectar(const char* nome) {



        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed with error: %d\n", iResult);
            return 1;
        }
        ZeroMemory(&result, sizeof(result));
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        // Resolve the server address and port
        iResult = getaddrinfo(NULL,porta, &hints, &result);
        if (iResult != 0) {
            printf("getaddrinfo failed with error: %d\n", iResult);
            WSACleanup();
            return false;
        }

        // Create a SOCKET for the server to listen for client connections.

        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return false;
        }


        // Setup the TCP listening socket
        iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            printf("bind failed with error: %d\n", WSAGetLastError());
            freeaddrinfo(result);
            closesocket(ListenSocket);
            WSACleanup();
            return false;
        }

        freeaddrinfo(result);

        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            printf("listen failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return false;
        }

        debugServer("modo escuta...")
            ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            debugServer("falha na aceitação")
                printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return false;
        }
      
        clientes.insert(mk_par(nome, ClientSocket));
        debugServer("sucesso na aceitacao")
            closesocket(ListenSocket);
        connectado = true;
        listenb = true;
        ConnectLoop();
        return true;

    }



    bool conectarEXTERN(const char* nome) {



        auto prev = std::chrono::system_clock::now();
        auto next = prev + framerate{ 1 };

        WSACleanup();
        WSADATA wsaData;
        SOCKET ConnectSocket = INVALID_SOCKET;
        struct addrinfo* result = NULL,
            * ptr = NULL,
            hints;

        char recvbuf[DEFAULT_BUFLEN];
        int iResult;
        int recvbuflen = DEFAULT_BUFLEN;

        // Validate the parameters


        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed with error: %d\n", iResult);
            return false;
        }

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        // Resolve the server address and port
        iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
        if (iResult != 0) {
            debugServer("addr info falho")
                WSACleanup();
            return false;
        }

        // Attempt to connect to an address until one succeeds
        for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

            // Create a SOCKET for connecting to server
            ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                ptr->ai_protocol);
            if (ConnectSocket == INVALID_SOCKET) {
                debugServer("socket falho")
                    WSACleanup();
                return false;
            }

            // Connect to server.
            iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
            if (iResult == SOCKET_ERROR) {
                debugServer("conexao falhou")
                    closesocket(ConnectSocket);
                ConnectSocket = INVALID_SOCKET;
                return false;
            }
            break;
        }

        FD_ZERO(&fd_r);
        FD_ZERO(&fd_w);
        FD_SET(ClientSocket, &fd_r);
        FD_SET(ConnectSocket, &fd_r);

        FD_SET(ClientSocket, &fd_w);
        FD_SET(ConnectSocket, &fd_w);
        freeaddrinfo(result);

        if (ConnectSocket == INVALID_SOCKET) {
            debugServer("socket invalido")
                WSACleanup();
            return false;
        }

        // Send an initial buffer




        // Receive until the peer closes the connection


        // cleanup

        listenb = false;
        connectado = true;
        tm.tv_sec = 10;
        tm.tv_usec = 2;

        while (connectado) {

            FD_ZERO(&fd_r);
            FD_ZERO(&fd_w);
            FD_SET(ConnectSocket, &fd_r);
            FD_SET(ConnectSocket, &fd_w);


            const char* b = "tesdasdasdas";

            if (select(0, &fd_r, &fd_w, nullptr,&tm) > 0) {

                if (FD_ISSET(ConnectSocket, &fd_w) && TREAD_PAUSE) {
                    debugServer("eventos de envio")
                        if (filaEO.filaDeEnvio.size() > 0) {
                            aiString temp = filaEO.retiraE();
                            if (send(ConnectSocket, temp.C_Str(), temp.length, 0) > 0)
                                debugServer("enviado");
                            //retirar!!
                            TREAD_PAUSE = false;
                        }
                        else {
                            send(ConnectSocket, &"filaEO vazia"[0], 14, 0);
                            debugServer("filaEO vazia")

                        }
                }


                if (FD_ISSET(ConnectSocket, &fd_r))
                {
                    {
                        debugServer("eventos de recebimento")
                            aiString tempbuff;

                        switch (int i = recv(ConnectSocket, tempbuff.data, MAXLEN, 0))
                        {

                        case 0:
                            debugServer("nada recebido")
                                break;
                        case SOCKET_ERROR:
                            debugServer("conexao encerada")
                                break;
                        default:
                            debugServer("bytes recebidos");
                            debugServer(tempbuff);
                            filaEO.inserirR((char*)tempbuff.C_Str());
                            break;
                        }
                    }
                }



                ::sleep_until(next);
                next += framerate2{ 1 };

            }



            ::sleep_until(next);
            next += framerate{ 1 };

        }



        conectLoop = false;
        debugServer("conexao encerada")



            return true;

    }


    void ConnectLoop() {
        listenb = true;
        conectLoop = true;
        int bytes = 0, iResult = 0;

        FD_ZERO(&fd_r);
        FD_ZERO(&fd_w);
        FD_SET(ClientSocket, &fd_r);
        FD_SET(ConnectSocket, &fd_r);

        FD_SET(ClientSocket, &fd_w);
        FD_SET(ConnectSocket, &fd_w);

        timeval tm{};
        tm.tv_sec = 10;
        tm.tv_usec = 2;

        char recvBuff[DEFAULT_BUFLEN];
        using framerate = std::chrono::duration<int, std::ratio<1, 100>>;
        auto prev = std::chrono::system_clock::now();
        auto next = prev + framerate{ 1 };
        int N = 0;
        std::chrono::system_clock::duration sum{ 0 };

       
        ioctlsocket(ClientSocket, FIONBIO, &mode);

            if (listenb) {
                do {

                    mode = 1;

                    int Sn = 0;
                    if (Sn = select(0, &fd_r, nullptr, nullptr, &tm) > 0) {
                        mode = 0;
                        ZeroMemory(&recvBuff, sizeof(recvBuff));
                        // debugServer("select funciono HIV")
                        // debugServer(std::to_string(Sn))


                        if (FD_ISSET(ClientSocket, &fd_r) && !TREAD_PAUSE) {
                            ZeroMemory(recvBuff, sizeof(recvBuff));

                            debugServer("no recv")
                                iResult = recv(ClientSocket, recvBuff, DEFAULT_BUFLEN, 0);
                            if (iResult > 0) 
                            {
                                {
                                    std::vector<char>i;
                                    int count = 0;

                                    recvBuff[iResult] = NULL;
                                    while (recvBuff[count] != NULL) {
                                        i.push_back(recvBuff[count]);
                                        count++;
                                    }
                                    i.push_back(recvBuff[count + 1]);
                                    filaEO.inserirR(i.data());
                                }
                               // debugServer("BYTES recebidos")
                                 //   debugServer(std::to_string(iResult))
                                   // debugServer("Mensagen:")
                                   // debugServer(i.data())

                                   // TREAD_PAUSE = !TREAD_PAUSE;

                            }
                            else {
                                desconectarTodosEencerar();
                                    debugServer("desconectando")
                                        break;
                                
                            }

                        }
                    }

                    if (filaEO.filaDeEnvio.size() > 0 && TREAD_PAUSE) {
                        debugServer("no send")
                        aiString temp(filaEO.retiraE());
                        if (send(ClientSocket, temp.C_Str(), temp.length, 0) > 0) {
                            debugServer("enviado");

                        }
                        else { debugServer("nao enviado cheque o cliente") }
                        //retirar!!
                        TREAD_PAUSE = false;
                    }



                  /*  else {
                        debugServer("sem eventos de envio. fazendo send as cegas :/")
                            if (filaEO.filaDeEnvio.size() > 0) {
                                aiString temp = filaEO.retiraE();
                                if (send(ClientSocket, temp.C_Str(), temp.length, 0) > 0)
                                    debugServer("enviado")
                                    PAUSE_TREAD = !PAUSE_TREAD;
                            }}*/
                    
                    FD_ZERO(&fd_r);
                    FD_ZERO(&fd_w);
                    FD_SET(ClientSocket, &fd_r);
                    FD_SET(ClientSocket, &fd_w);

                      ::sleep_until(next);
                    next += framerate{ 1 };

                } while (connectado);
            }
        
        
        conectLoop = false;
        debugServer("conexao encerada")



    }





    bool desconectarCli(const char* nome) {

        if (!clientes.empty())
            closesocket(clientes.at(nome));
        return true;


    }


    bool desconectarTodosEencerar() {
        connectado = false;

        for (const auto& elem : clientes) {
            closesocket(elem.second);
            clientes.erase(clientes.find(elem.first));


            closesocket(ListenSocket);
            closesocket(ConnectSocket);
        }
        return WSACleanup();

    }

    //possivel err
    bool receberDe(const char* nome, char* data, int* tamanho = ((int*)0), UINT64 maxTenta = (20000)) {
        int tm = 0;
        int tentativas = 0;
        char inf[DEFAULT_BUFLEN] = "falha2";
        char* ptr_inf = &inf[0];

        while (tm <= 0 && tentativas < maxTenta) {

            tm = recv(clientes.at(nome), inf, DEFAULT_BUFLEN, 0);
            tentativas++;
        }

        if (tentativas >= maxTenta) {
            data = (char*)"falha ao receber";
            *tamanho = strlen(data);
            return false;
        }
        *tamanho = strlen(inf);
        inf[strlen(inf)] = '\0';
        strcpy(data, ptr_inf);
        return true;

    }

    bool enviarPara(const char* nome, char* data, size_t tamanho = ((size_t)0)) {

        send(clientes.at(nome), data, tamanho, 0);

        return true;

    }


    Netfila* retornaConexao() {
    
        return &filaEO;
    
    
    }


    bool receberComandosDe(const char *ferramenta,const char * id,std::vector<char>*valor){
        
       
        std::ostringstream comando;
        comando << ferramenta<<":"<<id<<"=";
        bool val=false;
        
       unsigned short tamanho = comando.str().length();

        for (size_t i = 0; i < filaEO.filaDeRecebimento.size(); i++)
        {
           std::string string= filaEO.filaDeRecebimento[i].C_Str();

           for (int i = 0; i < tamanho;i++) {
          
               if (!(comando.str().c_str()[i] == string.c_str()[i])) {
                   val = false;
                   break;
               }
               val = true;
           }
           if (val) {
               int cont = 0;
               valor->clear();
               while (string.c_str()[tamanho+cont] != NULL) {
                   valor->push_back(string.c_str()[tamanho+cont]);
                   cont++;
               }
               valor->push_back(NULL);
               return true;
           }


        }
        return false;
    }




};
#endif // !1