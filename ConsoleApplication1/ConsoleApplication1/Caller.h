#pragma once

namespace Caller
{
	class BaseCaller
	{
	public:
		BaseCaller() = default;
		virtual ~BaseCaller() {}

	private:

	};

	template<typename T >
	class Caller : public BaseCaller
	{
	public:
		void Call() { return static_cast<T*>(this)->Execute(); }
	};

	class Executor_1 : public Caller<Executor_1>
	{
	public:
		void Execute();
	};

	class Executor_2 : public Caller<Executor_2>
	{
	public:
		void Execute();
	};

}