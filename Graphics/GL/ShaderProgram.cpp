/* This file is part of Tank.
 *
 * Tank is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tank is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License and
 * the GNU Lesser General Public Licence along with Tank. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Copyright 2013 (©) Jamie Bayne, David Truby, David Watson.
 */

#include "ShaderProgram.hpp"

#include <stdexcept>
#include <fstream>

namespace tank {
namespace gl {

void ShaderProgram::bind(ShaderProgram *shader)
{
    if(shader->valid_) glUseProgram(shader->program_);
}

void ShaderProgram::unbind()
{
    glUseProgram(0);
}

ShaderProgram::ShaderProgram() : program_(0), valid_(true) { }

ShaderProgram::ShaderProgram(std::string file, Type type)
    :program_(0), valid_(false)
{
    loadFromFile(file,type);
}

ShaderProgram::ShaderProgram(std::string vertexShader,
               std::string fragmentShader)
    :program_(0), valid_(false)
{
    loadFromFile(vertexShader,fragmentShader);
}

ShaderProgram::~ShaderProgram()
{
    if(glIsProgram(program_))
    {
        glDeleteProgram(program_);
    }
}

/////////////////////////////////////////////
// Uniform Accessors
/////////////////////////////////////////////

void ShaderProgram::setUniform(std::string name, GLint value) const
{
    GLuint addr = glGetUniformLocation(program_, name.c_str());

    glProgramUniform1i(program_, addr,value);
}

void ShaderProgram::setUniform(std::string name, GLuint value) const
{
    GLuint addr = glGetUniformLocation(program_, name.c_str());

    glProgramUniform1ui(program_, addr,value);
}

void ShaderProgram::setUniform(std::string name, GLfloat value)  const
{
    GLuint addr = glGetUniformLocation(program_, name.c_str());

    glProgramUniform1f(program_, addr,value);
}

void ShaderProgram::setUniform(std::string name, glm::vec2 const& value) const
{
    GLuint addr = glGetUniformLocation(program_, name.c_str());

    glProgramUniform2fv(program_, addr, 1, &value[0]);
}

void ShaderProgram::setUniform(std::string name, glm::vec3 const& value) const
{
    GLuint addr = glGetUniformLocation(program_, name.c_str());

    glProgramUniform3fv(program_, addr, 1, &value[0]);
}

void ShaderProgram::setUniform(std::string name, glm::vec4 const& value) const
{
    GLuint addr = glGetUniformLocation(program_, name.c_str());

    glProgramUniform4fv(program_, addr, 1, &value[0]);
}

void ShaderProgram::setUniform(std::string name, glm::mat4 const& value) const
{
    GLuint addr = glGetUniformLocation(program_, name.c_str());

    glProgramUniformMatrix4fv(program_, addr, 1, false, &value[0][0]);
}

/////////////////////////////////////////////
// Loading Functions
/////////////////////////////////////////////

void ShaderProgram::loadFromFile(std::string file, Type type)
{
    std::string source = loadSourceFromFile(file);
    try
    {
        loadFromSource(source.c_str(), type);
    }
    catch(std::exception& e)
    {
        std::string error = e.what();
        error.append("\n " + file);

        throw std::runtime_error(error);
    }
}

void ShaderProgram::loadFromFile(std::string vertexFile,
                                   std::string fragmentFile)
{
    createProgram();

    std::string vertexShader = loadSourceFromFile(vertexFile);
    std::string fragmentShader = loadSourceFromFile(fragmentFile);

    try
    {
        loadFromSource(vertexShader.c_str(), fragmentShader.c_str());
    }
    catch(std::exception& e)
    {
        std::string error = e.what();
        error.append("\nFiles: " + vertexFile + ", " + fragmentFile);

        throw std::runtime_error(error);
    }
}

void ShaderProgram::loadFromSource(char const* source, Type type)
{
    createProgram();

    GLuint shaderObject = compileShaderFromSource(source, type);

    linkProgram({shaderObject});

    //If we made it here, must be valid
    valid_ = true;
}

void ShaderProgram::loadFromSource(char const* vertexShader,
                            char const* fragmentShader)
{
    createProgram();

    GLuint vertexObject;
    GLuint fragmentObject;

    vertexObject   = compileShaderFromSource(vertexShader,   Vertex);
    fragmentObject = compileShaderFromSource(fragmentShader, Fragment);

    linkProgram({vertexObject, fragmentObject});

    //If we made it here, must be valid
    valid_ = true;
}


std::string ShaderProgram::loadSourceFromFile(std::string file)
{
    std::fstream f(file, std::ios::in);

    if(f.fail())
    {
        std::string error = "Could not open file \"";
        error.append(file);
        error.append("\"");
        throw std::runtime_error(error);
    }

    std::string content;
    while(!f.eof())
    {
        std::string line;
        std::getline(f, line);
        content.append(line + '\n');
    }
    f.close();

    return content;
}

void ShaderProgram::createProgram()
{
    if(glIsProgram(program_))
    {
        glDeleteProgram(program_);
    }

    program_ = glCreateProgram();

    if(program_ == 0)
    {
        throw std::runtime_error("Could not create shader");
    }
}

GLuint ShaderProgram::compileShaderFromSource(char const* source, Type type)
{
    GLuint shaderObject = 0;

    if(type == Vertex)
    {
        shaderObject = glCreateShader(GL_VERTEX_SHADER);
    }
    else if(type == Fragment)
    {
        shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    }

    if(shaderObject == 0)
    {
        throw std::runtime_error("Could not create shader.");
    }

    glShaderSource(shaderObject, 1, &source, NULL);

    glCompileShader(shaderObject);

    GLint status;
    glGetShaderiv(shaderObject,GL_COMPILE_STATUS, &status);

    if(status != GL_TRUE)
    {
        char logText[1000];
        glGetShaderInfoLog(shaderObject, 1000, NULL, logText);

        std::ofstream logfile("compile_errors.txt",std::ios::out);

        if(!logfile.is_open())
        {
            throw std::runtime_error("Shader failed to compile. Could not open logfile");
        }

        logfile << logText << "\n";
        logfile << "Code:\n\n";
        logfile << source;
        logfile.close();

        std::string error = "Shader failed to compile (see compile_errors.txt)";
        glDeleteShader(shaderObject);

        throw std::runtime_error(error);
    }

    return shaderObject;
}

void ShaderProgram::linkProgram(std::vector<GLuint> const& shaderObjects)
{
    for(auto so : shaderObjects)
    {
        glAttachShader(program_, so);
    }

    glLinkProgram(program_);

    GLint status;
    glGetProgramiv(program_, GL_LINK_STATUS, &status);

    if(status != GL_TRUE)
    {
        char logText[1000];
        glGetProgramInfoLog(program_, 1000, NULL, logText);

        std::fstream logfile("link_errors.txt", std::ios::out);

        logfile << logText;
        logfile.close();
        std::string error = "Failed to link shader program. See link_errors.txt";

        for(auto so : shaderObjects)
        {
            glDeleteShader(so);
        }

        throw std::runtime_error(error);
    }

    for(auto so : shaderObjects)
    {
        glDeleteShader(so);
    }
}

}
}