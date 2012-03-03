/*
 * DO NOT EDIT THIS FILE.
 *
 * THIS FILE HAS BEEN AUTOMATICALLY GENERATED.
 * ANY CHANGES MADE TO THE CODE WILL BE LOST.
 *
 * TO MODIFY THE PARAMETER VALUES OR DYNAMICAL
 * EQUATIONS EDIT THE XML FILE (dynamics.xml).
 * 
 */
 
#ifndef VALLIS_INCLUDED_
#define VALLIS_INCLUDED_

#include "BaseModel.h"
#include "RungeKutta4.h"

#include "VallisParameterDialog.h"

class Vallis : public DynamicalModel
{
  Scalar u;
  Scalar a;
public:
  Vallis(Scalar U, Scalar A)
     : u(U), a(A)
  { }

  virtual ~Vallis() { }

  virtual Vector exact(const Point& p) const
    {
      return Vector(u*p[1] - a*p[0],p[0]*p[2] - p[1],1 - p[0]*p[1] - p[2]);
    }

private:
  virtual void _setValue(const std::string& name, Scalar value)
    {
      if      (name == "u") u = value;
      else if (name == "a") a = value;
    }
};

class VallisIntegrator : public Integrator
{
  Vallis* dynamics;
  RungeKutta4<Vallis>* integrator;
public:
  VallisIntegrator()
    {
      dynamics = new Vallis(12, 0.3);
      double stepSize = 0.01;
      integrator = new RungeKutta4<Vallis>(*dynamics, stepSize);
    }

  virtual ~VallisIntegrator()
    {
      delete dynamics;
      delete integrator;
    }

  inline Vector step(const Point& p)
    {
      return integrator->step(p);
    }

  virtual unsigned int getModelVersion()
    {
      return dynamics->getModelVersion();
    }

  virtual CaveDialog* createParameterDialog(GLMotif::PopupMenu *parent)
    {
      return new VallisParameterDialog(parent, dynamics);
    }

  virtual Vrui::Point center() const
    {
      return Vrui::Point(0, 0, 0);
    }
};

#endif
