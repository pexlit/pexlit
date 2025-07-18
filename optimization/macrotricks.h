#pragma once
// https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments
#define GetMacroOverLoad(_1, _2, _3, NAME, ...) NAME
#define wrap(...) __VA_ARGS__
#define COMMA ,

// add a member to your struct with [expression] = other variable&
#define addMemberName(memberName, expression, requireClause)		\
	constexpr T &memberName() noexcept requires(requireClause)				\
	{																\
		return expression;											\
	}																\
	constexpr const T &memberName() const noexcept requires(requireClause)	\
	{																\
		return expression;											\
	}

#define addTypeSizes(structName)            \
	typedef structName##n<1> structName##1; \
	typedef structName##n<2> structName##2; \
	typedef structName##n<3> structName##3; \
	typedef structName##n<4> structName##4;

#define addTemplateSize(shortenedTemplateName, size) \
	template <typename T>                            \
	using shortenedTemplateName##t##size = shortenedTemplateName##tn<T, size>;

#define addTemplateSizes(shortenedTemplateName)       \
	addTemplateSize(shortenedTemplateName, 1)         \
		addTemplateSize(shortenedTemplateName, 2)     \
			addTemplateSize(shortenedTemplateName, 3) \
				addTemplateSize(shortenedTemplateName, 4)

#define addTemplateType(shortenedTemplateName, typeTemplateName, typeName)                                                 \
	template <fsize_t axisCount>                                                                                           \
	using shortenedTemplateName##typeTemplateName##n = shortenedTemplateName##tn<typeName, axisCount>;                     \
                                                                                                                           \
	addTypeSizes(shortenedTemplateName##typeTemplateName)                                                                  \
                                                                                                                           \
		template <fsize_t axisCount>                                                                                       \
		using c##shortenedTemplateName##typeTemplateName##n = const shortenedTemplateName##typeTemplateName##n<axisCount>; \
                                                                                                                           \
	addTypeSizes(c##shortenedTemplateName##typeTemplateName)

#define addTemplateTypes(shortenedTemplateName)                                         \
	template <typename T, fsize_t axisCount>                                            \
	using c##shortenedTemplateName##tn = const shortenedTemplateName##tn<T, axisCount>; \
	addTemplateSizes(shortenedTemplateName)                                             \
		addTemplateSizes(c##shortenedTemplateName)                                      \
			addTemplateType(shortenedTemplateName, , fp)                                \
			addTemplateType(shortenedTemplateName, f, float)                                \
				addTemplateType(shortenedTemplateName, i, int)                          \
					addTemplateType(shortenedTemplateName, l, long)                     \
						addTemplateType(shortenedTemplateName, b, bool)                 \
							addTemplateType(shortenedTemplateName, s, size_t)

// https://stackoverflow.com/questions/18290523/is-a-default-move-constructor-equivalent-to-a-member-wise-move-constructor
#define addDefaultConstructors(structName, constructorType)              \
	constructorType structName(const structName &other) = default;       \
	constructorType structName(structName &&other) = default;            \
	constructorType structName &operator=(structName &&other) = default; \
	constructorType structName &operator=(const structName &other) = default;

// newexpression should be a macro that creates a new instance with the same size as the argument
#define addOperator(o, newExpression, structType, otherStructType, functionType, arraySize)                                                               \
	template <typename t2, typename resultType = decltype(std::declval<T>() o std::declval<t2>())>\
	functionType decltype(auto) operator o(const t2 &b) const requires (std::is_arithmetic_v<t2>)                                                                                 \
	{                                                                                                                                          \
		newExpression(resultType, (*this)) auto resultPtr = result.begin();                                                                    \
		auto const &endPtr = end();                                                                                                            \
		for (auto thisPtr = begin(); thisPtr < endPtr; thisPtr++, resultPtr++)                                                                 \
		{                                                                                                                                      \
			*resultPtr = *thisPtr o b;                                                                                                         \
		}                                                                                                                                      \
		return result;                                                                                                                         \
	}                                                                                                                                          \
	template <typename t2, typename resultType = decltype(std::declval<T>() o std::declval<t2>())>                                             \
	functionType decltype(auto) operator o(const otherStructType b) const                                                                     \
	{                                                                                                                                          \
		newExpression(resultType, b)\
		auto* __restrict resultPtr = &(*result.begin());                                                                          \
		auto* __restrict bPtr = &(*b.begin());                                                                                                                 \
		auto* __restrict thisPtr = &(*begin());\
		for(size_t i = 0; i < arraySize; i++){\
			resultPtr[i] = thisPtr[i] o bPtr[i];\
		}\
		return result;                                                                                                                         \
	}                                                                                                                                          \
	template <typename t2, typename resultType = decltype(std::declval<T>() o std::declval<t2>())>\
	functionType friend decltype(auto) operator o(const t2 &a, const structType &b)  requires (std::is_arithmetic_v<t2>)                                                           \
	{                                                                                                                                          \
		newExpression(resultType, b) auto resultPtr = result.begin();                                                                          \
		auto const &endPtr = b.end();                                                                                                          \
		for (auto bPtr = b.begin(); bPtr < endPtr; bPtr++, resultPtr++)                                                                        \
		{                                                                                                                                      \
			*resultPtr = a o(*bPtr);                                                                                                           \
		}                                                                                                                                      \
		return result;                                                                                                                         \
	}                                                                                                                                          \
	template <typename t2>                                                                                                                     \
	functionType structType& operator o##=(const otherStructType &b)                                                                            \
	{                                                                                                                                          \
		*this = *this o b;                                                                                                            \
		return *this;                                                                                                                       \
	}                                                                                                                                          \
	template <typename t2>                                                                                                                     \
	functionType structType& operator o##=(const t2 &b) requires (std::is_arithmetic_v<t2>)                                      \
	{                                                                                                                                          \
		for (T & val : (*this))                                                                                                                \
		{                                                                                                                                      \
			val o## = b;                                                                                                                       \
		}\
		return *this; \
	}

#define addOperators(newExpression, structType, otherStructType, functionType, arraySize)             \
	addOperator(+, newExpression, structType, wrap(otherStructType), functionType, arraySize)         \
		addOperator(-, newExpression, structType, wrap(otherStructType), functionType, arraySize)     \
			addOperator(*, newExpression, structType, wrap(otherStructType), functionType, arraySize) \
				addOperator(/, newExpression, structType, wrap(otherStructType), functionType, arraySize)

#define addAssignmentOperator(structName, functionType) \
	functionType structName &operator=(structName copy) \
	{                                                   \
		this->swap(copy);                               \
		return *this;                                   \
	}
