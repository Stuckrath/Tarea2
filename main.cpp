#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include "lib/pugixml.hpp"
#include <filesystem>
// Explicitly naming scope prevents global namespace pollution
using std::cout;
using std::endl;
using std::string;



#include <vector>
#include <iostream>

class Nodo{   //nodos en los que se basa el árbol
    public:
        string data;   
        std::vector<Nodo*> hijos;
        Nodo(string d){
            data = d;
        }
        Nodo(){
            data = "";  //valor por default
        }
        ~Nodo(){
            for (Nodo* hijo : hijos){
                delete hijo;
            }
        }
        void agregarHijo(Nodo* ref){  //agrega hijo al vector
            hijos.push_back(ref);
        }

        Nodo* getHijo(const string& dato){  //retorna hijo con el contenido pedido
            for (Nodo* child : hijos){
                if (child->data == dato){
                    return child;
                }
            }
            return nullptr;
        }

};


/// @brief  Clase que contiene toda la informacion sobre el libro que se escanea en el archivo XML
bool tryParseInt(const string& s, int& out){
    if (s.empty()) return false;
    try {
        size_t idx;
        int value = std::stoi(s, &idx);
        if (idx != s.size()) return false;
        out = value;
        return true;
    }
    catch (...) {
        return false;
    }
}

bool tryParseFloat(const string& s, float& out){
    if (s.empty()) return false;
    try {
        size_t idx;
        float value = std::stof(s, &idx);
        if (idx != s.size()) return false;
        out = value;
        return true;
    }
    catch (...) {
        return false;
    }
}

class LibroCreator{
    public:
        /// @brief  Utiliza libreria pugixml para acceder al nodo de libro y a partir de ahi sacar toda la información relevante para el arbol
        /// @param doc 
        void CrearNodo(pugi::xml_document* doc, Nodo* base){
            //Paso 1: Crea la estructura base del arbol a partir de la estructura compartida de los XMLs
            //Todos tienen un nodo base de la tag "GoodreadsResponse" que tiene un nodo hijo de la etiqueta "book"
            base->data="GoodreadsResponse";
            Nodo* book = new Nodo("book");
            base->agregarHijo(book);
            //Paso 2: Crea todos los nodos que van a contener información singular sobre el libro
            //Debido a que cada nodo solo puede contener una string de información, la info correspondiente a la etiqueta se
            //guardara como un nodo hijo al nodo de etiqueta correspondiente
            std::vector<string> etiquetasSimples = {"id", "title", "isbn", "publication_year", "language_code", "description", "average_rating", "num_pages"};
            for (string name : etiquetasSimples){
                book->agregarHijo(new Nodo(name));
            }
            //Paso 3: Extraer la informacion correspondiente usando el xml_node de pugixml, guardarla en un nodo y asignarla como
            //hijo del nodo "etiqueta" correspondiente
            pugi::xml_node root = doc->child("GoodreadsResponse").child("book");
            for (Nodo* etiqueta : book->hijos){
                etiqueta->agregarHijo(new Nodo(root.child(etiqueta->data).text().as_string()));
            }

            //Paso 4: Repetimos el paso 2 y 3, pero ahora con una raiz distinta, de modo que creemos nodos que contienen
            //la informacion de los libros similares al libro original
            Nodo* LibrosSimilares = new Nodo("similar_books");
            book->agregarHijo(LibrosSimilares);
            pugi::xml_node similares_root = root.child("similar_books");
            std::vector<string> etiquetasSimilares = {"title", "isbn", "publication_year"};

            for (pugi::xml_node node_similar : similares_root.children()){
                Nodo* singular_similar_book = new Nodo("book");
                for (string name : etiquetasSimilares) {
                    singular_similar_book->agregarHijo(new Nodo(name));
                }
                for (Nodo* etiqueta : singular_similar_book->hijos){
                    etiqueta->agregarHijo(new Nodo(node_similar.child(etiqueta->data).text().as_string()));
                }
                LibrosSimilares->agregarHijo(singular_similar_book);
            }
            return;
        }
};


//arbol implementado con los nodos definidos anteriormente
class Tree{
    private:
        Nodo root= Nodo();    //raiz del arbol

        //implementación éstandar de recorrido preorder
        void preorder(Nodo* base) {
            std::stack<Nodo*> auxStruct;
            auxStruct.push(base);  //ocupamos un stack auxiliar para ir recorriendo el arbol 
            string ids = "Lista de IDs: ";
            while (!auxStruct.empty()){   
                Nodo* nodo = auxStruct.top();
                auxStruct.pop();
                if (nodo->data=="id"){   //si nos encontramos con un nodo "id" revisamos su hijo por el valor
                    string id_singular = nodo->hijos.front()->data;
                    ids+=id_singular;  //agregamos el id a la lista de ids recorridos
                    ids+=", ";
                }
                for (Nodo* hijo : nodo->hijos){   //agregamos los hijos del nodo actual
                    auxStruct.push(hijo);
                }
            }
            cout << ids;    
            cout <<endl<<endl;
        }
        
        //borra el subarbol del nodo entregado
        void deleteSubtree(Nodo* node) {
            if (!node) return;
            delete node;
        }

        
    public:
       Tree(){
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(NULL);
        //Lectura de Archivos XML, deben estar guardados en una carpeta llamada "XMLs" junto al ejecutable
        namespace fs = std::filesystem;
        string c = "XMLs/";
        int i = 0;
        LibroCreator lb = LibroCreator();   //creamos creador de libros para que sea el encargado de poblar el árbol
        for (const auto & entry : fs::directory_iterator(c)){
            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load_file(entry.path().c_str());
            if (!result)
            break;
            
            lb.CrearNodo(&doc,&root);
            i++;
            if (i%250==0) {   //indicamos progreso
                cout<<"Archivos procesados: "<<i<<endl;
            }
        
        }
}


     void listar(){   //recorremos el arbol desde la raíz ocupando el algoritmo preorder definido anteriormente
            preorder(&root);
        }

        //método para borrar libros con un rating menor o igual al parametro
    void borrar_ratings(float rating){
        if (root.hijos.empty()){  //avisamos y retornamos si no hay libros que filtrar
            cout<<"El arbol ya está vacío, no se pueden borrar más libros"<<endl;
            return;
        }
        int removed = 0;
        //Para cada nodo del arbol
        for (size_t idx = 0; idx < root.hijos.size(); ){
            //1) Revisamos que sea un nodo valido
            Nodo* child = root.hijos[idx];
            if (!child){
                idx++;
                continue;
            }
            //2) Revisamos si es que tiene un nodo de "average_rating" valido
            Nodo* dato = child->getHijo("average_rating");
            if (!dato || dato->hijos.empty()){
                idx++;
                continue;
            }
            //3) Transformamos el valor guardado en string a un float
            string rat2 = dato->hijos[0]->data;
            float num = 0.0f;
            if (!tryParseFloat(rat2, num)){
                num = 0.0f;
            }
            //4) Revisamos si es que este libro tiene menor rating, y si ese es el caso borra el nodo (y por como está definido Nodo,
            //también todos sus hijos)
            if (num <= rating){
                root.hijos.erase(root.hijos.begin() + idx);
                deleteSubtree(child);
                removed++;
            } else {
                idx++;
            }
        }
        cout<<"Fueron borrados "<<removed<<" libros con rating menor o igual a "<<rating<<endl;
    }

    //metodo que indica que libros tienen libros similares con una publicación posterior
    void precursores(){
        cout << "Procesando precursores en " << root.hijos.size() << " libros..." << endl;
        string resultado = "Libros que son precursores: ";
        for (Nodo* child : root.hijos){
            if (!child) continue;
            Nodo* idNodo = child->getHijo("id");
            if (!idNodo || idNodo->hijos.empty()) continue;
            Nodo* dato = child->getHijo("publication_year");
            if (!dato || dato->hijos.empty()) continue;
            string str = dato->hijos[0]->data;
            int year;
            if (!tryParseInt(str, year)) continue;
            Nodo* similares = child->getHijo("similar_books");
            if (!similares){
                resultado += idNodo->hijos[0]->data;
                resultado += ", ";
                continue;
            }
            bool pastBookExists = false;
            for (Nodo* libro : similares->hijos){
                if (!libro) continue;
                Nodo* dateNode = libro->getHijo("publication_year");
                if (!dateNode || dateNode->hijos.empty()) continue;
                string dateStr = dateNode->hijos[0]->data;   
                int similar_year;
                if (!tryParseInt(dateStr, similar_year)) continue;
                if (similar_year <= year){  //chequeamos si el libro tiene predecesores
                    pastBookExists = true;
                    break;
                }
            }

            if (!pastBookExists){
                resultado += idNodo->hijos[0]->data;
                resultado += ", ";
            }
        }
        cout << resultado << endl;
        return;
    }
};

int main(){
    Tree arbol= Tree();
    arbol.listar();
    arbol.precursores();
    arbol.borrar_ratings(4);
    arbol.listar();
    arbol.borrar_ratings(10);
    arbol.listar();
    arbol.borrar_ratings(3);
    return 0;
}
