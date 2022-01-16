//
// Created by Tobiathan on 11/2/21.
//

#include "Intersector.h"
#include "../vendor/glm/geometric.hpp"
#include "../util/Util.h"

// credit: https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
bool Intersector::Segment(glm::vec2 p1, glm::vec2 q1, glm::vec2 p2, glm::vec2 q2) {
    int o1 = SegmentOrientation(p1, q1, p2);
    int o2 = SegmentOrientation(p1, q1, q2);
    int o3 = SegmentOrientation(p2, q2, p1);
    int o4 = SegmentOrientation(p2, q2, q1);

    return (o1 != o2 && o3 != o4);
}

int Intersector::SegmentOrientation(glm::vec2 p, glm::vec2 q, glm::vec2 r) {
    float val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;

    return (val > 0)? 1: 2;
}

// credit: https://stackoverflow.com/questions/14307158/how-do-you-check-for-intersection-between-a-line-segment-and-a-line-ray-emanatin
std::optional<glm::vec2> Intersector::RaySegment(glm::vec2 rayOrigin, glm::vec2 rayDirVec, glm::vec2 p1, glm::vec2 p2) {

    //return std::optional<glm::vec2>({1.0f, 1.0f});

    const auto v1 = rayOrigin - p1;
    const auto v2 = p2 - p1;
    const auto v3 = glm::vec2(-rayDirVec.y, rayDirVec.x);
    const float t1 = ((v2.x * v1.y) - (v2.y * v1.x)) / glm::dot(v2, v3); // uses 2D cross-product mag
    const float t2 = glm::dot(v1, v3) / glm::dot(v2, v3);

    if (t1 >= 0.0f && t2 >= 0.0f && t2 <= 1.0f) {
        return {rayOrigin + t1 * rayDirVec};
    }

    return std::nullopt;
}

// CREDIT: https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
bool Intersector::RayTriangleIntersection(Ray ray,
                                    Vec3 v0, Vec3 v1, Vec3 v2,
                                    MeshIntersection& outIntersection)
{
    const float EPSILON = 0.0000001;
    Vec3 edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = v1 - v0;
    edge2 = v2 - v0;
    h = glm::cross(ray.dir, edge2);
    a = glm::dot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.
    f = 1.0f / a;
    s = ray.origin - v0;
    u = f * glm::dot(s, h);
    if (u < 0.0 || u > 1.0)
        return false;
    q = glm::cross(s, edge1);
    v = f * glm::dot(ray.dir, q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * glm::dot(edge2, q);
    if (t > EPSILON) // ray intersection
    {
        outIntersection = {ray.origin + ray.dir * t, -glm::normalize(glm::cross(edge1, edge2))};
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}