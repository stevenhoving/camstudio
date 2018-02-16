# todo
* Combine various option and settings into a single configure dialog.
* Delete legacy profile section when fully converted.
* CamStudio needs to use the OpenDML (AVI 2.0) extensions so files with size greater than 2 GB can be created.
* Move global variables into classes (doc, mainframe and view) and use the profile class to read and set them through the App interface to the profile class.
* Rename profile setting strings to remove programming artifacts. Color values are always single values (DWORD). Cursor folder and file name should be strings not integer lengths.

# notes
`CEditTransparencyDlg and CTransparentWnd`

The interaction between these 2 classes is artificially convoluted.

In order allow the edit dialog to update the calling CTransparentWnd the dialog overloads PreModal() to pass pointers to the CTransparentWnd window and its members that control transparency. Then as the user edits the values the dialog calls CTransparentWnd::InvalidateTransparency to update the image. All this was done using pointers and casts. Ugh. The dialog class has been changed to force the caller to use the constructor to pass in references to the control values it uses and a pointer to itself. As before, the dialog will call CTransparentWnd::InvalidateTransparency when the values change.

What should be done is to pass the new values as arguments to CTransparentWnd::InvalidateTransparency(bool bEnable, int iLevel) and let it deal with its own internal state. The call to the edit dialog should simply pass in the current state values and copy the new state after the dialog completes. Note that the dialog still retains the old values and restores them on cancel.