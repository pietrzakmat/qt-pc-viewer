#ifndef GLPOINTCLOUDOBJECT_H
#define GLPOINTCLOUDOBJECT_H

#include <memory>
#include <mutex>
#include <iostream>

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QColor>
#include <QVector3D>

#include <openglwindow.h>
#include <tinycolormap.hpp>

class GLPointCloudObject
{
public:
    float m_point_size {1.F};
    bool m_x_inversion {false};
    bool m_y_inversion {false};
    bool m_z_inversion {false};
    bool m_inverse_depth_colors {false};
    bool m_use_original_colors {true};

    enum pc_encoding {
        DEPTH_grayscale,
        LUT_Turbo,
        LUT_Jet,
        LUT_Heat
    };
    pc_encoding m_pc_encoding {LUT_Turbo};

    void initialize_gl();
    void draw(const float point_size);

    void set_shader(QOpenGLShaderProgram* shader) {m_shader = shader;}
    QOpenGLShaderProgram* get_shader_program() {return m_shader;}

    void set_points(const graphics::VertexData &vertex_data);

    float m_thresh = 0.1F;

    QVector3D m_scale {1,1,1};
    QVector3D m_offset {0,0,0};
    QVector3D m_rotate {0,0,0};

    QMatrix4x4 get_model_mat();

private:
    bool m_initialized {false};
    std::size_t m_vertices_count {0};

    std::unique_ptr<QOpenGLVertexArrayObject> m_vao {nullptr};
    QOpenGLShaderProgram* m_shader {nullptr};

    std::tuple<float, float> find_min_max(const std::vector<QVector3D> &points, const float& thresh) const;
    float get_map_factor(const std::vector<QVector3D> &points, const float &thresh) const;
    std::vector<QVector4D> compute_colors_from_depth(const std::vector<QVector3D>& points, const pc_encoding &encoding) const;
};

inline std::tuple<float, float>
GLPointCloudObject::find_min_max(const std::vector<QVector3D> &points, const float &thresh) const
{
    float min = 100.F;
    float max = -100.F;

    for (const auto &p : points) {
        if (p.z() < min)
            min = p.z();
        else if (p.z() > max)
            max = p.z();
    }

    if (min < 0) min*=-1;
    if (max < 0) max*=-1;

    //    max*=thresh;

    return {min*(1.0F + thresh), max*(1.0F - thresh)};
}

inline float
GLPointCloudObject::get_map_factor(const std::vector<QVector3D> &points, const float& thresh) const
{
    auto [min, max] = find_min_max(points, thresh);
    float div = qFuzzyIsNull(max + min) ? 0.001F : (max + min);
    //    float div = (max + min) == 0 ? 0.001f : (max + min);
    return 1.F / div;
}

inline std::vector<QVector4D>
GLPointCloudObject::compute_colors_from_depth(const std::vector<QVector3D>& points, const pc_encoding& encoding) const
{
    const float factor = get_map_factor(points, m_thresh);
    std::vector<QVector4D> colors;
    colors.reserve(points.size());

    tinycolormap::ColormapType color_type = tinycolormap::ColormapType::Turbo;
    if (pc_encoding::LUT_Heat == encoding)
        color_type = tinycolormap::ColormapType::Heat;
    else if (pc_encoding::LUT_Jet == encoding)
        color_type = tinycolormap::ColormapType::Jet;

    for (const auto &p : points) {
        const float depth = p.z() < 0 ? p.z()*(-1) : p.z();
        const float intesity = m_inverse_depth_colors ? std::max(0.F, 1.F - (depth * factor))
                                                      : std::max(0.F, depth * factor);

        if (pc_encoding::DEPTH_grayscale == encoding) {
            colors.emplace_back(intesity, intesity, intesity, 1.0F);
        } else  {
            const tinycolormap::Color c = tinycolormap::GetColor(static_cast<double>(intesity), color_type);
            colors.emplace_back(static_cast<float>(c.r()), static_cast<float>(c.g()),static_cast<float>(c.b()), 1.0F);
        }
    }

    return colors;
}

#endif // GLPOINTCLOUDOBJECT_H
