// Depth.vert
#ifdef GL_ES
precision highp float;
#endif

attribute vec3 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

varying vec4 projectedPosition;

void main( void )
{
    vec3 finalPosition = position;

    projectedPosition =
            projectionMatrix *
            viewMatrix *
            modelMatrix *
            vec4( finalPosition, 1.0 );
    gl_Position = projectedPosition;
}
