/***************************************************************************
                          svt_pm  -  description
                             -------------------
    begin                : 22.02.2003
    author               : Maik Boltes
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt_includes
#include <svt_vertex_array.h>
#include <svt_mesh.h>
#include <svt_pm.h>

svt_pm_ver& svt_pm_ver::operator=(const svt_vertex_array_element& rhs)
{
    pkt = rhs.pkt;
    norm = rhs.norm;
    tex[0] = rhs.tex[0];
    tex[1] = rhs.tex[1];
    col = rhs.col;
    return *this;
}

svt_pm_ver& svt_pm_ver::operator*=(float rhs)
{
    pkt *= rhs;
    norm *= rhs;
    tex[0] *= rhs;
    tex[1] *= rhs;
    //col?
    return *this;
}

svt_pm_ver svt_pm_ver::operator*(float rhs)
{
    return svt_pm_ver(*this) *= rhs;
}

svt_pm_ver& svt_pm_ver::operator+=(const svt_pm_ver& rhs)
{
    pkt += rhs.pkt;
    norm += rhs.norm;    
    tex[0] += rhs.tex[0];
    tex[1] += rhs.tex[1];
    //col?
    return *this;
}

svt_pm_ver svt_pm_ver::operator+(const svt_pm_ver& rhs)
{
    return svt_pm_ver(*this) += rhs;
}

svt_pm::svt_pm() 
{ 
    akt = begin();     
    m_geomorph = false;
    m_geomorphSteps = 10;
}

svt_pm::svt_pm(const svt_pm& rhs) : list<svt_pm_elem>(rhs)
{ 
    //cout << "in : svt_pm copy constructor" << endl;
    akt = begin();
    // den akt-zeiger auf die richtige/gleiche stelle in liste setzen
    while ((akt != end()) && (akt->v_new.v != rhs.akt->v_new.v)) // end() darf nie einreten
        ++akt;
    m_geomorph = rhs.m_geomorph;
    m_geomorphSteps = rhs.m_geomorphSteps;
    //cout << "out: svt_pm copy constructor" << endl;
}

// rueckgabewert zeigt an, ob verfeinert werden konnte
bool svt_pm::split(svt_mesh &mesh) //, int anz
{
// anz noch nicht implementiert, sollte fuer geomorph in diese fkt integriert werden
    list<svt_pm_tri>::iterator it; 
    list<int>::iterator itint; 
    int i;

    if (akt == end()) // zur sicherheit, falls trotz vorherigem rueckgabewert false ein weiterer aufruf erfolgt
        return false;
    else
    {
        mesh.setVerNumber(mesh.getVerNumber() + 1);

        // dreiecke bei denen v1 zu v2 bzw v zu v_new wird
        for (itint = akt->t_switch.begin(); itint != akt->t_switch.end(); ++itint)
        {
            // dreieck anpassen
            for (i=0; i<3; i++)
                if (akt->v.v == mesh.m_oTriangles[*itint].ver[i])
                    mesh.m_oTriangles[*itint].ver[i] = akt->v_new.v;

            // dreieck zur dreiecksliste des neuen knoten hinzufuegen
            mesh.m_oVertices[akt->v_new.v].tri.push_back(*itint);
            // dreieck aus dreiecksliste von zu spaltendem knoten wegnehmen
            mesh.m_oVertices[akt->v.v].tri.remove(*itint);
        }

        // neu entstehende dreiecke eintragen
        for(it = akt->t_new.begin(); it != akt->t_new.end(); ++it)
        {
            mesh.setTriNumber(mesh.getTriNumber() + 1);
            // dreieck anpassen
            for (i=0; i<3; i++)
                mesh.m_oTriangles[it->t].ver[i] = it->ver[i];
            mesh.m_oTriangles[it->t].in = true;

            // dreieck zur dreiecksliste des neuen knoten, von zu spaltendem knoten und von einem weiteren punkt hinzufuegen
            for (i=0; i<3; i++)
                mesh.m_oVertices[mesh.m_oTriangles[it->t].ver[i]].tri.push_back(it->t);
        }

        if (m_geomorph)
        {            
            svt_pm_ver merk_ver;

            // weiches blenden: geomorph
            merk_ver = mesh.m_oVertices[akt->v.v];
            for (i=1; i<=m_geomorphSteps; i++)
            {
                mesh.m_oVertices[akt->v.v] = merk_ver*((m_geomorphSteps-i) / (float)m_geomorphSteps) + (akt->v)*(i / (float)m_geomorphSteps);
                mesh.m_oVertices[akt->v_new.v] = merk_ver*((m_geomorphSteps-i) / (float)m_geomorphSteps) + (akt->v_new)*(i / (float)m_geomorphSteps);

                // dreiecksnormale neu berechnen bei denen v1 zu v2 bzw v zu v_new wird
                for (itint = akt->t_switch.begin(); itint != akt->t_switch.end(); ++itint)
                    mesh.calcTriNormal(*itint);

                // dreiecksnormalen um bereits bestandenen pkt neu berechnen
                // (incl. neu entstandener dreiecke)
                for (itint = mesh.m_oVertices[akt->v.v].tri.begin(); 
                     itint != mesh.m_oVertices[akt->v.v].tri.end(); 
                     ++itint)
                    mesh.calcTriNormal(*itint);

                mesh.rebuildDL();
            }
        }
        else
        {
            // alle gespeicherten knotendaten umkopieren
            mesh.m_oVertices[akt->v.v] = akt->v;
            mesh.m_oVertices[akt->v_new.v] = akt->v_new;

            // dreiecksnormale neu berechnen bei denen v1 zu v2 bzw v zu v_new wird
            for (itint = akt->t_switch.begin(); itint != akt->t_switch.end(); ++itint)
                mesh.calcTriNormal(*itint);

            // dreiecksnormalen um bereits bestandenen pkt neu berechnen
            // (incl. neu entstandener dreiecke)
            for (itint = mesh.m_oVertices[akt->v.v].tri.begin(); 
                 itint != mesh.m_oVertices[akt->v.v].tri.end(); 
                 ++itint)
                mesh.calcTriNormal(*itint);
        }

        ++akt;

        if (akt == end())
            return false;
        else
            return true;
    }
}

// rueckgabewert zeigt an, ob vergroebert werden konnte
bool svt_pm::contract(svt_mesh &mesh) // , int anz (anz per default 1)
{
// anz noch nicht implementiert, sollte fuer geomorph in diese fkt integriert werden
    list<svt_pm_tri>::iterator it; 
    list<int>::iterator itint; 
    int i;

    if (akt == begin()) // zur sicherheit, falls trotz vorherigem rueckgabewert false ein weiterer aufruf erfolgt
        return false;
    else
    {
        --akt;

        mesh.setVerNumber(mesh.getVerNumber() - 1);

        mesh.m_oVertices[akt->v.v].pkt = akt->v_merge;

        // Texturkoordinate mitteln
        for (i = 0; i < 2; i++)
            mesh.m_oVertices[akt->v.v].tex[i] = (mesh.m_oVertices[akt->v.v].tex[i] + mesh.m_oVertices[akt->v_new.v].tex[i])/2.;

        // Farbwerte mitteln
        if (mesh.getVertexColored())
        {
            mesh.m_oVertices[akt->v.v].col.setR((mesh.m_oVertices[akt->v.v].col.getR()+mesh.m_oVertices[akt->v_new.v].col.getR())/2.);
            mesh.m_oVertices[akt->v.v].col.setG((mesh.m_oVertices[akt->v.v].col.getG()+mesh.m_oVertices[akt->v_new.v].col.getG())/2.);
            mesh.m_oVertices[akt->v.v].col.setB((mesh.m_oVertices[akt->v.v].col.getB()+mesh.m_oVertices[akt->v_new.v].col.getB())/2.);
        }

        // dreiecke bei denen v2 auf v1 bzw v_new auf v faellt
        for(itint = akt->t_switch.begin(); itint != akt->t_switch.end(); ++itint)
        {
            // dreieck anpassen
            for (i=0; i<3; i++)
                if (mesh.m_oTriangles[*itint].ver[i] == akt->v_new.v)
                    mesh.m_oTriangles[*itint].ver[i] = akt->v.v;

            // dreieck zur dreiecksliste des knotens, auf den beide zusammengefallenen sind, hinzufuegen
            mesh.m_oVertices[akt->v.v].tri.push_back(*itint);
            // dreieck zur aus dem zu entfernenden pktes austragen
            mesh.m_oVertices[akt->v_new.v].tri.remove(*itint);
        }

        // dreiecke austragen
        for(it = akt->t_new.begin(); it != akt->t_new.end(); ++it)
        {
            mesh.setTriNumber(mesh.getTriNumber() - 1);
            // dreieck anpassen
            mesh.m_oTriangles[it->t].in = false;

            // dreieck aus dreiecksliste des neuen knoten, von zu spaltendem knoten und von einem weiteren punkt austragen
            for (i=0; i<3; i++)
                mesh.m_oVertices[mesh.m_oTriangles[it->t].ver[i]].tri.remove(it->t);
        }

        // dreiecksnormalen um verbleibenden pkt neu berechnen
        for (itint = mesh.m_oVertices[akt->v.v].tri.begin(); 
             itint != mesh.m_oVertices[akt->v.v].tri.end(); 
             ++itint)
            mesh.calcTriNormal(*itint);

        // normale am knoten anpassen 
        mesh.calcVerNormal(akt->v.v);

        if (akt == begin())
            return false;
        else
            return true;
    }
}

bool svt_pm::save(svt_mesh &mesh, const char *saveFile)
{
    ofstream out(saveFile);

    if (!out)
        return false; // Fehler aufgetreten

    cout << "svt_pm> Writing " << saveFile << "... " << flush;

    out << mesh.getVerOrigNumber() << " " << mesh.getTriOrigNumber();
    if (mesh.getVertexColored())
        out << " c" << endl; // c=colored
    else
        out << " u" << endl; // u=unique
    out << *this <<endl; 

    cout << mesh.getVerOrigNumber() << " vertices, " << mesh.getTriOrigNumber() << " triangles finished." << endl;

    return true;
}

bool svt_pm::read(svt_mesh &mesh, const char *readFile)
{
    int verOrigNumber, triOrigNumber;
    char colorFlag;

    ifstream in(readFile);
    if (!in)
        return false; // Fehler aufgetreten

    cout << "svt_pm> Reading " << readFile << "... " << flush;

    in >> verOrigNumber >> triOrigNumber >> colorFlag;
    if ((colorFlag != 'u') && (colorFlag != 'c'))
    {
        cout << "not supported PM Format (possibly old version)!" << endl << flush;
        return false;
    }

    mesh.resizeVertex(verOrigNumber);
    mesh.resizeTriangle(triOrigNumber);
    if (colorFlag == 'c')
        mesh.setVertexColored(true);
    else
        mesh.setVertexColored(false);

    in >> *this;
    akt = begin(); // akt am anfang der pm-liste
    // da zu beginn der pm-liste einzelne punkte schon vorliegen, die
    // nicht aufeinandergefallen sind: bei jedem collapse verschwindet
    // genau ein knoten; nicht zusammenhaengende bereiche koennen
    // mgl nicht zusammenfallen, so dass mehr als ein knoten uebrigbleibt
    mesh.setVerNumber(verOrigNumber-this->size());

    if (colorFlag == 'c')
        cout << verOrigNumber << " vertices (colored), ";
    else
        cout << verOrigNumber << " vertices, ";
    cout << triOrigNumber << " triangles... initialize... ";

    // aufbau des originalnetzes
    while(split(mesh))
        ;
    mesh.setRuleVertexNumbers(mesh.getVerNumber());

    mesh.calcMinMaxAll();
    mesh.setMinX(mesh.getMinXAll());
    mesh.setMinY(mesh.getMinYAll()); 
    mesh.setMinZ(mesh.getMinZAll());
    mesh.setMaxX(mesh.getMaxXAll()); 
    mesh.setMaxY(mesh.getMaxYAll()); 
    mesh.setMaxZ(mesh.getMaxZAll());
 
    cout << "finished!" << endl;

    return true;
}

