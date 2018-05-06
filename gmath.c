#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gmath.h"
#include "matrix.h"
#include "ml6.h"

//lighting functions
color get_lighting( double *normal, double *view, color alight, double light[2][3], double *areflect, double *dreflect, double *sreflect) {
  color i;

  color a = calculate_ambient(alight,areflect);
  color d = calculate_diffuse(light,dreflect,normal);
  color s = calculate_specular(light,sreflect,view,normal);

  i.red = a.red + d.red + s.red;
  i.green = a.green + d.green + s.green;
  i.blue = a.blue + d.blue + s.blue;

  limit_color(&i);
  
  return i;
}

color calculate_ambient(color alight, double *areflect ) {
  color a;
  a.red = (int)alight.red * areflect[RED];
  a.green = (int)alight.green * areflect[GREEN];
  a.blue = (int)alight.blue * areflect[BLUE];
  return a;
}

color calculate_diffuse(double light[2][3], double *dreflect, double *normal ) {
  color d;

  normalize(normal);
  normalize(light[LOCATION]);
  
  double prod = dot_product(light[LOCATION],normal);

  d.red = light[COLOR][RED] * dreflect[RED] * prod;
  d.green = light[COLOR][GREEN] * dreflect[GREEN] * prod;
  d.blue = light[COLOR][BLUE] * dreflect[BLUE] * prod;

  limit_color(&d);
  
  return d;
}

color calculate_specular(double light[2][3], double *sreflect, double *view, double *normal ) {
  color s;

  normalize(normal);
  normalize(light[LOCATION]);
  normalize(view);
  
  double scale = dot_product(normal,light[LOCATION]);
  
  double *t = (double *)malloc(3 * sizeof(double));
  int i;
  for(i = 0; i < 3; i++)
    t[i] = scale * normal[i];


  double *r = (double *)malloc(3 * sizeof(double));
  for(i = 0; i < 3; i++)
    r[i] = (2 * t[i]) - light[LOCATION][i];

  double cosA = dot_product(r,view);

  if(cosA < 0){
    s.red = 0;
    s.green = 0;
    s.blue = 0;
  }
  else{
    s.red = light[COLOR][RED] * sreflect[RED] * pow(cosA,6);
    s.green = light[COLOR][GREEN] * sreflect[GREEN] * pow(cosA,6);
    s.blue = light[COLOR][BLUE] * sreflect[BLUE] * pow(cosA,6);
  }

  free(t);
  free(r);

  limit_color(&s);
  
  return s;
}


//limit each component of c to a max of 255
void limit_color( color * c ) {
  if(c->red > 255)
    c->red = 255;
  if(c->green > 255)
    c->green = 255;
  if(c->blue > 255)
    c->blue = 255;
  if(c->red < 0)
    c->red = 0;
  if(c->green < 0)
    c->green = 0;
  if(c->blue < 0)
    c->blue = 0;
}

//vector functions
//normalize vector, should modify the parameter
void normalize( double *vector ) {
  double x = vector[0];
  double y = vector[1];
  double z = vector[2];

  double mag = sqrt( (x * x) + (y * y) + (z * z));

  vector[0] = x / mag;
  vector[1] = y / mag;
  vector[2] = z / mag;

}

//Return the dot product of a . b
double dot_product( double *a, double *b ) {
  double ax = a[0];
  double ay = a[1];
  double az = a[2];

  double bx = b[0];
  double by = b[1];
  double bz = b[2];

  double prod = (ax * bx) + (ay * by) + (az * bz);
  
  return prod;
}

double *calculate_normal(struct matrix *polygons, int i) {

  double A[3];
  double B[3];
  double *N = (double *)malloc(3 * sizeof(double));

  A[0] = polygons->m[0][i+1] - polygons->m[0][i];
  A[1] = polygons->m[1][i+1] - polygons->m[1][i];
  A[2] = polygons->m[2][i+1] - polygons->m[2][i];

  B[0] = polygons->m[0][i+2] - polygons->m[0][i];
  B[1] = polygons->m[1][i+2] - polygons->m[1][i];
  B[2] = polygons->m[2][i+2] - polygons->m[2][i];

  N[0] = A[1] * B[2] - A[2] * B[1];
  N[1] = A[2] * B[0] - A[0] * B[2];
  N[2] = A[0] * B[1] - A[1] * B[0];

  return N;
}
