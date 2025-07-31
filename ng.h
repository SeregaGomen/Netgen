#pragma once

#include "nginterface.h"
#include "csg.hpp"
#include "stlgeom.hpp"

void scanCSG(shared_ptr<netgen::NetgenGeometry>&, double[3], double[3], double, double);
void setTerminate(int);
void refinementMesh(shared_ptr<netgen::NetgenGeometry>&, shared_ptr<netgen::Mesh>&);
void refineSecondOrder(shared_ptr<netgen::NetgenGeometry>&, shared_ptr<netgen::Mesh>&);
void refineHighOrder(shared_ptr<netgen::NetgenGeometry>&, shared_ptr<netgen::Mesh>&, int);
void refineBisection(shared_ptr<netgen::NetgenGeometry>&, shared_ptr<netgen::Mesh>&);
bool loadCSG(string, shared_ptr<netgen::NetgenGeometry>&, double[3], double[3], double, double);
bool loadSTL(string, shared_ptr<netgen::NetgenGeometry>&);
bool loadMesh(string, shared_ptr<netgen::Mesh>&);
int generateVolumeMesh(shared_ptr<netgen::Mesh>&);
int generateMesh(shared_ptr<netgen::NetgenGeometry>&, shared_ptr<netgen::Mesh>&);

