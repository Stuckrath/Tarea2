#include <iostream>
#include <string>
#include <vector>
#include "lib/pugixml.hpp"
#include <string>
// Explicitly naming scope prevents global namespace pollution
using std::cout;
using std::endl;
using std::string;
/// @brief Clase secundaria que será contenida en la clase de Libro, proviene de leer la información en la etiqueta de <similar_works>
class LibroSimilar{
    public:
        int id;
        int isbn;
        string titulo;
        LibroSimilar(int i, int is, string title){
            id = i;
            isbn = is;
            titulo = title;
        }
        /// @brief Funcion de Debug, para revisar que tenga correctamente ingresados los datos leidos por la clase Libro del archivo XML
        void check(){
            cout << "Checkeo de LibroSimilar" << endl;
            cout << "ID: " << id << endl;
            cout << "Titulo: " << titulo << endl;
            cout << "ISBN: " << isbn << endl;
        }
};
/// @brief  Clase que contiene toda la informacion sobre el libro que se escanea en el archivo XML
class Libro{
    public:
        int id; //id
        std::string titulo; //title *
        int isbn; //isbn
        int year; //publication_year
        std::string idioma; //language_code
        std::string desc; //description *
        float rating; // average_rating
        int pageNum; // num_pages *
        std::vector<LibroSimilar> similares = {};
        /// @brief  Utiliza libreria pugixml para acceder al nodo de libro y a partir de ahi sacar toda la información relevante para el arbol
        /// @param doc 
        Libro(pugi::xml_document* doc){
            pugi::xml_node root = doc->child("GoodreadsResponse").child("book");
            id = root.child("id").text().as_int();
            titulo = root.child("title").text().as_string();
            isbn = root.child("isbn").text().as_int();
            year = root.child("publication_year").text().as_int();
            idioma = root.child("language_code").text().as_string();
            desc = root.child("description").text().as_string();
            rating = root.child("average_rating").text().as_float();
            pageNum = root.child("num_pages").text().as_int();
            pugi::xml_node similares_root = root.child("similar_books");
            for (pugi::xml_node node_similar : similares_root.children()){
                int s_id = node_similar.child("id").text().as_int();
                string s_title = node_similar.child("title").text().as_string();
                int s_isbn = node_similar.child("isbn").text().as_int();
                LibroSimilar ls = LibroSimilar(s_id, s_isbn, s_title);
                similares.push_back(ls);
            }
        }
        /// @brief  Funcion de Debug, para revisar que tenga correctamente ingresados los datos leidos en el archivo XML
        void check(){
            cout << "Checkeo de LibroCompleto" << endl;
            cout << "ID: " << id << endl;
            cout << "Titulo: " << titulo << endl;
            cout << "ISBN: " << isbn << endl;
            cout << "Year: " << year << endl;
            cout << "Idioma: " << idioma << endl;
            cout << "Descripcion: " << desc << endl;
            cout << "Rating: " << rating << endl;
            cout << "Numero de Paginas: " << pageNum << endl;
            for (LibroSimilar ls : similares){
                ls.check();
            }
        }
};




int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    //Lectura de Archivos XML
    string c = "XMLs/1.xml";
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(c.c_str());
    if (!result)
        return -1;
    Libro l = Libro(&doc);
    l.check();
}