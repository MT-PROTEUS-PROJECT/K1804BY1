#pragma once

#include <model.hpp>
#include <array>
#include <stack>

class K1804BY1 final : public vsm::model
{
public:
	static constexpr DWORD MODEL_KEY = 0x00000003;

private:
	static constexpr uint8_t WORD_SIZE = 10;
	static constexpr uint16_t MAX_VALUE = 1023; // 2 ** 10 - 1
	static constexpr uint8_t STACK_DEPTH = 64;
	static constexpr size_t DELAY_NS = 500'000;

	using io_pins = std::array<vsm::pin, WORD_SIZE>;
private:
	// input
	vsm::pin C0;
	
	io_pins _pins_DR;
	io_pins _pins_R;
	io_pins _pins_D;
	
	vsm::pin PUP;
	vsm::pin FE;
	vsm::pin S1;
	vsm::pin S2;
	vsm::pin ZA;
	vsm::pin RE;
	vsm::pin T;
	vsm::pin OE;

	// output
	vsm::pin C4;
	io_pins _pins_Y;

	ABSTIME _last_update;
	int32_t _last_popped_value;

private:
	uint16_t _cmk;
	uint16_t _ra;
	std::stack<uint16_t> _stack;

private:
	uint16_t getCMK() const noexcept;
	bool updateCMK() noexcept; // true if overflow

	uint16_t getRA() const noexcept;
	void updateRA() noexcept;

	uint16_t getStack() noexcept;
	void updateStack();

	void writeRes(ABSTIME time, uint16_t res);

public:
	K1804BY1();

	VOID setup(IINSTANCE *instance, IDSIMCKT *dsimckt);
	VOID simulate(ABSTIME time, DSIMMODES mode);
};
