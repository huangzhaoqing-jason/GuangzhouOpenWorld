#include "Core/Platform/GZGCDDispatch.h"

#if PLATFORM_MAC
	#include <dispatch/dispatch.h>
#else
	#include "Async/ParallelFor.h"
	#include "Async/Async.h"
#endif

namespace GZGCD
{
#if PLATFORM_MAC
	static dispatch_queue_t GetDispatchQueue(EDispatchQueue Queue)
	{
		switch (Queue)
		{
		case EDispatchQueue::Main:
			return dispatch_get_main_queue();
		case EDispatchQueue::HighPriority:
			return dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
		case EDispatchQueue::Default:
			return dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0);
		case EDispatchQueue::Background:
			return dispatch_get_global_queue(QOS_CLASS_BACKGROUND, 0);
		default:
			return dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0);
		}
	}

	void Async(EDispatchQueue Queue, TFunction<void()> Task)
	{
		dispatch_queue_t QueueRef = GetDispatchQueue(Queue);
		dispatch_async(QueueRef, ^{ Task(); });
	}

	void Sync(EDispatchQueue Queue, TFunction<void()> Task)
	{
		dispatch_queue_t QueueRef = GetDispatchQueue(Queue);
		dispatch_sync(QueueRef, ^{ Task(); });
	}

	void ParallelFor(int32 Num, TFunction<void(int32 Index)> Body)
	{
		if (Num <= 0) return;
		dispatch_queue_t QueueRef = dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0);
		dispatch_apply((size_t)Num, QueueRef, ^(size_t Index)
		{
			Body((int32)Index);
		});
	}
#else
	void Async(EDispatchQueue Queue, TFunction<void()> Task)
	{
		// 非 Mac 平台回退到 UE 任务图，避免编译依赖 GCD
		if (Queue == EDispatchQueue::Main)
		{
			AsyncTask(ENamedThreads::GameThread, MoveTemp(Task));
		}
		else
		{
			Async(EAsyncExecution::ThreadPool, MoveTemp(Task));
		}
	}

	void Sync(EDispatchQueue Queue, TFunction<void()> Task)
	{
		// 同步执行：非 Mac 下直接在当前线程调用
		Task();
	}

	void ParallelFor(int32 Num, TFunction<void(int32 Index)> Body)
	{
		if (Num <= 0) return;
		::ParallelFor(Num, [&](int32 Index)
		{
			Body(Index);
		});
	}
#endif
}
