//this class exists to simplify quaternion math. it should never have any virtual and any additional member values
//since it wouldn't be logical and this class is used to define the serial communication
class Quaternion
{
public:
	Quaternion() = default;
	inline Quaternion(float qw, float qx, float qy, float qz): w(qw), x(qx), y(qy), z(qz) {};
	Quaternion(Quaternion &&) = default;
	Quaternion(const Quaternion &) = default;
	Quaternion &operator=(Quaternion &&) = default;
	Quaternion &operator=(const Quaternion &) = default;
	bool operator==(const Quaternion &) const;
	~Quaternion() = default;

	void invert();
	Quaternion operator*(const Quaternion &) const;

	//hiding value in objects is pointless in this context
	float w;
	float x;
	float y;
	float z;
};
