#include "legato.h"
#include "interfaces.h"

//--------------------------------------------------------------------------------------------------
/**
 * Check: SIM State.
 *
 */
//--------------------------------------------------------------------------------------------------

le_result_t yellow_test_CheckSimState(void)
{
	le_sim_States_t state;
	le_result_t res = le_sim_SelectCard(LE_SIM_EXTERNAL_SLOT_1);
	LE_ASSERT(res == LE_OK);

	state = le_sim_GetState(LE_SIM_EXTERNAL_SLOT_1);
	if (LE_SIM_READY == state)
	{
		return LE_OK;
	}
	else
	{
		return LE_FAULT;
	}
}

//--------------------------------------------------------------------------------------------------
/**
 * Check: signal quality.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t yellow_test_MeasureSignalStrength
(
    uint32_t* qual
)
{
    le_result_t res;
    res = le_mrc_GetSignalQual(qual);
    return res;
}


COMPONENT_INIT
{
    LE_INFO("YellowTest Service Start.");
}
