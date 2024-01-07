#ifndef POOLS_CLASSES_H
#define POOLS_CLASSES_H
#include <wtypes.h>
#include <cstdint>
#include <entities/CEntity.hpp>


template<typename T>
class pool_iterator
{
public:
	T* pool = nullptr;
	uint32_t index = 0;

	explicit pool_iterator(T* pool, int32_t index = 0)
	{
		this->pool = pool;
		this->index = index;
	}

	pool_iterator& operator++()
	{
		for (index++; index < pool->m_size; index++)
		{
			if (pool->is_valid(index))
			{
				return *this;
			}
		}

		index = pool->m_size;
		return *this;
	}

	rage::CEntity* operator*()
	{
		auto addr = pool->get_address(index);
		return (rage::CEntity*)addr;
	}

	bool operator!=(const pool_iterator& other) const
	{
		return this->index != other.index;
	}
};

// Common functions for VehiclePool and GenericPool
template<typename T>
class PoolUtils
{
public:
	//inline auto to_array()
	//{
	//	std::vector<Entity> arr;
	//	for (auto entity : *static_cast<T*>(this))
	//	{
	//		if (entity)
	//			arr.push_back(big::g_pointers->m_gta.m_ptr_to_handle(entity));
	//	}

	//	return arr;
	//}

	auto begin()
	{
		return ++pool_iterator<T>(static_cast<T*>(this), -1);
	}

	auto end()
	{
		return ++pool_iterator<T>(static_cast<T*>(this), static_cast<T*>(this)->m_size);
	}
};

class VehiclePool : public PoolUtils<VehiclePool>
{
public:
	UINT64* m_pool_address;
	UINT32 m_size;
	char _Padding2[36];
	UINT32* m_bit_array;
	char _Padding3[40];
	UINT32 m_item_count;

	inline bool is_valid(UINT32 i)
	{
		return (m_bit_array[i >> 5] >> (i & 0x1F)) & 1;
	}

	inline UINT64 get_address(UINT32 i)
	{
		return m_pool_address[i];
	}
};

class GenericPool : public PoolUtils<GenericPool>
{
public:
	UINT64 m_pool_address; // 0x0
	BYTE* m_bit_array;     // 0x8
	UINT32 m_size;         // 0x10
	UINT32 m_item_size;    // 0x14
	UINT32 m_pad[2];       // 0x18
	UINT32 m_item_count;   // 0x20

	inline bool is_valid(UINT32 i)
	{
		return mask(i) != 0;
	}

	inline UINT64 get_address(UINT32 i)
	{
		return mask(i) & (m_pool_address + (uint64_t)i * m_item_size);
	}

	inline int get_item_count()
	{
		return (4 * m_item_count) >> 2;
	}

private:
	inline long long mask(UINT32 i)
	{
		long long num1 = m_bit_array[i] & 0x80;
		return ~((num1 | -num1) >> 63);
	}
};
static_assert(offsetof(GenericPool, GenericPool::m_item_count) == 0x20);



class CPickupHandle
{
public:
	class CPickup* m_pickup; //0x0000
	int32_t m_handle; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010
static_assert(sizeof(CPickupHandle) == 0x10, "CPickupHandle is not properly sized");

class CObjectHandle
{
public:
	class CObject* m_object; //0x0000
	int32_t m_handle; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010
static_assert(sizeof(CObjectHandle) == 0x10, "CObjectHandle is not properly sized");

class CObjectList
{
public:
	class CObjectHandle m_objects[2300]; //0x0000
}; //Size: 0x8FC0

class CPickupList
{
public:
	class CPickupHandle m_pickups[73]; //0x0000
};

class CObjectInterface
{
public:
	char pad_0000[344]; //0x0000
	class CObjectList* m_list; //0x0158
	int32_t m_max; //0x0160
	char pad_0164[4]; //0x0164
	int32_t m_cur; //0x0168
}; //Size: 0x016C

class CPedHandle
{
public:
	class CPed* m_ped; //0x0000
	int32_t m_handle; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010
static_assert(sizeof(CPedHandle) == 0x10, "CPedHandle is not properly sized");

class CPedList
{
public:
	class CPedHandle m_peds[256]; //0x0000
}; //Size: 0x1000

class CPedInterface
{
public:
	char pad_0000[256]; //0x0000
	class CPedList* m_list; //0x0100
	int32_t m_max; //0x0108
	char pad_010C[4]; //0x010C
	int32_t m_cur; //0x0110
}; //Size: 0x0114

class CPickupInterface
{
public:
	char pad_0000[256]; //0x0000
	class CPickupList* m_list; //0x0100
	int32_t m_max; //0x0108
	char pad_010C[4]; //0x010C
	int32_t m_cur; //0x0110
}; //Size: 0x0114

class CVehicleHandle
{
public:
	class CVehicle* m_vehicle; //0x0000
	int32_t m_handle; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010
static_assert(sizeof(CVehicleHandle) == 0x10, "CVehicleHandle is not properly sized");

class CVehicleList
{
public:
	class CVehicleHandle m_vehicles[300]; //0x0000
}; //Size: 0x12C0

class CVehicleInterface
{
public:
	char pad_0000[384]; //0x0000
	class CVehicleList* m_list; //0x0180
	int32_t m_max; //0x0188
	char pad_018C[4]; //0x018C
	int32_t m_cur; //0x0190
}; //Size: 0x0194

class CCameraInterface
{
public:
	char pad_0x0000[0x248]; //0x0000
	class CCameraList* m_list; //0x0180
	int32_t m_max; //0x0188
	char pad_018C[4]; //0x018C
	int32_t m_cur; //0x0190
}; //Size: 0x0194

class CReplayInterface
{
public:
	void* N000006F5; //0x0000 
	class CCameraInterface* m_camera; //0x0008 
	class CVehicleInterface* m_vehicle; //0x0010
	class CPedInterface* m_ped; //0x0018
	class CPickupInterface* m_pickup; //0x0020 
	class CObjectInterface* m_object; //0x0028
}; //Size: 0x0030

#endif // !POOLS_CLASSES_H
