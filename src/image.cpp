/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2014 Artem Pavlenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#include <mapnik/image.hpp>
#include <mapnik/image_reader.hpp>
#include <mapnik/image_util.hpp>

namespace mapnik {

image::image(image_data_any && data) noexcept
    : data_(std::move(data)) {}

void image::set_data(image_data_any && data)
{
    data_ = std::move(data);
}

image image::read_from_file(std::string const& filename)
{
    std::unique_ptr<image_reader> reader(get_image_reader(filename));
    if (reader)
    {
        unsigned w = reader->width();
        unsigned h = reader->height();
        image_data_any data = reader->read(0, 0, w, h);
        return image(std::move(data));
    }
    else
    {
        return image();
    }
}

namespace detail {

struct save_to_file_visitor : mapnik::util::static_visitor<>
{
    save_to_file_visitor(std::string const& filename, std::string const& format)
        : filename_(filename),
          format_(format) {}

    void operator() (image_data_rgba8 const& data) const
    {
        save_to_file(data,filename_, format_);
    }

    template <typename T>
    void operator() (T const& data) const {}

    std::string const& filename_;
    std::string const& format_;
};

}

void image::save_to_file(std::string const& filename, std::string const& format)
{
    util::apply_visitor(detail::save_to_file_visitor(filename, format), data_);
}

}
