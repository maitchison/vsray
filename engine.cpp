//#include <GL/freeglut.h>
//#include <GL/freeglut_ext.h>
//#include <GL/freeglut_std.h>

/*

todo:

[done] Scenes 
[done] Planes
[done] Reflections
progessive render
put classes into files
proper camera tracer
UV mapping
aa
lighting 
shadows

triangles / meshes
light gathering (ambient occlusion)

*/

#include <vector>

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
	#include <GL/gl.h>
#endif

// why is c++ so bad with having to do things like this?
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <algorithm>
#include <limits>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SCREEN_SCALE = 2;
uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
GLuint tex;

float currentTime = 0.0;
const float EPSILON = 0.00001;

/*
--------------------------------------------------------
Utilities
--------------------------------------------------------
*/

/***
 * Convert from degrees to radians.
 * @param: deg: the degrees to convert
 * @return: the result in radians.
 */
float deg2rad(float deg)
{
	return deg * (M_PI / 180.0);
}

float randf()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

int clip(int x, int a, int b)
{
	if (x < a) return a;
	if (x > b) return b;
	return x;
}

float clipf(float x, float a, float b)
{
	if (x < a) return a;
	if (x > b) return b;
	return x;
}

float minf(float a, float b) 
{
	return (a < b) ? a : b;
}

struct Color
{
	float r;
	float g;
	float b;
	float a;

	Color()
	{
		r = g = b = 0.0f;
	}

	Color(float _r, float _g, float _b, float _a=1.0)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}	

	int int24() 
	{
		int _r = clip(int(r * 255), 0, 255);
		int _g = clip(int(g * 255), 0, 255);
		int _b = clip(int(b * 255), 0, 255);
		return (_r) + (_g << 8) + (_b << 16);
	}

};

struct Vec3d
{
	float x;
	float y;
	float z;

	Vec3d()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	Vec3d(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	float abs() {
		return sqrtf(abs2());
	}

	float abs2() {
		return x * x + y * y + z * z;
	}

	float xAngle() {
		return atan2(y,z);
	}

	float yAngle() {
		return atan2(z,x);
	}

	float zAngle() {
		return atan2(y,x);
	}

	static float Dot(Vec3d a, Vec3d b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	Vec3d normalized() {
		float l = abs();
		return Vec3d(x/l, y/l, z/l);
	}

	/* Overload + operator to add two Box objects. */
	Vec3d operator+(Vec3d b)
	{
		return Vec3d(this->x + b.x, this->y + b.y, this->z + b.z);
	}

	Vec3d operator-(Vec3d b)
	{
		return Vec3d(this->x - b.x, this->y - b.y, this->z - b.z);
	}

	/* Overload + operator to add two Box objects. */
	Vec3d operator*(float b)
	{
		return Vec3d(b * x, b * y, b * z);
	}

	void rotateX(float theta) 
	{
		float nx = x;
		float ny = y * cos(theta) - z * sin(theta);
		float nz = y * sin(theta) + z * cos(theta);
		x = nx; y = ny; z = nz;
	}

	void rotateY(float theta)
	{
		float nx = x * cos(theta) + z * sin(theta);
		float ny = y;
		float nz = x * -sin(theta) + z * cos(theta);
		x = nx; y = ny; z = nz;
	}

	void rotateZ(float theta)
	{
		float nx = x * cos(theta) - y * sin(theta);
		float ny = x * sin(theta) + y * cos(theta);
		float nz = z;
		x = nx; y = ny; z = nz;
	}


	/** Rotate around x, then y then z */
	void rotate(float rx, float ry, float rz) 
	{				
		rotateX(rx);
		rotateY(ry);
		rotateZ(rz);
	}

};


/*
--------------------------------------------------------
Ray tracing code
--------------------------------------------------------
*/

struct CollisionResult;
struct Entity;
struct Ray;
struct Sphere;
struct Plane;
class Scene;

CollisionResult RaySphereIntersection(Ray* ray, Sphere* sphere);
CollisionResult RayPlaneIntersection(Ray* ray, Plane* plane);

void putPixel(int x, int y, Color col);

class Scene
{
	std::vector<Entity*> entities = std::vector<Entity*>();

public:
	void AddEntity(Entity* entity)
	{
		entities.push_back(entity);
	}

	CollisionResult Trace(Ray* ray);	
};




struct CollisionResult
{
	Vec3d location;
	float distance;
	Vec3d normal;
	Entity* entity;

	CollisionResult(Vec3d location, Entity* entity = NULL, float distance=0) 
	{
		this->location = location;
		this->entity = entity;
		this->distance = distance;
	}

	static CollisionResult Empty() 
	{
		return CollisionResult(Vec3d(0, 0, 0), NULL);
	}

	bool hit() 
	{
		return entity != NULL;
	}

};


struct Entity
{
	Vec3d location;
	Color color;

	Entity(Vec3d location)
	{
		this->location = location;
		this->color = Color(0.5f,0.5f,0.5f);
	}

	public:
	/** test ray collision with this entity. */
	virtual CollisionResult Trace(Ray* ray);

	virtual Color getColor(Vec3d location) 
	{
		return color;
	}

};

struct Ray : Entity
{
	Vec3d direction;
	Entity* owner;

	Ray(Vec3d location, Vec3d direction) : Entity(location)
	{
		this->direction = direction;	
		this->owner = NULL;	
	}

	Vec3d Project(Vec3d p)
	{
		p = p - location;
		return location + (direction * (Vec3d::Dot(direction, p) / direction.abs2()));
	}

};

struct Sphere : Entity
{
	float radius;

	public:
	Sphere(Vec3d location, float radius) : Entity(location)
	{
		this->radius = radius;
	}

	public:
	virtual CollisionResult Trace(Ray* ray) override
	{
		return RaySphereIntersection(ray, this);
	}

	virtual Color getColor(Vec3d location) override
	{
		return color;
		//bool cell = location.x < this->location.x;
		//return cell ? Color(1,0,0) : Color(0,0,1);
	}

};

struct Plane : Entity
{
	Vec3d normal;

	public:
	Plane(Vec3d location, Vec3d normal) : Entity(location)
	{
		this->normal = normal;
	}

	public:
	virtual CollisionResult Trace(Ray* ray) override
	{
		return RayPlaneIntersection(ray, this);
	}

	virtual Color getColor(Vec3d location) override
	{
		bool cell = (int(location.x / 1) + int(location.z / 1)) % 2;
		return cell ? Color(0,0,0) : Color(0.5, 0.5, 0.5);
	}

};


CollisionResult RaySphereIntersection(Ray* ray, Sphere* sphere)
{	
	// first we project the circle center onto the line.
	// p is now the closest point of the ray to the sphere.
	Vec3d p = ray->Project(sphere->location);
	float l = (p-ray->location).abs();
	
	// now we have a triangle from the sphere's center, to the projected point, and the intesection point.
	// we use r^2 = a^2 + b^2 and solve for a.
	float b = (p-sphere->location).abs();

	// check if we are too far away to hit the sphere.
	if (b > sphere->radius) return CollisionResult::Empty();

	float r = sphere->radius;
	float c = sqrt(r * r - b * b);

	// pick the closest point
	float t1 = l + c;
	float t2 = l - c;

	/*	
	float t = 0;
	if ((t1 > 0) && (t1 < t2)) t = t1;
	if ((t2 > 0) && (t2 < t1)) t = t2;
	*/
	float t = t2;

	if (t < EPSILON) return CollisionResult::Empty();

	Vec3d hit = ray->location + (ray->direction * t);

	CollisionResult result = CollisionResult(hit, sphere, t);	
	result.normal = (hit - sphere->location).normalized();
	return result;
}

CollisionResult RayPlaneIntersection(Ray* ray, Plane* plane)
{	
	// translated from python from
	// https://stackoverflow.com/questions/5666222/3d-line-plane-intersection

	// rotate objects such that plane is centered at 0,0 and facing up.
    Vec3d u = ray->direction;
    float dot = Vec3d::Dot(plane->normal, u);

    if (abs(dot) > EPSILON)
	{
        Vec3d w = ray->location - plane->location;
        float t = -Vec3d::Dot(plane->normal, w) / dot;
        if (t > EPSILON)
		{
			CollisionResult result = CollisionResult(ray->location + ray->direction * t, plane, t);
			result.normal = plane->normal.normalized();
			return result;
		}
	} 
    
	// The ray is parallel to plane, or behind it.
    return CollisionResult::Empty();
}



CollisionResult Entity::Trace(Ray* ray) 
{
	return CollisionResult::Empty();
}


struct Camera : Entity
{
	Vec3d rotation;
	float fov;
	int pixelOn = 0;
	Scene* scene;

	Camera(Vec3d location) : Entity(location)
	{
		this->fov = 90.0;	
	}

	Color TraceRay(Ray ray, int depth = 0)
	{
		// trace the ray
		CollisionResult result = scene->Trace(&ray);
		Color col = Color(0, 0, 0);
		if (result.hit()) {
			col = result.entity->getColor(result.location);

			float factor = clipf(50.0 / result.distance, 0.0f, 1.0f);
			col.r *= factor;
			col.g *= factor;
			col.b *= factor;

			// reflection
			// stub: add material and isReflective
			if (depth < 5) {
				Vec3d incident = (ray.direction).normalized();
				Vec3d normal = result.normal.normalized();
				Vec3d reflected = (incident - ((normal * 2) * (Vec3d::Dot(incident, normal)))).normalized();

				// we should push off from the surface a little to make sure we don't hit the same collision point again.
				// instead I set an owner and disable self reflections

				Ray reflectionRay = Ray(result.location, reflected);
				reflectionRay.owner = result.entity;

				// trace the ray
				Color reflectedColor = TraceRay(reflectionRay, depth + 1);
				float ref = 1.0;
				float ref2 = clipf(0.5 - depth / 10, 0, 1);
				col.r = (col.r * ref) + (reflectedColor.r * (ref2));
				col.g = (col.g * ref) + (reflectedColor.g * (ref2));
				col.b = (col.b * ref) + (reflectedColor.b * (ref2));
			}
		}
		return col;
	}


	/** Render this number of pixels.  Rendering can be done bit by bit.  Passing -1 causes entire frame to render. */
	int Render(int pixels = -1, bool autoReset=false)
	{
		int totalPixels = SCREEN_WIDTH * SCREEN_HEIGHT;
		float aspectRatio = float(SCREEN_WIDTH / SCREEN_HEIGHT);

		if (pixels == -1) {
			pixels = totalPixels - pixelOn;
		}

		int i;
		for (i = 0; i < pixels; i++) {
			
			pixelOn++;

			if (pixelOn >= totalPixels)
			{
				// reset.
				if (autoReset) pixelOn = 0;
				return i;
			}

			int x = pixelOn % SCREEN_WIDTH;
			int y = pixelOn / SCREEN_WIDTH;
				
			// find the rays direction
			float rx = (2 * ((x + 0.5) / SCREEN_WIDTH) - 1) * tan(fov / 2 * M_PI / 180) * aspectRatio;
			float ry = (1 - 2 * ((y + 0.5) / SCREEN_HEIGHT)) * tan(fov / 2 * M_PI / 180);
			Vec3d dir = Vec3d(rx, -ry, -1).normalized();

			dir.rotateX(rotation.x);
			dir.rotateY(rotation.y);
			dir.rotateZ(rotation.z);

			Ray ray = Ray(location, dir);
			Color col = TraceRay(ray);

			putPixel(x, y, col);
		}
		return i;
	}

};

CollisionResult Scene::Trace(Ray* ray)
{
	// we need to loop through all entities and check which one the ray hits first.
	// this can be a little slow, and it would be better to do some kind of axis-aligned
	// bounding box, or bounding sphere.
		
	CollisionResult closestCollision = CollisionResult::Empty();
	closestCollision.distance = 999999.0f;

	for (int i=0; i < entities.size(); i++)
	{

		if (entities[i] == ray->owner) {
			continue;
		}

		CollisionResult result = entities[i]->Trace(ray);
		if (result.hit() && (result.distance < closestCollision.distance)) 
		{
			//printf("%d %f %d\n", i, result.distance, entities.size());
			closestCollision = result;
		}
	}

	return closestCollision;
}


/*
--------------------------------------------------------
Display code
--------------------------------------------------------
*/

/** Place a pixel on the frame buffer. */
void putPixel(int x, int y, Color col)
{
	if ((x < 0) | (y < 0) | (x >= SCREEN_WIDTH) | (y >= SCREEN_HEIGHT)) {
		return;
	}
	buffer[y][x] = col.int24();
}

void clear(Color col)
{
	uint32_t c = col.int24();
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {			
			buffer[y][x] = c;
		}
	}
}

/** Blit buffer to screen */
void blit()
{		
	//upload to GPU texture (slow, shoud use glTextSubImage2D
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	//match projection to window resolution (could be in reshape callback)
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//clear and draw quad with texture (could be in display callback)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, tex);
	glEnable(GL_TEXTURE_2D);	
	glDisable(GL_DEPTH_TEST);

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(0, 0);
	glTexCoord2i(0, 1); glVertex2i(0, SCREEN_HEIGHT);
	glTexCoord2i(1, 1); glVertex2i(SCREEN_WIDTH, SCREEN_HEIGHT);
	glTexCoord2i(1, 0); glVertex2i(SCREEN_WIDTH, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFlush(); //don't need this with GLUT_DOUBLE and glutSwapBuffers

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();	
}

void display(void)
{
	blit();
}

void initFrameBuffer(void)
{
	// init texture	
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			int cell = ((x / 8) + (y / 8)) % 2;
			float v = 0.5 + cell * 0.5;
			buffer[y][x] = Color(v, v, v).int24();
		}
	}
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

}

/*
--------------------------------------------------------
Ray Tracing
--------------------------------------------------------
*/

Scene scene = Scene();

Camera camera = Camera(Vec3d(0, 1, 30));

void Mode3(float elapsed)
{
	camera.Render(10000);
}

void initRayTrace(void)
{
	camera.fov = 90;
	camera.scene = &scene;

	Sphere* sphere1;
	Sphere* sphere2;
	Sphere* sphere3;

	sphere1 = new Sphere(Vec3d(0, 5, 0), 5);
	sphere1->color = Color(1.0f, 0.5, 0.5);
	scene.AddEntity(sphere1);	

	sphere2 = new Sphere(Vec3d(-12, 5, +4), 5);
	sphere2->color = Color(0.5, 1.0f, 0.5);
	scene.AddEntity(sphere2);

	sphere3 = new Sphere(Vec3d(12, 3, -7), 3);
	sphere3->color = Color(0.5, 0.5, 1.0f);
	scene.AddEntity(sphere3);

	Plane* plane1 = new Plane(Vec3d(0,0,0), Vec3d(0,1,0));
	plane1->color = Color(0.5f, 0.5f, 1.0f);
	scene.AddEntity(plane1);

}

/*
--------------------------------------------------------
General
--------------------------------------------------------
*/


void initialize(void)
{
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	initFrameBuffer();
	initRayTrace();
}

float lastFrameTime = 0.0f;
int frameOn = 0;

void update(void)
{
	currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;	
	float elapsed = currentTime - lastFrameTime;


	if (elapsed < (0.5 / 1000.0)) {
		return;
	}

	Mode3(elapsed);
	glutPostRedisplay();
	lastFrameTime = currentTime;
	/*
	if (frameOn++ % 100 == 0)
	{
		float fps = 1.0 / elapsed;
		printf("Time %f, Elapsed %f, Frame rate %f\n", currentTime, elapsed, fps);
	}*/

}

int main(int argc, char **argv)
{
	printf("Hello main\n");

	Vec3d v = Vec3d(1, 0, 0);
	printf("%f %f %f\n", v.xAngle(), v.yAngle(), v.zAngle());
	v = Vec3d(0, 1, 0);
	printf("%f %f %f\n", v.xAngle(), v.yAngle(), v.zAngle());
	v = Vec3d(0, 0, 1);
	printf("%f %f %f\n", v.xAngle(), v.yAngle(), v.zAngle());

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("VSRAY");
	initialize();
	glutDisplayFunc(display);
	glutIdleFunc(update);
	glutMainLoop();
	return 0;
}