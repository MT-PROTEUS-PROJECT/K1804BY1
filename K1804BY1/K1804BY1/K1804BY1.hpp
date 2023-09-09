#pragma once

#include <model.hpp>
#include <array>
#include <stack>

class K1804BY1 final : public vsm::model
{
public:
	static constexpr DWORD MODEL_KEY = 0x00000003;

private:
	static constexpr uint8_t WORD_SIZE = 4;
	static constexpr uint8_t MAX_VALUE = 15; // 2 ** 4 - 1
	static constexpr uint8_t STACK_DEPTH = 4;

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

private:
	uint8_t _cmk;
	uint8_t _ra;
	std::stack<uint8_t> _stack;

private:
	uint8_t getCMK() const noexcept;
	bool updateCMK() noexcept; // true if overflow

	uint8_t getRA() const noexcept;
	void updateRA() noexcept;

	uint8_t getStack() const noexcept;
	void updateStack();

	void writeRes(ABSTIME time, uint8_t res);

public:
	K1804BY1();

	VOID setup(IINSTANCE *instance, IDSIMCKT *dsimckt);
	VOID simulate(ABSTIME time, DSIMMODES mode);
};
