#ifndef GENERADORHTML_HPP
#define GENERADORHTML_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "tokens.hpp"
// Función para ignorar los caracteres especiales en HTML
std::string ignorarCaracteresHTML(const std::string& texto) {
    std::string resultado;
    resultado.reserve(texto.size());
    for (char c : texto) {
        switch (c) {
            case '&': resultado += "&amp;"; break;
            case '<': resultado += "&lt;"; break;
            case '>': resultado += "&gt;"; break;
            case '"': resultado += "&quot;"; break;
            case '\'': resultado += "&apos;"; break;
            default: resultado += c; break;
        }
    }
    return resultado;
}
// Función para obtener la clase CSS según el tipo de token

std::string obtenerClaseCSS(TokenType tipo) {
    switch(tipo){
        case TokenType::KEYWORD_VAR:
        case TokenType::KEYWORD_IF:
        case TokenType::KEYWORD_ELSE:
        case TokenType::KEYWORD_WHILE:
        case TokenType::KEYWORD_FUNCTION:
        case TokenType::KEYWORD_RETURN:
        case TokenType::KEYWORD_TRUE:
        case TokenType::KEYWORD_FALSE:
            return "keyword";

        case TokenType::IDENTIFIER:
            return "identifier";

        case TokenType::VAL_NUM:
            return "number";

        case TokenType::OP_SUM:
        case TokenType::OP_SUBT:
        case TokenType::OP_MULT:
        case TokenType::OP_DIV:
        case TokenType::OP_EQUALS:
            return "operator";

        case TokenType::OP_IS_EQUAL:
        case TokenType::OP_IS_DIFFERENT:
        case TokenType::OP_LESS_THAN:
        case TokenType::OP_GREATER_THAN:
        case TokenType::OP_LESS_THAN_EQUAL:
        case TokenType::OP_GREATER_THAN_EQUAL:
            return "comparison";

        case TokenType::LEFT_PAREN:
        case TokenType::RIGHT_PAREN:
        case TokenType::LEFT_BRACE:
        case TokenType::RIGHT_BRACE:
        case TokenType::SEMICOLON:
        case TokenType::COMMA:
            return "punctuation";

        case TokenType::UNKNOWN:
            return "unknown"; 

        default:
            return ""; 

    }
}

void generarCSS(){
    std::ofstream cssFile("styles.css");
    if (!cssFile.is_open()) {
        std::cerr << "Error al crear el archivo CSS." << std::endl;
        return;
    }
    css_file << "/* Estilos Globales */\n"
             << "body { font-family: 'Courier New', monospace; background-color: #1e1e1e; color: #cfcfcf; padding: 25px; line-height: 1.6; }\n"
             << ".container { max-width: 900px; margin: 0 auto; }\n"
             << "pre { background-color: #252526; padding: 20px; border-radius: 6px; border: 1px solid #3c3c3c; overflow-x: auto; white-space: pre-wrap; }\n"
             << "h2 { font-family: Arial, sans-serif; margin-bottom: 15px; }\n\n"
             
             << "/* Paleta de Colores del Resaltador (Modo Oscuro) */\n"
             << ".keyword { color: #569cd6; font-weight: bold; }\n"
             << ".identifier { color: #9cdcfe; }\n"
             << ".number { color: #b5cea8; }\n"
             << ".operator { color: #d4d4d4; }\n"
             << ".comparison { color: #dcdcaa; }\n"
             << ".punctuation { color: #ffd700; }\n"
             << ".unknown { color: #f44747; text-decoration: underline wavy #f44747; font-weight: bold; }\n\n"
             
             << "/* Vista Especial de Éxito Sintáctico */\n"
             << ".success-msg { color: #4ec9b0; font-family: Arial, sans-serif; font-weight: bold; margin-bottom: 10px; }\n\n"
             
             << "/* Vista Especial de Error Sintáctico (Código Rojo y Tachado) */\n"
             << ".error-box { background-color: #3a1e1e; border: 2px solid #f44747; border-radius: 6px; padding: 15px; margin-bottom: 25px; font-family: Arial, sans-serif; color: #f44747; }\n"
             << ".error-box h3 { margin-top: 0; color: #ff6b6b; }\n"
             << ".error-item { margin-bottom: 8px; font-size: 14px; color: #ffccd0; }\n"
             << ".codigo-error { background-color: #2d1818 !important; border: 1px dashed #f44747 !important; color: #a0a0a0 !important; text-decoration: line-through; opacity: 0.75; }\n";
             
    css_file.close();
}
// Función para generar el archivo HTML con el resaltado de sintaxis
void generarHTML(const std::vector<Token>& tokens, const std::vector<std::pair<TokenType, std::string>>& lista_errores, const std::string& archivo_salida = "index.html")
{
    generarCSS();

    std::ofstream htmlFile(archivo_salida);
    if (!htmlFile.is_open()) {
        std::cerr << "Error al crear el archivo HTML." << std::endl;
        return;
    }

    html_file << "<!DOCTYPE html>\n<html lang=\"es\">\n<head>\n"
              << "    <meta charset=\"UTF-8\">\n"
              << "    <title>Analizador Sintáctico - Resultado</title>\n"
              << "    <link rel=\"stylesheet\" href=\"style.css\">\n"
              << "</head>\n<body>\n"
              << "    <div class=\"container\">\n";

    //Checamos si el archivo tiene errores sintácticos
    bool tiene_errores = !lista_errores.empty();
    if (tiene_errores) {
        html_file << "        <div class=\"error-box\">\n"
                  << "            <h3>❌ Errores Sintácticos Detectados (" << lista_errores.size() << ")</h3>\n";
        
        int idx = 1;
        for (const auto& err : lista_errores) {
            html_file << "            <div class=\"error-item\">\n"
                      << "                <strong>Defecto " << idx++ << ":</strong> " << ignorarCaracteresHTML(err.second) << "\n"
                      << "            </div>\n";
        }
        html_file << "        </div>\n";
        
        // Rúbrica: Salida especial (código en rojo/tachado) aplicando la clase .codigo-error
        html_file << "        <pre class=\"codigo-error\"><code>";
    } else {
        // Código válido: Muestra mensaje limpio de éxito
        html_file << "        <div class=\"success-msg\">✔️ Análisis Exitoso: El programa es sintácticamente correcto.</div>\n"
                  << "        <pre><code>";
    }

    html_file << "</code></pre>\n"
              << "    </div>\n"
              << "</body>\n</html>";

    html_file.close();
    std::cout << "Archivo HTML generado exitosamente: " << archivo_salida << std::endl;
}
#endif // GENERADORHTML_HPP