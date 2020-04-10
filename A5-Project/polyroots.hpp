#ifndef CS488_POLYROOTS_HPP
#define CS488_POLYROOTS_HPP

#include <stdlib.h>

size_t quadraticRoots(double A, double B, double C, double roots[2]);
size_t cubicRoots(double A, double B, double C, double roots[3]);
size_t quarticRoots(double A, double B, double C, double D, double roots[4]);

#endif /* CS488_POLYROOTS_HPP */

/*
 * Copyright (c) 1990, Graphics and AI Laboratory, University of Washington
 * Copying, use and development for non-commercial purposes permitted.
 *                  All rights for commercial use reserved.
 */
