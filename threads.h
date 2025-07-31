#pragma once

#include <QThread>

class QWidget;
namespace netgen {
class Mesh;
class NetgenGeometry;
}; // namespace netgen

class NgThread : public QThread
{
private:
    std::shared_ptr<netgen::Mesh> mesh;
    std::shared_ptr<netgen::NetgenGeometry> geometry;
    bool generated{false};

protected:
    void run();

public:
    explicit NgThread(std::shared_ptr<netgen::Mesh> &mesh,
                      std::shared_ptr<netgen::NetgenGeometry> &geometry)
        : QThread()
        , mesh(mesh)
        , geometry(geometry)
    {}
    ~NgThread() = default;
    bool isGenerated() { return generated; }
};
