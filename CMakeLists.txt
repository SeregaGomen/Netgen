cmake_minimum_required(VERSION 3.16)

project(Netgen VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS OpenGLWidgets)
find_package(ZLIB REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Netgen
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        csgview.h csgview.cpp


        libsrc/csg/algprim.cpp
        libsrc/csg/algprim.hpp
        libsrc/csg/brick.cpp
        libsrc/csg/brick.hpp
        libsrc/csg/bspline2d.cpp
        libsrc/csg/csg.hpp
        libsrc/csg/csgeom.cpp
        libsrc/csg/csgeom.hpp
        libsrc/csg/csgparser.cpp
        libsrc/csg/csgparser.hpp
        libsrc/csg/curve2d.cpp
        libsrc/csg/curve2d.hpp
        libsrc/csg/edgeflw.cpp
        libsrc/csg/edgeflw.hpp
        libsrc/csg/explicitcurve2d.cpp
        libsrc/csg/explicitcurve2d.hpp
        libsrc/csg/extrusion.cpp
        libsrc/csg/extrusion.hpp
        libsrc/csg/gencyl.cpp
        libsrc/csg/gencyl.hpp
        libsrc/csg/genmesh.cpp
        libsrc/csg/geoml.hpp
        libsrc/csg/identify.cpp
        libsrc/csg/identify.hpp libsrc/csg/manifold.cpp libsrc/csg/manifold.hpp libsrc/csg/meshsurf.cpp libsrc/csg/meshsurf.hpp libsrc/csg/polyhedra.cpp libsrc/csg/polyhedra.hpp libsrc/csg/python_csg.cpp libsrc/csg/revolution.cpp libsrc/csg/revolution.hpp libsrc/csg/singularref.cpp libsrc/csg/singularref.hpp libsrc/csg/solid.cpp libsrc/csg/solid.hpp libsrc/csg/specpoin.cpp libsrc/csg/specpoin.hpp libsrc/csg/spline3d.cpp libsrc/csg/spline3d.hpp libsrc/csg/surface.cpp libsrc/csg/surface.hpp libsrc/csg/triapprox.cpp libsrc/csg/triapprox.hpp libsrc/csg/vscsg.hpp libsrc/csg/zrefine.cpp libsrc/general/archive_base.hpp libsrc/general/array.cpp libsrc/general/array.hpp libsrc/general/autodiff.hpp libsrc/general/autoptr.hpp libsrc/general/bitarray.cpp libsrc/general/bitarray.hpp libsrc/general/dynamicmem.cpp libsrc/general/dynamicmem.hpp libsrc/general/flags.cpp libsrc/general/flags.hpp libsrc/general/gzstream.cpp libsrc/general/gzstream.h libsrc/general/hashtabl.cpp libsrc/general/hashtabl.hpp libsrc/general/mpi_interface.cpp libsrc/general/mpi_interface.hpp libsrc/general/myadt.hpp libsrc/general/mystring.cpp libsrc/general/mystring.hpp libsrc/general/netgenout.hpp libsrc/general/ngexception.cpp libsrc/general/ngexception.hpp libsrc/general/ngpython.hpp libsrc/general/optmem.cpp libsrc/general/optmem.hpp libsrc/general/parthreads.cpp libsrc/general/parthreads.hpp libsrc/general/profiler.cpp libsrc/general/profiler.hpp libsrc/general/seti.cpp libsrc/general/seti.hpp libsrc/general/sort.cpp libsrc/general/sort.hpp libsrc/general/spbita2d.cpp libsrc/general/spbita2d.hpp libsrc/general/stack.hpp libsrc/general/symbolta.cpp libsrc/general/symbolta.hpp libsrc/general/table.cpp libsrc/general/table.hpp libsrc/general/template.hpp libsrc/geom2d/genmesh2d.cpp libsrc/geom2d/geom2dmesh.cpp libsrc/geom2d/geom2dmesh.hpp libsrc/geom2d/geometry2d.cpp libsrc/geom2d/geometry2d.hpp libsrc/geom2d/python_geom2d.cpp libsrc/geom2d/vsgeom2d.hpp libsrc/gprim/adtree.cpp libsrc/gprim/adtree.hpp libsrc/gprim/geom2d.cpp libsrc/gprim/geom2d.hpp libsrc/gprim/geom3d.cpp libsrc/gprim/geom3d.hpp libsrc/gprim/geomfuncs.cpp libsrc/gprim/geomfuncs.hpp libsrc/gprim/geomobjects.hpp libsrc/gprim/geomobjects2.hpp libsrc/gprim/geomops.hpp libsrc/gprim/geomops2.hpp libsrc/gprim/geomtest3d.cpp libsrc/gprim/geomtest3d.hpp libsrc/gprim/gprim.hpp libsrc/gprim/spline.cpp libsrc/gprim/spline.hpp libsrc/gprim/splinegeometry.cpp libsrc/gprim/splinegeometry.hpp libsrc/gprim/transform3d.cpp libsrc/gprim/transform3d.hpp libsrc/include/acisgeom.hpp libsrc/include/csg.hpp libsrc/include/csgeom.hpp libsrc/include/geometry2d.hpp libsrc/include/gprim.hpp libsrc/include/incopengl.hpp libsrc/include/incvis.hpp libsrc/include/linalg.hpp libsrc/include/meshing.hpp libsrc/include/myadt.hpp libsrc/include/mydefs.hpp libsrc/include/mystdlib.h libsrc/include/nginterface.h libsrc/include/nginterface_v2.hpp libsrc/include/nginterface_v2_impl.hpp libsrc/include/opti.hpp libsrc/include/parallel.hpp libsrc/include/stlgeom.hpp libsrc/include/visual.hpp libsrc/interface/nginterface.cpp libsrc/interface/nginterface_v2.cpp libsrc/interface/read_fnf_mesh.cpp libsrc/interface/readtetmesh.cpp libsrc/interface/readuser.cpp libsrc/interface/writeabaqus.cpp libsrc/interface/writediffpack.cpp libsrc/interface/writedolfin.cpp libsrc/interface/writeelmer.cpp libsrc/interface/writefeap.cpp libsrc/interface/writefluent.cpp libsrc/interface/writegmsh.cpp libsrc/interface/writegmsh2.cpp libsrc/interface/writejcm.cpp libsrc/interface/writeOpenFOAM15x.cpp libsrc/interface/writepermas.cpp libsrc/interface/writetecplot.cpp libsrc/interface/writetet.cpp libsrc/interface/writetochnog.cpp libsrc/interface/writeuser.cpp libsrc/interface/writeuser.hpp libsrc/interface/wuchemnitz.cpp libsrc/linalg/bfgs.cpp libsrc/linalg/densemat.cpp libsrc/linalg/densemat.hpp libsrc/linalg/linalg.hpp libsrc/linalg/linopt.cpp libsrc/linalg/linsearch.cpp libsrc/linalg/opti.hpp libsrc/linalg/polynomial.cpp libsrc/linalg/polynomial.hpp libsrc/linalg/vector.hpp libsrc/meshing/adfront2.cpp libsrc/meshing/adfront2.hpp libsrc/meshing/adfront3.cpp libsrc/meshing/adfront3.hpp libsrc/meshing/basegeom.cpp libsrc/meshing/basegeom.hpp libsrc/meshing/bcfunctions.cpp libsrc/meshing/bcfunctions.hpp libsrc/meshing/bisect.cpp libsrc/meshing/bisect.hpp libsrc/meshing/boundarylayer.cpp libsrc/meshing/boundarylayer.hpp libsrc/meshing/classifyhpel.hpp libsrc/meshing/clusters.cpp libsrc/meshing/clusters.hpp libsrc/meshing/curvedelems.cpp libsrc/meshing/curvedelems.hpp libsrc/meshing/delaunay.cpp libsrc/meshing/delaunay2d.cpp libsrc/meshing/findip.hpp libsrc/meshing/findip2.hpp libsrc/meshing/geomsearch.cpp libsrc/meshing/geomsearch.hpp libsrc/meshing/global.cpp libsrc/meshing/global.hpp libsrc/meshing/hpref_hex.hpp libsrc/meshing/hpref_prism.hpp libsrc/meshing/hpref_pyramid.hpp libsrc/meshing/hpref_quad.hpp libsrc/meshing/hpref_segm.hpp libsrc/meshing/hpref_tet.hpp libsrc/meshing/hpref_trig.hpp libsrc/meshing/hprefinement.cpp libsrc/meshing/hprefinement.hpp libsrc/meshing/improve2.cpp libsrc/meshing/improve2.hpp libsrc/meshing/improve2gen.cpp libsrc/meshing/improve3.cpp libsrc/meshing/improve3.hpp libsrc/meshing/localh.cpp libsrc/meshing/localh.hpp libsrc/meshing/meshclass.cpp libsrc/meshing/meshclass.hpp libsrc/meshing/meshfunc.cpp libsrc/meshing/meshfunc.hpp libsrc/meshing/meshfunc2d.cpp libsrc/meshing/meshing.hpp libsrc/meshing/meshing2.cpp libsrc/meshing/meshing2.hpp libsrc/meshing/meshing3.cpp libsrc/meshing/meshing3.hpp libsrc/meshing/meshtool.cpp libsrc/meshing/meshtool.hpp libsrc/meshing/meshtype.cpp libsrc/meshing/meshtype.hpp libsrc/meshing/msghandler.cpp libsrc/meshing/msghandler.hpp libsrc/meshing/netrule2.cpp libsrc/meshing/netrule3.cpp libsrc/meshing/parallelmesh.cpp libsrc/meshing/paralleltop.cpp libsrc/meshing/paralleltop.hpp libsrc/meshing/parser2.cpp libsrc/meshing/parser3.cpp libsrc/meshing/prism2rls.cpp libsrc/meshing/pyramid2rls.cpp libsrc/meshing/pyramidrls.cpp libsrc/meshing/python_mesh.cpp libsrc/meshing/quadrls.cpp libsrc/meshing/refine.cpp libsrc/meshing/ruler2.cpp libsrc/meshing/ruler2.hpp libsrc/meshing/ruler3.cpp libsrc/meshing/ruler3.hpp libsrc/meshing/secondorder.cpp libsrc/meshing/smoothing2.5.cpp libsrc/meshing/smoothing2.cpp libsrc/meshing/smoothing3.cpp libsrc/meshing/specials.cpp libsrc/meshing/specials.hpp libsrc/meshing/tetrarls.cpp libsrc/meshing/topology.cpp libsrc/meshing/topology.hpp libsrc/meshing/triarls.cpp libsrc/meshing/validate.cpp libsrc/meshing/validate.hpp libsrc/stlgeom/meshstlsurface.cpp libsrc/stlgeom/meshstlsurface.hpp libsrc/stlgeom/stlgeom.cpp libsrc/stlgeom/stlgeom.hpp libsrc/stlgeom/stlgeomchart.cpp libsrc/stlgeom/stlgeommesh.cpp libsrc/stlgeom/stlline.cpp libsrc/stlgeom/stlline.hpp libsrc/stlgeom/stltool.cpp libsrc/stlgeom/stltool.hpp libsrc/stlgeom/stltopology.cpp libsrc/stlgeom/stltopology.hpp libsrc/stlgeom/vsstl.hpp

        meshview.h meshview.cpp
        stlview.h stlview.cpp
        csgsetup.ui csgsetup.h csgsetup.cpp
        meshsetup.ui meshsetup.h meshsetup.cpp
        threads.h threads.cpp
        defs.h
        ng.cpp
        ng.h
        redirector.h
        Netgen.qrc
        imagesetup.h imagesetup.cpp imagesetup.ui
        meshinfo.h meshinfo.cpp meshinfo.ui

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Netgen APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Netgen SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Netgen
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Netgen PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Netgen PRIVATE Qt${QT_VERSION_MAJOR}::OpenGLWidgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Netgen)
endif()
set_target_properties(Netgen PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


target_link_libraries(Netgen  PRIVATE ZLIB::ZLIB)


#set(MPI_INCLUDE_DIR /opt/intel/oneapi/mpi/2021.15/include/)
#set(MPI_LIBRARIES /opt/intel/oneapi/2021.1/lib/mpifort /opt/intel/oneapi/2021.1/lib/mpi)

#include_directories("../../eigen/")
include_directories("libsrc/include/")
# include_directories("libsrc/")

include(GNUInstallDirs)
install(TARGETS Netgen
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)



if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Netgen)
endif()
