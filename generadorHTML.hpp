#ifndef GENERADORHTML_HPP
#define GENERADORHTML_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "tokens.hpp"

// Función para escapar caracteres especiales en HTML
std::string ignorarCaracteresHTML(const std::string& texto) {
    std::string resultado;
    resultado.reserve(texto.size());
    for (char c : texto) {
        switch (c) {
            case '&':  resultado += "&amp;"; break;
            case '<':  resultado += "&lt;"; break;
            case '>':  resultado += "&gt;"; break;
            case '"':  resultado += "&quot;"; break;
            case '\'': resultado += "&apos;"; break;
            default:   resultado += c; break;
        }
    }
    return resultado;
}
// Función para obtener la clase CSS según el tipo de token

std::string obtenerClaseCSS(TokenType tipo) {
    switch(tipo) {
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
// Función para generar el archivo HTML con el resultado del análisis sintáctico
void generarCSS() {
    std::ofstream cssFile("styles.css"); 
    if (!cssFile.is_open()) {
        std::cerr << "Error al crear el archivo CSS." << std::endl;
        return;
    }
    cssFile << "/* Estilos Globales */\n"
            << "body { font-family: 'Courier New', monospace; background-color: #1e1e1e; color: #cfcfcf; padding: 25px; line-height: 1.6; }\n"
            << ".container { max-width: 900px; margin: 0 auto; }\n"
            << "pre { background-color: #252526; padding: 20px; border-radius: 6px; border: 1px solid #3c3c3c; overflow-x: auto; white-space: pre-wrap; font-size: 16px; }\n"
            << "h2 { font-family: Arial, sans-serif; margin-bottom: 15px; }\n\n"
            << "/* Paleta de Colores del Resaltador */\n"
            << ".keyword { color: #569cd6; font-weight: bold; }\n"
            << ".identifier { color: #9cdcfe; }\n"
            << ".number { color: #b5cea8; }\n"
            << ".operator { color: #d4d4d4; }\n"
            << ".comparison { color: #dcdcaa; }\n"
            << ".punctuation { color: #ffd700; }\n"
            << ".unknown { color: #f44747; text-decoration: underline wavy #f44747; font-weight: bold; }\n\n"
            << ".success-msg { color: #4ec9b0; font-family: Arial, sans-serif; font-weight: bold; margin-bottom: 15px; font-size: 18px; }\n\n"
            << "/* Vista Especial de Error Sintáctico */\n"
            << ".error-box { background-color: #3a1e1e; border: 2px solid #f44747; border-radius: 6px; padding: 15px; margin-bottom: 25px; font-family: Arial, sans-serif; color: #f44747; }\n"
            << ".error-box h3 { margin-top: 0; color: #ff6b6b; }\n"
            << ".error-item { margin-bottom: 8px; font-size: 14px; color: #ffccd0; }\n"
            << ".codigo-error span { text-decoration: line-through; opacity: 0.6; }\n"
            << ".codigo-error .unknown { color: #ff6b6b; text-decoration: underline wavy #f44747; opacity: 1; }\n"
            << ".codigo-error { background-color: #2d1818 !important; border: 2px dashed #f44747 !important; }\n";
    cssFile.close();
}
// Función principal para generar el HTML con los tokens y errores

void generarHTML(const std::vector<Token>& tokens, const std::vector<std::pair<TokenType, std::string>>& lista_errores, const std::string& archivo_salida = "index.html") {
    generarCSS();

    std::ofstream htmlFile(archivo_salida); 
    if (!htmlFile.is_open()) {
        std::cerr << "Error al crear el archivo HTML." << std::endl;
        return;
    }

    htmlFile << "<!DOCTYPE html>\n<html lang=\"es\">\n<head>\n"
             << "    <meta charset=\"UTF-8\">\n"
             << "    <title>Analizador Sintáctico - Resultado</title>\n"
             << "    <link rel=\"stylesheet\" href=\"styles.css\">\n"
             << "</head>\n<body>\n"
             << "    <div class=\"container\">\n"
             << "        <h2>Resultado del Análisis Sintáctico</h2>\n";

    bool tiene_errores = !lista_errores.empty();
    if (tiene_errores) {
        htmlFile << "        <div class=\"error-box\">\n"
                 << "            <h3>❌ Errores Sintácticos Detectados (" << lista_errores.size() << ")</h3>\n";
        int idx = 1;
        for (const auto& err : lista_errores) {
            htmlFile << "            <div class=\"error-item\">\n"
                     << "                <strong>Defecto " << idx++ << ":</strong> " << ignorarCaracteresHTML(err.second) << "\n"
                     << "            </div>\n";
        }
        htmlFile << "        </div>\n"
                 << "        <pre class=\"codigo-error\"><code>";
    } else {
        htmlFile << "        <div class=\"success-msg\">✔️ Análisis Exitoso: El programa es sintácticamente correcto.</div>\n"
                 << "        <pre><code>";
    }

    // Recorrer y pintar los tokens dentro del HTML
    for (size_t i = 0; i < tokens.size(); ++i) {
        const Token& tok = tokens[i];
        std::string clase = obtenerClaseCSS(tok.type);
        htmlFile << "<span class=\"" << clase << "\">" << ignorarCaracteresHTML(tok.value) << "</span>";

        if (i + 1 < tokens.size()) {
            TokenType actual = tok.type;
            TokenType siguiente = tokens[i + 1].type;
            if (actual == TokenType::SEMICOLON || actual == TokenType::LEFT_BRACE || actual == TokenType::RIGHT_BRACE) {
                htmlFile << "\n";
                if (actual == TokenType::LEFT_BRACE) htmlFile << "    ";
            } else if (actual != TokenType::LEFT_PAREN && siguiente != TokenType::RIGHT_PAREN && 
                     siguiente != TokenType::SEMICOLON && siguiente != TokenType::COMMA) {
                htmlFile << " ";
            }
        }
    }

    htmlFile << "</code></pre>\n    </div>\n</body>\n</html>";
    htmlFile.close();
    std::cout << "Archivo HTML generado exitosamente: " << archivo_salida << std::endl;
}
#endif 