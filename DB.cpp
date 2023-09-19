
#include <mysql.h>

#define SEPARADOR_COM 20

#define SEPARADOS  std::vector<std::vector<char>>

#define DBDG(str) DB_DEBUG.push_back(str);


std::vector<std::string>DB_DEBUG;


std::vector<std::vector<char>> separador(const char* bytes, char separador = (','))

{
	std::vector< std::vector<char>>parametros;
	std::vector<char>temp;


	for (int i = 0; bytes[i] != NULL; i++)
	{

		if (bytes[i] != separador) {
			temp.push_back(bytes[i]);
		}
		else {
			temp.push_back(NULL);
			parametros.push_back(temp);
			temp.clear();
			continue;
		}
		

	}
	temp.push_back(NULL);
	parametros.push_back(temp);
	return parametros;

}

MYSQL_BIND ligacao(const char* link)
{
	MYSQL_BIND bind;
	memset(&bind, 0, sizeof(bind));
	bind.buffer = (void*)link;
	bind.buffer_length = strlen(link);
	bind.buffer_type = MYSQL_TYPE_STRING;

	return bind;
}

MYSQL* DB_INIT()
{
	MYSQL* conn;

	if (mysql_query(conn,"select * from admins"))
	{
		
		mysql_close(conn);
		exit(0);

	}


	//MYSQL_RES* result = mysql_store_result(conn);
	

	return conn;
     

}


void DbQuery( const char * query,bool * car,char *data){

	if (*car)
	//	return;

	*car = true;

	MYSQL* conn = mysql_init(NULL);

	if (mysql_real_connect(conn, "localhost", "root", "admin", "praecdb", 3306, nullptr,0) == NULL) {
		mysql_close(conn);
		exit(1);

	}

	
	if (mysql_query(conn,query))
	{
		strcpy(data,"falha");
	
		//mysql_close(conn);
		//exit(0);

	}


	MYSQL_RES* result = mysql_store_result(conn);

	
	
	if (result == nullptr) {
		mysql_free_result(result);
	   mysql_close(conn);
		return;
	}
	MYSQL_ROW row = mysql_fetch_row(result);
	strcpy(data,row[1]);

	mysql_free_result(result);

	
	mysql_close(conn);

}



MYSQL_RES * DbSelectALL(char * table,char* data=(nullptr)) {


	MYSQL* conn = mysql_init(NULL);

	if (mysql_real_connect(conn, "localhost", "root", "admin", "praecdb", 3306, nullptr, 0) == NULL) {
		mysql_close(conn);
		exit(1);

	}


	if (mysql_query(conn, std::string("select * from ").append(table).c_str()))
	{   
		if(data !=nullptr)
		strcpy(data, "falha");

		//mysql_close(conn);
		//exit(0);

	}


	MYSQL_RES* result = mysql_store_result(conn);



	if (result == nullptr) {
		mysql_free_result(result);
		mysql_close(conn);
		return nullptr;
	}

	mysql_close(conn);

	return result;

}

void DbInserir(const char* table,const char * campos,char * valores,char* data = (nullptr)) {

	 SEPARADOS vals= separador(valores);
	 SEPARADOS camps = separador(campos);
	 if (vals.size() != camps.size())
		 return;

	std::string query = std::string("INSERT INTO ").append(table).append("(").append(campos).append(") VALUES(?");
	for (int i = 0; i < vals.size()-1; i++) query.append(",?");

	query.append(")");


	MYSQL* conn = mysql_init(NULL);

	if (mysql_real_connect(conn, "localhost", "root", "admin", "praecdb", 3306, nullptr, 0) == NULL) {
		mysql_close(conn);
		exit(1);

	}
	DBDG(query)
		
	
	MYSQL_STMT* stmt;
	
	if ((stmt = mysql_stmt_init(conn))==nullptr) { exit(2); };

	if (mysql_stmt_prepare(stmt, query.c_str(), strlen(query.c_str())) != 0) { exit(-1); };
	

	
	MYSQL_BIND *bind= new MYSQL_BIND[vals.size()];
	
	
	
	for (int i = 0; i < vals.size();i++){
		bind[i] = ligacao(vals[i].data());
	}


	if (mysql_stmt_bind_param(stmt, bind) != 0) { exit(1); };
	
	if (mysql_stmt_execute(stmt) ==2) { exit(2); };
	

	if (mysql_stmt_close(stmt)) { exit(9); };


	mysql_close(conn);
	delete[] bind;
	return ;

}

void DbUpdate(const char * table, const char * campos_updt, const char *valores_updt, const char * condicão,const char * cond_vals)
{

	SEPARADOS camps = separador(campos_updt);
	SEPARADOS vals = separador(valores_updt);
	SEPARADOS conds = separador(condicão);
	SEPARADOS conds_vals = separador(cond_vals);

	if (vals.size() != camps.size())
		return;


	std::string query = std::string("UPDATE ").append(table).append(" SET ");

	for (int i = 0; i < vals.size(); i++) { query.append(camps[i].data()).append("=").append("?").append(" "); }

	query.append(" WHERE ");
	 
	query.append(conds[0].data()).append(" = ").append("?");
	for (int i = 1; i < conds.size(); i++) { query.append(" AND ").append(conds[i].data()).append(" = ").append("?"); }

	query.append(";");

	MYSQL* conn = mysql_init(NULL);

	if (mysql_real_connect(conn, "localhost", "root", "admin", "praecdb", 3306, nullptr, 0) == NULL) {
		mysql_close(conn);
		exit(1);

	}
	DBDG(query)
		DBDG(std::string(std::to_string(conds_vals.size())))

		if (mysql_query(conn, "SET SQL_SAFE_UPDATES = 0") != 0);
	
		MYSQL_BIND* bind = new MYSQL_BIND[vals.size()+conds_vals.size()];

	memset(bind, 0, sizeof(bind));

	for (int i = 0; i < vals.size(); i++) {
		bind[i] = ligacao(vals[i].data());
	}

	for (int i =0; i < conds_vals.size(); i++) {
	bind[i+vals.size()] = ligacao(conds_vals[i].data());
	}

	MYSQL_STMT* stmt;

	if ((stmt = mysql_stmt_init(conn)) == nullptr) { exit(2); };

	if (mysql_stmt_prepare(stmt, query.c_str(), strlen(query.c_str())) != 0) { exit(10); };


	if (mysql_stmt_bind_param(stmt, bind) != 0) { exit(1); };

	if (mysql_stmt_execute(stmt) !=0) { DBDG(mysql_stmt_error(stmt)) };


	if (mysql_stmt_close(stmt)) { exit(9); };


	mysql_close(conn);

	delete[] bind;
	return;

}


void Dbtest() {




	MYSQL* conn = mysql_init(NULL);

	if (mysql_real_connect(conn, "localhost", "root", "admin", "praecdb", 3306, nullptr, 0) == NULL) {
		mysql_close(conn);
		exit(1);

	}

	mysql_query(conn,"INSERT INTO usuarios (nome, senha) VALUES ('caio', '1234')");
	mysql_close(conn);

	return;

}

void fecharDb(MYSQL* conn) {

	mysql_close(conn);
}




