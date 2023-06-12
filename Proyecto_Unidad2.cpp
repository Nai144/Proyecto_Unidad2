#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <limits>
#include <unordered_map>
#include <string>
#include <list>
#include <algorithm>
using namespace std;

class Guardian {
public:
    Guardian(const string& nombre, int poder,const string& mainmaster, const string& villa)
        : Nombre(nombre),Poder(poder),MainMaster(mainmaster), Villa(villa)  {}

    string Nombre;
    int Poder;
    string MainMaster;
    string Villa;
    list<Guardian*> aprendices;
};

class Nodo {
public:
    Nodo(Guardian* guardian) : guardian(guardian) {}

    Guardian* guardian;
    vector<Nodo*> aprendices;
};

struct Aldea {
    string aldea;
    list<string> aldeasAdyacentes;
};

unordered_map<string, Aldea> leerArchivoCrearGrafo(const string& nombreArchivo);
void mostrarMapa(unordered_map<string, Aldea> mapa);
Nodo* crearNodo(Guardian* guardian);
void agregarHijo(Nodo* padre, Nodo* hijo);
void imprimirArbol(Nodo* nodo, int nivel = 0);
list<Guardian*> CargarLista(list<Guardian*> ListaG);
Nodo* buscarGuardian(Nodo* nodo, const string& nombre);
Nodo* listaToArbol(Nodo* raiz,list<Guardian*> ListaG);
void buscarMaestros(const list<Guardian*>& ListaG);
Guardian* crearPlayer(Nodo* raiz,list<Guardian*> ListaG);
void aldeasAdyacentes(unordered_map<string, Aldea> mapa,string NombreAldea);
Aldea* buscarAldea(unordered_map<string, Aldea>& mapa, const string& nombreAldea);
int GetRollResultAprendiz();
int GetRollResultMaestro();
int entrenar(list<Guardian*> ListaG,string whoEntrenar);
void buscarMaestrosVilla(const list<Guardian*>& ListaG,string villa);
void Alquimia(unordered_map<string, Aldea>& mapa,string aldeaDestino,string aldeaActual);

//Guardian* selectPlayer(Nodo* raiz,list<Guardian*> ListaG);

int main() {
	int end=0,gameOption=0;
	string aldeaActual="", aldeaViajar="", entrenador="";
	//lista para trabajar y comprobar datos de los guardianes
    list<Guardian*> listaGuardian;
	listaGuardian = CargarLista(listaGuardian);
	//mapa de las aldeas
	string nombreArchivo = "Aldeas.txt";
    unordered_map<string, Aldea> grafo = leerArchivoCrearGrafo(nombreArchivo);
	mostrarMapa(grafo);

    // Crear el árbol y agregar los personajes
    Nodo* raiz = nullptr;
    raiz = listaToArbol(raiz,listaGuardian);

	//creacion o seleccion del personaje	
	Guardian* jugador=nullptr;
	while(end !=1)
	{
		cout << "Para crear un personaje escriba [1] para seleccionarlo [2]" << endl;
		cin >> gameOption;
		if(gameOption==1)
		{
			cout << "Lista de maestros" << endl;
			buscarMaestros(listaGuardian);
			cout << endl;
			jugador = crearPlayer(raiz,listaGuardian);
			aldeaActual=jugador->Villa;	
			end=1;		
		}
		if(gameOption==2)
		{
			
		//	jugador = selectPlayer(raiz,listaGuardian);
			aldeaActual=jugador->Villa;	
			end=1;		
		}
		
	}
	end=0;
	gameOption=0;
	while(end !=1)
	{
		cout << "Selecccione una opcion" << endl;
		cout << "[1] Viajar" << endl;
		cout << "[2] Entrenar" << endl;
		cout << "[3] Alquimia" << endl;
		cout << "[4] Finalizar" << endl;
		cin >> gameOption;
		if (gameOption == 1)
		{
			gameOption=0;
    		//buscarAldea(grafo, aldeaActual); // Muestra la información de la aldea actual
    		aldeasAdyacentes(grafo,jugador->Villa);
    		cout << "A que villa deseas viajar?" << endl;
    		cin >> aldeaViajar;

    		Aldea* destino = buscarAldea(grafo, aldeaViajar);
    		if (destino != nullptr)
			{
        	// Comprueba si el destino es una aldea adyacente
        		auto iter = find(destino->aldeasAdyacentes.begin(), destino->aldeasAdyacentes.end(), aldeaActual);
        		if (iter != destino->aldeasAdyacentes.end())
				{
            	aldeaActual = aldeaViajar; // Actualiza la aldea actual del jugador
            	cout << "Viaje exitoso. Has llegado a la aldea " << aldeaActual << endl;
            	aldeasAdyacentes(grafo,jugador->Villa);
            	jugador->Villa=aldeaActual;
            	jugador->Poder++;
            	
        		} else
				{
            		cout << "La aldea " << aldeaViajar << " no es una aldea adyacente. No puedes viajar alli." << endl;
        		}
    		
			}else
			{
       			 cout << "La aldea " << aldeaViajar << " no existe. Verifica el nombre de la aldea e intenta nuevamente." << endl;
    		}	
		}
		if(gameOption==2)
		{
			//busca los maestros disponibles 
			buscarMaestrosVilla(listaGuardian, aldeaActual);
			cout << "Con quien deseas entrenar?" << endl;
    		cin >> entrenador;
    		jugador-> Poder+entrenar(listaGuardian, entrenador);
		}	
		if(gameOption==3)
		{
			//muestra las aldeas a donde puedes moverte
			aldeasAdyacentes(grafo,jugador->Villa);
			cout << "Hacia que aldea deseas crear un camino" << endl;
    		cin >> aldeaViajar;
    		
    		for(const auto& al : grafo)
    		{
    			if(al.first==aldeaViajar)
    			{
    				int random =rand()%1+2;
    				jugador->Poder= jugador->Poder-random;
    				Alquimia(grafo,aldeaViajar,aldeaActual);
				}
			}	
		}
		if(gameOption==4)
		{
			end=1;
		}
		
	}	
	imprimirArbol(raiz);		
			

    // Liberar memoria
    for (const auto& guardian : listaGuardian) {
    //	cout << "dlete "<<endl;
        delete guardian;
    }


    return 0;
}

Guardian* selectPlayer(Nodo* raiz, list<Guardian*> ListaG){
	
}

void Alquimia(unordered_map<string, Aldea>& mapa,string aldeaDestino, string aldeaActual){


    // Crear la unión entre la aldea actual y la aldea no adyacente
    mapa[aldeaActual].aldeasAdyacentes.push_back(aldeaActual);
    mapa[aldeaDestino].aldeasAdyacentes.push_back(aldeaDestino);

    cout << "Se ha creado una unión entre " << aldeaActual << " y " << aldeaDestino << "." << endl;
}

int entrenar(list<Guardian*> ListaG, string whoEntrenar){
	int resultaEntrenar=0;
	for (const auto& guardian : ListaG) {
		//si el personaje con el que netranremos tiene aprendices (Maestro)
        if (!guardian->aprendices.empty() &&  whoEntrenar == guardian->Nombre)
		{
            cout << "Has entrenado con el Maestro: " << guardian->Nombre << endl;
            resultaEntrenar = GetRollResultMaestro();
            return resultaEntrenar;
        }//si el personaje con el que netranremos no tiene aprendices (aprendiz)
        else if(guardian->aprendices.empty() &&  whoEntrenar == guardian->Nombre)
        {
        	cout << "Has entrenado con el Aprendiz: " << guardian->Nombre << endl;
        	resultaEntrenar = GetRollResultAprendiz();
        	return resultaEntrenar;
		}//si llegamos al ultimo personaje y este no es, nos dira que tenemos que escribir bien el nombre
		else if(ListaG.back()->Nombre == guardian->Nombre && whoEntrenar !=guardian->Nombre)
		{
			cout << "Nadie va a entrenar contigo sino escribes bien su nombre " << endl;
			resultaEntrenar=0;
			return resultaEntrenar;
		}
    }
    //return resultaEntrenar;
}
//busca los maestros en una villa y sus aprendices
void buscarMaestrosVilla(const list<Guardian*>& ListaG,string villa){
	    for (const auto& guardian : ListaG){
        if (guardian->Villa ==villa) {
        	cout << "El nombre del Maestro es: " << guardian->Nombre << endl;
        	for (const auto& aprendiz : guardian->aprendices)
        	{
        		cout << "El nombre del aprendiz es: " << aprendiz->Nombre << endl;
			}
            
        }
    }
}
//resultados de los dados dependiendo si es un aprendiz o un maestro
int GetRollResultAprendiz(){
	int random;
	random =rand()%6+1;
	if( 2 < random )
	{
		cout << "El entrenamiento ha sido exitoso " << endl;
		return 1;
	}
	return 0;
}
int GetRollResultMaestro(){
	int random;
	random =rand()%6+1;
	if( 4 < random )
	{
		cout << "El entrenamiento ha sido exitoso " << endl;
		return 2;
	}
	return 0;
}
//busca una aldea y la devuelve
Aldea* buscarAldea(unordered_map<string, Aldea>& mapa, const string& nombreAldea) {
    auto iter = mapa.find(nombreAldea);
    if (iter != mapa.end()) {
        return &(iter->second);
    }
    return nullptr;
}
//busca las aldeas adyacentes y las imprime
void aldeasAdyacentes(unordered_map<string, Aldea> mapa,string NombreAldea){
		for (const auto& par : mapa) {
			if(par.first == NombreAldea)
			{
				cout << "Aldea actual: " << par.first << endl;
        		cout << "Aldeas adyacentes: ";
        		for (const auto& aldeaAdyacente : par.second.aldeasAdyacentes) {
            		cout << aldeaAdyacente << " ";
        		}
        		cout << endl;
        		cout << "-------------------" << endl;
			}
        
    }
}
// crear un personaje
Guardian* crearPlayer(Nodo* raiz,list<Guardian*> ListaG){
	
	int correct=0;
	string nombrePlayer;
	int poderPlayer = 50;
	string villaIncial;
	string maestroInicial;
	Guardian* Player = nullptr;
	while(correct != 1)
	{
		cout << "Ingresa el Nombre de tu personaje" << endl;
		cin >> nombrePlayer;
		cout << "Ingresa el Nombre de tu maestro" << endl;
		cout << "Estos son los posibles maestros de los que puede ser aprendiz" << endl;
		buscarMaestros(ListaG);
		cin >> maestroInicial;
		Nodo* MaestroAux = buscarGuardian(raiz,maestroInicial);
		if (MaestroAux != nullptr)
		{
			Player =new Guardian(nombrePlayer,poderPlayer,maestroInicial,MaestroAux->guardian->Villa);
		//	ListaG.push_back(newGuardian);
			Nodo* newPlayer = crearNodo(Player);
           	agregarHijo(MaestroAux, newPlayer);
        //    MaestroAux->guardian->aprendices.push_back(Player); // Agregar aprendiz al maestro
          
            cout <<MaestroAux->guardian->Nombre<<endl;
            correct=1;
        }else
        {
        	cout << "Uno de los datos no fueron validos, por favor intentalo de nuevo" << endl;
		}
	
	}
	return Player;
	
}
//imprime los maestros y su ciudad inicial
void buscarMaestros(const list<Guardian*>& ListaG)
{
    for (const auto& guardian : ListaG) {
        if (!guardian->aprendices.empty() && guardian->MainMaster != "") {
            cout << "El nombre del Maestro es: " << guardian->Nombre <<", la ciudad inicial es "<< guardian->Villa<< endl;
        }
    }
}
//intento de convertir la lista a un arbol
Nodo* listaToArbol(Nodo* raiz,list<Guardian*> ListaG){
	 for (const auto& guardian : ListaG) {
        Nodo* nuevoNodo = crearNodo(guardian);

        if (raiz == nullptr) {
            raiz = nuevoNodo;

        } else {
        	// Agrega el nuevo nodo como hijo de la raíz (puedes modificar esta lógica según tus necesidades)
           	Nodo* MaestroAux = buscarGuardian(raiz,nuevoNodo->guardian->MainMaster);
           	/*
        	if(nuevoNodo->guardian->MainMaster == MaestroAux->guardian->Nombre)
        	{
        		agregarHijo(MaestroAux, nuevoNodo);	
			}
			*/
				if (MaestroAux != nullptr)
				{
                	agregarHijo(MaestroAux, nuevoNodo);
               		MaestroAux->guardian->aprendices.push_back(nuevoNodo->guardian); // Agregar aprendiz al maestro
            	}	  
        }
    }
	return raiz;
}
//imprime el mapa
void mostrarMapa(unordered_map<string, Aldea> mapa){
	   for (const auto& par : mapa) {
        cout << "Aldea: " << par.first << endl;
        cout << "Aldeas adyacentes: ";
        for (const auto& aldeaAdyacente : par.second.aldeasAdyacentes) {
            cout << aldeaAdyacente << " ";
        }
        cout << endl;
        cout << "-------------------" << endl;
    }
 } 
//grafo
unordered_map<string, Aldea> leerArchivoCrearGrafo(const string& nombreArchivo) {
    unordered_map<string, Aldea> grafo;

    ifstream archivo(nombreArchivo);
    if (!archivo) {
        cerr << "Error al abrir el archivo." << endl;
        return grafo;
    }

	
    string linea;
    getline(archivo, linea); // Skip the header line
    while (getline(archivo, linea)) {
        istringstream iss(linea);
        string aldea1, aldea2;
        if (getline(iss, aldea1, ',') && getline(iss, aldea2, ',')) {
            // Buscar o crear el nodo correspondiente a aldea1
            auto iter1 = grafo.find(aldea1);
            if (iter1 == grafo.end()) {
                Aldea nodo1;
                nodo1.aldea = aldea1;
                grafo[aldea1] = nodo1;
            }

            // Buscar o crear el nodo correspondiente a aldea2
            auto iter2 = grafo.find(aldea2);
            if (iter2 == grafo.end()) {
                Aldea nodo2;
                nodo2.aldea = aldea2;
                grafo[aldea2] = nodo2;
            }

            // Agregar la conexión entre aldea1 y aldea2
            grafo[aldea1].aldeasAdyacentes.push_back(aldea2);
            grafo[aldea2].aldeasAdyacentes.push_back(aldea1);
        }
    }

    archivo.close();

    return grafo;
}
//busqueda de un guardian 
Nodo* buscarGuardian(Nodo* nodo, const string& nombre) {
    if (nodo == nullptr) {
        return nullptr;
    }

    if (nodo->guardian->Nombre == nombre) {
    //	cout << "El nombre del Maestro es: " << nodo->guardian->Nombre <<", la ciudad inicial es "<< nodo->guardian->Villa<< endl;
        return nodo;
    }
//	if(nodo->hijos != nullptr)
    for (const auto& aprendiz : nodo->aprendices) {
        Nodo* resultado = buscarGuardian(aprendiz, nombre);
        if (resultado != nullptr) {
            return resultado;
        }
    }

    return nullptr;
}

void imprimirArbol(Nodo* nodo, int nivel){
    if (nodo == nullptr) {
    //	cout << "No funciono "<<endl;
        return;
    }

    for (int i = 0; i < nivel; ++i) {
        cout << "  ";
    }

 //   cout << "Nombre: " << nodo->guardian->Nombre << ", Villa: " << nodo->guardian->Villa << ", Poder: " << nodo->guardian->Poder << endl;

    for (const auto& aprendiz : nodo->aprendices) {
        imprimirArbol(aprendiz, nivel + 1);
    }
}
void agregarHijo(Nodo* padre, Nodo* aprendiz) {
    padre->aprendices.push_back(aprendiz);
}

Nodo* crearNodo(Guardian* guardian) {
	//cout << "se ha añadido con exito "<<endl;
    Nodo* nodo = new Nodo(guardian);
    return nodo;
}

list<Guardian*> CargarLista(list<Guardian*> ListaG)
{

	//Leer el archivo
	int i=1;
	string filename = "Guardians2.txt";
	ifstream file(filename);
	if (file.is_open()) {
		string line;
		getline(file, line); // Skip the header line
		while (getline(file, line)) {
			
			stringstream ss(line);
			string NameCharacter, MainMasterCharacter, VillageCharacter;
			string PowerCharacter;
			int PwC;
			getline(ss, NameCharacter, ',');
			getline(ss, PowerCharacter, ',');
			getline(ss, MainMasterCharacter, ',');
			getline(ss, VillageCharacter, ',');
			PwC = stoi(PowerCharacter);
			Guardian* newGuardian =new Guardian(NameCharacter,PwC,MainMasterCharacter,VillageCharacter);
			ListaG.push_back(newGuardian);
			i++;
		}
		file.close();
		return ListaG;
	}
	else {
	cout << "Failed to open file: " << filename << endl;
	return ListaG;
	}
	
}
