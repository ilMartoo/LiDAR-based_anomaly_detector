/**
 * @file AnomalyReport.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 10/05/2022
 *
 * Definición e implementación de la clase AnomalyReport
 *
 */

#ifndef ANOMALYREPORT_CLASS_H
#define ANOMALYREPORT_CLASS_H

#include <vector>
#include <utility>

#include <anomaly_detection/Comparison.hh>
#include <anomaly_detection/FaceComparison.hh>

class AnomalyReport {
   public:
    const bool similar;                                 ///< Representa si las caras comparadas son similares (false si caras_objecto == 0)
    const Comparison generalComparison;                 ///< Comparación general entre objeto y modelo
    const long deltaFaces;                              ///< Delta de caras entre modelo y objeto (caras_modelo - caras_objeto)
    const std::vector<FaceComparison> faceComparisons;  ///< Vector de resultados de las comparaciones de caras individuales

    /**
     * Constructor
     * @param face Cara del modelo que mejor se corresponde con el objecto
     * @param xrd Anomalía del radio en x
     * @param yrd Anomalía del radio en y
     * @param zrd Anomalía del radio en z
     */
    AnomalyReport(bool similar, const Comparison &generalComparison, long deltaFaces, const std::vector<FaceComparison> &faceComparisons)
        : similar(similar), generalComparison(generalComparison), deltaFaces(deltaFaces), faceComparisons(faceComparisons) {}
    /**
     * Destructor
     */
    ~AnomalyReport() {}
};

#endif  // ANOMALYREPORT_CLASS_H