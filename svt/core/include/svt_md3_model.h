/***************************************************************************
                          svt_md3_model.h  -  description
                             -------------------
    begin                : Nov 07 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_MD3_MODEL_H
#define SVT_MD3_MODEL_H

#include <svt_node.h>
#include <svt_md3.h>

/** A md3 file loader. Create a svt object of a md3 file (3d object file format).
  *@author Stefan Birmanns
  */
class DLLEXPORT svt_md3_model : public svt_node
{
protected:
    svt_md3* head;
    svt_md3* upper;
    svt_md3* lower;

public:
    svt_md3_model(svt_pos* _tpos,
                  char* head_file, char* head_texture,
                  char* upper_file, char* upper_texture,
                  char* lower_file, char* lower_texture)
        : svt_node(NULL,NULL,_tpos)
    {
        if (head_file != NULL)
            head = new svt_md3(NULL,head_file, head_texture);
        else
            head = NULL;

        if (upper_file != NULL)
            upper = new svt_md3(NULL,upper_file, upper_texture);
        else
            upper = NULL;

        if (lower_file != NULL)
            lower = new svt_md3(NULL,lower_file, lower_texture);
        else
            lower = NULL;

    };
    virtual ~svt_md3_model(){};

    virtual void drawGL()
    {
        if (lower != NULL)
        {
            lower->drawGL();
            lower->applyTag("tag_torso");
        }

        if (upper != NULL)
        {
            upper->drawGL();
            upper->applyTag("tag_head");
        }

        if (head != NULL)
            head->drawGL();
    };
};
#endif
