#include <assert.h>
#include <limits.h>
#include "mock_inputs.h"

int main(void)
{
    int16_t axes[MAX_AXIS_NUM];
    uint8_t buttons[MAX_BUTTONS_NUM / 8];
    MockInputsGenerate(0, axes, buttons);
    assert(axes[0] == AXIS_MIN_VALUE && buttons[0] == 1);
    MockInputsGenerate(2000, axes, buttons);
    assert(axes[0] == 0 && buttons[0] == 4);
    MockInputsGenerate(4000, axes, buttons);
    assert(axes[0] == AXIS_MAX_VALUE);
    MockInputsGenerate(8000, axes, buttons);
    assert(axes[0] == AXIS_MIN_VALUE && buttons[1] == 1);
    for (uint32_t t = 0; t < 16000; t++)
    {
        MockInputsGenerate(t, axes, buttons);
        for (unsigned i = 0; i < MAX_AXIS_NUM; i++)
            assert(axes[i] >= AXIS_MIN_VALUE && axes[i] <= AXIS_MAX_VALUE);
        for (unsigned i = 0; i < MAX_BUTTONS_NUM; i++)
            assert(((buttons[i / 8] >> (i % 8)) & 1) ==
                   (t % 1000 < 500 && i == t / 1000));
    }
    MockInputsGenerate(UINT32_MAX, axes, buttons);
    for (unsigned i = 0; i < MAX_AXIS_NUM; i++)
        assert(axes[i] >= AXIS_MIN_VALUE && axes[i] <= AXIS_MAX_VALUE);
    return 0;
}
