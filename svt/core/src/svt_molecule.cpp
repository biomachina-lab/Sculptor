/***************************************************************************
                          svt_molecule.cpp  -  description
                             -------------------
    begin                : Jul 05 14:26:21 CET 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include "svt.h"

void svt_molecule::calcInterMolForce()
{
    svt_atom* first = (svt_atom*)this->getSon();

    first->calcForce();
};

void svt_molecule::calcForce()
{
    force_x = 0;
    force_y = 0;
    force_z = 0;

    svt_atom* current = (svt_atom*)this->getSon();

    while(current != NULL)
    {
        force_x += current->getForceX();
        force_y += current->getForceY();
        force_z += current->getForceZ();

        current = (svt_atom*)current->getNext();
    }
};

void svt_molecule::absolutePos()
{
    svt_atom* current = (svt_atom*)this->getSon();

    while(current != NULL)
    {
        current->pos_x += SVT_GLTOAX(this->getPos()->getX());
        current->pos_y += SVT_GLTOAY(this->getPos()->getY());
        current->pos_z += SVT_GLTOAZ(this->getPos()->getZ());

        current = (svt_atom*)current->getNext();
    }
};

void svt_molecule::localePos()
{
    svt_atom* current = (svt_atom*)this->getSon();

    while(current != NULL)
    {
        current->pos_x -= SVT_GLTOAX(this->getPos()->getX());
        current->pos_y -= SVT_GLTOAY(this->getPos()->getY());
        current->pos_z -= SVT_GLTOAZ(this->getPos()->getZ());

        current = (svt_atom*)current->getNext();
    }
};

void svt_molecule::initForces()
{
    svt_atom* current = (svt_atom*)this->getSon();

    while(current != NULL)
    {
        current->initForce();
        current = (svt_atom*)current->getNext();
    }
};

void svt_molecule::updateVerlet()
{
    svt_atom* first = (svt_atom*)this->getSon();

    first->updateVerlet();
};

void svt_molecule::calcMolForce(svt_molecule* mol)
{
    svt_atom* first1 = (svt_atom*)this->getSon();
    svt_atom* first2 = (svt_atom*)mol->getSon();

    first1->calcGroupForce(first2);
};
