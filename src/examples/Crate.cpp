#include "SDL.h"
#include "Rasterizer.h"
#include "VertexProcessor.h"
#include "ObjData.h"
#include "vector_math.h"

typedef vmath::vec3<float> vec3f;
typedef vmath::vec4<float> vec4f;
typedef vmath::mat4<float> mat4f;

using namespace swr;

class PixelShader : public PixelShaderBase<PixelShader> {
public:
	static const bool InterpolateZ = false;
	static const bool InterpolateW = false;
	static const int VarCount = 3;

	static SDL_Surface* surface;
	static SDL_Surface* texture;

	static void drawPixel(const PixelData &p)
	{
		int rint = (int)(p.var[0] * 255);
		int gint = (int)(p.var[1] * 255);
		int bint = (int)(p.var[2] * 255);

		Uint32 color = rint << 16 | gint << 8 | bint;

		Uint32 *buffer = (Uint32*)((Uint8 *)surface->pixels + (int)p.y * surface->pitch + (int)p.x * 4);
		*buffer = color;
	}
};

SDL_Surface* PixelShader::surface;
SDL_Surface* PixelShader::texture;

class VertexShader : public VertexShaderBase<VertexShader> {
public:
	static const int AttribCount = 2;

	static mat4f modelViewProjectionMatrix;

	static void processVertex(VertexShaderInput in, VertexShaderOutput *out)
	{
		const ObjData::VertexArrayData *data = static_cast<const ObjData::VertexArrayData*>(in[0]);

		vec4f position = modelViewProjectionMatrix * vec4f(data->vertex, 1.0f);

		out->x = position.x;
		out->y = position.y;
		out->z = position.y;
		out->w = position.w;
		out->var[0] = data->texcoord.x;
		out->var[1] = data->texcoord.y;
	}
};

mat4f VertexShader::modelViewProjectionMatrix;

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow(
		"Crate",
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED,
		640, 
		480,
		0
	);

	SDL_Surface *screen = SDL_GetWindowSurface(window);
	srand(1234);

	std::vector<ObjData::VertexArrayData> vdata;
	std::vector<int> idata;
	ObjData::loadFromFile("data/crate.obj").toVertexArray(vdata, idata);

	Rasterizer r;
	VertexProcessor v(&r);
	
	r.setRasterMode(RasterMode::Span);
	r.setScissorRect(0, 0, 640, 480);
	r.setPixelShader<PixelShader>();
	PixelShader::surface = screen;

	v.setViewport(0, 0, 640, 480);
	v.setCullMode(CullMode::CW);
	v.setVertexShader<VertexShader>();

	mat4f lookAtMatrix = vmath::lookat_matrix(vec3f(3.0f, 2.0f, 5.0f), vec3f(0.0f), vec3f(0.0f, 1.0f, 0.0f));
	mat4f perspectiveMatrix = vmath::perspective_matrix(60.0f, 4.0f / 3.0f, 0.1f, 10.0f);
	VertexShader::modelViewProjectionMatrix = perspectiveMatrix * lookAtMatrix;

	v.setVertexAttribPointer(0, sizeof(ObjData::VertexArrayData), &vdata[0]);
	v.drawElements(DrawMode::Triangle, idata.size(), &idata[0]);

	SDL_UpdateWindowSurface(window);

	SDL_Event e;
	while (SDL_WaitEvent(&e) && e.type != SDL_QUIT);

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}