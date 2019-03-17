
#include <sstream>
#include <Urho3D/Math/Vector3.h>
using namespace Urho3D;	

	static const int res = 100;
	Vector3 Quadratic(float x)
	{
		const float scaleY = 2.0f / res;
		return Vector3(x, x*x*scaleY, res);
	}

	Vector3 Linear(float x)
	{
		const float scaleY = 2.0f / res;
		return Vector3(x, x*scaleY, res);
	}

	Vector3 Sin(float amplitude, float phase, float freq, float x) {
		float real_x = x * 10 * M_PI / res;
		float y = amplitude * sin(freq * real_x + phase);
		return Vector3(real_x, y, res);
	}

	Vector3 Sine(float x, float z, float t)
	{
		Vector3 p;
		float y = sin(M_PI * (x + t));
		return Vector3(x, y, z);
	}

	Vector3 MultiSine(float x, float z, float t)
	{
		float y = sin(M_PI * (x + t));
		y += sin(2.0f * M_PI * (x + 2.0f * t)) / 2.0f;
		y *= 2.0f / 3.0f;
		Vector3 p;
		return Vector3(x, y, z);
	}

	Vector3 Sine2D(float x, float  z, float t)
	{
		float sinx = sin(M_PI * (x + t));
		float sinz = sin(M_PI * (z + t));
		Vector3 p;
		float y = (sinx + sinz) * 0.5f;
		return Vector3(x, y, z);
	}

	Vector3 MultiSine2DFunction(float x, float z, float t)
	{
		float y = 4.0f * sin(M_PI * (x + z + t * 0.5f));
		y += sin(M_PI * (x + t));
		y += sin(2.0f * M_PI * (z + 2.0f * t)) * 0.5f;
		y *= 1.0f / 5.5f;
		Vector3 p;
		return Vector3(x, y, z);
	}

	Vector3 Ripple(float x, float z, float t)
	{
		float d = sqrt(x * x + z * z);
		float y = sin(M_PI * (4.0f * d - t));
		y /= 1.0f + 10.0f + d;
		return Vector3(x, y, z);
	}

	Vector3 Cylinder(float u, float v, float t)
	{
		float radius = 0.8f + sin(M_PI * (6.0f * u + 2.0f * v + t)) * 0.2f;
		Vector3 p;
		float x = radius * sin(M_PI * u);
		float y = v;
		float z = radius * cos(M_PI * u);
		return Vector3(x, y, z);
	}

	Vector3 sphere(float u, float v, float t)
	{
		float r = 0.8f + sin(M_PI * (6.0f * u + t)) * 0.1f;
		r += sin(M_PI * (4.0f * v + t)) * 0.1f;
		float s = r * cos(M_PI * 0.5f * v);
		float x = s * sin(M_PI * u);
		float y = r * sin(M_PI * 0.5f * v);
		float z = s * cos(M_PI * u);
		return Vector3(x, y, z);
	}

	Vector3 Torus(float u, float v, float t)
	{
		float r1 = 0.65f + sin(M_PI * (6.0f * u + t)) * 0.1f;
		float r2 = 0.2f + sin(M_PI * (4.0f * v + t)) * 0.05f;
		float s = r2 * cos(M_PI * v) + r1;
		float x = s * sin(M_PI * u);
		float y = r2 * sin(M_PI * v);
		float z = s * cos(M_PI * u);
		return Vector3(x, y, z);
	}


