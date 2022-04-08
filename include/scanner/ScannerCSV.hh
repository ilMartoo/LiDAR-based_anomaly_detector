/**
 * @file ScannerCSV.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 19/03/2022
 *
 * Definicion del objeto ScannerCSV
 *
 */

#ifndef SCANNERCSV_CLASS_H
#define SCANNERCSV_CLASS_H

#include <iostream>
#include <string>
#include <fstream>
#include <functional>
#include <thread>

#include "scanner/IScanner.hh"
#include "models/Point.hh"

/**
 * Implementación de la interfaz IScanner para la lectura de puntos de archivos CSV obtenidos del
 * programa propietario Livox Viewer
 */
class ScannerCSV : public IScanner {
   public:
    /**
     * Constructor del objeto ScannerCSV
     * @param filename Archivo contenedor de datos
     */
    ScannerCSV(const std::string &filename) : filename(filename), exit(true) {}

    /**
     * Destructor del scanner
     */
    ~ScannerCSV() {
        if (this->infile.is_open()) {
            this->infile.close();
        }
        delete executionThread;
    };

    /**
     * Inicialización del escaner
     * @return Se devolverá true si se ha establecido el escaner correctamente
     */
    bool init();

    /**
     * Comienza la obtención de puntos
     * @return Se devolverá true al finalizar de leer correctamente el archivo o false si ocurre un
     * error en el proceso
     */
    bool start();

    /**
     * Establece la función especificada como función de callback a la que se llamará cada vez que
     * se escanee un nuevo punto
     * @param func Función de callback a ser llamada por el sensor
     * @return Se devolverá true si se ha establecido el callback correctamente
     */
    bool setCallback(const std::function<void(const Point &p)> func);

    /**
     * Finaliza el escaner
     */
    void stop();

   private:
    const std::string &filename;  ///< Nombre del archivo de datos
    std::ifstream infile;         ///< Stream del archivo de datos

    std::function<void(const Point &p)> callback;  ///< Función de callback

    std::thread *executionThread;  ///< Hilo de ejecución del escaner
    bool exit;                     ///< Variable para la finalización del hilo

    /**
     * Lee los puntos del archivo de input
     */
    void readData();
};

#endif  //SCANNERCSV_CLASS_H