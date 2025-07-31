#include "csg.hpp"
#include "nginterface.h"
#include "stlgeom.hpp"

namespace netgen {
CSGeometry *ParseCSG(istream &);
}

using namespace netgen;

void scanCSG(shared_ptr<NetgenGeometry> &geometry,
             double minX[3],
             double maxX[3],
             double detail,
             double facets)
{
    Box<3> box(Point<3>(minX[0], minX[1], minX[2]), Point<3>(maxX[0], maxX[1], maxX[2]));

    static_cast<CSGeometry *>(geometry.get())
        ->FindIdenticSurfaces(1e-8 * (static_cast<CSGeometry *>(geometry.get())->MaxSize()));
    static_cast<CSGeometry *>(geometry.get())->SetBoundingBox(box);
    static_cast<CSGeometry *>(geometry.get())->CalcTriangleApproximation(detail, facets);
}

bool loadCSG(string file,
             shared_ptr<NetgenGeometry> &geometry,
             double minX[3],
             double maxX[3],
             double detail,
             double facets)
{
    ifstream in(file);
    Box<3> box(Point<3>(minX[0], minX[1], minX[2]), Point<3>(maxX[0], maxX[1], maxX[2]));
    //CSGeometry *ParseCSG(istream&);

    geometry = make_shared<NetgenGeometry>();
    try {
        geometry = shared_ptr<NetgenGeometry>(ParseCSG(in));
    } catch (...) {
        cerr << "Problem in CSG-file!" << endl;
        return false;
    }

    if (!geometry.get()) {
        cout << "geo-file should start with 'algebraic3d'" << endl;
        return false;
    }
    static_cast<CSGeometry *>(geometry.get())
        ->FindIdenticSurfaces(1e-8 * static_cast<CSGeometry *>(geometry.get())->MaxSize());
    static_cast<CSGeometry *>(geometry.get())->SetBoundingBox(box);
    static_cast<CSGeometry *>(geometry.get())->CalcTriangleApproximation(detail, facets);
    return true;
}

bool loadSTL(string file, shared_ptr<NetgenGeometry> &geometry)
{
    ifstream in(file);

    geometry = make_shared<NetgenGeometry>();
    try {
        geometry = shared_ptr<NetgenGeometry>(STLGeometry::Load(in));
    } catch (...) {
        cerr << "Problem in STL-file!" << endl;
        return false;
    }
    if (!static_cast<STLGeometry *>(geometry.get())->GetNT()) {
        cerr << "Problem in STL-file!" << endl;
        return false;
    }
    return true;
}

bool loadMesh(string fileName, shared_ptr<Mesh> &mesh)
{
    mesh = make_shared<Mesh>();
    try {
        mesh->Load(fileName);
    } catch (...) {
        cerr << "Problem in VOL-file!" << endl;
        return false;
    }
    return true;
}

void setTerminate(int terminate)
{
    multithread.terminate = terminate;
}

void refinementMesh(shared_ptr<NetgenGeometry> &geometry, shared_ptr<Mesh> &mesh)
{
    geometry->GetRefinement().Refine(*mesh);
    cout << "Elements after refinement: " << mesh->GetNP() << endl;
    cout << "Points   after refinement: " << mesh->GetNE() << endl;
}

void refineSecondOrder(shared_ptr<NetgenGeometry> &geometry, shared_ptr<Mesh> &mesh)
{
    geometry->GetRefinement().MakeSecondOrder(*mesh);
    cout << "Elements after refinement: " << mesh->GetNP() << endl;
    cout << "Points   after refinement: " << mesh->GetNE() << endl;
}

void refineHighOrder(shared_ptr<NetgenGeometry> &geometry, shared_ptr<Mesh> &mesh, int order)
{
    mparam.elementorder = order;
    mesh->GetCurvedElements().BuildCurvedElements(&geometry->GetRefinement(), mparam.elementorder);
    mesh->SetNextMajorTimeStamp();
    cout << "Elements after refinement: " << mesh->GetNP() << endl;
    cout << "Points   after refinement: " << mesh->GetNE() << endl;
}

void refineBisection(shared_ptr<NetgenGeometry> &geometry, shared_ptr<Mesh> &mesh)
{
    BisectionOptions biopt;

    if (!mesh->LocalHFunctionGenerated())
        mesh->CalcLocalH(mparam.grading);
    mesh->LocalHFunction().SetGrading(mparam.grading);
    geometry->GetRefinement().Bisect(*mesh, biopt);
    mesh->UpdateTopology();
    mesh->GetCurvedElements().BuildCurvedElements(&geometry->GetRefinement(), mparam.elementorder);
}

// Generates volume mesh from an existing surface mesh
int generateVolumeMesh(shared_ptr<Mesh> &mesh)
{
    MeshVolume(mparam, *mesh.get());
    RemoveIllegalElements(*mesh.get());
    OptimizeVolume(mparam, *mesh.get());
    return 0;
}

int generateMesh(shared_ptr<NetgenGeometry> &geometry, shared_ptr<Mesh> &mesh)
{
    return geometry->GenerateMesh(mesh, mparam, 1, 10);
}
