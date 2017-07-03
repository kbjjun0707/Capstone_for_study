#ifndef POINT_H
#define POINT_H

namespace iMiner
{
	template<class T>
	class Vector3 {
	public:
		T x, y, z;
		Vector3(T xx = (T)0, T yy = (T)0, T zz = (T)0) { x = xx; y = yy; z = zz; }
		Vector3(const T t[3]) { x = t[0]; y = t[1]; z = t[2]; }
		Vector3(const Vector3<T> &t) { *this = t; }
		void set(T xx, T yy, T zz) { x = xx; y = yy; z = zz; }
		void set(const T t[3]) { x = t[0]; y = t[1]; z = t[2]; }
		void set(const Vector3<T> &t) { x = t.x; y = t.y; z = t.z; }
		Vector3<T> &flip() { x = -x; y = -y; z = -z; return *this; }
		template<class tt>
		void setsub(Vector3<tt> a, Vector3<tt> b) { x = (T)a.x - (T)b.x; y = (T)a.y - (T)b.y; (T)a.z - (T)b.z; }
		template<class tt>
		void setsum(Vector3<tt> a, Vector3<tt> b) { x = (T)a.x + (T)b.x; y = (T)a.y + (T)b.y; (T)a.z + (T)b.z; }
		Vector3<T> &cross(Vector3<T> a) {
			Vetor3<T> c(y*a.z - z*a.y, z*a.x - x*a.z, x*a.y - y*a.x);
			return c;
		}
		T dot(Vector3<T> a) { return x*a.x + y*a.y + z*a.z; }
		void normalize() {
			double sizeSq = x*x + y*y + z*z;
			if (sizeSq < 1e-10) {
				cerr << "\nnormalize sees vector (0, 0, 0)!";
				return;
			}
			T scaleFactor = 1.0 / (T)sqrt(sizeSq);
			x *= scaleFactor; y *= scaleFactor; z *= scaleFactor;
		}
		T *getvec() {
			T res[3] = { x,y,z };
			return res;
		}
		Vector3& operator=(Vector3<T> &b) { this->x = b.x; this->y = b.y; this->z = b.z; return *this; }
	};

	template<class T>
	class Point3 {
	public:
		T x, y, z;
		Point3(T xx = (T)0, T yy = (T)0, T zz = (T)0) { x = xx; y = yy; z = zz; }
		Point3(const T t[3]) { x = t[0]; y = t[1]; z = t[2]; }
		Point3(const Point3<T> &t) { x = t.x; y = t.y; z = t.z; }
		void set(T xx, T yy, T zz) { x = xx; y = yy; z = zz; }
		void set(const T t[3]) { x = t[0]; y = t[1]; z = t[2]; }
		void set(const Point3<T> &t) { x = t.x; y = t.y; z = t.z; }
		T *getvec() {
			T res[3] = { x,y,z };
			return res;
		}
		Point3& operator=(Point3<T> &b) { this->x = b.x; this->y = b.y; this->z = b.z; return *this; }
		Point3& operator=(T b[]) { this->x = b[0]; this->y = b[1]; this->z = b[2]; return *this; }
		Point3& operator+(Point3<T> &b) { return Point3<T>(this->x + b.x, this->y + b.y, this->z + b.z); }
		Point3& operator+=(Point3<T> &b) { this->x += b.x; this->y += b.y; this->z += b.z; return *this; }
	};

	template<class T>
	class Point2 {
	public:
		T x, y;
		Point2(T xx = (T)0, T yy = (T)0) { x = xx; y = yy; }
		Point2(const T t[2]) { x = t[0]; y = t[1]; }
		Point2(const Point2<T> &t) { x = t.x; y = t.y; }
		void set(T xx, T yy) { x = xx; y = yy; }
		void set(const T t[2]) { x = t[0]; y = t[1]; }
		void set(const Point2<T> &t) { x = t.x; y = t.y; }
		T *getvec() {
			T res[2] = { x,y };
			return res;
		}
		Point2& operator=(Point2<T> &b) { this->x = b.x; this->y = b.y; return *this; }
		Point2& operator=(T b[]) { this->x = b[0]; this->y = b[1]; return *this; }
		Point2& operator+(Point2<T> &b) { return { this->x + b.x, this->y + b.y }; }
		Point2& operator+=(Point2<T> &b) { this->x += b.x; this->y += b.y; return *this; }
	};
}


#endif // !POINT_H