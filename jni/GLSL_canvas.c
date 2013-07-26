#include <jni.h>

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>

#include <android/log.h>
#include <android_native_app_glue.h>


#include <stdio.h>
#include <math.h>

#define SEC_IN_NS 1000000000
#define SEC_IN_US 1000000

#define LOG_TAG ("GLSL_canvas")
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
struct engine {




	struct android_app* app;
	EGLDisplay display;
	EGLSurface surface;

	int animating;

};


//#3) When you render the fullscreen quad, you definately don't want to use the same projection matrix.
//The easiest is to just use no matrix at all (define input coordinates in normalized device coordinates).
//		Just draw your quad with vertices (+/-1, +/-1, 0) and that will align with the fullscreen
//		without any transform required.


typedef struct {
  EGLNativeWindowType  nativeWin;
  EGLDisplay  display;
  EGLContext  context;
  EGLSurface  surface;
  EGLint      majorVersion;
  EGLint      minorVersion;
  int         width;
  int         height;
} ScreenSettings;

/*const char vShaderSrc[] =
  "attribute vec3 aPosition;\n"
  "attribute vec2 aTex;\n"
  "varying   vec2 vTex;\n"
  "uniform   mat4 uVpMatrix;\n"
  "uniform   mat4 uModelMatrix;\n"
  "\n"
  "void main(void) {\n"
  "  vTex = aTex;\n"
  "  gl_Position = uVpMatrix * uModelMatrix * vec4(aPosition, 1.0);\n"
  "}\n";

const char fShaderSrc[] =
  "precision mediump float;\n"\
  "varying   vec2  vTex;\n"
  "void main()        \n"
  "{                  \n"
  "  gl_FragColor = vec4(vTex.y, vTex.x, 0.5, 1.0);\n"
  "}                  \n";*/


//const char v_shader_k[] =
////		  "attribute vec2 position; \n"
//		"attribute vec3 position;\n"
//		"attribute vec2 tex;\n"
//		"void main() { \n"
//		"		gl_Position = vec4(position, 1.0); \n"
//		"}\n";

const char v_shader_k[] =
		  "attribute vec2 position; \n"
		"attribute vec2 tex;\n"
		"void main() { \n"
		"		gl_Position = vec4(position, 0.0, 1.0); \n"
		"}\n";


//const char f_shader_k[] =
////	"#ifdef GL_ES\n"
//	"precision mediump float;\n"
////	"#endif\n"
//	"uniform float time;\n"
//	"uniform vec2 resolution;\n"
//	"uniform vec2 pos_move;\n"
//
//	"void main( void ) {\n"
//	"	float sum = .01;\n"
//	"	float size = resolution.x / 2.0;\n"
//	"	float g = .9;\n"
//	"	int num = 30;\n"
//	"	float t = time/3.0;\n"
//
//	"	for (int i = 0; i < 2; ++i) {\n"
//	"		vec2 position = resolution / 2.0;\n"
//	"		position.x += sin(t / 3.0 + 1.0 * float(i)) * resolution.x * 0.25;\n"
////	"		position.y += tan(t / 556.0 + (2.0 + sin(t) * 0.01) * float(i)) * resolution.y * 0.25;\n"
//	"		float dist = length(gl_FragCoord.xy - position);\n"
//	"		sum += size / pow(dist, g);\n"
//	"	}\n"
//
//
//
//
//	"	float val = sum / float(num);\n"
//	"	vec4 color = vec4(val*0.8, val*0.0, val*0.5, 1);\n"
//	"	gl_FragColor = vec4(color);\n"
//	"}\n";

const char f_shader_k[] =
	"#ifdef GL_ES\n"
	"precision mediump float;\n"
	"#endif\n"

	"uniform vec2 mouse;\n"
	"uniform vec2 resolution;\n"
	"uniform float time;\n"

	"void main(){\n"
	"	vec2 light_pos = resolution*mouse;\n"
	"	float intensity = 0.028;\n"

	"	float dist = distance(gl_FragCoord.xy, light_pos);\n"

	"	vec3 light_color = vec3(0, 1.0, 0.8);\n"
	"	float alpha = 0.8 / (dist*intensity);\n"
	"	vec4 final_color = vec4(light_color, 1.0)*vec4(alpha, alpha, alpha, 1.0);\n"
	"	gl_FragColor = final_color;\n"
	"}\n";


const char f_shader_k2[] =
	"#ifdef GL_ES\n"
	"precision mediump float;\n"
	"#endif\n"

	"uniform vec2 mouse;\n"
	"uniform vec2 resolution;\n"
	"uniform float time;\n"

	"void main(){\n"
	"	vec2 light_pos = resolution*mouse;\n"
	"	float intensity = 0.048;\n"

	"	float dist = distance(gl_FragCoord.xy, light_pos);\n"

	"	vec3 light_color = vec3(1.0, 0.0, 0.0);\n"
	"	float alpha = 0.8 / (dist*intensity);\n"
	"	vec4 final_color = vec4(light_color, 1.0)*vec4(alpha, alpha, alpha, 0.59 );\n"

	"	gl_FragColor = final_color;\n"
//		"	gl_FragColor = vec4(1.0, 0.0, 0.0, 0.5);\n"
	"}\n";


//const char f_shader_k[] =
//		"precision	mediump float;		\n"
//		"varying		vec2  	tex;			\n"
//  "void main() {\n"
////  "  vtex = tex;\n"
////  "  gl_Position = uVpMatrix * uModelMatrix * vec4(aPosition, 1.0);\n"
//		"  		gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);	\n"
//  "}\n";

//typedef struct {
//    float m[16];
//} Mat4;
//
//typedef struct {
//  GLint   aPosition;
//  GLint   aTex;
//  GLint   uVpMatrix;
//  GLint   uModelMatrix;
//  Mat4    VpMatrix;
//} ShaderParams;

typedef struct {
	GLint		position;
	GLint 	tex;
	GLint		time;
	GLint		resolution;
	GLint 	mouse;
} shader_params_k;


//typedef struct {
//    GLfloat x, y, z;
//    GLfloat u, v;
//} VertexType;


typedef struct {
	GLfloat x, y, z;
	GLfloat u, v;
} vertex;


//VertexType vObj[] = {
//  {-0.5f, 		-0.5f, 	0.0f, 		0.0f, 		1.0f},
//  {0.5f, 		-0.5f, 	0.0f, 		1.0f, 		1.0f},
//  {0.0f, 		0.5f, 		0.0f, 		0.5f,		0.0f},
//};

vertex bg_quad[] = {
	{-1.0f, -1.0f, 0.0f, 0.0, 0.0},
	{1.0f, -1.0f, 0.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
	{-1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
};



//Mat4 viewMat;
//Mat4 rotMat;
//Mat4 rotMat2;
//Mat4 modelMat;
//Mat4 modelMat2;


unsigned short iObj[] = {
  0, 1, 3, 2
};

//ShaderParams    g_sp;
ScreenSettings  g_sc;


shader_params_k sp_k;

GLuint g_vbo;
GLuint g_ibo;
GLuint g_program;

GLuint g_program_2;




GLuint LoadShader(GLenum type, const char *shaderSource)
{
  GLuint shader;
  GLint compiled;

  shader = glCreateShader(type);
  if (shader == 0) return 0;

  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);

  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) { // compile error
    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen > 0) {
      void* infoLog = malloc(sizeof(char) * infoLen);
      glGetShaderInfoLog(shader, infoLen, NULL, (char*)infoLog);
      LOGE("Error compiling shader:\n%s\n", infoLog);
//      LOGE("Error compiling shader");
      free(infoLog);
    }
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

int InitShaders(GLuint *program, char const *vShSrc, char const *fShSrc)
{
  GLuint vShader;
  GLuint fShader;
  GLint  linked;
  GLuint prog;

  vShader = LoadShader(GL_VERTEX_SHADER, vShSrc);
  fShader = LoadShader(GL_FRAGMENT_SHADER, fShSrc);

  prog = glCreateProgram();
  if (prog == 0) return 0;

  glAttachShader(prog, vShader);
  glAttachShader(prog, fShader);
  glLinkProgram(prog);
  glGetProgramiv (prog, GL_LINK_STATUS, &linked);
  if (!linked) { // error
    GLint infoLen = 0;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen > 0) {
      void* infoLog = malloc(sizeof(char) * infoLen);
      glGetProgramInfoLog(prog, infoLen, NULL, (char*)infoLog);
      LOGE("Error linking program:\n%s\n", infoLog);
//      LOGE("Error linking program");
      free ( infoLog );
    }
    glDeleteProgram (prog);
    return GL_FALSE;
  }
  glDeleteShader(vShader);
  glDeleteShader(fShader);

  *program = prog;
  return GL_TRUE;
}

void createBuffer()
{
  glGenBuffers(1, &g_vbo);
  // vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(bg_quad), bg_quad, GL_STATIC_DRAW);

  // index buffer
  glGenBuffers(1, &g_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iObj), iObj, GL_STATIC_DRAW);
}

struct timezone tzp;
struct timeval get_time;
void get_time_long(unsigned long* t) {


	gettimeofday(&get_time, &tzp);

	*t = (get_time.tv_sec * SEC_IN_US) + get_time.tv_usec;

}

float get_time_s() {

	gettimeofday(&get_time, &tzp	);

	return (float)get_time.tv_sec;
}


float t = 0.0F;
void Draw() {

//	LOGD("Draw", "Draw() called");

	glUseProgram(g_program);

//  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
//  glEnableVertexAttribArray(sp_k.position);
//  glEnableVertexAttribArray(sp_k.tex);
	glVertexAttribPointer(sp_k.position, 3, GL_FLOAT, GL_FALSE, 20, (void*) 0);
	glVertexAttribPointer(sp_k.tex, 2, GL_FLOAT, GL_FALSE, 20, (void*) 12);
	glEnableVertexAttribArray(0);

	t += 0.005F;
	if (t >= 1.4F)
		t = -0.40F;

	glUniform2f(sp_k.mouse, t, 0.50F);
	glUniform2f(sp_k.resolution, (float) g_sc.width, (float) g_sc.height);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);

	glUseProgram(g_program_2);
	glUniform2f(sp_k.mouse, t * 0.25, 0.750F);
	glUniform2f(sp_k.resolution, (float) g_sc.width, (float) g_sc.height);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);

}

/*void makeUnit(Mat4 *m)
{
  memset(m, 0, sizeof(Mat4));
  m->m[0] = m->m[5] = m->m[10]= m->m[15] = 1.0f;
}

void makeProjectionMatrix(Mat4 *m, float n, float f, float hfov, float r)
{
  float w = 1.0f / tan(hfov * 0.5f * M_PI / 180);
  float h = w * r;
  float q = 1.0f / (f - n);

  m->m[0] = w;
  m->m[5] = h;
  m->m[10]= -(f + n) * q;
  m->m[11]= -1.0f;
  m->m[14]= -2.0f * f * n * q;
  m->m[1] = m->m[2] = m->m[3] = m->m[4]  = m->m[6]  = m->m[7]
          = m->m[8] = m->m[9] = m->m[12] = m->m[13] = m->m[15] = 0.0f;
}*/

/*void setPosition(Mat4 *m, float x, float y, float z)
{
  m->m[12] = x;
  m->m[13] = y;
  m->m[14] = z;
}

void setRotationX(Mat4 *m, float degree)
{
  float rad = ((float)degree * M_PI / 180.0);
  m->m[ 5] = cos(rad);
  m->m[ 9] = - sin(rad);
  m->m[ 6] = sin(rad);
  m->m[10] = cos(rad);
}

void mulMatrix(Mat4 *r, Mat4 *a, Mat4 *b)
{
  float a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15;
  float b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15;

  a0 =a->m[ 0]; a1 =a->m[ 1]; a2 =a->m[ 2]; a3 =a->m[ 3];
  a4 =a->m[ 4]; a5 =a->m[ 5]; a6 =a->m[ 6]; a7 =a->m[ 7];
  a8 =a->m[ 8]; a9 =a->m[ 9]; a10=a->m[10]; a11=a->m[11];
  a12=a->m[12]; a13=a->m[13]; a14=a->m[14]; a15=a->m[15];
  b0 =b->m[ 0]; b1 =b->m[ 1]; b2 =b->m[ 2]; b3 =b->m[ 3];
  b4 =b->m[ 4]; b5 =b->m[ 5]; b6 =b->m[ 6]; b7 =b->m[ 7];
  b8 =b->m[ 8]; b9 =b->m[ 9]; b10=b->m[10]; b11=b->m[11];
  b12=b->m[12]; b13=b->m[13]; b14=b->m[14]; b15=b->m[15];

  r->m[ 0] = a0 * b0 + a4 * b1 +  a8 * b2 + a12 * b3;
  r->m[ 1] = a1 * b0 + a5 * b1 +  a9 * b2 + a13 * b3;
  r->m[ 2] = a2 * b0 + a6 * b1 + a10 * b2 + a14 * b3;
  r->m[ 3] = a3 * b0 + a7 * b1 + a11 * b2 + a15 * b3;
  r->m[ 4] = a0 * b4 + a4 * b5 +  a8 * b6 + a12 * b7;
  r->m[ 5] = a1 * b4 + a5 * b5 +  a9 * b6 + a13 * b7;
  r->m[ 6] = a2 * b4 + a6 * b5 + a10 * b6 + a14 * b7;
  r->m[ 7] = a3 * b4 + a7 * b5 + a11 * b6 + a15 * b7;
  r->m[ 8] = a0 * b8 + a4 * b9 +  a8 * b10+ a12 * b11;
  r->m[ 9] = a1 * b8 + a5 * b9 +  a9 * b10+ a13 * b11;
  r->m[10] = a2 * b8 + a6 * b9 + a10 * b10+ a14 * b11;
  r->m[11] = a3 * b8 + a7 * b9 + a11 * b10+ a15 * b11;
  r->m[12] = a0 * b12+ a4 * b13+  a8 * b14+ a12 * b15;
  r->m[13] = a1 * b12+ a5 * b13+  a9 * b14+ a13 * b15;
  r->m[14] = a2 * b12+ a6 * b13+ a10 * b14+ a14 * b15;
  r->m[15] = a3 * b12+ a7 * b13+ a11 * b14+ a15 * b15;
}*/


EGLBoolean pi_SurfaceCreate(ANativeWindow* nw) {

	LOGD("pi_SurfaceCreate", "pi_SurfaceCreate() called");
// ScreenSettings *sc = &g_sc;

    memset(&g_sc, 0, sizeof(ScreenSettings));
//    LOGD("pi_SurfaceCreate", "memset");

	EGLint attrib[] = {
		EGL_RED_SIZE,       8,
		EGL_GREEN_SIZE,     8,
		EGL_BLUE_SIZE,      8,
		EGL_ALPHA_SIZE,     8,
		EGL_DEPTH_SIZE,     24,
		EGL_RENDERABLE_TYPE,
		EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};
	EGLint context[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
	EGLint numConfigs;
	EGLConfig config;

	g_sc.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (g_sc.display == EGL_NO_DISPLAY)
		return EGL_FALSE;

	if (!eglInitialize(g_sc.display, &g_sc.majorVersion, &g_sc.minorVersion))
		return EGL_FALSE;

	if (!eglChooseConfig(g_sc.display, attrib, &config, 1, &numConfigs))
		return EGL_FALSE;

	EGLint format;
	eglGetConfigAttrib(g_sc.display, config, EGL_NATIVE_VISUAL_ID, &format);
	ANativeWindow_setBuffersGeometry(nw, 0, 0, format);

	g_sc.surface = eglCreateWindowSurface(g_sc.display, config, nw, NULL);
	if (g_sc.surface == EGL_NO_SURFACE)
		return EGL_FALSE;

	g_sc.context = eglCreateContext(g_sc.display, config, EGL_NO_CONTEXT, context);
	if (g_sc.context == EGL_NO_CONTEXT)
		return EGL_FALSE;

	if (!eglMakeCurrent(g_sc.display, g_sc.surface, g_sc.surface, g_sc.context))
		return EGL_FALSE;

	EGLint w, h;
	eglQuerySurface(g_sc.display, g_sc.surface, EGL_WIDTH, &w);
	eglQuerySurface(g_sc.display, g_sc.surface, EGL_HEIGHT, &h);

	g_sc.width = w;
	g_sc.height = h;

  	LOGD("pi_SurfaceCreate", "g_sc.width: %d", g_sc.width);
  	LOGD("pi_SurfaceCreate", "g_sc.height: %d", g_sc.height);

  	return EGL_TRUE;
}












void init_pi(struct engine* e) {

	unsigned int frames = 0;
	int res;
//	Mat4 viewMat;
//	Mat4 rotMat;
//	Mat4 modelMat;
	float aspect;

//	res = SurfaceCreate(&g_sc);
	res = pi_SurfaceCreate(e->app->window);
	if (!res)
		return;


	res = InitShaders(&g_program, v_shader_k, f_shader_k);
	if (!res)
		return;



	res = InitShaders(&g_program_2, v_shader_k, f_shader_k2);
	if (!res)
		return;


	createBuffer();
//
//	glUseProgram(g_program);
//
//
	sp_k.position = glGetAttribLocation(g_program, "position");
	sp_k.tex = glGetAttribLocation(g_program, "tex");
	sp_k.resolution = glGetUniformLocation(g_program, "resolution");
	sp_k.time = glGetUniformLocation(g_program, "time");
	sp_k.mouse = glGetUniformLocation(g_program, "mouse");

		glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


//
//
//
//
//
////	aspect = (float) g_sc.width / (float) g_sc.height;
////	makeProjectionMatrix(&g_sp.VpMatrix, 1, 1000, 53, aspect);
////	makeUnit(&viewMat);
////	setPosition(&viewMat, 0, 0, -2);
////	mulMatrix(&g_sp.VpMatrix, &g_sp.VpMatrix, &viewMat);
////	glUniformMatrix4fv(g_sp.uVpMatrix, 1, GL_FALSE, g_sp.VpMatrix.m);
////
////	makeUnit(&modelMat);
////	glUniformMatrix4fv(g_sp.uModelMatrix, 1, GL_FALSE, modelMat.m);
////
////	makeUnit(&rotMat);
////	setRotationX(&rotMat, 0.5); /* 30 degree/sec */
//
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//
//	glEnable(GL_DEPTH_TEST);
//	glClearColor(0.0f, 0.2f, 0.4f, 1.0f);



}



/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* e = (struct engine*)app->userData;

    switch (cmd) {
        case APP_CMD_SAVE_STATE:
        	LOGD("engine_handle_cmd", "APP_CMD_SAVE_STATE");
            break;

        case APP_CMD_INIT_WINDOW:
        	LOGD("engine_handle_cmd", "APP_CMD_INIT_WINDOW");
				init_pi(e);
				e->animating = 1;
            break;
        case APP_CMD_START:
        	LOGD("engine_handle_cmd", "APP_CMD_START");

//			ANativeActivity_setWindowFlags(app->activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0);

        	break;
        case APP_CMD_TERM_WINDOW:
        	LOGD("engine_handle_cmd", "APP_CMD_TERM_WINDOW");
            break;
        case APP_CMD_GAINED_FOCUS:
        	LOGD("engine_handle_cmd", "APP_CMD_GAINED_FOCUS");
            break;
        case APP_CMD_LOST_FOCUS:
        	LOGD("engine_handle_cmd", "APP_CMD_LOST_FOCUS");
        	break;
        case APP_CMD_STOP:
        	LOGD("engine_handle_cmd", "APP_CMD_STOP");
        	break;
        case APP_CMD_DESTROY:
        	LOGD("engine_handle_cmd", "APP_CMD_DESTROY");
        	break;
    }
}

void android_main(struct android_app* state) {
	app_dummy();

	struct engine e;
	state->userData = &e;

	e.app = state;

    // Make sure glue isn't stripped.
    app_dummy();
    memset(&e, 0, sizeof(struct engine));
    state->userData = &e;
    state->onAppCmd = engine_handle_cmd;
//    state->onInputEvent = engine_handle_input;
    e.app = state;
	LOGD("call_order", "android_main e.app = state");

	while (1) {


		int ident, events;
		struct android_poll_source* source;
		while ((ident=ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0) {
			if (source != NULL) {
				source->process(state, source);
			}
			if (state->destroyRequested != 0) {
				return;
			}
		}


		if(e.animating) {

			glViewport(0, 0, g_sc.width, g_sc.height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Draw();
//			/* X Rotation */
//			mulMatrix(&modelMat, &modelMat, &rotMat);
//			glUniformMatrix4fv(g_sp.uModelMatrix, 1, GL_FALSE, modelMat.m);
//
//			glUseProgram(g_program);
//			glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
//			glEnableVertexAttribArray(g_sp.aPosition);
//			glEnableVertexAttribArray(g_sp.aTex);
//			glVertexAttribPointer(g_sp.aPosition, 3, GL_FLOAT, GL_FALSE, 20, (void*)0);
//			glVertexAttribPointer(g_sp.aTex, 2, GL_FLOAT, GL_FALSE, 20, (void*)12);
//			glEnableVertexAttribArray(0);
////			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
//
//			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
			eglSwapBuffers(g_sc.display, g_sc.surface);

		}


	}
}
