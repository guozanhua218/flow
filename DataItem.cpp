/*******************************************************************************
 DataItem: OpenGL context data.
 Copyright (c) 2006-2008 Jordan Van Aalsburg

 This file is part of the Dynamics Toolset.

 The Dynamics Toolset is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option) any
 later version.

 The Dynamics Toolset is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License
 along with the Dynamics Toolset. If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/
#include "DataItem.h"

// Vrui includes
//
#include <Misc/ThrowStdErr.h>
#include <GL/Extensions/GLARBVertexShader.h>
#include <GL/Extensions/GLARBFragmentShader.h>

// External includes
//
#include "VruiStreamManip.h"
#include "IO/ansi-color.h"

#include <string.h>

namespace DTS
{

//
// Shader utility methods
//

namespace
{

/*********************************************************
 Support functions to compile/link vertex/fragment shaders:
 *********************************************************/

void glCompileShaderFromString(GLhandleARB shaderObject, const char* shaderSource)
{
   /* Determine the length of the source string: */
   GLint shaderSourceLength=GLint(strlen(shaderSource));

   /* Upload the shader source into the shader object: */
   const GLcharARB* ss=reinterpret_cast<const GLcharARB*> (shaderSource);
   glShaderSourceARB(shaderObject, 1, &ss, &shaderSourceLength);

   /* Compile the shader source: */
   glCompileShaderARB(shaderObject);

   /* Check if the shader compiled successfully: */
   GLint compileStatus;
   glGetObjectParameterivARB(shaderObject, GL_OBJECT_COMPILE_STATUS_ARB, &compileStatus);
   if (!compileStatus)
   {
      /* Get some more detailed information: */
      GLcharARB compileLogBuffer[2048];
      GLsizei compileLogSize;
      glGetInfoLogARB(shaderObject, sizeof(compileLogBuffer), &compileLogSize, compileLogBuffer);

      /* Signal an error: */
      Misc::throwStdErr("glCompileShaderFromString: Error \"%s\" while compiling shader", compileLogBuffer);
   }
}

GLhandleARB glCompileVertexShaderFromString(const char* shaderSource)
{
   /* Create a new vertex shader: */
   GLhandleARB vertexShaderObject=glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);

   try
   {
      /* Load and compile the shader source: */
      glCompileShaderFromString(vertexShaderObject, shaderSource);
   } catch (std::runtime_error err)
   {
      /* Clean up and re-throw the exception: */
      glDeleteObjectARB(vertexShaderObject);

      throw ;
   }

   return vertexShaderObject;
}

GLhandleARB glCompileFragmentShaderFromString(const char* shaderSource)
{
   /* Create a new vertex shader: */
   GLhandleARB fragmentShaderObject=glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

   try
   {
      /* Load and compile the shader source: */
      glCompileShaderFromString(fragmentShaderObject,shaderSource);
   }
   catch(std::runtime_error err)
   {
      /* Clean up and re-throw the exception: */
      glDeleteObjectARB(fragmentShaderObject);

      throw;
   }

   return fragmentShaderObject;
}

GLhandleARB glLinkShader(GLhandleARB vertexShaderObject,GLhandleARB fragmentShaderObject)
{
   /* Create the program object: */
   GLhandleARB programObject=glCreateProgramObjectARB();

   /* Attach all shader objects to the shader program: */
   glAttachObjectARB(programObject,vertexShaderObject);
   glAttachObjectARB(programObject,fragmentShaderObject);

   /* Link the program: */
   glLinkProgramARB(programObject);

   /* Check if the program linked successfully: */
   GLint linkStatus;
   glGetObjectParameterivARB(programObject,GL_OBJECT_LINK_STATUS_ARB,&linkStatus);
   if(!linkStatus)
   {
      /* Get some more detailed information: */
      GLcharARB linkLogBuffer[2048];
      GLsizei linkLogSize;
      glGetInfoLogARB(programObject,sizeof(linkLogBuffer),&linkLogSize,linkLogBuffer);

      /* Signal an error: */
      Misc::throwStdErr("glLinkShader: Error \"%s\" while linking shader program",linkLogBuffer);
   }

   return programObject;
}

} // namespace::

//
// DataItem methods
//

DataItem::DataItem(void)
   : hasPointParameterExtension(GLARBPointParameters::isSupported()),
   hasVertexBufferObjectExtension(GLARBVertexBufferObject::isSupported()),
   hasShaders(GLARBShaderObjects::isSupported()&&GLARBVertexShader::isSupported()&&GLARBFragmentShader::isSupported()),
   vertexBufferId(0), spriteTextureObjectId(0),
   version(0),
   vertexShaderObject(0),fragmentShaderObject(0),programObject(0),
   numParticles(0)
{
   master::filter masterout(std::cout);

   masterout() << "Initializing OpenGL extensions..." << std::endl;

   masterout() << "\tGL_ARB_POINT_PARAMETERS : ";
   if(hasPointParameterExtension)
   {
      GLARBPointParameters::initExtension();
      masterout() << ansi::green(ansi::BOLD) << "OK" << ansi::endl;
   }
   else
   {
      masterout() << ansi::red(ansi::BOLD) << "NOT SUPPORTED" << ansi::endl;
   }

   masterout() << "\tGL_ARB_VERTEX_BUFFER_OBJECT : ";
   if (hasVertexBufferObjectExtension)
   {
      // initialize the vertex buffer object extension
      GLARBVertexBufferObject::initExtension();

      // create a vertex buffer object
      glGenBuffersARB(1,&vertexBufferId);

      masterout() << ansi::green(ansi::BOLD) << "OK" << ansi::endl;
   }
   else
   {
      masterout() << ansi::red(ansi::BOLD) << "NOT SUPPORTED" << ansi::endl;
   }

   glGenTextures(1, &spriteTextureObjectId);

   masterout() << "\tGL_ARB_SHADER_OBJECTS : ";
   if(hasShaders)
   {
      /* Initialize the OpenGL extensions: */
      GLARBShaderObjects::initExtension();
      GLARBVertexShader::initExtension();
      GLARBFragmentShader::initExtension();

      /* Source code for vertex and fragment programs: */
      static const char* vertexProgram="\
         uniform float scaledParticleRadius; \
         \
         void main() \
         { \
         vec4 vertexEye; \
         \
         /* Transform the vertex to eye coordinates: */ \
         vertexEye=gl_ModelViewMatrix*gl_Vertex; \
         \
         /* Calculate point size based on vertex' eye distance along z direction: */ \
         gl_PointSize=scaledParticleRadius*2.0*vertexEye.w/vertexEye.z; \
         \
         /* Use standard vertex position for fragment generation: */ \
         gl_FrontColor=gl_Color; \
         gl_Position=ftransform(); \
         }";
      static const char* fragmentProgram="\
         uniform sampler2D tex0; \
         \
         void main() \
         { \
         gl_FragColor=texture2D(tex0,gl_TexCoord[0].xy)*gl_Color; \
         }";

      /* Compile and link the point size computation shader program: */
      vertexShaderObject=glCompileVertexShaderFromString(vertexProgram);
      fragmentShaderObject=glCompileFragmentShaderFromString(fragmentProgram);
      programObject=glLinkShader(vertexShaderObject,fragmentShaderObject);
      scaledParticleRadiusLocation=glGetUniformLocationARB(programObject,"scaledParticleRadius");
      tex0Location=glGetUniformLocationARB(programObject,"tex0");
      masterout() << ansi::green(ansi::BOLD) << "OK" << ansi::endl;
   }
   else
   {
      masterout() << ansi::red(ansi::BOLD) << "NOT SUPPORTED" << ansi::endl;
   }

   /* Display list for StaticSolverTool */
   dataDisplayListVersion = 0;
   dataDisplayListId=glGenLists(1);
}

DataItem::~DataItem(void)
{
   if(vertexBufferId>0)
   {
      // delete the vertex buffer object
      glDeleteBuffersARB(1,&vertexBufferId);
   }

   // delete texture object(s)
   glDeleteTextures(1, &spriteTextureObjectId);

   if(hasShaders)
   {
      glDeleteObjectARB(programObject);
      glDeleteObjectARB(vertexShaderObject);
      glDeleteObjectARB(fragmentShaderObject);
   }

   /* Display list for StaticSolverTool */
   glDeleteLists(dataDisplayListId, 1);

}

} // namspace::DTS
