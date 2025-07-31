#include "threads.h"
#include "csg.hpp"
#include "ng.h"
#include "stlgeom.hpp"

using namespace netgen;

// Запуск построения сетки
void NgThread::run(void)
{
    try {
        if (geometry == nullptr)
            generated = !generateVolumeMesh(mesh) ? true : false;
        else
            generated = !generateMesh(geometry, mesh) ? true : false;
    } catch (...) {
        cerr << "Meshing error!" << endl;
        return;
    }
    cout << "------------------------------------------------" << endl;
    cout << "Points:   " << mesh->GetNP() << endl;
    cout << "Elements: " << mesh->GetNE() << endl;
}
