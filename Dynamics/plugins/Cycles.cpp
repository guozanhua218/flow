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
 
#include "Cycles.h"
  Integrator* maker()
  {
    return new CyclesIntegrator;
  }

  class CyclesProxy
  {
    public:
      CyclesProxy()
      {
        Factory["Cycles"] = maker;
      }
  };

  CyclesProxy p;
