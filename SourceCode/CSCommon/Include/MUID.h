#pragma once

#include <climits>
#include <map>
#include <vector>
#include "GlobalTypes.h"

/// MAIET Unique ID
struct MUID {
	u32	High{};
	u32	Low{};

	MUID() = default;
	MUID(u32 h, u32 l) {
		High = h;
		Low = l;
	}
	explicit MUID(u64 x) {
		High = u32(x >> 32);
		Low = u32(x & 0xFFFF'FFFF);
	}

	void SetZero() {
		High = Low = 0;
	}
	void SetInvalid() {
		SetZero();
	}

	MUID Increase(u32 nSize = 1) {
		if (Low + nSize > UINT_MAX) {
			_ASSERT(High < UINT_MAX);
			Low = nSize - (UINT_MAX - Low);
			High++;
		}
		else {
			Low += nSize;
		}
		return *this;
	}

	bool IsInvalid() const {
		if (High == Low && Low == 0) return true;
		return false;
	}
	bool IsValid() const {
		if (High == Low && Low == 0) return false;
		return true;
	}

	inline friend bool operator > (const MUID& a, const MUID& b) {
		if (a.High > b.High) return true;
		if (a.High == b.High) {
			if (a.Low > b.Low) return true;
		}
		return false;
	}
	inline friend bool operator >= (const MUID& a, const MUID& b) {
		if (a.High > b.High) return true;
		if (a.High == b.High) {
			if (a.Low >= b.Low) return true;
		}
		return false;
	}
	inline friend bool operator < (const MUID& a, const MUID& b) {
		if (a.High < b.High) return true;
		if (a.High == b.High) {
			if (a.Low < b.Low) return true;
		}
		return false;
	}
	inline friend bool operator <= (const MUID& a, const MUID& b) {
		if (a.High < b.High) return true;
		if (a.High == b.High) {
			if (a.Low <= b.Low) return true;
		}
		return false;
	}

	inline MUID& operator=(int v) {
		High = 0;
		Low = v;
		return *this;
	}
	inline friend bool operator==(const MUID& a, const MUID& b) {
		if (a.High == b.High) {
			if (a.Low == b.Low) return true;
		}
		return false;
	}
	inline friend bool operator!=(const MUID& a, const MUID& b) {
		if (a.Low != b.Low) return true;
		if (a.High != b.High) return true;
		return false;
	}
	inline friend MUID& operator++(MUID& a) {
		a.Increase();
		return a;
	}
	u64 AsU64() const {
		return (u64(High) << 32) | Low;
	}

	static MUID Invalid();
};

struct MUIDRANGE {
	MUID Start;
	MUID End;

	bool IsEmpty() const {
		return Start == End;
	}
	void Empty() {
		SetZero();
	}
	void SetZero() {
		Start.SetZero();
		End.SetZero();
	}
};

#define MAKEMUID(_high, _low)	MUID(_high, _low)

class MUIDRefMap final : protected std::map<MUID, void*> {
	MUID m_CurrentMUID;
public:
	MUIDRefMap();
	~MUIDRefMap();

	MUID Generate(void* pRef);

	void* GetRef(const MUID& uid);

	void* Remove(const MUID& uid);

	MUIDRANGE Reserve(int nSize);

	MUIDRANGE GetReservedCount();
};

class MUIDRefCache : public std::map<MUID, void*> {
public:
	MUIDRefCache(void);
	virtual ~MUIDRefCache(void);
	void Insert(const MUID& uid, void* pRef);
	void* GetRef(const MUID& uid);
	void* Remove(const MUID& uid);
};

namespace std
{
	template <>
	class hash<MUID> : public hash<uint64_t>
	{
	public:
		size_t operator()(const MUID& UID) const
		{
			static_assert(sizeof(u64) == sizeof(MUID), "MUID is not 64 bits wide");

			u64 uid64;
			memcpy(&uid64, &UID, sizeof(UID));
			return hash<uint64_t>::operator()(uid64);
		}
	};
}
