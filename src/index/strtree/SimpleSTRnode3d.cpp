/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2020 Paul Ramsey
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <geos/index/strtree/SimpleSTRnode3d.h>

#include <iostream>

using namespace geos::geom;

namespace geos {
namespace index { // geos.index
namespace strtree { // geos.index.strtree


/*public*/
void
SimpleSTRnode3d::toString(std::ostream& os, int indentLevel) const
{
    for (int i = 0; i < indentLevel; i++) {
        os << "  ";
    }
    os << bounds << " [" << level << "]" << std::endl;
    for (auto* node: childNodes) {
        node->toString(os, indentLevel+1);
    }
}

/*public*/
void
SimpleSTRnode3d::addChildNode3d(SimpleSTRnode3d* childNode)
{
    if (bounds.isNull())
        bounds = childNode->getEnvelope();
    else
        bounds.expandToInclude(childNode->getEnvelope());

    childNodes.push_back(childNode);
}

/*public*/
std::size_t
SimpleSTRnode3d::getNumNodes() const
{
    std::size_t count = 1;
    if (isLeaf())
        return count;

    for (auto* node: getChildNodes())
        count += node->getNumNodes();

    return count;
}

/*public*/
std::size_t
SimpleSTRnode3d::getNumLeafNodes() const
{
    std::size_t count = isLeaf() ? 1 : 0;
    for (auto* node: getChildNodes())
        count += node->getNumLeafNodes();
    return count;
}

bool
SimpleSTRnode3d::removeItem(void *itemToRemove)
{
    for (auto it = childNodes.begin(); it != childNodes.end(); ++it) {
        if ((*it)->getItem() == itemToRemove) {
            childNodes.erase(it);
            return true;
        }
    }
    return false;
}

bool
SimpleSTRnode3d::removeChild(SimpleSTRnode3d *child)
{
    for (auto it = childNodes.begin(); it != childNodes.end(); ++it) {
        if ((*it) == child) {
            childNodes.erase(it);
            return true;
        }
    }
    return false;
}



} // namespace geos.index.strtree
} // namespace geos.index
} // namespace geos


