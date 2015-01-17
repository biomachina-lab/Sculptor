/***************************************************************************
                          svt_scenegraph.cpp  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_core.h>
#include <svt_scenegraph.h>
#include <svt_init.h>
#include <svt_window.h>

#include <svt_iostream.h>

svt_scenegraph::svt_scenegraph()
{
    setSon(NULL);
    setName("svt_scenegraph");
}

svt_scenegraph::~svt_scenegraph()
{
    if (getSon() != NULL)
	delete getSon();
}

/* Print the structure of the scenegraph */
void svt_scenegraph::printGraph(){
	
    printName();
    cout << "|" << endl;
    cout << "*-*" << endl;

    if (getSon() != NULL)
	printGraph(getSon(),1);
    else
	cout << "no content in the scenegraph!" << endl;
}

/* Print the structure of the scenegraph starting with a special node and in a special hierarchy-level */
void svt_scenegraph::printGraph(svt_node* start, int hier)
{
    svt_node* temp = start;

    //Print data of the current node
    for(int i=0;i<hier;i++) cout << "| ";
    temp->printName();

    //Is there a deeper hierarchy level?
    if (temp->getSon() != NULL){
	for(int i=0;i<hier;i++) cout << "| ";
	cout << "|" << endl;
	for(int j=0;j<hier;j++) cout << "| ";
	cout << "*-*" << endl;
	printGraph(temp->getSon(),hier+1);
    }

    //Is there something left to print?
    if (temp->getNext() != NULL){
	for(int i=0;i<hier;i++) cout << "| ";
	cout << "|" << endl;
	printGraph(temp->getNext(),hier);
    }	else if (hier != 0){
	for(int i=0;i<hier-1;i++) cout << "| ";
	cout << "*-*" << endl;
    }
}

/**
 * vrml drawing routine
 */
void svt_scenegraph::drawVrml()
{
    cout << "#VRML V2.0 utf8" << endl;
}
