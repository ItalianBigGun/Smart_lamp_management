#ifndef DATAANALYST_H
#define DATAANALYST_H
#include <iostream>
#include "test.pb.h"

class DataAnalyst
{
public:
    explicit DataAnalyst();
    virtual ~DataAnalyst();
    virtual bool analyze(my::Node msgnode) = 0;
private:
};

#endif // DATAANALYST_H
