#include "K1804BY1.hpp"

#include <limits>
#include <bitset>

K1804BY1::K1804BY1() : _prev_addr(0), _prev_stack_state(0), _last_popped_value(-1), _cmk(0), _ra(0)
{
    static_assert(std::numeric_limits<decltype(_cmk)>::max() >= 2 * MAX_VALUE);
    static_assert(std::numeric_limits<decltype(_ra)>::max() >= MAX_VALUE);
    static_assert(std::numeric_limits<decltype(_stack)::value_type>::max() >= MAX_VALUE);
}

uint16_t K1804BY1::getCMK() const noexcept
{
    return _cmk;
}

bool K1804BY1::updateCMK() noexcept
{
    ++_cmk;
    if (_cmk > MAX_VALUE)
    {
        _cmk = 0;
        return true;
    }
    return false;
}

uint16_t K1804BY1::getRA() const noexcept
{
    return _ra;
}

void K1804BY1::updateRA() noexcept
{
    _ra = vsm::model::make_number(_pins_R);
}

uint16_t K1804BY1::getStack() noexcept
{
    if (_last_popped_value != -1)
    {
        auto res = _last_popped_value;
        _last_popped_value = -1;
        return res;
    }
    return _stack.empty() ? 0 : _stack.top();
}

void K1804BY1::updateStack()
{
    if (FE->isactive())
        return;
    if (PUP->isactive() && _stack.size() < STACK_DEPTH)
        _stack.push(_cmk);
    else if (PUP->isinactive() && !_stack.empty())
    {
        _last_popped_value = _stack.top();
        _stack.pop();
    }
}

void K1804BY1::writeRes(ABSTIME time, uint16_t res)
{
    std::bitset<WORD_SIZE> bits(res);
    for (uint16_t i = 0; i < WORD_SIZE; ++i)
        _pins_Y[i].set(time, 500, (bits[i] == 1) ? SHI : SLO);
}

VOID K1804BY1::setup(IINSTANCE *instance, IDSIMCKT *dsimckt)
{
    _instance = instance;

    C0.init(_instance, "C0");
    C4.init(_instance, "C4");

    vsm::model::init_pins(_instance, _pins_DR, "DR");
    vsm::model::init_pins(_instance, _pins_R, "R");
    vsm::model::init_pins(_instance, _pins_D, "D");
    vsm::model::init_pins(_instance, _pins_Y, "Y");

    PUP.init(_instance, "PUP");
    FE.init(_instance, "$FE$");
    S1.init(_instance, "S1");
    S2.init(_instance, "S2");
    ZA.init(_instance, "$ZA$");
    RE.init(_instance, "$RE$");
    T.init(_instance, "T");
    OE.init(_instance, "$OE$");

    _last_update = 0;
}

VOID K1804BY1::simulate(ABSTIME time, DSIMMODES mode)
{
    auto check_addr = 0;
    // CMK
    if (T->isposedge() && C0->isactive())
    {
       C4.set(time, 500, updateCMK() ? SHI : SLO);
    }
    // RA
    if (RE->isnegedge())
    {
        check_addr = 1;
        updateRA();
    }

    if (ZA->isinactive() || OE->isactive())
    {
        writeRes(time, 0);
        return;
    }

    if (time - _last_update < DELAY_NS)
    {
        _last_update = time;
        return;
    }

    if (T->isnegedge())
    {
        // STACK
        updateStack();

        auto src_addr = S1->isactive() + S2->isactive() * 2;
        uint16_t res = 0;
        switch (src_addr)
        {
        case 0:
            res = getCMK();
            break;
        case 1:
            res = getRA();
            break;
        case 2:
            res = getStack();
            break;
        case 3:
            res = vsm::model::make_number(_pins_D);
            break;
        default:
            return;
        }
        res |= vsm::model::make_number(_pins_DR);
        _cmk = res;

        writeRes(time, res);
        _last_update = time;
    }
}

extern "C"
{
    IDSIMMODEL __declspec(dllexport) *createdsimmodel(CHAR *device, ILICENCESERVER *license_server)
    {
        return license_server->authorize(K1804BY1::MODEL_KEY) ? new K1804BY1 : nullptr;
    }

    VOID __declspec(dllexport) deletedsimmodel(IDSIMMODEL *model)
    {
        delete static_cast<K1804BY1 *>(model);
    }
}
