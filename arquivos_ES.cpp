#include <fstream>
#include <vector>



class Armazenamento {

	

public:
	Armazenamento(const char * arquivo) {

		if (!arq.is_open()) {
			arq = std::fstream(arquivo);
		}
	}

	std::fstream arq;




	bool ler_arquivo(std::vector<char>& data) {
		size_t bfs = 0;

		if (arq.is_open()) {
			arq.seekg(0,std::fstream::end);
			bfs = (size_t)arq.tellg();

			data.resize(bfs);
			arq.seekg(0);
			arq.read(data.data(), bfs);
			

			arq.close();
			return true;
		}

		return false;
	}

	bool ler_arquivo(std::vector<char>& data,size_t inicio,size_t fim) {

		if (arq.is_open()) {
			data.resize(fim-inicio);
			arq.seekg(inicio);
			arq.read(data.data(),fim-inicio);

			return true;
		}

		return false;
	}


	bool gravar(const char * buff){


		
		if (arq.is_open()) {
		arq.write(buff,strlen(buff));
		arq.flush();
	
		return true;
		}
		return false;


	}

	bool excluir() {
	
		if (arq.is_open()) {
			arq.close();
			arq.open("tex.txt.txt",std::fstream::trunc);			
			
			return true;
		}
		return false;
	
	
	}

	bool fechar() {
	
		if (arq.is_open()) {

			arq.close();
			return true;
		}
		return false;
	
	}




};