// Copyright 2024 Christopher Durham. SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "CoreMinimal.h"

/**
 * Helper macro to bind a UObject instance and a member UFUNCTION to a dynamic multi-cast delegate.
 *
 * @param	ThisDelegate	Dynamic mutlicast delegate to bind to
 * @param	UserObject		UObject instance, as TScriptInterface
 * @param	FuncName		Function pointer to member UFUNCTION, usually in form &IInterfaceName::FunctionName
 */
#define BUTTPLUG_BindScriptInterface( ThisDelegate, UserObject, FuncName ) \
	Buttplug::Private::TDynamicDelegateHelper(ThisDelegate).BindScriptInterface_Impl(UserObject, FuncName, STATIC_FUNCTION_FNAME(TEXT(#FuncName)))

/**
 * Helper macro to bind a UObject instance and a member UFUNCTION to a dynamic multi-cast delegate.
 *
 * @param	ThisDelegate    Dynamic mutlicast delegate to bind to
 * @param	UserObject		UObject instance, as TScriptInterface
 * @param	FuncName		Function pointer to member UFUNCTION, usually in form &IInterfaceName::FunctionName
 */
#define BUTTPLUG_AddScriptInterface( ThisDelegate, UserObject, FuncName ) \
	Buttplug::Private::TDynamicMulticastDelegateHelper(ThisDelegate).AddScriptInterface_Impl(UserObject, FuncName, STATIC_FUNCTION_FNAME(TEXT(#FuncName)))

namespace Buttplug::Private
{

/** Helper class for binding interface methods to dynamic single-cast delegates */
template<typename ThreadSafetyMode, typename RetValType, typename... ParamTypes>
class TDynamicDelegateHelper
{
public:
	/** The actual delegate class */
	using FDelegate = TBaseDynamicDelegate<ThreadSafetyMode, RetValType, ParamTypes...>;
	FDelegate& ThisDelegate;

	/** Templated helper class to define a typedef for user's method pointer */
	template<typename UserClass>
	using TMethodPtrResolver = typename FDelegate::template TMethodPtrResolver<UserClass>;

	/**
	 * Binds a UObject instance and an IInterface method address to this delegate.
	 *
	 * @param	InUserObject		UObject instance
	 * @param	InMethodPtr			Member function address pointer
	 * @param	InFunctionName		Name of member function, without class name
	 *
	 * NOTE:  Do not call this function directly.  Instead, call BUTTPLUG_BindScriptInterface()
	 *        which is a macro proxy function that automatically sets the function name string for the caller.
	 */
	template<typename UserInterface>
	void BindScriptInterface_Impl(TScriptInterface<UserInterface> InUserObject, TMethodPtrResolver<UserInterface>::FMethodPtr InMethodPtr, FName InFunctionName)
	{
		static_assert(TIsIInterface<UserInterface>::Value, "Should only call this with interface methods");
		UObject* UserObject = InUserObject.GetObject();
		check(UserObject != nullptr && InMethodPtr != nullptr);

		// NOTE: We're not actually storing the incoming method pointer or calling it.  We simply require it for type-safety reasons.
		check(UserObject->Implements<UserInterface::UClassType>());

		ThisDelegate.BindUFunction(UserObject, InFunctionName);
		ensureMsgf(ThisDelegate.IsBound(), TEXT("Unable to bind delegate to '%s' (function might not be marked as a UFUNCTION or object may be pending kill)"), *InFunctionName.ToString());
	}
};

/** Helper class for binding interface methods to dynamic multi-cast delegates */
template<typename ThreadSafetyMode, typename RetValType, typename... ParamTypes>
class TDynamicMulticastDelegateHelper
{
public:
	/** The actual delegate class */
	using FDelegate = TBaseDynamicMulticastDelegate<ThreadSafetyMode, RetValType, ParamTypes...>;
	FDelegate& ThisDelegate;

	/** The single-cast delegate helper class for this multi-cast delegate */
	using FSinglecastHelper = TDynamicDelegateHelper<ThreadSafetyMode, RetValType, ParamTypes...>;

	/** Templated helper class to define a typedef for user's method pointer */
	template<typename UserClass>
	using TMethodPtrResolver = typename FSinglecastHelper::template TMethodPtrResolver<UserClass>;

	/**
	 * Binds a UObject instance and a IInterface method address to this multi-cast delegate.
	 *
	 * @param	InUserObject		UObject instance
	 * @param	InMethodPtr			Member function address pointer
	 * @param	InFunctionName		Name of member function, without class name
	 *
	 * NOTE:  Do not call this function directly.  Instead, call BUTTPLUG_AddScriptInterface()
	 *        which is a macro proxy function that automatically sets the function name string for the caller.
	 */
	template<typename UserInterface>
	void AddScriptInterface_Impl(TScriptInterface<UserInterface> InUserObject, TMethodPtrResolver<UserInterface>::FMethodPtr InMethodPtr, FName InFunctionName)
	{
		static_assert(TIsIInterface<UserInterface>::Value, "Should only call this with interface methods");
		UObject* UserObject = InUserObject.GetObject();
		check(UserObject != nullptr && InMethodPtr != nullptr);

		// NOTE: We're not actually storing the incoming method pointer or calling it.  We simply require it for type-safety reasons.
		check(UserObject->Implements<UserInterface::UClassType>());

		typename FSinglecastHelper::FDelegate NewDelegate;
		FSinglecastHelper(NewDelegate).BindScriptInterface_Impl(InUserObject, InMethodPtr, InFunctionName);

		ThisDelegate.Add(NewDelegate);
	}
};

} // namespace Buttplug::Private
