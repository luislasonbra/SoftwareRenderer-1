#pragma once

/** @file */ 

namespace swr {

/// Maximum affine variables used for interpolation across the triangle.
const int MaxAVars = 16;

/// Maximum perspective variables used for interpolation across the triangle.
const int MaxPVars = 16;

/// Vertex input structure for the Rasterizer. Output from the VertexProcessor.
struct Vertex {
	float x;
	float y;
	float z;
	float w;

	/// Affine variables.
	float avar[MaxAVars];

	/// Perspective variables.
	float pvar[MaxPVars];
};


/// Interface for the rasterizer used by the VertexProcessor.
class IRasterizer {
public:
	virtual ~IRasterizer() {}

	/// Draw a list of points.
	/** Points with indices == -1 will be ignored. */
	virtual void drawPointList(const Vertex *vertices, const int *indices, size_t indexCount) const = 0;

	/// Draw a list if lines.
	/** Lines  with indices == -1 will be ignored. */
	virtual void drawLineList(const Vertex *vertices, const int *indices, size_t indexCount) const = 0;

	/// Draw a list of triangles.
	/** Triangles  with indices == -1 will be ignored. */
	virtual void drawTriangleList(const Vertex *vertices, const int *indices, size_t indexCount) const = 0;
};

} // end namespace swr