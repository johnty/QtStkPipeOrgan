#ifndef ORGANFLUE_H
#define ORGANFLUE_H

#include "Flute.h"
#include "Stk.h"
#include <QDebug>

using namespace stk;

class OrganFlue: public stk::Flute
{
public:
    OrganFlue(StkFloat lowestFreq = 16.0);

    void noteOn( StkFloat frequency, StkFloat amplitude );

//signals:

//public slots:
};

#endif // ORGANFLUE_H
