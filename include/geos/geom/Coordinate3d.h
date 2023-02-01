/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2006 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#pragma once

#include <geos/export.h>
#include <geos/constants.h> // for DoubleNotANumber
#include <set>
#include <stack>
#include <vector> // for typedefs
#include <string>
#include <limits>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251) // warning C4251: needs to have dll-interface to be used by clients of class
#endif

namespace geos {
namespace geom { // geos.geom

struct CoordinateLessThen;

class GEOS_DLL CoordinateXYZ {

    static CoordinateXYZ _nullCoord;

public:
    CoordinateXYZ()
        : x(0.0)
        , y(0.0)
        , z(0.0)
    {}

    CoordinateXYZ(double xNew, double yNew, double zNew)
        : x(xNew)
        , y(yNew)
        , z(zNew)
    {}

    /// x-coordinate
    double x;

    /// y-coordinate
    double y;

    /// z-coordinate
    double z;

    /// Output function
    GEOS_DLL friend std::ostream& operator<< (std::ostream& os, const CoordinateXYZ& c);

    /// Equality operator for Coordinate. 2D only.
    GEOS_DLL friend bool operator==(const CoordinateXYZ& a, const CoordinateXYZ& b)
    {
        return a.equals3D(b);
    };

    /// Inequality operator for Coordinate. 2D only.
    GEOS_DLL friend bool operator!=(const CoordinateXYZ& a, const CoordinateXYZ& b)
    {
        return ! a.equals3D(b);
    };

    bool isValid() const
    {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    };

    bool equals3D(const CoordinateXYZ& other) const
    {
        if(x != other.x) {
            return false;
        }
        if(y != other.y) {
            return false;
        }
        if(z != other.z) {
            return false;
        }
        return true;
    };

    bool equals3D(const CoordinateXYZ& other, double tolerance) const
    {
        if (std::abs(x - other.x) > tolerance) {
            return false;
        }
        if (std::abs(y - other.y) > tolerance) {
            return false;
        }
        if (std::abs(z - other.z) > tolerance) {
            return false;
        }
        return true;
    };

    /// 3D only
    bool equals(const CoordinateXYZ& other) const
    {
        return equals3D(other);
    };

    /// TODO: deprecate this, move logic to CoordinateLessThen instead
    int compareTo(const CoordinateXYZ& other) const
    {
        if(x < other.x) {
            return -1;
        }
        if(x > other.x) {
            return 1;
        }
        if(y < other.y) {
            return -1;
        }
        if(y > other.y) {
            return 1;
        }
        if(z < other.z) {
            return -1;
        }
        if(z > other.z) {
            return 1;
        }
        return 0;
    };

    static CoordinateXYZ& getNull();

    double distance(const CoordinateXYZ& p) const
    {
        double dx = x - p.x;
        double dy = y - p.y;
        double dz = z - p.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    };

    double distanceSquared(const CoordinateXYZ& p) const
    {
        double dx = x - p.x;
        double dy = y - p.y;
        double dz = z - p.z;
        return dx * dx + dy * dy + dz * dz;
    };

    bool isNull() const
    {
        return (std::isnan(x) && std::isnan(y) && std::isnan(z));
    };

    void setNull()
    {
        x = DoubleNotANumber;
        y = DoubleNotANumber;
        z = DoubleNotANumber;
    };

    struct GEOS_DLL HashCode
    {
        std::size_t operator()(const CoordinateXYZ& c) const
        {
            size_t h = std::hash<double>{}(c.x);
            h ^= std::hash<double>{}(c.y) << 1;
            h ^= std::hash<double>{}(c.z) << 2;
            // z ordinate ignored for consistency with operator==
            return h;
        };
    };

    ///  Returns a string of the form <I>(x,y,z)</I> .
    std::string toString() const;
};

/**
 * \class Coordinate geom.h geos.h
 *
 * \brief
 * Coordinate is the lightweight class used to store coordinates.
 *
 * It is distinct from Point, which is a subclass of Geometry.
 * Unlike objects of type Point (which contain additional
 * information such as an envelope, a precision model, and spatial
 * reference system information), a Coordinate only contains
 * ordinate values and accessor methods.
 *
 * Coordinate objects are two-dimensional points, with an additional
 * z-ordinate. JTS does not support any operations on the z-ordinate except
 * the basic accessor functions.
 *
 * Constructed coordinates will have a z-ordinate of DoubleNotANumber.
 * The standard comparison functions will ignore the z-ordinate.
 *
 */
// Define the following to make assignments and copy constructions
// NON-(will let profilers report usages)
//#define PROFILE_COORDINATE_COPIES 1
/*
class GEOS_DLL Coordinate : public CoordinateXYZ {

private:

    static Coordinate _nullCoord;

public:
    /// A set of const Coordinate pointers
    typedef std::set<const Coordinate*, CoordinateLessThen> ConstSet;

    /// A vector of const Coordinate pointers
    typedef std::vector<const Coordinate*> ConstVect;

    /// A stack of const Coordinate pointers
    typedef std::stack<const Coordinate*> ConstStack;

    /// A vector of Coordinate objects (real object, not pointers)
    typedef std::vector<Coordinate> Vect;

    /// z-coordinate
    double z;

    /// Output function
    GEOS_DLL friend std::ostream& operator<< (std::ostream& os, const Coordinate& c);

    Coordinate()
        : CoordinateXYZ(0.0, 0.0, 0.0)
        , z(DoubleNotANumber)
        {};

    Coordinate(double xNew, double yNew, double zNew = DoubleNotANumber)
        : CoordinateXYZ(xNew, yNew, zNew)
        {};

    explicit Coordinate(const CoordinateXYZ& other)
        : CoordinateXYZ(other)
        {};

    void setNull()
    {
        CoordinateXYZ::setNull();
    };

    static Coordinate& getNull();

    bool isNull() const
    {
        return (std::isnan(x) && std::isnan(y) && std::isnan(z));
    };

    /// 3D comparison
    bool equals3D(const Coordinate& other) const
    {
        return (x == other.x) && (y == other.y) &&
               ((z == other.z) || (std::isnan(z) && std::isnan(other.z)));
    };

    ///  Returns a string of the form <I>(x,y,z)</I> .
    std::string toString() const;
};


/// Strict weak ordering Functor for Coordinate
struct GEOS_DLL CoordinateLessThen {

    bool operator()(const CoordinateXYZ* a, const CoordinateXYZ* b) const
    {
        if(a->compareTo(*b) < 0) {
            return true;
        }
        else {
            return false;
        }
    };

    bool operator()(const CoordinateXYZ& a, const CoordinateXYZ& b) const
    {
        if(a.compareTo(b) < 0) {
            return true;
        }
        else {
            return false;
        }
    };

};

/// Strict weak ordering operator for Coordinate
inline bool operator<(const CoordinateXYZ& a, const CoordinateXYZ& b)
{
    return CoordinateLessThen()(a, b);
}
*/
} // namespace geos.geom
} // namespace geos

#ifdef _MSC_VER
#pragma warning(pop)
#endif


























