#ifndef LIGHT_H
#define LIGHT_H

class Light {
	int light_type;
public:
	GLfloat Light_Pos[4];
	GLfloat Light_A[4];
	GLfloat Light_D[4];
	GLfloat Light_S[4];
	Light(int type) {
		light_type = type;
		Light_Pos[0] = 0.0f; Light_Pos[1] = 0.0f; Light_Pos[2] = -1000.0f; Light_Pos[3] = 1.0f;

		Light_A[0] = Light_A[1] = Light_A[2] = 0.9f;
		Light_D[0] = Light_D[1] = Light_D[2] = 0.9f;
		Light_S[0] = Light_S[1] = Light_S[2] = 0.678f;
		Light_A[3] = Light_D[3] = Light_S[3] = 1.0f;
	}
	void run() {
		glEnable(GL_LIGHTING);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_A);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

		glLightfv(light_type, GL_AMBIENT, Light_A);
		glLightfv(light_type, GL_DIFFUSE, Light_D);
		glLightfv(light_type, GL_SPECULAR, Light_S);

		glLightfv(light_type, GL_POSITION, Light_Pos);

		//glLightf(light_type, GL_CONSTANT_ATTENUATION, 0.0008);
		//glLightf(light_type, GL_LINEAR_ATTENUATION, 0.00016);
		//glLightf(light_type, GL_QUADRATIC_ATTENUATION, 0.00008);

		glEnable(light_type);
	}

};

#endif // !LIGHT_H