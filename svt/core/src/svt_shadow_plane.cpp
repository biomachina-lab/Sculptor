/***************************************************************************
                          svt_shadow_plane.cpp
                          -------------------
    begin                : 6/7/2003
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_shadow_plane.h>
#include <svt_lightsource.h>

svt_shadow_plane( svt_node* pNode, svt_lightsource* pLight ) : svt_node(),
    m_pShadowNode( pNode ),
    m_pLight( pLight),
    m_fWidth( 40.0f ),
    m_fLength( 40.0f )
{

}

void svt_shadow_plane::calcShadowMat()
{
    svt_matrix4f* pLightMat = m_pLight->getTransformation();
    svt_matrix4f* pGroundMat = getTransformation();
    svt_matrix4f pShadowMat();

    /* Find dot product between light position vector and ground plane normal. */
    Real32 dot = groundplane[X] * lightpos[X] +
        groundplane[Y] * lightpos[Y] +
        groundplane[Z] * lightpos[Z] +
        groundplane[W] * lightpos[W];

    shadowMat.setAt(0, 0, dot - lightpos[X] * groundplane[X]);
    shadowMat.setAt(1, 0, 0.f - lightpos[X] * groundplane[Y]);
    shadowMat.setAt(2, 0, 0.f - lightpos[X] * groundplane[Z]);
    shadowMat.setAt(3, 0, 0.f - lightpos[X] * groundplane[W]);

    shadowMat.setAt(X, 1, 0.f - lightpos[Y] * groundplane[X]);
    shadowMat.setAt(1, 1, dot - lightpos[Y] * groundplane[Y]);
    shadowMat.setAt(2, 1, 0.f - lightpos[Y] * groundplane[Z]);
    shadowMat.setAt(3, 1, 0.f - lightpos[Y] * groundplane[W]);

    shadowMat.setAt(X, 2, 0.f - lightpos[Z] * groundplane[X]);
    shadowMat.setAt(1, 2, 0.f - lightpos[Z] * groundplane[Y]);
    shadowMat.setAt(2, 2, dot - lightpos[Z] * groundplane[Z]);
    shadowMat.setAt(3, 2, 0.f - lightpos[Z] * groundplane[W]);

    shadowMat.setAt(X, 3, 0.f - lightpos[W] * groundplane[X]);
    shadowMat.setAt(1, 3, 0.f - lightpos[W] * groundplane[Y]);
    shadowMat.setAt(2, 3, 0.f - lightpos[W] * groundplane[Z]);
    shadowMat.setAt(3, 3, dot - lightpos[W] * groundplane[W]);

    pShadowMat.applyGL();
}

void svt_shadow_plane::drawGL()
{
    /* clear stencil buffer */
    glClear(GL_STENCIL_BUFFER_BIT);

    /* Don't update color or depth. */
    glDisable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    /* Draw 1 into the stencil buffer. */
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

    /* Now render the object the shadow is casted to; pixels just get their stencil set to 1. */
    glBegin(GL_QUADS);
      //glTexCoord2f(0.0f, 0.0f);
      glVertex3f(-m_fWidth/2, 0.0f, m_fLength/2);
      //glTexCoord2f(0.0f, 1.0f);
      glVertex3f(m_fWidth/2, 0.0f, m_fLength/2);
      //glTexCoord2f(1.0f, 1.0f);
      glVertex3f(m_fWidth/2, 0.0f, -m_fLength/2);
      //glTexCoord2f(1.0f, 0.0f);
      glVertex3f(-m_fWidth/2, 0.0f, -m_fLength/2);
    glEnd();

    /* Re-enable update of color and depth. */
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    /* Now, only render where stencil is set to 1. */
    glStencilFunc(GL_EQUAL, 1, 0xffffffff);  /* draw if ==1 */
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    /* Render 50% black shadow color on top of whatever the
     floor appareance is. */
    glDisable(GL_LIGHTING); /* Force the 50% black. */
    glColor4f(0.0, 0.0, 0.0, 0.5);

    /* Project the shadow. */
    glPushMatrix();
    /* Calculate and apply shadow matrix */
    calcShadowMat();
    m_pShadowNode->draw(SVT_MODE_GL);
    glPopMatrix();

    /* reset everything */
    glEnable(GL_LIGHTING);
    glDisable(GL_STENCIL_TEST);
}
