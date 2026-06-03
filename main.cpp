#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include "lib/pugixml.hpp"
#include <string>
#include <filesystem>
// Explicitly naming scope prevents global namespace pollution
using std::cout;
using std::endl;
using std::string;


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
};


/// @brief  Clase que contiene toda la informacion sobre el libro que se escanea en el archivo XML
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

            std::stack<Nodo*> testeo;
            testeo.push(base);
            while (!testeo.empty()){
                Nodo* auxNode = testeo.top();
                testeo.pop();
                cout<<auxNode->data<<endl;
                for (Nodo* n: auxNode->hijos) testeo.push(n);
            }
            return;
        }
};




int main() {
    bool debugging = true;
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
            return -1;
        i++;
        Nodo node = Nodo(); 
        lb.CrearNodo(&doc,&node);
        if(debugging){
            std::stack<Nodo*> testeo;
            testeo.push(&node);
            while (!testeo.empty()){
                Nodo* auxNode = testeo.top();
                testeo.pop();
                cout<<auxNode->data<<endl;
                for (Nodo* n: auxNode->hijos) testeo.push(n);
            }
        }
        return 1;
    }
    return 0;
}