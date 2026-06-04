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

        Nodo* getHijo(string dato){
            for (Nodo* child: hijos){
                if (child->data==dato){
                    return child;
                }
            }
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

            return;
        }
};

/*
class Tree{
    private:
        Nodo root= Nodo();
       
        void preorder(Nodo* nodo) {
            if (!nodo)
                return;
            if (nodo->data=="id"){
                Nodo* n_id=(nodo->hijos)[0];
                cout<<n_id->data<<endl;
            }
            for (Nodo* child : nodo->hijos)
                preorder(child);
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

            /*std::stack<Nodo*> testeo;
            testeo.push(base);
            while (!testeo.empty()){
                Nodo* auxNode = testeo.top();
                testeo.pop();
                cout<<auxNode->data<<endl;
                for (Nodo* n: auxNode->hijos) testeo.push(n);
            }
            return;*/
        }
        
    public:
     void listar(){
            preorder(&root);
        }

    void borrar_ratings(float rating){
        for (Nodo* child : root.hijos){
            Nodo* dato=child->getHijo("average_rating");
            string rat2= ((dato->hijos)[0])->data;
            float num= std::stof(rat2);
            if (num<=rating){
                deleteSubtree(child);
            }  
        }                  
    }

    void precursores(){
        for (Nodo* child : root.hijos){
            Nodo* dato=child->getHijo( "publication_year");
            string str= ((dato->hijos)[0])->data;
            int año= std::stoi(str);
            Nodo* similares=child->getHijo("similar_books");
            int i=0;
            for (Nodo* libro : similares->hijos){
                Nodo* date=libro->getHijo( "publication_year");
                int añoS= std::stoi( ((date->hijos)[0])->data);
                if (añoS>año){
                    i++;
                }    
            }
            if (i==(similares->hijos).size()){
            cout<<(((child->getHijo("id")))->hijos[0])->data<<endl;}
        }  
    }

    int crear(){
        namespace fs = std::filesystem;
        string c = "XMLs/";
        int i = 0;
        for (const auto & entry : fs::directory_iterator(c)){
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(entry.path().c_str());
        if (!result)
            return -1;
        Nodo* nodoBase = new Nodo();
        this->agregarNodo(&doc, nodoBase);
        i++;
        if (i%100==0) {
            cout<<"Archivos procesados: "<<i<<endl;
            std::stack<Nodo*> testeo;
            testeo.push(nodoBase);
            while (!testeo.empty()){
                Nodo* auxNode = testeo.top();
                testeo.pop();
                cout<<auxNode->data<<" | ";
                for (Nodo* n: auxNode->hijos) testeo.push(n);
                if (auxNode->hijos.empty())cout<<endl;
            }
            cout <<endl;
        }
    }
    return 1;
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
                cout<<auxNode->data<<" | ";
                for (Nodo* n: auxNode->hijos) testeo.push(n);
                if (auxNode->hijos.empty())cout<<endl;
            }
            cout <<endl;
        }
        if (i>=5) return 1;
    }
    return 0;
}

*/

class Tree{
    private:
        Nodo root= Nodo();


          void preorder(Nodo* nodo) {
            if (!nodo)
                return;
            if (nodo->data=="id"){
                Nodo* n_id=(nodo->hijos)[0];
                cout<<n_id->data<<endl;
            }
            for (Nodo* child : nodo->hijos)
                preorder(child);
        }
        
        void deleteSubtree(Nodo* node) {
        if (!node) return;
            for (auto child : node->hijos)
                deleteSubtree(child);
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
    LibroCreator lb = LibroCreator();
    for (const auto & entry : fs::directory_iterator(c)){
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(entry.path().c_str());
        if (!result)
           break;
        i++;
      
        lb.CrearNodo(&doc,&root);
       
    }
}


     void listar(){
            preorder(&root);
        }

    void borrar_ratings(float rating){
        for (Nodo* child : root.hijos){
            Nodo* dato=child->getHijo("average_rating");
            string rat2= ((dato->hijos)[0])->data;
            float num= std::stof(rat2);
            if (num<=rating){
                deleteSubtree(child);
            }  
        }                  
    }

    void precursores(){
        for (Nodo* child : root.hijos){
            Nodo* dato=child->getHijo( "publication_year");
            string str= ((dato->hijos)[0])->data;
            int año= std::stoi(str);
            Nodo* similares=child->getHijo("similar_books");
            int i=0;
            for (Nodo* libro : similares->hijos){
                Nodo* date=libro->getHijo( "publication_year");
               int añoS = (date->hijos[0])->data.empty() ? 0 : std::stoi((date->hijos[0])->data);
                if (añoS>año){
                    i++;
                }    
            }
            if (i==(similares->hijos).size()){
            cout<<(((child->getHijo("id")))->hijos[0])->data<<endl;}
            else {
                cout<<"No hay precursores"<<endl;
            }
        }  
    }
};

int main(){
    Tree arbol= Tree();
    cout<<"aaaaa"<<endl;
    arbol.listar();
    arbol.precursores();

}
