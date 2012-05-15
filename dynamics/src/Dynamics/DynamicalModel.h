#ifndef DYNAMICAL_MODEL_H
#define DYNAMICAL_MODEL_H

//
// STL includes
//
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

//
// Project includes
//
#include <Coordinate.h>
#include <Parameter.h>
#include <Vector.h>

template <typename ScalarParam>
class DynamicalModel : public ParameterClass<ScalarParam>,
                       public CoordinateClass<ScalarParam>
{

public:

    typedef ScalarParam Scalar;
    typedef DTS::Vector<ScalarParam> Vector;

    typedef typename CoordinateClass<ScalarParam>::Coordinate Coordinate;
    typedef typename ParameterClass<ScalarParam>::RealParameter Parameter;

    /**
        After the constructor, the model is assumed fixed in the number of
        parameters.  Deriving from ParameterClass provides automatic version
        updating whenever a parameter value changes, but note, we do not track
        changes to the parameter names, default values, or min/max values.  
        In principle, these should be fixed after the constructor.
    **/
    DynamicalModel();
    virtual ~DynamicalModel();

    /*
        Return the value of the differential equation evaluated at x.

        The first functor allocates space for the output. The second functor
        writes to a preallocated vector, which is assumed to be distinct from
        the input vector. There will be problems if x == out.
    */
    Vector operator()(Vector const& x) const;
    virtual void operator()(Vector const& x, Vector & out) const = 0;

    Vector getDefaultPoint() const;
    //Vector getCenterPoint() const;

    int getDimension(void) const;
    std::string const& getName() const;
    unsigned int const& getVersion() const;

protected:

    // Subclasses can specify a name in their constructor.
    std::string name;
    //Vector center;

    // inherited from ParameterClass
    unsigned int updateVersion();

private:

    // Whenever there is a change the system, we declare it changed.
    unsigned int version;
};


/**
 * Implementations
 */

template <typename ScalarParam>
DynamicalModel<ScalarParam>::DynamicalModel()
    : version(0)
{
}

template <typename ScalarParam>
DynamicalModel<ScalarParam>::~DynamicalModel()
{
}

template <typename ScalarParam>
inline
DTS::Vector<ScalarParam> DynamicalModel<ScalarParam>::operator()(Vector const& p) const
{
    Vector out(3);
    this->operator()(p, out);
    return out;
}

template <typename ScalarParam>
DTS::Vector<ScalarParam> DynamicalModel<ScalarParam>::getDefaultPoint() const
{
    Vector out(getDimension());
    std::vector<Coordinate> coords = this->getCoords();
    for (unsigned i=0; i < coords.size(); i++)
    {
        out[i] = coords[i].defaultValue;
    }
    return out;
}

/*
template <typename ScalarParam>
DTS::Vector<ScalarParam> DynamicalModel<ScalarParam>::getCenterPoint() const
{
    return center;
}
*/


template <typename ScalarParam>
inline
int DynamicalModel<ScalarParam>::getDimension() const
{
    // getCoords() is a non-dependent name.
    // So we must resolve it now by explictly using 'this'.
    // http://eli.thegreenplace.net/2012/02/06/dependent-name-lookup-for-c-templates/
    return this->getCoords().size();
}


template <typename ScalarParam>
inline
std::string const& DynamicalModel<ScalarParam>::getName() const
{
    return name;
}

template <typename ScalarParam>
inline
unsigned int const& DynamicalModel<ScalarParam>::getVersion() const
{
    return version;
}

template <typename ScalarParam>
inline
unsigned int DynamicalModel<ScalarParam>::updateVersion()
{
    return ++version;
}

#endif
