

#include "server2.cpp"

 void rotinaServer(bool * status,server *serve) {

     *status = true;
    serve->conectar("dispositivo");
   *status = false;
   std::this_thread::yield();

}

 void rotina(const char* executavel, bool* status) {


     *status = true;
     system(executavel);
     *status = false;
   
    std::this_thread::yield();

 }

 class conexão{


    public:
   
    server server{};
    std::thread threadDispositivo;
    std::thread threadServer;
    bool statusDispositivo;
    bool statusServer;
    bool carregado;

    conexão(const char * dispositivo,const char * executavel,const char * porta) {

        memset(this, 0, sizeof(this));
        carregado = true;
        server.porta = (char *)porta;

        threadServer = std::thread(rotinaServer, &statusServer, &server);
        threadDispositivo =  std::thread(rotina, executavel, &statusDispositivo);
       
     
    }
    ~conexão() {

        carregado = false;
    }

   void fechar() {
     
      
    }

    bool enviar(const char* data)
    {    

        server.filaEO.inserirE((char*)data);
     //ADICIONAR O THREAD PAUSE
        return true;
    }
    aiString receber_ultima_mensagem() {
        std::vector<aiString>* temF = &server.filaEO.filaDeRecebimento;

        return (*temF)[temF->size() - 1];
    }
   
   
};

class Impresora :public conexão
{
public:
    Impresora() : conexão("impresora", "node app.js", "27016")
    {

    }

    ~Impresora()
    {
       

    }


};

class banco_de_dados :public conexão
{
   
public:
    banco_de_dados() : conexão("banco de dados", "node db.js","2733")
    {


    }

 

};

class Buffer :public std::vector<std::vector<char>> {

   
    size_t linhas=0;

    Buffer() {}

   Buffer( const char * data) {
       this->resize(strlen(data));
       std::vector<char>temp;
       for (int i = 0; data[i] != NULL; i++)
           temp.push_back(data[i]);

       this->push_back(temp);
       linhas++;
    }

   void adicionar( const char * data) {
       std::vector<char>temp;
       for (int i = 0; data[i] != NULL; i++)
           temp.push_back(data[i]);

       this->push_back(temp);
       linhas++;
   }

};