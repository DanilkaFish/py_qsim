#ifndef _DRAWER
#define _DRAWER

#include "common.h"


struct QC_representation{
    Qubits qubits;
    std::string s;
};

class Drawer{
public:
    virtual void draw(const QC_representation& qcr){};
};

typedef std::shared_ptr<Drawer> DrawerPtr;



#endif 