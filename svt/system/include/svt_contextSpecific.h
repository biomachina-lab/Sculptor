/***************************************************************************
                          svt_contextSpecific
			  ---------------
    begin                : 30.01.2002
    author               : Frank Delonge
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_CONTEXT_SPECIFIC
#define SVT_CONTEXT_SPECIFIC

#include <svt_stlVector.h>
#include <svt_exception.h>
#include <svt_window.h>

/**
 * Container classes that automatically create multiple
 * instances of an object: one instance for each OpenGL context 
 * (svt_vanvasSpecific) or window (svt_windowSpecific).
 * The idea is that every thread can work with the same scenegraph, and
 * only context-sensitve objects (e.g. display lists) get dublicated, 
 * which allows thread-safe usage of context sensitive data. 
 * Due to overloaded operators '->' and cast operator to hosted object
 * type, the container class behaves (almost) as if it is the hosted class
 * itself, calls alwas refer to the instance of the current context.
 * 
 * Here some example code to see how usage of this class should look like:
 * Deklaration:
 * \code
 * class svt_node { 
 *   svt_contextSpecific<svt_diplayList> m_oDisplayList;
 *   ...
 * };
 * \endcode
 *
 * Definition:
 * \code
 * svt_node::svt_node(...) : m_oDisplayList() {...}
 * \endcode
 * 
 * calls:
 * \code
 * // recompiles the display list of the current context
 * m_oDisplayList->recompile();
 *
 * // recompiles the display list of the context 0 (==window 1)
 * m_oDisplayList[0].recompile() ;
 *
 * // recompile all display lists:
 * for (int i=0; i<m_oDisplayList.size(); i++)
 *   m_oDisplayList[i].recompile(); 
 *
 * // or shorter:
 * forAllWindows(m_oDisplayList, recompile());
 *
 * // example usage of cast operator
 * // use svt_displayList's assign operator to assign to instance of current context:
 * m_oDisplayList = svt_DisplayList(...);
 * \endcode
 *
 * Note:
 *  - Make sure that the container object is constucted ofter the system 
 *    configuration (in which the amount of screens is specified) is evaluated,
 *    i.e. after svt_init() is called. Otherwise, the constructor of 
 *    svt_contextSensitive will throw an svt_exception.
 *  - Make sure that the hosted object has a valid copy-contructor, since
 *    it will be invoked when dublcating instances in container object 
 *    construction.
 *
 * \author Frank Delonge
 */
template <class T> class DLLEXPORT svt_windowSpecific
{

private:
    /**
     * Vector of all instances
     */
    vector<T> m_vec;

public:

    /**
     * Contructor of conatainer class.
     *  \throws svt_exception If svt_init() has not been invoked before
     */
    svt_windowSpecific() : m_vec(max(0,svt_getWindowsNum()))
    {
	if (svt_getWindowsNum() <= 0 )
	    //	  throw svt_exception("svt_windowSpecific(): Number of Windows not set yet, call svt_init() before constructing this object");
	    cout << "svt_windowSpecific(): Number of Windows not set yet, call svt_init() before constructing this object" << endl;
    }

    /**
     * Copy Contructor of conatainer class.
     */
    svt_windowSpecific(const svt_windowSpecific& that) : m_vec(that.m_vec)
    {
    }

    /**
     * Destructor
     */
    virtual ~svt_windowSpecific()
    {
    }


    /**
     *  Cast container class to reference to instance of current context
     */
    operator T& ()
    {
	svt_win_handle win=svt_getCurrentWindow();
	if (win>0 && (win <= int(size())))
	    return m_vec[win-1];
	throw svt_exception("Cannot determinate current window!");
    }

    /**
     *  Cast container class to reference to instance of current context (const version)
     */
    operator const T& () const
    {
	svt_win_handle win=svt_getCurrentWindow();
	if (win>0 && (win <= int(size())))
	    return m_vec[win-1];
	throw svt_exception("Cannot determinate current window!");
    }

    /**
     * overloaded '->'-operator, returns  reference to instance of current context
     */
    T* operator-> ()
    {
	svt_win_handle win=svt_getCurrentWindow();
	if (win>0 && (win <= int(size())))
	    return &(m_vec[win-1]);
	throw svt_exception("Cannot determinate current window!");
    }

    /**
     * overloaded '->'-operator, returns  reference to instance of current context (const version)
     */
    const T* operator-> () const
    {
	svt_win_handle win=svt_getCurrentWindow();
	if (win>0 && (win <= int(size())))
	    return &(m_vec[win-1]);
	throw svt_exception("Cannot determinate current window!");
    }

    /**
     * opertor[] to access instance of specified object.
     * Note that window/context counting begins with 0 in
     * contrast to svt_getCurrentWindow() which returns values from 1 to
     * svt_getWindowsNum().
     */
    T& operator[] (int i)
    {
	if (i>=0 && (i < int(size())))
	    return m_vec[i];
	throw svt_exception("Cannot determinate current window!");
    }

    /**
     * opertor[] to access instance of specified object (const version).
     * Note that window/context counting begins with 0 in
     * contrast to svt_getCurrentWindow() which returns values from 1 to
     * svt_getWindowsNum().
     */
    const T& operator[](int i) const
    {
	if (i>=0 && (i < int(size())))
	    return m_vec[i];
	throw svt_exception("Cannot determinate current window!");
    }

    /**
     * get current size
     * (amount of hosted instances)
     */
    unsigned size() const
    {
	return m_vec.size();
    }

};

/**
 * Macro-Funktion that ivokes the specified call \c call for all
 * hosted instances of container object \c obj
 */

#define forAllWindows(obj, call)     \
    do { unsigned i;                 \
    for (i=0;i<obj.size();i++)       \
    obj[i].call; }                   \
    while(0)

// sorry for this code crap.... but this way it is save to use this macro
// in simple if / else branches

template <class T> class DLLEXPORT svt_canvasSpecific
{

private:
    /**
     * Vector of all instances
     */
    vector<T> m_vec;

public:

    /**
     * Contructor of conatainer class.
     *  \throws svt_exception If svt_init() has not been invoked before
     */
    svt_canvasSpecific() : m_vec(max(svt_getCanvasNum(),0))
    {
	if (svt_getCanvasNum() <= 0 )
	    //	  throw svt_exception("svt_canvasSpecific(): Number of Windows not set yet, call svt_init() before constructing this object");
	    cout << "svt_canvasSpecific(): Number of Windows not set yet, call svt_init() before constructing this object" << endl;
    }

    /**
     * Copy Contructor of conatainer class.
     */
    svt_canvasSpecific(const svt_canvasSpecific& that) : m_vec(that.m_vec)
    {
    }

    /**
     * Destructor
     */
    virtual ~svt_canvasSpecific()
    {
    }

    /**
     *  Cast container class to reference to instance of current context
     */
    operator T& ()
    {
	int can = svt_getCurrentCanvas();
	if (can >= 0 && can < int(size()))
	    return m_vec[can];
	throw svt_exception("Cannot determinate current canvas!");
    }

    /**
     *  Cast container class to reference to instance of current context (const version)
     */
    operator const T& () const
    {
	int can = svt_getCurrentCanvas();
	if (can >= 0 && can < int(size()))
	    return m_vec[can];
	throw svt_exception("Cannot determinate current canvas!");
    }

    /**
     * overloaded '->'-operator, returns  reference to instance of current context
     */
    T* operator-> ()
    {
	int can = svt_getCurrentCanvas();
	if (can >= 0 && can < int(size()))
	    return &(m_vec[can]);
	throw svt_exception("Cannot determinate current canvas!");
    }

    /**
     * overloaded '->'-operator, returns  reference to instance of current context (const version)
     */
    const T* operator-> () const
    {
	int can = svt_getCurrentCanvas();
	if (can >= 0 && can < int(size()))
	    return &(m_vec[can]);
	throw svt_exception("Cannot determinate current canvas!");
    }

    /**
     * operator[] to access instance of specified object.
     * Note that window/context counting begins with 0 in
     * contrast to svt_getCurrentWindow() which returns values from 1 to
     * svt_getWindowsNum().
     */
    T& operator[] (int i)
    {
	if ( i >= 0 && i < int(size()))
	    return m_vec[i];
	throw svt_exception("Cannot determinate current canvas!");
    }

    /**
     * opertor[] to access instance of specified object (const version).
     * Note that window/context counting begins with 0 in
     * contrast to svt_getCurrentWindow() which returns values from 1 to
     * svt_getWindowsNum().
     */
    const T& operator[](int i) const
    {
	if ( i >= 0 && i < int(size()))
	    return m_vec[i];
	throw svt_exception("Cannot determinate current canvas!");
    }

    /**
     * get current size
     * (amount of hosted instances)
     */
    unsigned size() const
    {
	return m_vec.size();
    }
};

/**
 * Macro-Funktion that ivokes the specified call \c call for all
 * hosted instances of container object \c obj
 */
#define forAllCanvas(obj, call)     \
 do { unsigned i;                   \
      for (i=0;i<obj.size();i++)    \
	    obj[i].call; }          \
 while(0)

// sorry for this code crap.... but this way it is save to use this macro
// in simple if / else branches

#endif
