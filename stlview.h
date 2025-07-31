#pragma once

#include "csgview.h"

class STLView : public CSGView
{
protected:
    void initObject() override;
    void calcRadius() override;

public:
    STLView(void *geometry, QWidget *parent)
        : CSGView(geometry, parent)
    {}
    ~STLView() = default;
};
