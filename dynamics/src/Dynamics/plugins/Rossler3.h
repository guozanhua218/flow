#ifndef ROSSLER3_H
#define ROSSLER3_H

#include <limits>

#include <DynamicalModel.h>
#include <Coordinate.h>
#include <Parameter.h>

class Rossler3 : public DynamicalModel<double>
{
public:
    Rossler3(Scalar a=.2,  Scalar b=.2, Scalar c=5.7)
    : DynamicalModel<double>()
    {
        name = "Rossler";

        double inf = std::numeric_limits<Scalar>::infinity();
        addCoordinate( Coordinate("x", 5, -15, 15) );
        addCoordinate( Coordinate("y", 5, -15, 15) );
        addCoordinate( Coordinate("z", 5, -1, 30) );
        addCoordinate( Coordinate("t", 0, 0, inf) );        

        addRealParameter( RealParameter("a", a, -.5,    .5,  0.2, 0.01) );
        addRealParameter( RealParameter("b", b, -.5,    .5,  0.2, 0.01) );
        addRealParameter( RealParameter("c", c, 0,    10.0,  5.7, 0.01) );
    }

    virtual ~Rossler3() { }

    virtual void operator()(Vector const& p, Vector & out) const
    {
        out[0] = -p[1] - p[2];
        out[1] = p[0] + realParamValues[0] * p[1];
        out[2] = realParamValues[1] + p[2] * (p[0] - realParamValues[2]);
        out[3] = 1;
    }
};

#endif