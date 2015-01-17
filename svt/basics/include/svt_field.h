/***************************************************************************
                          svt_field.h  -  description
                             -------------------
    begin                : 10.04.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_FIELD_H
#define SVT_FIELD_H

#include <svt_basics.h>
#include <svt_array.h>

#define SVT_FIELD_IMMEDIATE 1
#define SVT_FIELD_FLUSH     2

/** A resizeable array class.
  *@author Stefan Birmanns
  */
template<class dt>
    class DLLEXPORT svt_field {

    protected:
        svt_array<svt_field*> sendTo;
        dt data;
        int mode;

    public:
        svt_field(int m=SVT_FIELD_IMMEDIATE)
        {
            mode = m;
        };
        virtual ~svt_field() { };

    public:
        /// connect this field to another field (this field will post the data to the other field)
        void connectTo(svt_field<dt> *rec)
        {
            sendTo+=rec;
        };

        /// disconnect from another field. The data will not longer be send to this field.
        void disconnectFrom(svt_field<dt> *rec)
        {
            int i = sendTo.find(rec);

            if (i != -1)
                sendTo.delElement(i);
        };

        /// set the data. If mode == SVT_FIELD_IMMEDIATE (default), the data will be propagated through the connected nodes.
        void set(dt el)
        {
            data = el;

            onChange();

            if (mode == SVT_FIELD_FLUSH)
                return;

            int i;
            for(i=0;i<sendTo.numElements();i++)
                sendTo[i]->set(el);
        };

        /// get the data
        dt get() { return data; };

        /// flush the data through the pipeline (only in SVT_FIELD_FLUSH mode)
        void flush()
        {
            int i;
            if (mode != SVT_FIELD_FLUSH)
                return;

            for(i=0;i<sendTo.numElements();i++)
                sendTo[i]->set(data);
        };

        /// set the mode (SVT_FIELD_IMMEDIATE or SVT_FIELD_FLUSH). See set() for details.
        void setMode(int m) { mode = m; };

        /// get the mode (SVT_FIELD_IMMEDIATE or SVT_FIELD_FLUSH). See set() for details.
        void getMode() { return mode; };

        /// this function is called whenever the object changes. It is a virtual function, so you should overload it and write your own code there.
        virtual void onChange()
        {
        };

        /// print the current field status
        void print()
        {
            cout << "svt_field: connected to " << sendTo.numElements() << " other fields" << endl;
            cout << "svt_field: data " << get() << endl;
        };
    };

#endif
