#include "OrganFlue.h"

OrganFlue::OrganFlue(StkFloat lowestFreq) : Flute(lowestFreq)
{
    //override some default settings here
    endReflection_ = 0.5;
    jetReflection_ = 0.5;
    noiseGain_     = 0.00;    // Breath pressure random component.
    vibratoGain_   = 0.00;    // Breath periodic vibrato component.
    jetRatio_      = 0.32;

}

void OrganFlue::noteOn(StkFloat frequency, StkFloat amplitude)
{
    qDebug() << "organFlue noteOn; freq = " << frequency << " amp = " << amplitude;
    Flute::noteOn(frequency, 1.0);
}
