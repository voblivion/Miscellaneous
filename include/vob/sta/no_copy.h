#pragma once

#include <utility>


namespace vob::sta
{
	using namespace std;

	template <typename Type>
	class no_copy
		: public Type
	{
	public:
#pragma region Constructors
		no_copy(no_copy const& a_other) {}

		no_copy(no_copy&& a_other) noexcept
			: Type{ move(a_other) }
		{}

		explicit no_copy()
			: Type{}
		{}

		/*template <typename... Args>
		explicit NoCopy(Args&&... a_args)
			: Type{ std::forward<Args>(a_args)... }
		{}*/

		~no_copy() = default;
#pragma endregion
#pragma region Operators
		no_copy& operator=(Type const& a_other)
		{
			return *this;
		}

		no_copy& operator=(Type&& a_other)
		{
			Type::operator=(std::move(a_other));
			return *this;
		}

		no_copy& operator=(no_copy const& a_other)
		{
			return *this;
		}

		no_copy& operator=(no_copy&& a_other) noexcept
		{
			Type::operator=(std::move(a_other));
			return *this;
		}
#pragma endregion
	};
}