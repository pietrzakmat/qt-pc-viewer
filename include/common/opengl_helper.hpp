#ifndef OPENGL_HELPER_HPP
#define OPENGL_HELPER_HPP

#include <iostream>

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>
#include <QOpenGLFramebufferObject>
#include <QImage>

namespace graphics {

//class Texture {
//public:
//    unsigned int id;
//    std::string type;
//    std::string path;
//};

struct VertexData
{
    std::vector<QVector3D> positions;
    std::vector<QVector4D> colors;
    std::vector<QVector3D> normals;
    std::vector<QVector2D> tex_coords;
};


namespace texture_helper {

inline void
createTextureOneChannelLuminance(QOpenGLFramebufferObject *fbo, GLenum inputFormat, GLint minMagFilterType)
{
    glBindTexture(GL_TEXTURE_2D, fbo->texture());
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(fbo->format().internalTextureFormat()), fbo->width(), fbo->height(), 0, GL_RED, inputFormat, nullptr);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,minMagFilterType);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minMagFilterType);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

inline void
createTextureOneChannelLuminance(QOpenGLFramebufferObject *fbo, GLint internalFormat, GLenum inputFormat, GLint minMagFilterType)
{
    glBindTexture(GL_TEXTURE_2D, fbo->texture());
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, fbo->width(), fbo->height(), 0, GL_RED, inputFormat,nullptr);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,minMagFilterType);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minMagFilterType);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

inline void
createTextureOneChannelLuminance(GLuint texId, int width, int height, GLint internalFormat, GLuint inputFormat, GLint minMagFilterType)
{
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RED, inputFormat, nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,minMagFilterType);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minMagFilterType);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

inline void
createTextureOneChannelLuminance(GLuint texId, GLuint inputFormat, GLint minMagFilterType, int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_RED, inputFormat, nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,minMagFilterType);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minMagFilterType);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

inline void
create_texture_rgb_fbo(QOpenGLFramebufferObject *fbo, GLenum inputFormat, GLint minMagFilterType)
{
    glBindTexture(GL_TEXTURE_2D, fbo->texture());
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(fbo->format().internalTextureFormat()), fbo->width(), fbo->height(), 0, GL_RGB, inputFormat, nullptr);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,minMagFilterType);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minMagFilterType);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

inline void
create_texture_rgb_fbo(QOpenGLFramebufferObject *fbo, GLint internalFormat, GLuint inputFormat, GLint minMagFilterType)
{
    glBindTexture(GL_TEXTURE_2D, fbo->texture());
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, fbo->width(), fbo->height(), 0, GL_RGB, inputFormat, nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,minMagFilterType);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,minMagFilterType);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

inline void
create_texture_rgb(GLuint texId, GLenum inputFormat, GLint minMagFilterType, int width, int height, const std::uint8_t *data=nullptr)
{
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLint>(width), static_cast<GLint>(height), 0, GL_RGB, inputFormat, data);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, minMagFilterType);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, minMagFilterType);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

inline GLuint
create_texture_rgb(const GLint width, const GLint height, const std::uint8_t* data)
{
    GLuint texId;
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    f->glGenTextures(1, &texId);
    f->glBindTexture(GL_TEXTURE_2D, texId);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    f->glGenerateMipmap(GL_TEXTURE_2D);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

//    f->glGenerateMipmap(GL_TEXTURE_2D);
//    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texId;
}

inline QImage
load_to_qimage_rgb(const char* file_path)
{
    QImage img(file_path);
    img = img.convertToFormat(QImage::Format_RGB888);

    if (img.isNull() || img.width() == 0 || img.height() == 0){
        std::cerr << "Error load_texture: " << file_path << "\n";
        return img;
    }

    while (img.width()*3 < img.bytesPerLine()) { // Fix by scaling to be able to work with data
        img = img.scaledToWidth(img.width()+1);
    }

    return img;
}

inline std::tuple<int, int>
load_image_rgb_to_texture_id(const GLuint& tex_id, const char* file_path)
{
    QImage img = load_to_qimage_rgb(file_path);

    std::cerr << "load_texture: " << file_path << " to tex_id: " << tex_id << "\n";
    create_texture_rgb(tex_id, GL_UNSIGNED_BYTE, GL_LINEAR, img.width(), img.height(), img.mirrored().bits());
    return {img.width(), img.height()};
}


inline void
load_image_to_fbo(QOpenGLFramebufferObject* fbo, const std::uint8_t* data)
{

//    std::shared_ptr<QOpenGLFramebufferObject> fbo = std::make_shared<QOpenGLFramebufferObject>(img.width(), img.height());

//    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
//    f->glBindTexture(GL_TEXTURE_2D, fbo->texture());
//    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbo->width(), fbo->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, img.bits());

//    f->glGenerateMipmap(GL_TEXTURE_2D);
//    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D, fbo->texture());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbo->width(), fbo->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//    std::cerr << "load_texture_to_fbo: " << file_path << "\n";
//    std::cerr << "fbo->texture(): " << fbo->texture() << "\n";

//    img.save("test_img.png");
//    fbo->toImage().save("test_fbo_img.png");

}


}

}

#endif // OPENGL_HELPER_HPP
