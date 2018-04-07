# Copyright (C) 2018  Steven Hoving
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

project(libpsd C)

set(LIBPSD_SOURCE
    adjustment.c
    bevel_emboss.c
    bitmap.c
    blend.c
    boundary.c
    brightness_contrast.c
    channel_image.c
    channel_mixer.c
    color.c
    color_balance.c
    color_mode.c
    color_overlay.c
    curves.c
    descriptor.c
    drop_shadow.c
    effects.c
    file_header.c
    fixed.c
    gaussian_blur.c
    gradient_blend.c
    gradient_fill.c
    gradient_map.c
    gradient_overlay.c
    hue_saturation.c
    image_data.c
    image_resource.c
    inner_glow.c
    inner_shadow.c
    invert.c
    layer_mask.c
    levels.c
    outer_glow.c
    path.c
    pattern.c
    pattern_fill.c
    pattern_overlay.c
    photo_filter.c
    posterize.c
    psd.c
    psd_system.c
    psd_zip.c
    rect.c
    satin.c
    selective_color.c
    solid_color.c
    stream.c
    stroke.c
    test.c
    threshold.c
    thumbnail.c
    type_tool.c
    libpsd.h
    psd_bitmap.h
    psd_blend.h
    psd_color.h
    psd_config.h
    psd_descriptor.h
    psd_fixed.h
    psd_gradient.h
    psd_math.h
    psd_rect.h
    psd_stream.h
    psd_system.h
    psd_types.h
)

source_group(libpsd FILES
    ${LIBPSD_SOURCE}
)

add_library(libpsd STATIC
    ${LIBPSD_SOURCE}
)

target_link_libraries(libpsd
    zlibstatic
)

target_include_directories(libpsd
  PUBLIC
    .
)

install(
    DIRECTORY include
    DESTINATION include
)
