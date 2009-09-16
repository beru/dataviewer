//
// SyncActive windows v1.0 BETA
//
// Copyright © 2002 Rui Godinho Lopes <rui@ruilopes.com>
// All rights reserved.
//
// This source file(s) may be redistributed by any means PROVIDING they
// are not sold for profit without the authors expressed written consent,
// and providing that this notice and the authors name and all copyright
// notices remain intact.
//
// It would be nice, but not necessary, that you acknowledge the author
// in your application "About Box" or Documentation.
//
// An email letting the author know that you are using it would be nice
// as well.  That's not much to ask considering the amount of work that
// went into this.
//
// THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED. USE IT AT YOUT OWN RISK. THE AUTHOR ACCEPTS NO
// LIABILITY FOR ANY DATA DAMAGE/LOSS THAT THIS PRODUCT MAY CAUSE.
//

//
// This code is implemented in others programming languages, visit
// http://www.ruilopes.com for more information.
//

//
// FEATURES
// --------
//
// - Windows that can syncronize the active state with a owner window;
// - Can automatically hide floating windows when application is deactivated;
// - Automatically hides floating windows when owner window is minimized or hidden;
// - Disables owned windows when owner is disabled;
// - Does not suffer from "shell activation problems" that some application have (read bellow).
//
//
// HISTORY
// -------
//
// 11 mar 2002 1.0 beta 0 [features are now complete]
// + Show/Hide owned windows when our window is show/hide.
// ~ Code tweaks
//
// 02 mar 2002 1.0 alpha 3
// * Fixed another shell problem. [Problem 2]
//
// 26 feb 2002 1.0 alpha 2
// + Only show hidden windows if they were hidden by our logic.
//   And not by a user call to ShowWindow or equivalent.
//
// 24 feb 2002	1.0 alpha 1
//	* The shell activation problem is now fixed. [Problem 1]
// + Show/Hide windows when application is activated/deactivated.
// + Enable/Disable owned windows when the Owner window is activated/deactivated.
//
// 23 feb 2002 1.0 alpha 0
// + first alpha version.
//

//
// TODO
// ----
// - Base the template class in a concrete class to minimize
//   code size.
//
//
// POSSIBLE TODO
//	-------------
// [if you need any of these, feel free to contact me!]
//
// - I din't yet figure-out a decent way of drawing the inactive
//   state of disabled windows when our app gains the active app
//   state...  this happens because when our app gains the active
//   state all windows title bar is show in the active state even
//   when some windows are disabled :(
//   I can only draw the inactive state when our app is active...
//
//   A good alternative would be to hide the disabled floating
//   window...
//
//   I've notice that WM_WINDOWPOSCHANGING message is send to
//   all windows!? Maybe this would help....
//   The windows message WM_SYNCPAINT migth also be useful.
//   FYI I don't really need this functionallity... though it
//   would be very nice to have it.... on the other way... why
//   would one want a disabled floating window? Well, maybe
//   YOU?!
//
// - When we are getting/loosing the active state to a window in
//   our process/thread, show/hide all top-level windows that do
//   not have the (KeepVisible | KeepAllwaysVisible) flags.
//

/* During the implementation of this code I've found some problems
 * with Windows Shell...
 *
 * Problem 2:
 *
 *	NOTE: I've managed to get arround this problem.
 *
 *	When we click the icon of our windows in the task-bar our window
 * get minimized, but we receive the WM_ACTIVATEAPP message teeling
 * that we are gaining the active state! when in reality we are
 * loosing it... to known if we are really active we should test
 * the 0x01 bit. (NOTE: This is not documented anywere)
 * WHY does the task-bar beaves like this?! Man... a simple thing
 * becomes a real pain... :(((
 *
 *
 *
 * Problem 1:
 *
 *	NOTE: The following problem no longer occours.
 *
 * This is only here to show how to do this on other apps and
 * because this Activation problem presists, for example, in
 * buttons and edit boxes.
 *
 * SHELL ACTIVATION PROBLEM [The tray bar or our logic has some bugs :(]
 * ------------------------
 * 1. Select App by clicking on App Icon on System Tray Bar
 * 2. Select Explorer by clicking on Explorer Icon on System Tray Bar
 * 3. Click on a empty System Tray Bar space
 * 4. Click the minimize button of Explorer window
 * 5. Now the App is draw in active state :(
 *    If you dont see anything strange, try to move your move over
 *    the close button of this App, and you will see that it will
 *    draw in the inactive state.
 *
 * NOTE: VS 6.0 also suffers from this same strange beaviour...
 *       So all MFC apps also suffer from this :/
 *
 *       But VS 6.0 Main Title bar stays in the correct state
 *       the floating windows are the ones with this problem.
 *
 *       The following apps suffer all the same problems as this app:
 *       - EditPlus (an MFC app)
 *       - Paint Shop Pro 7
 *       - PhotoShop 6
 *       - WordPad (Even the caret from text editor stays blinking...)
 *
 *			- NotePad (Only the caret stays blinking, no active title bar problem)
 *         (Even NotePad... This must be a bug with Shell Tray Bar?)
 *       - Command Prompt (the same problem as notepad)
 *         Well, this caret problem is present in all applications with caret...
 *         Even VS.NET has this problem :(
 *
 *         Even the dialog box buttons mark the visual style as active, but are not active at all :(
 *
 * There are other ways to make this happen... but I get bored trying
 * to figure the exact pattern to do that :(
 */


#if !defined(AFX_ISYNCACTIVEIMPL_H__F9D69501_DF17_4894_BA13_B60D8B1DB1E1__INCLUDED_)
#define AFX_ISYNCACTIVEIMPL_H__F9D69501_DF17_4894_BA13_B60D8B1DB1E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifdef _DEBUG
#define TRACECURRENTWINDOWTEXT() TraceWindowText(static_cast<T*>(this));
template <typename T> // this is a template just because I don't want to create an extra cpp file...
void TraceWindowText(T *pThis)
{
	TCHAR buf[512];
	::GetWindowText(pThis->m_hWnd, buf, 511);
	buf[511] = 0;
	ATLTRACE(buf);
}
#else
#define TRACECURRENTWINDOWTEXT()
#endif



namespace Rgl
{
	// Private message sended to window for syncronization purpouses.
	//NOTE: This message id should be unique in your application...
	enum { WM_SYNCACTIVE = WM_APP+999 };


/*
//TODO: The ideia is to base the template class in a concrete class
// to minimize code size.
class CSyncActive : public CMessageMap
{
public:
	CSyncActive()
	{
		_dwFlags = 0;
	}

protected:

private:
	DWORD _dwFlags;
}
*/

/// Every form that want's to participate in activation
/// syncronization must implement this interface.
// NOTE: The classes do not realy inherit from this interfaces
//       They support this by handling the WM_SYNCACTIVE private
//       message.
struct ATL_NO_VTABLE ISyncActive
{
	/// <summary>Returns <c>true</c> if this object is forcing the active state.</summary>
	virtual BOOL IsForceActive() = 0;
	/// <summary>Changes the current active state of this object and of all owned objects.</summary>
	virtual void SetForceActive(BOOL bForceActive) = 0;
	/// <summary>Returns <c>true</c> if this object is syncronizing the active state with owner.</summary>
	virtual BOOL IsSyncActive() = 0;
	/// <summary>Changes the current syncronizing of the active state with owner.</summary>
	virtual void SetSyncActive(BOOL bSyncActive) = 0;
	/// <summary>Returns <c>true</c> if this object is allways visible.</summary>
	virtual BOOL IsKeepAllwaysVisible() = 0;
	/// <summary>Changes the current flag os allways visible.</summary>
	virtual void SetKeepAllwaysVisible(BOOL bKeepVisible) = 0;
};


/** Base class for SyncActive windows.
  *
  * @param T Your class type.
  * @author Rui Lopes <rui@ruilope.com>
  */
template <typename T>
class ATL_NO_VTABLE ISyncActiveImpl : public CMessageMap
{
public:
	/** @param bSyncActive Pass TRUE is this window will syncronize the active state with owner.
	  * @param bKeepAllwaysVisible Pass TRUE is this window will stay visible when application is disabled.
	  */
	ISyncActiveImpl(BOOL bSyncActive, BOOL bKeepAllwaysVisible = FALSE)
	{
		_dwFlags = bSyncActive ? SyncActive : 0;
		if (bKeepAllwaysVisible)
			_dwFlags |= KeepAllwaysVisible;
	}


public:
	BEGIN_MSG_MAP(ISyncActiveImpl<T>)
		MESSAGE_HANDLER(WM_SYNCACTIVE, OnSyncActive)
		MESSAGE_HANDLER(WM_NCACTIVATE, OnNcActivate)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_ACTIVATEAPP, OnActivateApp)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MESSAGE_HANDLER(WM_ENABLE, OnEnable)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()


public:
	// Start of ISyncActive implementation
	//

	BOOL IsForceActive()
	{
		return (_dwFlags & ForceActive) != 0;
	}

	void SetForceActive(BOOL bForceActive)
	{
		if (bForceActive == IsForceActive())
			return;

		if (bForceActive)
			_dwFlags |= ForceActive;
		else
			_dwFlags &= ~ForceActive;

		// update the activation state of this form and of all owned forms.
		UpdateActivationState();
	}

	BOOL IsSyncActive()
	{
		return (_dwFlags & SyncActive) != 0;
	}

	void SetSyncActive(BOOL bSyncActive)
	{
		if (bSyncActive)
			_dwFlags |= SyncActive;
		else
			_dwFlags &= ~SyncActive;
	}

	BOOL IsKeepAllwaysVisible()
	{
		return (_dwFlags & KeepAllwaysVisible) != 0;
	}

	void SetKeepAllwaysVisible(BOOL bKeepVisible)
	{
		if (bKeepVisible)
			_dwFlags |= KeepAllwaysVisible;
		else
			_dwFlags &= ~KeepAllwaysVisible;
	}


	//
	// End of ISyncActive implementation


protected:
	/** Called when the ForceActive state has changed.
	  * @param newState New state of ForceActive state.
	  */
	virtual void OnForceActiveChanged(BOOL nNewState)
	{
		// by default this does nothing.
	}


private:
	// Message map handlers:

	/** Handler of WM_NCACTIVATE message.
	  */
	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// NOTE: The documentation says that when we receive a WM_NCACTIVATE
		// message, lParam is not used, but that is incorrect. It seems that
		// lParam contains the handle of the window being activated/deactivated
		// when our window is being deactivated/activated. lParam can also
		// be NULL, but this only happens when that window does not belong to
		// our own process.
		//
		// If you followed the help, you also have to handle the WM_ACTIVATE
		// message and place the following code block there.
		{
			DWORD dwOwnerFlags;
			HWND hOurOwner = GetTopLevelOwner(static_cast<T*>(this)->m_hWnd, &dwOwnerFlags);
			HWND hOtherOwner = GetTopLevelOwner((HWND)lParam);

			if (hOurOwner != hOtherOwner) {
				ATLASSERT(hOurOwner != NULL); // Our Owner is not a SyncActive window!? BIG Error.

				SetForceActive(hOurOwner, LOWORD(wParam) != WA_INACTIVE);
			}
		}

		if (IsForceActive())
			wParam = TRUE;

		return ::DefWindowProc(static_cast<T*>(this)->m_hWnd, uMsg, wParam, lParam);
	}


	/** Show/Hide application floating windows when we are getting/loosing
	  * the active state to other application.
	  */
	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE; // We only want to intercept this message.

		if (HIWORD(wParam)) { // Minimize?
			_dwFlags |= IsMinimized;
		} else {
			if (_dwFlags & IsMinimized) {
				//
				// Show all floating windows that were hidden when they received
				// a WM_ACTIVATEAPP to hide, but don't get hidden because their
				// owner was minimized.  This is needed due a bug in Windows
				// task-bar...
				// NOTE: There is no problem is user only click the minimize
				// button to hide our window...
				//
				// To see what I'm talking about do this:
				// 1. Comment to following SetOwnedSyncActiveWindowsFlags call
				//    AND comment the "if (!IsIconic(...)...)" on the OnActivateApp
				//    method below this one.
				// 2. Left-Click on our window icon in task-bar and you will see
				//    that our window will minimize AND also that our floating
				//    windows get displayed on screen?!
				// 3. Don't forget to uncomment the commented code in 1.!
				//
				SetOwnedSyncActiveWindowsFlags(SAF_SHOW);
				_dwFlags &= ~IsMinimized;
			}
		}

		return 0;
	}


	/** Show/Hide ourself when the activation state of application
	  * have changed.
	  * NOTE: This message is sended to all top-level windows of
	  * our application.
	  */
	LRESULT OnActivateApp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE; // We only want to intercept this message.

		// Show/Hide ourselfs if we do NOT have the KeepAllwaysVisible flag
		if (!(_dwFlags & KeepAllwaysVisible)) {
			// show/hide the window if window was_hidden_by_us/is_visible.

			// Ignore WM_ACTIVATEAPP if our owner is minimized or hidden.
			// See the OnActivate method for details.
			HWND hTopLevelOwner = GetTopLevelOwner(static_cast<T*>(this)->m_hWnd);
			if (!IsIconic(hTopLevelOwner) && IsWindowVisible(hTopLevelOwner))
				CommonShowWindow(wParam);
		}

		return 0;
	}

	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE; // We only want to intercept this message.

		// sync the visible state of our owned windows
		SetOwnedSyncActiveWindowsFlags(wParam ? SAF_SHOW : SAF_HIDE);
		
		return 0;
	}


	/** Syncronize our activation state with owner if this form
	  * syncronizes active state with owner.
	  */
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE; // We only want to intercept this message.

		if (IsSyncActive())
			SyncForceActive();

		return 0;
	}


	/** Syncronizes our owned windows enable state with our new enable state.
	  */
	LRESULT OnEnable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE; // We only want to intercept this message.

		SetEnableWindow(wParam);

		return 0;
	}


	/** Our private WM_SYNCACTIVE message handler.
	  */
	LRESULT OnSyncActive(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam & (SAF_FORCEACTIVE | SAF_FORCEINACTIVE))
			SetForceActive(wParam & SAF_FORCEACTIVE);

		if (wParam & (SAF_ENABLE | SAF_DISABLE))
			SetEnableWindow(wParam & SAF_ENABLE);

		if (wParam & (SAF_SHOW | SAF_HIDE))
			SetShowWindow(wParam & SAF_SHOW);

		if (wParam & SAF_RETURNFLAGS)
			return _dwFlags | IsSyncActiveWindow;

		return 0;
	}



private:
	// Implementation:

	// Flags returned from our WM_SYNCACTIVE private message handler.
	enum Flags
	{
		// Window want's to syncronize the activation state with owner.
		SyncActive         = 0x00000001,
		// Keep this window visible even when owner is deactivated by a window of our app.
		KeepVisible        = 0x00000002, //NOTE: NOT IMPLEMENTED
		// Keep this window visible even when owner is deactivated by a any window.
		KeepAllwaysVisible = 0x00000004,

		// Window will draw in the active state, even when not active.
		ForceActive        = 0x00010000,
		// Is set when this window is hidden when owner is deactivated.
		HiddenByUs         = 0x00020000,
		// Is set when this window is disabled when owner is disabled.
		DisabledByUs       = 0x00040000,

		// Is set when window is minimized, is reset when is restored
		IsMinimized        = 0x40000000,


		// Value returned by a windows when that window processes
		// the WM_SYNCACTIVE message
		IsSyncActiveWindow = 0x80000000
	};


	// Flags sended to our WM_SYNCACTIVE private message.
	enum SYNCACTIVEFLAGS
	{
		// Changes window activation state to...
		SAF_FORCEACTIVE   = 0x00000001, // ... Active.
		SAF_FORCEINACTIVE = 0x00000002, // ... Inactive.

		// Changes window enabled state to...
		SAF_ENABLE        = 0x00000004, // ... Enabled
		SAF_DISABLE       = 0x00000008, // ... Disabled.

		// Changes window visibility state to...
		SAF_SHOW          = 0x00000010, // ... Visible.
		SAF_HIDE				= 0x00000020, // ... Invisible.

		// Retuns window flags (from the Flags enum)
		SAF_RETURNFLAGS   = 0x80000000
	};


	/** Syncronizes the activation state of this form with owner
	  * even if we are not syncronizing our state with owner.
	  */
	void SyncForceActive()
	{
		DWORD dwFlags;
		HWND hWnd = GetTopLevelOwner(static_cast<T*>(this)->m_hWnd, &dwFlags);
		if (hWnd)
			SetForceActive(dwFlags & ForceActive);
	}


	/** Update the activation state of our form and all of our owned forms.
	  * The activation state of all forms bellow our owned forms will also be updated.
	  */
	void UpdateActivationState()
	{
		BOOL bActive = IsForceActive();

		// update our activation state
		::DefWindowProc(static_cast<T*>(this)->m_hWnd, WM_NCACTIVATE, bActive ? 1 : 0, NULL);

		// notifies this class that the force active state has changed
		OnForceActiveChanged(bActive);

		// update the activation state of our owned windows
		SetOwnedSyncActiveWindowsFlags(bActive ? SAF_FORCEACTIVE : SAF_FORCEINACTIVE);
	}


	/** Set's the enable state of our window and our onwed windows.
	  */
	void SetEnableWindow(BOOL bEnable)
	{
		BOOL bCallEnableWindow = FALSE;

		if (bEnable) {
			// only enable this window if the window was disabled by us
			if (_dwFlags & DisabledByUs) {
				_dwFlags &= ~DisabledByUs;
				bCallEnableWindow = TRUE;
			}
		} else {
			// only disable this window if it is active
			if (static_cast<T*>(this)->IsWindowEnabled()) {
				_dwFlags |= DisabledByUs;
				bCallEnableWindow = TRUE;
			}
		}


		// An interesting fact: EnableWindow passes the argument value to WM_ENABLE message as-is.
		if (bCallEnableWindow)
			static_cast<T*>(this)->EnableWindow(bEnable);

		// sync the enable state of our owned windows
		SetOwnedSyncActiveWindowsFlags(bEnable ? SAF_ENABLE : SAF_DISABLE);
	}


	/** Set's the visibility state of our window and our onwed windows.
	  */
	void SetShowWindow(BOOL bShow)
	{
		// show/hide the window if window was_hidden_by_us/is_visible.
		CommonShowWindow(bShow);

		// sync the visible state of our owned windows
		SetOwnedSyncActiveWindowsFlags(bShow ? SAF_SHOW : SAF_HIDE);
	}


	/** Retrives the flags from a IsSyncActiveWindow Window.
	  * @return TRUE If hWnd Window is a IsSyncActiveWindow.
	  */
	static BOOL GetSyncActiveWindowFlags(IN HWND hWnd, OUT DWORD *pdwFlags)
	{
		return (*pdwFlags = ::SendMessage(hWnd, WM_SYNCACTIVE, SAF_RETURNFLAGS, NULL)) & IsSyncActiveWindow;
	}


	/** Set's the flags of a IsSyncActiveWindow Window.
	  */
	static void SetSyncActiveWindowFlags(IN HWND hWnd, IN DWORD dwFlags)
	{
		::SendMessage(hWnd, WM_SYNCACTIVE, dwFlags, 0);
	}


	/** Set's the flags in all owned windows.
	  */
	void SetOwnedSyncActiveWindowsFlags(DWORD dwFlags)
	{
		HWND hWndThis = static_cast<T*>(this)->m_hWnd;

		HWND hWnd = ::GetWindow(::GetDesktopWindow(), GW_CHILD);
		while (hWnd) {
			if (hWndThis == ::GetWindow(hWnd, GW_OWNER))
				SetSyncActiveWindowFlags(hWnd, dwFlags);
			hWnd = ::GetWindow(hWnd, GW_HWNDNEXT);
		}
	}


	/** show/hide the window if window was_hidden_by_us/is_visible.
	  */
	void CommonShowWindow(BOOL bShow)
	{
		BOOL bCallShowWindow = FALSE;

		if (bShow) {
			// only show this window if the window was hidden by us
			if (_dwFlags & HiddenByUs) {
				_dwFlags &= ~HiddenByUs;
				bCallShowWindow = TRUE;
			}
		} else {
			// only hide this window if it is visible
			if (static_cast<T*>(this)->IsWindowVisible()) {
				_dwFlags |= HiddenByUs;
				bCallShowWindow = TRUE;
			}
		}

		if (bCallShowWindow)
			static_cast<T*>(this)->ShowWindow(bShow ? SW_SHOWNOACTIVATE : SW_HIDE);
	}


	/** Returns the top-level SyncActive owner form from a Form.
	  */
	static HWND GetTopLevelOwner(IN HWND hWnd, OUT DWORD *pdwFlags = NULL)
	{
		// Get the top-level owner form that does not
		// syncronize the active state with owner.

		DWORD dwFlags;

		while (GetSyncActiveWindowFlags(hWnd, &dwFlags)) {
			if (dwFlags & SyncActive) {
				if ((hWnd = ::GetWindow(hWnd, GW_OWNER)) == NULL)
					return NULL;
			}
			else {
				if (pdwFlags)
					*pdwFlags = dwFlags;
				return hWnd;
			}
		}

		return NULL;
	}


	/** Forces the activation state of hWnd window.
	  */
	void SetForceActive(HWND hWnd, BOOL bForceActive)
	{
		SetSyncActiveWindowFlags(hWnd, bForceActive ? SAF_FORCEACTIVE : SAF_FORCEINACTIVE);
	}

private:
	DWORD _dwFlags;
};


} // namespace Rgl

#endif // !defined(AFX_ISYNCACTIVEIMPL_H__F9D69501_DF17_4894_BA13_B60D8B1DB1E1__INCLUDED_)
