#include "frustum.hpp"

frustum_planes extract_frustum_planes(const glm::mat4& mat, const bool normalize)
{
	frustum_planes frustum;

    // Left clipping plane
    frustum.left.a = mat[0][3] + mat[0][0];
    frustum.left.b = mat[1][3] + mat[1][0];
    frustum.left.c = mat[2][3] + mat[2][0];
    frustum.left.d = mat[3][3] + mat[3][0];

    // Right clipping plane
    frustum.right.a = mat[0][3] - mat[0][0];
    frustum.right.b = mat[1][3] - mat[1][0];
    frustum.right.c = mat[2][3] - mat[2][0];
    frustum.right.d = mat[3][3] - mat[3][0];

    // Top clipping plane
    frustum.top.a = mat[0][3] - mat[0][1];
    frustum.top.b = mat[1][3] - mat[1][1];
    frustum.top.c = mat[2][3] - mat[2][1];
    frustum.top.d = mat[3][3] - mat[3][1];

    // Bottom clipping plane
    frustum.bot.a = mat[0][3] + mat[0][1];
    frustum.bot.b = mat[1][3] + mat[1][1];
    frustum.bot.c = mat[2][3] + mat[2][1];
    frustum.bot.d = mat[3][3] + mat[3][1];

    // Near clipping plane
    frustum.near.a = mat[0][3] + mat[0][2];
    frustum.near.b = mat[1][3] + mat[1][2];
    frustum.near.c = mat[2][3] + mat[2][2];
    frustum.near.d = mat[3][3] + mat[3][2];

    // Far clipping plane
    frustum.far.a = mat[0][3] - mat[0][2];
    frustum.far.b = mat[1][3] - mat[1][2];
    frustum.far.c = mat[2][3] - mat[2][2];
    frustum.far.d = mat[3][3] - mat[3][2];

    // Normalize the plane equations, if requested
    if (normalize)
    {
		frustum.left = Plane::normalize(frustum.left);
		frustum.right = Plane::normalize(frustum.right);
		frustum.top = Plane::normalize(frustum.top);
		frustum.bot = Plane::normalize(frustum.bot);
		frustum.near = Plane::normalize(frustum.near);
		frustum.far = Plane::normalize(frustum.far);
    } 

	return frustum;
}

bool aabb_intersects_frustum(const frustum_planes& frustum, const Aabb& aabb)
{
    return aabb_intersects_plane(frustum.near, aabb)
           || aabb_intersects_plane(frustum.far, aabb)
           || aabb_intersects_plane(frustum.top, aabb)
           || aabb_intersects_plane(frustum.bot, aabb)
           || aabb_intersects_plane(frustum.left, aabb)
           || aabb_intersects_plane(frustum.right, aabb);
}

bool aabb_outside_frustum(const frustum_planes& frustum, const Aabb& aabb)
{
    return aabb_outside_plane(frustum.near, aabb)
           || aabb_outside_plane(frustum.far, aabb)
           || aabb_outside_plane(frustum.top, aabb)
           || aabb_outside_plane(frustum.bot, aabb)
           || aabb_outside_plane(frustum.left, aabb)
           || aabb_outside_plane(frustum.right, aabb);
}
