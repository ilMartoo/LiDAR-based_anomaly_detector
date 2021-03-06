/**
 * @file CLICommand.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 19/04/2022
 *
 * @brief Definicion e implementación del objeto CLICommand
 *
 */

#ifndef CLICOMMAND_CLASS_H
#define CLICOMMAND_CLASS_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include "logging/string_format.h"

enum CLICommandType {
    kExit,     ///< Finalización del programa
    kHelp,     ///< Impresión de la ayuda
    kChrono,   ///< Gestión de cronometros
    kDefine,   ///< Caracterización de objetos y fondo
    kSet,      ///< Definición de parametros
    kDiscard,  ///< Descarte de puntos
    kObject,   ///< Opciones de los objetos
    kModel,    ///< Opciones de los modelos
    kInfo,     ///< Impresión de las opciones actuales
    kList,     ///< Listado de objetos y modelos
    kAnalyze,  ///< Analizador de anomalías
    kUnknown   ///< Comando desconocido
};

/**
 * @brief Comando de la CLI
 */
class CLICommand {
   private:
    enum CLICommandType type;         ///< Tipo de comando
    std::vector<std::string> params;  ///< Vector de argumentos del comando

   public:
    /**
     * Parser de comandos de CLI
     * @param input Texto con el comando
     * @return Objecto CLICommandType
     */
    static CLICommand parse(const std::string& input) {
        std::vector<std::string> params;
        std::string item;

        std::istringstream commandStream(input);

        while (std::getline(commandStream, item, ' ')) {
            if (!item.empty()) {
                params.push_back(item);
            }
        }

        if (params.size() > 0) {
            // EXIT
            if (params[0] == "exit") {
                return {kExit, params};
            }
            // HELP
            else if (params[0] == "help") {
                return {kHelp, params};
            }
            // CHRONO
            else if (params[0] == "chrono") {
                return {kChrono, params};
            }
            // DEFINE
            else if (params[0] == "define") {
                return {kDefine, params};
            }
            // SET
            else if (params[0] == "set") {
                return {kSet, params};
            }
            // DISCARD
            else if (params[0] == "discard") {
                return {kDiscard, params};
            }
            // OBJECT
            else if (params[0] == "object") {
                return {kObject, params};
            }
            // MODEL
            else if (params[0] == "model") {
                return {kModel, params};
            }
            // INFO
            else if (params[0] == "info") {
                return {kInfo, params};
            }
            // LIST
            else if (params[0] == "list") {
                return {kList, params};
            }
            // ANALYZE
            else if (params[0] == "analyze") {
                return {kAnalyze, params};
            }
        }

        // ??? UNKNOWN COMMAND ???
        return {};
    }
    /**
     * Devuelve el tipo del comando
     * @return Tipo de comando
     */
    const enum CLICommandType getType() const { return type; }

    /**
     * Compruba si el comando es válido
     * @return Devuelve true si el comando es válido
     */
    const bool isValid() const { return type != kUnknown; }

    /**
     * Obtiene el número de parametros del comando
     * @return Numero de parámetros
     */
    const int numParams() const { return params.size() > 0 ? params.size() - 1 : 0; }

    /**
     * Operador [] que permite obtener parametros del comando
     * @params[0] i Index del comando a acceder
     * @return Devuelve el parametro en la posición especificada
     */
    const std::string& operator[](unsigned int i) const {
        static const std::string empty_string("");
        return params.size() - 1 > i ? params[i + 1] : empty_string;
    }

    /**
     * Destructor
     */
    ~CLICommand() {}

   private:
    CLICommand(enum CLICommandType type, std::vector<std::string>& params) : type(type), params(params) {}
    CLICommand() : type(kUnknown), params() {}
};

#endif  // COMMAND_CLASS_H
