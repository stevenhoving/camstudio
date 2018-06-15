#pragma once

#include <vector>
#include <CamCapture/cam_gdiplus_fwd.h>

// MouseCaptureWndProc referenced variables

extern int g_iDefineMode;
extern BOOL g_bCapturing;
extern BOOL g_bAllowNewRecordStartKey;

extern HBITMAP g_hSavedBitmap;

extern HWND g_hWndGlobal;
extern HWND g_hMouseCaptureWnd;
extern HWND g_hFixedRegionWnd;

class settings_model;
class cam_capture_source;
struct cam_frame;

class mouse_capture_wnd
{
public:
    mouse_capture_wnd(settings_model &settings, HWND parent);
    ~mouse_capture_wnd();

    void RegisterWIndowClass(HINSTANCE instance);
    HWND CreateRegionWindow(HINSTANCE instance, HWND parent);
    void show();

    LRESULT WINAPI WndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam);
    int DestroyShiftWindow();

    void on_mouse_move(HWND hWnd);
    void on_lbutton_up(HWND hWnd);

    settings_model &settings_;

    // used for variable
    CPoint capture_rect_drag_start_;
    CPoint capture_rect_drag_end_;
    CRect capture_rect_drag_rect_old_{0, 0, 0, 0};
    CRect capture_rect_drag_rect_{0, 0, 0, 0};

    // used for fixed
    CRect capture_rect_;

    // used to clamp
    CRect max_screen_rect_;

    // used to keep track of what we have pained so we can erase it.
    std::vector<CRect> paint_rect_;
    HBRUSH newbrush;// = (HBRUSH)CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 100));
    HBRUSH transparant_brush_{nullptr};
    HWND hwnd_{nullptr};

    std::unique_ptr<cam_capture_source> background_capture_;
    cam_frame *background_frame_{nullptr};
    std::unique_ptr<Gdiplus::Bitmap> background_bitmap_;
private:
    void on_lbutton_down(HWND hWnd);
    void on_rbutton_down(HWND hWnd);
    void on_key_down(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void on_paint(HWND hWnd, WPARAM wParam, LPARAM lParam);
};
