


#include "myImGui.cpp"
#include <stdlib.h>
#include "dispositivos.cpp"
#include "DB.cpp"
//#include "objetosUI.cpp"


#include "elgin.h"




#define _IMAGE_PATH "C:/projetos/HIV/source/"
#define _MODEL_PATH ""
#define _DATA_PATH " "


using framerate3 = std::chrono::duration<int, std::ratio<1, 200>>;

MYSQL* conn;

vulkan app;
GUI Gui;

char buff[250];
char buffServer[250];
char buffBanco[250];

char buff2[250];
bool car;

double vz = 0;
double v = 0;

double r = 0;
double g = 0;
double b = 0;
double a = 5;



auto prev = std::chrono::system_clock::now();
auto next = prev + framerate3{ 1 };



ImVec2 tamanho_da_janela{};

inline void carregar_imagem(const char * nome_imagem,vulkan::MyTextureData &texture,const char * CAMINHO=(_IMAGE_PATH)){

    if (texture.carregada) { return; }
    texture.carregada = true;
    std::ostringstream str;
    str << _IMAGE_PATH << nome_imagem;

    app.createTextureImage(str.str().c_str(), texture);
    
  
    
}

inline void liberar(vulkan::MyTextureData *tex) {
    if (tex->carregada) {
        app.destroir_image(tex);
        tex->carregada = false;
    }
}

bool chamada = false;
void impresora(int i) {
        chamada = true;
        std::system("node app.js");
        chamada = false;
        std::this_thread::yield();
}
std::thread thd;


std::vector<char>data;

vulkan::MyTextureData textura;





vulkan::MyTextureData textura_vendas;

bool estado_da_janela;
bool estado_da_menu_bar= false;

inline void pagina_inicial()

{

  
    tamanho_da_janela.y = app.swapChainExtent.height;
    tamanho_da_janela.x = app.swapChainExtent.width;

    Netfila* CONN = Server.retornaConexao();
    Armazenamento arquivo = Armazenamento("tex.txt.txt");
 

   carregar_imagem("imagem_programador.jpg",textura);
   carregar_imagem("baixados.jpg", textura_vendas);

    Begin("janela", &estado_da_janela, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    SetWindowSize(tamanho_da_janela);
    SetWindowPos(ImVec2(0,0));
    
    ImGuiCol i=1;
    PushStyleColor(ImGuiCol_WindowBg,ImVec4(r/10,g/10,b/10,a/10));
    BeginMenuBar();

    MenuItem("vendas", texto, &estado_da_menu_bar, true);
    EndMenuBar();

    ImGui::Text("estado da menu bar");
    ImGui::Text(std::to_string(estado_da_menu_bar).c_str());
    if (estado_da_menu_bar) {
        estado_da_menu_bar = false;
            std::system("start HIV"); }

    ImGui::Text("items amarzenados\n\n\n");

    if (arquivo.ler_arquivo(data))  ImGui::Text(data.data());

    ImGui::Text("fim");

    ImGui::Text("BYTES recebidos via TCP/IP\n\n\n");

    for (int i = 0; i < CONN->filaDeRecebimento.size(); i++)  ImGui::Text(CONN->filaDeRecebimento[i].C_Str());

    ImGui::Text("fim");


    
 
    ImVec2 vec;
    std::vector<char>valor;
    valor.clear();
    valor.push_back('9');
    valor.push_back('5');
    valor.push_back(NULL);
    if (Server.receberComandosDe("imagems", "1", &valor)) {
        int val = std::stoi(valor.data());
      
        vec.x = val;
        vec.y = val;
    }

       
    InputDouble("ajuste no x",&v);
    InputDouble("ajuste no z", &vz);

    InputDouble("ajuste no vermelho", &r);
    InputDouble("ajuste no verde", &g);
    InputDouble("ajuste no azul", &b);
    InputDouble("ajuste na trasparencia", &a);

    
    if(textura.carregada)
    Image(textura.DS, ImVec2(textura.Width -v, textura.Height-vz ), ImVec2(0,0), ImVec2(1.0,1.0));
    if(textura_vendas.carregada)
    Image(textura_vendas.DS, ImVec2(textura_vendas.Width - v, textura.Height - vz));

    if(textura.carregada)
    ImageButton(textura.DS,vec);
   
    if (Button("liberar imagem"))
    {
        liberar(&textura);
        liberar(&textura_vendas);
    }

    ImGui::End();



}


Impresora *impre =nullptr;
inline void debugDoServidor() {


    Begin("debug do Servidor");


    


    InputText("ensira a msg",buffServer,256);

   // DbQuery(buff, &car, buff2);
  
    if (Button("enviar")) {
        Server.filaEO.inserirE(buffServer);
        TREAD_PAUSE = true;
       
      //  if (!chamada) {
       //     thd = std::thread(impresora,0);    
     //   }

        
    }

    if (Button("imprimir")) {



        if (impre==nullptr)
            impre = new Impresora();

        while(!(impre->statusServer == true && impre->statusDispositivo)) {
       
        }

        impre->enviar("first");
        impre->enviar((const char *)buff);
        TREAD_PAUSE = true;
        debugServer("enviado");

        if (impre != nullptr)
            if (!impre->statusServer && !impre->statusDispositivo) {
                impre->~Impresora();
                delete impre;
            }



       
    }

 
    ImGui::Text("mensagems lançadas pelo servidor:");

    for (int i = 0; i < mensagensS.size(); i++)  ImGui::Text(mensagensS[i].C_Str());



    ImGui::End();

}

void debugBancoDeDados()
{



    InputText("ensira a msg", buffBanco, 256);
    if (Button("inserir"))
    {
        DbInserir("usuarios", "usuario,nome,email,senha", buffBanco);

    }
   //separador
    {
        Text("teste do separador:");
        SEPARADOS sep = separador(buffBanco);
        for (int i = 0; i < sep.size(); i++) Text(sep[i].data());

        ImGui::Text("mensagems lançadas pelo banco de dados:");

        for (int i = 0; i < DB_DEBUG.size(); i++)  ImGui::Text(DB_DEBUG[i].c_str());

    }
   //select
    {
        MYSQL_RES* res = DbSelectALL(buffBanco);
        if (res != nullptr) {
            for (int i = 0; i < res->row_count; i++) {
                Text(mysql_fetch_row(res)[1]);
            }

            mysql_free_result(res);
        }
    }
    if (Button("update")) 
    {
    
        DbUpdate("usuarios","usuario",buffBanco,"senha,nome","ad,ads");
    
    }



}


inline void corpo() {
    debugBancoDeDados();
    pagina_inicial();
    debugDoServidor();
  

}




int  WinMain(
     HINSTANCE hInstance,
     HINSTANCE hPrevInstance,
     LPSTR     lpCmdLine,
     int       nShowCmd
){
   

   
    omp_set_num_threads(2);
   // conn = DB_INIT();
        app.init();
       
        Gui.initImGui(app,corpo);
     
#pragma omp parallel shared(connectado,conectLoop,mensagensS)       
        {
            debugServer(std::to_string(omp_get_num_threads()));

#pragma omp single nowait
            {
                //   app.loop(imGui);
                app.loop(imGuiL);
              //  fecharDb(conn);

            }

#pragma omp single nowait
            {
                if (!Server.conectarEXTERN("servidor"))
                    Server.conectar("cliente");

            }




       



  
            }
   
       

        return 0;
    
}



