#include "GL.hpp"

namespace wwidget {

void GL::init() {
	#define GET(X)

	GET(glGetStringi);
	GET(glBindBuffer);
	GET(glGenBuffers);
	GET(glDeleteBuffers);
	GET(glBufferData);
	GET(glBufferSubData);
	GET(glGetBufferSubData);
	GET(glNamedBufferData);
	GET(glNamedBufferSubData);
	GET(glGetNamedBufferSubData);
	GET(glBindVertexArray);
	GET(glGenVertexArrays);
	GET(glDeleteVertexArrays);
	GET(glEnableVertexAttribArray);
	GET(glDisableVertexAttribArray);
	GET(glVertexAttribPointer);
	GET(glVertexAttribIPointer);
	GET(glVertexAttribLPointer);
	GET(glVertexAttribDivisor);
	GET(glPrimitiveRestartIndex);
	GET(glTexStorage2D);
	GET(glGenerateMipmap);
	GET(glGenFramebuffers);
	GET(glDeleteFramebuffers);
	GET(glBindFramebuffer);
	GET(glFramebufferTexture2D);
	GET(glGenRenderbuffers);
	GET(glDeleteRenderbuffers);
	GET(glBindRenderbuffer);
	GET(glRenderbufferStorage);
	GET(glFramebufferRenderbuffer);
	GET(glUseProgram);
	GET(glCreateShader);
	GET(glDeleteShader);
	GET(glCreateProgram);
	GET(glDeleteProgram);
	GET(glAttachShader);
	GET(glDetachShader);
	GET(glGetShaderiv);
	GET(glGetProgramiv);
	GET(glShaderSource);
	GET(glCompileShader);
	GET(glGetShaderInfoLog);
	GET(glGetProgramInfoLog);
	GET(glLinkProgram);
	GET(glShaderBinary);
	GET(glGetProgramBinary);
	GET(glGetUniformLocation);
	GET(glUniform1f);
	GET(glUniform2f);
	GET(glUniform2fv);
	GET(glUniform3f);
	GET(glUniform3fv);
	GET(glUniform4f);
	GET(glUniform4fv);
	GET(glUniform1i);
	GET(glUniformMatrix3fv);
	GET(glUniformMatrix4fv);
	GET(glDrawArraysInstanced);
	GET(glDrawElementsInstanced);
	GET(glBindFrameBuffer);
	GET(glGetFramebufferParameteriv);
	GET(glBeginQuery);
	GET(glEndQuery);
	GET(glCreateQueries);
	GET(glGenQueries);
	GET(glDeleteQueries);
	GET(glDebugMessageCallback);
}

} // namespace wwidget
