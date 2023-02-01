/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2005-2006 Refractions Research Inc.
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************
 *
 * Last port: geom/Envelope.java rev 1.46 (JTS-1.10)
 *
 **********************************************************************/

#include <geos/geom/Envelope3d.h>
#include <geos/geom/Coordinate3d.h>

#include <algorithm>
#include <sstream>
#include <cmath>


#ifndef GEOS_DEBUG
#define GEOS_DEBUG 0
#endif

#if GEOS_DEBUG
#include <iostream>
#endif

namespace geos {
namespace geom { // geos::geom

/*public*/
bool
Envelope3d::intersects(const CoordinateXYZ& p1, const CoordinateXYZ& p2,
                       const CoordinateXYZ& q)
{
    //OptimizeIt shows that Math#min and Math#max here are a bottleneck.
    //Replace with direct comparisons. [Jon Aquino]
    if(((q.x >= (p1.x < p2.x ? p1.x : p2.x)) && (q.x <= (p1.x > p2.x ? p1.x : p2.x))) &&
            ((q.y >= (p1.y < p2.y ? p1.y : p2.y)) && (q.y <= (p1.y > p2.y ? p1.y : p2.y))) &&
            ((q.z >= (p1.z < p2.z ? p1.z : p2.z)) && (q.z <= (p1.z > p2.z ? p1.z : p2.z)))) {
        return true;
    }
    return false;
}

/*public*/
bool
Envelope3d::intersects(const CoordinateXYZ& a, const CoordinateXYZ& b) const
{
    // These comparisons look redundant, but an alternative using
    // std::minmax performs no better and compiles down to more
    // instructions.
    double envminx = (a.x < b.x) ? a.x : b.x;
    if(!(maxx >= envminx)) { // awkward comparison catches cases where this->isNull()
        return false;
    }

    double envmaxx = (a.x > b.x) ? a.x : b.x;
    if(envmaxx < minx) {
        return false;
    }

    double envminy = (a.y < b.y) ? a.y : b.y;
    if(envminy > maxy) {
        return false;
    }

    double envmaxy = (a.y > b.y) ? a.y : b.y;
    if(envmaxy < miny) {
        return false;
    }

    double envminz = (a.z < b.z) ? a.z : b.z;
    if(envminz > maxz) {
        return false;
    }

    double envmaxz = (a.z > b.z) ? a.z : b.z;
    if(envmaxz < minz) {
        return false;
    }

    return true;
}

/*public*/
Envelope3d::Envelope3d(const std::string& str)
{
    // The string should be in the format:
    // Env[7.2:2.3,7.1:8.2]

    // extract out the values between the [ and ] characters
    std::string::size_type index = str.find('[');
    std::string coordString = str.substr(index + 1, str.size() - 1 - 1);

    // now split apart the string on : and , characters
    std::vector<std::string> values = split(coordString, ":,");

    // create a new envelope
    init(strtod(values[0].c_str(), nullptr),
         strtod(values[1].c_str(), nullptr),
         strtod(values[2].c_str(), nullptr),
         strtod(values[3].c_str(), nullptr),
         strtod(values[4].c_str(), nullptr),
         strtod(values[5].c_str(), nullptr));
}

/*public*/
bool
Envelope3d::covers(const Envelope3d& other) const
{
    return
        other.minx >= minx &&
        other.maxx <= maxx &&
        other.miny >= miny &&
        other.maxy <= maxy &&
        other.minz >= minz &&
        other.maxz <= maxz;
}

/*public*/
bool
Envelope3d::equals(const Envelope3d* other) const
{
    if(isNull()) {
        return other->isNull();
    }
    return  other->minx == minx &&
            other->maxx == maxx &&
            other->miny == miny &&
            other->maxy == maxy &&
            other->minz == minz &&
            other->maxz == maxz;
}

/* public */
std::ostream&
operator<< (std::ostream& os, const Envelope3d& o)
{
    os << "Env[" << o.minx << ":" << o.maxx << ","
       << o.miny << ":" << o.maxy << ","
       << o.minz << ":" << o.maxz << "]";
    return os;
}


/*public*/
std::string
Envelope3d::toString() const
{
    std::ostringstream s;
    s << *this;
    return s.str();
}


/*public*/
size_t
Envelope3d::hashCode() const
{
    auto hash = std::hash<double>{};

    //Algorithm from Effective Java by Joshua Bloch [Jon Aquino]
    std::size_t result = 17;
    result = 37 * result + hash(minx);
    result = 37 * result + hash(maxx);
    result = 37 * result + hash(miny);
    result = 37 * result + hash(maxy);
    result = 37 * result + hash(minz);
    result = 37 * result + hash(maxz);
    return result;
}

/*public static*/
std::vector<std::string>
Envelope3d::split(const std::string& str, const std::string& delimiters)
{
    std::vector<std::string> tokens;

    // Find first "non-delimiter".
    std::string::size_type lastPos = 0;
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);

    while(std::string::npos != pos || std::string::npos != lastPos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);

        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }

    return tokens;
}

/*public*/
bool
Envelope3d::centre(CoordinateXYZ& p_centre) const
{
    if(isNull()) {
        return false;
    }
    p_centre.x = (getMinX() + getMaxX()) / 2.0;
    p_centre.y = (getMinY() + getMaxY()) / 2.0;
    p_centre.z = (getMinZ() + getMaxZ()) / 2.0;
    return true;
}

/*public*/
bool
Envelope3d::intersection(const Envelope3d& env, Envelope3d& result) const
{
    if(isNull() || env.isNull() || ! intersects(env)) {
        return false;
    }

    double intMinX = minx > env.minx ? minx : env.minx;
    double intMinY = miny > env.miny ? miny : env.miny;
    double intMaxX = maxx < env.maxx ? maxx : env.maxx;
    double intMaxY = maxy < env.maxy ? maxy : env.maxy;
    double intMinZ = minz > env.minz ? minz : env.minz;
    double intMaxZ = maxz < env.maxz ? maxz : env.maxz;
    result.init(intMinX, intMaxX, intMinY, intMaxY, intMinZ, intMaxZ);
    return true;
}

/*public*/
void
Envelope3d::translate(double transX, double transY, double transZ)
{
    if(isNull()) {
        return;
    }
    init(getMinX() + transX, getMaxX() + transX,
         getMinY() + transY, getMaxY() + transY,
         getMinZ() + transZ, getMaxZ() + transZ);
}


/*public*/
void
Envelope3d::expandBy(double deltaX, double deltaY, double deltaZ)
{
    minx -= deltaX;
    maxx += deltaX;
    miny -= deltaY;
    maxy += deltaY;
    minz -= deltaZ;
    maxz += deltaZ;

    // check for envelope disappearing
    if(minx > maxx || miny > maxy || minz > maxz) {
        setToNull();
    }
}


bool
operator< (const Envelope3d& a, const Envelope3d& b)
{
    /*
    * Compares two envelopes using lexicographic ordering.
    * The ordering comparison is based on the usual numerical
    * comparison between the sequence of ordinates.
    * Null envelopes are less than all non-null envelopes.
    */
    if (a.isNull()) {
        // null == null
        if (b.isNull())
            return false;
        // null < notnull
        else
            return true;
    }
    // notnull > null
    if (b.isNull())
        return false;

    // compare based on numerical ordering of ordinates
    if (a.getMinX() < b.getMinX()) return true;
    if (a.getMinX() > b.getMinX()) return false;
    if (a.getMinY() < b.getMinY()) return true;
    if (a.getMinY() > b.getMinY()) return false;
    if (a.getMinZ() < b.getMinZ()) return true;
    if (a.getMinZ() > b.getMinZ()) return false;

    if (a.getMaxX() < b.getMaxX()) return true;
    if (a.getMaxX() > b.getMaxX()) return false;
    if (a.getMaxY() < b.getMaxY()) return true;
    if (a.getMaxY() > b.getMaxY()) return false;
    if (a.getMaxZ() < b.getMaxZ()) return true;
    if (a.getMaxZ() > b.getMaxZ()) return false;
    return false; // == is not strictly <
}



} // namespace geos::geom
} // namespace geos
