#ifndef GRAPHICS_MATH_HPP
#define GRAPHICS_MATH_HPP

#pragma once
#include <cmath>
#include <iostream>

#include <QMatrix4x4>
#include <QVector2D>

namespace graphics {

namespace math {

inline float
clampf(const float& val, const float& lo, const float& hi)
{
    return std::min(std::max(val, lo), hi);
}

inline float
deg_to_radians(const float& v)
{
    return v * M_PIf32 / 180.0f;
}

inline float
radians_to_deg(const float& v)
{
    return v * 180.0f / M_PIf32;
}

// To screen
inline QVector4D
world_to_eye(const QVector3D& v, const QMatrix4x4& view)
{
    return view * QVector4D(v.x(), v.y(), v.z(), 1.f);
}

inline QVector4D
eye_to_clip(const QVector4D& v, const QMatrix4x4& proj)
{
    return proj * v;
}

inline QVector3D
clip_to_ndc(const QVector4D& v)
{
    auto const ndc_4 = v / v.w();
    return QVector3D(ndc_4.x(), ndc_4.y(), ndc_4.z());
}

inline QVector2D
ndc_to_screen(const QVector3D& ndc, const QVector2D& window_size)
{
    float const x = ((ndc.x() + 1.f) / 2.f) * window_size.x();
    float const y = ((1.f - ndc.y()) / 2.f) * window_size.y();

    return QVector2D(x, y);
}

 // From screen
inline QVector3D
screen_to_ndc(const QVector2D& screen_coords, const QVector2D& window_size, float const z)
{
    float const x = 2.f * screen_coords.x() / static_cast<float>(window_size.x()) - 1.f;
    float const y = -2.f * screen_coords.y() / static_cast<float>(window_size.y()) + 1.f;
    return QVector3D(x, y, z);
}

inline QVector4D
ndc_to_clip(const QVector3D& ndc)
{
    return QVector4D(ndc.x(), ndc.y(), ndc.z(), 1.f);
}

inline QVector4D
clip_to_eye(const QVector4D &clip, const QMatrix4x4& proj)
{
    const QVector4D eye_coord = proj.inverted() * clip;
    return QVector4D(eye_coord.x(), eye_coord.y(), eye_coord.z(), 0.f);
}

inline QVector3D
eye_to_world(const QVector4D &eye, const QMatrix4x4& view)
{
    const QVector4D world_pos = view.inverted() * eye;
    return QVector3D(world_pos.x(), world_pos.y(), world_pos.z());
}

inline QVector2D
world_to_screen(const QVector3D& world, const QMatrix4x4& proj, const QMatrix4x4& view, const QVector2D& window_size)
{
//    qDebug() << "!!!world_to_screen, view:\n"  << view;
    const QVector4D eye  = world_to_eye(world, view);
    const QVector4D clip = eye_to_clip(eye, proj);
    const QVector3D ndc  = clip_to_ndc(clip);
    const QVector2D screen  = ndc_to_screen(ndc, window_size);

    return screen;
}

inline QVector3D
screen_to_world(const QVector2D& screen_point, const QMatrix4x4& proj, const QMatrix4x4& view, const QVector2D& window_size, const QVector3D& origin_world, float const z)
{
    const QVector3D ndc   = screen_to_ndc(screen_point, window_size, z);
//    const QVector4D clip  = ndc_to_clip(ndc);
//    const QVector4D eye   = clip_to_eye(clip, proj);
//    const QVector3D world = eye_to_world(eye, view);

    QVector4D clip(ndc.x(), ndc.y(), ndc.z(), 1.f);
    const QVector4D eye_coord = proj.inverted() * clip;
    const QVector4D world_pos = view.inverted() * QVector4D(eye_coord.x(), eye_coord.y(), eye_coord.z(), 0.f);
    const QVector3D ray_world(world_pos.x(), world_pos.y(), world_pos.z());
    QVector3D world = ray_world - origin_world; // = ray_world;

    // No projection
//    QVector4D eye_coord(ndc.x(), ndc.y(), ndc.z(), 0.f);
//    const QVector4D world_pos = view.inverted() * eye_coord;
//    const QVector3D ray_world(world_pos.x(), world_pos.y(), world_pos.z());
//    QVector3D world = ray_world - origin_world;

    return world;
}


inline QVector3D
ndc_to_world(const QVector3D& ndc, const QMatrix4x4& proj, const QMatrix4x4& view)
{
    const QVector4D clip  = ndc_to_clip(ndc);
    const QVector4D eye   = clip_to_eye(clip, proj);
    const QVector3D world = eye_to_world(eye, view);
    return world.normalized();
}

//    QVector3D ray_cast(const float u, const float v)
//    {
//        // Step 1: VIEWPORT space (u,v) --> Normalized Device Space (NDC) [x,y,z]
//        float x = (2.0F * u) / static_cast<float>(m_window_width) - 1.0F;
//        float y = 1.0F - (2.0F * v) / static_cast<float>(m_window_height); // Normally y goes up, but here direction is down by convention
//    //    float y = (2.f * v) / static_cast<float>(m_window_height) - 1.f;
//        QVector3D ray_nds = QVector3D(x, y, 0.0f);
//        // Step 2: Normalized Device Space (NDC) [x,y,z] --> CLIP space [x,y,z,w]
//        QVector4D ray_clip = QVector4D(ray_nds.x(), ray_nds.y(), -1.0F, 1.0F); // like NDC but in homogeneous coordinates
//        // eye space to clip we would multiply by projection so clip space to eye space is the inverse projection
//        // Step 3:  CLIP space [x,y,z,w] --> CAMERA (EYE) space [x,y,z,1]
//        QVector4D ray_eye = m_proj.inverted() * ray_clip;
//        // convert point to forwards
//        ray_eye = QVector4D(ray_eye.x(), ray_eye.y(), 1.0F, 0.0f);
//        // world space to eye space is usually multiply by view so eye space to world space is inverse view
//        // Step 4: CAMERA (EYE) space [x,y,z,1] --> WORLD space [x,y,z,1]
//        QVector4D inv_ray_world = m_view.inverted() * ray_eye;
//        QVector3D ray_world = QVector3D(inv_ray_world.x(), inv_ray_world.y(), inv_ray_world.z());
//        ray_world = ray_world.normalized();

//        return ray_world;
//    }

inline QVector3D
ray_cast(const QVector2D& screen_point, const QMatrix4x4& proj, const QMatrix4x4& view, const QVector2D& window_size, const QVector3D& origin_world, const float z)
{
    return screen_to_world(screen_point, proj, view, window_size, origin_world, z).normalized();
}

inline QVector3D
ray_plane_intersection(const QVector3D ray_position, const QVector3D ray_direction, const QVector3D plane_normal, const QVector3D plane_position)
{
    float d = QVector3D::dotProduct(plane_normal, plane_position - ray_position) / (0.001f+QVector3D::dotProduct(ray_direction, plane_normal));
//    float d = QVector3D::dotProduct(plane_normal, QVector3D(0,0,1)) / (0.001f+QVector3D::dotProduct(ray_direction, plane_normal));
    std::cerr << "d= " << d << "\n";
    return ray_position + ray_direction * d;
}

inline QVector3D
compute_plane_normal(const QVector3D p1, const QVector3D p2, const QVector3D p3)
{
    QVector3D p1p2 = p2 - p1;
    QVector3D p1p3 = p3 - p1;
    QVector3D n = QVector3D::crossProduct(p1p2, p1p3);

//    std::cerr << "compute_plane_normal n.normalized(): " << n.normalized().x() << ", " << n.normalized().y() << ", " << n.normalized().z() << "\n";

    return n.normalized();

//    return QVector3D::crossProduct(p2 - p1, p3 - p1).normalized();
}

/*!
 * \brief ray_intersects_triangle
 * Möller–Trumbore intersection algorithm
 * Code based on:
 * https://en.wikipedia.org/wiki/Möller–Trumbore_intersection_algorithm
 * \param ray_origin
 * \param ray_vector
 * \param vertex0
 * \param vertex1
 * \param vertex2
 * \param out_intersection_point
 * \return
 */
inline bool
ray_intersects_triangle(const QVector3D ray_origin, const QVector3D ray_vector, const QVector3D vertex0, const QVector3D vertex1, const QVector3D vertex2, QVector3D& out_intersection_point)
{
    constexpr float EPSILON = 0.0000001f;
    QVector3D edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = QVector3D::crossProduct(ray_vector, edge2);
    a = QVector3D::dotProduct(edge1, h);
    if (a > -EPSILON && a < EPSILON){
        std::cerr << "This ray is parallel to this triangle.\n";
        return false;    // This ray is parallel to this triangle.
    }
    f = 1.0F/a;
    s = ray_origin - vertex0;
    u = f * QVector3D::dotProduct(s, h);
    if (u < 0.0f || u > 1.0F) {
        return false;
    }
    q = QVector3D::crossProduct(s, edge1);
    v = f * QVector3D::dotProduct(ray_vector, q);

    if (v < 0.0f || u + v > 1.0F){
        return false;
    }
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * QVector3D::dotProduct(edge2, q);
    if (t > EPSILON) // ray intersection
    {
        out_intersection_point = ray_origin + ray_vector * t;
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

inline int
compute_world_intersection_point(const QVector2D mouse_pos, const QVector3D ray_origin,
                                 const QMatrix4x4& proj, const QMatrix4x4& view,
                                 const QVector2D& window_size,
                                 const std::vector<std::vector<QVector3D>> &quad_vertices)
{
    QVector3D origin_world = QVector3D(0,0,0);// ray_origin;
    QVector3D ray = ray_cast(mouse_pos, proj, view, window_size, origin_world, -1.f);
    //qDebug() << "ray :" << ray;

    QVector3D out_intersection_point;
    std::size_t cnt = 0;
    for (auto q : quad_vertices) {
        if (ray_intersects_triangle(ray_origin, ray, q[0], q[1], q[2], out_intersection_point)) {
            qDebug() << cnt << "triangleA at out_intersection_point" << out_intersection_point;
            return static_cast<int>(cnt);
        }
        if (ray_intersects_triangle(ray_origin, ray, q[3], q[4], q[5], out_intersection_point)) {
            qDebug() << cnt << "triangleB at out_intersection_point" << out_intersection_point;
            return static_cast<int>(cnt);
        }

        cnt++;
    }

//    qDebug() << "at end out_intersection_point " << out_intersection_point;

    return -1;
}

}

}

#endif // GRAPHICS_MATH_HPP
