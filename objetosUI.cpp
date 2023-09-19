
#ifndef  GLFW_INCLUDE_VULKAN

#include "myVulkan.h"

#endif


#define UIJanelaLimpa ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar

#define UIJanelaALL

#define BOTAO 0
#define INPUT 1


/*

typedef void(*func)(void);



vulkan & contexto;


void initUI(vulkan& app) { contexto = app; }



class UIobjeto 
{

public:

	char* nome;
	ImVec2 posicao;
	bool carregado;
	bool nao_carregar=true;
	
	vulkan::MyTextureData imagem;
	bool possui_imagem;
	bool nao_carregar_imagem;

	std::vector<UIobjeto>::iterator itr;

	void * ptr;

	short tipo;

	void(*Loop)(void);
	void(*Load)(void);


	void carregar(bool obj, bool img) { nao_carregar = !obj; nao_carregar_imagem = !img; }
	
	inline void carregar_imagem(const char* nome_imagem, const char* CAMINHO = (_IMAGE_PATH)) {

		if (this->imagem.carregada || nao_carregar_imagem) { return; }
		this->imagem.carregada = true;
		std::ostringstream str;
		str << _IMAGE_PATH << nome_imagem;

		contexto.createTextureImage(str.str().c_str(), this->imagem);



	}


	inline void liberar() {
		if (this->imagem.carregada) {
			contexto.destroir_image(&this->imagem);
			this->imagem.carregada = false;
			nao_carregar_imagem = !nao_carregar_imagem;
		}
	}


};


class botao: public UIobjeto 
{

	
public:




	botao(void(*loop)(void),void(*load)(void)=nullptr, char* nome = (char*)("enviar"),bool carregar=true)
	{
	    
		this->nome = nome;
		this->Loop = loop;
		this->Load = load;
		this->nao_carregar = !carregar;
		this->ptr = this;
		this->tipo = BOTAO;
	
	}

	void loop() {

		if (!this->carregado && !this->nao_carregar) {
			if(Load !=nullptr)
			Load();
			carregado = true;
		}
		if (!this->nao_carregar) {
			if (possui_imagem)
			{
				if (ImGui::ImageButton(imagem.DS, posicao)) { Loop(); }
			}
			else {
				if (ImGui::Button(nome)) { Loop(); }
			}
		}
		
   }

};



class Janela:public UIobjeto 
{
public:
	    bool aberto;
		ImGuiWindowFlags flags;
		
		

	    std::vector<UIobjeto>objetos;



		void adicionarObj(UIobjeto l) {
			if (l.carregado == false) 
			{
			l.carregar(true, false); 
			objetos.push_back(l); }
		}

		void bandeiras(ImGuiWindowFlags flags) { this->flags = flags; }

		void apagar(std::vector<UIobjeto>::iterator itr, unsigned short op=(0)) { op == 1 ? objetos.erase(itr) : objetos.erase(itr); }

		void Run() {

			
			if (aberto) {

				ImGui::Begin(nome, &aberto, flags);
				if (carregado) {
					for ( unsigned short i = 0; objetos.size() > i; i++)
					
				 { 

						switch (objetos[i].tipo)
						{

						case BOTAO:
							botao* bot = (botao*)objetos[i].ptr;
							bot->loop();
							break;
						case INPUT:

							break;

						}
						
			
					
					
					}
				}
				else {
					carregado = true;
				//	for (int i = 0; Loads.size() > i; i++) { Loads[i](); }
				}
				ImGui::End();

			}
		}


};*/