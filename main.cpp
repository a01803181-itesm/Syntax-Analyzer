#include <iostream>
#include <fstream>   
#include <sstream>   
#include <string>
#include "lexer.hpp"
#include "parser.hpp"
#include "generadorHTML.hpp"

int main() {
    std::string nombre_archivo = "input_files/invalid_code.txt"; 

    std::ifstream archivo(nombre_archivo);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo '" << nombre_archivo << "'" << std::endl;
        return 1;
    }

    // Leemos todo el contenido del txt y lo vaciamos en codigo_fuente
    std::stringstream buffer;
    buffer << archivo.rdbuf();//.rdbuf() lee el contenido del archivo y lo almacena en el buffer
    std::string codigo_fuente = buffer.str();//.str() convierte el contenido del buffer en una cadena de texto (string)
    archivo.close();

    std::cout << "--- Analizando archivo: " << nombre_archivo << " ---" << std::endl;
    Lexer lx(codigo_fuente);
    Parser ps(lx);

    generarHTML(lx.getTokens(), ps.getErrors(), "analisis.html"); // Generamos el HTML con los tokens y errores encontrados

    return 0;
}