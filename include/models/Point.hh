/**
 * @file Point.hh
 * @author Martín Suárez (martin.suarez.garcia@rai.usc.es)
 * @date 19/03/2022
 *
 * @brief Definición e implementación del objeto Point
 *
 */

#ifndef POINT_CLASS_H
#define POINT_CLASS_H

#include <ostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <limits>

#include "armadillo"

/**
 * Enum de tipos de cluster
 */
enum PointCluster {
    cUnclassified = -1,  ///< Punto no clasificado
    cCorePoint = -2,     ///< Punto core
    cBorderPoint = -3,   ///< Punto frontera
    cNoise = -4          ///< Ruido
};

/**
 * @brief Representación de un punto perteneciente a una nube de puntos tridimensional
 */
class Point {
   private:
    double x;  ///< Localización en el eje x del punto
    double y;  ///< Localización en el eje y del punto
    double z;  ///< Localización en el eje z del punto
    int cID;   ///< Cluster ID

   public:
    /**
     * Constructor
     */
    Point() : x(), y(), z(), cID(cUnclassified) {}
    /**
     * Constructor
     * @param x Posición en x del punto
     * @param y Posición en y del punto
     * @param z Posición en z del punto
     * @param cID Cluster ID
     */
    Point(double x, double y, double z, int cID = cUnclassified) : x(x), y(y), z(z), cID(cID) {}
    /**
     * Constructor
     * @param x Posición en x del punto
     * @param y Posición en y del punto
     * @param z Posición en z del punto
     * @param cID Cluster ID
     */
    Point(int x, int y, int z, int cID = cUnclassified) : x((double)x), y((double)y), z((double)z), cID(cID) {}

    ////// Operaciones tridimensionales
    /**
     * Calcúla la distancia euclidea entre dos puntos
     * @param p Punto contra el que calcular la distancia
     * @return double distancia de separación entre los dos puntos
     */
    double distance3D(const Point &p) const {
        double dx = x - p.x;
        double dy = y - p.y;
        double dz = z - p.z;
        return std::sqrt((dx * dx) + (dy * dy) + (dz * dz));
    }
    /**
     * Rota el punto respecto al punto (0,0,0) según la matriz de rotacion
     * @param rot Matriz de rotación
     * @return Punto resultado de la rotación
     */
    Point rotate(const arma::mat33 &rot) const {
        return Point(rot(0, 0) * x + rot(0, 1) * y + rot(0, 2) * z,
                     rot(1, 0) * x + rot(1, 1) * y + rot(1, 2) * z,
                     rot(2, 0) * x + rot(2, 1) * y + rot(2, 2) * z);
    }

    ////// Operaciones vectoriales
    /**
     * Calcúla el módulo de un vector
     * @return módulo del vector
     */
    double module() const { return std::sqrt((x * x) + (y * y) + (z * z)); }
    /**
     * Producto escalar de dos vectores
     * @param v Vector contra el que realizar el producto escalar
     * @return Valor resultado del producto escalar
     */
    double scalarProduct(const Point &v) const { return (x * v.x) + (y * v.y) + (z * v.z); }
    /**
     * Producto vectorial de dos vectores
     * @param v Vector contra el que realizar el producto vectorial
     * @return Vector resultado del producto vectorial
     */
    Point crossProduct(const Point &v) const { return Point(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
    /**
     * Calcula el ángulo de separación entre dos vectores
     * @param v Vector contra el que medir la distancia angular
     * @return Ángulo de separación en radianes
     */
    double vectorialAngle(const Point &v) const { return std::acos(this->scalarProduct(v) / (this->module() * v.module())); }

    ////// Getters
    /**
     * Devuelve la localización en el eje x del punto
     * @return Posición en x del punto
     */
    double getX() const { return x; }
    /**
     * Devuelve la localización en el eje y del punto
     * @return Posición en y del punto
     */
    double getY() const { return y; }
    /**
     * Devuelve la localización en el eje z del punto
     * @return Posición en z del punto
     */
    double getZ() const { return z; }
    /**
     * Devuelve el ID del cluster al que pertenece el punto
     * @return ID del cluster
     */
    int getClusterID() const { return cID; }

    ////// Setters
    /**
     * Setter de la coordenada x
     * @param x Valor de x a establecer
     */
    void setX(double x) { this->x = x; }
    /**
     * Setter de la coordenada y
     * @param y Valor de y a establecer
     */
    void setY(double y) { this->y = y; }
    /**
     * Setter de la coordenada z
     * @param z Valor de z a establecer
     */
    void setZ(double z) { this->z = z; }
    /**
     * Setter del ID del cluster al que pertenece el punto
     * @param cID ID del nuevo cluster
     */
    void setClusterID(int cID) { this->cID = cID; }

    ////// Strings e impresión
    /**
     * Crea un ID del punto según sus coordenadas
     * @return ID del punto
     */
    std::string ID() const {
        std::stringstream id;
        id << std::fixed << std::setprecision(6) << x << y << z;
        return id.str();
    }
    /**
     * Obtiene un string con los datos del punto
     * @return String con los datos del punto
     */
    std::string string() const {
        std::stringstream line;
        line << std::fixed << std::setprecision(6) << x << ", " << y << ", " << z;
        return line.str();
    }
    // Imprime la información del punto p
    friend std::ostream &operator<<(std::ostream &strm, const Point &p) { return strm << p.string(); }

    ////// Operadores
    /**
     * Operador de igualdad
     * @param p Punto a igualar
     * @return true si los puntos tienen las mismas coordenadas
     */
    bool operator==(const Point &p) const { return ((std::fabs(x - p.x) <= std::numeric_limits<double>::epsilon()) && (std::fabs(y - p.y) <= std::numeric_limits<double>::epsilon()) && (std::fabs(z - p.z) <= std::numeric_limits<double>::epsilon())); }
    /**
     * Operador de desigualdad
     * @param p Punto a comparar
     * @return true si los puntos tienen distintas coordenadas
     */
    bool operator!=(const Point &p) const { return !(*this == p); }
    /**
     * Operador de resta de puntos
     * @param p Punto a restar
     * @return Punto resultado de la operacion
     */
    Point operator-(const Point &p) const { return Point(x - p.x, y - p.y, z - p.z); }
    /**
     * Operador de resta de puntos
     * @param d double a restar
     * @return Punto resultado de la operacion
     */
    Point operator-(double d) const { return Point(x - d, y - d, z - d); }
    /**
     * Operador de suma de puntos
     * @param p Punto a sumar
     * @return Punto resultado de la operacion
     */
    Point operator+(const Point &p) const { return Point(x + p.x, y + p.y, z + p.z); }
    /**
     * Operador de suma de puntos
     * @param d double a sumar
     * @return Punto resultado de la operacion
     */
    Point operator+(double d) const { return Point(x + d, y + d, z + d); }
    /**
     * Operador de división de puntos
     * @param p Punto a dividir
     * @return Punto resultado de la operacion
     */
    Point operator/(const Point &p) const { return Point(x / p.x, y / p.y, z / p.z); }
    /**
     * Operador de división de puntos
     * @param d double a dividir
     * @return Punto resultado de la operacion
     */
    Point operator/(double d) const { return Point(x / d, y / d, z / d); }
    /**
     * Operador de multiplicación de puntos
     * @param p Punto a multiplicar
     * @return Punto resultado de la operacion
     */
    Point operator*(const Point &p) const { return Point(x * p.x, y * p.y, z * p.z); }
    /**
     * Operador de multiplicación de puntos
     * @param d double a multiplicar
     * @return Punto resultado de la operacion
     */
    Point operator*(double d) const { return Point(x * d, y * d, z * d); }
    /**
     * Comparador de IDs de puntos
     * @param p Punto a comparar
     * @return Punto resultado de la operacion
     */
    bool operator<(const Point &p) const { return ID() < p.ID(); }
};

typedef Point Vector;  ///< Definición de Vector como un Point

#endif  // POINT_CLASS_H