//---------------------------------------------------------
//!@file Vector.hpp
//!@brief Vectorを扱います
//!@author OxthLy
//!@date 2018/11/3
//!@note Vector2Type,Vector3,Vector4Type　など
//!@note 参考URL https://github.com/tonarinohito/MyGameTemplate/blob/master/gameTemple/src/Utility/Vec.hpp 
//!@note by tonarinohito
//---------------------------------------------------------
#pragma once
#include <string>


//-------------------------------------------
//!@class Vector2Type
//!@brief 2次元ベクトルを扱います
//-------------------------------------------
template<typename Type>
class Vector2Type
{
public:
	Vector2Type() :xValue(0), yValue(0) {};
	Vector2Type(const Type x, const Type y)
		: xValue(x), yValue(y)
	{};
	Vector2Type(const Vector2Type& vector2)
		: xValue(vector2.xValue), yValue(vector2.yValue)
	{};
	virtual Vector2Type() = default;
	//!@brief　オフセットした値のコピーを取得します
	[[nodiscard]] const Vector2Type offsetCopy(const Type offsetX, const Type offsetY) const noexcept
	{
		return { xValue + offsetX,yValue + offsetY };
	}
	//!@brief　オフセットした値を取得します
	[[nodiscard]] const Vector2Type& offset(const Type offsetX, const Type offsetY) const noexcept
	{
		xValue += offsetX;
		yValue += offsetY;
		return *this;
	}
	//!@brief 値を0にします
	[[noreturn]] void zero() const noexcept
	{
		xValue = 0;
		yValue = 0;
	}
	//!@brief 内積を取得します
	[[nodiscard]] const Type dot(const Vector2Type& vector2) const noexcept
	{
		return xValue * vector2.xValue + yValue * vector2.yValue;
	}
	//!@brief 内積を取得します
	[[nodiscard]] static Type dot(const Vector2Type& v1, const Vector2Type& v2) noexcept
	{
		return v1.xValue * v2.xValue + v1.yValue * v2.yValue;
	}
	//!@brief 外積を取得します
	[[nodiscard]] const Type closs(const Vector2Type& vector2) const noexcept
	{
		return xValue * vector2.yValue - vector2.xValue * yValue;
	}
	//!@brief 外積を取得します
	[[nodiscard]] static Type closs(const Vector2Type& v1, const Vector2Type& v2) noexcept
	{
		return v1.xValue * v2.yValue - v2.xValue * v1.yValue;
	}
	//!@brief 2点間の距離を取得します
	[[nodiscard]] const Type getDistance(const Vector2Type& vector2) const noexcept
	{
		const Type xDistance = xValue - vector2.xValue;
		const Type yDistance = yValue - vector2.yValue;
		return sqrtf(xDistance * xDistance + yDistance * yDistance);
	}
	//!@brief 長さを取得します
	[[nodiscard]] const Type length() const noexcept
	{
		return sqrtf(xValue * xValue + yValue * yValue);
	}
	//!@brief 自身を正規化した値を取得します
	[[nodiscard]] const Type& nomalize() const noexcept
	{
		Type len = length();
		xValue /= len;
		yValue /= len;
		return *this;
	}
	//!@brief 正規化した値のコピーを取得します
	[[nodiscard]] const Type nomalized() const noexcept
	{
		Type len = length();
		return { xValue / len,yValue / len };
	}

	[[nodiscard]] Vector2Type operator+() const noexcept
	{
		return { *this };
	}
	[[nodiscard]] Vector2Type operator-() const noexcept
	{
		return { -xValue,-yValue };
	}
	[[nodiscard]] Vector2Type operator+(const Vector2Type& vector2) const noexcept
	{
		return { xValue + vector2.xValue,yValue + vector2.yValue };
	}
	[[nodiscard]] Vector2Type operator-(const Vector2Type& vector2) const noexcept
	{
		return { xValue - vector2.xValue,yValue - vector2.yValue };
	}
	[[nodiscard]] Vector2Type operator*(const Vector2Type& vector2) const noexcept
	{
		return { xValue * vector2.xValue,yValue * vector2.yValue };
	}
	[[nodiscard]] Vector2Type operator*(const Type& scale) const noexcept
	{
		return { xValue * scale,yValue * scale };
	}
	[[nodiscard]] Vector2Type operator-(const Vector2Type& vector2) const throw(std::exception)
	{
		if (xValue == 0 || yValue == 0) { throw std::exception("0除算を行いました"); }
		return { xValue / vector2.xValue,yValue / vector2.yValue };
	}
	[[nodiscard]] Vector2Type operator-(const Type& scale) const throw(std::exception)
	{
		if (xValue == 0 || yValue == 0) { throw std::exception("0除算を行いました"); }
		return { xValue / scale,yValue / scale };
	}
	[[nodiscard]] Vector2Type& operator=(const Vector2Type& vector2) const noexcept
	{
		xValue = vector2.xValue;
		yValue = vector2.yValue;
		return *this;
	}
	[[nodiscard]] Vector2Type& operator=(const Type& scale) const noexcept
	{
		xValue += scale;
		yValue += scale;
		return *this;
	}
	[[nodiscard]] Vector2Type& operator+=(const Vector2Type& vector2) const noexcept
	{
		xValue += vector2.xValue;
		yValue += vector2.yValue;
		return *this;
	}
	[[nodiscard]] Vector2Type& operator-=(const Vector2Type& vector2) const noexcept
	{
		xValue -= vector2.xValue;
		yValue -= vector2.yValue;
		return *this;
	}
	[[nodiscard]] Vector2Type& operator*=(const Vector2Type& vector2) const noexcept
	{
		xValue *= vector2.xValue;
		yValue *= vector2.yValue;
		return *this;
	}
	[[nodiscard]] Vector2Type& operator/=(const Vector2Type& vector2) const throw(std::exception)
	{
		if (xValue == 0 || yValue == 0) { throw std::exception("0除算を行いました"); }
		xValue /= vector2.xValue;
		yValue /= vector2.yValue;
		return *this;
	}
	[[nodiscard]] const Type& operator()(const int index) const noexcept
	{
		if (index == 0) { return xValue; }
		if (index == 1) { return yValue; }
	}
	[[nodiscard]] const bool operator==(const Vector2Type& vector2) const noexcept
	{
		return xValue == vector2.xValue && yValue == vector2.yValue;
	}
	[[nodiscard]] const bool operator!=(const Vector2Type& vector2) const noexcept
	{
		return !(*this != vector2);
	}
public:
	Type xValue;
	Type yValue;
};

using Vector2Int = Vector2Type<int>;
using Vector2Float = Vector2Type<float>;
using Vector2Double = Vector2Type<double>;
//デフォルトでfloat型
using Vector2 = Vector2Float;


//-------------------------------------------
//!@class Vector3
//!@brief 3次元ベクトルを扱います
//-------------------------------------------
template<typename Type>
class Vector3Type
{
public:
	Vector3Type() :xValue(0), yValue(0), zValue(0) {};
	Vector3Type(const Type x, const Type y,const Type z)
		: xValue(x), yValue(y),zValue(z)
	{};
	Vector3Type(const Vector3& vector3)
		: xValue(vector3.xValue), yValue(vector3.yValue),zValue(vector3.zValue)
	{};
	virtual Vector3Type() = default;
	//!@brief　オフセットした値のコピーを取得します
	[[nodiscard]] const Vector3Type offsetCopy(const Type offsetX, const Type offsetY,const Type offsetZ) const noexcept
	{
		return { xValue + offsetX,yValue + offsetY ,zValue + offsetZ };
	}
	//!@brief　オフセットした値を取得します
	[[nodiscard]] const Vector3Type& offset(const Type offsetX, const Type offsetY,const Type offsetZ) const noexcept
	{
		xValue += offsetX;
		yValue += offsetY;
		zValue += offsetZ;
		return *this;
	}
	//!@brief 値を0にします
	[[noreturn]] void zero() const noexcept
	{
		xValue = 0;
		yValue = 0;
		zValue = 0;
	}
	//!@brief 内積を取得します
	[[nodiscard]] const Type dot(const Vector3Type& vector3) const noexcept
	{
		return xValue * vector3.xValue + yValue * vector3.yValue + zValue * vector3.zValue;;
	}
	//!@brief 内積を取得します
	[[nodiscard]] static Type dot(const Vector3Type& v1, const Vector3Type& v2) noexcept
	{
		return v1.xValue * v2.xValue + v1.yValue * v2.yValue + v1.zValue * v2.zValue;
	}
	//!@brief 外積を取得します
	[[nodiscard]] const Vector3 closs(const Vector3Type& vector3) const noexcept
	{
		return { yValue * vector3.zValue - zValue * vector3.yValue,
				 zValue * vector3.xValue - xValue * vector3.zValue,
				 xValue * vector3.yValue - yValue * vector3.xValue, };
	}
	//!@brief 外積を取得します
	[[nodiscard]] static Vector3Type closs(const Vector3Type& v1, const Vector3Type& v2) noexcept
	{
		return { v1.yValue * v2.zValue - v1.zValue * v2.yValue,
				 v1.zValue * v2.xValue - v1.xValue * v2.zValue,
				 v1.xValue * v2.yValue - v1.yValue * v2.xValue, };
	}
	//!@brief 2点間の距離を取得します
	[[nodiscard]] const Type getDistance(const Vector3Type& vector3) const noexcept
	{
		const Type xDistance = xValue - vector3.xValue;
		const Type yDistance = yValue - vector3.yValue;
		const Type zDistance = zValue - vector3.zValue;
		return sqrtf(xDistance * xDistance + yDistance * yDistance + zDistance * zDistance);
	}
	//!@brief 長さを取得します
	[[nodiscard]] const Type length() const noexcept
	{
		return sqrtf(xValue * xValue + yValue * yValue,zValue * zValue);
	}
	//!@brief 自身を正規化した値を取得します
	[[nodiscard]] const Type& nomalize() const noexcept
	{
		Type len = length();
		xValue /= len;
		yValue /= len;
		zValue /= len;
		return *this;
	}
	//!@brief 正規化した値のコピーを取得します
	[[nodiscard]] const Type nomalized() const noexcept
	{
		Type len = length();
		return { xValue / len,yValue / len,zValue / len };
	}

	[[nodiscard]] Vector3Type operator+() const noexcept
	{
		return { *this };
	}
	[[nodiscard]] Vector3Type operator-() const noexcept
	{
		return { -xValue,-yValue ,-zValue};
	}
	[[nodiscard]] Vector3Type operator+(const Vector3Type& vector3) const noexcept
	{
		return { xValue + vector3.xValue,yValue + vector3.yValue ,zValue + vector3.zValue};
	}
	[[nodiscard]] Vector3Type operator-(const Vector3Type& vector3) const noexcept
	{
		return { xValue - vector3.xValue,yValue - vector3.yValue ,zValue * vector3.zValue };
	}
	[[nodiscard]] Vector3Type operator*(const Vector3Type& vector3) const noexcept
	{
		return { xValue * vector3.xValue,yValue * vector3.yValue ,zValue * vector3.zValue };
	}
	[[nodiscard]] Vector3Type operator*(const Type& scale) const noexcept
	{
		return { xValue * scale,yValue * scale ,zValue * scale };
	}
	[[nodiscard]] Vector3Type operator-(const Vector3Type& vector3) const throw(std::exception)
	{
		if (xValue == 0 || yValue == 0 || zValue == 0) { throw std::exception("0除算を行いました"); }
		return { xValue / vector3.xValue,yValue / vector3.yValue,zValue / vector3.zValue };
	}
	[[nodiscard]] Vector3Type operator-(const Type& scale) const throw(std::exception)
	{
		if (xValue == 0 || yValue == 0 || zValue == 0) { throw std::exception("0除算を行いました"); }
		return { xValue / scale,yValue / scale ,zValue / scale };
	}
	[[nodiscard]] Vector3Type& operator=(const Vector3Type& vector3) const noexcept
	{
		xValue = vector3.xValue;
		yValue = vector3.yValue;
		zValue = vector3.zValue;
		return *this;
	}
	[[nodiscard]] Vector3Type& operator=(const Type& scale) const noexcept
	{
		xValue += scale;
		yValue += scale;
		zValue += scale;
		return *this;
	}
	[[nodiscard]] Vector3Type& operator+=(const Vector3Type& vector3) const noexcept
	{
		xValue += vector3.xValue;
		yValue += vector3.yValue;
		zValue += vector3.zValue;
		return *this;
	}
	[[nodiscard]] Vector3Type& operator-=(const Vector3Type& vector3) const noexcept
	{
		xValue -= vector3.xValue;
		yValue -= vector3.yValue;
		zValue -= vector3.zValue:
		return *this;
	}
	[[nodiscard]] Vector3Type& operator*=(const Vector3Type& vector3) const noexcept
	{
		xValue *= vector3.xValue;
		yValue *= vector3.yValue;
		zValue *= vector3.zValue:
		return *this;
	}
	[[nodiscard]] Vector3Type& operator/=(const Vector3Type& vector3) const throw(std::exception)
	{
		if (xValue == 0 || yValue == 0 || zValue == 0) { throw std::exception("0除算を行いました"); }
		xValue /= vector3.xValue;
		yValue /= vector3.yValue;
		zValue /= vector3.zValue:
		return *this;
	}
	[[nodiscard]] const Type& operator()(const int index) const noexcept
	{
		if (index == 0) { return xValue; }
		if (index == 1) { return yValue; }
		if (index == 2) { return zValue; }
	}
	[[nodiscard]] const bool operator==(const Vector3Type& vector3) const noexcept
	{
		return xValue == vector3.xValue && yValue == vector3.yValue && zValue == vector3.zValue;
	}
	[[nodiscard]] const bool operator!=(const Vector3Type& vector3) const noexcept
	{
		return !(*this != vector3);
	}
public:
	Type xValue;
	Type yValue;
	Type zValue;
};

using Vector3Int = Vector3Type<int>;
using Vector3Float = Vector3Type<float>;
using Vector3Double = Vector3Type<double>;
//デフォルトでfloat型
using Vector3 = Vector3Float;


//-------------------------------------------
//!@class Vector4Type
//!@brief 4次元ベクトルを扱います
//-------------------------------------------
template<typename Type>
class Vector4Type
{
public:
	Vector4Type() :xValue(0), yValue(0), zValue(0),wValue(0) {};
	Vector4Type(const Type x, const Type y, const Type z,const Type w)
		: xValue(x), yValue(y), zValue(z),wValue(w)
	{}
	Vector4Type(const Vector4Type& vector4)
		: xValue(vector4.xValue), yValue(vector4.yValue), zValue(vector4.zValue),wValue(vector4.wValue)
	{}
	Vector4Type(const Vector3& vector3, const Type w = 0)
		: xValue(vector3.xValue), yValue(vector3.yValue), zValue(vector3.zValue), wValue(w)
	{}
	virtual Vector4Type() = default;
	//!@brief　オフセットした値のコピーを取得します
	[[nodiscard]] const Vector4Type offsetCopy(const Type offsetX, const Type offsetY, const Type offsetZ,const Type offsetW) const noexcept
	{
		return { xValue + offsetX,yValue + offsetY ,zValue + offsetZ ,wValue = offsetW};
	}
	//!@brief　オフセットした値を取得します
	[[nodiscard]] const Vector4Type& offset(const Type offsetX, const Type offsetY, const Type offsetZ,const Type offsetW) const noexcept
	{
		xValue += offsetX;
		yValue += offsetY;
		zValue += offsetZ;
		wValue += offsetW;
		return *this;
	}
	//!@brief 値を0にします
	[[noreturn]] void zero() const noexcept
	{
		xValue = 0;
		yValue = 0;
		zValue = 0;
		wValue 
	}
	//!@brief 内積を取得します
	[[nodiscard]] const Type dot(const Vector3Type& vector3) const noexcept
	{
		return xValue * vector3.xValue + yValue * vector3.yValue + zValue * vector3.zValue;
	}
	//!@brief 内積を取得します
	[[nodiscard]] static Type dot(const Vector3Type& v1, const Vector3& v2) noexcept
	{
		return v1.xValue * v2.xValue + v1.yValue * v2.yValue + v1.zValue * v2.zValue;
	}
	//!@brief 外積を取得します
	[[nodiscard]] const Vector3Type closs(const Vector3Type& vector3) const noexcept
	{
		return { yValue * vector3.zValue - zValue * vector3.yValue,
				 zValue * vector3.xValue - xValue * vector3.zValue,
				 xValue * vector3.yValue - yValue * vector3.xValue, };
	}
	//!@brief 外積を取得します
	[[nodiscard]] static Vector3Type closs(const Vector3Type& v1, const Vector3& v2) noexcept
	{
		return { v1.yValue * v2.zValue - v1.zValue * v2.yValue,
				 v1.zValue * v2.xValue - v1.xValue * v2.zValue,
				 v1.xValue * v2.yValue - v1.yValue * v2.xValue, };
	}
	//!@brief 2点間の距離を取得します
	[[nodiscard]] const Type getDistance(const VVector3Typeector3& vector3) const noexcept
	{
		const Type xDistance = xValue - vector3.xValue;
		const Type yDistance = yValue - vector3.yValue;
		const Type zDistance = zValue - vector3.zValue;
		return sqrtf(xDistance * xDistance + yDistance * yDistance + zDistance * zDistance);
	}
	//!@brief 長さを取得します
	[[nodiscard]] const Type length() const noexcept
	{
		return sqrtf(xValue * xValue + yValue * yValue, zValue * zValue);
	}
	//!@brief 自身を正規化した値を取得します
	[[nodiscard]] const Type& nomalize() const noexcept
	{
		Type len = length();
		xValue /= len;
		yValue /= len;
		zValue /= len;
		return *this;
	}
	//!@brief 正規化した値のコピーを取得します
	[[nodiscard]] const Type nomalized() const noexcept
	{
		Type len = length();
		return { xValue / len,yValue / len,zValue / len };
	}

	[[nodiscard]] Vector4Type operator+() const noexcept
	{
		return { *this };
	}
	[[nodiscard]] Vector4Type operator-() const noexcept
	{
		return { -xValue,-yValue ,-zValue,-wValue };
	}
	[[nodiscard]] Vector4Type operator+(const Vector4Type& vector4) const noexcept
	{
		return { xValue + vector4.xValue,yValue + vector4.yValue ,zValue + vector4.zValue ,wValue + vector4.wValue };
	}
	[[nodiscard]] Vector4Type operator-(const Vector4Type& vector4) const noexcept
	{
		return { xValue - vector4.xValue,yValue - vector4.yValue ,zValue * vector4.zValue ,wValue + vector4.wValue };
	}
	[[nodiscard]] Vector4Type operator*(const Vector4Type& vector4) const noexcept
	{
		return { xValue * vector4.xValue,yValue * vector4.yValue ,zValue * vector4.zValue ,wValue + vector4.wValue };
	}
	[[nodiscard]] Vector4Type operator*(const Type& scale) const noexcept
	{
		return { xValue * scale,yValue * scale ,zValue * scale ,wValue * scale};
	}
	[[nodiscard]] Vector4Type operator-(const Vector4Type& vector4) const throw(std::exception)
	{
		if (xValue == 0 || yValue == 0 || zValue == 0 || wValue == 0) { throw std::exception("0除算を行いました"); }
		return { xValue / vector4.xValue,yValue / vector4.yValue,zValue / vector4.zValue ,wValue / vector4.wValue };
	}
	[[nodiscard]] Vector4Type operator-(const Type& scale) const throw(std::exception)
	{
		if (xValue == 0 || yValue == 0 || zValue == 0 || wValue == 0) { throw std::exception("0除算を行いました"); }
		return { xValue / scale,yValue / scale ,zValue / scale , wValue / scale };
	}
	[[nodiscard]] Vector4Type& operator=(const Vector4Type& vector4) const noexcept
	{
		xValue = vector4.xValue;
		yValue = vector4.yValue;
		zValue = vector4.zValue;
		wValue = vector4.wValue
		return *this;
	}
	[[nodiscard]] Vector3& operator=(const Type& scale) const noexcept
	{
		xValue += scale;
		yValue += scale;
		zValue += scale;
		wValue += scale;
		return *this;
	}
	[[nodiscard]] Vector4Type& operator+=(const Vector4Type& vector4) const noexcept
	{
		xValue += vector4.xValue;
		yValue += vector4.yValue;
		zValue += vector4.zValue;
		wValue += vector4.wValue;
		return *this;
	}
	[[nodiscard]] Vector4Type& operator-=(const Vector4Type& vector4) const noexcept
	{
		xValue -= vector4.xValue;
		yValue -= vector4.yValue;
		zValue -= vector4.zValue:
		wValue -= vector4.wValue;
		return *this;
	}
	[[nodiscard]] Vector4Type& operator*=(const Vector4Type& vector4) const noexcept
	{
		xValue *= vector4.xValue;
		yValue *= vector4.yValue;
		zValue *= vector4.zValue:
		wValue *= vector4.wValue;
		return *this;
	}
	[[nodiscard]] Vector4Type& operator/=(const Vector4Type& vector4) const throw(std::exception)
	{
		if (xValue == 0 || yValue == 0 || zValue == 0 || wValue == 0) { throw std::exception("0除算を行いました"); }
		xValue /= vector4.xValue;
		yValue /= vector4.yValue;
		zValue /= vector4.zValue:
		wValue /= vector4.wValue;
		return *this;
	}
	[[nodiscard]] const Type& operator()(const int index) const noexcept
	{
		if (index == 0) { return xValue; }
		if (index == 1) { return yValue; }
		if (index == 2) { return zValue; }
		if (index == 3) { return wValue; }
	}
	[[nodiscard]] const bool operator==(const Vector4Type& vector4) const noexcept
	{
		return xValue == vector4.xValue && yValue == vector4.yValue && zValue == vector4.zValue && wValue == vector4.wValue;
	}
	[[nodiscard]] const bool operator!=(const Vector4Type& vector4) const noexcept
	{
		return !(*this != vector4);
	}
public:
	Type xValue;
	Type yValue;
	Type zValue;
	Type wValue;
};


using Vector4Int = Vector4Type<int>;
using Vector4Float = Vector4Type<float>;
using Vector4Double = Vector4Type<double>;
//デフォルトでfloat型
using Vector4 = Vector4Float;


