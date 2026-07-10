#pragma once

class IStamina
{
public:
	virtual ~IStamina() = default;

	virtual bool CanConsume(float amount) const = 0;
	virtual void Consume(float amount) = 0;
	virtual float GetCurrentStamina() const = 0;
	virtual float GetMaxStamina() const = 0;
	virtual bool IsExhausted() const = 0;
};