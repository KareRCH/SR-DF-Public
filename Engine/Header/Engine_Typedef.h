#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

using std::unordered_multimap;
using std::unordered_map;
using std::unordered_set;
using std::pair;

namespace Engine
{
	typedef		bool						_bool;

	typedef		signed char					_byte;
	typedef		unsigned char				_ubyte;

	typedef		wchar_t						_tchar;
	
	typedef		signed short				_short;
	typedef		unsigned short				_ushort;

	typedef		signed int					_int;
	typedef		unsigned int				_uint;

	typedef		signed long					_long;
	typedef		unsigned long				_ulong;

	typedef		signed long long			_longlong;
	typedef		unsigned long long			_ulonglong;

	typedef		float						_float;
	typedef		double						_double;

	// 아키텍처에 따른 소수점 정밀도 높이기
#ifdef _WIN64
#if _WIN64
	typedef		_double						_real;
#endif // _WIN64 == 1
#else
	typedef		_float						_real;
#endif // _WIN64

	// 추가 정의 타입
	template <typename T>
	using _range = pair<T, T>;

	template <typename Key, typename Value>
	using _unmap = unordered_map<Key, Value>;

	template <typename Key, typename Value>
	using _ummap = unordered_multimap<Key, Value>;

	template <typename Key>
	using _unset = unordered_set<Key>;


	typedef		D3DXVECTOR2					_vec2;
	typedef		D3DXVECTOR3					_vec3;
	typedef		D3DXVECTOR4					_vec4;

	typedef		D3DXMATRIX					_matrix;

	/*typedef	 struct tagMyVec3 : public D3DXVECTOR3
	{
		float	Get_Dot(const d3dxvector3& vTemp);
	
	};*/
}

#endif // Engine_Typedef_h__
