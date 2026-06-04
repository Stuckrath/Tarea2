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

class Nodo{
    public:
        string data;
        std::vector<Nodo*> hijos;
        Nodo(string d){
            data = d;
        }
        Nodo(){
            data = "";
        }
        ~Nodo(){
            for (Nodo* hijo : hijos){
                delete hijo;
            }
        }
        void agregarHijo(Nodo* ref){
            hijos.push_back(ref);
        }

        Nodo* getHijo(const string& dato){
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

class Tree{
    private:
        Nodo root= Nodo();

        void preorder(Nodo* base) {
            std::stack<Nodo*> auxStruct;
            auxStruct.push(base);
            string ids = "Lista de IDs: ";
            while (!auxStruct.empty()){
                Nodo* nodo = auxStruct.top();
                auxStruct.pop();
                if (nodo->data=="id"){
                    string id_singular = nodo->hijos.front()->data;
                    ids+=id_singular;
                    ids+=", ";
                }
                for (Nodo* hijo : nodo->hijos){
                    auxStruct.push(hijo);
                }
            }
            cout << ids;
            cout <<endl;
        }
        
        void deleteSubtree(Nodo* node) {
        if (!node) return;
            for (auto child : node->hijos)
                deleteSubtree(child);
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
        LibroCreator lb = LibroCreator();
        for (const auto & entry : fs::directory_iterator(c)){
            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load_file(entry.path().c_str());
            if (!result)
            break;
            
            //this->agregarNodo(&doc,&root);
            lb.CrearNodo(&doc,&root);
            i++;
            if (i%250==0) {
                cout<<"Archivos procesados: "<<i<<endl;
            }
        
        }
}


     void listar(){
            preorder(&root);
        }

    void borrar_ratings(float rating){
        for (Nodo* child : root.hijos){
            Nodo* dato = child->getHijo("average_rating");
            if (!dato || dato->hijos.empty()) continue;
            string rat2 = dato->hijos[0]->data;
            float num;
            if (!tryParseFloat(rat2, num)) continue;
            if (num <= rating){
                deleteSubtree(child);
            }
        }
    }

    void precursores(){
        string base_prec = "Libros que son precursores: ";
        for (Nodo* child : root.hijos){
            Nodo* idNodo = child->getHijo("id");
            if (!idNodo || idNodo->hijos.empty()) continue;
            Nodo* dato = child->getHijo("publication_year");
            if (!dato || dato->hijos.empty()) continue;
            string str = dato->hijos[0]->data;
            int year;
            if (!tryParseInt(str, year)) continue;
            Nodo* similares = child->getHijo("similar_books");
            if (!similares) continue;
            bool pastBookExists = false;
            for (Nodo* libro : similares->hijos){
                Nodo* dateNode = libro->getHijo("publication_year");
                if (!dateNode || dateNode->hijos.empty()) continue;
                string dateStr = dateNode->hijos[0]->data;
                int similar_year;
                if (!tryParseInt(dateStr, similar_year)) continue;
                if (similar_year <= year){
                    pastBookExists = true;
                    break;
                }
            }

            if (!pastBookExists){
                base_prec += idNodo->hijos[0]->data;
                base_prec += ", ";
            }
        }
        cout << base_prec << endl;
    }
};

int main(){
    Tree arbol= Tree();
    cout<<"aaaaa"<<endl;
    arbol.listar();
    arbol.precursores();

}
