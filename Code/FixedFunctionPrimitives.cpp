#define WIN32_LEAN_AND_MEAN 
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <windowsx.h>

#ifdef USE_GLEW
#include <GL\glew.h>
#include <GL\wglew.h>
#endif
#include <gl\GL.h>

#include <cmath>

#include "FixedFunctionPrimitives.h"

#ifndef  M_PI
	#define M_PI 3.14159265358979323846f
#endif

void Render(const Circle& circle) {
	glBegin(GL_LINE_LOOP);

	for (int i = 0; i < 360; ++i) {
		glVertex2f(circle.position.x + cosf(DEG2RAD(i)) * circle.radius, circle.position.y + sinf(DEG2RAD(i)) * circle.radius);
	}

	glEnd();
}

void Render(const Line2D& line) {
	glBegin(GL_LINES);
	glVertex3f(line.start.x, line.start.y, 0.0f);
	glVertex3f(line.end.x, line.end.y, 0.0f);
	glEnd();
}

void Render(const Point2D& point) {
	glBegin(GL_POINTS);
	glVertex3f(point.x, point.y, 0.0f);
	glEnd();
}

void Render(const Rectangle2D& rect) {
	vec2 min = GetMin(rect);
	vec2 max = GetMax(rect);

	glBegin(GL_LINES); 
	glVertex3f(min[0], min[1], 0.0f);
	glVertex3f(min[0], max[1], 0.0f);
	glVertex3f(min[0], max[1], 0.0f);
	glVertex3f(max[0], max[1], 0.0f);
	glVertex3f(max[0], max[1], 0.0f);
	glVertex3f(max[0], min[1], 0.0f);
	glVertex3f(max[0], min[1], 0.0f);
	glVertex3f(min[0], min[1], 0.0f);
	glEnd();
}

void Render(const OrientedRectangle& rect) {
	glPushMatrix();
	glTranslatef(rect.position.x, rect.position.y, 0.0f);
	glRotatef(rect.rotation, 0.0f, 0.0f, 1.0f);

	vec2 min = vec2(-rect.halfExtents.x, -rect.halfExtents.y);
	vec2 max = vec2(+rect.halfExtents.x, +rect.halfExtents.y);
	
	glBegin(GL_LINES);
	glVertex3f(min[0], min[1], 0.0f);
	glVertex3f(min[0], max[1], 0.0f);
	glVertex3f(min[0], max[1], 0.0f);
	glVertex3f(max[0], max[1], 0.0f);
	glVertex3f(max[0], max[1], 0.0f);
	glVertex3f(max[0], min[1], 0.0f);
	glVertex3f(max[0], min[1], 0.0f);
	glVertex3f(min[0], min[1], 0.0f);
	glEnd();

	glPopMatrix();
}

void FixedFunctionSubdivTetrahedron(float* a, float* b, float* c, int div, float r) {
	if (div <= 0) {
		glNormal3fv(a);
		glVertex3f(a[0] * r, a[1] * r, a[2] * r);
		
		glNormal3fv(b);
		glVertex3f(b[0] * r, b[1] * r, b[2] * r);
		
		glNormal3fv(c);
		glVertex3f(c[0] * r, c[1] * r, c[2] * r);
	}
	else {
		float ab[3], ac[3], bc[3];

		ab[0] = (a[0] + b[0]) / 2.0f;
		ac[0] = (a[0] + c[0]) / 2.0f;
		bc[0] = (b[0] + c[0]) / 2.0f;

		ab[1] = (a[1] + b[1]) / 2.0f;
		ac[1] = (a[1] + c[1]) / 2.0f;
		bc[1] = (b[1] + c[1]) / 2.0f;
		
		ab[2] = (a[2] + b[2]) / 2.0f;
		ac[2] = (a[2] + c[2]) / 2.0f;
		bc[2] = (b[2] + c[2]) / 2.0f;

		// Normalize ab
		float d = sqrtf(ab[0] * ab[0] + ab[1] * ab[1] + ab[2] * ab[2]);
		ab[0] /= d; ab[1] /= d; ab[2] /= d;
		// Normalize ac
		d = sqrtf(ac[0] * ac[0] + ac[1] * ac[1] + ac[2] * ac[2]);
		ac[0] /= d; ac[1] /= d; ac[2] /= d;
		// Normalize bc
		d = sqrtf(bc[0] * bc[0] + bc[1] * bc[1] + bc[2] * bc[2]);
		bc[0] /= d; bc[1] /= d; bc[2] /= d;

		FixedFunctionSubdivTetrahedron(a, ab, ac, div - 1, r);
		FixedFunctionSubdivTetrahedron(b, bc, ab, div - 1, r);
		FixedFunctionSubdivTetrahedron(c, ac, bc, div - 1, r);
		FixedFunctionSubdivTetrahedron(ab, bc, ac, div - 1, r); 
	}
}

void FixedFunctionSphere() {
	FixedFunctionSphere(2, 1.0f);
}

void FixedFunctionSphere(int numDivisions, float radius) {
	static float X = 0.525731112119133606f;
	static float Y = 0.0f;
	static float Z = 0.850650808352039932f;

	static float vdata[12][3] = {
		{ -X,Y,Z },{ X,Y,Z },{ -X,Y,-Z },{ X,Y,-Z },
		{ Y,Z,X },{ Y,Z,-X },{ Y,-Z,X },{ Y,-Z,-X },
		{ Z,X,Y },{ -Z,X,Y },{ Z,-X,Y },{ -Z,-X,Y } 
	};
	static int tindices[20][3] = { 
		{ 0,4,1 },{ 0,9,4 },{ 9,5,4 },{ 4,5,8 },{ 4,8,1 },
		{ 8,10,1 },{ 8,3,10 },{ 5,3,8 },{ 5,2,3 },{ 2,7,3 },
		{ 7,10,3 },{ 7,6,10 },{ 7,11,6 },{ 11,0,6 },{ 0,1,6 },
		{ 6,1,10 },{ 9,0,11 },{ 9,11,2 },{ 9,2,5 },{ 7,2,11 } 
	};

	glBegin(GL_TRIANGLES);

	for (int i = 0; i < 20; ++i) {
		FixedFunctionSubdivTetrahedron(
			vdata[tindices[i][0]], vdata[tindices[i][1]], vdata[tindices[i][2]], numDivisions, radius
		);
	}

	glEnd();
}

void FixedFunctionCube() {
	FixedFunctionCube(1.0f, 1.0f, 1.0f);
}

void FixedFunctionCube(float extentsX, float extentsY, float extentsZ) {
	float min[] = { -extentsX, -extentsY, -extentsZ };
	float max[] = { +extentsX, +extentsY, +extentsZ };

	glBegin(GL_TRIANGLES);

#if 1
	// Top!
	glNormal3f(0.0f, 1.0f, 0.0f);
	/*0*/glVertex3f(min[0], max[1], min[2]);
	/*1*/glVertex3f(max[0], max[1], min[2]);
	/*3*/glVertex3f(min[0], max[1], max[2]);
	/*1*/glVertex3f(max[0], max[1], min[2]);
	/*2*/glVertex3f(max[0], max[1], max[2]);
	/*3*/glVertex3f(min[0], max[1], max[2]);
#endif

#if 1
	// Front!
	glNormal3f(0.0f, 0.0f, 1.0f);
	/*0*/glVertex3f(min[0], max[1], max[2]);
	/*1*/glVertex3f(max[0], max[1], max[2]);
	/*3*/glVertex3f(min[0], min[1], max[2]);
	/*1*/glVertex3f(max[0], max[1], max[2]);
	/*2*/glVertex3f(max[0], min[1], max[2]);
	/*3*/glVertex3f(min[0], min[1], max[2]);
#endif

#if 1
	// Left
	glNormal3f(-1.0f, 0.0f, 0.0f);
	/*0*/glVertex3f(min[0], max[1], max[2]);
	/*3*/glVertex3f(min[0], min[1], max[2]);
	/*1*/glVertex3f(min[0], max[1], min[2]);
	/*1*/glVertex3f(min[0], max[1], min[2]);
	/*3*/glVertex3f(min[0], min[1], max[2]);
	/*2*/glVertex3f(min[0], min[1], min[2]);
#endif

#if 1
	// Bottom!
	glNormal3f(0.0f, -1.0f, 0.0f);
	/*0*/glVertex3f(min[0], min[1], min[2]);
	/*3*/glVertex3f(min[0], min[1], max[2]);
	/*1*/glVertex3f(max[0], min[1], min[2]);
	/*1*/glVertex3f(max[0], min[1], min[2]);
	/*3*/glVertex3f(min[0], min[1], max[2]);
	/*2*/glVertex3f(max[0], min[1], max[2]);
#endif

#if 1
	// Back!
	glNormal3f(0.0f, 0.0f, -1.0f);
	/*0*/glVertex3f(min[0], max[1], min[2]);
	/*3*/glVertex3f(min[0], min[1], min[2]);
	/*1*/glVertex3f(max[0], max[1], min[2]);
	/*1*/glVertex3f(max[0], max[1], min[2]);
	/*3*/glVertex3f(min[0], min[1], min[2]);
	/*2*/glVertex3f(max[0], min[1], min[2]);
#endif

#if 1
	// Right
	glNormal3f(1.0f, 0.0f, 0.0f);
	/*0*/glVertex3f(max[0], max[1], max[2]);
	/*1*/glVertex3f(max[0], max[1], min[2]);
	/*3*/glVertex3f(max[0], min[1], max[2]);
	/*1*/glVertex3f(max[0], max[1], min[2]);
	/*2*/glVertex3f(max[0], min[1], min[2]);
	/*3*/glVertex3f(max[0], min[1], max[2]);
#endif

	glEnd();
}

void FixedFunctionTorus(float outerRaidus, float innerRadius) {
	float diff = (outerRaidus - innerRadius);
	float minorRad = diff * 0.5f;
	float majorRad = outerRaidus - minorRad;
	FixedFunctionTorus(15, 10, majorRad, minorRad);
}

void FixedFunctionCylinder(float height, float radius) {
	FixedFunctionCylinder(15, height, radius);
}

void FixedFunctionCylinder(int slices, float height, float radius) {
	// Modified from http://math.hws.edu/graphicsbook/c4/s2.html
	height *= 0.5f;
	float twopi_slices = (2.0f * M_PI) / (float)slices;

	// HULL
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i <= slices; i++) {
		double angle = twopi_slices * (float)i;  // i 16-ths of a full circle
		double x = cos(angle);
		double z = sin(angle);

		float norm[] = { x, 0.0f, z };
		float d = sqrtf(norm[0] * norm[0] + norm[1] * norm[1] + norm[2] * norm[2]);
		norm[0] /= d; norm[1] /= d; norm[2] /= d;

		x *= radius;
		z *= radius;

		glNormal3f(norm[0], norm[1], norm[2]); 
		glVertex3f(x, height, z);  
		glVertex3f(x, -height, z);
	}
	glEnd();

	// TOP CAP
	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_TRIANGLE_FAN); 
	for (int i = 0; i <= slices; i++) {
		double angle = twopi_slices * (float)i;
		double x = cos(angle) * radius;
		double z = sin(angle) * radius;
		glVertex3f(x, height, z);
	}
	glEnd();

	// BOTTOM CAP
	glNormal3f(0.0f, -1.0f, 0.0f);
	glBegin(GL_TRIANGLE_FAN); 
	for (int i = slices; i >= 0; i--) {
		double angle = twopi_slices * (float)i;
		double x = cos(angle) * radius;
		double z = sin(angle) * radius;
		glVertex3f(x, -height, z);
	}
	glEnd();
}

/* From the opengl super bible! */
void FixedFunctionTorus(int TORUS_MAJOR_RES, int TORUS_MINOR_RES, float TORUS_MAJOR, float TORUS_MINOR) {
	int    i, j, k;
	double s, t, x, y, z, nx, ny, nz, scale, twopi;

	twopi = 2.0 * M_PI;
	for (i = 0; i < TORUS_MINOR_RES; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= TORUS_MAJOR_RES; j++)
		{
			for (k = 1; k >= 0; k--)
			{
				s = (i + k) % TORUS_MINOR_RES + 0.5;
				t = j % TORUS_MAJOR_RES;

				// Calculate point on surface
				x = (TORUS_MAJOR + TORUS_MINOR * cos(s * twopi / TORUS_MINOR_RES)) * cos(t * twopi / TORUS_MAJOR_RES);
				y = TORUS_MINOR * sin(s * twopi / TORUS_MINOR_RES);
				z = (TORUS_MAJOR + TORUS_MINOR * cos(s * twopi / TORUS_MINOR_RES)) * sin(t * twopi / TORUS_MAJOR_RES);

				// Calculate surface normal
				nx = x - TORUS_MAJOR * cos(t * twopi / TORUS_MAJOR_RES);
				ny = y;
				nz = z - TORUS_MAJOR * sin(t * twopi / TORUS_MAJOR_RES);
				scale = 1.0 / sqrt(nx*nx + ny*ny + nz*nz);
				nx *= scale;
				ny *= scale;
				nz *= scale;

				glNormal3f((float)nx, (float)ny, (float)nz);
				glVertex3f((float)x, (float)y, (float)z);
			}
		}

		glEnd();
	}
}

/* https://www.opengl.org/archives/resources/code/samples/redbook/torus.c
void FixedFunctionTorus(int numc, int numt) {
	int i, j, k;
	double s, t, x, y, z, twopi;

	twopi = 2 * M_PI;
	for (i = 0; i < numc; i++) {
		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= numt; j++) {
			for (k = 1; k >= 0; k--) {
				s = (i + k) % numc + 0.5;
				t = j % numt;

				x = (1 + .1*cos(s*twopi / numc))*cos(t*twopi / numt);
				y = (1 + .1*cos(s*twopi / numc))*sin(t*twopi / numt);
				z = .1 * sin(s * twopi / numc);
				glVertex3f(x, y, z);
			}
		}
		glEnd();
	}
} */