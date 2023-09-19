#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <thread>
#include <map>
#include <chrono>
#include <iostream>

#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>


template <class Clock, class Duration>
void
sleep_until(std::chrono::time_point<Clock, Duration> tp)
{
    using namespace std::chrono;
    std::this_thread::sleep_until(tp - 10us);
    while (tp >= Clock::now())
        ;
}


using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

using v8::Context;
using v8::Function;
using v8::FunctionTemplate;
using v8::Number;
using v8::ObjectTemplate;


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define PAUSE_TREAD THREAD_PAUSE

typedef void(*f)(void);

typedef std::map<const char*, SOCKET>::value_type sock;

#define  mk_par( x,y) std::map<const char*, SOCKET>::value_type(x,y)

#define   debugServer(x) mensagensS.push_back(std::string(x));

std::vector<std::string>mensagensS;



struct Netfila : public node::ObjectWrap {

    bool em_uso_para_escrita;
    bool em_uso_para_leitura;
    std::vector<std::string>filaDeEnvio;
    std::vector<std::string>filaDeRecebimento;



    bool inserirR(char* data) {

        //   if (em_uso_para_escrita == false) {
        em_uso_para_escrita = true;
        filaDeRecebimento.push_back(std::string(data));
        em_uso_para_escrita = false;
        return true;
        //   }
        //   else {

         //      return false;
        //   }

    }

    std::string retiraR() {


        std::string temp;
        em_uso_para_leitura = true;
        temp = filaDeRecebimento[filaDeRecebimento.size() - 1];
        filaDeRecebimento.pop_back();
        em_uso_para_leitura = false;
        return temp;

    }

    std::string retiraE() {


        std::string temp;
        em_uso_para_leitura = true;
        temp = this->filaDeEnvio[this->filaDeEnvio.size() - 1];
        this->filaDeEnvio.pop_back();
        em_uso_para_leitura = false;
        return temp;

    }

    bool inserirE(char* data) {

        if (em_uso_para_escrita == false) {
            em_uso_para_escrita = true;
            filaDeEnvio.push_back(std::string(data));
            em_uso_para_escrita = false;
            return true;
        }
        else {

            return false;
        }

    }





};




class server {
public:

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


    Netfila filaEO{};
    bool connectado = false;
    bool conectLoop = false;
    bool listenb = false;
    bool TREAD_PAUSE = false;

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
        iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
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
                            if (!connectado) {
                                debugServer("desconectando")
                                    break;
                            }
                        }

                    }
                }

                if (filaEO.filaDeEnvio.size() > 0 && TREAD_PAUSE) {
                    debugServer("no send")
                        std::string temp(filaEO.retiraE());
                    if (send(ClientSocket, temp.c_str(), temp.length(), 0) > 0) {
                        debugServer("enviado");

                    }
                    else { debugServer("nao enviado cheque o cliente") }
                    //retirar!!
                    TREAD_PAUSE = false;
                }



                /*  else {
                      debugServer("sem eventos de envio. fazendo send as cegas :/")
                          if (filaEO.filaDeEnvio.size() > 0) {
                              std::string temp = filaEO.retiraE();
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


    bool receberComandosDe(const char* ferramenta, const char* id, std::vector<char>* valor) {


        std::vector<char>comando;

        int cont = 0;
        while (ferramenta[cont] != NULL)
        {
            comando.push_back(ferramenta[cont]);
            cont++;
        }
        comando.push_back(':');
        while (id[cont] != NULL)
        {
            comando.push_back(ferramenta[cont]);
            cont++;
        }
        comando.push_back('=');
        comando.push_back(NULL);

        std::string cmd(comando.data());


        // comando << ferramenta << ":" << id << "=";
        bool val = false;

        unsigned short tamanho = cmd.length();

        for (size_t i = 0; i < filaEO.filaDeRecebimento.size(); i++)
        {
            std::string string = filaEO.filaDeRecebimento[i].c_str();

            for (int i = 0; i < tamanho; i++) {

                if (!(cmd.c_str()[i] == string.c_str()[i])) {
                    val = false;
                    break;
                }
                val = true;
            }
            if (val) {
                int cont = 0;
                valor->clear();
                while (string.c_str()[tamanho + cont] != NULL) {
                    valor->push_back(string.c_str()[tamanho + cont]);
                    cont++;
                }
                valor->push_back(NULL);
                return true;
            }


        }
        return false;
    }

public:










};

class CONN {

public:
    server Server{};

    std::thread* serverloop;

    /* static void Init(v8::Local<v8::Object> exports)
      {
          Isolate* isolate = exports->GetIsolate();
          Local<Context> context = isolate->GetCurrentContext();

          Local<ObjectTemplate> addon_data_tpl = ObjectTemplate::New(isolate);
          addon_data_tpl->SetInternalFieldCount(1);  // 1 field for the MyObject::New()
          Local<Object> addon_data =
              addon_data_tpl->NewInstance(context).ToLocalChecked();

          // Prepare constructor template
          Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New, addon_data);
          tpl->SetClassName(String::NewFromUtf8(isolate, "CONN").ToLocalChecked());
          tpl->InstanceTemplate()->SetInternalFieldCount(1);

          // Prototype
        /* NODE_SET_PROTOTYPE_METHOD(tpl, "plusOne", PlusOne);

          Local<Function> constructor = tpl->GetFunction(context).ToLocalChecked();
          addon_data->SetInternalField(0, constructor);
          exports->Set(context, String::NewFromUtf8(
              isolate, "MyObject").ToLocalChecked(),
              constructor).FromJust();
      }
  */

};


bool conectarEXTERN(const char* nome, CONN* p) {


    auto prev = std::chrono::system_clock::now();
    auto next = prev + server::framerate{ 1 };

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

    FD_ZERO(&p->Server.fd_r);
    FD_ZERO(&p->Server.fd_w);
    FD_SET(p->Server.ClientSocket, &p->Server.fd_r);
    FD_SET(ConnectSocket, &p->Server.fd_r);

    FD_SET(p->Server.ClientSocket, &p->Server.fd_w);
    FD_SET(ConnectSocket, &p->Server.fd_w);
    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        debugServer("socket invalido")
            WSACleanup();
        return false;
    }

    // Send an initial buffer




    // Receive until the peer closes the connection


    // cleanup

    p->Server.listenb = false;
    p->Server.connectado = true;

    p->Server.tm.tv_sec = 5;
    p->Server.tm.tv_usec = 2;
    p->Server.mode = 1;
    ioctlsocket(ConnectSocket, FIONBIO, (u_long*)&p->Server.mode);

    while (p->Server.connectado) {

        FD_ZERO(&p->Server.fd_r);
        FD_ZERO(&p->Server.fd_w);
        FD_SET(ConnectSocket, &p->Server.fd_r);
        FD_SET(ConnectSocket, &p->Server.fd_w);

        const char* b = "tesdasdasdas";


        p->Server.mode = 0;
        //  if (FD_ISSET(ConnectSocket, &fd_w) && PAUSE_TREAD) {
        if (select(0, &p->Server.fd_r, nullptr, nullptr, &p->Server.tm) > 0) {

            if (FD_ISSET(ConnectSocket, &p->Server.fd_r) && !p->Server.TREAD_PAUSE) {
                p->Server.mode = 0;
                debugServer("evento de recebimento")
                    ZeroMemory(p->Server.recvBuff, DEFAULT_BUFLEN);
                iResult = recv(ConnectSocket, p->Server.recvBuff, DEFAULT_BUFLEN, 0);
                if (iResult > 0) {


                    {
                        std::vector<char>i;
                        int count = 0;

                        p->Server.recvBuff[iResult] = NULL;
                        while (p->Server.recvBuff[count] != NULL) {
                            i.push_back(p->Server.recvBuff[count]);
                            count++;
                        }
                        i.push_back(p->Server.recvBuff[count + 1]);
                        p->Server.filaEO.inserirR(i.data());
                    }





                    p->Server.TREAD_PAUSE = !p->Server.TREAD_PAUSE;


                }

            }
        }

        if (p->Server.filaEO.filaDeEnvio.size() > 0 && p->Server.TREAD_PAUSE) {
            debugServer("eventos de envio")
                std::string temp(p->Server.filaEO.retiraE());
            if (send(ConnectSocket, temp.c_str(), temp.length(), 0) > 0)
                debugServer("enviado");
            //retirar!!

            p->Server.TREAD_PAUSE = !p->Server.TREAD_PAUSE;
        }

        //    }

    //send else




/*   else {
       debugServer("sem eventos de envio. fazendo send as cegas :/")
       if (filaEO.filaDeEnvio.size() > 0) {
           aiString temp = filaEO.retiraE();

           PAUSE_TREAD = !PAUSE_TREAD;
       }}*/




        ::sleep_until(next);
        next += server::framerate{ 1 };

    }



    p->Server.conectLoop = false;
    debugServer("conexao encerada")



        return true;

}




void conEx(std::thread* loop, CONN* p) {

    loop = new std::thread(conectarEXTERN, "servidor", p);

}
void fecharConn(std::thread* loop) {

    loop->join();
    delete loop;

}


void INICIALIZAR_SERVER(const v8::FunctionCallbackInfo<Value>& args) {

    CONN* p = (CONN*)malloc(sizeof(CONN));

    Local<Number>num = Number::New(args.GetIsolate(), (long long)p);

    conEx(p->serverloop, p);
    args.GetReturnValue().Set(num);

}



void RECEBER(const FunctionCallbackInfo<Value>& args) {


    long long ptr = args[0].As<v8::Number>()->Value();
    CONN* p = (CONN*)ptr;

    auto prev = std::chrono::system_clock::now();
    auto next = prev + server::framerate{ 1 };








    ::sleep_until(next);
    next += server::framerate{ 1 };

    for (int i = 0; i < 5; i++) {
        p->Server.filaEO.inserirE((char*)"eae:lado da impresora");
        p->Server.TREAD_PAUSE = true;
        ::sleep_until(next);
        next += server::framerate{ 1 };
        //  p->Server.filaEO.inserirR("ESCREVENDO DE VOLTA");
    }




    //char rec[256];

  //  ZeroMemory(rec, 256);


  //  strcpy(rec, p->Server.filaEO.retiraR().c_str());


}



void RECV(const FunctionCallbackInfo<Value>& args) {
    Isolate *   is= args.GetIsolate();

    long long ptr = args[0].As<v8::Number>()->Value();
    CONN* p = (CONN*)ptr;
    if (p->Server.filaEO.filaDeRecebimento.size() > 0) {
        Local<String>data = String::NewFromUtf8(is, p->Server.filaEO.filaDeRecebimento[0].c_str()).ToLocalChecked();
        args.GetReturnValue().Set(data);
    }
    else {

        Local<String>data = String::NewFromUtf8(is, "nada recebido").ToLocalChecked();
        args.GetReturnValue().Set(data);
    }


}

void ENVIAR(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
   
    long long v = args[0].As<Number>()->Value();
    Local<String> dataS = args[1].As<String>();
    String::Utf8Value dataU(isolate, dataS);
    


    CONN* p = (CONN*)v;


    char data[DEFAULT_BUFLEN];
    strcpy(data, *dataU);
    p->Server.filaEO.inserirE(data);
    p->Server.TREAD_PAUSE = true;


}
void CONNECTADO(const v8::FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    Local<String> data;




    data = String::NewFromUtf8(isolate, "isconn").ToLocalChecked();

    args.GetReturnValue().Set(data);

}



void FERCHAR_CON(const v8::FunctionCallbackInfo<Value>& args) {


    long long v = args[0].As<Number>()->Value();
    CONN* p = (CONN*)v;
    fecharConn(p->serverloop);

    free(p);

    args.GetReturnValue().Set(0);

}


void init(Local<Object>exports) {

    NODE_SET_METHOD(exports, "Serve_init", INICIALIZAR_SERVER);
    NODE_SET_METHOD(exports, "loop", RECEBER);
    NODE_SET_METHOD(exports, "receber", RECV);
    NODE_SET_METHOD(exports, "enviar", ENVIAR);
    NODE_SET_METHOD(exports, "fechar", FERCHAR_CON);
    NODE_SET_METHOD(exports, "isconn", CONNECTADO);


}


NODE_MODULE(NODE_GYP_MODULE_NAME, init)




