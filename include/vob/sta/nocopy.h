#pragma once

namespace vob::sta
{
	template <typename BaseType>
	class NoCopy
		: public BaseType
	{
	public:
		// (Con|De)structors
		NoCopy(NoCopy const& a_other) {}

		NoCopy(NoCopy&& a_other) noexcept
			: BaseType{ std::move(a_other) }
		{}

		explicit NoCopy()
			: BaseType{}
		{}

		/*template <typename... Args>
		explicit NoCopy(Args&&... a_args)
			: BaseType{ std::forward<Args>(a_args)... }
		{}*/

		~NoCopy() = default;

		// Operators
		NoCopy& operator=(BaseType const& a_other)
		{
			return *this;
		}

		NoCopy& operator=(BaseType&& a_other)
		{
			BaseType::operator=(std::move(a_other));
			return *this;
		}

		NoCopy& operator=(NoCopy const& a_other)
		{
			return *this;
		}

		NoCopy& operator=(NoCopy&& a_other) noexcept
		{
			BaseType::operator=(std::move(a_other));
			return *this;
		}
	};
}