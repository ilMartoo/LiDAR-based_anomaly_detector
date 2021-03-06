/**
 * @file DBScan.cc
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 18/05/2022
 *
 * @brief Implementación del algoritmo DBScan
 *
 */

#include <utility>
#include <vector>

#include "models/Point.hh"
#include "models/Octree.hh"
#include "models/Kernel.hh"
#include "object_characterization/DBScan.hh"
#include "models/Geometry.hh"

#include "app/config.h"

std::vector<std::vector<size_t>> DBScan::clusters(std::vector<Point> &points) {
    int clusterID = 0;
    std::vector<std::vector<size_t>> clusters;
    Octree clustermap(points);

    for (auto &p : points) {
        if (p.getClusterID() == cUnclassified) {
            std::pair<bool, std::vector<size_t>> expansion = expandCluster(p, clusterID, points, clustermap);
            if (expansion.first) {
                clusters.push_back(expansion.second);
                ++clusterID;
            }
        }
    }

    return clusters;
}

std::pair<bool, std::vector<size_t>> DBScan::expandCluster(Point &centroid, int clusterID, std::vector<Point> &points, const Octree &map) {
    auto clusterSeeds = centroidNeighbours(centroid, points, map);

    // Centroide no contiene la cantidad mínima de puntos
    if (clusterSeeds.second.size() < MIN_CLUSTER_POINTS) {
        centroid.setClusterID(cNoise);
        return {false, {}};
    }

    // Expandimos el cluster
    else {
        std::vector<size_t> clusterPoints = clusterSeeds.second;

        size_t index = 0, indexCorePoint = 0;
        for (auto &i : clusterSeeds.second) {
            points[i].setClusterID(clusterID);
            if (points[i] == centroid) {
                indexCorePoint = index;
            }
            ++index;
        }
        clusterSeeds.second.erase(clusterSeeds.second.begin() + indexCorePoint);  // Eliminamos el centroide para el calculo de vecinos

        // Expandimos a través de los puntos vecinos al centroide
        for (size_t i = 0, seedsSize = clusterSeeds.second.size(); i < seedsSize; ++i) {
            auto clusterNeighbours = centroidNeighbours(points[clusterSeeds.second[i]], points, map);

            // Comprobación de que no es un punto frontera
            if (clusterNeighbours.first >= MIN_CLUSTER_POINTS) {
                for (auto &i : clusterNeighbours.second) {
                    if (points[i].getClusterID() == cUnclassified) {
                        clusterSeeds.second.push_back(i);
                        ++seedsSize;
                    }
                    points[i].setClusterID(clusterID);

                    clusterPoints.push_back(i);  // Añadimos punto al vector de indices totales
                }
            }
        }

        return {true, clusterPoints};
    }
}

std::pair<size_t, std::vector<size_t>> DBScan::centroidNeighbours(const Point &centroid, const std::vector<Point> &points, const Octree &map) {
    std::vector<size_t> clusterIndex;

    std::vector<Point *> neighbourPoints = map.searchNeighbors(centroid, CLUSTER_POINT_PROXIMITY, Kernel_t::sphere);

    for (Point *&np : neighbourPoints) {
        if (np->getClusterID() < 0) {
            // A partir del estandar C++0x los elementos de un vector estan contiguos en memoria (menos los tipo bool)
            // Haciendo uso de aritmetica de punteros le restamos a una dirección de un punto del vector (np) la dirección
            // inicial (&*points.begin()) obteniendo de esta forma el índice del elemento en tiempo constante sin recurrir
            // a una búsqueda lineal
            clusterIndex.push_back((size_t)(np - &*points.begin()));
        }
    }

    return {neighbourPoints.size(), clusterIndex};
}

std::vector<std::vector<size_t>> DBScan::normals(std::vector<Point> &points) {
    int clusterID = 0;
    std::vector<std::vector<size_t>> faces;

    Octree clustermap(points);
    std::vector<Vector> normals = Geometry::computeNormals(points, clustermap, NORMAL_CALC_POINT_PROXIMITY);  // Cálculo de las normales

    for (size_t i = 0; i < points.size(); ++i) {
        if (points[i].getClusterID() == cUnclassified && normals[i] != Vector(0, 0, 0)) {
            std::pair<bool, std::vector<size_t>> expansion = expandNormalCluster(i, clusterID, points, normals, clustermap);
            if (expansion.first) {
                faces.push_back(expansion.second);
                ++clusterID;
            }
        }
    }

    return faces;
}

std::pair<bool, std::vector<size_t>> DBScan::expandNormalCluster(size_t centroid, int clusterID, std::vector<Point> &points, const std::vector<Vector> &normals, const Octree &map) {
    auto clusterSeeds = centroidNormalNeighbours(centroid, normals[centroid], points, normals, map);

    // Centroide no contiene la cantidad mínima de puntos
    if (clusterSeeds.second.size() < MIN_FACE_POINTS) {
        points[centroid].setClusterID(cNoise);
        return {false, {}};
    }

    // Expandimos el cluster
    else {
        std::vector<size_t> clusterPoints = clusterSeeds.second;
        std::vector<Vector> clusterNormals;  // Referencias a las normales de los puntos
        clusterNormals.reserve(clusterSeeds.second.size());

        // Guardado de los puntos iniciales del cluster
        size_t index = 0, indexCorePoint = 0;
        for (auto &i : clusterSeeds.second) {
            points[i].setClusterID(clusterID);
            clusterNormals.push_back(normals[i]);
            if (points[i] == points[centroid]) {
                indexCorePoint = index;
            }
            ++index;
        }
        clusterSeeds.second.erase(clusterSeeds.second.begin() + indexCorePoint);  // Eliminamos el centroide para el calculo de vecinos

        // Expandimos a través de los puntos vecinos al centroide
        for (size_t i = 0, seedsSize = clusterSeeds.second.size(); i < seedsSize; ++i) {
            Vector meanNormal = Geometry::mean(clusterNormals);
            auto clusterNeighbours = centroidNormalNeighbours(clusterSeeds.second[i], meanNormal, points, normals, map);

            // Comprobación de que no es un punto frontera
            if (clusterNeighbours.first >= MIN_FACE_POINTS) {
                for (auto &i : clusterNeighbours.second) {
                    if (points[i].getClusterID() == cUnclassified) {
                        clusterSeeds.second.push_back(i);
                        ++seedsSize;
                    }
                    points[i].setClusterID(clusterID);

                    clusterNormals.push_back(normals[i]);  // Añadimos referencia para el calculo de la normal media
                    clusterPoints.push_back(i);            // Añadimos punto al vector de indices totales
                }
            }
        }

        return {true, clusterPoints};
    }
}

std::pair<size_t, std::vector<size_t>> DBScan::centroidNormalNeighbours(size_t centroid, const Vector &meanNormal, const std::vector<Point> &points, const std::vector<Vector> &normals, const Octree &map) {
    std::vector<size_t> clusterIndex;
    size_t neighbours = 0;
    std::vector<Point *> neighbourPoints = map.searchNeighbors(points[centroid], FACE_POINT_PROXIMITY, Kernel_t::sphere);

    size_t i;
    for (Point *&np : neighbourPoints) {
        // A partir del estandar C++0x los elementos de un vector estan contiguos en memoria (menos los tipo bool)
        // Haciendo uso de aritmetica de punteros le restamos a una dirección de un punto del vector (np) la dirección
        // inicial (&*points.begin()) obteniendo de esta forma el índice del elemento en tiempo constante sin recurrir
        // a una búsqueda lineal
        i = (size_t)(np - &*points.begin());

        if (normals[i] != Vector(0, 0, 0) &&
            ((
                 normals[centroid].vectorialAngle(normals[i]) <= MAX_NORMAL_VECT_ANGLE &&
                 meanNormal.vectorialAngle(normals[i]) <= MAX_MEAN_VECT_ANGLE) ||
             meanNormal.vectorialAngle(normals[i]) <= MAX_MEAN_VECT_ANGLE_SINGLE)) {
            ++neighbours;
            if (np->getClusterID() < 0) {
                clusterIndex.push_back(i);
            }
        }
    }

    return {neighbours, clusterIndex};
}
