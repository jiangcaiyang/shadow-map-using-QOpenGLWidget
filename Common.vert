// MyCube.vert

// 属性变量
attribute vec3 position;
attribute vec3 normal;
attribute vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 modelViewNormalMatrix;
uniform mat4 lightViewMatrix;
uniform mat4 lightProjectionMatrix;

// 转换到varying中的
varying vec3 viewSpacePosition;
varying vec2 v_texCoord;
varying vec3 v_normal;
varying vec4 v_shadowCoord;

void main( void )
{
    viewSpacePosition = vec3( viewMatrix * modelMatrix * vec4( position, 1.0 ) );

    v_texCoord = texCoord;

    v_normal = modelViewNormalMatrix * normal;

    v_shadowCoord = lightProjectionMatrix *
            lightViewMatrix *
            modelMatrix *
            vec4( position, 1.0 );

    gl_Position = projectionMatrix *
            viewMatrix *
            modelMatrix *
            vec4( position, 1.0 );
}
