#include <iostream>
#include <vector>
#include <algorithm>
#include <variant>
#include <any>

template<typename Interface, typename... Ts>
struct Implementation
{
public:
	template<typename ConcreteType>
	Implementation(ConcreteType&& object)
		: storage{ std::forward<ConcreteType>(object) }
	, getter{ [](std::variant<Ts...>& storage) -> Interface& { return std::get<ConcreteType>(storage); } }
	{}

	Interface* operator->() { return &getter(storage); }

private:
	std::variant<Ts...> storage;
	Interface& (*getter)(std::variant<Ts...>&);
};

template<typename Interface>
struct Implementation<Interface>
{
public:
	template<typename ConcreteType>
	Implementation(ConcreteType&& object)
		: storage{ std::forward<ConcreteType>(object) }
	, getter{ [](std::any& storage) -> Interface& { return std::any_cast<ConcreteType&>(storage); } }
	{}

	Interface* operator->() { return &getter(storage); }

private:
	std::any storage;
	Interface& (*getter)(std::any&);
};


struct ICalculator
{
	virtual int compute(int input) const = 0;
	virtual void log(int input, int output) const = 0;
};

struct BigCalculator : ICalculator
{
	int compute(int input) const override
	{
		return input * 5;
	}

	void log(int input, int output) const override
	{
		std::cout << "BigCalculator took an input of " << input << " and produced an output of " << output << '\n';
	}

};

struct SmallCalculator : ICalculator
{

	int  compute(int input) const override
	{
		return input + 2;
	}

	void log(int input, int output) const override
	{
		std::cout << "SmallCalculator took an input of " << input << " and produced an output of " << output << '\n';
	}
};

using v_Calculator = Implementation<ICalculator, SmallCalculator, BigCalculator>; //calling variant implementation
using a_Calculator = Implementation<ICalculator>; //calling any implementation

template <typename Calculator>
void compute() {
	std::vector<Calculator> calculations{ Calculator{ SmallCalculator{} }, Calculator{ BigCalculator{} } };

	std::for_each(std::begin(calculations), std::end(calculations), [](auto& calc) { auto res = calc->compute(5); calc->log(5, res);  });
}
int main() {
	compute<a_Calculator>(); // use any
	compute<v_Calculator>(); // use variant
}