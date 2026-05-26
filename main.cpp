#include "lexer.hpp"
#include "parser.hpp"
#include "html_generator.hpp"

int main() {
    std::string codigo_fuente = "var x = 5; if (x > 2) { x = x + 1; }";

    Lexer lx(codigo_fuente);
    Parser ps(lx);

    // Llamas a la función pasándole los tokens de tu lexer y los errores de tu parser
    generarReporteHTML(lx.getTokens(), ps.getErrors(), "analisis.html");

    return 0;
}