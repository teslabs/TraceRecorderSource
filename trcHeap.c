/*
* Percepio Trace Recorder for Tracealyzer v4.6.3
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The implementation for heaps.
*/

#include <trcRecorder.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#if (TRC_USE_HEAPS == 1)

traceResult xTraceHeapCreate(const char *szName, TraceUnsignedBaseType_t uxCurrent, TraceUnsignedBaseType_t uxHighWaterMark, TraceUnsignedBaseType_t uxMax, TraceHeapHandle_t *pxHeapHandle)
{
	TraceUnsignedBaseType_t uxStates[3];

	uxStates[TRC_HEAP_STATE_INDEX_CURRENT] = uxCurrent;
	uxStates[TRC_HEAP_STATE_INDEX_HIGHWATERMARK] = uxHighWaterMark;
	uxStates[TRC_HEAP_STATE_INDEX_MAX] = uxMax;

	return xTraceObjectRegisterInternal(PSF_EVENT_HEAP_CREATE, 0, szName, 3, uxStates, TRC_ENTRY_OPTION_HEAP, (TraceObjectHandle_t*)pxHeapHandle);
}

traceResult xTraceHeapAlloc(TraceHeapHandle_t xHeapHandle, void *pvAddress, TraceUnsignedBaseType_t uxSize)
{
	TraceUnsignedBaseType_t uxCurrent, uxHighWaterMark;
	TraceEventHandle_t xEventHandle = 0;
	
	if (xHeapHandle == 0)
	{
		/* This can happen */
		return TRC_FAIL;
	}

	/* If the address is null we assume this was a failed alloc attempt */
	if (pvAddress != 0)
	{
		/* This should never fail */
		TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntryGetState(xHeapHandle, TRC_HEAP_STATE_INDEX_CURRENT, &uxCurrent) == TRC_SUCCESS);

		/* This should never fail */
		TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntryGetState(xHeapHandle, TRC_HEAP_STATE_INDEX_HIGHWATERMARK, &uxHighWaterMark) == TRC_SUCCESS);

		uxCurrent += uxSize;

		if (uxCurrent > uxHighWaterMark)
		{
			uxHighWaterMark = uxCurrent;
			/* This should never fail */
			TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntrySetState(xHeapHandle, TRC_HEAP_STATE_INDEX_HIGHWATERMARK, uxHighWaterMark) == TRC_SUCCESS);
		}

		/* This should never fail */
		TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntrySetState(xHeapHandle, TRC_HEAP_STATE_INDEX_CURRENT, uxCurrent) == TRC_SUCCESS);
	}

	/* We need to check this */
	if (xTraceEventBegin(pvAddress != 0 ? PSF_EVENT_MALLOC : PSF_EVENT_MALLOC_FAILED, sizeof(void*) + sizeof(TraceUnsignedBaseType_t), &xEventHandle) == TRC_SUCCESS)
	{
		xTraceEventAddPointer(xEventHandle, pvAddress);
		xTraceEventAddUnsignedBaseType(xEventHandle, uxSize);
		xTraceEventEnd(xEventHandle);
	}

	return TRC_SUCCESS;
}

traceResult xTraceHeapFree(TraceHeapHandle_t xHeapHandle, void *pvAddress, TraceUnsignedBaseType_t uxSize)
{
	TraceUnsignedBaseType_t uxCurrent;
	TraceEventHandle_t xEventHandle = 0;

	if (xHeapHandle == 0)
	{
		/* This can happen */
		return TRC_FAIL;
	}

	/* If the address is null we assume this was a failed alloc attempt */
	if (pvAddress != 0)
	{
		/* This should never fail */
		TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntryGetState(xHeapHandle, TRC_HEAP_STATE_INDEX_CURRENT, &uxCurrent) == TRC_SUCCESS);

		uxCurrent -= uxSize;

		/* This should never fail */
		TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntrySetState(xHeapHandle, TRC_HEAP_STATE_INDEX_CURRENT, uxCurrent) == TRC_SUCCESS);
	}

	/* We need to check this */
	if (xTraceEventBegin(pvAddress != 0 ? PSF_EVENT_FREE : PSF_EVENT_FREE_FAILED, sizeof(void*) + sizeof(TraceUnsignedBaseType_t), &xEventHandle) == TRC_SUCCESS)
	{
		xTraceEventAddPointer(xEventHandle, pvAddress);
		xTraceEventAddUnsignedBaseType(xEventHandle, uxSize);
		xTraceEventEnd(xEventHandle);
	}

	return TRC_SUCCESS;
}

#endif /* (TRC_USE_HEAPS == 1) */

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */
