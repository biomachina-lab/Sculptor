/***************************************************************************
                          svt_pm  -  description
                             -------------------
    begin                : 22.02.2003
    author               : Maik Boltes
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_PM_H
#define SVT_PM_H

#include <svt_core.h>
#include <svt_init.h>
#include <svt_types.h>
#include <svt_node.h>
#include <svt_stlList.h>
#include <svt_vector3.h>
#include <svt_stlString.h>
#include <svt_iostream.h>
#include <svt_strstream.h>

//#pragma warning (disable: 4275)

//forward declaration
// from svt_vertex_array
class svt_vertex_array_element;
// from svt_mesh
class svt_mesh;

class svt_pm_ver {          // Knotenstruktur
public:
    UInt32 v;                  // Nummern des Knoten (beginnt bei 0, in smf-datei ab 1)
    svt_vector3<double> pkt;   // Punktkoordinaten
    svt_vector3<double> norm;  // Normale am Knoten zum glatten shading bei Zeichnung (koennte man auch berechnen!!!!)
    Point2f tex;               // Texturkoordinate
    svt_color col;             // Farbe

    svt_pm_ver()
    {
        v = 0;
        tex[0] = 0.0;
        tex[1] = 0.0;
    }

    svt_pm_ver(const svt_pm_ver& v)
    {
        this->v = v.v;
        pkt = v.pkt;
        norm = v.norm;
        tex[0] = v.tex[0];
        tex[1] = v.tex[1];
        col = v.col;
    }

    svt_pm_ver& operator=(const svt_vertex_array_element& rhs);
    svt_pm_ver& operator*=(float rhs);
    svt_pm_ver  operator*(float rhs);
    svt_pm_ver& operator+=(const svt_pm_ver& rhs);
    svt_pm_ver  operator+(const svt_pm_ver& rhs);

    // braucht VC7 um remove zu implementieren; wird nicht explizit im Code benutzt
    bool operator== (const svt_pm_ver& x) const
    {
        return (v == x.v) && (pkt == x.pkt) && (norm == x.norm) && (tex == x.tex) ;
    }
};

class DLLEXPORT svt_pm_tri {          // Dreieckstruktur
public:
    UInt32 t;        // Nummer des Dreiecks
    UInt32 ver[3];   // Eckpunktnummern (nicht noetig fuer t_switch, dort muesste nur v zu v_new gemacht gemacht werden!!!) (beginnt bei 0, in smf-datei ab 1)
    //    svt_vector3<double> norm;  // Normale  wird nun immer neu berechnet

    // braucht VC7 um remove zu implementieren, was aber nicht gebraucht wird; wird nicht explizit im Code benutzt
    bool operator== (const svt_pm_tri& x) const
    {
        return (t == x.t) && (ver == x.ver);
    }
};

class DLLEXPORT svt_pm_elem {  
public:
    svt_pm_ver v, v_new;         // originalknoten vor verschmelzung
    svt_vector3<double> v_merge; // verschmelzungsknoten
    list<svt_pm_tri> t_new;      // liste der neu hinzuzufuegenden dreiecke bei expansion
    list<int> t_switch;          // liste der sich veraendernden dreiecke

    // braucht VC7 um remove zu implementieren // wird nicht wirklich benoetigt
    bool operator==(const svt_pm_elem& rhs) const
    {
        return (v == rhs.v) && (v_new == rhs.v_new) && (v_merge == rhs.v_merge) && (t_new == rhs.t_new) && (t_switch == rhs.t_switch);
    }
    // braucht VC7 um remove zu implementieren // wird nicht wirklich benoetigt
    bool operator<(const svt_pm_elem& rhs) const
    {
        return v.v < rhs.v.v; // wird nicht benoetigt und macht auch keinen sinn
    }
};

//extern template class DLLEXPORT std::list<svt_pm_elem>;
class DLLEXPORT svt_pm : public list<svt_pm_elem>
{
public:
    svt_pm();

    svt_pm(const svt_pm& rhs); // copy constructor (von list wird zuvor aufgerufen)

    void add(svt_pm_elem &elem) { push_front(elem); akt = begin(); }
    void delLast() { pop_front(); akt = begin(); }

    bool split(svt_mesh &mesh); //, int anz = 1
    bool contract(svt_mesh &mesh); //, int anz = 1

    void enableGeomorph() { m_geomorph = true; }
    void disableGeomorph() { m_geomorph = false; }

    bool save(svt_mesh &mesh, const char *saveFile);
    bool read(svt_mesh &mesh, const char *readFile);

    void clearAll()
    {
        clear();
        akt = begin();
    }

    // gibt das aktuelle element zurueck, das als naechstes eingefuegt werden soll
    // bzw. was als letztes herausgenommen wurde
    // oder das erste PM-Element wenn alle elemente breits eingefuegt wurden (darf nicht benutzt
    // werden, also immer pm auf end() testen)
    list<svt_pm_elem>::const_iterator getAkt() { return akt; }

private:

    list<svt_pm_elem>::iterator akt; // zeigt auf naechstes einzufuegendes element
    bool m_geomorph;
    int m_geomorphSteps;
};

inline ostream& operator<< (ostream& s, const svt_pm_ver& v) {
    s << v.v << ": " << v.pkt << ", " << v.norm << ", " << v.tex[0] << " " << v.tex[1] 
      << ", " << v.col.getR() << " " << v.col.getG() << " " << v.col.getB();
    return s;
}
inline istream& operator>> (istream& s, svt_pm_ver& v) {
    double dum;

    if (!(s >> v.v))
    {
        s.clear(ios::failbit);
        return s;
    }
    s.ignore(2);
    s >> v.pkt;
    s.ignore(2);
    s >> v.norm;
    s.ignore(2);
    s >> v.tex[0] >> v.tex[1];
    s.ignore(2);
    s >> dum; v.col.setR(dum);
    s >> dum; v.col.setG(dum);
    s >> dum; v.col.setB(dum);
    return s;
}

inline ostream& operator<< (ostream& s, const svt_pm_tri& t) {
    s << t.t << " (" << t.ver[0] << " " << t.ver[1] << " " << t.ver[2] << ")";
    return s;
}
inline istream& operator>> (istream& s, svt_pm_tri& t) {
    s >> t.t;
    s.ignore(2);
    s >> t.ver[0] >> t.ver[1] >> t.ver[2];
    s.ignore(1);
    return s;
}

inline ostream& operator<< (ostream& s, const svt_pm_elem& v) {
    list<svt_pm_tri>::const_iterator it;
    list<int>::const_iterator it2;

    s << v.v << endl ;
    s << v.v_new << endl;
    s << v.v_merge << endl;
    for (it = v.t_new.begin(); it != v.t_new.end(); ++it)
        s << (*it) << " ";
    s << endl;
    for (it2 = v.t_switch.begin(); it2 != v.t_switch.end(); ++it2)
        s << (*it2) << " ";
    s << endl;
    return s;
}

inline istream& operator>> (istream& s, svt_pm_elem& v) {
    string str;
    stringstream istr, istr2;
    svt_pm_tri t;
    int ti;

    if (!(s >> v.v))
    {
        s.clear(ios::failbit);
        return s;
    }
    s >> v.v_new;
    s >> v.v_merge; 
    getline(s, str);// ueberliesst newline
    getline(s, str);
    istr << str;
    while (istr >> t)
        v.t_new.push_back(t);
    getline(s, str);
    istr2 << str;
    while (istr2 >> ti)
        v.t_switch.push_back(ti);
    return s;
}

inline ostream& operator<< (ostream& s, const svt_pm& l) {
    svt_pm::const_iterator it;
    for (it = l.begin(); it != l.end(); ++it)
        s << (*it);
    return s;
}
inline istream& operator>> (istream& s, svt_pm& l) {
    svt_pm_elem v;
    while (s >> v) // wenn ein weiterer Datensatz vorliegt
    {
        l.push_back(v);
        v.t_new.clear();
        v.t_switch.clear();
    }
    return s;
}

#endif
