/*
 * animation/query/geometry_query.h
 *
 * Copyright 2019 Endless Mobile, Inc.
 *
 * libanimation is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * libanimation is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with eos-companion-app-service.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Definition for geometry cache. A function is kept to query the
 * geometry of the box in case it is invalidated. The idea behind
 * this class to allow the animations to have an up to date view of
 * of the relevant geometry at all times, but avoid expensive calls
 * to re-query the geometry on every frame (for instance, the true
 * geometry at any point might only be known by some javascript calling
 * code - we allow it to signal on geometry changes that the geometry
 * has been updated while the javascript code is being executed, as opposed
 * to the C++ code calling back into the javascript code to determine
 * the updated geometry on every frame).
 */

#pragma once

#include <memory>

#include <animation/geometry.h>

namespace animation
{
    template <typename T>
    class BoxQuery
    {
        public:

            typedef std::shared_ptr <BoxQuery <T>> Shared;

            virtual ~BoxQuery () {};
            virtual geometry::Box <T> const & Geometry () const = 0;
    };

    template <typename T>
    class BoxQueryStorage :
        public BoxQuery <T>
    {
        public:

            typedef std::shared_ptr <BoxQuery <T>> Shared;

            BoxQueryStorage (geometry::Box <T> const &box);
            BoxQueryStorage (geometry::Box <T> &&box);

            void Update (geometry::Box <T> const &box);
            void Update (geometry::Box <T> &&box);
            geometry::Box <T> const & Geometry () const override;

        private:

            /* Not copyable or movable */
            BoxQueryStorage (BoxQueryStorage const &query) = delete;
            BoxQueryStorage (BoxQueryStorage &&query) = delete;
            BoxQueryStorage & operator= (BoxQueryStorage const &query) = delete;
            BoxQueryStorage & operator= (BoxQueryStorage &&query) = delete;

            geometry::Box <T> box;
    };

    template <typename T>
    BoxQueryStorage <T>::BoxQueryStorage (geometry::Box <T> const &box) :
        box(box)
    {
    }

    template <typename T>
    BoxQueryStorage <T>::BoxQueryStorage (geometry::Box <T> &&box) :
        box (std::move (box))
    {
    }

    template <typename T>
    void BoxQueryStorage <T>::Update (geometry::Box <T> const &box)
    {
        this->box = box;
    }

    template <typename T>
    void BoxQueryStorage <T>::Update (geometry::Box <T> &&box)
    {
        this->box = std::move(box);
    }

    template <typename T>
    geometry::Box <T> const & BoxQueryStorage <T>::Geometry () const
    {
        return box;
    }

    template <typename T>
    typename animation::BoxQuery <T>::Shared MakeStaticBoxQuery (animation::Box <T> &&box)
    {
        return std::make_shared <animation::BoxQueryStorage <T> > (std::move (box));
    }

    template <typename T>
    typename animation::BoxQuery <T>::Shared MakeStaticBoxQuery (animation::Box <T> const &box)
    {
        return std::make_shared <animation::BoxQueryStorage <T> > (box);
    }
}
