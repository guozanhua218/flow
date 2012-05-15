#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include <cmath>

#include "Parameter.h"

template <typename ScalarParam>
class Experiment;

template <typename ScalarParam>
class Transformer : public ParameterClass<ScalarParam>
{

friend class Experiment<ScalarParam>;

public:
    typedef DynamicalModel<ScalarParam> Model;
    typedef typename DynamicalModel<ScalarParam>::Scalar Scalar;
    typedef typename DynamicalModel<ScalarParam>::Vector Vector;
    
    Transformer(Model const& model);
    virtual ~Transformer();

    /*
        The first method allocates space for the output. The second method
        writes to a preallocated vector, which is assumed to be distinct from
        the input vector. There will be problems if v == out.
    */
    Vector transform(Vector const& v);
    virtual void transform(Vector const& v, Vector & out);

    Vector invTransform(Vector const& v);
    virtual void invTransform(Vector const& v, Vector & out);

    std::string const& getName() const;
    void setName(std::string const& name);

    unsigned int const& getVersion() const;

protected:
    Model const& model;
    std::string name;

    unsigned int updateVersion();

private:
    unsigned int version;

};


//
// Implementation
//


template <typename ScalarParam>
Transformer<ScalarParam>::Transformer(Model const& model)
: model(model),
  name("transformer"),
  version(0)
{
}

template <typename ScalarParam>
Transformer<ScalarParam>::~Transformer()
{
}

template <typename ScalarParam>
typename DynamicalModel<ScalarParam>::Vector Transformer<ScalarParam>::transform(Vector const& v)
{
    Vector out(3);
    transform(v, out);
    return out;
}

template <typename ScalarParam>
void Transformer<ScalarParam>::transform(Vector const& v, Vector & out)
{
    // Take the first three components

    out[0] = v[0];
    out[1] = v[1];
    out[2] = v[2];
}

template <typename ScalarParam>
typename DynamicalModel<ScalarParam>::Vector Transformer<ScalarParam>::invTransform(Vector const& v)
{
    Vector out(model.getDimension());
    invTransform(v, out);
    return out;
}

template <typename ScalarParam>
void Transformer<ScalarParam>::invTransform(Vector const& v, Vector & out)
{
    // Take the first three components

    out[0] = v[0];
    out[1] = v[1];
    out[2] = v[2];
    
    typename CoordinateClass<ScalarParam>::Coordinates coords = model.getCoords();
    for ( int i = 3; i < model.getDimension(); i++ )
    {
        // Set the value to the midpoint of the suggested min/max coordinate values
        // Since we use minValue and maxValue for sliders, they must be finite.
        out[i] = coords[i].minValue;
        out[i] += (coords[i].maxValue - coords[i].minValue) / 2;
    }
}

template <typename ScalarParam>
inline
std::string const& Transformer<ScalarParam>::getName() const
{
    return name;
}

template <typename ScalarParam>
inline
void Transformer<ScalarParam>::setName(std::string const& value)
{
    name = value;
}

template <typename ScalarParam>
inline
unsigned int const& Transformer<ScalarParam>::getVersion() const
{
    return version;
}

template <typename ScalarParam>
inline
unsigned int Transformer<ScalarParam>::updateVersion()
{
    return ++version;
}

#endif
