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
            base->data="GoodreadsResponse";
            Nodo* book = new Nodo("book");
            base->agregarHijo(book);
            pugi::xml_node root = doc->child("GoodreadsResponse").child("book");
            std::vector<string> etiquetasSimples = {"id", "title", "isbn", "publication_year", "language_code", "description", "average_rating", "num_pages"};
            for (string name : etiquetasSimples){
                book->agregarHijo(new Nodo(name));
            }
            for (Nodo* etiqueta : book->hijos){
                etiqueta->agregarHijo(new Nodo(root.child(etiqueta->data).text().as_string()));
            }
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
            if (ids == "Lista de IDs: "){
            cout << "Lista de libros está vacía!";     
            }
            else{
            cout << ids;   } 
            cout <<endl<<endl;
        }
        
        //borra el subarbol del nodo entregado
        void deleteSubtree(Nodo* node) {
        if (!node) return;   //no hacemos nada si el puntero es nulo
            for (auto child : node->hijos)
                deleteSubtree(child);   //se borra de forma recursiva los subarboles de los hijos
            delete node;
        }

        void agregarNodo(pugi::xml_document* doc, Nodo* base){
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
            //Paso 5: Agrega el nodo raiz del arbol creado a partir del XML como un hijo del nodo base del arbol general
            this->root.agregarHijo(base);
            return;
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
            
            //this->agregarNodo(&doc,&root);
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
        int i =0;   //contador para llevar cuenta de cuantos libros han sido borrados
        for (Nodo* child : root.hijos){  // revisamos los libros
            Nodo* dato=child->getHijo("average_rating");   //se obtiene el nodo que apunta al rating de este libro
            string rat2= ((dato->hijos)[0])->data;   //se obtiene el rating del libro
            float num= (rat2=="") ? 0 : std::stof(rat2);  //string se pasa a float con un default de 0 si no se encuentra el dato
            if (num<=rating){
                deleteSubtree(child); //si el rating del libro es mejor al parametro se elimina el libro (y por lo tanto su subarbol)
                i++;
                
            }  
        } 
        cout<<"Fueron borrados "<<i<<" libros con rating menor o igual a "<<rating<<endl;  
    }

    //metodo que indica que libros tienen libros similares con una publicación posterior
    void precursores(){
        string base_prec = "Libros que son precursores: ";
        for (Nodo* child : root.hijos){   //se recorren los libros
            Nodo* idNodo = child->getHijo("id");  //se obtiene nodo que apunta al id del libro
            if (!idNodo || idNodo->hijos.empty()) continue;  //si nodo es nulo o no apunta al id seguimos a otro libro
            Nodo* dato = child->getHijo("publication_year");  //obtenemos nodo a año de publicacion
            if (!dato || dato->hijos.empty()) continue; //revisamos otro libro si este no tiene año de publicación
            string str = dato->hijos[0]->data;  //string con año de publicación
            int year;
            if (!tryParseInt(str, year)) continue;
            Nodo* similares = child->getHijo("similar_books");  //obtenemos nodos que apuntan a libros similares
            if (!similares) continue; //si no hay libros similares nos saltamos a otro libro
            bool pastBookExists = false;  //booleano que indica si hay un libro que es predecesor de este
            for (Nodo* libro : similares->hijos){
                Nodo* dateNode = libro->getHijo("publication_year"); //obtenemos año de publicación de libros similares
                if (!dateNode || dateNode->hijos.empty()) continue;
                string dateStr = dateNode->hijos[0]->data;   
                int similar_year;
                if (!tryParseInt(dateStr, similar_year)) continue;
                if (similar_year <= year){  //chequeamos si el libro tiene predecesores
                    pastBookExists = true;
                    break;
                }
            }

            if (!pastBookExists){  //si libro no tiene predesesores significa que es precursor
                base_prec += idNodo->hijos[0]->data; //agregamos libro a lista de precursores
                base_prec += ", ";
            }
        }
        cout << base_prec << endl << endl;
    }
};

int main(){
    Tree arbol= Tree();
    cout<<"aaaaa"<<endl;
    arbol.listar();
    arbol.precursores();
    arbol.borrar_ratings(4);
    arbol.listar();
    arbol.borrar_ratings(10);
    arbol.listar();
    arbol.borrar_ratings(3);
    return 0;
}
