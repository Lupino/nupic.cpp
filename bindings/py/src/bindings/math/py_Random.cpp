/* ---------------------------------------------------------------------
 * Numenta Platform for Intelligent Computing (NuPIC)
 * Copyright (C) 2018, Numenta, Inc.  Unless you have an agreement
 * with Numenta, Inc., for a separate license for this software code, the
 * following terms and conditions apply:
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Affero Public License for more details.
 *
 * You should have received a copy of the GNU Affero Public License
 * along with this program.  If not, see http://www.gnu.org/licenses.
 *
 * http://numenta.org/licenses/
 *
 * Author: @chhenning, 2018
 * ---------------------------------------------------------------------
 */

/** @file
PyBind11 bindings for Random class
*/

// the use of 'register' keyword is removed in C++17
// Python2.7 uses 'register' in unicodeobject.h
#ifdef _WIN32
#pragma warning( disable : 5033)  // MSVC
#else
#pragma GCC diagnostic ignored "-Wregister"  // for GCC and CLang
#endif

#include <pybind11/pybind11.h>
#include <pybind11/iostream.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <nupic/utils/Random.hpp>

#include "bindings/engine/py_utils.hpp"

namespace py = pybind11;

namespace nupic_ext {

    void init_Random(py::module& m)
    {
        typedef nupic::Random Random_t;

        py::class_<Random_t> Random(m, "Random");

        Random.def(py::init<nupic::UInt64>(), py::arg("seed") = 0)
            .def("getUInt32", &Random_t::getUInt32, py::arg("max") = Random_t::MAX32)
            .def("getReal64", &Random_t::getReal64)
            .def("max", &Random_t::max)
            .def("min", &Random_t::min)
            .def_readonly_static("MAX32", &Random_t::MAX32);

        //////////////////
        // sample
        /////////////////


        Random.def("sample",
            [](Random_t& r, py::array_t<nupic::UInt32>& population, py::array_t<nupic::UInt32>& choices)
        {
            if (population.ndim() != 1 || choices.ndim() != 1)
            {
                throw std::runtime_error("Number of dimensions must be one.");
            }

            r.sample(get_it(population), population.size(), get_it(choices), choices.size());

            return choices;
        });

        //////////////////
        // shuffle
        /////////////////

        Random.def("shuffle",
            [](Random_t& r, py::array_t<nupic::UInt32>& a)
        {
            //py::scoped_ostream_redirect stream(
            //    std::cout,                               // std::ostream&
            //    py::module::import("sys").attr("stdout") // Python output
            //);

            if (a.ndim() != 1)
            {
                throw std::runtime_error("Number of dimensions must be one.");
            }

            r.shuffle(get_it(a), get_end(a));
        });

        ////////////////////

        Random.def("initializeUInt32Array", [](Random_t& self, py::array_t<nupic::UInt32>& a, nupic::UInt32 max_value)
        {
            auto array_data = get_it(a);

            for (nupic::UInt32 i = 0; i != a.size(); ++i)
                array_data[i] = self.getUInt32() % max_value;

        });


        //////////////////
        // pickle
        /////////////////
        Random.def(py::pickle(
            [](const Random_t& r)
        {
            std::stringstream ss;
            ss << r;
            return ss.str();
        },
            [](const std::string& str)
        {
            if (str.empty())
            {
                throw std::runtime_error("Empty state");
            }

            std::stringstream ss(str);
            Random_t r;
            ss >> r;

            return r;
        }
        ));

    }

} // namespace nupic_ext