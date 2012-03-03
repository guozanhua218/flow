/*******************************************************************************
 TrilinearInterpolator: A trilinear interpolator.
 Copyright (c) 2006-2008

 This file is part of the Dynamics Toolset.

 The Dynamics Toolset is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option) any
 later version.

 The Dynamics Toolset is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License
 along with the Dynamics Toolset. If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/
#ifndef TRILINEARINTERPOLATOR_H
#define TRILINEARINTERPOLATOR_H

#include "Interpolator.h"

class TrilinearInterpolator : public Interpolator
{
public:
    TrilinearInterpolator(DynamicalModel* m) : Interpolator(m) {}

protected:
    virtual Vector _interpolate(const Point& p) const;
};

#endif


