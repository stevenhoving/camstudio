/**
 * Copyright(C) 2018  Steven Hoving
 *
 * This program is free software : you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "cam_rect.h"
#include "cam_annotarion.h"
#include "cam_virtual_screen_info.h"

#include <screen_capture_desktop_duplication/dxgi_system.h>
#include <screen_capture_desktop_duplication/dxgi_device.h>
#include <screen_capture_desktop_duplication/dxgi_output_duplication.h>

#include <windows.h>
#include <memory>
#include <vector>

namespace cam
{
    class stop_watch;
} // namespace cam

struct cam_frame
{
    BITMAPINFO *bitmap_info{nullptr};
    unsigned char *bitmap_data{nullptr};
    int width{0};
    int height{0};
    int stride{0};
};

class cam_capture_source
{
public:
    cam_capture_source() = delete;
    cam_capture_source(HWND hwnd, const cam::rect<int> &view);
    ~cam_capture_source();

    /*!
     * \param[in] src_capture_rect the rectangle of the capture source.
     * \todo the src_capture_rect does not belong here...
     */
    bool capture_frame(unsigned int timeout, const cam::rect<int> &capture_rect, cam_frame* frame);

    void enable_annotations();

    void add_annotation(std::unique_ptr<cam_iannotation> annotation);

protected:
    void _draw_annotations(const cam::rect<int> &capture_rect);
    auto _translate_from_virtual(const POINT &mouse_position) -> point<int>;

private:
	std::unique_ptr<dxgi_system> system_adapter_;
	std::unique_ptr<dxgi_device> device_;
	std::unique_ptr<dxgi_output_duplication> duplication_;
	dxgi_adapter adapter_;
	Microsoft::WRL::ComPtr<IDXGIOutput1> output_;
	dxgi_texture_staging* previouse_frame_{nullptr};


    //BITMAPINFO bitmap_info_;
    //HBITMAP bitmap_frame_;
    //HWND hwnd_;
    //HDC desktop_dc_;
    //HDC memory_dc_;
    //cam::rect<int> src_rect_;
    cam::virtual_screen_info virtual_screen_info_;
	//
    //unsigned char *bitmap_data_{nullptr};
    //cam_frame frame_;
    //cam::rect<int> captured_rect_;
	//
    //HGDIOBJ old_selected_bitmap_{nullptr};
	//
    bool enable_annotations_{false};
    std::vector<std::unique_ptr<cam_iannotation>> annotations_;
    std::unique_ptr<cam::stop_watch> stopwatch_;
	//
    //// hack...
    //std::vector<MSLLHOOKSTRUCT> mouse_events_;
};
